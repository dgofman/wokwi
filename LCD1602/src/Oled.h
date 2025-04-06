/**
 ******************************************************************************
 * @file    Oled.h
 * @author  David Gofman
 ******************************************************************************
 **/

#ifndef OLED_H
#define OLED_H

#include <stdint.h>

// Function declarations for initializing and controlling the OLED
void OLED_Init(void);
void OLED_SetCursor(int x, int y);
void OLED_DrawChar(int x, int y, char c);
void OLED_DrawString(const char *string);
void OLED_Clear(void);
void OLED_On(void);
void OLED_Off(void);
void OLED_begin(uint8_t cols, uint8_t lines, uint8_t charsize);

#endif // OLED_H
