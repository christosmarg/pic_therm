#ifndef _EXTERN_H_
#define _EXTERN_H_

#define NO_BIT_DEFINES
#include <pic14regs.h>
#include <stdint.h>

/* MCU related constants */
#define _XTAL_FREQ 16000000	/* Crystal oscillator running at 16MHz */

#define BTN_PORT PORTBbits.RB0
#define BTN_TRIS TRISBbits.TRISB0
#define BTN_DEBOUNCE_TIME_MS 20

#define LED_PORT PORTBbits.RB1
#define LED_TRIS TRISBbits.TRISB1

#define OUTPUT 0
#define INPUT 1

/* Non MCU related constants */
#define NULL ((void *)0)
#define BUFSIZ (3 * sizeof(uint32_t))

#endif /* _EXTERN_H_ */
