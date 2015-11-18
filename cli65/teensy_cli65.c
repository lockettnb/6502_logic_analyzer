/*
*   2014/11/21 created
*   2015/05/17 adapted to work with 6502 ICE on port
*   2015/11/03 adapted to work with 6502 logic analyser
*
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>

#include "serial.h"

#define TRUE 1
#define FALSE 0
#define NULL_CHAR '\0'
#define RX_BUFFERSIZE 200000 

// Global Variables
char *program_name;
int port;               // file handle for tty port
int done = FALSE;

// buffer to receive response from teensy
char rx_buffer[RX_BUFFERSIZE];


/* Option Flags set by `--options' */
int verbose = FALSE;
int help    = FALSE;
int version = FALSE;
int debug   = FALSE;


// *************************************************
// trim whitespace from left and right of string
//   2015/05/10 created
char *trimwhite(char *s)
{
char *tail;

    // trim left (front) 
    while(isspace(*s)) s++;

    // trim right (back/tail)
    tail = s + strlen(s);
    while(isspace(*--tail));
    *(tail+1) = '\0';

    return s;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void chomp(char *s) 
{
char *tail;

    tail = s + strlen(s);
    while(iscntrl(*--tail));
    *(tail+1) = '\0';
}


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int read_response(char *s)
{
char buf[32];
int done=FALSE;
int i;
s[0]='\0';

    while (!done) {
        serialport_read_until(port, buf, '\n', 31,5000);
        chomp(buf);
        if(strcmp(buf, "<eot>") ==0) done=TRUE;
        printf("%s\n", buf);

        if(!done) { //record serial data to buffer
            sprintf(buf, "%s\n", buf);
            strcat(s, buf);
        }
    }
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void xhelp(char *arg[])
{
    printf(">>help\n");

int i = 0;
char *helptext[] = {
    " ",
    "set     +set trigger address     -- set <addr>",
    "run     +run caputre and record data at trigger address",
    "disp    +display                 -- disp <start addr> [end addr]",
    "dump    +dump entire data buffer -- dump <filename> ",
    "history +display command history",
    "q       +exit program",
    "quit    +exit program",
    "                                                     jal 2015/11",
    NULL_CHAR
    };

    while (helptext[i] != NULL_CHAR){
        printf("%s\n", helptext[i++]);
    }
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void xalive(char *arg[])
{
char buf[32];
int rc;

    sprintf(buf, "at\n");
    rc = serialport_write(port, buf);    
    if(rc !=0) {
        fprintf(stderr, "Alive: failed to write to Teensy serial port\n");
        return;
    }
    read_response(rx_buffer);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void xquit(char *arg[])
{
    printf(">>quit and exit\n");
    done = TRUE;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void xset(char *arg[])
{
char *addr;
char buf[32];
char resetvector[6]="fffc";
int rc;

    addr = " ";
    if(arg[0] != NULL) {
        addr = arg[0]; 
    }

    sprintf(buf, "t %s\r", addr);

    rc = serialport_write(port, buf);    
    if(rc !=0) {
        fprintf(stderr, "Set: failed to write to Teensy serial port\n");
        return;
    }

    read_response(rx_buffer);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void xrun(char *arg[])
{
char buf[32];
int rc;

    sprintf(buf, "r\r");
    rc = serialport_write(port, buf);    
    if(rc !=0) {
        fprintf(stderr, "Run: failed to write to Teensy serial port\n");
        return;
    }
    read_response(rx_buffer);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void xdisp(char *arg[])
{
char buf[32];
char *start = "0";
char *end = "32";
int rc;

    if(arg[0] != NULL) start = arg[0]; 
    if(arg[1] != NULL) end = arg[1]; 

    sprintf(buf, "d %s %s \r", start, end);

    rc = serialport_write(port, buf);    
    if(rc !=0) {
        fprintf(stderr, "Display: failed to write to Teensy serial port\n");
        return;
    }

    read_response(rx_buffer);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void xdump(char *arg[])
{
char buf[32];
FILE *fp;
int rc;
    sprintf(buf, "dd\r");

    rc = serialport_write(port, buf);    
    if(rc !=0) {
        fprintf(stderr, "Dump: failed to write to Teensy serial port\n");
        return;
    }

    read_response(rx_buffer);

    if(arg[0] != NULL){
        fp=fopen(arg[0], "w"); 
        if(fp == NULL) {
            fprintf(stderr, "Dump: Error opending file\n");
            return;
        }
        fprintf(fp, "%s", rx_buffer);
        fclose(fp);
    }
}


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void xhistory(char *arg[])
{
    printf(">>history\n");
 HIST_ENTRY **hlist;
 int i;

 hlist = history_list ();
 if (hlist)
     for (i = 0; hlist[i]; i++)
          printf ("%i: %s\n", i + history_base, hlist[i]->line);
}


void int_handler(int sig) {

    printf("\n stopping CLI process \n");
    done = TRUE;
}
    


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
main (int argc, char *argv[])
{
char *line, *trimline, *command;
char *arg_list[12];
int i,j; 
struct termios old_termio;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// monitor command list
struct cmd {
    const char *name;             // name of command
    void (*f)(char **);     // command function
};

struct cmd cmdlist[] = {
    {"q",       xquit  },
    {"quit",    xquit  },
    {"set",     xset},
    {"disp",    xdisp},
    {"dump",    xdump},
    {"run",     xrun   },
    {"alive",   xalive  },
    {"at",      xalive  },
    {"help",    xhelp},
    {"history", xhistory  },
    {'\0',      NULL  }
};

signal(SIGINT, int_handler);

// open port then wait a bit for port to reset
port = serialport_init( "/dev/ttyACM0", 115200, &old_termio);
sleep(2);
xalive(arg_list);

// loop reading and executing commands  
while (! done) {
    line = readline ("CLI6502> ");
    if (line == NULL) { 
        fprintf(stderr, "Read Loop: failed to read command line\n");
        exit(1);
    }

    trimline = trimwhite (line);

    if (*trimline) {    // as long as this line is not blank add to history
      add_history (trimline);
    }

    command = strtok(trimline," ");     // command should be the first thing
    if(command != NULL) {
     for(i=0; cmdlist[i].name; i++) {   // loop thru list look for command
        if(strcmp(command, cmdlist[i].name) == 0) {
            j=0;
            do {    // split line into an array of tokens
                arg_list[j] = strtok(NULL," ");
            } while(arg_list[j++] != NULL);
            arg_list[j]=NULL;
            // dispatch command 
            cmdlist[i].f(arg_list);     // exec the command with args
            break;
        }
     } 
    }
    free (line);
}
    serialport_close(port, &old_termio);

} //main


