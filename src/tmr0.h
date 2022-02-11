#ifndef _TMR0_H_
#define _TMR0_H_

#include "extern.h"

#define TMR0_CLK0 (0 << 5)		/* Use the CLK0 pin */
/*
 * Bit 3 assigns the prescalar to TMR0.
 * Bits 0-2 of OPTION_REG indicate the prescalar value. In this case,
 * 111 indicates a prescalar value of 256.
 */
#define TMR0_PRESCALAR_256 0b0111
/* 
 * We want 1ms delay @ 16Mhz with 64 prescalar, the formula is:
 *
 * 256 - ((delay * (fosc / 1000)) / (prescalar * 4)) =
 * 256 - ((1 * (16000000 / 1000)) / (256 * 4)) =
 * 256 - ((1 * 16000) / (256 * 4)) = ~240
 *
 * sdcc thinks there's an overflow here (hint: he's wrong)...
 */
#define TMR0_DELAY (256 - ((1 * (_XTAL_FREQ / 1000)) / (256 * 4)))

typedef void (*ev_handler)(void);

void	tmr0_init(void);
void	tmr0_delay_ms(uint16_t);
int	tmr0_set_event(ev_handler, uint16_t);

#endif /* _TMR0_H_ */
