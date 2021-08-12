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
public:
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
		int GetRed() {
			return (float)color.b[0] ;
		}
		int GetGreen() {
			return (float)color.b[1] ;
		}
		int GetBlue() {
			return (float)color.b[2] ;
		}

	};

	virtual bool OnUserCreate() { return true; };
	virtual bool OnUserUpdate(float fElapseTime) { return true; };

private:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

		static PAINTSTRUCT ps;

		switch (uMsg) {

		
		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
			break;
		}
	}

public: 
	void Genertaetexture() {
		uint32_t id;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glEnable(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
			GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR);

	}
	void display() {
		
		glViewport(0, 0, m_width, m_height);
		 
		glClear(GL_COLOR_BUFFER_BIT);

		//glPixelStorei(GL_PACK_ROW_LENGTH, m_width);

//		glMatrixMode(GL_PROJECTION);
		//glLoadIdentity();
		//gluOrtho2D(-1,m_width,m_height, -1);
		//gluOrtho2D()
		//glMatrixMode(GL_MODELVIEW);

		for (int i = 0; i < m_width * m_height; i++)
			buffer[i] = m_screenBuffer[i].color.c;

		
		
		int toptexel = 1, lefttexel = -1, righttexel = 1, bottomtexel = -1;
		
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,m_width, m_height,0 ,GL_RGBA, GL_UNSIGNED_BYTE, buffer);
		//glTexSubImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
		
		glBegin(GL_QUADS);

		glTexCoord2f(1, 1); glVertex2f((GLfloat)righttexel, (GLfloat)bottomtexel);
		glTexCoord2f(0, 1); glVertex2f((GLfloat)lefttexel, (GLfloat)bottomtexel);
		glTexCoord2f(0, 0); glVertex2f((GLfloat)lefttexel, (GLfloat)toptexel);
		glTexCoord2f(1, 0); glVertex2f((GLfloat)righttexel, (GLfloat)toptexel);

		glEnd();
		//glDrawPixels(m_width, m_height,GL_RGBA, GL_UNSIGNED_BYTE, buffer);
		SwapBuffers(hdc);

	}
public:

	void DrawLine(int x1, int y1, int x2, int y2, pixel rgb) {
		auto swap = [&](int x, int y) {int acc = x; x = y; y = acc; };
			if (x2 > x1) {
				swap(x2, x1);
				swap(y2, y1);
			}

			if (x2 - x1 == 0)
			{
				if (y1 > y2)
					swap(y1, y2);
				for (int i = y1; i <= y2; i++)
					DrawPixel(x1, i, rgb.GetRed(), rgb.GetGreen(), rgb.GetBlue());
			
			
				return;
			}
			if (y2 - y1 == 0) {
				for (int i = x1; i <= x2; i++)
					DrawPixel(i, y1, rgb.GetRed(), rgb.GetGreen(), rgb.GetBlue());
				return;
			}
			if (abs(x2 - x1) > abs(y2 - y1))
			{
				int sign = 0;
				if (y2 - y1 > 0)
					sign = 1;
				else
					sign = -1;


				int dx = abs(x2 - x1);
				int dy = abs(y2 - y1);
				int acc = 0;
				int y = y1;
				for (int i = x1; i <= x2; i++) {
					DrawPixel(i, y, rgb.GetRed(), rgb.GetGreen(), rgb.GetBlue());
					acc += 2 * dy;
					if (2 * acc > dx) {
						y += sign;
						acc -= 2 * dx;

					}


				}


			}
			else {

				if (y1 > y2)
				{
					swap(y1, y2);
					swap(x1, x2);
				}


				int sign = 0;
				if (x2 - x1 > 0)
					sign = 1;
				else
					sign = -1;


				int dx = abs(x2 - x1);
				int dy = abs(y2 - y1);
				int acc = 0;
				int x = x1;
				for (int i = y1; i <= y2; i++) {
					DrawPixel(x, i, rgb.GetRed(), rgb.GetGreen(), rgb.GetBlue());
					acc += 2 * dx;
					if (2 * acc > dy) {
						x += sign;
						acc -= 2 * dy;

					}


				}



			}



	}








	void DrawPixel(int x, int y, int r, int g, int b) {
		pixel rgb;
		rgb.SetRGB(r, g, b);
			for(int i  = 0; i < m_fontY; i++)
				for (int j = 0; j < m_fontX; j++) {
					int posx = m_fontX * x + j;
					int posy = m_fontY * y + i;
					if (posx >= 0 && posx < m_width && posy >= 0 && posy < m_height)
						m_screenBuffer[(posy)*m_width + posx] = rgb;
				}


	}
	void ConstructPixel(int width, int height, int posx, int posy, int fontX, int fontY, std::wstring name) {
		m_width = width * fontX;
		m_height = height * fontY;
		m_fontX = fontX;
		m_fontY = fontY;
		wc.lpfnWndProc =  WindowProc;
		//gsgfsfs
		wc.hInstance = GetModuleHandle(nullptr);
		wc.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
		wc.lpszClassName = name.c_str();
		RegisterClass(&wc);

		DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		DWORD dwStyle = WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_THICKFRAME;
		RECT rWndRect = { 0, 0, m_width, m_height };
		AdjustWindowRectEx(&rWndRect, dwStyle, FALSE, dwExStyle);
		int mwidth = rWndRect.right - rWndRect.left;
		int mheight = rWndRect.bottom - rWndRect.top;


		//buffer = new uint32_t[m_width * m_height];

		m_hwnd = CreateWindowEx(
			0,                              // Optional window styles.
			name.c_str(),                     // Window class
			name.c_str(),    // Window text
			WS_OVERLAPPEDWINDOW,            // Window style

			// Size and position
			posx, posy, mwidth, mheight  ,

			NULL,       // Parent window    
			NULL,       // Menu
			GetModuleHandle(nullptr),  // Instance handle
			NULL        // Additional application data
		);
		//enableOpengl(m_hwnd);


		m_screenBuffer = new pixel[(m_width) * (m_height)];
		memset(m_screenBuffer, 0, sizeof(pixel) * (m_width) * (m_height));
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

	 int32_t *buffer;
	void viewDisplay() {
		enableOpengl(m_hwnd);
		auto t1 = std::chrono::high_resolution_clock::now();
		auto t2 = std::chrono::high_resolution_clock::now();

		std::chrono::duration<float> elapseTime = t2 - t1;
		float fElapseTime = elapseTime.count();
		Genertaetexture();
		OnUserCreate();
		buffer = new int32_t[m_width * m_height];
		while (1) {
		//	display();
			t2 = std::chrono::high_resolution_clock::now();
			elapseTime = t2 - t1;
			fElapseTime = elapseTime.count();
			t1 = t2;
			OnUserUpdate(fElapseTime);
			display();
			//std::cout << 1.0f / fElapseTime << std::endl;;
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