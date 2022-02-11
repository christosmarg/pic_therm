#ifndef _BME280_H_
#define _BME280_H_

#include <stdint.h>

int		bme280_init(void);
int32_t		bme280_read_temp(void);
uint32_t	bme280_read_humid(void);

#endif /* _BME280_H_ */
