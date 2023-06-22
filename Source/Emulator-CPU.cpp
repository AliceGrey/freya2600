#include "Emulator.hpp"

#include <cstdio>

bool Emulator::Tick()
{
    union OPCODE_t {
        struct {
            uint8_t inst : 3;
            uint8_t mode : 3;
            uint8_t group : 2;
        };

        struct {
            uint8_t flag : 2;
            uint8_t test : 1;
            uint8_t : 5;
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
        // TODO:
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
        // TODO:
        break;

    // PLA (Pull Accumulator From Stack)
    case 0x68:
        // TODO:
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
        // TODO:
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
        // TODO:
        break;

    // TXS (Transfer Index X To Stack Pointer)
    case 0x9A:
        // TODO:
        break;

    // TAX (Transfer Accumulator To Index X)
    case 0xAA:
        X = A;
        SET_NZ(X);
        break;

    // TSX (Transfer Stack Pointer To Index X)
    case 0xBA:
        // TODO:
        break;

    // DEX (Decrement Index Register X By One)
    case 0xCA:
        // TODO:
        break;

    // NOP (No Operation)
    case 0xEA:
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
        else if (opcode.group == 0x01) {

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

            // ADC
            case 0b011:
                // TODO:
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
                    byte value = ReadByte(NextByte());
                    word result = A - data - ~C;
                    V = ((A ^ value) & (A ^ result) & 0x80); // wtf
                    C = !(result & 0xFF00);
                    A = (result & 0xFF);
                    SET_NZ(A);
                }
                break;
            }

        }
        else if (opcode.group == 0x10) {

            bool isA = (opcode.mode == 0b010);
            bool isX = (opcode.inst == 0b1000 || opcode.inst == 0b101);

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
        else if (opcode.group == 0x00) {

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
                // TODO:
                break;

            // CPX
            case 0b111:
                // TODO:
                break;
            }
        }
    }

    /*

    switch (opcode) {
        // ADC
        // Add Memory to Accumulator with Carry
        // A + M + C -> A, C
        // N	Z	C	I	D	V
        // +	+	+	-	-	+
        // addressing	assembler	 opc	bytes	cycles
        // immediate	ADC #oper	 69     2	    2  
        // zeropage	ADC oper	     65	    2	    3  
        // zeropage,X	ADC oper,X	 75	    2	    4  
        // absolute	ADC oper	     6D	    3	    4  
        // absolute,X	ADC oper,X	 7D	    3	    4* 
        // absolute,Y	ADC oper,Y	 79	    3	    4* 
        // (indirect,X)	ADC (oper,X) 61	    2	    6  
        // (indirect),Y	ADC (oper),Y 71	    2	    5* 

        //Add with carry x-indexed, indirect - 2 bytes - 6 cycles
        case 0x61:
        //Add with carry zeropage - 2 bytes - 3 cycles
        case 0x65:
        //Add with carry y-indexed, indirect - 2 bytes - 5 cycles
        case 0x71:
        //Add with carry zeropage - x-indexed - 2 bytes - 4 cycles
        case 0x75:
        //Add with carry immediate - 2 bytes - 2 cycles
        case 0x69:
        case 0x79:
        case 0x6D:
        case 0x7D:
        //AND ("AND" Memory with Accumulator)
        //Immediate
        case 0x29:
            A = A & NextByte();
            Z = (A == 0);
            N = (A & 0x80);
            break;
        //ASL
        //BCC (Branch on Carry Clear)
        //Relative
        case 0x90:
        {
                int8_t offset = (int8_t)NextByte();
                if (C == 0){
                    PC += offset;
                }
            }
            break;
        //BCS
        //BEQ (Branch on Result Zero)
        // Relative
        case 0xF0:
            {
                int8_t offset = (int8_t)NextByte();
                if (Z == 1){
                    PC += offset;
                }
            }
            break;
        //BIT (Test Bits in Memory with Accumulator)
        // Zero Page
        case 0x24:
            {
                byte value = ReadByte(NextByte());
                N = (value & 0x80);
                V = (value & 0x40);
                A = A & value;
                Z = (A == 0);
            }
            break;

        //BMI(Branch on Result Minus)
        // Relative
        case 0x30:
            {
                int8_t offset = (int8_t)NextByte();
                if (N == 1){
                    PC += offset;
                }
            }
            break;
        //BNE (Branch on Result Not Zero)
        // Relative
        case 0xD0:
            {
                int8_t offset = (int8_t)NextByte();
                if (Z == 0){
                    PC += offset;
                }
            }
            break;
        //BPL (Branch on Result Plus)
        // Relative
        case 0x10:
            {
                int8_t offset = (int8_t)NextByte();
                if (N == 0) {
                    PC += offset;
                }
            }    
            break;

        //BRK (Break)(Implied)
        case 0x00:
            I = 1;
            PushWord(PC + 2);
            PushByte(SR);
            PC = ReadWord(0x1FFE);
            //printf("BRK %04X\n", PC);
            break;
        //BVC
        //BVS
        //CLC (Clear Carry Flag)
        case 0x18:
            C = 0;
            break;
        //CLD (Clear Decimal Mode)
        case 0xD8:
            D = 0;
            break;
        //CLI (Clear Interrupt Disable Bit)
        case 0x58:
            I = 0;
            break;
        //CLV (Clear Overflow Flag)
        case 0xB8:
            V = 0;
            break;
        //CMP (Compare Memory with Accumulator)
        // Immediate
        case 0xC9:
            {
                word result = A - NextByte();
                C = !(result & 0xFF00);
                Z = ((result & 0xFF) == 0);
                N = ((result & 0xFF) & 0x80);
            }
            break;
        //CPX
        //CPY
        //DEC
        //DEX (Decrement Index X by One)
        case 0xCA:
            X = X - 1;
            Z = (X == 0);
            N = (X & 0x80);
            break;
        //DEY (Decrement Index Y by One)
        case 0x88:
            Y = Y - 1;
            Z = (Y == 0);
            N = (Y & 0x80);
            break;
        //EOR ("Exclusive OR" Memory with Accumulator)
        //Immediate
        case 0x49:
            A = A ^ NextByte();
            Z = (A == 0);
            N = (A & 0x80);
            break;
        
        // Zero Page
        case 0x45:
            A = A ^ ReadByte(0x0000 + NextByte());
            Z = (A == 0);
            N = (A & 0x80);
            break;

        //INC (Increment Memory by One)
        // Zero Page
        case 0xE6:
            {
                uint16_t address = ReadByte(PC);
                uint8_t result = ReadByte(address) + 1;
                WriteByte(address, result);
                PC += 1;
                Z = (result == 0);
                N = (result & 0x80);
            }
            break;
        //INX (Increment Index X by One)
        case 0xE8:
            X = X + 1;
            Z = (X == 0);
            N = (X & 0x80);
            break;
        //INY (Increment Index Y by One)
        case 0xC8:
            Y = Y + 1;
            Z = (Y == 0);
            N = (Y & 0x80);
            break;
        //JMP (Absolute)
        case 0x4C:
            PC = NextWord();
            break;
        
        // JMP (Indirect)
        case 0x6C:
            PC = ReadWord(NextWord());
            break;

        // JSR
        case 0x20:
            PushWord(PC + 2);
            PC = NextWord();
            break;
            
        /// LDA (Load Accumulator with Memory)
        
        // Immediate
        case 0xA9:
            A = NextByte();
            Z = (A == 0);
            N = (A & 0x80);
            break;
            
        // Absolute
        case 0xAD:
            A = ReadByte(NextWord());
            Z = (A == 0);
            N = (A & 0x80);
            break;
            
        // Absolute,X-Indexed
        case 0xBD:
            A = ReadByte(NextWord() + X);
            Z = (A == 0);
            N = (A & 0x80);
            break;
            
        // Absolute,Y-Indexed
        case 0xB9:
            A = ReadByte(NextWord() + Y);
            Z = (A == 0);
            N = (A & 0x80);
            break;
        
        // Zero Page
        case 0xA5:
            A = ReadByte(0x0000 + NextByte());
            Z = (A == 0);
            N = (A & 0x80);
            break;
        
        // Zero Page,X-Indexed
        case 0xB5:
            A = ReadByte(0x0000 + NextByte() + X);
            Z = (A == 0);
            N = (A & 0x80);
            break;
        
        // Zero Page Indirect Y-Indexed
        case 0xB1:
            A = ReadByte(ReadWord(NextByte()) + Y);
            Z = (A == 0);
            N = (A & 0x80);
            break;

        /// LDX (Load Index Register X From Memory)
        //Zero Page
        case 0xA6:
            X = ReadByte(NextByte());
            Z = (X == 0);
            N = (X & 0x80);
            break;
            
        // Immediate
        case 0xA2:
            X = NextByte();
            PC += 1;
            Z = (X == 0);
            N = (X & 0x80);
            break;
            
        // Absolute
        case 0xAE:
            X = ReadByte(NextWord());
            PC += 2;
            Z = (X == 0);
            N = (X & 0x80);
            break;
        
        // Absolute,Y-Indexed
        case 0xBE:
            break;

        //LDY (Load Index Register Y From Memory)(Immediate)
        case 0xA0:
            Y = NextByte();
            PC += 1;
            Z = (Y == 0);
            N = (Y & 0x80);
            break;
        //LSR (Logical Shift Right)(Accumulator)
        case 0x4A:
            C = (A & 0x01);
            A >>= 1;
            N = 0;
            Z = (A == 0);
            break;

        //NOP
        // Zero Page
        case 0x04:
        case 0x44:
        case 0x64:
            PC += 1;
            break;
        // Implied (EA Does Nothing LMAO) 
        case 0xEA:
            break;
        
        //ORA ("OR" Memory with Accumulator)
        // Immediate
        case 0x09:
            A = A | NextByte();
            Z = (A == 0);
            N = (A & 0x80);
            break;
            
        //PHA
        //PHP (Push Processor Status On Stack)
        // Implied
        case 0x08:
            PushByte(SR);
            break;
            
        //PLA
        // case 0x68:
        //     SR = PopByte() & 0b;
        //PLP
        //ROL
        //ROR
        //RTI
        //RTS (Return From Subroutine)
        //Implied
        case 0x60:
            PC = PopWord();
            break;

        //SBC (Subtract Memory from Accumulator with Borrow)
        // Zero Page
        case 0xE5:
            {
                // look, don't ask
                byte value = ReadByte(NextByte());
                word result = A - value - ~C;
                V = ((A ^ value) & (A ^ result) & 0x80); // wtf
                C = !(result & 0xFF00);
                A = (result & 0xFF);
                Z = (A == 0);
                N = (A & 0x80);
            }
            break;
        //SEC (Set Carry Flag)
        case 0x38:
            C = 1;
            break;
        //SED (Set Decimal Flag)
        case 0xF8:
            D = 1;
            break;
        //SEI (Set Interrupt Disable Status)
        case 0x78:
            I = 1;
            break;
        //STA
        // Zero Page
        case 0x85:
            WriteByte(NextByte(),A);
            break;
        //STX
        //STY
        //TAX (Transfer Accumulator To Index X)
        // Only two things are certain in life....
        case 0xAA:
            X = A;
            N = (X & 0x80);
            Z = (X == 0);
            break;
        //TAY (Transfer Accumulator To Index Y)
        case 0xA8:
            Y = A;
            N = (Y & 0x80);
            Z = (Y == 0);
            break;
        //TSX (Transfer Index X to Stack Pointer)
        case 0x9A:
            SP = X;
            break;
        //TXA
        //TXS
        //TYA
    default:
        fprintf(stderr, "Unrecognized opcode: %02X\n", opcode);
        return false;
    }

    */

    return true;
}