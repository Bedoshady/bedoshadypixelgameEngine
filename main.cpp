#include <Windows.h>
#include<gl/GL.h>
#include<gl/GLU.h>
#include<iostream>


#include "bedoPIxelGameEngine.h"
int main() {
    bedoPixelGameEngine j;
    j.ConstructPixel(640, 320, 0, 0, 4, 4, L"hello");
    j.run();
	return 0;
}