/*/////////////////////////////////////////////////////////////////////////////
//                             OS Zero v0.0                                  //
//      Simplest cooperative multitasking kernel intended to smallest        //
//      8bit microcontrollers. Implemented by STVD and CXSTM8 compiler       //
;/             (C) Simplest System Solutions (canceled)                      //
//	is a personal logo of independent developer Vyacheslav Azarov        //
//      License bellow. Email: slavaza63@gmail.com  Skipe: slavaza63         //
//////////////////////////////////////////////////////////////////////////// */

#include <stdio.h>
#include "kernel.h"

#define LED_PORT	PA
#define LED_PIN		2

#define PORT_DDR(port) port##_DDR
#define PORT_IDR(port) port##_IDR
#define PORT_ODR(port) port##_ODR
#define PORT_CR1(port) port##_CR1
#define PORT_CR2(port) port##_CR2

#define set_pin_floating(port, pin)\
PORT_DDR(port)&=(char)~(1 << (pin));PORT_CR1(port)|=(char)~(1 << (pin));\
				    PORT_CR2(port)&=(char)~(1 << (pin))
#define set_pin_output(port, pin)\
PORT_DDR(port)|=(char)(1 << (pin));PORT_CR1(port)|= (char) (1 << (pin))

#define get_pin(port, pin)	 (PORT_ODR(port) &  (char) (1 << (pin)))
#define set_pin_low(port, pin)	  PORT_ODR(port) &= (char)~(1 << (pin))
#define set_pin_high(port, pin)	  PORT_ODR(port) |= (char) (1 << (pin))

void * mutex = NULL;
#define grab(m)     await(m == NULL || m == self); m = self
#define release(m)  if (m == self) m = NULL
//
// structure of processing data
//
typedef struct my_data_t {
	unsigned int	blink_time;
	unsigned int	blink_number;
}
	my_data_t;
//
// resources of process instances
//
struct {
	char __[169]; // preserve space for the stack
	my_data_t _;  // fields depend on application
}
	inst1, inst2;
//
// reentrant processing script
//
 void process(void)
{
// sharing
	grab(mutex);  
	printf("thread %p started.\n", self);
	release(mutex);
// play
	while (((my_data_t*)self)->blink_number-- != 0)
	{
// pause
		delay(((my_data_t*)self)->blink_time);
// sharing
		grab(mutex);
		printf("thread %p %d\n", self, 
			((my_data_t*)self)->blink_number);
		release(mutex);
// blink
		if (get_pin(LED_PORT, LED_PIN) != 0) 
			set_pin_low(LED_PORT, LED_PIN);
		else 
			set_pin_high(LED_PORT, LED_PIN);
	}
	set_pin_high(LED_PORT, LED_PIN);
// sharing
	grab(mutex);
	printf("thread %p done.\n", self);
	release(mutex);
// die
}

#define UART_CR2_TEN	(1 << 3)
#define UART_CR3_STOP2	(1 << 5)
#define UART_CR3_STOP1	(1 << 4)
#define UART_SR_TXE	(1 << 7)
#define UART_SR_RXNE	(1 << 5)

void stdio_init(void)
{
	set_pin_floating(PC, 2);//57 rx USART connections
	set_pin_floating(PC, 3);//58 tx
        SYSCFG_RMPCR1 &= (char)~0x30; // remap to PC2,PC3

//	CLK_DIVR  = 0x00;	// Set the frequency to 16 MHz
	CLK_PCKENR1 = 0xFF;	// Enable peripherals
//
	USART1_CR3 &= (char)~(UART_CR3_STOP1 | UART_CR3_STOP2); // 1 stop bit
#if (0)
	USART1_BRR2 = 0x03; USART1_BRR1 = 0x68; // 9600 baud
#else
	USART1_BRR2 = 0x1B; USART1_BRR1 = 0xA0; // 2400 baud
#endif
	USART1_CR2 = UART_CR2_TEN; // Allow TX	
}


char getchar(void)
{
	while(!(USART1_SR & UART_SR_RXNE)) yield();
	return USART1_DR;
}

char putchar(char c)
{
	while(!(USART1_SR & UART_SR_TXE)) yield();
	return USART1_DR = c;	
}

int main(void)
{
	start();
	stdio_init();
	
	puts("started!");
	
// init led
	set_pin_output(LED_PORT, LED_PIN);

// aligth 
	set_pin_low(LED_PORT, LED_PIN);
	
// instance 1
	inst1._.blink_time	= 1000; // ms
	inst1._.blink_number	= 10;
	spawn(&inst1._, process);
	
// instance 2
	inst2._.blink_time	= 1111; // ms
	inst2._.blink_number	= 20;
	spawn(&inst2._, process);

// launch
	yield();
	
// root loop
	await(!alive(&inst1._) && !alive(&inst2._));
	
// the end
	puts("finished!");
	
// aligth 
	set_pin_low(LED_PORT, LED_PIN);
	
	delay(5000);
	
//  extinguish
	set_pin_high(LED_PORT, LED_PIN);
	
	puts("reset!");
	delay(1000);	

// live loop
	return reset(), 0;
}

/*/////////////////////////////////////////////////////////////////////////////
//                              LICENSE                                      //
//  Nothing can prevent you to use these materials in any of your purposes   //
//     in case of mention of the author and his logo in your projects.       //
//////////////////////////////////////////////////////////////////////////// */

