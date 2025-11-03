/**
 * This Library was originally written by Olivier Van den Eede (4ilo) in 2016.
 * Some refactoring was done and SPI support was added by Aleksander Alekseev (afiskon) in 2018.
 *
 * https://github.com/afiskon/stm32-ssd1306
 */
#include "SSD1306.h"
#include "LLE_I2C.h"
#include "LLE_SPI.h"
#include <math.h>
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

    SSD1306::SSD1306(II2CAccess* i2cPort, uint8_t address)
        : i2c_Access(i2cPort), spi_Access(nullptr), isSPI(false), address(address)
    {
        memset(Buffer, 0, SSD1306_BUFFER_SIZE);
        Display.CurrentX = 0;
        Display.CurrentY = 0;
        Display.Initialized = 0;
        Display.DisplayOn = 0;
    }

    SSD1306::SSD1306(ISPIAccess* spiPort, SPIMode mode, uint8_t csID)
        : i2c_Access(nullptr), spi_Access(spiPort), isSPI(true), csPin(csID), mode(mode)
    {
        memset(Buffer, 0, SSD1306_BUFFER_SIZE);
        Display.CurrentX = 0;
        Display.CurrentY = 0;
        Display.Initialized = 0;
        Display.DisplayOn = 0;
    }

    void SSD1306::Init()
    {
        // // Reset OLED if reset pin is provided
        // if (resetPin)
        // {
        //     Reset();
        // }

        // Wait for the screen to boot
        // HAL_Delay(100);

        // Init OLED
        WriteCommand(SSD1306_COMMAND_DISPLAY_OFF);                                   // 0xAE

        WriteCommand(SSD1306_COMMAND_SET_DISPLAY_CLOCK_DIV);                         // 0xD5
        WriteCommand(0x80);                                                          // The suggested ratio 0x80

        WriteCommand(SSD1306_COMMAND_SET_MULTIPLEX);                                 // 0xA8
        WriteCommand(SSD1306_HEIGHT - 1);

        WriteCommand(SSD1306_COMMAND_SET_DISPLAY_OFFSET);                            // 0xD3
        WriteCommand(0x00);                                                          // No offset

        WriteCommand(SSD1306_COMMAND_SET_START_LINE | 0x00);                         // 0x40 line #0

        WriteCommand(SSD1306_COMMAND_CHARGE_PUMP);                                   // 0x8D
        WriteCommand(0x14);                                                          // Use internal DC-DC Boost

        //WriteCommand(SSD1306_COMMAND_MEMORY_MODE);                                   // 0x20
        //WriteCommand(0x00);                                                          // 0x0 horizontal addressing mode

        WriteCommand(SSD1306_COMMAND_SEG_REMAP | 0x01);                              // 0xA0 / 0xA1

        WriteCommand(SSD1306_COMMAND_COM_SCAN_DEC);                                  // 0xC8

        WriteCommand(SSD1306_COMMAND_SET_COM_PINS);                                  // 0xDA
        WriteCommand(0x12);

        WriteCommand(SSD1306_COMMAND_SET_CONTRAST);                                  // 0x81
        WriteCommand(0xCF);

        WriteCommand(SSD1306_COMMAND_SET_PRECHARGE);                                 // 0xD9
        WriteCommand(0x22);

        WriteCommand(SSD1306_COMMAND_SET_VCOM_DETECT);                               // 0xDB
        WriteCommand(0x30);

        WriteCommand(SSD1306_COMMAND_ENTIRE_DISPLAY_ON_RESUME);                      // 0xA4

        WriteCommand(SSD1306_COMMAND_NORMAL_DISPLAY);                                // 0xA6

        WriteCommand(SSD1306_COMMAND_DISPLAY_ON);                                    // 0xAF

        Display.CurrentX = 0;
        Display.CurrentY = 0;
        Display.Initialized = 1;
        Display.DisplayOn = 1;
    }

    void SSD1306::Fill(SSD1306_COLOR color)
    {
        // Fill buffer with selected color
        uint32_t i;
        for (i = 0; i < sizeof(Buffer); i++)
        {
            Buffer[i] = (color == Black) ? 0x00 : 0xFF;
        }
    }

    void SSD1306::UpdateScreen()
    {
        // Write data to controller memory
        if (Display.Initialized)
        {
            // Set column and page address range
            WriteCommand(SSD1306_COMMAND_COLUMN_ADDR);
            WriteCommand(0);
            WriteCommand(SSD1306_WIDTH - 1);

            WriteCommand(SSD1306_COMMAND_PAGE_ADDR);
            WriteCommand(0);
            WriteCommand(SSD1306_HEIGHT / 8 - 1);

            // Write buffer data
            if (isSPI)
            {
                // SPI write buffer
                WriteData(Buffer, SSD1306_BUFFER_SIZE);
            }
            else
            {
                // I2C requires control byte for data
                uint8_t buffer[SSD1306_BUFFER_SIZE + 1]; // +1 for control byte
                buffer[0] = SSD1306_CONTROL_BYTE_DATA_STREAM;
                memcpy(buffer + 1, Buffer, SSD1306_BUFFER_SIZE);

                // Write the buffer to I2C
                i2c_Access->I2C_WriteMethod(address, buffer, SSD1306_BUFFER_SIZE + 1);
            }
        }
    }

    void SSD1306::DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color)
    {
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

    char SSD1306::WriteChar(char ch, Display_Font_t Font, SSD1306_COLOR color)
    {
        // Check if character is valid
        if (ch < 32 || ch > 126)
            return 0;

        // Check if font is monospaced or proportional
        if (Font.char_width == nullptr)
        {
            // Monospaced font
            uint32_t i, b, j;

            // Calculate character position in font data array
            uint32_t font_data_position = (ch - 32) * Font.height;
            // Get character data
            const uint16_t* data = &Font.data[font_data_position];

            // Get height in bytes
            uint8_t height_bytes = (Font.height + 7) / 8;

            // Draw the character
            for (i = 0; i < Font.width; i++)
            {
                uint8_t line = 0;
                for (b = 0; b < height_bytes; b++)
                {
                    line |= data[b * Font.width + i] << (b * 8);
                }

                for (j = 0; j < Font.height; j++)
                {
                    if (line & (1 << j))
                    {
                        DrawPixel(Display.CurrentX + i, Display.CurrentY + j, color);
                    }
                    else
                    {
                        DrawPixel(Display.CurrentX + i, Display.CurrentY + j, (SSD1306_COLOR)!color);
                    }
                }
            }

            // The current position is now after the character
            Display.CurrentX += Font.width;

            // Return written char for validation
            return ch;
        }
        else
        {
            // Proportional font
            uint32_t i, b, j;

            // Calculate character position in font data array
            uint32_t font_data_position = 0;
            for (uint8_t k = 0; k < (ch - 32); k++)
            {
                font_data_position += Font.char_width[k];
            }

            // Get character width
            uint8_t char_width = Font.char_width[ch - 32];

            // Get height in bytes
            uint8_t height_bytes = (Font.height + 7) / 8;

            // Get character data
            const uint16_t* data = &Font.data[font_data_position];

            // Draw the character
            for (i = 0; i < char_width; i++)
            {
                uint8_t line = 0;
                for (b = 0; b < height_bytes; b++)
                {
                    line |= data[b * char_width + i] << (b * 8);
                }

                for (j = 0; j < Font.height; j++)
                {
                    if (line & (1 << j))
                    {
                        DrawPixel(Display.CurrentX + i, Display.CurrentY + j, color);
                    }
                    else
                    {
                        DrawPixel(Display.CurrentX + i, Display.CurrentY + j, (SSD1306_COLOR)!color);
                    }
                }
            }

            // The current position is now after the character
            Display.CurrentX += char_width;

            // Return written char for validation
            return ch;
        }
    }

    char SSD1306::WriteString(char* str, Display_Font_t Font, SSD1306_COLOR color)
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

    void SSD1306::SetCursor(uint8_t x, uint8_t y)
    {
        Display.CurrentX = x;
        Display.CurrentY = y;
    }

    void SSD1306::Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color)
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

    void SSD1306::DrawArc(uint8_t x, uint8_t y, uint8_t radius, uint16_t start_angle, uint16_t sweep, SSD1306_COLOR color)
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

    void SSD1306::DrawArcWithRadiusLine(uint8_t x, uint8_t y, uint8_t radius, uint16_t start_angle, uint16_t sweep, SSD1306_COLOR color)
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

    void SSD1306::DrawCircle(uint8_t par_x, uint8_t par_y, uint8_t par_r, SSD1306_COLOR color)
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

    void SSD1306::FillCircle(uint8_t par_x, uint8_t par_y, uint8_t par_r, SSD1306_COLOR par_color)
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

    void SSD1306::Polyline(const DISPLAY_VERTEX* par_vertex, uint16_t par_size, SSD1306_COLOR color)
    {
        if (par_size < 2)
            return;

        for (uint16_t i = 0; i < (par_size - 1); i++)
        {
            Line(par_vertex[i].x, par_vertex[i].y, par_vertex[i + 1].x, par_vertex[i + 1].y, color);
        }
    }

    void SSD1306::DrawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color)
    {
        Line(x1, y1, x2, y1, color);
        Line(x1, y1, x1, y2, color);
        Line(x2, y1, x2, y2, color);
        Line(x1, y2, x2, y2, color);
    }

    void SSD1306::FillRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color)
    {
        for (uint8_t y = y1; y <= y2; y++)
        {
            for (uint8_t x = x1; x <= x2; x++)
            {
                DrawPixel(x, y, color);
            }
        }
    }

    Display_Error_t SSD1306::InvertRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
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

    void SSD1306::DrawBitmap(uint8_t x, uint8_t y, const unsigned char* bitmap, uint8_t w, uint8_t h, SSD1306_COLOR color)
    {
        for (uint8_t j = 0; j < h; j++)
        {
            for (uint8_t i = 0; i < w; i++)
            {
                // If bit is set in bitmap, draw pixel
                if (bitmap[j * w + i] != 0x00)
                {
                    DrawPixel(x + i, y + j, color);
                }
            }
        }
    }

    void SSD1306::SetContrast(const uint8_t value)
    {
        WriteCommand(0x81);    // Set contrast control
        WriteCommand(value);
    }

    void SSD1306::SetDisplayOn(const bool on)
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

    bool SSD1306::GetDisplayOn() const
    {
        return Display.DisplayOn;
    }

    void SSD1306::Reset()
    {
        // TODO: Reset the display using the reset pin if provided
    }

    SSD1306_Error_t SSD1306::FillBuffer(uint8_t* buf, uint32_t len)
    {
        if (len != SSD1306_BUFFER_SIZE)
            return SSD1306_ERR;

        memcpy(Buffer, buf, len);
        return SSD1306_OK;
    }

    // Private methods
    void SSD1306::WriteCommand(uint8_t byte)
    {
        uint8_t buffer[2];
        buffer[0] = SSD1306_CONTROL_BYTE_CMD_SINGLE;
        buffer[1] = byte;
        if (isSPI)
        {
            // SPI mode command write
            spi_Access->WriteSPI(buffer, 2, csPin, mode);
        }
        else
        {
            // I2C mode command write
            i2c_Access->I2C_WriteMethod(address, buffer, 2);
        }
    }

    void SSD1306::WriteData(uint8_t* buffer, size_t buff_size)
    {
        if (isSPI)
        {
            // SPI mode data write
            spi_Access->WriteSPI(buffer, buff_size, csPin, mode);
        }
        else
        {
            // I2C mode data write handled in UpdateScreen method
            i2c_Access->I2C_WriteMethod(address, buffer, buff_size);
        }
    }

    float SSD1306::DegToRad(float par_deg)
    {
        return par_deg * 3.14159265359f / 180.0f;
    }

    uint16_t SSD1306::NormalizeTo0_360(uint16_t par_deg)
    {
        uint16_t loc_angle = par_deg % 360;
        if (loc_angle < 0)
            loc_angle += 360;

        return loc_angle;
    }
}