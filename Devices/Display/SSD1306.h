/**
 * This Library was originally written by Olivier Van den Eede (4ilo) in 2016.
 * Some refactoring was done and SPI support was added by Aleksander Alekseev (afiskon) in 2018.
 *
 * https://github.com/afiskon/stm32-ssd1306
 */
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <_ansi.h>
#include "LLE_Display.h"
#include "LLE_I2C.h"
#include "LLE_SPI.h"

namespace LowLevelEmbedded::Devices::Display
{
    #ifdef SSD1306_X_OFFSET
    #define SSD1306_X_OFFSET_LOWER (SSD1306_X_OFFSET & 0x0F)
    #define SSD1306_X_OFFSET_UPPER ((SSD1306_X_OFFSET >> 4) & 0x07)
    #else
    #define SSD1306_X_OFFSET_LOWER 0
    #define SSD1306_X_OFFSET_UPPER 0
    #endif

        // SSD1306 OLED height in pixels
    #ifndef SSD1306_HEIGHT
    #define SSD1306_HEIGHT          64
    #endif

        // SSD1306 width in pixels
    #ifndef SSD1306_WIDTH
    #define SSD1306_WIDTH           128
    #endif

    #ifndef SSD1306_BUFFER_SIZE
    #define SSD1306_BUFFER_SIZE   SSD1306_WIDTH * SSD1306_HEIGHT / 8
    #endif

    // Color enumeration
    typedef enum {
        Black = 0x00, // Black color, no pixel
        White = 0x01  // White color, pixel is set
    } SSD1306_COLOR;

    // Font type
    typedef Display_Font_t SSD1306_Font_t;

    // Error type
    typedef enum {
        SSD1306_OK = 0x00,
        SSD1306_ERR = 0x01
    } SSD1306_Error_t;

    // Struct to store transformations
    typedef struct {
        uint16_t CurrentX;
        uint16_t CurrentY;
        uint8_t Initialized;
        uint8_t DisplayOn;
    } SSD1306_t;

    typedef DISPLAY_VERTEX SSD1306_VERTEX;

    class SSD1306 : public IDisplay<SSD1306_COLOR>
    {
    private:
        // Screenbuffer
        uint8_t Buffer[SSD1306_BUFFER_SIZE];
        // Screen object
        SSD1306_t Display;

        II2CAccess* i2c_Access;
        ISPIAccess* spi_Access;
        bool isSPI = false;
        uint8_t address;
        uint16_t csPin;
        SPIMode mode;

        // Low-level procedures
        void WriteCommand(uint8_t byte);
        void WriteData(uint8_t* buffer, size_t buff_size);
        float DegToRad(float par_deg);
        uint16_t NormalizeTo0_360(uint16_t par_deg);

    public:
        SSD1306(II2CAccess* i2cPort, uint8_t address);
        SSD1306(ISPIAccess* spiPort, SPIMode mode = SPIMode::Mode0, uint8_t cs_ID = 0);
        void Init() override;
        void Fill(SSD1306_COLOR color) override;
        void UpdateScreen();
        void DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color) override;
        char WriteChar(char ch, Display_Font_t Font, SSD1306_COLOR color) override;
        char WriteString(char* str, Display_Font_t Font, SSD1306_COLOR color) override;
        void SetCursor(uint8_t x, uint8_t y) override;
        void Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color) override;
        void DrawArc(uint8_t x, uint8_t y, uint8_t radius, uint16_t start_angle, uint16_t sweep,
                   SSD1306_COLOR color) override;
        void DrawArcWithRadiusLine(uint8_t x, uint8_t y, uint8_t radius, uint16_t start_angle,
                                 uint16_t sweep, SSD1306_COLOR color) override;
        void DrawCircle(uint8_t par_x, uint8_t par_y, uint8_t par_r, SSD1306_COLOR color) override;
        void FillCircle(uint8_t par_x, uint8_t par_y, uint8_t par_r, SSD1306_COLOR par_color) override;
        void Polyline(const DISPLAY_VERTEX* par_vertex, uint16_t par_size, SSD1306_COLOR color) override;
        void DrawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color) override;
        void FillRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color) override;
        Display_Error_t InvertRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) override;
        void DrawBitmap(uint8_t x, uint8_t y, const unsigned char* bitmap, uint8_t w, uint8_t h, SSD1306_COLOR color) override;
        void SetContrast(const uint8_t value);
        void SetDisplayOn(const bool on) override;
        bool GetDisplayOn() const override;
        void Reset() override;
        SSD1306_Error_t FillBuffer(uint8_t* buf, uint32_t len);
    };
}