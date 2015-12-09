/* boot.c - a trivial serial port bootloader for LM32.

   Public domain.

   Awful code below. Be warned. */

#include <stdint.h>

#include "board.h"
#include "uart.h"

#define USER_START 0xa00

//const char hexchars[] = "0123456789abcdef";


int read_blocking(uint8_t *what)
{
	int cnt = 500000;
	int b;
	while(cnt--)
	{ 
		b=uart_read_byte();
		if(b >= 0)
			break;
	}

	if(cnt > 0)
	{
		*what = b;
		return 0;
	}else
		return -1;
}

main()
{
	int i;
	
	int len, boot_active = 0;
	uint8_t *ptr;
	
	uart_init_hw();

#ifdef SIMULATION
	void (*f)() = USER_START;	
	f();
#endif
    
	again:
	len = 0;
	boot_active = 0;
	ptr = (uint8_t*)USER_START;

	uart_write_byte('B');
	uart_write_byte('o');
	uart_write_byte('o');
	uart_write_byte('t');
	uart_write_byte('?');
	
	for(i=0;i<500000;i++)
		if(uart_read_byte () == 'Y')
		{
			boot_active = 1;
			break;
		}
	
	if(boot_active)
	{
		uint8_t b;
		uart_write_byte('O');
		uart_write_byte('K');

		if(read_blocking(&b) < 0) goto again;
		len = (uint32_t)b;

		len <<=8;
		if(read_blocking(&b) < 0) goto again;
		len |= (uint32_t)b;

		len <<=8;
		if(read_blocking(&b) < 0) goto again;
		len |= (uint32_t)b;

		len <<=8;
		if(read_blocking(&b) < 0) goto again;
		len |= (uint32_t)b;
		
		for(i=0;i<len;i++)
		{
			if(read_blocking(ptr) < 0)
				goto again;
			ptr++;
		}

		uart_write_byte('G');
		uart_write_byte('o');
		uart_write_byte('!');


    		void (*f)() = USER_START;	
		f();

		
	} else {
		uart_write_byte('T');
		uart_write_byte('o');
		uart_write_byte('u');
		uart_write_byte('t');


    		void (*f)() = USER_START;	
		f();
//		goto again;
	}

	
}
