#include "tmr0.h"
#include "util.h"

#define TMR0_REQ_MAX 2

struct timer_req {
	ev_handler handler;	/* ISR callback */
	uint16_t rate;		/* Interval */
	uint16_t cnt;		/* Current time */
};

static void tmr0_isr(void) __interrupt;

static struct timer_req	reqs[TMR0_REQ_MAX];

static void
tmr0_isr(void) __interrupt
{
	struct timer_req *r;
	uint8_t i;

	if (INTCONbits.TMR0IF != 1)
		return;
	for (i = 0; i < TMR0_REQ_MAX; i++) {
		r = &reqs[i];
		if (r->handler == NULL)
			continue;
		if (++r->cnt == r->rate) {
			r->handler();
			r->cnt = 0;
		}
	}
	TMR0 = TMR0_DELAY;
	INTCONbits.TMR0IF = 0; /* Clear interrupt flags */
}

void
tmr0_init(void)
{
	memset(reqs, 0, sizeof(reqs));
	OPTION_REG = 0;
	OPTION_REG |= TMR0_CLK0 | TMR0_PRESCALAR_256;
	TMR0 = TMR0_DELAY;
	INTCONbits.TMR0IE = 1;	/* TMR0 Interrupt Enable */
	INTCONbits.GIE = 1;	/* Global Interrupt Enable */
	INTCONbits.PEIE = 1;	/* Peripheral Interrupt Enable */
}

void
tmr0_delay_ms(uint16_t t)
{
	while (t--) {
		while (INTCONbits.TMR0IF == 0)
			;	/* nothing */
		INTCONbits.TMR0IF = 0;
		TMR0 = TMR0_DELAY;
	}
}

int
tmr0_set_event(ev_handler handler, uint16_t rate)
{
	struct timer_req *r;
	uint8_t i;

	for (i = 0; i < TMR0_REQ_MAX; i++) {
		r = &reqs[i];
		if (r->handler != NULL)
			continue;
		r->handler = handler;
		r->rate = rate;
		r->cnt = 0;
		return (1);
	}
	return (0);
}
