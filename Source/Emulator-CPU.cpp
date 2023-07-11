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

    static unsigned index = 0;
    // if (index > 894) {
        // printRegisters();
    // }
    ++index;

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
                data = ReadByte(address);
                N = ((data & 0x80) > 0);
                V = ((data & 0x40) > 0);
                data &= A;
                Z = (data == 0);
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