#ifndef _LCD_H_
#define _LCD_H_

#include <stdint.h>

#define LCD_DELAY_STARTUP 15
#define LCD_DELAY_CMD 2
#define LCD_SEL_INST 0		/* Instruction register for RS */
#define LCD_SEL_DATA 1		/* Data register for RS */
#define LCD_CLEAR 0x01		/* Clear screen */
#define LCD_CURS_ROW1 0x02	/* Set cursor on (0, 0) */
#define LCD_CURS_ROW2 0xc0	/* Set cursor on (1, 0) */
#define LCD_CURS_INC 0x06	/* Auto-increment cursor */
#define LCD_CURS_OFF 0x0c	/* Hide cursor */
#define LCD_MODE 0x38		/* 8-bit, 16x2 */
//TODO: backlight

/* Function macros */
#define lcd_putc(c) lcd_data(c, LCD_SEL_DATA)
#define lcd_cmd(cmd) lcd_data(cmd, LCD_SEL_INST)

void	lcd_init(void);
void	lcd_data(uint8_t, uint8_t);
void	lcd_puts(const char *);

#endif /* _LCD_H_ */
