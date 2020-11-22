#pragma once
#include <windows.h>
#include <stdio.h>

//#define __FILE_NAME__ (strrchr("\\"__FILE__, '\\') + 1)
#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

void log(const char file[], int line, const char* format, ...);

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Deletes a buffer
#define RELEASE( x ) \
    {\
       if( x != nullptr )\
       {\
         delete x;\
	     x = nullptr;\
       }\
    }

// Deletes an array of buffers
#define RELEASE_ARRAY( x ) \
	{\
       if( x != nullptr )\
       {\
           delete[] x;\
	       x = nullptr;\
		 }\
	 }

// Configuration -----------
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define FULLSCREEN false
#define RESIZABLE true
#define VSYNC true
#define TITLE "Super Awesome Engine"