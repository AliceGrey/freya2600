#include "Emulator.hpp"

#include <cstdio>

struct opcode {
    uint8_t aaa : 3;
    uint8_t bbb : 3;
    uint8_t cc : 2;
};

bool Emulator::Tick()
{
    union {
        struct {
            uint8_t aaa : 3;
            uint8_t bbb : 3;
            uint8_t cc : 2;
        };
        
        uint8_t opcode;
    };
    
    opcode = NextByte();
    //fprintf(stderr, "Opcode: %02X\n", opcode);

     if (cc == 0x01) {
         const char * INSTRS[] = { 
             "ORA", "AND", "EOR", "ADC", "STA", "LDA", "CMP", "SBC",
         };

         //printf("%s \n", INSTRS[aaa]);
        
         const char * ADDR_MODE[] = {
             "Zero Page,X",
             "Zero Page",
             "#Immediate",
             "Absolute",
             "(Zero Page),Y",
             "Zero Page,X",
             "Absolute,Y",
             "Absolute,X"
         };
        
         //printf("%s\n", ADDR_MODE[bbb]);
     }
     else if (cc == 0x10) {
         const char * INSTRS[] = { 
             "ASL", "ROL", "LSR", "ROR", "STX", "LDX", "DEC", "INC",
         };

         //printf("%s \n", INSTRS[aaa]);
        
         const char * ADDR_MODE[] = {
             "#Immediate",
             "Zero Page",
             "Accumulator",
             "Absolute",
             "Zero Page,X",
            "Absolute,X",
             "",
             ""
         };
        
        // printf("%s\n", ADDR_MODE[bbb]);
     }
     else if (cc == 0x00) {
         const char * INSTRS[] = { 
             "BIT", "JMP", "JMP?", "STY", "LDY", "CPY", "CPX",
         };

         //printf("%s \n", INSTRS[aaa]);
        
         const char * ADDR_MODE[] = {
             "#Immediate",
             "Zero Page",
             "Absolute",
             "Zero Page,X",
             "Absolute,X",
             "",
             "",
             ""
         };
        
        // printf("%s\n", ADDR_MODE[bbb]);
     }


    // printf("Addressing Mode: %s\n", ADDR_MODE[bbb]);

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

    return true;
}