#pragma once

#include <string>

#include <Windows.h>

#include "Math.h"
#include "Colors.h"

namespace Screen
{
	int autoInit();
	int init(unsigned short int w, unsigned short int h);

	void pixel(Math::Point pos, CHAR_INFO c);
	void line(Math::Point v1, Math::Point v2, CHAR_INFO c);
	void triangle(Math::Point v1, Math::Point v2, Math::Point v3, CHAR_INFO c);
	void fillrect(Math::Point pos1, Math::Point pos2, CHAR_INFO c);

	void drawTriangle(Math::Point v1, Math::Point v2, Math::Point v3, CHAR_INFO c);
	void drawTriangleRaw(Math::Vector2d v1, Math::Vector2d v2, Math::Vector2d v3, CHAR_INFO c);

	void putStr(Math::Point pos, std::wstring string, CHAR_INFO c);

	unsigned short int getw();
	unsigned short int geth();
	double getScreenRatio();
	double getCharRatio();

	Math::Point getDMousePos();

	void flush();
	void updateMouse();

	int generateConsole();

	void writeStr(const wchar_t* str);
	void writeInt(const int number);
}

