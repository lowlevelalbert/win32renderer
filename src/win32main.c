#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "display.h"
#include "vector.h"

#define N_POINTS (9 * 9 * 9)

LRESULT CALLBACK WindProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

///////////////////////////////////////////////
// GLOBALS 
///////////////////////////////////////////////
bool global_Running;

int global_ClientAreaWidth;

int global_ClientAreaHeight;

color_buffer_t global_ColorBuffer;

HDC DeviceContext;

double fov_factor = 640;

vec3_t cube_points[N_POINTS];
vec2_t projected_points[N_POINTS];

vec3_t camera_position = { .x = 0, .y = 0, .z = -5 };
vec3_t cube_rotation = { .x = 0, .y = 0, .z = 0 };

LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
LARGE_INTEGER Frequency;

///////////////////////////////////////////////
// A function that transform a 3d vector into a
// 2d vector projected on a plane
///////////////////////////////////////////////
vec2_t project(vec3_t point)
{
	vec2_t projected_point = {
		.x = (fov_factor * point.x) / point.z,
		.y = (fov_factor * point.y) / point.z
	};
	return projected_point;
}

void setup(void)
{
	int point_count = 0;
	// load the array of vectors
	for (double x = -1; x <= 1; x += 0.25)
	{	
		for (double y = -1; y <= 1; y += 0.25)
		{
			for (double z = -1; z <= 1; z += 0.25)
			{
				vec3_t current_point = { .x = x, .y = y, .z = z};
				cube_points[point_count++] = current_point;
			}
		}
	}
}

void update(void)
{
	QueryPerformanceCounter(&StartingTime);
	int time_to_wait = FRAME_TARGET_TIME - ElapsedMicroseconds.QuadPart;
    
	// set the FPS cap to 30 frames
	if (time_to_wait > 0 && time_to_wait < FRAME_TARGET_TIME)
	{
		time_to_wait /= 1000;
		Sleep(time_to_wait);
	}
	
	cube_rotation.x += 0.01;
	cube_rotation.y += 0.01;
	cube_rotation.z += 0.01;
    
	for (int i = 0; i < N_POINTS; i++)
	{
		vec3_t point = cube_points[i];
        
		vec3_t transformed_point = vec3_rotate_x(point, cube_rotation.x);
		transformed_point = vec3_rotate_y(transformed_point, cube_rotation.y);
		transformed_point = vec3_rotate_z(transformed_point, cube_rotation.z);
		transformed_point.z -= camera_position.z;
		vec2_t projected_point = project(transformed_point);
		projected_points[i] = projected_point;
	}
    
	QueryPerformanceCounter(&EndingTime);
	ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;
	ElapsedMicroseconds.QuadPart *= 100000;
	ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;
}

void render()
{
	ClearColorBuffer(0xFF000000);
    
	DrawGrid();
    
	// Draw each point of the cube projecting it in 2d
	// and translating it to the center of the screen.
	// Instead of pixels, draw each point(vertex) as 4x4 rectangle
	for (int i = 0; i < N_POINTS; i++)
	{
		vec2_t current_point = projected_points[i];

		DrawRect(current_point.x + (global_ClientAreaWidth / 2), current_point.y + (global_ClientAreaHeight / 2), 4, 4, 0xFFFFFF00);
	}
    
	PresentColorBuffer();
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR cmdline, int showcmd)
{
    
	// query the frequency of the performance counter
	QueryPerformanceFrequency(&Frequency);
    
	// register the window with the OS
	wchar_t* className = L"NTTWNDCLASS";
	HWND window = NULL;
	MSG msg = {0};
	WNDCLASSEXW wc = {0};
    
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WindProc;
	wc.lpszClassName = className;
	
	if (RegisterClassExW(&wc) == 0)
	{
		OutputDebugString(L"Error registering the window");
	}
	
	// create the window
	window = CreateWindowEx(0,
                            className,
                            L"Win32 Renderer",
                            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            NULL,
                            NULL,
                            hInstance,
                            NULL);
    
	if(!window)
	{
		OutputDebugString(L"Error creating the window");
	}
    
	// since we have used CS_OWNDC we don't need
	// to acquire a DC every time we need to draw.
	DeviceContext = GetDC(window);
    
	// create the color buffer for the first time
	// and set it to the client's area dimension.
	ResizeColorBuffer();
    
	global_Running = true;
    
	setup();
    
	// start the "game loop"
	while(global_Running)
	{
		// process input
		if (PeekMessage(&msg, window, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		update();
		render();
	}
	return(0);
}

LRESULT CALLBACK WindProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result;
	switch(message)
	{
		case WM_SIZE:
        // TODO: save client area new dimension using LOWORD and HIWORD macro
        global_ClientAreaWidth = LOWORD(lParam);
        global_ClientAreaHeight = HIWORD(lParam);
        ResizeColorBuffer();
        break;
        
		case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			global_Running = false;
		break;
        
		case WM_CLOSE:
        DestroyWindow(window);
        break;
        
		case WM_DESTROY:
        global_Running = false;
        break;
        
		default:
        result = DefWindowProc(window, message, wParam, lParam);
	}
	return(result);
}
