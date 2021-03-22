#pragma once

#define DEFAULT_QUATERNION { 1.0, 0.0, 0.0, 0.0 }

namespace Math
{
	/* --- INT TYPES --- */

	struct Point
	{
		int x, y;
	};

	/* --- BASIC --- */

	int round(double);

	/* --- TRIGANOMETRY --- */

	extern const double pi;

	/* --- VECTORS --- */

	struct Vector
	{
		double x, y, z;
	};

	struct Triangle
	{
		Vector v1, v2, v3;
	};

	Vector operator-(const Vector&);
	Vector operator+(const Vector&, const Vector&);
	Vector operator-(const Vector&, const Vector&);
	Vector operator*(const Vector&, double);
	Vector operator/(const Vector&, double);

	double dot(const Vector&, const Vector&);
	Vector cross(const Vector&, const Vector&);

	Vector normalize(const Vector&);
	double len(const Vector&);

	double getProjection(const Vector& axisVector, const Vector& v);
	Vector getProjectionVector(const Vector& axisVector, const Vector& v);

	bool LinePlaneIntersection(const Vector& p1, const Vector& p2, const Vector& plane_normal, const Vector& plane_pos, Vector& intersection);
	int spliceTriangleByPlane(Triangle in, const Vector& plane_pos, const Vector& plane_normal, Triangle &out1, Triangle &out2);

	/* --- 2D MATH --- */

	struct Vector2d
	{
		double x, y;
	};

	struct Triangle2d
	{
		Vector2d v1, v2, v3;
	};

	Point roundToPoint(const Vector2d&);

	Vector2d getLineIntersectionOX(const Vector2d&, const Vector2d&, double y = 0);
	Vector2d getLineIntersectionOY(const Vector2d&, const Vector2d&, double x = 0);

	int boundTriangleInQuad(const Vector2d& qSize, const Triangle2d& t1, Triangle2d& out_t1, Triangle2d& out_12);

	/* --- QUATERNIONS --- */

	struct Quaternion
	{
		double w, x, y, z;
	};

	Quaternion createQuaternion(Vector u, double angle);
	Vector rotate(Vector, Quaternion);
	Quaternion combine(Quaternion, Quaternion);
}