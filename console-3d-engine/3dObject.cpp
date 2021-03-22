#include <fstream>
#include <string>
#include <sstream>

#include "Colors.h"
#include "3dObject.h"

CHAR_INFO Object::defaultChar = { L'M', BGBLACK | FGLBLUE };

Object* createObjectFromFile(std::string filename)
{
	std::ifstream file(filename);

	if (!file.good())
	{
		return nullptr;
	}

	Object *obj = new Object;
	if (!obj) return nullptr;

	std::string str;
	CHAR_INFO defaultChar = Object::defaultChar;

	while (std::getline(file, str))
	{
		std::stringstream ss(str);
		std::string first;
		ss >> first;

		if (first == "p")
		{
			Object::Point p = { 0.0, 0.0, 0.0, defaultChar };

			ss >> p.pos.x >> p.pos.y >> p.pos.z >> p.color.Char.AsciiChar >> p.color.Attributes;
			obj->points.push_back(p);
		}
		else if (first == "l")
		{
			Object::Line l = { 0, 0, defaultChar };

			ss >> l.points[0] >> l.points[1] >> l.color.Char.AsciiChar >> l.color.Attributes;
			l.points[0]--;
			l.points[1]--;
			obj->lines.push_back(l);
		}
		else if (first == "f")
		{
			Object::Face f = { 0, 0, 0, defaultChar };
			ss >> f.points[0] >> f.points[1] >> f.points[2] >> f.color.Char.AsciiChar >> f.color.Attributes;
			obj->faces.push_back(f);
		}
		else if (first == "pos")
		{
			Math::Vector pos = { 0.0, 0.0, 0.0 };
			ss >> pos.x >> pos.y >> pos.z;
			obj->position = pos;
		}
		else if (first == "size")
		{
			double k;
			if (!(ss >> k))
				k = 1.0;
			obj->resize(k);
		}
		else if (first == "setcolor")
		{
			CHAR_INFO c;
			ss >> c.Char.AsciiChar >> c.Attributes;
			defaultChar = c;
		}
	}

	return obj;
}

Object* createObjectFromOBJFile(std::string filename)
{
	std::ifstream file(filename);

	if (!file.is_open()) return nullptr;

	Object *obj = new Object;

	if (!obj) return nullptr;

	ColoredChar c = { 'M', BGBLACK || FGGREY };

	while (!file.eof())
	{
		std::string line, firstToken;
		std::getline(file, line);
		std::stringstream ss(line);


		ss >> firstToken;

		if (firstToken == "v")
		{
			Object::Point p;
			ss >> p.pos.x >> p.pos.y >> p.pos.z;
			p.color = c;
			obj->points.push_back(p);
		}
		else if (firstToken == "f")
		{
			Object::Face f;
			std::string s;
			unsigned int p1, p2, p3;
			ss >> s; p2 = std::stoi(s) - 1;
			ss >> s; p3 = std::stoi(s) - 1;
			for (int i = 0; ss >> s; i++)
			{
				// TODO: need true triangulation algorithm here
				p1 = p2; p2 = p3;
				p3 = std::stoi(s) - 1;

				obj->faces.push_back({ { p1, p2, p3 }, c });

				/*
				std::stringstream sss(s);
				std::getline(sss, s, '/');
				f.points[i] = std::stoi(s) - 1;
				*/
			}
			//f.color = c;
			//obj->faces.push_back(f);
		}
		else if (firstToken == "setcharcolor")
		{
			ss >> c.Char.AsciiChar >> c.Attributes;
			if (c.Char.AsciiChar == '_')
				c.Char.AsciiChar = ' ';
		}

	}

	file.close();

	return obj;
}

void Object::resize(double k)
{
	for (auto iter = points.begin(); iter != points.end(); iter++)
		iter->pos = iter->pos * k;
}
