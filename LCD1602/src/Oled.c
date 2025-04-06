/**
 ******************************************************************************
 * @file    Oled.c
 * @author  David Gofman
 ******************************************************************************
 **/

#include "Oled.h"
#include "OledDriver.h"

// Global instance of the OLED display
OLED oled;

// Function to initialize the OLED display
void OLED_Init(void)
{
    // Initialize pin numbers for OLED control
    oled.rs_pin = 12;                                               // Register Select pin
    oled.enable_pin = 11;                                           // Enable pin
    oled.data_pins[0] = 10;                                         // Data pin 0
    oled.data_pins[1] = 9;                                          // Data pin 1
    oled.data_pins[2] = 8;                                          // Data pin 2
    oled.data_pins[3] = 7;                                          // Data pin 3
    oled.display_function = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS; // 4-bit mode, 1-line display, 5x8 font
    oled.initialized = 0;                                           // Flag to indicate initialization status

    // Call OLED_begin for default 20x4 initialization
    OLED_begin(20, 4, LCD_5x8DOTS);
}

// Function to begin OLED display setup
void OLED_begin(uint8_t cols, uint8_t lines, uint8_t charsize)
{
    // Configure the number of lines on the OLED display
    if (lines > 1)
    {
        oled.display_function |= LCD_2LINE; // Enable 2-line display mode
    }
    oled.num_lines = lines;

    // Set row offsets based on the number of columns and lines
    oled.row_offsets[0] = 0x00;        // Offset for the first line
    oled.row_offsets[1] = 0x40;        // Offset for the second line
    oled.row_offsets[2] = 0x00 + cols; // Offset for the third line (if applicable)
    oled.row_offsets[3] = 0x40 + cols; // Offset for the fourth line (if applicable)

    // Set character size for the display
    if ((charsize != LCD_5x8DOTS) && (lines == 1))
    {
        oled.display_function |= LCD_5x10DOTS; // Use 5x10 dot size for single line display
    }

    // Set pin modes for control and data pins (assumed to be OUTPUT elsewhere)
    pinMode(oled.rs_pin, OUTPUT);
    if (oled.rw_pin != 255)
    {
        pinMode(oled.rw_pin, OUTPUT);
    }
    pinMode(oled.enable_pin, OUTPUT);
    for (int i = 0; i < 4; i++)
    {
        pinMode(oled.data_pins[i], OUTPUT); // Set data pins to OUTPUT mode
    }

    // Initial power-up delay
    delayMicroseconds(50000);
    digitalWrite(oled.rs_pin, LOW);
    digitalWrite(oled.enable_pin, LOW);
    if (oled.rw_pin != 255)
    {
        digitalWrite(oled.rw_pin, LOW);
    }

    // 4-bit initialization sequence
    oled_write4bits(0x03);
    delayMicroseconds(4500); // Wait for 4.5ms
    oled_write4bits(0x03);
    delayMicroseconds(4500); // Wait for another 4.5ms
    oled_write4bits(0x03);
    delayMicroseconds(150); // Wait for 150us
    oled_write4bits(0x02);  // Switch to 4-bit mode

    // Set display function (mode)
    oled_command(LCD_FUNCTIONSET | oled.display_function);

    // Turn on the display
    oled.display_control = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    OLED_On();

    // Clear the display
    OLED_Clear();

    // Set entry mode for cursor
    oled.display_mode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    oled_command(LCD_ENTRYMODESET | oled.display_mode);

    // Mark OLED as initialized
    oled.initialized = 1;
}

// Function to set the cursor position on the OLED display
void OLED_SetCursor(int x, int y)
{
    const size_t max_lines = sizeof(oled.row_offsets) / sizeof(*oled.row_offsets);

    // Ensure the y-coordinate is within valid range
    if (y >= max_lines)
        y = max_lines - 1;
    if (y >= oled.num_lines)
        y = oled.num_lines - 1;

    // Set the cursor position based on x and y coordinates
    oled_command(LCD_SETDDRAMADDR | (x + oled.row_offsets[y]));
}

// Function to draw a character on the OLED display
void OLED_DrawChar(int x, int y, char c)
{
    OLED_SetCursor(x, y); // Set cursor position
    oled_send(c, HIGH);   // Send character data in data mode
}

// Function to draw a string on the OLED display
void OLED_DrawString(const char *string)
{
    while (*string)
    {
        oled_send(*string++, HIGH); // Send each character in the string
    }
}

// Function to clear the OLED display
void OLED_Clear(void)
{
    oled_command(LCD_CLEARDISPLAY); // Send clear display command
    delayMicroseconds(2000);        // Wait for 2ms for clear to complete
}

// Function to turn on the OLED display
void OLED_On(void)
{
    oled.display_control |= LCD_DISPLAYON;                   // Set display on flag
    oled_command(LCD_DISPLAYCONTROL | oled.display_control); // Send command to update display control
}

// Function to turn off the OLED display
void OLED_Off(void)
{
    oled.display_control &= ~LCD_DISPLAYON;                  // Clear display on flag
    oled_command(LCD_DISPLAYCONTROL | oled.display_control); // Send command to update display control
}
