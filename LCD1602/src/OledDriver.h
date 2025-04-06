/**
 ******************************************************************************
 * @file    OledDriver.h
 * @author  David Gofman
 ******************************************************************************
 **/

#ifndef OLED_DRIVER_H
#define OLED_DRIVER_H

#include <stdint.h>

#ifdef STM32C031xx
#include "stm32c0xx_ll_gpio.h"
#endif

#ifdef STM32F4
#include "stm32f4xx_ll_gpio.h"
#endif

// Define HIGH and LOW states
#define HIGH 0x1
#define LOW 0x0
#define INPUT 0x0
#define OUTPUT 0x1

// Enum for OLED commands and settings
typedef enum
{
    LCD_5x8DOTS = 0x00,             // 5x8 dot matrix font
    LCD_5x10DOTS = 0x04,            // 5x10 dot matrix font
    LCD_4BITMODE = 0x00,            // 4-bit mode (data width)
    LCD_1LINE = 0x00,               // 1-line display mode
    LCD_2LINE = 0x08,               // 2-line display mode
    LCD_DISPLAYCONTROL = 0x08,      // Control display settings
    LCD_CLEARDISPLAY = 0x01,        // Clear display command
    LCD_DISPLAYON = 0x04,           // Display ON command
    LCD_CURSOROFF = 0x00,           // Cursor OFF command
    LCD_BLINKOFF = 0x00,            // No blink for the cursor
    LCD_ENTRYSHIFTDECREMENT = 0x00, // No shift for the entry cursor
    LCD_ENTRYLEFT = 0x02,           // Left to right entry mode
    LCD_ENTRYMODESET = 0x04,        // Entry mode settings
    LCD_FUNCTIONSET = 0x20,         // Function set command
    LCD_SETDDRAMADDR = 0x80,        // Set DDRAM address command
} OledCommand;

// Struct to hold OLED display settings
typedef struct
{
    uint8_t rs_pin;           // Register Select pin
    uint8_t rw_pin;           // Read/Write pin (optional)
    uint8_t enable_pin;       // Enable pin
    uint8_t data_pins[8];     // Data pins for communication
    uint8_t display_function; // Display configuration (e.g., number of lines)
    uint8_t display_control;  // Display control settings
    uint8_t display_mode;     // Entry mode settings
    uint8_t initialized;      // Flag to indicate if OLED is initialized
    uint8_t num_lines;        // Number of lines on the display
    uint8_t row_offsets[4];   // Offsets for each row
} OLED;

// External declaration of the global OLED instance
extern OLED oled;

// Function declarations for OLED driver
void digitalWrite(uint32_t ulPin, uint32_t ulVal);
void pinMode(uint32_t ulPin, uint32_t ulMode);
void delayMicroseconds(uint32_t us);

void oled_send(uint8_t value, uint8_t mode);
void oled_write4bits(uint8_t value);
void oled_pulseEnable(void);
void oled_command(uint8_t value);

#endif // OLED_DRIVER_H
