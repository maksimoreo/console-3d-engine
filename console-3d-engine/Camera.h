#pragma once

#include <Windows.h>

#include "Transformable.h"
#include "3dObject.h"

class Camera : public Transformable
{
public:
	Camera(Math::Vector pos = { 0.0, 0.0, 0.0 }, Math::Quaternion q = { 1.0, 0.0, 0.0, 0.0 }, double screenX = 0, double screenY = 0, double fov = 90);
	// if screenX or screenY == 0 they will be calculated automatically to adjust whole window

	void prepare(); // calculate stuff after transformations (no need to call if no transformations were used)
	void draw(Object const &obj);

	void draw_point(Math::Vector v, CHAR_INFO c);
	void draw_line(Math::Vector v1, Math::Vector v2, CHAR_INFO c);
	void draw_face(Math::Vector v1, Math::Vector v2, Math::Vector v3, CHAR_INFO c);

private:
	static void (*pixelCorrection)(Math::Vector &);
	Math::Point getOrthoProjection(Math::Vector);


	double screenXLenght, screenYLenght;
	Math::Vector /*cameraNormal,*/ screenX, screenY;

	double cameraBlindZone = 20.0f;

	double fov, tg_half_fov, cos_half_fov;
	// double vfov, tg_half_vfov, cos_half_vfov; // vertical
	// double hfov, tg_half_hfov, cos_half_hfov; // horizontal
};

void _pixelCorrectionX(Math::Vector &);
void _pixelCorrectionY(Math::Vector &);