#pragma once

#include <LovyanGFX.hpp>
#include "bitslate_config.h"

class LGFX_BitSlate : public lgfx::LGFX_Device {
private:
    lgfx::Panel_ILI9488 _panel;
    lgfx::Bus_SPI _bus;
    lgfx::Touch_XPT2046 _touch;

public:
    LGFX_BitSlate() {
        {
            auto cfg = _bus.config();

            cfg.spi_host = SPI2_HOST;
            cfg.spi_mode = 0;
            cfg.freq_write = 40000000;
            cfg.freq_read = 16000000;
            cfg.spi_3wire = false;
            cfg.use_lock = true;
            cfg.dma_channel = SPI_DMA_CH_AUTO;

            cfg.pin_sclk = PIN_TFT_SCLK;
            cfg.pin_mosi = PIN_TFT_MOSI;
            cfg.pin_miso = PIN_TFT_MISO;
            cfg.pin_dc   = PIN_TFT_DC;

            _bus.config(cfg);
            _panel.setBus(&_bus);
        }

        {
            auto cfg = _panel.config();

            cfg.pin_cs   = PIN_TFT_CS;
            cfg.pin_rst  = PIN_TFT_RST;
            cfg.pin_busy = -1;

            cfg.panel_width  = 320;
            cfg.panel_height = 480;

            cfg.memory_width  = 320;
            cfg.memory_height = 480;

            cfg.offset_x = 0;
            cfg.offset_y = 0;
            cfg.offset_rotation = 0;

            cfg.dummy_read_pixel = 8;
            cfg.dummy_read_bits = 1;

            cfg.readable = false;
            cfg.invert = false;
            cfg.rgb_order = false;
            cfg.dlen_16bit = false;
            cfg.bus_shared = true;

            _panel.config(cfg);
        }

        {
            auto cfg = _touch.config();

            cfg.x_min = 200;
            cfg.x_max = 3900;
            cfg.y_min = 200;
            cfg.y_max = 3900;

            cfg.pin_int = PIN_TOUCH_IRQ;
            cfg.bus_shared = true;
            cfg.offset_rotation = 0;

            cfg.spi_host = SPI2_HOST;
            cfg.freq = 2500000;

            cfg.pin_sclk = PIN_TFT_SCLK;
            cfg.pin_mosi = PIN_TFT_MOSI;
            cfg.pin_miso = PIN_TFT_MISO;
            cfg.pin_cs   = PIN_TOUCH_CS;

            _touch.config(cfg);
            _panel.setTouch(&_touch);
        }

        setPanel(&_panel);
    }

    bool getTouch(uint16_t* x, uint16_t* y) {
        bool touched = lgfx::LGFX_Device::getTouch(x, y);
        if (touched && x != nullptr) {
            *x = this->width() - 1 - *x;
        }
        return touched;
    }
};
