#pragma once
#include "Math.h"

class Transformable
{
public:
	Transformable(Math::Vector pos = { 0, 0, 0 }, Math::Quaternion quaternion = { 1.0, 0, 0, 0 }) : position(pos), quaternion(quaternion) {}

	virtual void control();

	void go(Math::Vector);

	void setRotation(Math::Quaternion);
	void setPosition(Math::Vector);

	void rotate(Math::Quaternion);
	void move(Math::Vector);

	Math::Vector getPosition() const { return this->position; }
	Math::Quaternion getRotation() const { return this->quaternion; }

	Math::Vector getNormal() const { return normal; }
protected:
	Math::Vector position;
	Math::Quaternion quaternion;
private:
	Math::Vector normal;
};
