/* Copyright (c) 2017 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

#ifndef _st7565LCD_H_
#define _st7565LCD_H_

#include "sdk_errors.h"
#include "nrf_lcd.h"

// Details concerning used LCD display. The one used in the example is 128x32 pixels.
#define ST7565_LCD_WIDTH    ((uint16_t)128)
#define ST7565_LCD_HEIGHT   ((uint16_t)32)


// Functions below are required by nrf_gfx library

/**
 * @brief Function for initializing an SPI transaction manager and the LCD controller.
 */
ret_code_t st7565_init(void);


/**
 * @brief Function for uninitializing an SPI transaction manager instance and for clearing
 * the LCD display buffer
 *
 */
void st7565_uninit(void);


/**
 * @brief Function for drawing a single pixel in the LCD display buffer.
 *
 * @param[in] x             Horizontal coordinate of the pixel.
 * @param[in] y             Vertical coordinate of the pixel.
 * @param[in] color         Color of the pixel in LCD accepted format.
 */
void st7565_pixel_draw(uint16_t x, uint16_t y, uint32_t color);


/**
 * @brief Function for drawing a filled rectangle in the LCD display buffer.
 * 
 * @param[in] x             Horizontal coordinate of the point where to start drawing the rectangle.
 * @param[in] y             Vertical coordinate of the point where to start drawing the rectangle.
 * @param[in] width         Width of the image.
 * @param[in] height        Height of the image.
 * @param[in] color         Color with which to fill the rectangle in LCD accepted format.
 */
void st7565_rect_draw(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);


/**
 * @brief Function for displaying data from an LCD display buffer
 *
 */
void st7565_display_screen(void);


/**
 * @brief Function for setting inversion of colors on the screen.
 *
 * @param[in] invert        If true, inversion will be set.
 */
void st7565_display_invert(bool invert);


/**
 * @brief Function for clearing the LCD display buffer and the LCD screen
 *
 */
void st7565_clear_display(void);

#endif

