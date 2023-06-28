#include "Emulator.hpp"

#include <cstdio>

#include <fmt/format.h>

void Emulator::TickCPU()
{
    if (WSYNC) {
        CPUCycleCount += 1;
        return;
    }

    LastInstructionAddress = PC;
    printRegisters();

    OperationCode opcode = { ._raw = NextByte() };

    #define SET_NZ(VALUE) \
        N = (((VALUE) & 0x80) > 0); \
        Z = ((VALUE) == 0);

    byte data;
    word address;
    word result; // The 16-bit result of 8-bit math, used to look for overflows
    byte small_result; // The 8-bit result of 8-bit math

    bool found = true;

    //printf("PC=%04X OP=%02X inst=%d mode=%d group=%d\n", PC - 1, opcode._raw, opcode.inst, opcode.mode, opcode.group);

    switch (opcode._raw) {
    // BRK (Break Command, Software Interrupt)
    case 0x00:
        I = 1;
        PushWord(PC + 2);
        PushByte(SR);
        PC = ReadWord(0xFFFE);
        CPUCycleCount += 1;
        break;

    // JSR (Jump to Subroutine)
    case 0x20:
        PushWord(PC + 1);
        PC = NextWord();
        CPUCycleCount += 1;
        break;

    // RTI (Return from Interrupt)
    case 0x40:
        SR = PopByte();
        PC = PopWord();
        I = 0;
        CPUCycleCount += 2;
        break;

    // RTS (Return from Subroutine)
    case 0x60:
        PC = PopWord() + 1;
        CPUCycleCount += 3;
        break;

    // PHP (Push Processor Status on Stack)
    case 0x08:
        PushByte(SR);
        CPUCycleCount += 1;
        break;

    // PLP (Pull Processor Status from Stack)
    case 0x28:
        SR = PopByte();
        CPUCycleCount += 2;
        break;

    // PHA (Push Accumulator on Stack)
    case 0x48:
        PushByte(A);
        CPUCycleCount += 1;
        break;

    // PLA (Pull Accumulator from Stack)
    case 0x68:
        A = PopByte();
        CPUCycleCount += 2;
        break;

    // DEY (Decrement Y by One)
    case 0x88:
        Y -= 1;
        SET_NZ(Y);
        CPUCycleCount += 1;
        break;

    // TAY (Transfer Accumulator to Y)
    case 0xA8:
        Y = A;
        SET_NZ(Y);
        CPUCycleCount += 1;
        break;

    // INY (Increment Y by One)
    case 0xC8:
        Y += 1;
        SET_NZ(Y);
        CPUCycleCount += 1;
        break;

    // INX (Increment X by One)
    case 0xE8:
        X += 1;
        SET_NZ(X);
        CPUCycleCount += 1;
        break;

    // CLC (Clear Carry Flag)
    case 0x18:
        C = 0;
        CPUCycleCount += 1;
        break;

    // SEC (Set Carry Flag)
    case 0x38:
        C = 1;
        CPUCycleCount += 1;
        break;

    // CLI (Clear Disable Interrupt Flag)
    case 0x58:
        I = 0;
        CPUCycleCount += 1;
        break;

    // SEI (Set Disable Interrupt Flag)
    case 0x78:
        I = 1;
        CPUCycleCount += 1;
        break;

    // TYA (Transfer Y To Accumulator)
    case 0x98:
        A = Y;
        SET_NZ(A);
        CPUCycleCount += 1;
        break;

    // CLV (Clear Overflow Flag)
    case 0xB8:
        V = 0;
        CPUCycleCount += 1;
        break;

    // CLD (Clear Decimal Mode Flag)
    case 0xD8:
        D = 0;
        CPUCycleCount += 1;
        break;

    // SED (Set Decimal Mode Flag)
    case 0xF8:
        D = 1;
        CPUCycleCount += 1;
        break;

    // TXA (Transfer X to Accumulator)
    case 0x8A:
        A = X;
        SET_NZ(A);
        CPUCycleCount += 1;
        break;

    // TXS (Transfer X to Stack Pointer)
    case 0x9A:
        SP = X;
        CPUCycleCount += 1;
        break;

    // TAX (Transfer Accumulator to X)
    case 0xAA:
        X = A;
        SET_NZ(X);
        CPUCycleCount += 1;
        break;

    // TSX (Transfer Stack Pointer to X)
    case 0xBA:
        X = SP;
        SET_NZ(X);
        CPUCycleCount += 1;
        break;

    // DEX (Decrement Register X by One)
    case 0xCA:
        X -= 1;
        SET_NZ(X);
        CPUCycleCount += 1;
        break;

    // NOP (No Operation)
    case 0xEA: // Get in the game
        CPUCycleCount += 1;
        break;

    default:
        found = false;
        break;
    }

    if (!found) {
        // Branch Instructions
        if (opcode.mode == 0b100 && opcode.group == 0b00) {

            // $10: BPL (Branch on Result Plus)
            // $30: BMI (Branch on Result Minus)
            // $50: BVC (Branch on Overflow Clear)
            // $70: BVS (Branch on Overflow Set)
            // $90: BCC (Branch on Carry Clear)
            // $B0: BCS (Branch on Carry Set)
            // $D0: BNE (Branch on Result Not Zero)
            // $F0: BEQ (Branch on Result Zero)

            byte check[] = { N, V, C, Z };

            int8_t offset = (int8_t)NextByte();

            if (check[opcode.flag] == opcode.test) {
                PC += offset;
                CPUCycleCount += 1;
            }
        }
        else if (opcode.group == 0b01) {

            switch (opcode.mode) {
            // (Zero Page,X)
            case 0b000:
                address = ReadWord(0x0000 + (byte)(NextByte() + X));
                CPUCycleCount += 1;
                break;

            // Zero Page
            case 0b001:
                address = 0x0000 + NextByte();
                break;

            // #Immediate
            case 0b010:
                address = PC;
                PC += 1;
                break;

            // Absolute
            case 0b011:
                address = NextWord();
                break;

            // (Zero Page),Y
            case 0b100:
                address = ReadWord(NextByte()) + Y;
                break;

            // Zero Page,X
            case 0b101:
                address = 0x0000 + (byte)(NextByte() + X);
                CPUCycleCount += 1;
                break;

            // Absolute,Y
            case 0b110:
                address = NextWord() + Y;
                break;

            // Absolute,X
            case 0b111:
                address = NextWord() + X;
                break;
            }

            switch (opcode.inst) {
            // ORA (Bitwise OR Memory with Accumulator)
            // $09: #Immediate
            // $0D: Absolute
            // $1D: Absolute,X
            // $19: Absolute,Y
            // $05: Zero Page
            // $15: Zero Page,X
            // $01: (Zero Page,X)
            // $11: (Zero Page),Y
            case 0b000:
                A |= ReadByte(address);
                SET_NZ(A);
                break;

            // AND (Bitwise AND Memory with Accumulator)
            // $29: #Immediate
            // $2D: Absolute
            // $3D: Absolute,X
            // $39: Absolute,Y
            // $25: Zero Page
            // $35: Zero Page,X
            // $21: (Zero Page,X)
            // $31: (Zero Page),Y
            case 0b001:
                A &= ReadByte(address);
                SET_NZ(A);
                break;

            // EOR (Bitwise XOR Memory with Accumulator)
            // $49: #Immediate
            // $4D: Absolute
            // $5D: Absolute,X
            // $59: Absolute,Y
            // $45: Zero Page
            // $55: Zero Page,X
            // $41: (Zero Page,X)
            // $51: (Zero Page),Y
            case 0b010:
                A ^= ReadByte(address);
                SET_NZ(A);
                break;

            // ADC (Add Memory to Accumulator with Carry)
            // $69: #Immediate
            // $6D: Absolute
            // $7D: Absolute,X
            // $79: Absolute,Y
            // $65: Zero Page
            // $75: Zero Page,X
            // $61: (Zero Page,X)
            // $71: (Zero Page),Y
            case 0b011:
                data = ReadByte(address);
                result = A + data + C;
                C = (result & 0xFF00);
                A = (result & 0xFF);
                V = ((A ^ data) & (A ^ result) & 0x80); // wtf // We overflowing boiiiiii // #help
                SET_NZ(A);
                break;

            // STA (Store Accumulator into Memory)
            // $8D: Absolute
            // $9D: Absolute,X
            // $99: Absolute,Y
            // $85: Zero Page
            // $95: Zero Page,X
            // $81: (Zero Page,X)
            // $91: (Zero Page),Y
            case 0b100:
                WriteByte(address, A);
                break;

            // LDA (Load Accumulator from Memory)
            // $A9: #Immediate
            // $AD: Absolute
            // $BD: Absolute,X
            // $B9: Absolute,Y
            // $A5: Zero Page
            // $B5: Zero Page,X
            // $A1: (Zero Page,X)
            // $B1: (Zero Page),Y
            case 0b101:
                A = ReadByte(address);
                SET_NZ(A);
                break;

            // CMP (Compare Memory with Accumulator)
            // $C9: #Immediate
            // $CD: Absolute
            // $DD: Absolute,X
            // $D9: Absolute,Y
            // $C5: Zero Page
            // $D5: Zero Page,X
            // $C1: (Zero Page,X)
            // $D1: (Zero Page),Y
            case 0b110:
                result = A - ReadByte(address);
                C = !(result & 0xFF00);
                SET_NZ(result & 0xFF00);
                break;

            // SBC (Subtract Memory from Accumulator with Borrow)
            /*
            This instruction subtracts the value of memory and borrow from the value of the accumulator, using two's complement arithmetic, and stores the result in the accumulator. 
            Borrow is defined as the carry flag complemented; therefore, a resultant carry flag indicates that a borrow has not occurred.
            This instruction affects the accumulator. The carry flag is set if the result is greater than or equal to 0. The carry flag is reset when the result is less than 0, indicating a borrow. 
            The over­flow flag is set when the result exceeds +127 or -127, otherwise it is reset. The negative flag is set if the result in the accumulator has bit 7 on, otherwise it is reset. 
            The Z flag is set if the result in the accumulator is 0, otherwise it is reset.
            Note on the MOS 6502:
            In decimal mode, the N, V and Z flags are not consistent with the decimal result.*/
            // $E9: #Immediate
            // $ED: Absolute
            // $FD: Absolute,X
            // $F9: Absolute,Y
            // $E5: Zero Page
            // $F5: Zero Page,X
            // $E1: (Zero Page,X)
            // $F1: (Zero Page),Y
            //TODO: Decimal Mode
            case 0b111:
                data = ReadByte(address);
                result = A - data - (C ? 0 : 1);
                V = ((A ^ data) & (A ^ result) & 0x80); // wtf
                C = !(result & 0xFF00);
                A = (result & 0xFF);
                SET_NZ(A);
                break;
            }
        }
        else if (opcode.group == 0b10) {

            bool isA = (opcode.mode == 0b010);
            bool isX = (opcode.inst == 0b100 || opcode.inst == 0b101);

            switch (opcode.mode) {
            // #Immediate
            case 0b000:
                address = PC;
                PC += 1;
                break;

            // Zero Page
            case 0b001:
                address = 0x0000 + NextByte();
                break;

            // Accumulator
            case 0b010:
                break;

            // Absolute
            case 0b011:
                address = NextWord();
                break;

            // Zero Page,X/Y
            case 0b101:
                address = 0x0000 + (byte)(NextByte() + (isX ? Y : X));
                break;

            // Absolute,X/Y
            case 0b111:
                address = NextWord() + (isX ? Y : X);
                break;
            }

            switch (opcode.inst) {
            // ASL (Arithmetic Shift Left)
            // $0A: Accumulator
            // $0E: Absolute
            // $1E: Absolute,X
            // $06: Zero Page
            // $16: Zero Page,X
            case 0b000:
                if (isA) {
                    C = (A & 0x80);
                    A <<= 1;
                    SET_NZ(A);
                }
                else {
                    data = ReadByte(address);
                    C = (data & 0x80);
                    data <<= 1;
                    SET_NZ(data);
                    WriteByte(address, data);
                }
                break;

            // ROL (Rotate Left)
            // $2A: Accumulator
            // $2E: Absolute
            // $3E: Absolute,X
            // $26: Zero Page
            // $36: Zero Page,X
            case 0b001:
                if (isA) {
                    result = (A << 1) | C;
                    C = (result & 0x0100);
                    A = result;
                    SET_NZ(A);
                }
                else {
                    data = ReadByte(address);
                    result = (data << 1) | C;
                    C = (result & 0x0100);
                    data = result;
                    SET_NZ(data);
                    WriteByte(address, data);
                }
                break;

            // LSR (Logical Shift Right)
            // $4A: Accumulator
            // $4E: Absolute
            // $5E: Absolute,X
            // $46: Zero Page
            // $56: Zero Page,X
            case 0b010:
                if (isA) {
                    C = (A & 0x01);
                    A >>= 1;
                    SET_NZ(A);
                }
                else {
                    data = ReadByte(address);
                    C = (data & 0x01);
                    data >>= 1;
                    SET_NZ(data);
                    WriteByte(address, data);
                }
                break;

            // ROR (Rotate Right)
            // $6A: Accumulator
            // $6E: Absolute
            // $7E: Absolute,X
            // $66: Zero Page
            // $76: Zero Page,X
            case 0b011:
                if (isA) {
                    result = ((A | (C << 7)) >> 1);
                    C = (A & 0x01);
                    A = result;
                    SET_NZ(A);
                }
                else {
                    data = ReadByte(address);
                    result = ((data | (C << 7)) >> 1);
                    C = (data & 0x01);
                    data = result;
                    SET_NZ(data);
                }
                break;

            // STX (Store Index Register X in Memory)
            // $8E: Absolute
            // $86: Zero Page
            // $96: Zero Page,Y
            case 0b100:
                WriteByte(address, X);
                break;

            // LDX (Load Index Register X from Memory)
            // $A2: #Immediate
            // $AE: Absolute
            // $BE: Absolute,Y
            // $A6: Zero Page
            // $B6: Zero Page,Y
            case 0b101:
                X = ReadByte(address);
                SET_NZ(X);
                break;

            // DEC (Decrement Memory by One)
            // $CE: Absolute
            // $DE: Absolute,X
            // $C6: Zero Page
            // $D6: Zero Page,X
            case 0b110:
                data = ReadByte(address) - 1;
                SET_NZ(data);
                WriteByte(address, data);
                break;

            // INC (Increment Memory by One)
            // $EE: Absolute
            // $FE: Absolute,X
            // $E6: Zero Page
            // $F6: Zero Page,X
            case 0b111:
                data = ReadByte(address) + 1;
                SET_NZ(data);
                WriteByte(address, data);
                break;
            }
        }
        else if (opcode.group == 0b00) {

            switch (opcode.mode) {
            // #Immediate
            case 0b000:
                address = PC;
                PC += 1;
                break;

            // Zero Page
            case 0b001:
                address = 0x0000 + NextByte();
                break;

            // Absolute
            case 0b011:
                address = NextWord();
                break;

            // Zero Page,X
            case 0b101:
                address = 0x0000 + (byte)(NextByte() + X);
                break;

            // Absolute,X
            case 0b111:
                address = NextWord() + X;
                break;
            }

            switch (opcode.inst) {
            // BIT (Test Bits in Memory with Accumulator)
            // $2C: Absolute
            // $24: Zero Page
            case 0b001: 
                data = ReadByte(address) & A;
                V = (data & 0x40);
                SET_NZ(data);
                break;

            // $4C: JMP (Jump to Address)
            case 0b010:
                PC = address;
                break;

            // $6C: JMP (Jump to Address Indirect)
            case 0b011:
                PC = ReadWord(address);
                break;

            // STY (Store Index Register Y into Memory)
            // $8C: Absolute
            // $84: Zero Page
            // $94: Zero Page,X
            case 0b100:
                WriteByte(address, Y);
                break;

            // LDY (Load Index Register Y from Memory)
            // $A0: #Immediate
            // $AC: Absolute
            // $BC: Absolute,X
            // $A4: Zero Page
            // $B4: Zero Page,X
            case 0b101:
                Y = ReadByte(address);
                SET_NZ(Y);
                break;

            // CPY (Compare Index Register Y to Memory)
            // $C0: #Immediate
            // $CC: Absolute
            // $C4: Zero Page
            case 0b110:
                result = Y - ReadByte(address);
                C = !(result & 0xFF00);
                SET_NZ(result & 0xFF);
                break;

            // CPX (Compare Index Register X to Memory)
            // $E0: #Immediate
            // $EC: Absolute
            // $E4: Zero Page
            case 0b111:
                result = X - ReadByte(address);
                C = !(result & 0xFF00);
                SET_NZ(result & 0xFF);
                break;
            }
        }
    }

    // poof
    //printf("SP=%02X A=%02X X=%02X Y=%02X\n",SP, A, X, Y);
    //printf("C=%02X Z=%02X I=%02X D=%02X V=%02X N=%02X\n", C, Z, I, D, V, N);
    //printRAMGrid(RAM);
}

struct Instruction
{
    unsigned bytes;
    const char * format;
};

static const Instruction INSTRUCTIONS[] = {
    { 1, "brk" },
    { 2, "ora ({0:02x},x)" },
    { 1, "e$02" },
    { 1, "e$03" },
    { 1, "e$04" },
    { 2, "ora  {0:02x}" },
    { 2, "asl  {0:02x}" },
    { 1, "e$07" },
    { 1, "php" },
    { 2, "ora #{0:02x}" },
    { 1, "asl a" },
    { 1, "e$0b" },
    { 1, "e$0c" },
    { 1, "ora" },
    { 2, "asl  {0:02x}" },
    { 1, "e$0f" },
    { 2, "bpl  {2:04x}" },
    { 2, "ora ({0:02x}),y" },
    { 1, "e$12" },
    { 1, "e$13" },
    { 1, "e$14" },
    { 2, "ora  {0:02x},x" },
    { 2, "asl  {0:02x},x" },
    { 1, "e$17" },
    { 1, "clc" },
    { 3, "ora  {1:02x}{0:02x},y" },
    { 1, "e$1a" },
    { 1, "e$1b" },
    { 1, "e$1c" },
    { 3, "ora  {1:02x}{0:02x},x" },
    { 3, "asl  {1:02x}{0:02x},x" },
    { 1, "e$1f" },
    { 3, "jsr  {1:02x}{0:02x}" },
    { 2, "and ({0:02x},x)" },
    { 1, "e$22" },
    { 1, "e$23" },
    { 2, "bit  {0:02x}" },
    { 2, "and  {0:02x}" },
    { 2, "rol  {0:02x}" },
    { 1, "e$27" },
    { 1, "plp" },
    { 2, "and #{0:02x}" },
    { 1, "rol a" },
    { 1, "e$2b" },
    { 3, "bit  {1:02x}{0:02x}" },
    { 3, "and  {1:02x}{0:02x}" },
    { 3, "rol  {1:02x}{0:02x}" },
    { 1, "e$2f" },
    { 2, "bmi  {2:04x}" },
    { 2, "and ({0:02x}),y" },
    { 1, "e$32" },
    { 1, "e$33" },
    { 1, "e$34" },
    { 2, "and  {0:02x},x" },
    { 2, "rol  {0:02x},x" },
    { 1, "e$37" },
    { 1, "sec" },
    { 2, "and  {0:02x},y" },
    { 1, "e$3a" },
    { 1, "e$3b" },
    { 1, "e$3c" },
    { 3, "and  {1:02x}{0:02x},x" },
    { 3, "rol  {1:02x}{0:02x},x" },
    { 1, "e$3f" },
    { 1, "rti" },
    { 2, "eor ({0:02x},x)" },
    { 1, "e$42" },
    { 1, "e$43" },
    { 1, "e$44" },
    { 2, "eor  {0:02x}" },
    { 2, "lsr  {0:02x}" },
    { 1, "e$47" },
    { 1, "pha" },
    { 2, "eor #{0:02x}" },
    { 1, "lsr a" },
    { 1, "e$4b" },
    { 3, "jmp  {1:02x}{0:02x}" },
    { 3, "eor  {1:02x}{0:02x}" },
    { 3, "lsr  {1:02x}{0:02x}" },
    { 1, "e$4f" },
    { 2, "bvc  {2:04x}" },
    { 2, "eor ({0:02x}),y" },
    { 1, "e$52" },
    { 1, "e$53" },
    { 1, "e$54" },
    { 2, "eor  {0:02x},x" },
    { 2, "lsr  {0:02x},x" },
    { 1, "e$57" },
    { 1, "cli" },
    { 3, "eor  {1:02x}{0:02x},y" },
    { 1, "e$5a" },
    { 1, "e$5b" },
    { 1, "e$5c" },
    { 3, "eor  {1:02x}{0:02x},x" },
    { 3, "lsr  {1:02x}{0:02x},x" },
    { 1, "e$5f" },
    { 1, "rts" },
    { 2, "adc ({0:02x},x)" },
    { 1, "e$62" },
    { 1, "e$63" },
    { 1, "e$64" },
    { 2, "adc  {0:02x}" },
    { 2, "ror  {0:02x}" },
    { 1, "e$67" },
    { 1, "pla" },
    { 2, "adc #{0:02x}" },
    { 1, "ror a" },
    { 1, "e$6b" },
    { 3, "jmp ({1:02x}{0:02x})" },
    { 3, "adc  {1:02x}{0:02x}" },
    { 3, "ror  {1:02x}{0:02x}" },
    { 1, "e$6f" },
    { 2, "bvs  {2:04x}" },
    { 2, "adc ({0:02x}),y" },
    { 1, "e$72" },
    { 1, "e$73" },
    { 1, "e$74" },
    { 2, "adc  {0:02x},x" },
    { 2, "ror  {0:02x},x" },
    { 1, "e$77" },
    { 1, "sei" },
    { 3, "adc  {1:02x}{0:02x},y" },
    { 1, "e$7a" },
    { 1, "e$7b" },
    { 1, "e$7c" },
    { 3, "adc  {1:02x}{0:02x},x" },
    { 3, "ror  {1:02x}{0:02x},x" },
    { 1, "e$7f" },
    { 1, "e$80" },
    { 2, "sta ({0:02x},x)" },
    { 1, "e$82" },
    { 1, "e$83" },
    { 2, "sty  {0:02x}" },
    { 2, "sta  {0:02x}" },
    { 2, "stx  {0:02x}" },
    { 1, "e$87" },
    { 1, "dey" },
    { 1, "e$89" },
    { 1, "txa" },
    { 1, "e$8b" },
    { 3, "sty  {1:02x}{0:02x}" },
    { 3, "sta  {1:02x}{0:02x}" },
    { 3, "stx  {1:02x}{0:02x}" },
    { 1, "e$8f" },
    { 2, "bcc  {2:04x}" },
    { 2, "sta ({0:02x}),y" },
    { 1, "e$92" },
    { 1, "e$93" },
    { 2, "sty  {0:02x},x" },
    { 2, "sta  {0:02x},x" },
    { 2, "stx  {0:02x},y" },
    { 1, "e$97" },
    { 1, "tax" },
    { 3, "sta  {1:02x}{0:02x},y" },
    { 1, "txs" },
    { 1, "e$9b" },
    { 1, "e$9c" },
    { 3, "sta  {1:02x}{0:02x},x" },
    { 1, "e$9e" },
    { 1, "e$9f" },
    { 2, "ldy #{0:02x}" },
    { 2, "lda ({0:02x},x)" },
    { 2, "ldx #{0:02x}" },
    { 1, "e$a3" },
    { 2, "ldy  {0:02x}" },
    { 2, "lda  {0:02x}" },
    { 2, "ldx  {0:02x}" },
    { 1, "e$a7" },
    { 1, "tay" },
    { 2, "lda #{0:02x}" },
    { 1, "tax" },
    { 1, "e$ab" },
    { 3, "ldy  {1:02x}{0:02x}" },
    { 3, "lda  {1:02x}{0:02x}" },
    { 3, "ldx  {1:02x}{0:02x}" },
    { 1, "e$af" },
    { 2, "bcs  {2:04x}" },
    { 2, "lda ({0:02x}),y" },
    { 1, "e$b2" },
    { 1, "e$b3" },
    { 2, "ldy  {0:02x},x" },
    { 2, "lda  {0:02x},x" },
    { 2, "ldx  {0:02x},y" },
    { 1, "e$b7" },
    { 1, "clv" },
    { 3, "lda  {1:02x}{0:02x},y" },
    { 1, "tsx" },
    { 1, "e$bb" },
    { 3, "ldy  {1:02x}{0:02x}" },
    { 3, "lda  {1:02x}{0:02x},x" },
    { 3, "ldx  {1:02x}{0:02x},y" },
    { 1, "e$bf" },
    { 2, "cpy #{0:02x}" },
    { 2, "cmp ({0:02x},x)" },
    { 1, "e$c2" },
    { 1, "e$c3" },
    { 2, "cpy  {0:02x}" },
    { 2, "cmp  {0:02x}" },
    { 2, "dec  {0:02x}" },
    { 1, "e$c7" },
    { 1, "iny" },
    { 2, "cmp #{0:02x}" },
    { 1, "dex" },
    { 1, "e$cb" },
    { 3, "cpy  {1:02x}{0:02x}" },
    { 3, "cmp  {1:02x}{0:02x}" },
    { 3, "dec  {1:02x}{0:02x}" },
    { 1, "e$cf" },
    { 2, "bne  {2:04x}" },
    { 2, "cmp ({0:02x}),y" },
    { 1, "e$d2" },
    { 1, "e$d3" },
    { 1, "e$d4" },
    { 2, "cmp  {0:02x},x" },
    { 2, "dec  {0:02x},x" },
    { 1, "e$d7" },
    { 1, "cld" },
    { 3, "cmp  {1:02x}{0:02x},y" },
    { 1, "e$da" },
    { 1, "e$db" },
    { 1, "e$dc" },
    { 3, "cmp  {1:02x}{0:02x},x" },
    { 3, "dec  {1:02x}{0:02x},x" },
    { 1, "e$df" },
    { 2, "cpx #{0:02x}" },
    { 2, "sbc ({0:02x},x)" },
    { 1, "e$2" },
    { 1, "e$3" },
    { 2, "cpx  {0:02x}" },
    { 2, "sbc  {0:02x}" },
    { 2, "inc  {0:02x}" },
    { 1, "e$e7" },
    { 1, "inx" },
    { 2, "sbc #{0:02x}" },
    { 1, "nop" },
    { 1, "e$b" },
    { 3, "cpx  {1:02x}{0:02x}" },
    { 3, "sbc  {1:02x}{0:02x}" },
    { 3, "inc  {1:02x}{0:02x}" },
    { 1, "e$ef" },
    { 2, "beq  {2:04x}" },
    { 2, "sbc ({0:02x}),y" },
    { 1, "e$f2" },
    { 1, "e$f3" },
    { 1, "e$f4" },
    { 2, "sbc  {0:02x},x" },
    { 2, "inc  {0:02x},x" },
    { 1, "e$f7" },
    { 1, "sed" },
    { 3, "sbc {2:02x}{1:02x},y" },
    { 1, "e$fa" },
    { 1, "e$fb" },
    { 1, "e$fc" },
    { 3, "sbc {2:02x}{1:02x},x" },
    { 3, "inc {2:02x}{1:02x},x" },
    { 1, "e$ff" },
};

const char * Emulator::Disassemble(word address)
{   
    // TODO: Remove duplicate reads
    byte d0 = ReadByte(address + 0, false);
    byte d1 = ReadByte(address + 1, false);
    byte d2 = ReadByte(address + 2, false);

    const auto& inst = INSTRUCTIONS[d0];

    static char disassembly[1024];

    memset(disassembly, 0, sizeof(disassembly));
    fmt::format_to_n(disassembly, sizeof(disassembly), fmt::runtime(inst.format), d1, d2, address + (int8_t)d1 + inst.bytes);

    return disassembly;
}

void Emulator::printRegisters()
{	
    // TODO: Remove duplicate reads
    byte d0 = ReadByte(LastInstructionAddress + 0, false);
    byte d1 = ReadByte(LastInstructionAddress + 1, false);
    byte d2 = ReadByte(LastInstructionAddress + 2, false);

    const auto& inst = INSTRUCTIONS[d0];

    char disassembly[1024];

    memset(disassembly, 0, sizeof(disassembly));
    fmt::format_to_n(disassembly, sizeof(disassembly), fmt::runtime(inst.format), d1, d2, LastInstructionAddress + (int8_t)d1 + inst.bytes);

/*
				fprintf(zlog, "\n(%3d %3d %3d) (%3d %3d) (%3d %3d %3d %3d %3d) ",
					 (int)frame, line, cycle, line-42, cycle*3-68,
					 (P0_Pos-68+5)%160, (P1_Pos-68+5)%160, (M0_Pos-68+4)%160,
					 (M1_Pos-68+4)%160, (BL_Pos-68+4)%160);
*/
    int P0_Position = 0;
    int P1_Position = 0;
    int M0_Position = 0;
    int M1_Position = 0;
    int BL_Position = 0;
    int TIACollide = 0;
    byte B = 1;

	int sline = 0;
	int cyc = CPUCycleCount;
	int clk = CPUCycleCount*3-68;
	if (cyc == 76)
	{
		cyc -= 76;
		clk -= 228;
		sline++;
	}

	fprintf(tLog,
        "\n(%5d %4d %3d %3d)  (%3d %3d %3d %3d %3d)  <%4x>  ",
		(int)FrameCount,
        MemoryLine + 1,
        (int)(CPUCycleCount - LastWSYNC),
        (int)TIACycleCount,
		(P0_Position - 68 + 5) % 160,
        (P1_Position - 68 + 5) % 160,
        (M0_Position - 68 + 4) % 160,
		(M1_Position - 68 + 4) % 160,
        (BL_Position - 68 + 4) % 160,
        TIACollide
    );

	fprintf(tLog, "%c%c%c%c%c%c%c ",
        N ? 'N' : 'n',
	    V ? 'V' : 'v',
	    B ? 'B' : 'b',
	    D ? 'D' : 'd',
	    I ? 'I' : 'i',
	    Z ? 'Z' : 'z',
	    C ? 'C' : 'c'
    );

    fprintf(tLog, "%02x %02x %02x %02x  ", A, X, Y, SP);
    fprintf(tLog, "%04x: ", LastInstructionAddress);
    //fprintf(tLog, "%02x       ", opcode);

    if (inst.bytes == 1) {
        fprintf(tLog, "%02x       ", d0);
    }
    else if (inst.bytes == 2) {
        fprintf(tLog, "%02x %02x    ", d0, d1);
    }
    else if (inst.bytes == 3) {
        fprintf(tLog, "%02x %02x %02x ", d0, d1, d2);
    }

    fprintf(tLog, "%s", Disassemble(LastInstructionAddress));
    fflush(tLog);
}