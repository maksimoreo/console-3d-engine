#pragma once

#include <string>
#include <vector>
#include <Windows.h>

#include "Colors.h"
#include "Transformable.h"

struct Object : public Transformable
{
public:
	static CHAR_INFO defaultChar;
	friend Object * createObjectFromFile(std::string filename);

	struct Point
	{
		Math::Vector pos;
		CHAR_INFO color;
	};

	struct Line
	{
		unsigned int points[2]; // indexes
		CHAR_INFO color;
	};

	struct Face
	{
		unsigned int points[3]; // indexes
		CHAR_INFO color;
	};

	/*
	void drawPoints();
	void drawLines();
	void drawFaces();
	*/

	unsigned int getPointCount() const { return pointCount; }
	unsigned int getLineCount() const { return lineCount; }
	unsigned int getFaceCount() const { return faceCount; }

	void resize(double k);

	std::vector<Point> points;
	std::vector<Line> lines;
	std::vector<Face> faces;
private:
	unsigned int pointCount, lineCount, faceCount;
};

Object* createObjectFromFile(std::string filename);
Object* createObjectFromOBJFile(std::string filename);