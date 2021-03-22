// 2018

#include <iostream>
#include <fstream>
#include <cmath>
#include <chrono>
#include <thread>

#include <Windows.h>

#include "Math.h"
#include "Screen.h"
#include "Camera.h"
#include "3dObject.h"

/*

ideas to implement:

done - create simple engine prototype: points, lines, polygons (no fill)
done - read objects from file
done - timing with chrono
done - triangles (fill), culling

  --- [ YOU ARE HERE ] ---

	 - refactor asap, SOLID
     - winapi events (or stay with GetKeyState)
	 - z-buffer
	 - lights, shades (needs z-buffer?)
	 - process object files (currently there is setcharcolor command and #comment but maybe need more customization?)
	 - triangulation (make triangles from  quads or polygons in obj file)
	 - more on objects (components maybe)
	 - scripting (lua or js or smt else?)
	 - audio
	 - optimizations (faster triangles, lines, buffering, etc.)
	 - physics (jkjk)
	 - threads (why?)

*/

int wmain(int argc, wchar_t* argv[])
{
	int error = Screen::autoInit();
	if (error) return error;

	std::vector<Object*> objects;

	objects.push_back(createObjectFromOBJFile("cube.txt"));
	if (!objects[0]) return 1;
	objects[0]->setPosition({ 100, 0, 100 });
	objects[0]->resize(7);

	Camera camera;

	// prepare timing
	std::chrono::steady_clock::time_point start, end, second;
	std::chrono::duration<float, std::milli> elapsed_time(0), time_to_wait;
	// info per current second
	unsigned short int frames_per_second = 0;
	std::chrono::duration<float, std::milli> min_time = std::chrono::milliseconds(0), max_time = std::chrono::milliseconds(0); // within the last second
	float average_time = 0.0f; // within the last second
	// info about previous second
	unsigned short int frames_per_second_prev = 0;
	std::chrono::duration<float, std::milli> min_time_prev, max_time_prev;

	// main loop
	while (true)
	{
		// -- timing code
		start = std::chrono::high_resolution_clock::now();
		// --

		Screen::fillrect({ 0, 0 }, { Screen::getw() - 1, Screen::geth() - 1 }, { ' ', 0 });

		// some random logic code
		objects[0]->rotate(Math::createQuaternion({ 0.0, 1.0, 0.0 }, 0.01));
		objects[0]->rotate(Math::createQuaternion({ 0.0, 0.0, 1.0 }, 0.005));

		if (GetKeyState(VK_ESCAPE) & 0x8000) break;

		camera.control();
		camera.prepare();

		for (auto iter = objects.begin(); iter != objects.end(); iter++)
			camera.draw(**iter);

		// print previous frame's time
		Screen::putStr({ 1, 1 }, std::wstring(L"time: ") + std::to_wstring(elapsed_time.count()), { ' ', BGLRED | FGWHITE });
		Screen::putStr({ 1, 2 }, std::wstring(L"max time: ") + std::to_wstring(max_time_prev.count()), { ' ', BGLRED | FGWHITE });
		Screen::putStr({ 1, 3 }, std::wstring(L"min time: ") + std::to_wstring(min_time_prev.count()), { ' ', BGLRED | FGWHITE });
		Screen::putStr({ 1, 4 }, std::wstring(L"average time: ") + std::to_wstring(average_time), { ' ', BGLRED | FGWHITE });
		Screen::putStr({ 1, 5 }, std::wstring(L"frames: ") + std::to_wstring(frames_per_second_prev), { ' ', BGLRED | FGWHITE });
		Screen::putStr({ 1, 7 }, std::wstring(L"PRESS [ESC] TO EXIT"), { ' ', BGBLACK | FGWHITE });

		/*for (int i = 0; i < NUMBER_OF_SHADES; i++)
		{
			Screen::pixel({ i + 1, 7 }, SHADES[i]);
			Screen::pixel({ i + 1, 8 }, SHADES2[i]);
		}*/

		Screen::updateMouse();
		Screen::flush();


		// -- timing code
		frames_per_second++;

		if (std::chrono::high_resolution_clock::now() - second > std::chrono::seconds(1))
		{
			min_time_prev = min_time;
			max_time_prev = max_time;
			min_time = elapsed_time;
			max_time = elapsed_time;
			average_time = 1.0f / frames_per_second * 1000.0f;
			frames_per_second_prev = frames_per_second;
			frames_per_second = 0;
			second = std::chrono::high_resolution_clock::now();
		}


		if (elapsed_time > max_time)
			max_time = elapsed_time;
		if (elapsed_time < min_time)
			min_time = elapsed_time;

		end = std::chrono::high_resolution_clock::now();
		elapsed_time = end - start;
		time_to_wait = std::chrono::duration<float, std::milli>(16.0f) - elapsed_time;
		if (time_to_wait > std::chrono::duration<float, std::milli>(1.0f))
			std::this_thread::sleep_for(time_to_wait);
		// --
	}

	return 0;
}
