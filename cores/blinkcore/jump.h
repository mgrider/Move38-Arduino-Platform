/*
 * jump.h
 *
 * Clear stack and jump. Used for passing control between games and bootloader
 * without accumulating cruft on the stack each time.
 *
 */

#ifndef JUMP_H_
#define  JUMP_H_

/*

    Ok, I know that this is ugly, but I tried so many other ways and they did not work.

    Note that this code disables interrupts before fiddling with the stack registers, or else
    an interrupt could come in the middle of the update and end up clobbering random loctions
    in SRAM.

    As is, it works perfectly so a call to `CLEAR_STACK_JMP( "0x3800" );` compiles out to ...


     392a:	04 e0       	ldi	r16, 0x04	; 4
     392c:	0e bf       	out	0x3e, r16	; 62
     392e:	0f ef       	ldi	r16, 0xFF	; 255
     3930:	0d bf       	out	0x3d, r16	; 61
     3932:	0c 94 00 1c 	jmp	0x3800	; 0x3800 <__vectors>

   .. which results in the target getting jumped to with an empty stack.

   BE CAREFUL:

   1. Don't let any whitespace after the backslashes!
   2. Clobbers R16
   3. cli() before calling
   
   TODO: Pick a better reg than r16. Maybe use temp_reg_? 

*/

#define CLEAR_STACK() {             \
    __asm__ __volatile__            \
    (                               \
    "ldi 16 , %0    \n"             \
    "out __SP_H__, 16    \n"        \
    "ldi 16 , %1    \n"             \
    "out __SP_L__, 16    \n"        \
    :                               \
    : "M" ( RAMEND >> 8 ), "M" ( RAMEND & 0xff )  \
    :                               \
    );                              \
}


// Clear the stack and jump to address. Address specified as string!
// cli() first. 


#define CLEAR_STACK_JMP( x ) { CLEAR_STACK(); asm( "jmp " x ); }


#endif 