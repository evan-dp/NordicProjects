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

#include "sdk_common.h"

#if NRF_MODULE_ENABLED(ILI9341)

#include "nrf_lcd.h"
#include "nrf_drv_spi.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"

// Set of commands described in ILI9341 datasheet.
#define ILI9341_NOP         0x00
#define ILI9341_SWRESET     0x01
#define ILI9341_RDDID       0x04
#define ILI9341_RDDST       0x09

#define ILI9341_SLPIN       0x10
#define ILI9341_SLPOUT      0x11
#define ILI9341_PTLON       0x12
#define ILI9341_NORON       0x13

#define ILI9341_RDMODE      0x0A
#define ILI9341_RDMADCTL    0x0B
#define ILI9341_RDPIXFMT    0x0C
#define ILI9341_RDIMGFMT    0x0D
#define ILI9341_RDSELFDIAG  0x0F

#define ILI9341_INVOFF      0x20
#define ILI9341_INVON       0x21
#define ILI9341_GAMMASET    0x26
#define ILI9341_DISPOFF     0x28
#define ILI9341_DISPON      0x29

#define ILI9341_CASET       0x2A
#define ILI9341_PASET       0x2B
#define ILI9341_RAMWR       0x2C
#define ILI9341_RAMRD       0x2E

#define ILI9341_PTLAR       0x30
#define ILI9341_MADCTL      0x36
#define ILI9341_PIXFMT      0x3A

#define ILI9341_FRMCTR1     0xB1
#define ILI9341_FRMCTR2     0xB2
#define ILI9341_FRMCTR3     0xB3
#define ILI9341_INVCTR      0xB4
#define ILI9341_DFUNCTR     0xB6

#define ILI9341_PWCTR1      0xC0
#define ILI9341_PWCTR2      0xC1
#define ILI9341_PWCTR3      0xC2
#define ILI9341_PWCTR4      0xC3
#define ILI9341_PWCTR5      0xC4
#define ILI9341_VMCTR1      0xC5
#define ILI9341_VMCTR2      0xC7
#define ILI9341_PWCTRSEQ    0xCB
#define ILI9341_PWCTRA      0xCD
#define ILI9341_PWCTRB      0xCF

#define ILI9341_RDID1       0xDA
#define ILI9341_RDID2       0xDB
#define ILI9341_RDID3       0xDC
#define ILI9341_RDID4       0xDD

#define ILI9341_GMCTRP1     0xE0
#define ILI9341_GMCTRN1     0xE1
#define ILI9341_DGMCTR1     0xE2
#define ILI9341_DGMCTR2     0xE3
#define ILI9341_TIMCTRA     0xE8
#define ILI9341_TIMCTRB     0xEA

#define ILI9341_ENGMCTR     0xF2
#define ILI9341_INCTR       0xF6
#define ILI9341_PUMP        0xF7

#define ILI9341_MADCTL_MY  0x80
#define ILI9341_MADCTL_MX  0x40
#define ILI9341_MADCTL_MV  0x20
#define ILI9341_MADCTL_ML  0x10
#define ILI9341_MADCTL_RGB 0x00
#define ILI9341_MADCTL_BGR 0x08
#define ILI9341_MADCTL_MH  0x04

static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(ILI9341_SPI_INSTANCE);

static inline void spi_write(const void * data, size_t size)
{
    APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, data, size, NULL, 0));
}

static inline void write_command(uint8_t c)
{
    nrf_gpio_pin_clear(ILI9341_DC_PIN);
    spi_write(&c, sizeof(c));
}

static inline void write_data(uint8_t c)
{
    nrf_gpio_pin_set(ILI9341_DC_PIN);
    spi_write(&c, sizeof(c));
}

static void set_addr_window(uint16_t x_0, uint16_t y_0, uint16_t x_1, uint16_t y_1)
{
    ASSERT(x_0 < x_1);
    ASSERT(y_0 < y_1);

    write_command(ILI9341_CASET);
    write_data(x_0 >> 8);
    write_data(x_0);
    write_data(x_1 >> 8);
    write_data(x_1);
    write_command(ILI9341_PASET);
    write_data(y_0 >> 8);
    write_data(y_0);
    write_data(y_1 >> 8);
    write_data(y_1);
    write_command(ILI9341_RAMWR);
}

static void command_list(void)
{
    write_command(ILI9341_PWCTRB);
    write_data(0x00);
    write_data(0XC1);
    write_data(0X30);

    write_command(ILI9341_TIMCTRA);
    write_data(0x85);
    write_data(0x00);
    write_data(0x78);

    write_command(ILI9341_PWCTRSEQ);
    write_data(0x39);
    write_data(0x2C);
    write_data(0x00);
    write_data(0x34);
    write_data(0x02);

    write_command(ILI9341_PUMP);
    write_data(0x20);

    write_command(ILI9341_TIMCTRB);
    write_data(0x00);
    write_data(0x00);

    write_command(ILI9341_PWCTR1);
    write_data(0x23);

    write_command(ILI9341_PWCTR2);
    write_data(0x10);

    write_command(ILI9341_VMCTR1);
    write_data(0x3e);
    write_data(0x28);

    write_command(ILI9341_VMCTR2);
    write_data(0x86);

    write_command(ILI9341_MADCTL);
    write_data(0x48);

    write_command(ILI9341_PIXFMT);
    write_data(0x55);

    write_command(ILI9341_FRMCTR1);
    write_data(0x00);
    write_data(0x18);

    write_command(ILI9341_DFUNCTR);
    write_data(0x08);
    write_data(0x82);
    write_data(0x27);

    write_command(ILI9341_ENGMCTR);
    write_data(0x00);

    write_command(ILI9341_GAMMASET);
    write_data(0x01);

    write_command(ILI9341_GMCTRP1);
    write_data(0x0F);
    write_data(0x31);
    write_data(0x2B);
    write_data(0x0C);
    write_data(0x0E);
    write_data(0x08);
    write_data(0x4E);
    write_data(0xF1);
    write_data(0x37);
    write_data(0x07);
    write_data(0x10);
    write_data(0x03);
    write_data(0x0E);
    write_data(0x09);
    write_data(0x00);

    write_command(ILI9341_GMCTRN1);
    write_data(0x00);
    write_data(0x0E);
    write_data(0x14);
    write_data(0x03);
    write_data(0x11);
    write_data(0x07);
    write_data(0x31);
    write_data(0xC1);
    write_data(0x48);
    write_data(0x08);
    write_data(0x0F);
    write_data(0x0C);
    write_data(0x31);
    write_data(0x36);
    write_data(0x0F);

    write_command(ILI9341_SLPOUT);
    nrf_delay_ms(120);
    write_command(ILI9341_DISPON);
}

static ret_code_t hardware_init(void)
{
    ret_code_t err_code;

    nrf_gpio_cfg_output(ILI9341_DC_PIN);

    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;

    spi_config.sck_pin  = ILI9341_SCK_PIN;
    spi_config.miso_pin = ILI9341_MISO_PIN;
    spi_config.mosi_pin = ILI9341_MOSI_PIN;
    spi_config.ss_pin   = ILI9341_SS_PIN;

    err_code = nrf_drv_spi_init(&spi, &spi_config, NULL, NULL);
    return err_code;
}

static ret_code_t ili9341_init(void)
{
    ret_code_t err_code;

    err_code = hardware_init();
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    command_list();

    return err_code;
}

static void ili9341_uninit(void)
{
    nrf_drv_spi_uninit(&spi);
}

static void ili9341_pixel_draw(uint16_t x, uint16_t y, uint32_t color)
{
    set_addr_window(x, y, x, y);

    const uint8_t data[2] = {color >> 8, color};

    nrf_gpio_pin_set(ILI9341_DC_PIN);

    spi_write(data, sizeof(data));

    nrf_gpio_pin_clear(ILI9341_DC_PIN);
}

static void ili9341_rect_draw(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color)
{
    set_addr_window(x, y, x + width - 1, y + height - 1);

    const uint8_t data[2] = {color >> 8, color};

    nrf_gpio_pin_set(ILI9341_DC_PIN);

    // Duff's device algorithm for optimizing loop.
    uint32_t i = (height * width + 7) / 8;

/*lint -save -e525 -e616 -e646 */
    switch ((height * width) % 8) {
        case 0:
            do {
                spi_write(data, sizeof(data));
        case 7:
                spi_write(data, sizeof(data));
        case 6:
                spi_write(data, sizeof(data));
        case 5:
                spi_write(data, sizeof(data));
        case 4:
                spi_write(data, sizeof(data));
        case 3:
                spi_write(data, sizeof(data));
        case 2:
                spi_write(data, sizeof(data));
        case 1:
                spi_write(data, sizeof(data));
            } while (--i > 0);
        default:
            break;
    }
/*lint -restore */

    nrf_gpio_pin_clear(ILI9341_DC_PIN);
}

static void ili9341_dummy_display(void)
{
    /* No implementation needed. */
}

static void ili9341_rotation_set(nrf_lcd_rotation_t rotation)
{
    write_command(ILI9341_MADCTL);
    switch (rotation) {
        case NRF_LCD_ROTATE_0:
            write_data(ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR);
            break;
        case NRF_LCD_ROTATE_90:
            write_data(ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR);
            break;
        case NRF_LCD_ROTATE_180:
            write_data(ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR);
            break;
        case NRF_LCD_ROTATE_270:
            write_data(ILI9341_MADCTL_MX | ILI9341_MADCTL_MY | ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR);
            break;
        default:
            break;
    }
}

static void ili9341_display_invert(bool invert)
{
    write_command(invert ? ILI9341_INVON : ILI9341_INVOFF);
}

static lcd_cb_t ili9341_cb = {
    .height = ILI9341_HEIGHT,
    .width = ILI9341_WIDTH
};


const nrf_lcd_t nrf_lcd_ili9341 = {
    .lcd_init = ili9341_init,
    .lcd_uninit = ili9341_uninit,
    .lcd_pixel_draw = ili9341_pixel_draw,
    .lcd_rect_draw = ili9341_rect_draw,
    .lcd_display = ili9341_dummy_display,
    .lcd_rotation_set = ili9341_rotation_set,
    .lcd_display_invert = ili9341_display_invert,
    .p_lcd_cb = &ili9341_cb
};

#endif // NRF_MODULE_ENABLED(ILI9341)
