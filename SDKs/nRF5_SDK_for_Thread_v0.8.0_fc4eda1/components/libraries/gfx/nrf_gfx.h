/* Copyright (c) 2017 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 */

#ifndef NRF_GFX_H__
#define NRF_GFX_H__

#include <stdint.h>
#include <stdbool.h>
#include "sdk_errors.h"
#include "nrf_lcd.h"
#include "nrf_font.h"

/** @file
 *
 * @addtogroup ili9341_config
 * @ingroup nrf_gfx
 * 
 * @addtogroup st7735_config
 * @ingroup nrf_gfx
 *
 * @defgroup nrf_gfx GFX Library
 * @{
 * @ingroup app_common
 *
 * @brief Module for drawing graphical objects like lines, circles, and rectangles.
 */

/**
 * @brief GFX point object structure.
 */
typedef struct
{
    uint16_t x;                 /**< Horizontal coordinate of the point where to start drawing the object. */
    uint16_t y;                 /**< Vertical coordinate of the point where to start drawing the object. */
}nrf_gfx_point_t;

/**
 * @brief GFX line object structure.
 */
typedef struct
{
    uint16_t x_start;           /**< Horizontal coordinate of the point where to start drawing the object. */
    uint16_t y_start;           /**< Vertical coordinate of the point where to start drawing the object. */
    uint16_t x_end;             /**< Horizontal coordinate of the point where to end drawing the object. */
    uint16_t y_end;             /**< Vertical coordinate of the point where to end drawing the object. */
    uint16_t thickness;         /**< Thickness of the border of the object. */
}nrf_gfx_line_t;

/**
 * @brief GFX circle object structure.
 */
typedef struct
{
    uint16_t x;                 /**< Horizontal coordinate of the centre of the object. */
    uint16_t y;                 /**< Vertical coordinate of the centre of the object. */
    uint16_t r;                 /**< Radius of the object. */
}nrf_gfx_circle_t;

/**
 * @brief GFX rectangle object structure.
 */
typedef struct
{
    uint16_t x;                 /**< Horizontal coordinate of the point where to start drawing the object. */
    uint16_t y;                 /**< Vertical coordinate of the point where to start drawing the object. */
    uint16_t width;             /**< Width of the object. */
    uint16_t height;            /**< Height of the object. */
}nrf_gfx_rect_t;

/**
 * @defgroup nrf_gfx_macros Macros for defining new graphic objects
 * @{
 */
#define NRF_GFX_POINT(_x, _y)   \
    {                           \
        .x = (_x),              \
        .y = (_y)               \
    }

#define NRF_GFX_LINE(_x_0, _y_0, _x_1, _y_1, _thickness)    \
    {                                               \
        .x_start = (_x_0),                          \
        .y_start = (_y_0),                          \
        .x_end = (_x_1),                            \
        .y_end = (_y_1),                            \
        .thickness = (_thickness)                   \
    }

#define NRF_GFX_CIRCLE(_x, _y, _radius)     \
    {                                       \
        .x = (_x),                          \
        .y = (_y),                          \
        .r = (_radius)                      \
    }

#define NRF_GFX_RECT(_x, _y, _width, _height)   \
    {                                           \
        .x = (_x),                              \
        .y = (_y),                              \
        .width = (_width),                      \
        .height = (_height)                     \
    }
/* @} */

/**
 * @brief Font descriptor type.
 */
typedef FONT_INFO nrf_gfx_font_desc_t;

/**
 * @brief Function for initializing the GFX library.
 *
 * @param[in] p_instance            Pointer to the LCD instance.
 *
 * @return Values returned by @ref nrf_lcd_t::lcd_init.
 */
ret_code_t nrf_gfx_init(nrf_lcd_t const * p_instance);

/**
 * @brief Function for uninitializing the GFX library.
 *
 * @param[in] p_instance            Pointer to the LCD instance.
 *
 * @return Values returned by @ref nrf_lcd_t::lcd_uninit.
 */
void nrf_gfx_uninit(nrf_lcd_t const * p_instance);

/**
 * @brief Function for drawing a point.
 *
 * @param[in] p_instance            Pointer to the LCD instance.
 * @param[in] p_point               Pointer to the point object.
 * @param[in] color                 Color of the object in the display accepted format.
 */
void nrf_gfx_point_draw(nrf_lcd_t const * p_instance, nrf_gfx_point_t const * p_point, uint32_t color);

/**
 * @brief Function for drawing a line.
 *
 * @param[in] p_instance            Pointer to the LCD instance.
 * @param[in] p_line                Pointer to the line object.
 * @param[in] color                 Color of the object in the display accepted format.
 *
 * @retval NRF_ERROR_INVALID_PARAM  If object position is not on the screen.
 * @retval NRF_SUCCESS              If object was successfully drawn.
 */
ret_code_t nrf_gfx_line_draw(nrf_lcd_t const * p_instance, nrf_gfx_line_t const * p_line, uint32_t color);

/**
 * @brief Function for drawing a circle.
 *
 * @param[in] p_instance            Pointer to the LCD instance.
 * @param[in] p_circle              Pointer to the circle object.
 * @param[in] color                 Color of the object in the display accepted format.
 * @param[in] fill                  If true, the circle will be filled.
 *
 * @retval NRF_ERROR_INVALID_PARAM  If object position is not on the screen.
 * @retval NRF_SUCCESS              If object was successfully drawn.
 *
 * @note The height and width of the drawn circle are determined by: radius * 2 + 1.
 *
 */
ret_code_t nrf_gfx_circle_draw(nrf_lcd_t const * p_instance,
                               nrf_gfx_circle_t const * p_circle,
                               uint32_t color,
                               bool fill);

/**
 * @brief Function for drawing a rectangle.
 *
 * @param[in] p_instance            Pointer to the LCD instance.
 * @param[in] p_rect                Pointer to the rectangle object.
 * @param[in] thickness             Thickness of the rectangle border.
 * @param[in] color                 Color of the object in the display accepted format.
 * @param[in] fill                  If true, the rectangle will be filled.
 *
 * @retval NRF_ERROR_INVALID_PARAM  If object position is not on the screen.
 * @retval NRF_SUCCESS              If object was successfully drawn.
 */
ret_code_t nrf_gfx_rect_draw(nrf_lcd_t const * p_instance,
                             nrf_gfx_rect_t const * p_rect,
                             uint16_t thickness,
                             uint32_t color,
                             bool fill);

/**
 * @brief Function for filling the screen with selected color.
 *
 * @param[in] p_instance            Pointer to the LCD instance.
 * @param[in] color                 Color of the screen in the display accepted format.
 */
void nrf_gfx_screen_fill(nrf_lcd_t const * p_instance, uint32_t color);

/**
 * @brief Function for drawing an image from a .bmp file.
 *
 * Data in img_buf is expected to be stored in 2-byte samples, little endianness, RGB565 format.
 * Pointer should skip the header and point to the first byte of data.
 *
 * @param[in] p_instance            Pointer to the LCD instance.
 * @param[in] p_rect                Pointer to the rectangle object.
 * @param[in] img_buf               Pointer to data from the .bmp file.
 *
 * @note Only compatible with displays that accept pixels in RGB565 format.
 */
ret_code_t nrf_gfx_bmp565_draw(nrf_lcd_t const * p_instance,
                               nrf_gfx_rect_t const * p_rect,
                               uint16_t const * img_buf);

/**
 * @brief Function for drawing an image from a .bmp file.
 *
 * Data in img_buf is expected to be stored in 2-byte samples, little endianness, RGB565 format.
 * Pointer should skip the header and point to the first byte of data.
 *
 * @param[in] p_instance            Pointer to the LCD instance.
 * @param[in] img_buf               Pointer to data from the .bmp file.
 *
 * @note Only compatible with displays that accept pixels in RGB565 format.
 */
void nrf_gfx_background_set(nrf_lcd_t const * p_instance, uint16_t const * img_buf);

/**
 * @brief Function for displaying data from an internal frame buffer.
 *
 * @param[in] p_instance            Pointer to the LCD instance.
 */
void nrf_gfx_display(nrf_lcd_t const * p_instance);

/**
 * @brief Function for setting screen rotation.
 *
 * @param[in] p_instance            Pointer to the LCD instance.
 * @param[in] rotation              Rotation to be made.
 */
void nrf_gfx_rotation_set(nrf_lcd_t const * p_instance, nrf_lcd_rotation_t rotation);

/**
 * @brief Function for setting inversion of colors.
 *
 * @param[in] p_instance            Pointer to the LCD instance.
 * @param[in] invert                If true, inversion will be set.
 */
void nrf_gfx_invert(nrf_lcd_t const * p_instance, bool invert);

/**
 * @brief Function for printing a string to the screen.
 *
 * @param[in] p_instance            Pointer to the LCD instance.
 * @param[in] p_point               Pointer to the point where to start drawing the object.
 * @param[in] font_color            Color of the font in the display accepted format.
 * @param[in] p_string              Pointer to the string.
 * @param[in] p_font                Pointer to the font descriptor.
 * @param[in] wrap                  If true, the string will be wrapped to the new line.
 */
ret_code_t nrf_gfx_print(nrf_lcd_t const * p_instance,
                         nrf_gfx_point_t const * p_point,
                         uint16_t font_color,
                         const char * p_string,
                         const nrf_gfx_font_desc_t * p_font,
                         bool wrap);

/**
 * @brief Function for getting the height of the screen.
 *
 * @param[in] p_instance            Pointer to the LCD instance.
 *
 */
uint16_t nrf_gfx_height_get(nrf_lcd_t const * p_instance);

/**
 * @brief Function for getting the width of the screen.
 *
 * @param[in] p_instance            Pointer to the LCD instance.
 *
 */
uint16_t nrf_gfx_width_get(nrf_lcd_t const * p_instance);

/* @} */

#endif //NRF_GFX_H__

