/*
*  2015/05/08 created
*  2015/05/14 read working on 502 CPU board!!!
*  2015/11/03 adapted from ICE project for 6502 Logic Analyser
*
*
* Copyright © 2015 by John Lockett
* 
* All rights reserved. All parts of this publication may be reproduced,
* distributed, or transmitted in any form or by any means, including
* photocopying, recording, or other electronic or mechanical methods
* For permission requests, do not write to the publisher
*/

#include <stdio.h>
#include <string.h>
#include "Arduino.h"
#include "HardwareSerial.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define debug(m) Serial.println(m) 

#define TRUE 1
#define FALSE 0
#define MAXLINE 32 
#define TESTPIN1 3
#define TESTPIN2 4

// trigger is setup as a global .... lazy programmer
uint32_t trigger = 0xfffc;

// 32 bit buffer to hold input samples
//
// each 32 bit in the bits array contains address, data and control 
//      15..0   16-bit address
//      16..23  8-bit data
//      24      1-bit read/write
//      25      1-bit sync
//      26      1-bit irq
# define BUFFERSIZE 10000
uint32_t bits[BUFFERSIZE];


// *************************************************
void xattention(char *arg[])
{
// all message use println therefore send msg<cr><lf>
// the unix end of the line must remove the control chars
    Serial.println("OK");
}

// *************************************************
void xdisplay(char *arg[])
{
int i;
int start = 0;
int end = 0;
char msg[MAXLINE];

uint32_t data;
uint32_t address;
uint32_t rw, sync, irq;

    if (arg[0])  start = strtol(arg[0], NULL, 10);
    if (arg[1])  end = strtol(arg[1], NULL, 10);

//     sprintf(msg, "display at %i to %i", start,end);
//     Serial.println(msg);

    for (i=start; i<=start+end; i++){
        address = bits[i] & 0xFFFF;
        data = (bits[i] & 0x00FF0000) >> 16;
        rw =   (bits[i] & 0x01000000) >> 24;
        sync = (bits[i] & 0x02000000) >> 25;
        irq =  (bits[i] & 0x04000000) >> 26;
        sprintf(msg, "%04lx  %02lx %02lx %02lx %02lx", address, data,rw,sync,irq);
        Serial.println(msg);
    }
//     Serial.println(">>display done");
}

// *************************************************
void xdump(char *arg[])
{
int i;
int start = 0;
int end = BUFFERSIZE-1;
char msg[MAXLINE];

uint32_t data;
uint32_t address;
uint32_t rw, sync, irq;

    for (i=start; i<=start+end; i++){
        address = bits[i] & 0xFFFF;
        data = (bits[i] & 0x00FF0000) >> 16;
        rw =   (bits[i] & 0x01000000) >> 24;
        sync = (bits[i] & 0x02000000) >> 25;
        irq =  (bits[i] & 0x04000000) >> 26;
        sprintf(msg, "%04lx  %02lx %02lx %02lx %02lx", address, data,rw,sync,irq);
        Serial.println(msg);
        Serial.flush();
    }
}


// *************************************************
void xtrigger(char *arg[])
{
int raddr;
char result[16];

    if (arg[0]) {
        raddr = strtol(arg[0], NULL, 16);
        trigger = raddr;
    } 

    sprintf(result, "Trigger %04lx", trigger);
    Serial.println(result);
}


// *************************************************
void xrun(char *arg[])
{
uint32_t clock;
uint32_t data;
uint32_t data0 = 0xFFFF;
uint32_t addr;
uint32_t address;
uint32_t address0;
uint32_t addr_ctrl;
uint32_t rw, sync, irq;

int head=0;
int done=FALSE;
int fail=FALSE;
int record=FALSE;
char msg[MAXLINE];

    digitalWriteFast(TESTPIN1, LOW);
    digitalWriteFast(TESTPIN2, LOW);
    Serial.println("....run starting");
    Serial.flush();
    noInterrupts();

// loop waiting for address to trigger 
while (!record) {

    //wait for clock to be low
    clock=1;
    while(clock) {
        clock = (GPIOB_PDIR & 0x00010000) ;
    }
    
    // wait for clock to go high
    while(!clock) {
        clock = GPIOB_PDIR & 0x00010000 ;
    }

    // read data bits
    digitalWriteFast(TESTPIN1, HIGH);
    addr =  GPIOC_PDIR & 0x0FFF;
    addr_ctrl =  GPIOB_PDIR;
    address0 = addr | ((addr_ctrl & 0x000F) << 12) ;
    rw   = (addr_ctrl & 0x00020000) >> 17;
    sync = (addr_ctrl & 0x00040000) >> 18;
    irq  = (addr_ctrl & 0x00080000) >> 19;

    if(address0 == trigger) {
        record=TRUE;
        digitalWriteFast(TESTPIN2, HIGH);
    }

    // we read data last as it takes memory a while to present it
    data =  GPIOD_PDIR & 0x00FF;

    // address, data and control bits saved into one 32 bit arrary
    bits[head]=address0 | (data << 16) | (rw<<24) | (sync<<25) | (irq<<26);

    digitalWriteFast(TESTPIN1, LOW);
}   // trigger loop

// save first set of address/data/control 
head++;

// loop filling buffer with address/data/control 
while (! done) {
    //wait for clock to be low
    clock=1;
    while(clock) {
        clock = GPIOB_PDIR & 0x00010000 ;
    }
    
    // wait for clock to go high
    while(!clock) {
        clock = GPIOB_PDIR & 0x00010000 ;
    }

    digitalWriteFast(TESTPIN1, HIGH);
    // read address and control line bits
    addr =  GPIOC_PDIR & 0x0FFF;
    addr_ctrl =  GPIOB_PDIR ;
    address = addr | ((addr_ctrl & 0x000F) << 12) ;

    rw   = (addr_ctrl & 0x00020000) >> 17;
    sync = (addr_ctrl & 0x00040000) >> 18;
    irq  = (addr_ctrl & 0x00080000) >> 19;


    // we read data last as it takes memory a while to present it
//     data0 =  GPIOD_PDIR & 0x00FF;

    __asm__ __volatile("NOP");
    __asm__ __volatile("NOP");
    __asm__ __volatile("NOP");
    __asm__ __volatile("NOP");
    __asm__ __volatile("NOP");
    data0 =  GPIOD_PDIR & 0x00FF;
    __asm__ __volatile("NOP");
    data =  GPIOD_PDIR & 0x00FF;
    if (data != data0) {
        fail = TRUE;
        done = TRUE;
    }

    // address, data and control bits saved into one 32 bit arrary
    bits[head++]=address | (data << 16) | (rw<<24) | (sync<<25) | (irq<<26);
    if(head >=BUFFERSIZE)  done=TRUE;

    digitalWriteFast(TESTPIN1, LOW);
} 

    digitalWriteFast(TESTPIN2, LOW);
    interrupts();
    Serial.println("....run complete");
    
    if(fail) {
        Serial.print(head);
        Serial.println("  :run failure...");
        sprintf(msg, "data  %04lx %04lx", data0, data);
        Serial.println(msg);
        sprintf(msg, "address %04lx %04lx", address0, address);
        Serial.println(msg);
    }
}


// *************************************************
void xhelp(char *arg[])
{
    Serial.println("Comands:");
    Serial.println("  AT         -- (A)ttention");
    Serial.println("  T xxxx     -- set (T)igger Address xxxx");
    Serial.println("  R          -- (R)un ");
    Serial.println("  D          -- (D)isplay recorded data ");
    Serial.println("  DD         -- (D)ata (D)ump recorded data ");
    Serial.println("  H          -- Help");
}


// interpeter command list
struct cmd {
    const char *name;             // name of command
    void (*f)(char **);     // command function
};

struct cmd cmdlist[] = {
    {"AT",   xattention  },
    {"R",    xrun  },
    {"T",    xtrigger   },
    {"D",    xdisplay  },
    {"DD",   xdump  },
    {"H",    xhelp   },
    {"?",    xhelp   },
    {'\0',      NULL  }
};

// *****************************************************
void xreadline(char *line)
{
byte ch;
int cc = 0;
int done_reading = FALSE;


    while (!done_reading) {
        if (Serial.available() > 0) {
            ch = Serial.read();
            switch (ch) {
            case 0x0a:
                line[cc]='\0';
                done_reading=TRUE;
                break;
            case 0x0d:
                line[cc]='\0';
                done_reading=TRUE;
                break;
            default: 
                if(cc < MAXLINE) {
                    line[cc++]=ch;
                }
                break;
            }
        }
    }
}


// *************************************************
void setup()
{
// 28 inputs pins.....
// Address A15-A0 = 18 19 17 16 30 29 27 28 12 11 13 10 9 23 22 15
// Data D7-D0 = 5 21 20 6 8 7 14 2
// Control Pins = 25 32 1 0
byte pin_table[] = {18,19,17,16,30,29,27,28,12,11,13,10,9,23,22,15,5,21,20,6,8,7,14,2,25,32,1,0};
int i;

    pinMode(TESTPIN1, OUTPUT);
    pinMode(TESTPIN2, OUTPUT);

    // set all Port pins as inputs
    for(i=0; i<=27; i++) {
        pinMode(pin_table[i], INPUT);
    }

    Serial.begin(115200); 

    // test pins; pulsed hight to show events like reading data etc
    digitalWriteFast(TESTPIN1, LOW);
    digitalWriteFast(TESTPIN2, LOW);
}

 

// *************************************************
int main(void)
{
char line[MAXLINE];
char *command;
char *arg_list[8];
int i,j;

setup();


while(1) {
    line[0]='\0';
    arg_list[0]=NULL;

    Serial.println("<eot>");
    // read command line from serial port

    xreadline(line);
    strupr(line);
    command = strtok(line," ");     // command should be the first thing

    if(command != NULL) {
     for(i=0; cmdlist[i].name; i++) {   // loop thru list look for command
        if(strcmp(command, cmdlist[i].name) == 0) {
            j=0;
            do {    // split line into an array of tokens
                arg_list[j] = strtok(NULL," ");
            } while(arg_list[j++] != NULL);
            // dispatch command 
            cmdlist[i].f(arg_list);     // exec the command with args
            break;
        }
     } 
    }

} // while
} // main
