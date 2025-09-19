#include <cstdint>
#include <functional>

namespace LowLevelEmbedded
{
    typedef struct
    {
        const uint8_t width; /**< Font width in pixels */
        const uint8_t height; /**< Font height in pixels */
        const uint16_t* const data; /**< Pointer to font data array */
        const uint8_t* const char_width; /**< Proportional character width in pixels (NULL for monospaced) */
    } Display_Font_t;

    typedef struct
    {
        uint8_t x;
        uint8_t y;
    } DISPLAY_VERTEX;

    typedef enum
    {
        DISPLAY_OK = 0x00,
        DISPLAY_ERR = 0x01
    } Display_Error_t;


    template <class DisplayColorType>
    class IDisplay
    {
    public:
        using color_type = DisplayColorType;
        virtual void Init() = 0;
        virtual void Fill(color_type color) = 0;
        virtual void DrawPixel(uint8_t x, uint8_t y, color_type color) = 0;
        virtual char WriteChar(char ch, Display_Font_t Font, color_type color) = 0;
        virtual char WriteString(char* str, Display_Font_t Font, color_type color) = 0;
        virtual void SetCursor(uint8_t x, uint8_t y) = 0;
        virtual void Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, color_type color) = 0;
        virtual void DrawArc(uint8_t x, uint8_t y, uint8_t radius, uint16_t start_angle, uint16_t sweep,
                     color_type color) = 0;
        virtual void DrawArcWithRadiusLine(uint8_t x, uint8_t y, uint8_t radius, uint16_t start_angle,
                                   uint16_t sweep, color_type color) = 0;
        virtual void DrawCircle(uint8_t par_x, uint8_t par_y, uint8_t par_r, color_type color) = 0;
        virtual void FillCircle(uint8_t par_x, uint8_t par_y, uint8_t par_r, color_type par_color) = 0;
        virtual void Polyline(const DISPLAY_VERTEX* par_vertex, uint16_t par_size, color_type color) = 0;
        virtual void DrawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, color_type color) = 0;
        virtual void FillRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, color_type color) = 0;

        /**
                 * @brief Invert color of pixels in rectangle (include border)
                 *
                 * @param x1 X Coordinate of top left corner
                 * @param y1 Y Coordinate of top left corner
                 * @param x2 X Coordinate of bottom right corner
                 * @param y2 Y Coordinate of bottom right corner
                 * @return SSD1306_Error_t status
                 */
        virtual Display_Error_t InvertRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) = 0;

        virtual void DrawBitmap(uint8_t x, uint8_t y, const unsigned char* bitmap, uint8_t w, uint8_t h, color_type color) = 0;

        virtual void SetDisplayOn(const bool on) = 0;

        virtual bool GetDisplayOn() const = 0;

        virtual void Reset()
        {
        };
    };
}
