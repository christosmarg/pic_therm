#include "bme280.h"
#include "i2c.h"
#include "tmr0.h"

#define MODE_WRITE 0
#define MODE_READ 1

#define BME280_DELAY		100

#define BME280_SAMPLE_SKIP	0x00
#define BME280_SAMPLE_X1	0x01
#define BME280_FILTER_OFF	0x00
#define BME280_STANDBY_0_5	0x00
#define BME280_MODE_NORMAL	0x03
#define BME280_RESET_POWERON	0xb6

#define BME280_ADDR		0xee
#define BME280_CHIPID		0x60

#define BME280_REG_DIG_T1	0x88
#define BME280_REG_DIG_T2	0x8a
#define BME280_REG_DIG_T3	0x8c

#define BME280_REG_DIG_H1	0xa1
#define BME280_REG_DIG_H2	0xe1
#define BME280_REG_DIG_H3	0xe3
#define BME280_REG_DIG_H4	0xe4
#define BME280_REG_DIG_H5	0xe5
#define BME280_REG_DIG_H6	0xe7

#define BME280_REG_CHIPID	0xd0
#define BME280_REG_SOFTRESET	0xe0

#define BME280_REG_CTLHUM	0xf2
#define BME280_REG_STATUS	0xf3
#define BME280_REG_CTL		0xf4
#define BME280_REG_CONFIG	0xf5
#define BME280_REG_PRESS_MSB	0xf7

struct bme280_calib {
	uint16_t	dig_t1;
	int16_t		dig_t2;
	int16_t 	dig_t3;
	/* Pressure fields are not used to preserve space. */
	uint8_t 	dig_h1;
	int16_t 	dig_h2;
	uint8_t 	dig_h3;
	int16_t 	dig_h4;
	int16_t 	dig_h5;
	int8_t  	dig_h6;
};

static void	bme280_write(uint8_t, uint8_t);
static uint8_t	bme280_read8(uint8_t);
static uint16_t	bme280_read16(uint8_t);
static void	bme280_update(void);

static struct bme280_calib cal;
static int32_t adc_t, adc_h, t_fine;

static void
bme280_write(uint8_t reg, uint8_t dat)
{
	i2c_start();
	i2c_write(BME280_ADDR | MODE_WRITE);
	i2c_write(reg);
	i2c_write(dat);
	i2c_stop();
}

static uint8_t
bme280_read8(uint8_t reg)
{
	uint8_t c;

	i2c_start();
	i2c_write(BME280_ADDR | MODE_WRITE);
	i2c_write(reg);
	i2c_restart();
	i2c_write(BME280_ADDR | MODE_READ);
	c = i2c_read(0);
	i2c_stop();
	return (c);
}

static uint16_t
bme280_read16(uint8_t reg)
{
	uint16_t c;

	i2c_start();
	i2c_write(BME280_ADDR | MODE_WRITE);
	i2c_write(reg);
	i2c_restart();
	i2c_write(BME280_ADDR | MODE_READ);
	c = i2c_read(1);
	c |= i2c_read(0) << 8;
	i2c_stop();
	return (c);
}

static void
bme280_update(void)
{
	union {
		uint8_t a[4];
		uint32_t v;
	} r;

	i2c_start();
	i2c_write(BME280_ADDR | MODE_WRITE);
	i2c_write(BME280_REG_PRESS_MSB);
	i2c_restart();
	i2c_write(BME280_ADDR | MODE_READ);

	/* Ignore pressure */
	(void)i2c_read(1);
	(void)i2c_read(1);
	(void)i2c_read(1);

	r.a[3] = 0x00;
	r.a[2] = i2c_read(1);
	r.a[1] = i2c_read(1);
	r.a[0] = i2c_read(1);
	adc_t = (r.v >> 4) & 0xfffff;

	r.a[2] = 0x00;
	r.a[1] = i2c_read(1);
	r.a[0] = i2c_read(0);
	adc_h = r.v & 0xffff;

	i2c_stop();
}

int
bme280_init(void)
{
	if (bme280_read8(BME280_REG_CHIPID) != BME280_CHIPID)
		return (-1);
	bme280_write(BME280_REG_SOFTRESET, BME280_RESET_POWERON);
	tmr0_delay_ms(BME280_DELAY);
	while ((bme280_read8(BME280_REG_STATUS) & 0x01) == 0x01)
		tmr0_delay_ms(BME280_DELAY);

	cal.dig_t1 = bme280_read16(BME280_REG_DIG_T1);
	cal.dig_t2 = bme280_read16(BME280_REG_DIG_T2);
	cal.dig_t3 = bme280_read16(BME280_REG_DIG_T3);

	cal.dig_h1 = bme280_read8(BME280_REG_DIG_H1);
	cal.dig_h2 = bme280_read16(BME280_REG_DIG_H2);
	cal.dig_h3 = bme280_read8(BME280_REG_DIG_H3);
	cal.dig_h4 = ((uint16_t)bme280_read8(BME280_REG_DIG_H4) << 4) |
	    (bme280_read8(BME280_REG_DIG_H4 + 1) & 0x0f);
	if (cal.dig_h4 & 0x0800)	/* H4 < 0 */
		cal.dig_h4 |= 0xf000;
	cal.dig_h5 = ((uint16_t)bme280_read8(BME280_REG_DIG_H5 + 1) << 4) |
	    (bme280_read8(BME280_REG_DIG_H5) >> 4);
	if (cal.dig_h5 & 0x0800)	/* H5 < 0 */
		cal.dig_h5 |= 0xf000;
	cal.dig_h6 = bme280_read8(BME280_REG_DIG_H6);

	bme280_write(BME280_REG_CTLHUM, BME280_SAMPLE_X1);
	bme280_write(BME280_REG_CONFIG, ((BME280_STANDBY_0_5 << 5) |
	    (BME280_FILTER_OFF << 2)) & 0xfc);
	bme280_write(BME280_REG_CTL, ((BME280_SAMPLE_X1 << 5) |
	    (BME280_SAMPLE_SKIP << 2)) | BME280_MODE_NORMAL);

	tmr0_delay_ms(BME280_DELAY);

	return (0);
}

/* Black magic taken (stolen) from BME280's datasheet. */
int32_t
bme280_read_temp(void)
{
	int32_t v1, v2;

	bme280_update();
	v1 = ((((adc_t >> 3) - ((int32_t)cal.dig_t1 << 1))) *
	    ((int32_t)cal.dig_t2)) >> 11;
	v2 = (((((adc_t >> 4) - ((int32_t)cal.dig_t1)) *
	    ((adc_t >> 4) - ((int32_t)cal.dig_t1))) >> 12) *
	    ((int32_t)cal.dig_t3)) >> 14;

	t_fine = v1 + v2;

	return ((t_fine * 5 + 128) / 256);
}

uint32_t
bme280_read_humid(void)
{
	int32_t v = 0;

	bme280_update();
	v = (t_fine - ((int32_t)76800));
 
	v = (((((adc_h << 14) - (((int32_t)cal.dig_h4) << 20) -
	    (((int32_t)cal.dig_h5) * v)) + ((int32_t)16384)) >> 15) *
	    (((((((v * ((int32_t)cal.dig_h6)) >> 10) *
	    (((v * ((int32_t)cal.dig_h3)) >> 11) + ((int32_t)32768))) >> 10) +
	    ((int32_t)2097152)) * ((int32_t)cal.dig_h2) + 8192) >> 14));

	v = (v - (((((v >> 15) * (v >> 15)) >> 7) * ((int32_t)cal.dig_h1)) >> 4));
	v = (v < 0 ? 0 : v);
	v = (v > 419430400 ? 419430400 : v);
   
	return ((uint32_t)(v >> 12));
}
