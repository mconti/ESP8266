#include <ESP8266WiFi.h>

extern "C" {
#include "gpio.h" 
}

void setup() {
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
  //PIN_PULLUP_DIS(PERIPHS_IO_MUX_GPIO2_U);
  PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO2_U);
}

void loop() {
  // put your main code here, to run repeatedly:
  //GPIO_OUTPUT_SET(1, 0);
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH
  gpio_output_set(BIT2, 0, BIT2, 0);  // HIGH

  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
  gpio_output_set(0, BIT2, BIT2, 0);  // LOW
}
