/*/////////////////////////////////////////////////////////////////////////////
//                             OS Zero v0.0                                  //
//      Simplest cooperative multitasking kernel intended to smallest        //
//      8bit microcontrollers. Implemented by STVD and CXSTM8 compiler       //
;/             (C) Simplest System Solutions (canceled)                      //
//	is a personal logo of independent developer Vyacheslav Azarov        //
//      License bellow. Email: slavaza63@gmail.com  Skipe: slavaza63         //
//////////////////////////////////////////////////////////////////////////// */

#include "kernel.h"

/* Implementation */

volatile unsigned int	msec = 0;
volatile void *		self = NULL;

void *	pending_queue[PENDING_QUEUE_LIMIT+1];
void **	pending_limit = &pending_queue[0];
void **	pending_point = &pending_queue[0];

static volatile union {
	unsigned long long_counter;
	unsigned int  counter[2];
}
	systick;

static void _remove_from_queue(void)
{
	void ** p;
	if  (pending_point == pending_limit) 
		pending_point = &pending_queue[0];
	else 
		for (p = pending_point; p != pending_limit; p++) 
			*p = *(p+1);
	pending_limit--;
}

static void _terminate(void)
{
// remove from queue
	_remove_from_queue();
	
// indicate on death
	*((void**)self-1) = NULL;

// resume next thread
	_asm("\tldw x, [_pending_point.w]\n\tldw sp, x\n");
	_asm("\tpopw x\n\tldw _self, x\n\tpop cc\n");
}


bool spawn(void * stack, void (*start)(void))
{
// overflaw
	if (pending_limit == pending_queue[PENDING_QUEUE_LIMIT])
	return false;

// initial
	(*(void **)(((char*)stack)-2)) 	= (void *)&_terminate;
	(*(void **)(((char*)stack)-4)) 	= (void *)start;
	*((char *)stack-5)		= (char)_asm("\tpush cc\n\tpop a\n");
	(*(void **)(((char *)stack)-7)) = stack;
	*(++pending_limit) 		= ((char *)stack)-8;

// success
	return true;
}

bool alive(void * stack)
{
	return (stack == NULL) || (*((void **)stack-1) != NULL);
}

void die(void)
{
	if (self == NULL) reset();
	else _asm("\tldw x, _self\n\tsubw	 x, #2\n\tldw sp, x\n");
}

unsigned long millis(void) // atomic
{
	char cc = (char)_asm("\tpush cc\n\tpop a\n\tsim\n");
	unsigned long result = systick.long_counter;
	_asm("\tpush a\n\tpop cc\n", (char)cc);
	return result;
}

void delay(unsigned long ms)
{
	unsigned long start = millis();
	while (millis() - start < ms) yield();
}

void reset(void) // atomic 
{
	_asm("\tsim\n"); __reset();
	IWDG_KR = 0xCC; IWDG_PR = 0x00; IWDG_RLR  = 0x00; for(;;);
}

void start(void) // atomic
{
	_asm("\tsim\n");
// clock
	CLK_DIVR	= 0x00; // CPU=MASTER=HSI
//	CLK_SWR		= 0xE1; // HSI=16MHz
	CLK_PCKENR1    |= 0xFF; // Enable peripherals
	
// 1 ms
	TIM4_ARR	= 250;	// 1/250
	TIM4_PSCR	= 6;	// 1/64
	TIM4_SR1	= 0;	// clear flags
	TIM4_CR1	= 1;    // start counting
	
// interrupt
	ITC_SPR7	|= 0x0C; // TIM 4 level 3	
	TIM4_IER	 = 0x01; // int enable
	_asm("\trim\n");
}

@interrupt void _tick_handler(void) // highest
{
  	_asm("\tsim\n");
	if (!++systick.counter[1]) systick.counter[0]++;
	msec++;
	TIM4_SR1 &= (char)~0x01; // clear pending
}
/*
#pragma asm
_yield:
		push	cc
		sim
		ldw	x, _self
		pushw	x
		ldw	y, sp
		ldw	x, _pending_point
		ldw	(x), y
		cpw	x, _pending_limit
		jrne	101$
		ldw	x, #_pending_queue
		jra	102$
101$:
		addw	x,#2
102$:
		ldw	_pending_point,x
		ldw	x, (x)
		ldw 	sp, x
		popw	x
		ldw	_self, x
		pop	cc
		ret
#pragma endasm
*/
/*/////////////////////////////////////////////////////////////////////////////
//                              LICENSE                                      //
//  Nothing can prevent you to use these materials in any of your purposes   //
//     in case of mention of the author and his logo in your projects.       //
//////////////////////////////////////////////////////////////////////////// */
