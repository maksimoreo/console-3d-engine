#include "Camera.h"

#include <wchar.h>
#include <string>
#include <cmath>

#include <Windows.h>

#include "Math.h"
#include "Transformable.h"
#include "Screen.h"
#include "3dObject.h"

void (*Camera::pixelCorrection)(Math::Vector &) = nullptr;

Camera::Camera(Math::Vector pos, Math::Quaternion q, double screenX, double screenY, double fov) : Transformable(pos, q), fov(fov), tg_half_fov(std::tan(fov / 2 * Math::pi / 180)),
	cos_half_fov(std::cos(fov / 2 * Math::pi / 180))
{
	if (screenX == 0 || screenY == 0)
	{
		screenXLenght = Screen::getw();
		screenYLenght = Screen::geth();
	}
	else
	{
		screenXLenght = screenX;
		screenYLenght = screenY;
	}

	if (Screen::getCharRatio() > 1)
		Camera::pixelCorrection = &_pixelCorrectionX;
	else
		Camera::pixelCorrection = &_pixelCorrectionY;

	this->cameraBlindZone = 10.0;
}

void Camera::draw(Object const &obj)
{
	// object's position relative to camera
	Math::Vector OBS = obj.getPosition() - this->position;

	// if angle(OBS, cameraNormal) < 90:
	if (Math::dot(this->getNormal(), OBS) / (Math::len(this->getNormal()) * Math::len(OBS)) >= 0)
	{

		for (auto iter = obj.faces.begin(); iter != obj.faces.end(); iter++)
		{
			Math::Vector v0 = Math::rotate(obj.points[iter->points[0]].pos, obj.getRotation()) + obj.getPosition(), 
				v1 = Math::rotate(obj.points[iter->points[1]].pos, obj.getRotation()) + obj.getPosition(),
				v2 = Math::rotate(obj.points[iter->points[2]].pos, obj.getRotation()) + obj.getPosition();
			this->draw_face(v0, v1, v2, iter->color);
		}

		// Draw center of the object for debug
		// this->draw_point(obj.getPosition(), { 'O', BGBLACK | FGLGREEN });
	}

}

void Camera::draw_point(Math::Vector point, CHAR_INFO c)
{

	Math::Vector pointFromCamera = point - this->position;

	double pz = Math::dot(pointFromCamera, this->getNormal());

	if (pz >= 0)
	{
		Math::Vector onCameraPlane = (pointFromCamera - this->getNormal() * pz) / pz;

		Math::Vector onScreen = {
			Math::dot(this->screenX, onCameraPlane) * tg_half_fov / Screen::getScreenRatio(),
			Math::dot(this->screenY, onCameraPlane) * tg_half_fov
		};

		pixelCorrection(onScreen);

		Screen::pixel({ (LONG)std::round((onScreen.x + 0.5) * Screen::getw()),
			(LONG)std::round((onScreen.y + 0.5) * Screen::geth()) }, c);
	}
}

void Camera::draw_line(Math::Vector point1, Math::Vector point2, CHAR_INFO c)
{
	Math::Vector p1 = point1 - this->position, p2 = point2 - this->position;
	double p1d = Math::dot(p1, this->getNormal()), p2d = Math::dot(p2, this->getNormal());

	Math::Vector onCameraPlane1, onCameraPlane2;

	if (p1d >= this->cameraBlindZone && p2d >= this->cameraBlindZone)
	{
		/*if (p1d == this->cameraBlindZone) onCameraPlane1 = p1;
		else*/ onCameraPlane1 = (p1 - this->getNormal() * p1d) / p1d;
		/*if (p2d == this->cameraBlindZone) onCameraPlane2 = p2;
		else*/ onCameraPlane2 = (p2 - this->getNormal() * p2d) / p2d;
	}
	else if (p1d < this->cameraBlindZone && p2d < this->cameraBlindZone)
	{
		return;
	}
	else
	{
		// find line-plane intersection as point
		// idea has been stolen from here:
		// https://en.wikipedia.org/wiki/Line%E2%80%93plane_intersection

		// line_pos = first point (first end of line)
		// line_n 

		Math::Vector line_pos = p1, // line's position (first end of line)
			line_n = p2 - p1, // line's normal
			plane_pos = this->getNormal() * this->cameraBlindZone; // let plane go some units forward
		//  plane_n = this->getNormal();

		double d, // [0, 1]
			smt = Math::dot(line_n, this->getNormal()); // dunno how to name this sry

		// smt actually cannot be equal zero, since (??)if: both points are on different sides
		// of plane, so it is impossible that it is
		// d is >= 0 and <= 1 for the same reason
		if (smt != 0)
		{
			d = Math::dot(plane_pos - line_pos, this->getNormal()) / smt;


			if (p1d < this->cameraBlindZone)
			{
				p1 = line_pos + line_n * d;
				p1d = Math::dot(p1, this->getNormal());
			}
			else
			{
				p2 = line_pos + line_n * d;
				p2d = Math::dot(p2, this->getNormal());
			}


			onCameraPlane1 = (p1 - this->getNormal() * p1d) / p1d;
			onCameraPlane2 = (p2 - this->getNormal() * p2d) / p2d;
		}
	}

	Math::Vector onScreen1 = { Math::dot(this->screenX, onCameraPlane1) * tg_half_fov / Screen::getScreenRatio(), Math::dot(this->screenY, onCameraPlane1) * tg_half_fov };
	Math::Vector onScreen2 = { Math::dot(this->screenX, onCameraPlane2) * tg_half_fov / Screen::getScreenRatio(), Math::dot(this->screenY, onCameraPlane2) * tg_half_fov };

	pixelCorrection(onScreen1);
	pixelCorrection(onScreen2);

	Screen::line(
		{
			Math::round((onScreen1.x + 0.5) * Screen::getw()),
			Math::round((onScreen1.y + 0.5) * Screen::geth())
		},
		{
			Math::round((onScreen2.x + 0.5) * Screen::getw()),
			Math::round((onScreen2.y + 0.5) * Screen::geth())
		},
	c);
}

void Camera::draw_face(Math::Vector v1, Math::Vector v2, Math::Vector v3, CHAR_INFO c)
{
	Math::Vector a = v2 - v1, b = v3 - v1;
	Math::Vector normal = Math::cross(a, b);
	if (Math::dot(v1 - this->getPosition(), normal) < 0)
	{
		Math::Vector p1 = v1 - this->position, p2 = v2 - this->position, p3 = v3 - this->position;
		//if (Math::dot(p1, this->getNormal()) > this->cameraBlindZone && Math::dot(p2, this->getNormal()) > this->cameraBlindZone && Math::dot(p3, this->getNormal()) > this->cameraBlindZone)
		//	Screen::triangle(getOrthoProjection(v1), getOrthoProjection(v2), getOrthoProjection(v3), c);

		Math::Triangle in = { v1, v2, v3 }, out1, out2;
		int triangle_num = Math::spliceTriangleByPlane(in, this->getPosition() + this->getNormal() * (this->cameraBlindZone + 0.02), this->getNormal(), out1, out2);
		
		/*
		if (triangle_num == 1)
			Screen::triangle(getOrthoProjection(out1.v1), getOrthoProjection(out1.v2), getOrthoProjection(out1.v3), c);
		else if (triangle_num == 2)
		{
			Screen::triangle(getOrthoProjection(out2.v1), getOrthoProjection(out2.v2), getOrthoProjection(out2.v3), { '#', BGBLACK | FGGREEN });
			Screen::triangle(getOrthoProjection(out1.v1), getOrthoProjection(out1.v2), getOrthoProjection(out1.v3), { '#', BGBLACK | FGRED });
		}
		*/

		if (triangle_num == 1)
			Screen::drawTriangle(getOrthoProjection(out1.v1), getOrthoProjection(out1.v2), getOrthoProjection(out1.v3), c);
		else if (triangle_num == 2)
		{
			Screen::drawTriangle(getOrthoProjection(out2.v1), getOrthoProjection(out2.v2), getOrthoProjection(out2.v3), c);
			Screen::drawTriangle(getOrthoProjection(out1.v1), getOrthoProjection(out1.v2), getOrthoProjection(out1.v3), c);
		}

		/*
		this->draw_line(v1, v2, c);
		this->draw_line(v2, v3, c);
		this->draw_line(v3, v1, c);
		*/
	}
}

void Camera::prepare()
{
	screenX = Math::rotate({ 0, 0, 1 }, quaternion);
	screenY = Math::rotate({ 0, -1, 0 }, quaternion);

	/*
	std::wstring str = L"cameraNormal: " + std::to_wstring(this->getNormal().x) + L' ' + std::to_wstring(this->getNormal().y) + L' ' + std::to_wstring(this->getNormal().z);
	Screen::putStr({ 1, 2 }, str.c_str(), { ' ', BGBLACK | FGWHITE });
	str = L"screenX: " + std::to_wstring(screenX.x) + L' ' + std::to_wstring(screenX.y) + L' ' + std::to_wstring(screenX.z);
	Screen::putStr({ 1, 3 }, str.c_str(), { ' ', BGBLACK | FGWHITE });
	str = L"screenY: " + std::to_wstring(screenY.x) + L' ' + std::to_wstring(screenY.y) + L' ' + std::to_wstring(screenY.z);
	Screen::putStr({ 1, 4 }, str.c_str(), { ' ', BGBLACK | FGWHITE });
	*/
}

Math::Point Camera::getOrthoProjection(Math::Vector point)
{
	Math::Vector p = point - this->position;
	double pd = Math::dot(p, this->getNormal());
	Math::Vector onCameraPlane;

	if (pd >= this->cameraBlindZone)
		onCameraPlane = (p - this->getNormal() * pd) / pd;
	else
		onCameraPlane = { 0.0, 0.0, 0.0 };

	Math::Vector onScreen = { Math::dot(this->screenX, onCameraPlane) * tg_half_fov / Screen::getScreenRatio(), Math::dot(this->screenY, onCameraPlane) * tg_half_fov };
	pixelCorrection(onScreen);

	return {
		Math::round((onScreen.x + 0.5) * Screen::getw()),
		Math::round((onScreen.y + 0.5) * Screen::geth())
	};
}

// temporary functions
// these will be used for pointers

void _pixelCorrectionX(Math::Vector &v)
{
	v.x /= Screen::getCharRatio();
}
void _pixelCorrectionY(Math::Vector &v)
{
	v.y *= Screen::getCharRatio();
}