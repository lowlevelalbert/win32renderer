#include "display.h"

void DrawPixel(uint32_t x, uint32_t y, uint32_t color)
{
	if (x >= 0 && x < global_ClientAreaWidth && y >= 0 && y < global_ClientAreaHeight)
		global_ColorBuffer.memory[(global_ClientAreaWidth * y) + x] = color;
}

void DrawRect(int x, int y, int width, int height, uint32_t color)
{
	int current_x;
	int current_y;
    
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			current_y = y + j;
			current_x = x + i;
			DrawPixel(current_x, current_y, color);
		}
	}
}

void DrawGrid(void)
{
	// draw a grid of 10x10 pixels of grey 0xFFA3A3A3
	for (int y = 0; y < global_ClientAreaHeight; y+=10)
	{
		for (int x = 0; x < global_ClientAreaWidth; x+=10)
		{
            global_ColorBuffer.memory[(global_ClientAreaWidth * y) + x] = 0xFFA3A3A3;
		}
	}
}

void ClearColorBuffer(uint32_t color)
{
	for (int y = 0; y < global_ClientAreaHeight; y++)
	{
		for (int x = 0; x < global_ClientAreaWidth; x++)
		{
            global_ColorBuffer.memory[(global_ClientAreaWidth * y) + x] = color;
		}
	}
}

void PresentColorBuffer(void)
{
	StretchDIBits(DeviceContext,
                  0, 
                  0,
                  global_ClientAreaWidth, 
                  global_ClientAreaHeight,
                  0, 
                  0,
                  global_ClientAreaWidth, 
                  global_ClientAreaHeight,
                  global_ColorBuffer.memory,
                  &global_ColorBuffer.bmpInfo,
                  DIB_RGB_COLORS,
                  SRCCOPY);	
}

void ResizeColorBuffer()
{	
	
	global_ColorBuffer.bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	global_ColorBuffer.bmpInfo.bmiHeader.biWidth = global_ClientAreaWidth;
	global_ColorBuffer.bmpInfo.bmiHeader.biHeight = -global_ClientAreaHeight;
	global_ColorBuffer.bmpInfo.bmiHeader.biPlanes = 1;
	global_ColorBuffer.bmpInfo.bmiHeader.biBitCount = 32;
	global_ColorBuffer.bmpInfo.bmiHeader.biCompression = BI_RGB;
    
	if (global_ColorBuffer.memory)
	{
		VirtualFree(global_ColorBuffer.memory, 0, MEM_RELEASE);
	}
    
    size_t BufferSize = global_ClientAreaWidth * global_ClientAreaHeight * sizeof(uint32_t);
    
	global_ColorBuffer.memory = (uint32_t *) VirtualAlloc(0, BufferSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    
	if (!global_ColorBuffer.memory)
	{
		OutputDebugStringW(L"Error allocating memory for the color buffer");
	}
}


