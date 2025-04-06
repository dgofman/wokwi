/**
 ******************************************************************************
 * @file    main.c
 * @author  David Gofman
 ******************************************************************************
 **/

#include "Oled.h"

int main(void)
{
    OLED_Init();
    OLED_DrawString("Hello World!");
    OLED_SetCursor(4, 1);
    OLED_DrawString("2015");

    while (1)
    {
        // Keep running
    }

    return 0;
}
