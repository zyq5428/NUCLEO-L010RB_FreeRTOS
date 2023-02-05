/*
 * SSD1306.h
 *
 *  Created on: Nov 19, 2022
 *
 */

#ifndef SSD1306_H_
#define SSD1306_H_

#include "system.h"
#include "fonts.h"
#include <Drivers/Simulation/SI2C/SI2C_Device.h>

/**
 * This SSD1306 LCD uses I2C for communication
 *
 * Library features functions for drawing lines, rectangles and circles.
 *
 * It also allows you to draw texts and characters using appropriate functions provided in library.
 *
 * \par Default pinout
 *
SSD1306    |STM32L4xx    |DESCRIPTION

VCC        |3.3V         |
GND        |GND          |
SCL        |PB8          |Serial clock line
SDA        |PB9          |Serial data line
 */

/**
 *
 * \par Select custom I2C settings
 *
 */
#define SSD1306_I2C				I2C1

//Select custom I2C address
#define SSD1306_I2C_ADDR		0x3C
#define SSD1306_CMD				0x00
#define SSD1306_DATA			0x40

//Select custom width and height if your LCD differs in size
#define SSD1306_WIDTH			128
#define SSD1306_HEIGHT			64

/**
 * @brief  SSD1306 color enumeration
 */
typedef enum {
    SSD1306_COLOR_BLACK = 0x00, /*!< Black color, no pixel */
    SSD1306_COLOR_WHITE = 0x01  /*!< Pixel is set. Color depends on LCD */
} SSD1306_COLOR_t;

/* Private SSD1306 structure */
typedef struct {
    uint16_t CurrentX;
    uint16_t CurrentY;
    uint8_t Inverted;
    uint8_t Initialized;
} SSD1306_t;

extern uint8_t OLED_Init(SI2C_Handle handle);
extern void SSD1306_Fill(SSD1306_COLOR_t color);
extern void SSD1306_UpdateScreen(SI2C_Handle handle);
extern uint8_t SSD1306_DrawPixel(uint16_t x, uint16_t y, SSD1306_COLOR_t color);
extern char SSD1306_WriteChar(char ch, FontDef Font, SSD1306_COLOR_t color);
extern char SSD1306_WriteString(char* str, FontDef Font, SSD1306_COLOR_t color);
extern void SSD1306_InvertColors(void);
extern void SSD1306_SetCursor(uint8_t x, uint8_t y);
extern uint8_t SSD1306_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, SSD1306_COLOR_t color);
extern uint8_t SSD1306_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t color);
extern uint8_t SSD1306_DrawFillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t color);
extern void SSD1306_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SSD1306_COLOR_t color);
extern void SSD1306_DrawFillTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SSD1306_COLOR_t color);
extern void SSD1306_DrawCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t color);
extern void SSD1306_DrawFillCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t color);
extern void SSD1306_ON(SI2C_Handle handle);
extern void SSD1306_OFF(SI2C_Handle handle);
extern void SSD1306_Monochrome(SI2C_Handle handle, SSD1306_COLOR_t color);

#endif /* SSD1306_H_ */
