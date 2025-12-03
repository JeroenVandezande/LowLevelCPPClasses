/**
 * This Library was originally written by Olivier Van den Eede (4ilo) in 2016.
 * Some refactoring was done and SPI support was added by Aleksander Alekseev (afiskon) in 2018.
 *
 * https://github.com/afiskon/stm32-ssd1306
 */
#include "SSD1306.h"
#include "LLE_I2C.h"
#include "LLE_SPI.h"
#include "../../Utilities/Delay.h"

#include <math.h>
#include <stdexcept>
#include <string.h>

namespace LowLevelEmbedded::Devices::Display
{
    // SSD1306 I2C Commands
    // SSD1306 Commands
    #define SSD1306_COMMAND_SET_CONTRAST                   0x81
    #define SSD1306_COMMAND_ENTIRE_DISPLAY_ON_RESUME       0xA4
    #define SSD1306_COMMAND_ENTIRE_DISPLAY_ON              0xA5
    #define SSD1306_COMMAND_NORMAL_DISPLAY                 0xA6
    #define SSD1306_COMMAND_INVERT_DISPLAY                 0xA7
    #define SSD1306_COMMAND_DISPLAY_OFF                    0xAE
    #define SSD1306_COMMAND_DISPLAY_ON                     0xAF
    #define SSD1306_COMMAND_SET_DISPLAY_OFFSET             0xD3
    #define SSD1306_COMMAND_SET_COM_PINS                   0xDA
    #define SSD1306_COMMAND_SET_VCOM_DETECT                0xDB
    #define SSD1306_COMMAND_SET_DISPLAY_CLOCK_DIV          0xD5
    #define SSD1306_COMMAND_SET_PRECHARGE                  0xD9
    #define SSD1306_COMMAND_SET_MULTIPLEX                  0xA8
    #define SSD1306_COMMAND_SET_LOW_COLUMN                 0x00
    #define SSD1306_COMMAND_SET_HIGH_COLUMN                0x10
    #define SSD1306_COMMAND_SET_START_LINE                 0x40
    #define SSD1306_COMMAND_MEMORY_MODE                    0x20
    #define SSD1306_COMMAND_COLUMN_ADDR                    0x21
    #define SSD1306_COMMAND_PAGE_ADDR                      0x22
    #define SSD1306_COMMAND_COM_SCAN_INC                   0xC0
    #define SSD1306_COMMAND_COM_SCAN_DEC                   0xC8
    #define SSD1306_COMMAND_SEG_REMAP                      0xA0
    #define SSD1306_COMMAND_CHARGE_PUMP                    0x8D
    #define SSD1306_COMMAND_EXTERNAL_VCC                   0x01
    #define SSD1306_COMMAND_SWITCH_CAP_VCC                 0x02

    // Screen dimensions
    #define SSD1306_LCD_WIDTH                              128
    #define SSD1306_LCD_HEIGHT                             64

    // I2C-specific commands
    #define SSD1306_CONTROL_BYTE_CMD_SINGLE                0x80
    #define SSD1306_CONTROL_BYTE_CMD_STREAM                0x00
    #define SSD1306_CONTROL_BYTE_DATA_STREAM               0x40

    // SPI-specific commands
    #define SSD1306_DATA                                   1
    #define SSD1306_COMMAND                                0

    template <bool Rotate90>
    SSD1306<Rotate90>::SSD1306(II2CAccess* i2cPort, uint8_t address)
        : i2c_Access(i2cPort), spi_Access(nullptr), isSPI(false), address(address)
    {
        memset(Buffer, 0, SSD1306_BUFFER_SIZE);
        Display.CurrentX = 0;
        Display.CurrentY = 0;
        Display.Initialized = 0;
        Display.DisplayOn = 0;
    }

    template <bool Rotate90>
    SSD1306<Rotate90>::SSD1306(ISPIAccess* spiPort, SPIMode mode, uint8_t csID)
        : i2c_Access(nullptr), spi_Access(spiPort), isSPI(true), csPin(csID), mode(mode)
    {
        memset(Buffer, 0, SSD1306_BUFFER_SIZE);
        Display.CurrentX = 0;
        Display.CurrentY = 0;
        Display.Initialized = 0;
        Display.DisplayOn = 0;
    }

    template <bool Rotate90>
    void SSD1306<Rotate90>::Init()
    {
        // Wait for the screen to boot
        Utility::Delay_ms(100);

        // Init OLED
        WriteCommand(SSD1306_COMMAND_DISPLAY_OFF);                                   // 0xAE

        // Addressing Mode
        WriteCommand(SSD1306_COMMAND_MEMORY_MODE);                                   // 0x20
        WriteCommand(0x00);                                                          // 0x0 horizontal addressing mode
        // WriteCommand(0x02);                                                          // 0x0 page addressing mode

        // Vertical Mirroring
        if (isMirroredVertically)
        {
            WriteCommand(SSD1306_COMMAND_COM_SCAN_INC);                              // 0xC0
        }
        else
        {
            WriteCommand(SSD1306_COMMAND_COM_SCAN_DEC);                              // 0xC8
        }

        WriteCommand(SSD1306_COMMAND_SET_START_LINE);                                // 0x40 line #0

        // Set Contrast
        WriteCommand(SSD1306_COMMAND_SET_CONTRAST);                                  // 0x81
        WriteCommand(0xCF);

        // Horizontal Mirroring
        if (isMirroredHorizontally)
        {
            WriteCommand(SSD1306_COMMAND_SEG_REMAP | 0x00);                          // 0xA0
        }
        else
        {
            WriteCommand(SSD1306_COMMAND_SEG_REMAP | 0x01);                          // 0xA1
        }

        // Inverse Color
        if (isInverseColor)
        {
            WriteCommand(SSD1306_COMMAND_INVERT_DISPLAY);                            // 0xA7
        }
        else
        {
            WriteCommand(SSD1306_COMMAND_NORMAL_DISPLAY);                            // 0xA6
        }

        // Set Multiplex Ratio Command
        if (SSD1306_HEIGHT == 128)
        {
            // Found in the Luma Python lib for SH1106.
            WriteCommand(0xFF);
        }
        else
        {
            //--set multiplex ratio(1 to 64) - CHECK
            WriteCommand(SSD1306_COMMAND_SET_MULTIPLEX);                             // 0xA8
        }

        // Set Multiplex Ratio Value
        if (SSD1306_HEIGHT == 32)
        {
            WriteCommand(0x1F);
        }
        else if (SSD1306_HEIGHT == 64)
        {
            WriteCommand(0x3F);
        }
        else if (SSD1306_HEIGHT == 128)
        {
            WriteCommand(0x3F);
        }
        else
        {
            throw new std::runtime_error("Invalid display height!");
        }

        //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
        WriteCommand(SSD1306_COMMAND_ENTIRE_DISPLAY_ON_RESUME);                      // 0xA4

        WriteCommand(SSD1306_COMMAND_SET_DISPLAY_OFFSET);                            // 0xD3
        WriteCommand(0x00);                                                          // No offset

        WriteCommand(SSD1306_COMMAND_SET_DISPLAY_CLOCK_DIV);                         // 0xD5
        WriteCommand(0x80);

        WriteCommand(SSD1306_COMMAND_SET_PRECHARGE);                                 // 0xD9
        WriteCommand(0x22);

        // COM Pins hardware configuration
        WriteCommand(SSD1306_COMMAND_SET_COM_PINS);                                  // 0xDA
        if (SSD1306_HEIGHT == 32)
        {
            WriteCommand(0x02);
        }
        else if (SSD1306_HEIGHT == 64)
        {
            WriteCommand(0x12);
        }
        else if (SSD1306_HEIGHT == 128)
        {
            WriteCommand(0x12);
        }
        else
        {
            throw new std::runtime_error("Invalid display height!");
        }

        WriteCommand(SSD1306_COMMAND_SET_VCOM_DETECT);                               // 0xDB
        WriteCommand(0x30);

        WriteCommand(SSD1306_COMMAND_CHARGE_PUMP);                                   // 0x8D
        WriteCommand(0x14);                                                          // Use internal DC-DC Boost

        WriteCommand(SSD1306_COMMAND_DISPLAY_ON);                                    // 0xAF

        Fill(Black);

        Display.CurrentX = 0;
        Display.CurrentY = 0;
        Display.Initialized = 1;
        Display.DisplayOn = 1;

        UpdateScreen();
    }



    template <bool Rotate90>
    void SSD1306<Rotate90>::Fill(SSD1306_COLOR color)
    {
        // Fill buffer with selected color
        memset(Buffer, (color == Black) ? 0x00 : 0xFF, sizeof(Buffer));
    }

    /* Write the screenbuffer with changed to the screen */
    template <bool Rotate90>
    void SSD1306<Rotate90>::UpdateScreen()
    {
        // Write data to each page of RAM. Number of pages
        // depends on the screen height:
        //
        //  * 32px   ==  4 pages
        //  * 64px   ==  8 pages
        //  * 128px  ==  16 pages
        // Write data to controller memory
        if (Display.Initialized)
        {
            for(uint8_t i = 0; i < SSD1306_HEIGHT/8; i++) {
                WriteCommand(0xB0 + i); // Set the current RAM page address.
                WriteCommand(0x00 + SSD1306_X_OFFSET_LOWER);
                WriteCommand(0x10 + SSD1306_X_OFFSET_UPPER);
                WriteData(&Buffer[SSD1306_WIDTH*i],SSD1306_WIDTH);
            }
        }
    }

    template <bool Rotate90>
    void SSD1306<Rotate90>::DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color)
    {
        // Translate x and y to be rotated 90 deg
        if constexpr (Rotate90)
        {
            uint8_t temp = y;
            y = SSD1306_HEIGHT - x - 1;
            x = temp;
        }

        if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT)
        {
            // Don't write outside the buffer
            return;
        }

        // Check if pixel should be inverted
        if (color == White)
        {
            Buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
        }
        else
        {
            Buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
        }
    }

    template <bool Rotate90>
    char SSD1306<Rotate90>::WriteChar(char ch, Display_Font_t Font, SSD1306_COLOR color)
    {
        uint32_t i, b, j;

        // Check if character is valid
        if (ch < 32 || ch > 126)
            return 0;

        // Char width is not equal to font width for proportional font
        const uint8_t char_width = Font.char_width ? Font.char_width[ch-32] : Font.width;
        // Check remaining space on current line
        if constexpr (Rotate90)
        {
            if (SSD1306_HEIGHT < (Display.CurrentX + char_width) ||
                SSD1306_WIDTH < (Display.CurrentY + Font.height))
            {
                // Not enough space on current line
                return 0;
            }
        }
        else
        {
            if (SSD1306_WIDTH < (Display.CurrentX + char_width) ||
                SSD1306_HEIGHT < (Display.CurrentY + Font.height))
            {
                // Not enough space on current line
                return 0;
            }
        }

        // Use the font to write
        for(i = 0; i < Font.height; i++) {
            b = Font.data[(ch - 32) * Font.height + i];
            for(j = 0; j < char_width; j++) {
                if((b << j) & 0x8000)  {
                    DrawPixel(Display.CurrentX + j, (Display.CurrentY + i), (SSD1306_COLOR) color);
                } else {
                    DrawPixel(Display.CurrentX + j, (Display.CurrentY + i), (SSD1306_COLOR)!color);
                }
            }
        }

        // The current space is now taken
        Display.CurrentX += char_width;

        // Return written char for validation
        return ch;
    }

    template <bool Rotate90>
    char SSD1306<Rotate90>::WriteString(char* str, Display_Font_t Font, SSD1306_COLOR color)
    {
        // Write until null-byte
        while (*str)
        {
            if (WriteChar(*str, Font, color) != *str)
            {
                // Char could not be written
                return *str;
            }

            // Next char
            str++;
        }

        // Everything ok
        return *str;
    }

    template <bool Rotate90>
    void SSD1306<Rotate90>::SetCursor(uint8_t x, uint8_t y)
    {
        Display.CurrentX = x;
        Display.CurrentY = y;
    }

    template <bool Rotate90>
    void SSD1306<Rotate90>::Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color)
    {
        int dx = (x2 >= x1) ? x2 - x1 : x1 - x2;
        int dy = (y2 >= y1) ? y2 - y1 : y1 - y2;
        int sx = (x2 >= x1) ? 1 : -1;
        int sy = (y2 >= y1) ? 1 : -1;
        int err = dx - dy;
        int e2;

        while (1)
        {
            DrawPixel(x1, y1, color);

            if (x1 == x2 && y1 == y2)
                break;

            e2 = 2 * err;

            if (e2 > -dy)
            {
                err -= dy;
                x1 += sx;
            }

            if (e2 < dx)
            {
                err += dx;
                y1 += sy;
            }
        }
    }

    template <bool Rotate90>
    void SSD1306<Rotate90>::DrawArc(uint8_t x, uint8_t y, uint8_t radius, uint16_t start_angle, uint16_t sweep, SSD1306_COLOR color)
    {
        float sx = (float)cos(DegToRad(start_angle));
        float sy = (float)sin(DegToRad(start_angle));
        uint16_t deg = NormalizeTo0_360(start_angle);

        for (; NormalizeTo0_360(deg) <= NormalizeTo0_360(start_angle + sweep); deg++)
        {
            float rad = DegToRad(deg);
            float cx = (float)cos(rad) * radius;
            float cy = (float)sin(rad) * radius;

            DrawPixel(x + (int)cx, y + (int)cy, color);
        }
    }

    template <bool Rotate90>
    void SSD1306<Rotate90>::DrawArcWithRadiusLine(uint8_t x, uint8_t y, uint8_t radius, uint16_t start_angle, uint16_t sweep, SSD1306_COLOR color)
    {
        DrawArc(x, y, radius, start_angle, sweep, color);
        float sx = (float)cos(DegToRad(start_angle));
        float sy = (float)sin(DegToRad(start_angle));
        uint16_t deg = NormalizeTo0_360(start_angle + sweep);
        float ex = (float)cos(DegToRad(deg));
        float ey = (float)sin(DegToRad(deg));

        Line(x, y, x + sx * radius, y + sy * radius, color);
        Line(x, y, x + ex * radius, y + ey * radius, color);
    }

    template <bool Rotate90>
    void SSD1306<Rotate90>::DrawCircle(uint8_t par_x, uint8_t par_y, uint8_t par_r, SSD1306_COLOR color)
    {
        int32_t x = -par_r;
        int32_t y = 0;
        int32_t err = 2 - 2 * par_r;
        int32_t e2;

        do
        {
            DrawPixel(par_x - x, par_y + y, color);
            DrawPixel(par_x + x, par_y + y, color);
            DrawPixel(par_x + x, par_y - y, color);
            DrawPixel(par_x - x, par_y - y, color);

            e2 = err;
            if (e2 <= y)
            {
                y++;
                err += y * 2 + 1;
                if (-x == y && e2 <= x)
                {
                    e2 = 0;
                }
            }

            if (e2 > x)
            {
                x++;
                err += x * 2 + 1;
            }
        } while (x <= 0);
    }

    template <bool Rotate90>
    void SSD1306<Rotate90>::FillCircle(uint8_t par_x, uint8_t par_y, uint8_t par_r, SSD1306_COLOR par_color)
    {
        int32_t x = -par_r;
        int32_t y = 0;
        int32_t err = 2 - 2 * par_r;
        int32_t e2;

        do
        {
            for (int i = par_x + x; i <= par_x - x; i++)
            {
                DrawPixel(i, par_y + y, par_color);
                DrawPixel(i, par_y - y, par_color);
            }

            e2 = err;
            if (e2 <= y)
            {
                y++;
                err += y * 2 + 1;
                if (-x == y && e2 <= x)
                {
                    e2 = 0;
                }
            }

            if (e2 > x)
            {
                x++;
                err += x * 2 + 1;
            }
        } while (x <= 0);
    }

    template <bool Rotate90>
    void SSD1306<Rotate90>::Polyline(const DISPLAY_VERTEX* par_vertex, uint16_t par_size, SSD1306_COLOR color)
    {
        if (par_size < 2)
            return;

        for (uint16_t i = 0; i < (par_size - 1); i++)
        {
            Line(par_vertex[i].x, par_vertex[i].y, par_vertex[i + 1].x, par_vertex[i + 1].y, color);
        }
    }

    template <bool Rotate90>
    void SSD1306<Rotate90>::DrawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color)
    {
        Line(x1, y1, x2, y1, color);
        Line(x1, y1, x1, y2, color);
        Line(x2, y1, x2, y2, color);
        Line(x1, y2, x2, y2, color);
    }

    template <bool Rotate90>
    void SSD1306<Rotate90>::FillRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color)
    {
        for (uint8_t y = y1; y <= y2; y++)
        {
            for (uint8_t x = x1; x <= x2; x++)
            {
                DrawPixel(x, y, color);
            }
        }
    }

    template <bool Rotate90>
    Display_Error_t SSD1306<Rotate90>::InvertRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
    {
        // Boundary check
        if ((x1 >= SSD1306_WIDTH) || (y1 >= SSD1306_HEIGHT) || (x2 >= SSD1306_WIDTH) || (y2 >= SSD1306_HEIGHT))
            return DISPLAY_ERR;

        // Loop through specified rectangle and invert pixels
        for (uint8_t y = y1; y <= y2; y++)
        {
            for (uint8_t x = x1; x <= x2; x++)
            {
                uint8_t bit = y % 8;
                uint8_t* byte = &Buffer[x + (y / 8) * SSD1306_WIDTH];

                // XOR the bit to invert it
                *byte ^= (1 << bit);
            }
        }

        return DISPLAY_OK;
    }

    template <bool Rotate90>
    void SSD1306<Rotate90>::DrawBitmap(uint8_t x, uint8_t y, const unsigned char* bitmap, uint8_t w, uint8_t h, SSD1306_COLOR color)
    {
        int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
        uint8_t byte = 0;

        if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) {
            return;
        }

        for (uint8_t j = 0; j < h; j++, y++) {
            for (uint8_t i = 0; i < w; i++) {
                if (i & 7) {
                    byte <<= 1;
                } else {
                    byte = (*(const unsigned char *)(&bitmap[j * byteWidth + i / 8]));
                }

                if (byte & 0x80) {
                    DrawPixel(x + i, y, color);
                }
            }
        }
        return;
    }

    // Mirrors display. Re-init and screen clear should be ran after.
    template <bool Rotate90>
    void SSD1306<Rotate90>::MirrorHorizontally(bool isMirrored)
    {
        isMirroredHorizontally = isMirrored;
    }

    // Mirrors display. Re-init and screen clear should be ran after.
    template <bool Rotate90>
    void SSD1306<Rotate90>::MirrorVertically(bool isMirrored)
    {
        isMirroredVertically = isMirrored;
    }

    template <bool Rotate90>
    void SSD1306<Rotate90>::InverseColor(bool isInverse)
    {
        isInverseColor = isInverse;
    }

    template <bool Rotate90>
    void SSD1306<Rotate90>::SetContrast(const uint8_t value)
    {
        WriteCommand(0x81);    // Set contrast control
        WriteCommand(value);
    }

    template <bool Rotate90>
    void SSD1306<Rotate90>::SetDisplayOn(const bool on)
    {
        if (on)
        {
            WriteCommand(SSD1306_COMMAND_DISPLAY_ON);
            Display.DisplayOn = 1;
        }
        else
        {
            WriteCommand(SSD1306_COMMAND_DISPLAY_OFF);
            Display.DisplayOn = 0;
        }
    }

    template <bool Rotate90>
    bool SSD1306<Rotate90>::GetDisplayOn() const
    {
        return Display.DisplayOn;
    }

    template <bool Rotate90>
    void SSD1306<Rotate90>::Reset()
    {
        // TODO: Reset the display using the reset pin if provided
        Init();
    }

    // Private methods
    template <bool Rotate90>
    void SSD1306<Rotate90>::WriteCommand(uint8_t byte)
    {
        uint8_t buffer[2];
        buffer[0] = byte;
        if (isSPI)
        {
            // TODO: Implement SPI
            // SPI mode command write
            spi_Access->WriteSPI(buffer, 2, csPin, mode);
        }
        else
        {
            // I2C mode command write
            i2c_Access->I2C_Mem_Write(address, 0x00, 1,  &byte, 1);
        }
    }

    template <bool Rotate90>
    void SSD1306<Rotate90>::WriteData(uint8_t* buffer, size_t buff_size)
    {
        if (isSPI)
        {
            // SPI mode data write
            spi_Access->WriteSPI(buffer, buff_size, csPin, mode);
        }
        else
        {
            // I2C mode data write handled in UpdateScreen method
            i2c_Access->I2C_Mem_Write(address, 0x40, 1, buffer, buff_size);
        }
    }

    template <bool Rotate90>
    float SSD1306<Rotate90>::DegToRad(float par_deg)
    {
        return par_deg * 3.14159265359f / 180.0f;
    }

    template <bool Rotate90>
    uint16_t SSD1306<Rotate90>::NormalizeTo0_360(uint16_t par_deg)
    {
        uint16_t loc_angle = par_deg % 360;
        if (loc_angle < 0)
            loc_angle += 360;

        return loc_angle;
    }
}

template class LowLevelEmbedded::Devices::Display::SSD1306<false>;
template class LowLevelEmbedded::Devices::Display::SSD1306<true>;