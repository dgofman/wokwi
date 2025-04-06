/**
 ******************************************************************************
 * @file    OledDriver.c
 * @author  David Gofman
 ******************************************************************************
 **/

#include "OledDriver.h"

// Pin mapping structure to associate GPIO ports and pins
typedef struct
{
    GPIO_TypeDef *port;
    uint32_t mask;
} PinMapping;

// Define GPIO pins for STM32F0 or STM32F4
#ifdef STM32C031xx
#define GPIOF_SUPPORTED
#endif

#ifdef STM32F4
#define GPIOF_NOT_SUPPORTED
#endif

// Array of GPIO pin mappings for different microcontroller pins
const PinMapping digitalPinMappings[] = {
    {GPIOB, GPIO_PIN_7},  // PB_7
    {GPIOB, GPIO_PIN_6},  // PB_6
    {GPIOA, GPIO_PIN_10}, // PA_10
    {GPIOB, GPIO_PIN_3},  // PB_3
    {GPIOB, GPIO_PIN_10}, // PB_10
    {GPIOB, GPIO_PIN_4},  // PB_4
    {GPIOB, GPIO_PIN_5},  // PB_5
    {GPIOA, GPIO_PIN_15}, // PA_15
    {GPIOA, GPIO_PIN_9},  // PA_9
    {GPIOC, GPIO_PIN_7},  // PC_7
    {GPIOB, GPIO_PIN_0},  // PB_0
    {GPIOA, GPIO_PIN_7},  // PA_7
    {GPIOA, GPIO_PIN_6},  // PA_6
    {GPIOA, GPIO_PIN_5},  // PA_5
    {GPIOB, GPIO_PIN_9},  // PB_9
    {GPIOB, GPIO_PIN_8},  // PB_8
    {GPIOD, GPIO_PIN_0},  // PD_0
    {GPIOD, GPIO_PIN_2},  // PD_2
    {GPIOA, GPIO_PIN_14}, // PA_14
    {GPIOA, GPIO_PIN_13}, // PA_13
    {GPIOC, GPIO_PIN_6},  // PC_6
    {GPIOC, GPIO_PIN_13}, // PC_13
    {GPIOC, GPIO_PIN_14}, // PC_14
#ifdef GPIOF_SUPPORTED
    {GPIOF, GPIO_PIN_0}, // PF_0
    {GPIOF, GPIO_PIN_1}, // PF_1
#endif
    {GPIOB, GPIO_PIN_11}, // PB_11
    {GPIOA, GPIO_PIN_2},  // PA_2
    {GPIOD, GPIO_PIN_1},  // PD_1
    {GPIOD, GPIO_PIN_3},  // PD_3
    {GPIOC, GPIO_PIN_11}, // PC_11
    {GPIOA, GPIO_PIN_0},  // PA_0
    {GPIOA, GPIO_PIN_1},  // PA_1
    {GPIOA, GPIO_PIN_4},  // PA_4
    {GPIOB, GPIO_PIN_1},  // PB_1
    {GPIOA, GPIO_PIN_11}, // PA_11
    {GPIOA, GPIO_PIN_12}, // PA_12
    {GPIOA, GPIO_PIN_3},  // PA_3
    {GPIOC, GPIO_PIN_15}, // PC_15
    {GPIOB, GPIO_PIN_12}, // PB_12
    {GPIOB, GPIO_PIN_2},  // PB_2
#ifdef GPIOF_SUPPORTED
    {GPIOF, GPIO_PIN_3}, // PF_3
#endif
    {GPIOA, GPIO_PIN_8},  // PA_8
    {GPIOB, GPIO_PIN_15}, // PB_15
    {GPIOB, GPIO_PIN_14}, // PB_14
    {GPIOB, GPIO_PIN_13}, // PB_13
#ifdef GPIOF_SUPPORTED
    {GPIOF, GPIO_PIN_2} // PF_2
#endif
};

// Function to set the state of a digital pin (HIGH/LOW)
void digitalWrite(uint32_t ulPin, uint32_t ulVal)
{
    const PinMapping pinMap = digitalPinMappings[ulPin];
    GPIO_TypeDef *port = pinMap.port;
    uint32_t mask = pinMap.mask;

    // Set or reset the pin based on the value (HIGH or LOW)
    if (ulVal)
    {
        LL_GPIO_SetOutputPin(port, mask); // Set pin HIGH
    }
    else
    {
        LL_GPIO_ResetOutputPin(port, mask); // Set pin LOW
    }
}

// Function to configure the mode of a digital pin (INPUT/OUTPUT)
void pinMode(uint32_t ulPin, uint32_t ulMode)
{
    const PinMapping pinMap = digitalPinMappings[ulPin];
    GPIO_TypeDef *port = pinMap.port;
    uint32_t mask = pinMap.mask;

    if (ulMode == OUTPUT)
    {
        LL_GPIO_SetPinMode(port, mask, LL_GPIO_MODE_OUTPUT);           // Set pin as output
        LL_GPIO_SetPinOutputType(port, mask, LL_GPIO_OUTPUT_PUSHPULL); // Push-pull output type
    }
    else if (ulMode == INPUT)
    {
        LL_GPIO_SetPinMode(port, mask, LL_GPIO_MODE_INPUT); // Set pin as input
        LL_GPIO_SetPinPull(port, mask, LL_GPIO_PULL_NO);    // No pull-up/down resistors
    }
}

// Function to create a delay in microseconds
void delayMicroseconds(uint32_t us)
{
    __IO uint32_t currentTicks = SysTick->VAL;
    const uint32_t tickPerMs = SysTick->LOAD + 1;
    const uint32_t nbTicks = ((us - ((us > 0) ? 1 : 0)) * tickPerMs) / 1000;
    uint32_t elapsedTicks = 0;
    __IO uint32_t oldTicks = currentTicks;

    // Wait until the desired number of microseconds has elapsed
    do
    {
        currentTicks = SysTick->VAL;
        elapsedTicks += (oldTicks < currentTicks) ? tickPerMs + oldTicks - currentTicks : oldTicks - currentTicks;
        oldTicks = currentTicks;
    } while (nbTicks > elapsedTicks);
}

// Function to send data or command to the OLED display
void oled_send(uint8_t value, uint8_t mode)
{
    digitalWrite(oled.rs_pin, mode); // Set RS pin for data or command mode
    if (oled.rw_pin != 255)
    {
        digitalWrite(oled.rw_pin, LOW); // Set RW pin to LOW for write operation
    }
    oled_write4bits(value >> 4); // Send the higher 4 bits
    oled_write4bits(value);      // Send the lower 4 bits
}

// Function to send 4 bits of data to the OLED display
void oled_write4bits(uint8_t value)
{
    for (int i = 0; i < 4; i++)
    {
        digitalWrite(oled.data_pins[i], (value >> i) & 0x01); // Write each bit to data pins
    }
    oled_pulseEnable(); // Pulse the enable pin to latch the data
}

// Function to pulse the enable pin to latch data on the OLED display
void oled_pulseEnable(void)
{
    digitalWrite(oled.enable_pin, LOW);  // Set enable pin to LOW
    delayMicroseconds(1);                // Short delay
    digitalWrite(oled.enable_pin, HIGH); // Set enable pin to HIGH
    delayMicroseconds(1);                // Short delay
    digitalWrite(oled.enable_pin, LOW);  // Set enable pin to LOW again
    delayMicroseconds(100);              // Short delay for stable operation
}

// Function to send a command to the OLED display
void oled_command(uint8_t value)
{
    oled_send(value, LOW); // Send the command value with mode set to LOW (command)
}
