/*
 * PCA9685_driver.c
 *
 *  Created on: Jun 23, 2026
 *      Author: mateusz.marszalek
 */

#include <PCA9685_driver.h>
pca9685_status_t PCA9685_set_sleep_mode(PCA9685_conf_t* cfg)
{
	if(!cfg || !cfg->write || !cfg->read )
	{
		return PCA9685_INV_CONF;
	}
	uint8_t mode = 0;
	PCA9685_ASSERT(cfg->read(cfg->i2c_address, MODE1, &mode, 1));

	mode = (mode & (~MODE1_RESTART)) | MODE1_SLEEP; // Don't touch the restart bit
	return cfg->write(cfg->i2c_address, MODE1, &mode, 1);
}

pca9685_status_t PCA9685_wake_up(PCA9685_conf_t* cfg)
{
	if(!cfg || !cfg->write || !cfg->read || !cfg->delay )
	{
		return PCA9685_INV_CONF;
	}
	uint8_t mode = 0;
	PCA9685_ASSERT(cfg->read(cfg->i2c_address, MODE1, &mode, 1));
	if(mode & MODE1_RESTART)
	{
		mode = (mode & (~(MODE1_RESTART | MODE1_SLEEP))) | MODE1_AI;
		PCA9685_ASSERT(cfg->write(cfg->i2c_address, MODE1, &mode, 1));

		cfg->delay(1);

		mode = mode | MODE1_RESTART;
		PCA9685_ASSERT(cfg->write(cfg->i2c_address, MODE1, &mode, 1));
	}
	else
	{
		mode = (mode & (~MODE1_SLEEP)) | MODE1_AI;
		PCA9685_ASSERT(cfg->write(cfg->i2c_address, MODE1, &mode, 1));
		cfg->delay(1);
	}
	return PCA9685_OK;
}

pca9685_status_t PCA9685_set_frequency(PCA9685_conf_t* cfg, uint16_t freq)
{
	if(!cfg || !cfg->write || !cfg->read || !cfg->delay )
	{
		return PCA9685_INV_CONF;
	}

	float prescaler_fl = (PCA9685_CLOCK / (PCA9685_RESOLUTION * freq)) - 1.0;
	uint8_t prescaler_int = (uint8_t)prescaler_fl;
	if(prescaler_int > PRESCALER_MAX || prescaler_int < PRESCALER_MIN) {return PCA9685_INV_ARG;}

	PCA9685_ASSERT(PCA9685_set_sleep_mode(cfg))
	PCA9685_ASSERT(cfg->write(cfg->i2c_address, PRESCALER_REG, &prescaler_int, 1))

	return PCA9685_wake_up(cfg);
}

pca9685_status_t PCA9685_set_oe(PCA9685_conf_t* cfg, pca9685_oe_pin_state_t state)
{
	if(!cfg || !cfg->set_oe )
	{
		return PCA9685_INV_CONF;
	}
	cfg->set_oe(state);
	return PCA9685_OK;

}

pca9685_status_t PCA9685_init(PCA9685_conf_t* cfg, uint16_t freq)
{
	if(!cfg || !cfg->write || !cfg->read || !cfg->delay || !cfg->sw_reset)
	{
		return PCA9685_INV_CONF;
	}
	PCA9685_ASSERT(cfg->sw_reset());
	cfg->delay(1);
	return PCA9685_set_frequency(cfg, freq);
}

pca9685_status_t PCA9685_set_duty_cycle(PCA9685_conf_t* cfg, uint8_t channel, uint16_t duty_cycle)
{
	if(!cfg || !cfg->write)
	{
		return PCA9685_INV_CONF;
	}
	if (channel > 15 || duty_cycle > 4095)
	{
		return PCA9685_INV_ARG;
	}

	uint8_t led_reg_b_addr = GET_LED_REG(channel);
	uint8_t buffer[4];

	buffer[0] = 0x00;
	buffer[1] = 0x00;
	buffer[2] = (uint8_t)(duty_cycle & 0xFF);
	buffer[3] = (uint8_t) (duty_cycle >> 8) & 0x0F;

	return cfg->write(cfg->i2c_address, led_reg_b_addr, buffer, 4);
 }

