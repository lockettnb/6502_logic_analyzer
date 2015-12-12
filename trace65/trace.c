/*
*   2015/11/10 created
*
*
* Copyright © 2015 by John Lockett
* 
* All rights reserved. All parts of this publication may be reproduced,
* distributed, or transmitted in any form or by any means, including
* photocopying, recording, or other electronic or mechanical methods
* No permission required ..... john.lockett@bellaliant.net*
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define BUFFER_SIZE 256
#define TRACE_SIZE 10002

#define TRUE 1
#define FALSE 0
#define FAIL 1
#define SUCCESS 0

#define DEBUG 0
#define debug_print(...)  do { if (DEBUG) fprintf(stderr, ##__VA_ARGS__); } while (0)


// #define debug_print(fmt, ...) \
        do { if (DEBUG) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
                                __LINE__, __func__, __VA_ARGS__); } while (0)

struct trace_node {
    uint16_t address;
    uint8_t  data;
    uint8_t  rw;
    uint8_t  sync;
    uint8_t  irq;
};

 
// Global Variables
char *program_name;
struct trace_node tbuf[TRACE_SIZE];


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int xdis(int node)
{
 uint8_t ops[6];
 int step;
 int i;

// some genius supplied the sync single just to help us know when
// the 6502 is fetching an instruction
if(tbuf[node].sync == 1) {
    // instruction ... disassemble required
    for(i=0; i<=5; i++) 
        ops[i] = tbuf[node+i].data;
    step = disassemble(tbuf[node].address, ops); 
} else {
    if(tbuf[node].rw) {
        //read memory
        printf("%04x+ %02x \n", tbuf[node].address, tbuf[node].data); 
    } else {
        // write to memory
        printf("%04x- %02x \n", tbuf[node].address, tbuf[node].data); 
    }
    step=1;
}
    return step;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
main (int argc, char *argv[])
{
FILE *fp;
char buf[BUFFER_SIZE];
uint8_t tracebuf[TRACE_SIZE];
int i;
int node = 0; 
char *token;
// int address, data, rw, sync, irq;
const char s[2] = " ";
   
    if((fp=fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "%s: error opening file <%s>\n", program_name, argv[1]);
        exit(FAIL);
    }

    // suck up the full file into a buffer
    while (fgets(buf,BUFFER_SIZE, fp)!=NULL) {
        buf[strcspn(buf, "\n")] = 0;
        debug_print("<%s>\n",buf);

        tbuf[node].address = (int) strtol(strtok(buf,s), NULL, 16);
        tbuf[node].data = (int) strtol(strtok(NULL,s), NULL, 16);
        tbuf[node].rw = (int) strtol(strtok(NULL, s), NULL, 16);
        tbuf[node].sync = (int) strtol(strtok(NULL, s), NULL, 16);
        tbuf[node++].irq = (int) strtol(strtok(NULL, s), NULL, 16);

//       printf("address %i %04x \n", address, address);
//       printf("data %i %04x \n", data, data);
//       printf("rw %i %04x \n", rw, rw);
//       printf("sync %i %04x \n", sync, sync);
    }
        fclose(fp);
        debug_print("Load complete with %i nodes\n",node);

    // run thru the buffer printing the assembler
    for(i=0; i<=node-1; i++){
        i = i + (xdis(i)-1);
    }

} //main

