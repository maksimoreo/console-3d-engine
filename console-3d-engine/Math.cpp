#include "Math.h"

#include <cmath>
#include <algorithm>
#include <assert.h>

namespace Math
{

	int round(double number)
	{
		return (int)(number + 0.5);
	}

	const double pi = std::acos(-1);

	Vector operator-(const Vector& v)
	{
		return { -v.x, -v.y, -v.z };
	}

	Vector operator+(const Vector& v1, const Vector& v2)
	{
		return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
	}

	Vector operator-(const Vector& v1, const Vector& v2)
	{
		return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
	}

	Vector operator*(const Vector& v, double n)
	{
		return { v.x * n, v.y * n, v.z * n };
	}

	Vector operator/(const Vector& v, double n)
	{
		return { v.x / n, v.y / n, v.z / n };
	}


	double dot(const Vector& v1, const Vector& v2)
	{
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	Vector cross(const Vector& v1, const Vector& v2)
	{
		return { v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x };
	}


	Vector normalize(const Vector& v)
	{
		double n = len(v);
		return v / n;
	}

	double len(const Vector& v)
	{
		return std::sqrt(dot(v, v));
	}


	double getProjection(const Vector& axisVector, const Vector& v)
	{
		return dot(axisVector, v) / len(axisVector);
	}

	Vector getProjectionVector(const Vector& axisVector, const Vector& v)
	{
		return axisVector * getProjection(axisVector, v) / len(axisVector);
	}


	bool LinePlaneIntersection(const Vector& p1, const Vector& p2, const Vector& plane_normal, const Vector& plane_pos, Vector& intersection)
	{
		Math::Vector line_pos = p1,
			line_dir = p2 - p1;
		double smt = Math::dot(line_dir, plane_normal);
		if (smt == 0.0)
		{
			// line is parallel to the plane
			if (Math::dot((plane_pos - line_pos), plane_normal) == 0)
			{
				// line is in the plane (every point on the line is also on the plane)
				intersection = line_pos;
				return true;
			}
			else
			{
				intersection = line_pos;
				return false;
			}
		}
		else
		{
			double d = Math::dot((plane_pos - line_pos), plane_normal) / smt;
			intersection = line_pos + line_dir * d;
			return (d >= 0.0 && d <= 1.0);
		}
	}

	int spliceTriangleByPlane(Triangle in, const Vector& plane_pos, const Vector& plane_normal, Triangle &out1, Triangle &out2)
	{
		bool v1_is_inside = Math::dot(plane_normal, in.v1 - plane_pos) >= 0.0,
			v2_is_inside = Math::dot(plane_normal, in.v2 - plane_pos) >= 0.0,
			v3_is_inside = Math::dot(plane_normal, in.v3 - plane_pos) >= 0.0;
		
		// rotate triangle, so that v1 is inside, then v2, then v3

		if (!v1_is_inside && !v2_is_inside && v3_is_inside)
		{
			Math::Vector temp = in.v1;
			in.v1 = in.v3;
			in.v3 = in.v2;
			in.v2 = temp;

			v1_is_inside = true; v2_is_inside = false; v3_is_inside = false;
		}
		if (!v1_is_inside && v2_is_inside && !v3_is_inside)
		{
			Math::Vector temp = in.v1;
			in.v1 = in.v2;
			in.v2 = in.v3;
			in.v3 = temp;

			v1_is_inside = true; v2_is_inside = false; v3_is_inside = false;
		}
		if (!v1_is_inside && v2_is_inside && v3_is_inside)
		{
			Math::Vector temp = in.v1;
			in.v1 = in.v2;
			in.v2 = in.v3;
			in.v3 = temp;

			v1_is_inside = true; v2_is_inside = true; v3_is_inside = false;
		}
		if (v1_is_inside && !v2_is_inside && v3_is_inside)
		{
			Math::Vector temp = in.v1;
			in.v1 = in.v3;
			in.v3 = in.v2;
			in.v2 = temp;

			v1_is_inside = true; v2_is_inside = true; v3_is_inside = false;
		}

		assert(v1_is_inside == (Math::dot(plane_normal, in.v1 - plane_pos) >= 0.0));
		assert(v2_is_inside == (Math::dot(plane_normal, in.v2 - plane_pos) >= 0.0));
		assert(v3_is_inside == (Math::dot(plane_normal, in.v3 - plane_pos) >= 0.0));

		int number_of_points_inside = int(v1_is_inside) + int(v2_is_inside) + int(v3_is_inside);


		switch (number_of_points_inside)
		{
		case 0:
			return 0;
			break;
		case 1:
			out1.v1 = in.v1;
			/*LinePlaneIntersection(in.v1, in.v2, plane_normal, plane_pos, out1.v2);
			LinePlaneIntersection(in.v1, in.v3, plane_normal, plane_pos, out1.v3);*/
			assert(LinePlaneIntersection(in.v1, in.v2, plane_normal, plane_pos, out1.v2));
			assert(LinePlaneIntersection(in.v1, in.v3, plane_normal, plane_pos, out1.v3));
			return 1;
			break;
		case 2:
			Math::Vector intersection1, intersection2;
			/*LinePlaneIntersection(in.v1, in.v3, plane_normal, plane_pos, intersection1);
			LinePlaneIntersection(in.v2, in.v3, plane_normal, plane_pos, intersection2);*/
			assert(LinePlaneIntersection(in.v1, in.v3, plane_normal, plane_pos, intersection1));
			assert(LinePlaneIntersection(in.v2, in.v3, plane_normal, plane_pos, intersection2));
			out1.v1 = in.v1;
			out1.v2 = intersection2/* + plane_normal*/;
			out1.v3 = intersection1/* + plane_normal*/;

			out2.v1 = in.v1;
			out2.v2 = in.v2;
			out2.v3 = intersection2/* + plane_normal*/;

			return 2;
			break;
		case 3:
			out1 = in;
			number_of_points_inside = 1;
			return 1;
			break;
		}
	}


	Point roundToPoint(const Vector2d& v)
	{
		return { (int)(v.x + 0.5), (int)(v.y + 0.5) };
	}

	Vector2d getLineIntersectionOX(const Vector2d& v1, const Vector2d& v2, double y)
	{
		// no check
		return { v1.x + (v2.x - v1.x) * v1.y / (v1.y - v2.y), 0 };
	}

	Vector2d getLineIntersectionOY(const Vector2d& v1, const Vector2d& v2, double x)
	{
		return { 0, v2.y - (v2.y - v1.y) * v2.x / (v2.x - v1.x) };
	}

	int boundTriangleInQuad(const Vector2d & qSize, const Triangle2d & t1, Triangle2d & out_t1, Triangle2d & out_12)
	{

		return 0;
	}


	Quaternion createQuaternion(Vector u, double angle)
	{
		double halfangle = angle / 2;
		double sinangle = std::sin(halfangle);
		return { std::cos(halfangle), u.x * sinangle, u.y * sinangle, u.z * sinangle };

	}
	Vector rotate(Vector v, Quaternion q)
	{
		Vector u = { q.x, q.y, q.z };
		double w = q.w;

		//working, but not optimised algorithm
		//n = v * w + cross(u, v)
		//return u * dot(u, v) + n * w + cross(n, -u)

		//Vector3d n = add<3, double>(mul(v, q[0]), cross(u, v));
		Vector n = v * q.w + cross(u, v);

		//return Vector3d(mul<3, double>(u, dot(u, v)) + mul<3, double>(n, w) + cross<double>(n, mul<3, double>(u, -1)));
		return u * dot(u, v) + n * w + cross(n, u * -1);
	}
	Quaternion combine(Quaternion a, Quaternion b)
	{
		/*Quaternion out;
		out[0] = a[0] * b[0] - a[1] * b[1] - a[2] * b[2] - a[3] * b[3];
		out[1] = a[0] * b[1] + a[1] * b[0] + a[2] * b[3] - a[3] * b[2];
		out[2] = a[0] * b[2] - a[1] * b[3] + a[2] * b[0] + a[3] * b[1];
		out[3] = a[0] * b[3] + a[1] * b[2] - a[2] * b[1] + a[3] * b[0];
		return out;*/

		return { a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z, a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
			a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x, a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w };
	}
}