#ifndef _I2C_H_
#define _I2C_H_

#include <stdint.h>

#define I2C_MASTER 0x08
#define I2C_SLEW_OFF 0x80
#define I2C_SLEW_ON 0x00
#define I2C_CLK_1MHZ 1000000

void	i2c_init(uint8_t, uint8_t, uint32_t);
void	i2c_hold(void);
void	i2c_start(void);
void	i2c_stop(void);
void	i2c_restart(void);
void	i2c_write(uint8_t);
uint8_t	i2c_read(uint8_t);

#endif /* _I2C_H_ */
