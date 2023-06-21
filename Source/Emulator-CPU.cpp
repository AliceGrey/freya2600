#include "Emulator.hpp"

void Emulator::Tick()
{
    uint8_t opcode = ReadByte(PC);
    ++PC;
    

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
        //BPL
        //BRK
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
        //DEY (Decrement Index Y by One)
        case 0x88:
            Y = Y - 1;
            Z = (Y == 0);
            N = (Y & 0x10);
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
            PC = ReadWord(PC);
            break;
        
        // JMP (Indirect)
        case 0x6C:
            PC = ReadWord(ReadWord(PC));
            break;

        // JSR
        case 0x20:
            PushWord(PC + 2);
            PC = ReadWord(PC);
            break;
            
        //LDA
        //LDX
        //LDY
        //LSR
        //NOP
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
        //TAX
        //TAY
        //TSX
        //TXA
        //TXS
        //TYA
//
    }
}