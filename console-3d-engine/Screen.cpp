#include <assert.h>
#include <wchar.h>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

#include <Windows.h>

#include "Screen.h"

namespace
{
	HWND hWindow;
	HANDLE _hConsole;

	CHAR_INFO * _buffer;
	//unsigned short int _consoleSize.x, _consoleSize.y;

	RECT windowRect;
	Math::Point mousePos, prevMousePos, dMousePos;

	Math::Point consoleCursor;

	Math::Point _consoleSize;
	double _consoleRatio;

	Math::Point _consoleCharSize;
	double _consoleCharRatio; // = _consoleCharSize.x / _consoleCharSize.y
}

int Screen::autoInit()
{
	// TODO: need logging in this function
	unsigned short int w, h;
	char input = 0;
	_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	while (true)
	{
		char input = 0;
		COORD maxWindowSize = GetLargestConsoleWindowSize(_hConsole);

		std::cout << "Max window size is: " << maxWindowSize.X << "x" << maxWindowSize.Y << std::endl;

		CONSOLE_FONT_INFOEX cfi;
		cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);
		if (!GetCurrentConsoleFontEx(_hConsole, FALSE, &cfi))
			return 1;
		std::cout << "Character size is: " << cfi.dwFontSize.X << "x" << cfi.dwFontSize.Y << std::endl;

		std::cout << "You can try to change font size to increase window size." << std::endl;
		std::cout << "You can use [ALT] + [ENTER] to switch to fullscreen (works only on Windows 10)." << std::endl;
		std::cout << "When you are done customizing, enter [R] to update, then [C] to start" << std::endl;
		std::cout << "[R] - Refresh." << std::endl;
		std::cout << "[C] - Continue with these values." << std::endl;
		std::cout << "[Q] - Quit." << std::endl;

		std::cin >> input;

		if (input >= 'a' && input <= 'z')
			input -= 'a' - 'A';
		
		if (input == 'C')
		{
			w = maxWindowSize.X;
			h = maxWindowSize.Y;
			break;
		}
		else if (input == 'Q')
			return 1;
		
	}

	//generateConsole();
	return init(w, h);
}

int Screen::init(unsigned short int width, unsigned short int height)
{
	hWindow = GetConsoleWindow();

	_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbinfo;
	if (!GetConsoleScreenBufferInfo(_hConsole, &csbinfo))
	{
		//std::cerr << "Error code: " << GetLastError() << std::endl;
		return 1;
	}


	COORD windowSize = { csbinfo.srWindow.Left - csbinfo.srWindow.Right + 1, csbinfo.srWindow.Bottom - csbinfo.srWindow.Top + 1 };

	//change console buffer size and window size

	//if w < current_w or h < current_h:
	//	change window size
	//	change buffer size
	//else
	//	change buffer size
	//	change window size

	if (width < windowSize.X || height < windowSize.Y)
	{
		//resize window
		SMALL_RECT info =
		{
			0,
			0,
			width - 1,
			height - 1
		};

		if (!SetConsoleWindowInfo(_hConsole, TRUE, &info))
		{
			//std::cerr
			return 3;
		}

		//resize buffer
		if (!SetConsoleScreenBufferSize(_hConsole, { (short)width, (short)height }))
		{
			//std::cerr
			return 4;
		}
	}
	else
	{
		//resize buffer
		if (!SetConsoleScreenBufferSize(_hConsole, { (short)width, (short)height }))
		{
			//std::cerr
			return 4;
		}

		//resize window
		SMALL_RECT info =
		{
			0,
			0,
			width - 1,
			height - 1
		};

		if (!SetConsoleWindowInfo(_hConsole, TRUE, &info))
		{
			//std::cerr
			return 3;
		}
	}


	if (_buffer)
		delete _buffer;

	_buffer = new CHAR_INFO[height * width];
	if (!_buffer) return 5;

	_consoleSize = { width, height };
	_consoleRatio = (double)width / (double)height;

	RECT winRect;
	GetClientRect(hWindow, &winRect);
	windowSize = { (SHORT)winRect.right, (SHORT)winRect.bottom };

	consoleCursor = { 0, 0 };

	// console character size
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	GetCurrentConsoleFontEx(_hConsole, FALSE, &cfi);
	_consoleCharSize = { cfi.dwFontSize.X, cfi.dwFontSize.Y };
	_consoleCharRatio = (double)cfi.dwFontSize.X / (double)cfi.dwFontSize.Y;

	return 0;
}

void Screen::flush()
{
	SMALL_RECT write_region = { 0, 0, _consoleSize.x - 1, _consoleSize.y - 1 };
	WriteConsoleOutput(_hConsole, _buffer, { (short)_consoleSize.x, (short)_consoleSize.y }, { 0, 0 }, &write_region);
}

void Screen::updateMouse()
{
	hWindow = GetConsoleWindow();
	GetClientRect(hWindow, &windowRect);

	RECT clientRect;
	POINT prevMousePos;
	GetClientRect(hWindow, &clientRect);
	GetCursorPos(&prevMousePos);

	dMousePos = { prevMousePos.x - windowRect.right / 2, prevMousePos.y - windowRect.bottom / 2};

	dMousePos = { prevMousePos.x - windowRect.right / 2, prevMousePos.y - windowRect.bottom / 2 };

	POINT newMousePos = { windowRect.right / 2, windowRect.bottom / 2 };
	ClientToScreen(hWindow, &newMousePos);
	SetCursorPos(newMousePos.x, newMousePos.y);
}

int Screen::generateConsole()
{
	HWND hConsoleWindow = GetConsoleWindow();
	DWORD consoleOwnersPID;
	GetWindowThreadProcessId(hConsoleWindow, &consoleOwnersPID);
	DWORD myPID = GetCurrentProcessId();

	if (consoleOwnersPID == myPID)
	{
		//std::cout << "I have my own console!" << std::endl;
		_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		consoleCursor = { 0, 0 };

		return 0;
	}
	else
	{
		//std::cout << "Console is not mine." << std::endl;

		if (!FreeConsole()) return 1;
		if (!AllocConsole()) return 2;

		//std::cout << "This text should appear in a new console." << std::endl;
		_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		consoleCursor = { 0, 0 };

		return 0;
	}
}

void Screen::pixel(Math::Point pos, CHAR_INFO c)
{
	assert(_buffer);
	assert(pos.x >= 0 && pos.x < _consoleSize.x && pos.y >= 0 && pos.y < _consoleSize.y);
	_buffer[pos.y * _consoleSize.x + pos.x] = c;

	/*
	if (pos.x >= 0 && pos.x < _consoleSize.x && pos.y >= 0 && pos.y < _consoleSize.y)
	{
		_buffer[pos.y * _consoleSize.x + pos.x] = c;
	}*/
}

void Screen::fillrect(Math::Point pos1, Math::Point pos2, CHAR_INFO c)
{
	if (pos2.x < 0 || pos2.y < 0 || pos1.x >= _consoleSize.x || pos1.y >= _consoleSize.y)
		return;

	assert(_buffer);

	if (pos1.x < 0) pos1.x = 0;
	if (pos1.y < 0) pos1.y = 0;
	if (pos2.x >= _consoleSize.x) pos2.x = _consoleSize.x - 1;
	if (pos2.y >= _consoleSize.y) pos2.y = _consoleSize.y - 1;

	for (unsigned short int y = pos1.y; y <= pos2.y; y++)
		for (unsigned short int x = pos1.x; x <= pos2.x; x++)
			_buffer[y * _consoleSize.x + x] = c;
}

void Screen::line(Math::Point v1, Math::Point v2, CHAR_INFO c)
{
	if (v1.x > v2.x)
	{
		//swap
		Math::Point temp = v1;
		v1 = v2;
		v2 = temp;
	}

	if (v1.x < 0)
	{
		if (v2.x < 0) return; // line is off-screen
		if (v1.y != v2.y)
			v1.y += (double)(v2.y - v1.y) * (double)-v1.x / (double)(v2.x - v1.x);
		v1.x = 0;
	}

	if (v1.y < 0)
	{
		if (v2.y < 0) return; // line is off-screen
		if (v1.x != v2.x)
			v1.x = v2.x - (double)(v2.x - v1.x) * (double)v2.y / (double)(v2.y - v1.y);
		v1.y = 0;
	}

	if (v1.y >= _consoleSize.y)
	{
		if (v2.y >= _consoleSize.y) return; // line is off-screen
		if (v1.x != v2.x)
			v1.x += (double)(v2.x - v1.x) * (double)(v1.y - _consoleSize.y + 1) / (double)(v1.y - v2.y + 1);
		v1.y = _consoleSize.y - 1;
	}

	if (v2.x >= _consoleSize.x)
	{
		if (v1.x >= _consoleSize.x) return; // line is off-screen
		if (v2.y != v1.y)
			v2.y -= (double)(v2.y - v1.y) * (double)(v2.x - _consoleSize.x + 1) / (double)(v2.x - v1.x + 1);
		v2.x = _consoleSize.x - 1;
	}

	if (v2.y < 0)
	{
		if (v1.y < 0) return; // line is off-screen
		if (v1.x != v2.x)
			v2.x = v1.x + (double)(v2.x - v1.x) * (double)v1.y / (double)(v1.y - v2.y);
		v2.y = 0;
	}

	if (v2.y >= _consoleSize.y)
	{
		if (v1.y >= _consoleSize.y) return; // line is off-screen
		if (v1.x != v2.x)
			v2.x = v1.x + (double)(v2.x - v1.x) * (double)(_consoleSize.y - v1.y + 1) / (double)(v2.y - v1.y + 1);
		v2.y = _consoleSize.y - 1;
	}



	double dx = v2.x - v1.x,
		dy = v2.y - v1.y;
	if (v2.x - v1.x == 0)
	{
		short int py = dy >= 0 ? 1 : -1;
		for (int y = v1.y; y <= v2.y; y += py)
			Screen::pixel({ v1.x, y }, c);
	}
	if (v2.y - v1.y == 0)
	{
		short int px = dx >= 0 ? 1 : -1;
		for (int x = v1.x; x <= v2.x; x += px)
			Screen::pixel({ x, v1.y }, c);
	}
	if (dx > std::abs(dy))
	{
		double err = 0.5,
			derr = std::abs(dy) / dx;

		int py = (dy > 0 ? 1 : -1);
		int y = v1.y;

		for (int x = v1.x; x <= v2.x; x++)
		{
			Screen::pixel({ x, y }, c);

			err += derr;
			if (err >= 1)
			{
				y += py;
				err -= 1;
			}
		}
	}
	else
	{
		double err = 0.5,
			derr = dx / (dy > 0 ? dy : -dy);

		int py = (dy > 0 ? 1 : -1);
		int x = v1.x;

		for (int y = v1.y; y != v2.y + py; y += py)
		{
			Screen::pixel({ x, y }, c);

			err += derr;
			if (err >= 1)
			{
				x++;
				err -= 1;
			}
		}
	}
}

void Screen::triangle(Math::Point v1, Math::Point v2, Math::Point v3, CHAR_INFO c)
{
	if (!(v1.x >= 0 && v1.x < getw() && v1.y >= 0 && v1.y < geth() &&
		v2.x >= 0 && v2.x < getw() && v2.y >= 0 && v2.y < geth() &&
		v3.x >= 0 && v3.x < getw() && v3.y >= 0 && v3.y < geth()))
		return;

	// v1.y < v2.y < v3.y
	
	if (v1.y > v2.y)
	{
		Math::Point temp = v1;
		v1 = v2;
		v2 = temp;
	}

	if (v1.y > v3.y)
	{
		Math::Point temp = v1;
		v1 = v3;
		v3 = temp;
	}

	if (v2.y > v3.y)
	{
		Math::Point temp = v2;
		v2 = v3;
		v3 = temp;
	}

	Math::Point d12 = { v2.x - v1.x, v2.y - v1.y },
		d13 = { v3.x - v1.x, v3.y - v1.y },
		d23 = { v3.x - v2.x, v3.y - v2.y };

	double der12 = (double)d12.x / (double)d12.y,
		der13 = (double)d13.x / (double)d13.y,
		der23 = (double)d23.x / (double)d23.y;

	double x12 = v1.x, x13 = v1.x, x23 = v2.x;

	if (v1.y != v2.y)
		for (int y = v1.y; y <= v2.y; y++)
		{
			if (x12 < x13)
				for (int x = (int)(x12 + 0.5); x < (int)(x13 + 0.5); x++)
					Screen::pixel({ x, y }, c);
			else
				for (int x = (int)(x12 + 0.5); x > (int)(x13 + 0.5); x--)
					Screen::pixel({ x, y }, c);

			x12 += der12;
			x13 += der13;
		}

	if (v2.y != v3.y)
		for (int y = v2.y; y <= v3.y; y++)
		{
			if (x12 < x13)
				for (int x = (int)(x23 + 0.5); x < (int)(x13 + 0.5); x++)
					Screen::pixel({ x, y }, c);
			else
				for (int x = (int)(x23 + 0.5); x >(int)(x13 + 0.5); x--)
					Screen::pixel({ x, y }, c);

			x23 += der23;
			x13 += der13;
		}
}

void Screen::drawTriangle(Math::Point v1, Math::Point v2, Math::Point v3, CHAR_INFO c)
{
	// sort v1.y < v2.y < v3.y
	if (v1.y > v3.y)
		std::swap(v1, v3);
	if (v1.y > v2.y)
		std::swap(v1, v2);
	if (v2.y > v3.y)
		std::swap(v2, v3);

	Math::Point d12 = { v2.x - v1.x, v2.y - v1.y },
		d23 = { v3.x - v2.x, v3.y - v2.y },
		d13 = { v3.x - v1.x, v3.y - v1.y };

	double k12 = (double)d12.x / (double)d12.y,
		k23 = (double)d23.x / (double)d23.y,
		k13 = (double)d13.x / (double)d13.y;

	double sign12 = (k12 > 0 ? 0.5 : -0.5),
		sign23 = (k23 > 0 ? 0.5 : -0.5),
		sign13 = (k13 > 0 ? 0.5 : -0.5);

	int x12 = v1.x, x23 = v2.x, x13 = v1.x;

	if (v1.y != v2.y)
		for (int y = std::max<int>(v1.y, 0); y <= std::min<int>(v2.y, Screen::geth() - 1); y++)
		{
			x12 = v1.x + (int)(k12 * (double)(y - v1.y) + sign12);
			x13 = v1.x + (int)(k13 * (double)(y - v1.y) + sign13);

			if (x12 < x13)
				for (int x = std::max<int>(x12, 0); x <= std::min<int>(x13, Screen::getw() - 1); x++)
					Screen::pixel({ x, y }, c);
			else if (x12 > x13)
				for (int x = std::min<int>(x12, Screen::getw() - 1); x >= std::max<int>(x13, 0); x--)
					Screen::pixel({ x, y }, c);
			else if (x12 >= 0 && x12 < Screen::getw()) // if x12 == x13
				Screen::pixel({ std::max<int>(0, std::min<int>((int)Screen::getw() - 1, (int)x13)), y }, c);
		}

	if (v2.y != v3.y)
		for (int y = std::max<int>(v2.y, 0); y <= std::min<int>(v3.y, Screen::geth() - 1); y++)
		{
			x13 = v1.x + (int)(k13 * (double)(y - v1.y) + sign13);
			x23 = v2.x + (int)(k23 * (double)(y - v2.y) + sign23);

			if (x23 < x13)
				for (int x = std::max<int>(x23, 0); x <= std::min<int>(x13, Screen::getw() - 1); x++)
					Screen::pixel({ x, y }, c);
			else if (x23 > x13)
				for (int x = std::min<int>(x23, Screen::getw() - 1); x >= std::max<int>(x13, 0); x--)
					Screen::pixel({ x, y }, c);
			else if (x23 >= 0 && x23 < Screen::getw()) // if x23 == x13
				Screen::pixel({ std::max<int>(0, std::min<int>((int)Screen::getw() - 1, (int)x13)), y }, c);
		}

}



unsigned short int Screen::getw()
{
	return _consoleSize.x;
}

unsigned short int Screen::geth()
{
	return _consoleSize.y;
}

double Screen::getScreenRatio()
{
	return _consoleRatio;
}

double Screen::getCharRatio()
{
	return _consoleCharRatio;
}

Math::Point Screen::getDMousePos()
{
	return dMousePos;
}

void Screen::putStr(Math::Point pos, std::wstring string, CHAR_INFO c)
{
	unsigned int y = pos.y;
	if (y >= 0 && y < _consoleSize.y)
	{
		unsigned int len = string.length();

		unsigned int x1, strx1;
		x1 = (pos.x < 0 ? 0 : pos.x);
		strx1 = pos.x < 0 ? -pos.x : 0;

		for (unsigned int i = 0; i < len - strx1 && i < _consoleSize.x - x1; i++)
			_buffer[y * _consoleSize.x + x1 + i] = { string[strx1 + i], c.Attributes };
	}
}

void Screen::writeStr(const wchar_t* str)
{
	_buffer[0] = { L'W', BGBLACK | FGWHITE };
	unsigned short int chunkIndex = 0;
	while (true)
	{
		unsigned int len = _consoleSize.x - consoleCursor.x + 1;
		for (unsigned int i = 0; i <= _consoleSize.x - consoleCursor.x; i++)
			if (*(str + chunkIndex + i) == '\0')
			{
				len = i; // str[len] == '\0'
				break;
			}
		
		if (len > _consoleSize.x - consoleCursor.x)
		{
			//len = getw();
			//unsigned short int end = _consoleSize.x > len ? len : _consoleSize.x;
			//end = end - consoleCursor.x;
			for (unsigned short int i = 0; i < _consoleSize.x - consoleCursor.x; i++)
				_buffer[consoleCursor.y * _consoleSize.x + consoleCursor.x + i] = { str[chunkIndex + i], BGBLACK | FGWHITE };
			
			chunkIndex += _consoleSize.x - consoleCursor.x;
			consoleCursor.x = 0;
			consoleCursor.y++;
			if (consoleCursor.y == _consoleSize.y)
				consoleCursor.y = 0;
		}
		else if (len <= _consoleSize.x - consoleCursor.x)
		{
			for (unsigned short int i = 0; i < len; i++)
				_buffer[consoleCursor.y * _consoleSize.x + consoleCursor.x + i] = { str[chunkIndex + i], BGBLACK | FGWHITE };
			
			if (consoleCursor.x + len < _consoleSize.x) consoleCursor.x += len;
			else
			{
				consoleCursor.x = 0;
				consoleCursor.y++;
				if (consoleCursor.y == _consoleSize.y)
					consoleCursor.y = 0;
			}

			break;
		}
	}
}
void Screen::writeInt(int number)
{
	char t[10];
	wchar_t str[11];

	unsigned int i;
	for (i = 0; i < 10 && number; i++)
	{
		t[i] = number % 10;
		number = number / 10;
	}

	for (unsigned int j = 0; j < i; j++)
		str[j] = t[i - j - 1] + '0';
	str[i] = '\0';

	writeStr(str);
}