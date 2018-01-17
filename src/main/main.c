/* Created 19 Nov 2016 by Chris Osborn <fozztexx@fozztexx.com>
 * http://insentricity.com
 *
 * Demo of driving WS2812 RGB LEDs using the RMT peripheral.
 *
 * This code is placed in the public domain (or CC0 licensed, at your option).
 */

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <soc/rmt_struct.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <driver/gpio.h>
#include <stdio.h>
#include "ws2812.h"

#define WS2812_PIN	13

#define PIXEL_COUNT 24

#define delay_ms(ms) vTaskDelay((ms) / portTICK_RATE_MS)

void rotate(void *params)
{
  rgbVal *pixels = malloc(sizeof(rgbVal) * PIXEL_COUNT);
  rgbVal black = makeRGBVal(0,0,0);
  rgbVal color = makeRGBVal(200,50,0);
  uint32_t cnt = 0;
  while(1){
    int current = cnt % PIXEL_COUNT;
    for(int i=0; i<PIXEL_COUNT; i++) {
      if(i == current)
        pixels[i] = color;
      else
        pixels[i] = black;
    }
    cnt++;
    ws2812_setColors(PIXEL_COUNT, pixels);
    delay_ms(50);
  }
}

void rainbow(void *pvParameters)
{
  const uint8_t anim_step = 10;
  const uint8_t anim_max = 250;
  const uint8_t pixel_count = 24; // Number of your "pixels"
  const uint8_t delay = 25; // duration between color changes
  rgbVal color = makeRGBVal(anim_max, 0, 0);
  uint8_t step = 0;
  rgbVal color2 = makeRGBVal(anim_max, 0, 0);
  uint8_t step2 = 0;
  rgbVal *pixels;


  pixels = malloc(sizeof(rgbVal) * pixel_count);

  while (1) {

    printf("running...\n");

    color = color2;
    step = step2;

    for (uint8_t i = 0; i < pixel_count; i++) {
      pixels[i] = color;

      if (i == 1) {
        color2 = color;
        step2 = step;
      }

      switch (step) {
      case 0:
        color.g += anim_step;
        if (color.g >= anim_max)
          step++;
        break;
      case 1:
        color.r -= anim_step;
        if (color.r == 0)
          step++;
        break;
      case 2:
        color.b += anim_step;
        if (color.b >= anim_max)
          step++;
        break;
      case 3:
        color.g -= anim_step;
        if (color.g == 0)
          step++;
        break;
      case 4:
        color.r += anim_step;
        if (color.r >= anim_max)
          step++;
        break;
      case 5:
        color.b -= anim_step;
        if (color.b == 0)
          step = 0;
        break;
      }
    }

    ws2812_setColors(pixel_count, pixels);

    delay_ms(delay);
  }
}

void app_main()
{
  nvs_flash_init();

  ws2812_init(WS2812_PIN);
  xTaskCreate(rainbow, "ws2812 rainbow demo", 4096, NULL, 10, NULL);

  //xTaskCreate(rotate, "ws2812 rotate demo", 4096, NULL, 10, NULL);

  return;
}
