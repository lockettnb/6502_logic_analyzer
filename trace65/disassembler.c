/*
*   6502 dissassembler
*
*
*  2014/11/26 created
*  2015/11/12 adapted disassembler for trace65
*
*
* Copyright © 2015 by John Lockett
* 
* All rights reserved. All parts of this publication may be reproduced,
* distributed, or transmitted in any form or by any means, including
* photocopying, recording, or other electronic or mechanical methods
* No permission required ..... john.lockett@bellaliant.net
*
*
*/

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <regex.h>
#include "e6502.h"

char *opcodes[] =
     {
        "BRK",               // 0x00
        "ORA (n,X)",         // 0x01
        "db n",              // 0x02
        "db n",              // 0x03
        "db n",              // 0x04
        "ORA n",             // 0x05
        "ASL n",             // 0x06
        "db n",              // 0x07
        "PHP",               // 0x08
        "ORA #n",            // 0x09
        "ASL A",             // 0x0A
        "db n",              // 0x0B
        "db n",              // 0x0C
        "ORA nn",            // 0x0D
        "ASL nn",            // 0x0E
        "db n",              // 0x0F

        "BPL n",            // 0x10
        "ORA (n),Y",         // 0x11
        "db n",              // 0x12
        "db n",              // 0x13
        "db n",              // 0x14
        "ORA n,X",           // 0x15
        "ASL n,X",           // 0x16
        "db n",              // 0x17
        "CLC",               // 0x18
        "ORA nn,Y",          // 0x19
        "db n",              // 0x1A
        "db n",              // 0x1B
        "db n",              // 0x1C
        "ORA nn,X",          // 0x1D
        "ASL nn,X",          // 0x1E
        "db n",              // 0x1F

        "JSR nn",            // 0x20
        "AND (n,X)",         // 0x21
        "db n",              // 0x22
        "db n",              // 0x23
        "BIT n",             // 0x24
        "AND n",             // 0x25
        "ROL n",             // 0x26
        "db n",              // 0x27
        "PLP",               // 0x28
        "AND #n",            // 0x29
        "ROL A",             // 0x2A
        "db n",              // 0x2B
        "BIT nn",            // 0x2C
        "AND nn",            // 0x2D
        "ROL nn",            // 0x2E
        "db n",              // 0x2F

        "BMI n",            // 0x30
        "AND (n),Y",         // 0x31
        "db n",              // 0x32
        "db n",              // 0x33
        "db n",              // 0x34
        "AND n,X",           // 0x35
        "ROL n,X",           // 0x36
        "db n",              // 0x37
        "SEC",               // 0x38
        "AND nn,Y",          // 0x39
        "db n",              // 0x3A
        "db n",              // 0x3B
        "db n",              // 0x3C
        "ORA nn,X",          // 0x3D
        "ASL nn,X",          // 0x3E
        "db n",              // 0x3F

        "RTI",               // 0x40
        "EOR (n,X)",         // 0x41
        "db n",              // 0x42
        "db n",              // 0x43
        "db n",              // 0x44
        "EOR n",             // 0x45
        "LSR n",             // 0x46
        "db n",              // 0x47
        "PHA",               // 0x48
        "EOR #n",            // 0x49
        "LSR A",             // 0x4A
        "db n",              // 0x4B
        "JMP nn",            // 0x4C
        "EOR nn",            // 0x4D
        "LSR nn",            // 0x4E
        "db n",              // 0x4F

        "BVC n",             // 0x50
        "EOR (n),Y",          // 0x51
        "db n",               // 0x52
        "db n",               // 0x53
        "db n",               // 0x54
        "EOR n,X",            // 0x55
        "LSR n,X",            // 0x56
        "db n",               // 0x57
        "CLI",                // 0x58
        "EOR nn,Y",           // 0x59
        "db n",               // 0x5A
        "db n",               // 0x5B
        "db n",               // 0x5C
        "EOR nn,X",           // 0x5D
        "LSR nn,X",           // 0x5E
        "db n",               // 0x5F

        "RTS",                // 0x60
        "ADC (n,X)",          // 0x61
        "db n",               // 0x62
        "db n",               // 0x63
        "db n",               // 0x64
        "ADC n",              // 0x65
        "ROR n",              // 0x66
        "db n",               // 0x67
        "PLA",                // 0x68
        "ADC #n",             // 0x69
        "ROR A",              // 0x6A
        "db n",               // 0x6B
        "JMP (nn)",           // 0x6C
        "ADC nn",             // 0x6D
        "ROR nn",             // 0x6E
        "db n",               // 0x6F

        "BVS n",              // 0x70
        "ADC (n),Y",          // 0x71
        "db n",               // 0x72
        "db n",               // 0x73
        "db n",               // 0x74
        "ADC n,X",            // 0x75
        "ROR n,X",            // 0x76
        "db n",               // 0x77
        "SEI",                // 0x78
        "ADC nn,Y",           // 0x79
        "db n",               // 0x7A
        "db n",               // 0x7B
        "db n",               // 0x7C
        "ADC nn,X",           // 0x7D
        "ROR nn,X",           // 0x7E
        "db n",               // 0x7F

        "db n",               // 0x80
        "STA (n,X)",          // 0x81
        "db n",               // 0x82
        "db n",               // 0x83
        "STY n",              // 0x84
        "STA n",              // 0x85
        "STX n",              // 0x86
        "db n",               // 0x87
        "DEY",                // 0x88
        "db n",               // 0x89
        "TXA A",              // 0x8A
        "db n",               // 0x8B
        "STY nn",             // 0x8C
        "STA nn",             // 0x8D
        "STX nn",             // 0x8E
        "db n",               // 0x8F

        "BCC n",              // 0x90
        "STA (n),Y",          // 0x91
        "db n",               // 0x92
        "db n",               // 0x93
        "STY n.X",            // 0x94
        "STA n,X",            // 0x95
        "STX n,Y",            // 0x96
        "db n",               // 0x97
        "TYA",                // 0x98
        "STA nn,Y",           // 0x99
        "TXS",                // 0x9A
        "db n",               // 0x9B
        "db n",               // 0x9C
        "STA nn,X",           // 0x9D
        "db n",               // 0x9E
        "db n",               // 0x9F

        "LDY #n",             // 0xA0
        "LDA (n,X)",          // 0xA1
        "LDX #n",             // 0xA2
        "db n",               // 0xA3
        "LDY n",              // 0xA4
        "LDA n",              // 0xA5
        "LDX n",              // 0xA6
        "db n",               // 0xA7
        "TAY",                // 0xA8
        "LDA #n",             // 0xA9
        "TAX",                // 0xAA
        "db n",               // 0xAB
        "LDY nn",             // 0xAC
        "LDA nn",             // 0xAD
        "LDX nn",             // 0xAE
        "db n",               // 0xAF

        "BCS n",              // 0xB0
        "LDA (n),Y",          // 0xB1
        "db n",               // 0xB2
        "db n",               // 0xB3
        "LDY n",              // 0xB4
        "LDA n,X",            // 0xB5
        "LDX n,Y",            // 0xB6
        "db n",               // 0xB7
        "CLV",                // 0xB8
        "LDA nn,Y",           // 0xB9
        "TSX",                // 0xBA
        "db n",               // 0xBB
        "LDY nn,X",           // 0xBC
        "LDA nn,X",           // 0xBD
        "LDX nn,Y",           // 0xBE
        "db n",               // 0xBF

        "CPY #n",             // 0xC0
        "CMP (n,X)",          // 0xC1
        "db n",               // 0xC2
        "db n",               // 0xC3
        "CPY n",              // 0xC4
        "CMP n",              // 0xC5
        "DEC n",              // 0xC6
        "db n",               // 0xC7
        "INY",                // 0xC8
        "CMP #n",             // 0xC9
        "DEX",                // 0xCA
        "db n",               // 0xCB
        "CPY nn",             // 0xCC
        "CMP nn",             // 0xCD
        "DEC nn",             // 0xCE
        "db n",               // 0xCF

        "BNE n",              // 0xD0
        "CMP (n),Y",          // 0xD1
        "db n",               // 0xD2
        "db n",               // 0xD3
        "db n",               // 0xD4
        "CMP n,X",            // 0xD5
        "DEC n,X",            // 0xD6
        "db n",               // 0xD7
        "CLD",                // 0xD8
        "CMP nn,Y",           // 0xD9
        "db n",               // 0xDA
        "db n",               // 0xDB
        "db n",               // 0xDC
        "CMP nn,X",           // 0xDD
        "DEC nn,X",           // 0xDE
        "db n",               // 0xDF

        "CPX #n",             // 0xE0
        "SBC (n,X)",          // 0xE1
        "db n",               // 0xE2
        "db n",               // 0xE3
        "CPX n",              // 0xE4
        "SBC n",              // 0xE5
        "INC n",              // 0xE6
        "db n",               // 0xE7
        "INX",                // 0xE8
        "SBC #n",             // 0xE9
        "NOP",                // 0xEA
        "db n",               // 0xEB
        "CPX nn",             // 0xEC
        "SBC nn",             // 0xED
        "INC nn",             // 0xEE
        "db n",               // 0xEF

        "BEQ n",              // 0xF0
        "SBC (n),Y",          // 0xF1
        "db n",               // 0xF2
        "db n",               // 0xF3
        "db n",               // 0xF4
        "SBC n,X",            // 0xF5
        "INC n,X",            // 0xF6
        "db n",               // 0xF7
        "SED",                // 0xF8
        "SBC nn,Y",           // 0xF9
        "db n",               // 0xFA
        "db n",               // 0xFB
        "db n",               // 0xFC
        "SBC nn,X",           // 0xFD
        "INC nn,X",           // 0xFE
        "db n"                // 0xFF
     };

/***********************************************************************/
// substitute the address for the "n" value in the opcode text
void substr_address (char *operation_text, char *final_text, char *address_text, int start, int size) {
int i;
int j=0;


    for(i=0; i<start; i++) {
        final_text[i]=operation_text[i];
    }

    j = 0;
    while (address_text[j]) {
          final_text[i++] = address_text[j++];
    }

    j=start+size;
    while (operation_text[j]) {
          final_text[i++] = operation_text[j++];
    }
    final_text[i]=NULL_CHAR;
}

/***********************************************************************/

int disassemble(uint16_t addr, uint8_t opcode[])
{
//     unsigned char *opcode = &memory[addr];
    int  op_size=1;
    char op_text[16];               // text for the opertion code including any address modes
    char op_text2[16];              // temporary place to build up the final op_text
    char address_text[16];
    int op_index;

    regex_t *dbreg, *addreg;    // compiled regular expressions
    int rc;                         // return code
    char r_data_byte[]= "db n";     // regex for data byte code
    char r_address_byte[]= "n+";    // regex for one byte of and address
    regmatch_t matches[MAXMATCH];
    int address_start;
    int address_byte_size;

    strcpy(op_text, opcodes[(int) opcode[0]]);
 
    // special handeling for JSR operation
    //  -the third cycle of the JSR instruction is actually discarded data
    //  -the sixth cycle actually fetches the high byte of the address
    if(opcode[0] == 0x20) {
        opcode[2]=opcode[5];
    }

    // make room for the regular expressions and zero them
    dbreg = (regex_t *) malloc(sizeof(regex_t));
    addreg = (regex_t *) malloc(sizeof(regex_t));
    memset(dbreg, 0, sizeof(regex_t));        // not sure why we do this
    memset(addreg, 0, sizeof(regex_t));        // not sure why we do this

    // compile regex to match an data byte
    rc=regcomp(dbreg, r_data_byte, REG_EXTENDED);
    if (rc !=0) {
        fputs("Error: problem compiling first regex", stderr);
        exit(1);
    }
    // compile second regex to match an adress byte(s)
    rc=regcomp(addreg, r_address_byte, REG_EXTENDED);
    if (rc !=0) {
        fputs("Error: problem compiling second regex", stderr);
        exit(1);
    }

    rc = regexec(dbreg, op_text, MAXMATCH, matches, 0);
    if(rc == 0) {             // match found, this is a byte of data not opcode
        sprintf(op_text, ".db $%02x", opcode[0]);
        op_size = 1;
    } else { 
        rc = regexec(addreg, op_text, MAXMATCH, matches, 0);
        if(rc == 0) {         // this is an operation code with address value
            address_start = matches[0].rm_so;
            address_byte_size = matches[0].rm_eo - address_start;
            if(address_byte_size == 1) {
                sprintf(address_text, "$%02x", opcode[1]);
                op_size=2;
            } 
            if(address_byte_size == 2) {
                sprintf(address_text, "$%02x%02x", opcode[2], opcode[1]);
                op_size=3;
            } 
        substr_address(op_text, op_text2, address_text, address_start, address_byte_size);
        strcpy(op_text, op_text2);
        }
   }   // else (not data byte)

    regfree(dbreg); free(dbreg);
    regfree(addreg); free(addreg);

    switch (op_size) {
        case 1: 
            printf("%04x  %02x        %s\n", addr, opcode[0], op_text); break;
        case 2: 
            printf("%04x  %02x %02x     %s\n", addr, opcode[0], opcode[1],op_text); 
            break;
        case 3: 
            printf("%04x  %02x %02x %02x  %s\n", addr, opcode[0], opcode[1], opcode[2],op_text); 
            break;
    }
    return(op_size);
}







