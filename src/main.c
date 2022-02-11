#include "extern.h"
#include "bme280.h"
#include "i2c.h"
#include "lcd.h"
#include "tmr0.h"
#include "util.h"

static __code uint16_t __at (_CONFIG) __configword =
    _FOSC_HS & _WDTE_OFF & _PWRTE_ON & _LVP_OFF & _WRT_OFF & _BOREN_ON &
    _CPD_OFF & _CP_OFF;

/*
 * TODO: turn off unneeded modules (e.g tmr1..) to minimize consumption.
 */

static void	ctx_main(void);
static void	ctx_uptime_maxtp(void);
static void	print_tp(int32_t);
static void	led_blink(void);
static void	button_debounce(void);

static uint32_t	timecnt = 0;	/* Seconds passed since start */
static int	f_ctx = 0;	/* Change context */
static uint32_t	humid;		/* Current humidity */
static int32_t	tp;		/* Current temperature */
static int32_t	maxtp = -99999;	/* Max temperature */
static char	buf[BUFSIZ+1] = {0}; /* Generic buffer */

#define LCD_PUTS_INT(buf, v) do {		\
	memset(buf, 0, sizeof(buf));		\
	lcd_puts(itoa(&buf[sizeof(buf)-1], v));	\
} while (0)

static void
ctx_main(void)
{
	/*lcd_cmd(LCD_CURS_ROW1);*/
	/*lcd_puts("ID: 19390133");*/

	lcd_cmd(LCD_CURS_ROW1);
	lcd_puts("Temp: ");
	print_tp(tp);

	lcd_cmd(LCD_CURS_ROW2);
	lcd_puts("Humid: ");
	LCD_PUTS_INT(buf, humid / 1024);
	lcd_putc('.');
	LCD_PUTS_INT(buf, ((humid * 100) / 1024) % 100);
	lcd_putc('%');
}

static void
ctx_uptime_maxtp(void)
{
	lcd_cmd(LCD_CURS_ROW1);
	lcd_puts("T: ");
	LCD_PUTS_INT(buf, timecnt);

	lcd_cmd(LCD_CURS_ROW2);
	lcd_puts("Max: ");
	print_tp(maxtp);
}

static void
print_tp(int32_t tp)
{
	if (tp < 0) {
		tp = -tp;
		lcd_putc('-');
	}
	LCD_PUTS_INT(buf, tp / 100);
	lcd_putc('.');
	LCD_PUTS_INT(buf, tp % 100);
	lcd_puts("\337C   ");
}

#undef LCD_PUTS_INT

static void
led_blink(void)
{
	LED_PORT ^= 1;
	/*
	 * Increment here since this function is called every 1 sec.
	 * No need to create another timer callback.
	 */
	timecnt++;
}

static void
button_debounce(void)
{
	static uint8_t cnt = 0;

	/* Button is pressed */
	if (BTN_PORT == 0) {
		if (cnt == 0) {
			/* Actual button functionality goes here. */
			f_ctx = 1;
			cnt++;
		}
		cnt = BTN_DEBOUNCE_TIME_MS;
	} else if (cnt != 0) {
		f_ctx = 0;
		cnt--;
	}
}

void
main(void)
{
	tmr0_init();
	tmr0_set_event(&led_blink, 1000);
	tmr0_set_event(&button_debounce, 1);
	lcd_init();
	i2c_init(I2C_MASTER, I2C_SLEW_OFF, I2C_CLK_1MHZ);
	if (bme280_init() < 0) {
		lcd_puts("BME280 error");
		for (;;); /* Hang */
	}

	BTN_TRIS = INPUT;
	LED_PORT = 1; /* LED on */
	LED_TRIS = OUTPUT;

	for (;;) {
		if ((tp = bme280_read_temp()) > maxtp)
			maxtp = tp;
		humid = bme280_read_humid();
		lcd_cmd(LCD_CLEAR);
		if (!f_ctx)
			ctx_main();
		else
			ctx_uptime_maxtp();
		tmr0_delay_ms(1000);
	}
}
