PRG            = tcp_server
OBJ            = $(PRG).o socket.o wizchip_conf.o w5500.o usart.o

VPATH 		   = src:../lib/Ethernet:../lib/Ethernet/W5500
INC	           = -I ../lib/Ethernet

MCU_TARGET     = atmega1284p
OPTIMIZE       = -O2    # options are 1, 2, 3, s
CC             = avr-gcc
F_CPU          = 20000000UL

override CFLAGS       = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS) -DF_CPU=$(F_CPU) \
   $(INC)
override LDFLAGS      = -Wl,-Map,$(PRG).map

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

all: $(PRG).elf lst text eeprom

$(PRG).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

tcp_server.o: tcp_server.c tcp_server.h
	$(CC) $(CFLAGS) -c $<
 
socket.o: socket.c socket.h
	$(CC) $(CFLAGS) -c $<

wizchip_conf.o: wizchip_conf.c wizchip_conf.h
	$(CC) $(CFLAGS) -c $<

w5500.o: w5500.c w5500.h
	$(CC) $(CFLAGS) -c $<

usart.o: usart.c usart.h
	$(CC) $(CFLAGS) -c $<

clean: 
	rm -rf *.o $(PRG).elf *.bin *.hex *.srec *.bak  
	rm -rf $(PRG)_eeprom.bin $(PRG)_eeprom.hex $(PRG)_eeprom.srec
	rm -rf *.lst *.map 

#setup for for USB programmer
#may need to be changed depending on your programmer
program: $(PRG).hex
	sudo avrdude -c usbasp -p atmega1284p -e -U flash:w:$(PRG).hex  -v

lst:  $(PRG).lst

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

# Rules for building the .text rom images

text: hex bin srec

hex:  $(PRG).hex
bin:  $(PRG).bin
srec: $(PRG).srec

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@

# Rules for building the .eeprom rom images

eeprom: ehex ebin esrec

ehex:  $(PRG)_eeprom.hex
ebin:  $(PRG)_eeprom.bin
esrec: $(PRG)_eeprom.srec

%_eeprom.hex: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@

%_eeprom.srec: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O srec $< $@

%_eeprom.bin: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O binary $< $@
