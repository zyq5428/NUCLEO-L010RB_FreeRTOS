/*
 * SSD1306.c
 *
 *  Created on: Nov 19, 2022
 *
 */
#include <easy_log.h>
#include "ssd1306.h"

/* Absolute value */
#define ABS(x)   ((x) > 0 ? (x) : -(x))

/* SSD1306 data buffer */
static uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];
// Screen object
static SSD1306_t SSD1306;

/* Write command */
HAL_StatusTypeDef SSD1306_WriteCommand(SI2C_Handle handle, uint8_t command)
{
	uint8_t tx_buf[2] = {SSD1306_CMD, 0x00};

	tx_buf[1] = command;
	if( IIC_Send_Multi_Byte(handle, tx_buf, 2) )
		return HAL_ERROR;
	else
		return HAL_OK;
}

/* Write data */
HAL_StatusTypeDef SSD1306_WriteData(SI2C_Handle handle, uint8_t data)
{
	uint8_t tx_buf[2] = {SSD1306_DATA, 0x00};

	tx_buf[1] = data;
	if( IIC_Send_Multi_Byte(handle, tx_buf, 2) )
		return HAL_ERROR;
	else
		return HAL_OK;
}

void SSD1306_Fill(SSD1306_COLOR_t color)
{
    /* Fill screenbuffer with a constant value (color) */
	uint32_t i;
	for(i = 0; i < sizeof(SSD1306_Buffer); i++)
	{
		SSD1306_Buffer[i] = (color == SSD1306_COLOR_BLACK) ? 0x00 : 0xFF;
	}
}

void SSD1306_UpdateScreen(SI2C_Handle handle)
{
	uint8_t i;
    for (i = 0; i < 8; i++)
    {
    	SSD1306_WriteCommand(handle, 0xB0 + i);
        SSD1306_WriteCommand(handle, 0x02);
        SSD1306_WriteCommand(handle, 0x10);

        IIC_Mem_Write(handle, 0x40, &SSD1306_Buffer[SSD1306_WIDTH * i], SSD1306_WIDTH);
    }
}

/* SSD1306 Init */
uint8_t OLED_Init(SI2C_Handle handle)
{
	/* Check if LCD connected to I2C */
	if(!(IIC_Check_Device(handle, 3)))
	{
		printf("SSD1306 is ready\r\n");
	} else {
		printf("SSD1306 is error\r\n");
	}
	/* A little delay */
	HAL_Delay(100);

	/* Init OLED */
    SSD1306_WriteCommand(handle, 0xAE); //display off
    SSD1306_WriteCommand(handle, 0x20); //Set Memory Addressing Mode
    SSD1306_WriteCommand(handle, 0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    SSD1306_WriteCommand(handle, 0xB0); //Set Page Start Address for Page Addressing Mode,0-7
    SSD1306_WriteCommand(handle, 0xC8); //Set COM Output Scan Direction
    SSD1306_WriteCommand(handle, 0x00); //---set low column address
    SSD1306_WriteCommand(handle, 0x10); //---set high column address
    SSD1306_WriteCommand(handle, 0x40); //--set start line address
    SSD1306_WriteCommand(handle, 0x81); //--set contrast control register
    SSD1306_WriteCommand(handle, 0xFF);
    SSD1306_WriteCommand(handle, 0xA1); //--set segment re-map 0 to 127
    SSD1306_WriteCommand(handle, 0xA6); //--set normal display
    SSD1306_WriteCommand(handle, 0xA8); //--set multiplex ratio(1 to 64)
    SSD1306_WriteCommand(handle, 0x3F); //
    SSD1306_WriteCommand(handle, 0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    SSD1306_WriteCommand(handle, 0xD3); //-set display offset
    SSD1306_WriteCommand(handle, 0x00); //-not offset
    SSD1306_WriteCommand(handle, 0xD5); //--set display clock divide ratio/oscillator frequency
    SSD1306_WriteCommand(handle, 0xF0); //--set divide ratio
    SSD1306_WriteCommand(handle, 0xD9); //--set pre-charge period
    SSD1306_WriteCommand(handle, 0x22); //
    SSD1306_WriteCommand(handle, 0xDA); //--set com pins hardware configuration
    SSD1306_WriteCommand(handle, 0x12);
    SSD1306_WriteCommand(handle, 0xDB); //--set vcomh
    SSD1306_WriteCommand(handle, 0x20); //0x20,0.77xVcc
    SSD1306_WriteCommand(handle, 0x8D); //--set DC-DC enable
    SSD1306_WriteCommand(handle, 0x14); //
    SSD1306_WriteCommand(handle, 0xAF); //--turn on SSD1306 panel

    /* Clear screen */
    SSD1306_Fill(SSD1306_COLOR_BLACK);

    /* Update screen */
    SSD1306_UpdateScreen(handle);

    /* Set default values */
    SSD1306.CurrentX = 0;
    SSD1306.CurrentY = 0;

    /* Initialized OK */
    SSD1306.Initialized = 1;

    /* Return OK */
    return HAL_OK;
}

/*
 * Draw one pixel in the screenbuffer
 * X => X Coordinate
 * Y => Y Coordinate
 * color => Pixel color
 */
uint8_t SSD1306_DrawPixel(uint16_t x, uint16_t y, SSD1306_COLOR_t color)
{
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT)
    {
        // Don't write outside the buffer
        return HAL_ERROR;
    }

    // Check if pixel should be inverted
    if (SSD1306.Inverted)
    {
        color = (SSD1306_COLOR_t)!color;
    }

    // Draw in the correct color
    if (color == SSD1306_COLOR_WHITE)
    {
        SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
    }
    else
    {
        SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
    }

    return HAL_OK;
}

/*
 * Draw 1 char to the screen buffer
 * ch      => Character to write
 * Font    => Font to use
 * color => Pixel color
 */
char SSD1306_WriteChar(char ch, FontDef Font, SSD1306_COLOR_t color)
{
    uint32_t i, b, j;

    // Check remaining space on current line
    if (SSD1306_WIDTH <= (SSD1306.CurrentX + Font.FontWidth) ||
        SSD1306_HEIGHT <= (SSD1306.CurrentY + Font.FontHeight))
    {
        // Not enough space on current line
        return HAL_ERROR;
    }

    // Translate font to screenbuffer
    for (i = 0; i < Font.FontHeight; i++)
    {
        b = Font.data[(ch - 32) * Font.FontHeight + i];
        for (j = 0; j < Font.FontWidth; j++)
        {
            if ((b << j) & 0x8000)
            {
            	SSD1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR_t) color);
            }
            else
            {
            	SSD1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR_t)!color);
            }
        }
    }

    // The current space is now taken
    SSD1306.CurrentX += Font.FontWidth;

    // Return written char for validation
    return ch;
}

/*
 * Write full string to screenbuffer
 */
char SSD1306_WriteString(char* str, FontDef Font, SSD1306_COLOR_t color)
{
    // Write until null-byte
    while (*str)
    {
        if (SSD1306_WriteChar(*str, Font, color) != *str)
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

/*
 * Invert background/foreground colors
 */
void SSD1306_InvertColors(void)
{
    SSD1306.Inverted = !SSD1306.Inverted;
}

/*
 * Set cursor position
 */
void SSD1306_SetCursor(uint8_t x, uint8_t y)
{
    SSD1306.CurrentX = x;
    SSD1306.CurrentY = y;
}

/*
 * Draw 1 Line to the screen buffer
 * x0,y0      First Cursor
 * x1,y1      Second Cursor
 * color => Pixel color
 */
uint8_t SSD1306_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, SSD1306_COLOR_t color)
{
    int16_t dx, dy, sx, sy, err, e2, i, tmp;

    /* Check for overflow */
    if (x0 >= SSD1306_WIDTH) {
        x0 = SSD1306_WIDTH - 1;
    }
    if (x1 >= SSD1306_WIDTH) {
        x1 = SSD1306_WIDTH - 1;
    }
    if (y0 >= SSD1306_HEIGHT) {
        y0 = SSD1306_HEIGHT - 1;
    }
    if (y1 >= SSD1306_HEIGHT) {
        y1 = SSD1306_HEIGHT - 1;
    }

    dx = (x0 < x1) ? (x1 - x0) : (x0 - x1);
    dy = (y0 < y1) ? (y1 - y0) : (y0 - y1);
    sx = (x0 < x1) ? 1 : -1;
    sy = (y0 < y1) ? 1 : -1;
    err = ((dx > dy) ? dx : -dy) / 2;

    if (dx == 0) {
        if (y1 < y0) {
            tmp = y1;
            y1 = y0;
            y0 = tmp;
        }

        if (x1 < x0) {
            tmp = x1;
            x1 = x0;
            x0 = tmp;
        }

        /* Vertical line */
        for (i = y0; i <= y1; i++) {
            SSD1306_DrawPixel(x0, i, color);
        }

        /* Return from function */
        return HAL_OK;
    }

    if (dy == 0) {
        if (y1 < y0) {
            tmp = y1;
            y1 = y0;
            y0 = tmp;
        }

        if (x1 < x0) {
            tmp = x1;
            x1 = x0;
            x0 = tmp;
        }

        /* Horizontal line */
        for (i = x0; i <= x1; i++) {
            SSD1306_DrawPixel(i, y0, color);
        }

        /* Return from function */
        return HAL_OK;
    }

    while (1) {
        SSD1306_DrawPixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) {
            break;
        }
        e2 = err;
        if (e2 > -dx) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy) {
            err += dx;
            y0 += sy;
        }
    }

    return HAL_OK;
}

/*
 * Draw Rectangle to the screen buffer
 * x,y      Start Cursor
 * w,h      WIDTH and HEIGHT
 * color => Pixel color
 */
uint8_t SSD1306_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t color)
{
    /* Check input parameters */
    if (
        x >= SSD1306_WIDTH ||
        y >= SSD1306_HEIGHT
    ) {
        /* Return error */
        return HAL_ERROR;
    }

    /* Check width and height */
    if ((x + w) >= SSD1306_WIDTH) {
        w = SSD1306_WIDTH - x;
    }
    if ((y + h) >= SSD1306_HEIGHT) {
        h = SSD1306_HEIGHT - y;
    }

    /* Draw 4 lines */
    SSD1306_DrawLine(x, y, x + w, y, color);         /* Top line */
    SSD1306_DrawLine(x, y + h, x + w, y + h, color); /* Bottom line */
    SSD1306_DrawLine(x, y, x, y + h, color);         /* Left line */
    SSD1306_DrawLine(x + w, y, x + w, y + h, color); /* Right line */

    return HAL_OK;
}

/*
 * Fill Rectangle to the screen buffer
 * x,y      Start Cursor
 * w,h      WIDTH and HEIGHT
 * color => Pixel color
 */
uint8_t SSD1306_DrawFillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t color)
{
    uint8_t i;

    /* Check input parameters */
    if (
        x >= SSD1306_WIDTH ||
        y >= SSD1306_HEIGHT
    ) {
        /* Return error */
    	return HAL_ERROR;
    }

    /* Check width and height */
    if ((x + w) >= SSD1306_WIDTH) {
        w = SSD1306_WIDTH - x;
    }
    if ((y + h) >= SSD1306_HEIGHT) {
        h = SSD1306_HEIGHT - y;
    }

    /* Draw lines */
    for (i = 0; i <= h; i++) {
        /* Draw lines */
        SSD1306_DrawLine(x, y + i, x + w, y + i, color);
    }

    return HAL_OK;
}

/*
 * Draw Triangle to the screen buffer
 * x1,y1      First Cursor
 * x2,y2      Second Cursor
 * x3,y3      Third Cursor
 * color => Pixel color
 */
void SSD1306_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SSD1306_COLOR_t color)
{
    /* Draw lines */
    SSD1306_DrawLine(x1, y1, x2, y2, color);
    SSD1306_DrawLine(x2, y2, x3, y3, color);
    SSD1306_DrawLine(x3, y3, x1, y1, color);
}

void SSD1306_DrawFillTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SSD1306_COLOR_t color)
{
    int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
            yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
            curpixel = 0;

    deltax = ABS(x2 - x1);
    deltay = ABS(y2 - y1);
    x = x1;
    y = y1;

    if (x2 >= x1) {
        xinc1 = 1;
        xinc2 = 1;
    } else {
        xinc1 = -1;
        xinc2 = -1;
    }

    if (y2 >= y1) {
        yinc1 = 1;
        yinc2 = 1;
    } else {
        yinc1 = -1;
        yinc2 = -1;
    }

    if (deltax >= deltay) {
        xinc1 = 0;
        yinc2 = 0;
        den = deltax;
        num = deltax / 2;
        numadd = deltay;
        numpixels = deltax;
    } else {
        xinc2 = 0;
        yinc1 = 0;
        den = deltay;
        num = deltay / 2;
        numadd = deltax;
        numpixels = deltay;
    }

    for (curpixel = 0; curpixel <= numpixels; curpixel++) {
        SSD1306_DrawLine(x, y, x3, y3, color);

        num += numadd;
        if (num >= den) {
            num -= den;
            x += xinc1;
            y += yinc1;
        }
        x += xinc2;
        y += yinc2;
    }
}

void SSD1306_DrawCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t color)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    SSD1306_DrawPixel(x0, y0 + r, color);
    SSD1306_DrawPixel(x0, y0 - r, color);
    SSD1306_DrawPixel(x0 + r, y0, color);
    SSD1306_DrawPixel(x0 - r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        SSD1306_DrawPixel(x0 + x, y0 + y, color);
        SSD1306_DrawPixel(x0 - x, y0 + y, color);
        SSD1306_DrawPixel(x0 + x, y0 - y, color);
        SSD1306_DrawPixel(x0 - x, y0 - y, color);

        SSD1306_DrawPixel(x0 + y, y0 + x, color);
        SSD1306_DrawPixel(x0 - y, y0 + x, color);
        SSD1306_DrawPixel(x0 + y, y0 - x, color);
        SSD1306_DrawPixel(x0 - y, y0 - x, color);
    }
}

void SSD1306_DrawFillCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t color)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    SSD1306_DrawPixel(x0, y0 + r, color);
    SSD1306_DrawPixel(x0, y0 - r, color);
    SSD1306_DrawPixel(x0 + r, y0, color);
    SSD1306_DrawPixel(x0 - r, y0, color);
    SSD1306_DrawLine(x0 - r, y0, x0 + r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        SSD1306_DrawLine(x0 - x, y0 + y, x0 + x, y0 + y, color);
        SSD1306_DrawLine(x0 + x, y0 - y, x0 - x, y0 - y, color);

        SSD1306_DrawLine(x0 + y, y0 + x, x0 - y, y0 + x, color);
        SSD1306_DrawLine(x0 + y, y0 - x, x0 - y, y0 - x, color);
    }
}

void SSD1306_ON(SI2C_Handle handle)
{
    SSD1306_WriteCommand(handle, 0x8D);
    SSD1306_WriteCommand(handle, 0x14);
    SSD1306_WriteCommand(handle, 0xAF);
}
void SSD1306_OFF(SI2C_Handle handle)
{
    SSD1306_WriteCommand(handle, 0x8D);
    SSD1306_WriteCommand(handle, 0x10);
    SSD1306_WriteCommand(handle, 0xAE);
}

void SSD1306_Monochrome(SI2C_Handle handle, SSD1306_COLOR_t color)
{
	SSD1306_ON(handle);
	SSD1306_Fill(color);
	SSD1306_UpdateScreen(handle);
}