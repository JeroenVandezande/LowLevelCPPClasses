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
        std::function<void(uint32_t delay)> Delay_ms;
        virtual void Init();
        void Fill(DisplayColorType color);
        void UpdateScreen();
        void DrawPixel(uint8_t x, uint8_t y, DisplayColorType color);
        char WriteChar(char ch, Display_Font_t Font, DisplayColorType color);
        char WriteString(char* str, Display_Font_t Font, DisplayColorType color);
        void SetCursor(uint8_t x, uint8_t y);
        void Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, DisplayColorType color);
        void DrawArc(uint8_t x, uint8_t y, uint8_t radius, uint16_t start_angle, uint16_t sweep,
                     DisplayColorType color);
        void DrawArcWithRadiusLine(uint8_t x, uint8_t y, uint8_t radius, uint16_t start_angle,
                                   uint16_t sweep, DisplayColorType color);
        void DrawCircle(uint8_t par_x, uint8_t par_y, uint8_t par_r, DisplayColorType color);
        void FillCircle(uint8_t par_x, uint8_t par_y, uint8_t par_r, DisplayColorType par_color);
        void Polyline(const DISPLAY_VERTEX* par_vertex, uint16_t par_size, DisplayColorType color);
        void DrawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, DisplayColorType color);
        void FillRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, DisplayColorType color);

        /**
                 * @brief Invert color of pixels in rectangle (include border)
                 *
                 * @param x1 X Coordinate of top left corner
                 * @param y1 Y Coordinate of top left corner
                 * @param x2 X Coordinate of bottom right corner
                 * @param y2 Y Coordinate of bottom right corner
                 * @return SSD1306_Error_t status
                 */
        Display_Error_t InvertRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);

        void DrawBitmap(uint8_t x, uint8_t y, const unsigned char* bitmap, uint8_t w, uint8_t h, DisplayColorType color);

        /**
                 * @brief Sets the contrast of the display.
                 * @param[in] value contrast to set.
                 * @note Contrast increases as the value increases.
                 * @note RESET = 7Fh.
                 */
        void SetContrast(const uint8_t value);

        void SetDisplayOn(const bool on);

        bool GetDisplayOn() const;

        virtual void Reset()
        {
        };
    };
}
