#include <string>

#include "Transformable.h"
#include "Screen.h"

#include <Windows.h>

// Magic numbers
#define TRANSFORMABLES_SPEED 0.7f
#define MOUSE_SENSITIVY 0.0015f

void Transformable::control()
{
	if (GetKeyState('A') & 0x8000) // Left
		this->go({ 0.0, 0.0, -TRANSFORMABLES_SPEED });
	if (GetKeyState('W') & 0x8000) // Forward
		this->go({ TRANSFORMABLES_SPEED, 0.0, 0.0 });
	if (GetKeyState('D') & 0x8000) // Right
		this->go({ 0.0, 0.0, TRANSFORMABLES_SPEED });
	if (GetKeyState('S') & 0x8000) // Backward
		this->go({ -TRANSFORMABLES_SPEED, 0.0, 0.0 });
	if (GetKeyState(VK_SPACE) & 0x8000) // Up
		this->go({ 0.0, TRANSFORMABLES_SPEED, 0.0 });
	if (GetKeyState(VK_LSHIFT) & 0x8000) // Down
		this->go({ 0.0, -TRANSFORMABLES_SPEED, 0.0 });

	if (GetKeyState('Q') & 0x8000) // Rotate counter-clockwise
		this->rotate(Math::createQuaternion({ 1, 0, 0 }, 0.02));
	if (GetKeyState('E') & 0x8000) // Rotate clockwise
		this->rotate(Math::createQuaternion({ 1, 0, 0 }, -0.02));

	// Where am i?
	//Screen::putStr({ 1, 1 }, (std::to_wstring(position.x) + L' ' + std::to_wstring(position.y) + L' ' + std::to_wstring(position.z)).c_str(), { ' ', BGBLACK | FGWHITE });

	Math::Point dMouse = Screen::getDMousePos();

	this->rotate(
		Math::combine(
			Math::createQuaternion({ 0, 1, 0 }, -(double)dMouse.x * MOUSE_SENSITIVY), 
			Math::createQuaternion({ 0, 0, 1 }, -(double)dMouse.y * MOUSE_SENSITIVY)
		)
	);
}

// Relative to normal vector
void Transformable::go(Math::Vector v)
{
	this->move(Math::rotate(v, this->quaternion));
}

void Transformable::move(Math::Vector v)
{
	this->position = this->position + v;
}

void Transformable::setRotation(Math::Quaternion q)
{
	this->quaternion = q;
	this->normal = Math::rotate({ 1.0, 0.0, 0.0 }, q);
}

void Transformable::setPosition(Math::Vector v)
{
	this->position = v;
}

void Transformable::rotate(Math::Quaternion q)
{
	this->quaternion = Math::combine(this->quaternion, q);
	this->normal = Math::rotate({ 1.0, 0.0, 0.0 }, this->quaternion);
}