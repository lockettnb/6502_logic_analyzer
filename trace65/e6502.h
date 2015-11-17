/*
*
*   2014/10/30 created
*   2015/11/12 adapted for trace 6502
*/

#define NULL_CHAR '\0'
#define TRUE -1
#define FALSE 0
#define SUCCESS 0
#define FAIL 1


// used in matching the address n the opcode (n or nn) in a regex
#define MAXMATCH 3

// execute a single instruction
int execute();
