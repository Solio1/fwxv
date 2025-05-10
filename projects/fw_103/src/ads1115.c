#include "ads1115.h"

#include "gpio_it.h"
#include "i2c.h"
#include "status.h"

StatusCode ads1115_init(ADS1115_Config *config, ADS1115_Address i2c_addr, GpioAddress *ready_pin) {
  if (config == NULL) {
    return status_code(STATUS_CODE_INVALID_ARGS);
  }

  config->i2c_addr = i2c_addr;

  uint16_t cmd;

  // Write Config register
  /* TODO: fill out this value */
  cmd = 0x8483;
  i2c_write_reg(config->i2c_port, i2c_addr, ADS1115_REG_CONFIG, (uint8_t *)(&cmd), 2);

  /* TODO (optional) */
  // Set low thresh to zero
  cmd = 0x0000;
  i2c_write_reg(config->i2c_port, i2c_addr, ADS1115_REG_LO_THRESH, (uint8_t *)(&cmd), 2);

  /* TODO (optional) */
  // Set high thresh to 1V
  cmd = 0x0000;
  i2c_write_reg(config->i2c_port, i2c_addr, ADS1115_REG_HI_THRESH, (uint8_t *)(&cmd), 2);

  // Register the ALRT pini 
  /* TODO (optional) */

  return STATUS_CODE_OK;
}

StatusCode ads1115_select_channel(ADS1115_Config *config, ADS1115_Channel channel) {
  if (config == NULL) {
    return status_code(STATUS_CODE_INVALID_ARGS);
  }

  uint16_t cmd;

  // Write Config register
  cmd = 0x8283;
  i2c_write_reg(config->i2c_port, config->i2c_addr, ADS1115_REG_CONFIG, (uint8_t *)(&cmd), 2);
  return STATUS_CODE_OK;
}

StatusCode ads1115_read_raw(ADS1115_Config *config, ADS1115_Channel channel, uint16_t *reading) {
  if (config == NULL || reading == NULL) {
    return status_code(STATUS_CODE_INVALID_ARGS);
  }

  // Select the channel (starts a conversion)
  StatusCode status = ads1115_select_channel(config, channel);
  if (!status_ok(status)) {
    return status;
  }


  // Read 2 bytes from the conversion register
  uint8_t buffer[2];
  status = i2c_read_reg(config->i2c_port, config->i2c_addr, ADS1115_REG_CONVERSION, buffer, 2);
  if (!status_ok(status)) {
    return status;
  }

  // Reassemble big-endian result
  *reading = ((uint16_t)buffer[0] << 8) | buffer[1];

  return STATUS_CODE_OK;
}

StatusCode ads1115_read_converted(ADS1115_Config *config, ADS1115_Channel channel, float *reading) {
  if (config == NULL || reading == NULL) {
    return status_code(STATUS_CODE_INVALID_ARGS);
  }

  uint16_t raw_reading = 0;
  StatusCode status = ads1115_read_raw(config, channel, &raw_reading);
  if (!status_ok(status)) {
    return status;
  }

  // Cast raw_reading to int16_t to handle negative values correctly
  int16_t signed_reading = (int16_t)raw_reading;

  // ADS1115 full-scale range is ±2.048V, which maps to ±32768
  *reading = ((float)signed_reading / 32768.0f) * 2.048f;

  return STATUS_CODE_OK;
}
