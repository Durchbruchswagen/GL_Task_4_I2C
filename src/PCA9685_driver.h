/*
 * PCA9685_driver.h
 *
 *  Created on: Jun 23, 2026
 *      Author: mateusz.marszalek
 */

#ifndef INC_PCA9685_DRIVER_H_
#define INC_PCA9685_DRIVER_H_

#include <stdint.h>

#define MODE1 0x00
#define MODE1_AI 0x20
#define MODE1_SLEEP 0x10
#define MODE1_RESTART 0x80

#define LED0_ON_L 0x06
#define GET_LED_REG(num) LED0_ON_L + (4*(num))

#define SW_RESET_ADDR 0
#define SW_DATA_BYTE 0x06

#define PRESCALER_REG 0xFE
#define PRESCALER_MIN 0x03
#define PRESCALER_MAX 0xFF

#define PCA9685_I2C_TIMEOUT 100
#define PCA9685_CLOCK 25000000.0
#define PCA9685_RESOLUTION 4096.0

typedef enum
{
	PCA9685_OE_ENABLE = 0,
	PCA9685_OE_DISABLE = 1,
}pca9685_oe_pin_state_t;

typedef enum
{
	PCA9685_OK = 0, //ok
	PCA9685_ERROR = -1, // general error
	PCA9685_INV_ARG = -2, // invalid argument for one of the function
	PCA9685_INV_CONF = -3, // PCA9685_conf is wrong (null pointer)
}pca9685_status_t;

#define PCA9685_ASSERT(status) if ((status != PCA9685_OK)) {return status;}

typedef pca9685_status_t (*pca9685_sw_reset)(void);
typedef pca9685_status_t (*pca9685_write_i2c)(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len);
typedef pca9685_status_t (*pca9685_read_i2c)(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len);
typedef void (*pca9685_set_oe_state)(pca9685_oe_pin_state_t state);
typedef void (*pca9685_delay)(uint32_t ms);

typedef struct
{
	pca9685_sw_reset sw_reset;
	pca9685_write_i2c write;
	pca9685_read_i2c read;
	pca9685_delay delay;
	pca9685_set_oe_state set_oe;
	uint8_t i2c_address;

} PCA9685_conf_t;

pca9685_status_t PCA9685_set_sleep_mode(PCA9685_conf_t* cfg);
pca9685_status_t PCA9685_wake_up(PCA9685_conf_t* cfg);
pca9685_status_t PCA9685_set_frequency(PCA9685_conf_t* cfg, uint16_t freq);
pca9685_status_t PCA9685_set_oe(PCA9685_conf_t* cfg, pca9685_oe_pin_state_t state);
pca9685_status_t PCA9685_init(PCA9685_conf_t* cfg, uint16_t freq);
pca9685_status_t PCA9685_set_duty_cycle(PCA9685_conf_t* cfg, uint8_t channel, uint16_t duty_cycle);

#endif /* INC_PCA9685_DRIVER_H_ */
