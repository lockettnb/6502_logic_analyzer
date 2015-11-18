# The name of your project (used to name the compiled .hex file)
# TARGET = $(nodir $CURDIR) 
TARGET = la65

# Path to arduino installation
ARDUINOPATH = /home/john/opt/arduino-1.6.1
TEENSYPATH = /home/john/src/teensy
INC = $(TEENSYPATH)/include
LIB = $(TEENSYPATH)/lib

# configurable options
OPTIONS = -DF_CPU=48000000 -DUSB_SERIAL -DLAYOUT_US_ENGLISH -DUSING_MAKEFILE

# options needed by many Arduino libraries to configure for Teensy 3.0
OPTIONS += -D__MK20DX256__ -DARDUINO=10600 -DTEENSYDUINO=121

# path location for Teensy Loader, teensy_post_compile and teensy_reboot
TOOLSPATH = $(ARDUINOPATH)/hardware/tools
COMPILERPATH = $(TOOLSPATH)/arm/bin

# compiler options for C only
# -c -O -g -Wall -ffunction-sections -fdata-sections -MMD -nostdlib -mthumb -mcpu=cortex-m4 -D__MK20DX256__ -DTEENSYDUINO=122 -DARDUINO=10601 -DF_CPU=96000000 -DARDUINO_ARCH_AVR -DUSB_SERIAL -DLAYOUT_US_ENGLISH -I/home/john/opt/arduino-1.6.1/hardware/teensy/avr/cores/teensy3 /home/john/opt/arduino-1.6.1/hardware/teensy/avr/cores/teensy3/serial1.c -o /tmp/build696379344937726292.tmp/serial1.c.o

CFLAGS = -c -O -g -Wall -ffunction-sections -fdata-sections -MMD -nostdlib -mthumb -mcpu=cortex-m4 -D__MK20DX256__ -DTEENSYDUINO=122 -DARDUINO=10601 -DF_CPU=96000000 -DARDUINO_ARCH_AVR -DUSB_SERIAL -DLAYOUT_US_ENGLISH 

# compiler options for C++ only
# CXXFLAGS = -std=gnu++0x -felide-constructors -fno-exceptions -fno-rtti
# -c -O -g -Wall -ffunction-sections -fdata-sections -MMD -nostdlib -fno-exceptions -felide-constructors -std=gnu++0x -fno-rtti -mthumb -mcpu=cortex-m4 -D__MK20DX256__ -DTEENSYDUINO=122 -DARDUINO=10601 -DF_CPU=96000000 -DARDUINO_ARCH_AVR -DUSB_SERIAL -DLAYOUT_US_ENGLISH -I/home/john/opt/arduino-1.6.1/hardware/teensy/avr/cores/teensy3 /home/john/opt/arduino-1.6.1/hardware/teensy/avr/cores/teensy3/Print.cpp -o /tmp/build696379344937726292.tmp/Print.cpp.o

CXXFLAGS = -c -O -g -Wall -ffunction-sections -fdata-sections -MMD -nostdlib -fno-exceptions -felide-constructors -std=gnu++0x -fno-rtti -mthumb -mcpu=cortex-m4 -D__MK20DX256__ -DTEENSYDUINO=122 -DARDUINO=10601 -DF_CPU=96000000 -DARDUINO_ARCH_AVR -DUSB_SERIAL -DLAYOUT_US_ENGLISH 


# linker options
# LDFLAGS = -Os -Wl,--gc-sections,--defsym=__rtc_localtime=0 --specs=nano.specs -mcpu=cortex-m4 -mthumb -T$(INC)/mk20dx256.ld
# -O -Wl,--gc-sections,--relax,--defsym=__rtc_localtime=1428588589 -T/home/john/opt/arduino-1.6.1/hardware/teensy/avr/cores/teensy3/mk20dx256.ld -mthumb -mcpu=cortex-m4 -o /tmp/build696379344937726292.tmp/Blinky_hi_low.cpp.elf /tmp/build696379344937726292.tmp/Blinky_hi_low.cpp.o /tmp/build696379344937726292.tmp/core.a -L/tmp/build696379344937726292.tmp -larm_cortexM4l_math -lm

LDFLAGS = -O -Wl,--gc-sections,--relax,--defsym=__rtc_localtime=0 -T$(INC)/mk20dx256.ld -mthumb -mcpu=cortex-m4

# libraries to link... my version of the library
LIBS = $(LIB)/core.a -larm_cortexM4l_math -lm

# names for the compiler programs
CC = $(abspath $(COMPILERPATH))/arm-none-eabi-gcc
CXX = $(abspath $(COMPILERPATH))/arm-none-eabi-g++
OBJCOPY = $(abspath $(COMPILERPATH))/arm-none-eabi-objcopy
OBJDUMP= $(abspath $(COMPILERPATH))/arm-none-eabi-objdump
SIZE = $(abspath $(COMPILERPATH))/arm-none-eabi-size

# automatically create lists of the sources and objects
# TODO: this does not handle Arduino libraries yet...
# C_FILES := $(wildcard *.c)
# CPP_FILES := $(wildcard *.cpp)
# OBJS := $(C_FILES:.c=.o) $(CPP_FILES:.cpp=.o)


.SECONDARY:

all: $(TARGET).hex
	make -C cli65
	make -C trace65

#$(TARGET).elf: $(OBJS) mk20dx256.ld
#$(TARGET).elf: $(OBJS)
#	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

%.hex: %.elf
	$(SIZE) $<
	$(OBJCOPY) -O ihex -R .eeprom $< $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I$(INC) -o "$@" "$<"

%.o: %.c
	$(CC) $(CFLAGS) -I$(INC) -o "$@" "$<"

%.elf: %.o
	$(CC) $(LDFLAGS) -o "$@" "$<" -L$(LIB) $(LIBS)

# if we upload while cli65 has ttyACM0 open the teensy will reboot   
# and find port 0 busy and get added as ttyACM1 ....the wrong port
upload: $(TARGET).hex
	-killall cli65
	$(abspath $(TOOLSPATH))/teensy_post_compile -file=$(TARGET) -path=$(shell pwd) -tools=$(abspath $(TOOLSPATH))
	-$(abspath $(TOOLSPATH))/teensy_reboot

dump: 
	$(OBJDUMP) -d $(TARGET).elf

# compiler generated dependency info
# -include $(OBJS:.o=.d)

clean:
	rm -f *.o *.d $(TARGET).elf $(TARGET).hex


