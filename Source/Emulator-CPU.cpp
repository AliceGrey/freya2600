#include "Emulator.hpp"

#include <cstdio>

void Emulator::TickCPU()
{
    if (WSYNC) {
        return;
    }
    
    union OPCODE_t {
        struct {
            uint8_t group : 2;
            uint8_t mode : 3;
            uint8_t inst : 3;
        };

        struct {
            uint8_t : 5;
            uint8_t test : 1;
            uint8_t flag : 2;
        };
        
        uint8_t _raw;
    };

    OPCODE_t opcode = {._raw = NextByte() };

    #define SET_NZ(VALUE) \
        N = ((VALUE) & 0x80); \
        Z = ((VALUE) == 0);

    byte data;
    word address;

    bool found = true;

    // printf("PC=%04X OP=%02X inst=%d mode=%d group=%d\n", PC - 1, opcode._raw, opcode.inst, opcode.mode, opcode.group);

    switch (opcode._raw) {
    // BRK:
    case 0x00:
        I = 1;
        PushWord(PC + 2);
        PushByte(SR);
        PC = ReadWord(0x1FFE);
        break;

    // JSR
    case 0x20:
        PushWord(PC + 2);
        PC = NextWord();
        break;

    // RTI (Return From Interrupt)
    case 0x40:
        SR = PopByte();
        PC = PopWord();
        I = 0;
        break;

    // RTS (Return From Subroutme)
    case 0x60:
        PC = PopWord();
        break;

    // PHP (Push Processor Status On Stack)
    case 0x08:
        PushByte(SR);
        break;

    // PLP (Pull Processor Status From Stack)
    case 0x28:
        SR = PopByte();
        break;

    // PHA (Push Accumulator On Stack)
    case 0x48:
        PushByte(A);
        break;

    // PLA (Pull Accumulator From Stack)
    case 0x68:
        A = PopByte();
        break;

    // DEY (Decrement Index Y by One)
    case 0x88:
        Y -= 1;
        SET_NZ(Y);
        break;

    // TAY (Transfer Accumulator To Index Y)
    case 0xA8:
        Y = A;
        SET_NZ(Y);
        break;

    // INY (Increment Index Y by One)
    case 0xC8:
        Y += 1;
        SET_NZ(Y);
        break;

    // INX (Increment Index X by One)
    case 0xE8:
        X += 1;
        SET_NZ(X);
        break;

    // CLC (Clear Carry Flag)
    case 0x18:
        C = 0;
        break;

    // SEC (Set Carry Flag)
    case 0x38:
        C = 1;
        break;

    // CLI (Clear Interrupt Disable)
    case 0x58:
        I = 0;
        break;

    // SEI (Set Interrupt Disable)
    case 0x78:
        I = 1;
        break;

    // TYA (Transfer Index Y To Accumulator)
    case 0x98:
        A = Y;
        SET_NZ(A);
        break;

    // CLV (Clear Overflow Flag)
    case 0xB8:
        V = 0;
        break;

    // CLD (Clear Decimal Mode)
    case 0xD8:
        D = 0;
        break;

    // SED (Set Decimal Mode)
    case 0xF8:
        D = 1;
        break;

    // TXA (Transfer Index X To Accumulator)
    case 0x8A:
        A = X;
        SET_NZ(A);
        break;

    // TXS (Transfer Index X To Stack Pointer)
    case 0x9A:
        SP = X;
        break;

    // TAX (Transfer Accumulator To Index X)
    case 0xAA:
        X = A;
        SET_NZ(X);
        break;

    // TSX (Transfer Stack Pointer To Index X)
    case 0xBA:
        X = SP;
        SET_NZ(X);
        break;

    // DEX (Decrement Index Register X By One)
    case 0xCA:
        X -= 1;
        SET_NZ(X);
        break;

    // NOP (No Operation)
    case 0xEA: // Get in the game
        break;

    default:
        found = false;
        break;
    }

    if (!found) {
        // Branch

        if (opcode.mode == 0b100 && opcode.group == 0b00) {

            int8_t offset = (int8_t)NextByte();

            switch (opcode.flag) {
            // N
            case 0b00:
                if (N == opcode.test) {
                    PC += offset;
                }
                break;
            // V
            case 0b01:
                if (V == opcode.test) {
                    PC += offset;
                }
                break;
            // C
            case 0b10:
                if (C == opcode.test) {
                    PC += offset;
                }
                break;
            // Z
            case 0b11:
                if (Z == opcode.test) {
                    PC += offset;
                }
                break;
            }
        }
        else if (opcode.group == 0b01) {

            switch (opcode.mode) {
            // (Zero Page,X)
            case 0b000:
                data = ReadByte(ReadWord(0x0000 + NextByte() + X));
                break;

            // Zero Page
            case 0b001:
                data = ReadByte(0x0000 + NextByte());
                break;

            // #Immediate
            case 0b010:
                data = NextByte();
                break;

            // Absolute
            case 0b011:
                data = ReadByte(NextWord());
                break;

            // (Zero Page),Y
            case 0b100:
                data = ReadByte(ReadWord(NextByte()) + Y);
                break;

            // Zero Page,X
            case 0b101:
                data = ReadByte(0x0000 + NextByte() + X);
                break;

            // Absolute,Y
            case 0b110:
                data = ReadByte(NextWord() + Y);
                break;

            // Absolute,X
            case 0b111:
                data = ReadByte(NextWord() + X);
                break;
            }

            switch (opcode.inst) {
            // ORA
            case 0b000:
                A |= data;
                SET_NZ(A);
                break;

            // AND
            case 0b001:
                A &= data;
                SET_NZ(A);
                break;

            // EOR
            case 0b010:
                A ^= data;
                SET_NZ(A);
                break;

            // ADC (Add Memory to Accumulator with Carry)
            case 0b011:
                {
                    word result = A + data + C;
                    C = (result & 0xFF00);
                    A = (result & 0xFF);
                    V = ((A ^ data) & (A ^ result) & 0x80); // wtf // We overflowing boiiiiii // #help
                    SET_NZ(A);
                }
                break;

            // STA
            case 0b100:
                A = data;
                SET_NZ(A);
                break;

            // LDA
            case 0b101:
                A = data;
                SET_NZ(A);
                break;

            // CMP
            case 0b110:
                {
                    word result = A - data;
                    C = !(result & 0xFF00);
                    SET_NZ(result & 0xFF00);
                }
                break;

            // SBC:
            case 0b111:
                {
                    word result = A - data - ~C;
                    V = ((A ^ data) & (A ^ result) & 0x80); // wtf
                    C = !(result & 0xFF00);
                    A = (result & 0xFF);
                    SET_NZ(A);
                }
                break;
            }

        }
        else if (opcode.group == 0b10) {

            bool isA = (opcode.mode == 0b010);
            bool isX = (opcode.inst == 0b100 || opcode.inst == 0b101);

            switch (opcode.mode) {
            // #Immediate
            case 0b000:
                data = NextByte();
                break;

            // Zero Page
            case 0b001:
                address = 0x0000 + NextByte();
                data = ReadByte(address);
                break;

            // Accumulator
            case 0b010:
                break;

            // Absolute
            case 0b011:
                address = NextWord();
                data = ReadByte(address);
                break;

            // Zero Page,X/Y
            case 0b101:
                address = 0x0000 + NextByte() + (isX ? Y : X);
                data = ReadByte(address);
                break;

            // Absolute,X/Y
            case 0b111:
                address = NextWord() + (isX ? Y : X);
                data = ReadByte(address);
                break;
            }

            switch (opcode.inst) {
            // ASL
            case 0b000:
                if (isA) {
                    C = (A & 0x80);
                    A <<= 1;
                    SET_NZ(A);
                }
                else {
                    C = (data & 0x80);
                    data <<= 1;
                    SET_NZ(data);
                    WriteByte(address, data);
                }
                break;

            // ROL
            case 0b001:
                if (isA) {
                    uint16_t result = (A << 1) | C;
                    C = (result & 0x0100);
                    A = result;
                    SET_NZ(A);
                }
                else {
                    uint16_t result = (data << 1) | C;
                    C = (result & 0x0100);
                    data = result;
                    SET_NZ(data);
                    WriteByte(address, data);
                }
                break;

            // LSR
            case 0b010:
                if (isA) {
                    C = (A & 0x01);
                    A >>= 1;
                    SET_NZ(A);
                }
                else {
                    C = (data & 0x01);
                    data >>= 1;
                    SET_NZ(data);
                    WriteByte(address, data);
                }
                break;

            // ROR
            case 0b011:
                if (isA) {
                    uint16_t result = ((A | (C << 7)) >> 1);
                    C = (A & 0x01);
                    A = result;
                    SET_NZ(A);
                }
                else {
                    uint16_t result = ((data | (C << 7)) >> 1);
                    C = (data & 0x01);
                    data = result;
                    SET_NZ(data);
                }
                break;

            // STX
            case 0b100:
                WriteByte(address, X);
                break;

            // LDX
            case 0b101:
                X = data;
                SET_NZ(X);
                break;

            // DEC
            case 0b110:
                {
                    uint16_t result = data - 1;
                    SET_NZ(X);
                    WriteByte(address, data);
                }
                break;

            // INC
            case 0b111:
                {
                    uint16_t result = data + 1;
                    SET_NZ(X);
                    WriteByte(address, data);
                }
                break;
            }
        }
        else if (opcode.group == 0b00) {

            switch (opcode.mode) {
            // #Immediate
            case 0b000:
                data = NextByte();
                break;

            // Zero Page
            case 0b001:
                address = 0x0000 + NextByte();
                data = ReadByte(address);
                break;

            // Absolute
            case 0b011:
                address = NextWord();
                data = ReadByte(address);
                break;

            // Zero Page,X
            case 0b101:
                address = 0x0000 + NextByte() + X;
                data = ReadByte(address);
                break;

            // Absolute,X
            case 0b111:
                address = NextWord() + X;
                data = ReadByte(address);
                break;
            }

            switch (opcode.inst) {
            // BIT
            case 0b001:
                data &= A;
                V = (data & 0x40);
                SET_NZ(data);
                break;

            // JMP (Indirect)
            case 0b010:
                PC = ReadWord(address);
                break;

            // JMP
            case 0b011:
                PC = address;
                break;

            // STY
            case 0b100:
                WriteByte(address, Y);
                break;

            // LDY
            case 0b101:
                Y = data;
                break;

            // CPY
            case 0b110:
                {
                    word result = Y - data;
                    C = !(result & 0xFF00);
                    SET_NZ(result & 0xFF);
                }
                break;

            // CPX
            case 0b111:
                {
                    word result = X - data;
                    C = !(result & 0xFF00);
                    SET_NZ(result & 0xFF);
                }
                break;
            }
        }
    }

    // poof
}