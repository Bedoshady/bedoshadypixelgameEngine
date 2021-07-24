#pragma once
#include<cstdint>
#include<windows.h>
#include<gl/GL.h>
#include<gl/glu.h>
#include<string>
#include <chrono>
#include<thread>
#include<iostream>
//class Renderer;
//class bedoPixelGameEngine;
class Renderer;

class bedoPixelGameEngine {

	struct pixel {
		union rgb {
			uint32_t c;
			uint8_t b[4];
		};
		rgb color;
		void SetRGB(int r, int g, int b) {
			//color.c = (r << 24) | (g << 16) | (b << 8);
			color.b[0] = r;
			color.b[1] = g;
			color.b[2] = b;
			color.b[3] = 0x00;


		}
		float GetRed() {
			return (float)color.b[3] / 255.0f;
		}
		float GetGreen() {
			return (float)color.b[2] / 255.0f;
		}
		float GetBlue() {
			return (float)color.b[1] / 255.0f;
		}

	};

	bool OnUserCreate() { return true; };
	bool OnUserUpdate(float fElapseTime) { return true; };

private:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

		static PAINTSTRUCT ps;

		switch (uMsg) {

		
		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
			break;
		}
	}
	int positionx = 0;
	int positiony = 0;
#define GL_UNSIGNED_INT_8_8_8_8_REV 0x8367
#define GL_UNSIGNED_INT_8_8_8_8 0x8035
public: 

	void display() {
		
		glViewport(0, 0, m_width, m_height);
		 
		glClear(GL_COLOR_BUFFER_BIT);
	
		for (int i = 0; i < m_height; i++) {
			for (int j = 0; j < m_width; j++) {
			//	DrawPixel(j, i, 255,  255, 255);
				
				//pixel h;
				//h.SetRGB(255,0,255);
				
			//	 m_screenBuffer[i * m_width + j] = h;
			}
		}
		
		//glDrawPixels(m_width, m_height,GL_RGBA, GL_UNSIGNED_BYTE, m_screenBuffer);

		//glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,m_width, m_height,0 ,GL_RGBA, GL_UNSIGNED_BYTE, m_screenBuffer);


		SwapBuffers(hdc);

	}
public:
	void DrawPixel(int x, int y, int r, int g, int b) {
		pixel rgb;
		rgb.SetRGB(r, g, b);
			
		if (x >= 0 && x < m_width && y >= 0 && y <  m_height)
					m_screenBuffer[(y) * m_width + x ] = rgb;



	}
	void ConstructPixel(int width, int height, int posx, int posy, int fontX, int fontY, std::wstring name) {
		m_width = width ;
		m_height = height;
		m_fontX = fontX;
		m_fontY = fontY;
		wc.lpfnWndProc =  WindowProc;
		wc.hInstance = GetModuleHandle(nullptr);
		wc.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
		wc.lpszClassName = name.c_str();
		RegisterClass(&wc);
		//buffer = new uint32_t[m_width * m_height];

		m_hwnd = CreateWindowEx(
			0,                              // Optional window styles.
			name.c_str(),                     // Window class
			name.c_str(),    // Window text
			WS_OVERLAPPEDWINDOW,            // Window style

			// Size and position
			posx, posy, m_width, m_height,

			NULL,       // Parent window    
			NULL,       // Menu
			GetModuleHandle(nullptr),  // Instance handle
			NULL        // Additional application data
		);
		//enableOpengl(m_hwnd);


		m_screenBuffer = new pixel[m_width * m_height];
		memset(m_screenBuffer, 0, sizeof(pixel) * m_width * m_height);
	}
	void enableOpengl(const HWND& hwnd) {
		int pf = 0;

		PIXELFORMATDESCRIPTOR pfd =
		{
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER | 0,   // Flags
			PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
			32,                   // Colordepth of the framebuffer.
			0, 0, 0, 0, 0, 0,
			0,
			0,
			0,
			0, 0, 0, 0,
			24,                   // Number of bits for the depthbuffer
			8,                    // Number of bits for the stencilbuffer
			0,                    // Number of Aux buffers in the framebuffer.
			PFD_MAIN_PLANE,
			0,
			0, 0, 0
		};
		hdc = GetDC(hwnd);
		pf = ChoosePixelFormat(hdc, &pfd);

		if (pf == 0) {
			MessageBox(NULL, L"ChoosePixelFormat() failed:  "
				"Cannot find a suitable pixel format.", L"Error", MB_OK);
			//std::cout << "fail in choose";
			//  return 0;
		}
		//    std::cout << "hello";
		if (SetPixelFormat(hdc, pf, &pfd) == FALSE) {
			MessageBox(NULL, L"SetPixelFormat() failed:  "
				"Cannot set format specified.", L"Error", MB_OK);
			//  return 0;
		//	std::cout << "fail in Set";
		}
		m_glRenderContext_t = wglCreateContext(hdc);
		wglMakeCurrent(hdc, m_glRenderContext_t);
		ShowWindow(hwnd, 1);
	}


	void viewDisplay() {
		enableOpengl(m_hwnd);
		auto t1 = std::chrono::high_resolution_clock::now();
		auto t2 = std::chrono::high_resolution_clock::now();

		std::chrono::duration<float> elapseTime = t2 - t1;
		float fElapseTime = elapseTime.count();
		
		
		while (1) {
		//	display();
			t2 = std::chrono::high_resolution_clock::now();
			elapseTime = t2 - t1;
			fElapseTime = elapseTime.count();
			t1 = t2;
			display();
			//std::cout << 1.0f / fElapseTime << std::endl;;
			//OnUserUpdate(fElapseTime);
			std::wstring h = L"bedoPixelgameEngine " + std::to_wstring(1.0f / fElapseTime);
			SetWindowText(m_hwnd, h.c_str());
		
		}
	}

	void run();
	void runImage() {

		std::thread thr = std::thread(&bedoPixelGameEngine::viewDisplay, this);
		MSG msg = { };
		while (GetMessage(&msg, NULL, 0, 0) > 0)
		{

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

//		rend.run();
		thr.join();


	}
private:
	int m_width, m_height;
	int m_fontX, m_fontY;
	WNDCLASS wc = { };

	
	HWND m_hwnd;
	HDC hdc;
	HGLRC m_glRenderContext_t;
	pixel* m_screenBuffer;
	// Create the window.

	
};





class Renderer {

public:



	
	//	std::thread thr = std::thread(&bedoPixelGameEngine::viewDisplay, this);
	void run() {
		MSG msg = { };
		while (GetMessage(&msg, NULL, 0, 0) > 0)
		{

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//thr.join();



	}

};
Renderer rend;

void bedoPixelGameEngine::run() {
	runImage();

}