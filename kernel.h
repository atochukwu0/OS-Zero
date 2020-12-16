/*/////////////////////////////////////////////////////////////////////////////
//                             OS Zero v0.0                                  //
//      Simplest cooperative multitasking kernel intended to smallest        //
//      8bit microcontrollers. Implemented by STVD and CXSTM8 compiler       //
;/             (C) Simplest System Solutions (canceled)                      //
//	is a personal logo of independent developer Vyacheslav Azarov        //
//      License bellow. Email: slavaza63@gmail.com  Skipe: slavaza63         //
//////////////////////////////////////////////////////////////////////////// */

#ifndef _KERNEL_H_
#define _KERNEL_H_

#include <stdbool.h>
#include <stddef.h>

/*/////////////////////////////////////////////////////////////////////////////
//     Definitions what depend on application reqirements                    //
//////////////////////////////////////////////////////////////////////////// */

#include <iostm8l152x.h>

#define PENDING_QUEUE_LIMIT	4

#define __reset() // You simple code to integrity saving before restart

/*/////////////////////////////////////////////////////////////////////////////
//     Thread control functions what implement horizontal iterations         //
//////////////////////////////////////////////////////////////////////////// */

void yield(void);

#define await(condition) while (!(condition)) yield()

volatile extern void * self;
bool spawn(void  * stack, void (* start)(void));
bool alive(void * stack);
void die(void);


/*/////////////////////////////////////////////////////////////////////////////
//        System timebase functions and countdown timers                     //
//////////////////////////////////////////////////////////////////////////// */

extern volatile unsigned int msec;
unsigned long millis(void);
void delay(unsigned long ms);

/*/////////////////////////////////////////////////////////////////////////////
//               Global system control of functionflity                      //
//////////////////////////////////////////////////////////////////////////// */

void start(void);
void reset(void);

/*_KERNEL_H_*/
#endif 
/*/////////////////////////////////////////////////////////////////////////////
//                              LICENSE                                      //
//  Nothing can prevent you to use these materials in any of your purposes   //
//     in case of mention of the author and his logo in your projects.       //
//////////////////////////////////////////////////////////////////////////// */
