;*/////////////////////////////////////////////////////////////////////////////
;/                             OS Zero v0.0                                  //
;/      Simplest cooperative multitasking kernel intended to smallest        //
;/      8bit microcontrollers. Implemented by STVD and CXSTM8 compiler       //
;/             (C) Simplest System Solutions (canceled)                      //
;/	is a personal logo of independent developer Vyacheslav Azarov        //
;/      License bellow. Email: slavaza63@gmail.com  Skipe: slavaza63         //
;/////////////////////////////////////////////////////////////////////////// */

		xref	_yield
		xdef	_self
		xdef	_pending_point
		xdef	_pending_limit
		xdef	_pending_queue
;		
		switch .text
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
;
		end

;*/////////////////////////////////////////////////////////////////////////////
;/                              LICENSE                                      //
;/  Nothing can prevent you to use these materials in any of your purposes   //
;/     in case of mention of the author and his logo in your projects.       //
;/////////////////////////////////////////////////////////////////////////// */
