6502 Logic Analyser (Analyzer)
==============================

Logic Analyser (with program trace/disassemble) for the world famous 6502 CPU.

This system uses the  Teensy 3.1 microcontroller to do the hard work of
monitoring and recording the address/data/control signals on the 6502
processor. The Teensy is controlled via a CLI program on a Linux computer to
set trigger addresses and start the record process.  The recorded data can be
uploaded to a linux computer where a trace program will disassembly the data.
The results show the program flow in 6502 assembler and includes all
read/writes of data.

The following example shows the 6502 CPU fetching the reset vector
0xFFFC/0xFFFD and the starting to execute the program at address 0xA000
The minus sign (-) show data reads and plus signs (+) show data writes.

    fffc+ 00 
    fffd+ a0 
    a000  d8        CLD
    a001+ a9 
    a001  a9 00     LDA #$00
    a003  8d 01 f8  STA $f801
    f801- 00 
    a006  8d 03 f8  STA $f803
    f803- 00 
    a009  a9 f0     LDA #$f0
    a00b  8d 00 f8  STA $f800
    f800- f0 
    a00e  a9 03     LDA #$03
    a010  8d 02 f8  STA $f802
    f802- 03 
    a013  a9 04     LDA #$04
    a015  8d 01 f8  STA $f801
    f801- 04 
    a018  8d 03 f8  STA $f803
    f803- 04 

PS... the code is from the first Meet-Me Conference Bridge which was
developed on an Ohio Scientific computer.
