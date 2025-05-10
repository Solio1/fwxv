/*
  Project Code for FW 103

  Assignment: Create an ADC driver to interface with the ADS1115 Multi-Channel ADC IC.

  Requirements:
    - Implement the ADC driver functions (set config, select and read from a channel)
    - ADC task to periodically measure the voltage of channel 0
    - Overvoltage interrupt (configure the interrupt to be on channel 0 with thresholds of 0V - 1V)
*/

#include <stdio.h>
#include "log.h"
#include "tasks.h"
#include "delay.h"
#include "gpio.h"  
#include "ads1115.h"

// Define GPIO for LED
GpioAddress led_addr = {
  .port = GPIO_PORT_B,
  .pin = 3,
};

// Define GPIO for ADS1115 "ready" pin
GpioAddress ready_pin = {
  .port = GPIO_PORT_B,
  .pin = 0,  
};

// LED toggle task
TASK(run_leds, TASK_STACK_512) {
  gpio_init_pin(&led_addr, GPIO_OUTPUT_PUSH_PULL, GPIO_STATE_HIGH);  // Configure LED pin as output

  ADS1115_Config config = {
    .i2c_addr = ADS1115_ADDR_GND,
    .i2c_port = ADS1115_I2C_PORT,
    .ready_pin = &ready_pin,
    .handler_task = run_leds, 
  };


  while (true) {
    gpio_toggle_state(&led_addr);
    delay_ms(1000);
  }
}

int main(void) {
  gpio_init();     
  log_init();      
  tasks_init();    

  tasks_init_task(run_leds, TASK_PRIORITY(1), NULL);

  tasks_start();   // Start the FreeRTOS-style scheduler

  return 0;
}