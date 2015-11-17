

la65.cpp Notes
--------------

The Makefile for la65.cpp is for the  command line.

I have version 1.6.1 of the Arduino IDE installed under my HOME directory
($HOME/opt/arduino-1.6.1) but I don't use it.  The Makefile is intended for
command line use.  The makefile points to the IDE tools (complier, objdump,
etc) but I keep a precompiled Teensy library & includes in a local source
directories ($HOME/src/teensy/lib and $HOME/src/teensy/include).

If you are using the IDE and want to use this code you will have to modify
the code to get rid of "main" and move all the code to the IDE "loop" section.

cli65 Notes
-----------
If you do a "make upload" to send a new program to the Teensy while the command
line (cli65) is running on the Linux box it will break the serial
communications.  You should quit the CLI before uploading a new program to the
Teensy.  

You will probably have to control-C out of the CLI program and this will leave
the tty settings all messed up. I ahve a TODO to fix this mean while you will
have to try "stty sane" and/or "reset" to recover you terminals sanity.

trace65 Notes
-------------
I usually capture the recorded data using the CLI dump command "dump
dump_filename" and then disassembly using "trace65". If you pipe the output to
a file "trace dump_file > disassm_file" you can view the result in your
favorite editor.

john
2015/11/17

