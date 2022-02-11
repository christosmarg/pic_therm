#include "extern.h"
#include "i2c.h"

#define I2C_SCL TRISCbits.TRISC3
#define I2C_SDA TRISCbits.TRISC4

void
i2c_init(uint8_t mode, uint8_t slew, uint32_t freq)
{
	I2C_SCL = INPUT;
	I2C_SDA = INPUT;
	SSPCON = mode | 0x20; /* MCU as master */
	SSPCON2 = 0;
	SSPADD = (_XTAL_FREQ / (4 * freq)) - 1; /* Bus clock speed */
	SSPSTAT &= 0x3f;
	SSPSTAT = slew;
}

void
i2c_hold(void)
{
	while ((SSPSTATbits.R_W) || (SSPCON2 & 0x1f))
		;	/* nothing */
}

void
i2c_start(void)
{
	i2c_hold();
	SSPCON2bits.SEN = 1;
}

void
i2c_stop(void)
{
	i2c_hold();
	SSPCON2bits.PEN = 1;
}

void
i2c_restart(void)
{
	i2c_hold();
	SSPCON2bits.RSEN = 1;
}

void
i2c_write(uint8_t c)
{
	SSPBUF = c;
	i2c_hold();
}

uint8_t
i2c_read(uint8_t ack)
{
	uint8_t c;

	SSPCON2bits.RCEN = 1;
	while (SSPSTATbits.BF == 0)
		;	/* nothing */
	c = SSPBUF;
	i2c_hold();
	SSPCON2bits.ACKDT = ack ? 0 : 1;
	SSPCON2bits.ACKEN = 1;
	return (c);
}
