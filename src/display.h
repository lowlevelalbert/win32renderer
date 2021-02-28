#ifndef DISPLAY_H
#define DISPLAY_H

#include <windows.h>
#include <stdbool.h>
#include <stdint.h>

#define FPS 30
#define FRAME_TARGET_TIME (1000000 / FPS) 

typedef struct {
	BITMAPINFO bmpInfo;
	uint32_t *memory;
} color_buffer_t; 

extern int global_ClientAreaWidth;
extern int global_ClientAreaHeight;
extern HDC DeviceContext;
extern color_buffer_t global_ColorBuffer;

void ClearColorBuffer(uint32_t color);
void DrawGrid(void);
void DrawPixel(uint32_t x, uint32_t y, uint32_t color);
void DrawRect(int x, int y, int width, int height, uint32_t color);
void PresentColorBuffer(void);
void ResizeColorBuffer(void);

#endif
