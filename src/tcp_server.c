#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <math.h>

#include "tcp_server.h"
#include "socket.h"
#include "uart.h"
#include "jsmn.h"

static const char *JSON_STRING = "{\"cmd\": \"Launch\", \"cmd\": 12, \"incorrect\": 5}";

struct json_object{
    uint8_t type;
    char* name;
    union _data {
        int integer;
        float decimal;
        char* string;
        } data;
    };

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
  if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
      strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
    return 0;
  }
  return -1;
}

static uint8_t json_serialize(struct json_object passed[], uint8_t num_objects, char* json_string){
    uint8_t pointer = 0;
    uint8_t temp_pointer = 0;
	json_string[pointer++] = '{';
    char temp[10];
	for (int i = 0; i < num_objects; i++){
	    json_string[pointer++] = '"';
        for(int j = 0; j < strlen(passed[i].name); j++){
            json_string[pointer++] = passed[i].name[j];
        }
	    json_string[pointer++] = '"';
	    json_string[pointer++] = ':';
	    json_string[pointer++] = ' ';
		switch(passed[i].type){			
            case 0:
                itoa(passed[i].data.integer,temp, 10);
                for(int j = 0; j < strlen(temp); j++){
                    json_string[pointer++] = temp[j];
                }
                break;
            case 1:
                temp_pointer = 0;
                dtostrf(passed[i].data.decimal, 10, 1, temp);
                while(temp[temp_pointer] == ' '){
                    temp_pointer += 1;
                }
                        
                for(int j = 0; j < strlen(temp) - temp_pointer; j++){
                    json_string[pointer++] = temp[temp_pointer + j];
                }
                break;
            case 2:
	            json_string[pointer++] = '"';
                for(int j = 0; j < strlen(passed[i].data.string); j++){
                    json_string[pointer++] = passed[i].data.string[j];
                }
	            json_string[pointer++] = '"';
                break;

		}	
        if(i+1 < num_objects){
	        json_string[pointer++] = ',';
        }
	}
	json_string[pointer++] = '}';
	json_string[pointer++] = 0;
    

	return 0;
}
    
void initSPI()
{
    DDRB |= (1 << PB5) | (1 << PB3) | (1 << PB2); //sck, mosi, ss outputs
    SPCR |= (1 << SPE) | (1 << MSTR);
}

void initTCNT0()
{
    TIMSK0 |= (1 << TOIE0); //enable overflow interrupt 
    TCCR0B |= (1 << CS02) | (1 << CS00); //1024 prescaling
}

void disableTCNT0()
{
    TCCR0B = 0;
}

void enableTCNT0()
{
    TCCR0B |= (1 << CS02) | (1 << CS00); //1024 prescaling
}

ISR(TIMER0_OVF_vect)
{
    uint8_t s0_ir = getSn_IR(0);

    if(!bit_is_clear(s0_ir, 2))
    {
        writeString("recv interrupt received!\r\n");
        setSn_IR(0, (s0_ir & (1 << 2))); //clear interrupt
        PORTB ^= (1 << 0);
    }
}

int32_t loopback_tcps(uint8_t sn, uint8_t* buf, uint16_t port)
{
    int32_t ret;
    uint16_t size = 0, sentsize = 0;

    switch(getSn_SR(sn))
    {
        case SOCK_ESTABLISHED:
            if(getSn_IR(sn) & Sn_IR_CON)
            {
                setSn_IR(sn,Sn_IR_CON);
            }
            if((size = getSn_RX_RSR(sn)) > 0) // Don't need to check SOCKERR_BUSY because it doesn't not occur.
            {
                if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE; // clips size if larger that data buffer
                ret = recv(sn, buf, size);

                if(ret <= 0) return ret;      // check SOCKERR_BUSY & SOCKERR_XXX. For showing the occurrence of SOCKERR_BUSY.
                size = (uint16_t) ret;
                sentsize = 0;

                while(size != sentsize)
                {
                    writeNumChar("First char received: ", *buf, 10);
                    ret = send(sn, buf+sentsize, size-sentsize);
                    if(ret < 0)
                    {
                        close(sn);
                        return ret;
                    }
                    sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
                }
            }
            break;
        case SOCK_CLOSE_WAIT :
            if( (ret = disconnect(sn)) != SOCK_OK) return ret;
            break;
        case SOCK_INIT :
            if( (ret = listen(sn)) != SOCK_OK) return ret;
            break;
        case SOCK_CLOSED:
            if( (ret = socket(sn, Sn_MR_TCP, port, 0x00)) != sn) return ret;
        default:
            break;
    }
    return 1;
}

int main()
{
    initUART();
    initSPI();
    //initTCNT0();
    DDRB |= 0xff;
	_delay_ms(1000);
    PORTB ^= 0xff;
	_delay_ms(1000);
    PORTB ^= 0xff;
	
	_delay_ms(1000);
	writeString("Started\n");

    struct json_object test1, test2, test3;
    
    test1.type = 0;
    test1.name = "int";
    test1.data.integer = 5;
    
    test2.type = 1;
    test2.name = "float";
    test2.data.decimal = 5.2;

    test3.type = 2;
    test3.name = "string";
    test3.data.string = "this is a string";
    
    char string[128];
    struct json_object topass[10];
    topass[0] = test1;
    topass[1] = test2;
    topass[2] = test3;
    json_serialize(topass, 3, string);

    writeString(string);
    
    return 0;

    uint8_t buf[128];
	int r;
	jsmn_parser p;
	jsmntok_t t[128];

	jsmn_init(&p);
	r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t, sizeof(t)/sizeof(t[0]));
	if (r < 0){
		writeString("Failed to parse JSON");
	} 
	
	for (int i = 1; i < r; i++){
		if (jsoneq(JSON_STRING, &t[i], "cmd") == 0) {
			uint8_t token_len = t[i +1].end - t[i +1].start;
			char token[token_len+1];
			for(int j = 0; j < token_len; j++){
				token[j] = *(JSON_STRING + t[i +1].start + j);
			}
			token[token_len] = 0;
			char temp[20];
			itoa(token_len,temp,10);
			writeString(temp);
			writeString("\n");
			writeString(token);
			writeString("\n");
			//writeString(JSON_STRING + t[i + 1].start);
		}
	}
	return 0;
    struct wiz_NetInfo_t network_config = 
    {
        {MAC},
        {IP},
        {SUBNET},
        {GATEWAY},
        {DNS},
        2
    };

    struct wiz_NetInfo_t temp;

    uint8_t txsize[8] = {1, 0, 0, 0, 0, 0, 0, 0};
    uint8_t rxsize[8] = {1, 0, 0, 0, 0, 0, 0, 0};

    //setup delay
    _delay_ms(2000);

    writeNumChar("Init return: ", wizchip_init(txsize, rxsize), 10);
    wizchip_setnetinfo(&network_config);
    wizchip_getnetinfo(&temp);

    writeNumChar("ip[0]: ", temp.ip[0], 10);
    writeNumChar("ip[1]: ", temp.ip[1], 10);
    writeNumChar("ip[2]: ", temp.ip[2], 10);
    writeNumChar("ip[3]: ", temp.ip[3], 10);

    writeNumChar("version: ", getVERSIONR(), 16);
    writeNumShort("retry count: ", getRTR(), 10);

    sei(); //enable global interrupts

    while(1)
    {
        //disableTCNT0();
        loopback_tcps(0, buf, 8080);
        writeNumChar("Socket 0 SR: ", getSn_SR(0), 16);
        //enableTCNT0();
        _delay_ms(500);
    }

    return 0;
}

