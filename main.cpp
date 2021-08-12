#include <Windows.h>
#include<gl/GL.h>
#include<gl/GLU.h>
#include<iostream>


#include "bedoPIxelGameEngine.h"

class example : public bedoPixelGameEngine
{
    bool OnUserCreate() {

        return true;
    }
public:
    int width = 256 * 4, height = 176 * 4;
        float x=0, y=0;
    bool OnUserUpdate(float fElpasedTime) {

//        DrawPixel(x, y, 128, 0, 0);
        for (int i = 0; i < height ; i++)
            for (int j = 0; j < width; j++)
                if (i > 80)
                    DrawPixel(j, i, 0, 128, 0);
                else
                    DrawPixel(j, i, 0, 0, 128);
        
        if (x >= width)
                x = 0;
                //y = 2;
        int speed = 20;
        x += (float)speed * fElpasedTime;
        
                //for(int i  =0; i < 20; i++)
                //for(int j = 0; j < 20; j++)
     
                 //   DrawPixel(x + j, y + i, 128, 0, 0);
                //pixel rgb;
                //rgb.SetRGB(128, 0, 128);

               // DrawLine(0, 0, 6, 10, rgb);

           
        return true;
    }

};


int main() {
    example j;
    
    j.ConstructPixel(j.width, j.height, 0, 0, 1, 1, L"hello");
    j.run();
	return 0;
}