#include "Emulator.hpp"

#include <cstdio>

void Emulator::Tick()
{
    uint8_t opcode = NextByte();

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
        //AND
        //ASL
        //BCC
        //BCS
        //BEQ
        //BIT
        //BMI
        //BNE
        //BPL (Branch on Result Plus)
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
            printf("BRK %04X\n", PC);
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
            V = 0;
            break;
        //CMP (Compare Memory with Accumulator)
        //CPX
        //CPY
        //DEC
        //DEX (Decrement Index X by One)
        case 0xCA:
            X = X - 1;
            Z = (X == 0);
            N = (X & 0x10);
            break;
        //DEY (Decrement Index Y by One)
        case 0x88:
            Y = Y - 1;
            Z = (Y == 0);
            N = (Y & 0x10);
            break;
        //EOR
        //INC
        //INX (Increment Index X by One)
        case 0xE8:
            X = X + 1;
            Z = (X == 0);
            N = (X & 0x10);
            break;
        //INY (Increment Index Y by One)
        case 0xC8:
            Y = Y + 1;
            Z = (Y == 0);
            N = (Y & 0x10);
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
            N = (A & 0x10);
            break;
            
        // Absolute
        case 0xAD:
            A = ReadByte(NextWord());
            Z = (A == 0);
            N = (A & 0x10);
            break;
            
        // Absolute,X-Indexed
        case 0xBD:
            A = ReadByte(NextWord() + X);
            Z = (A == 0);
            N = (A & 0x10);
            break;
            
        // Absolute,Y-Indexed
        case 0xB9:
            A = ReadByte(NextWord() + Y);
            Z = (A == 0);
            N = (A & 0x10);
            break;
        
        // Zero Page
        case 0xA5:
            A = ReadByte(0x0000 + NextByte());
            Z = (A == 0);
            N = (A & 0x10);
            break;
            
        /// LDX (Load Index Register X From Memory)
        
        // Immediate
        case 0xA2:
            X = NextByte();
            PC += 1;
            Z = (X == 0);
            N = (X & 0x10);
            break;
            
        // Absolute
        case 0xAE:
            X = ReadByte(NextWord());
            PC += 2;
            Z = (X == 0);
            N = (X & 0x10);
            break;
        
        // Absolute,Y-Indexed
        case 0xBE:
            break;

        //LDY (Load Index Register Y From Memory)(Immediate)
        case 0xA0:
            Y = NextByte();
            PC += 1;
            Z = (Y == 0);
            N = (Y & 0x10);
            break;
        //LSR (Logical Shift Right)(Accumulator)
        case 0x4A:
            C = (A & 0x01);
            A >>= 1;
            N = 0;
            Z = (A == 0);
            break;

        //NOP
        case 0x04:
        case 0x44:
        case 0x64:
            PC += 1;
            break;
        //ORA
        //PHA
        //PHP
        //PLA
        //PLP
        //ROL
        //ROR
        //RTI
        //RTS
        //SBC
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
        //STX
        //STY
        //TAX (Transfer Accumulator To Index X)
        // Only two things are certain in life....
        case 0xAA:
            X = A;
            N = (X & 0x10);
            Z = (X == 0);
            break;
        //TAY (Transfer Accumulator To Index Y)
        case 0xA8:
            Y = A;
            N = (Y & 0x10);
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
        break;
    }
}