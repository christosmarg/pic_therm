#include "extern.h"
#include "lcd.h"
#include "tmr0.h"

#define LCD_RS PORTCbits.RC0	/* Instruction reg: 0, Data reg: 1 */
#define LCD_RW PORTCbits.RC1	/* Write: 0, Read: 1 */
#define LCD_EN PORTCbits.RC2	/* Enable read/write */
#define LCD_RS_TRIS TRISCbits.TRISC0
#define LCD_RW_TRIS TRISCbits.TRISC1
#define LCD_EN_TRIS TRISCbits.TRISC2
#define LCD_PORT_DATA PORTD
#define LCD_TRIS_DATA TRISD

void
lcd_init(void)
{
	LCD_RS_TRIS = OUTPUT;
	LCD_RW_TRIS = OUTPUT;
	LCD_EN_TRIS = OUTPUT;
	LCD_PORT_DATA = OUTPUT;
	LCD_TRIS_DATA = OUTPUT;
	LCD_RS = 0;
	LCD_RW = 0;
	LCD_EN = 0;
	
	lcd_cmd(LCD_MODE);
	tmr0_delay_ms(LCD_DELAY_STARTUP);
	lcd_cmd(LCD_CURS_ROW1);
	lcd_cmd(LCD_CURS_OFF);
	lcd_cmd(LCD_CURS_INC);
	lcd_cmd(LCD_CLEAR);
}

void
lcd_data(uint8_t c, uint8_t rs)
{
	LCD_RS = rs;
	LCD_RW = 0;
	LCD_PORT_DATA = c;
	LCD_EN = 1;
	tmr0_delay_ms(LCD_DELAY_CMD);
	LCD_EN = 0;
}

void
lcd_puts(const char *str)
{
	while (*str != '\0')
		lcd_putc(*str++);
}
