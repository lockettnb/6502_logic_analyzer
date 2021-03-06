//
// arduino-serial-lib -- simple library for reading/writing serial ports
// 2006-2013, Tod E. Kurt, http://todbot.com/blog/
//
// 2015/11/17 added save/restore of ports termio settings (john)
//



#ifndef __ARDUINO_SERIAL_LIB_H__
#define __ARDUINO_SERIAL_LIB_H__

#include <stdint.h>   // Standard types 
#include <termios.h>  // POSIX terminal control definitions 

int serialport_init(const char* serialport, int baud, struct termios *old_toptions);
int serialport_close(int fd, struct termios *old_toptions);
int serialport_writebyte( int fd, uint8_t b);
int serialport_write(int fd, const char* str);
int serialport_read_until(int fd, char* buf, char until, int buf_max,int timeout);
int serialport_flush(int fd);

#endif

