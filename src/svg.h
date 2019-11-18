// Original file Copyright CMU462 Fall 2015:
// Kayvon Fatahalian, Keenan Crane,
// Sky Gao, Bryce Summers, Michael Choquette.
#pragma once

#include <map>
#include <vector>
#include <string>

#include "tinyxml2.h"
using namespace tinyxml2;

#include <glm/glm.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtc/constants.hpp>
using namespace glm;

#include "renderers/texture.h"
#include "renderers/pipeline.hpp"

typedef enum e_SVGElementType {
	NONE = 0,
	POINT,
	LINE,
	POLYLINE,
	RECT,
	POLYGON,
	ELLIPSE,
	IMAGE,
	GROUP,
	TRIANGLE
} SVGElementType;

struct Style {
	vec3 strokeColor;
	vec3 fillColor;
	float strokeWidth;
	float miterLimit;
	bool strokeVisible;
};

struct SVGElement {
	SVGElement(SVGElementType _type) : type(_type), transform(mat3(1.0))
	{
	}

	virtual ~SVGElement()
	{
	}

	virtual void draw(std::vector<DrawCommand>& dr, mat3 global_transform) = 0;

	// primitive type
	SVGElementType type;

	// styling
	Style style;

	// transformation list
	mat3 transform;
};

struct Triangle : SVGElement {
	Triangle() : SVGElement(TRIANGLE)
	{
	}
	vec2 p0_svg, p1_svg, p2_svg;

	void draw(std::vector<DrawCommand>& dr, mat3 global_transform);
};

struct ColorTri : Triangle {
	// Per-vertex colors. Should be interpolated between using
	// barycentric coordinates.
	vec3 p0_col, p1_col, p2_col;
};

struct TexTri : Triangle {
	// Per-vertex uv coordinates.
	// Should be interpolated between using barycentric coordinates.
	vec2 p0_uv, p1_uv, p2_uv;
	Texture *tex;
};

struct Group : SVGElement {
	Group() : SVGElement(GROUP)
	{
	}
	std::vector<SVGElement *> elements;

	void draw(std::vector<DrawCommand>& dr, mat3 global_transform);

	~Group();
};

struct Point : SVGElement {
	Point() : SVGElement(POINT)
	{
	}
	vec2 position;

	void draw(std::vector<DrawCommand>& dr, mat3 global_transform);
};

struct Line : SVGElement {
	Line() : SVGElement(LINE)
	{
	}
	vec2 from;
	vec2 to;

	void draw(std::vector<DrawCommand>& dr, mat3 global_transform);
};

struct Polyline : SVGElement {
	Polyline() : SVGElement(POLYLINE)
	{
	}
	std::vector<vec2> points;

	void draw(std::vector<DrawCommand>& dr, mat3 global_transform);
};

struct Rect : SVGElement {
	Rect() : SVGElement(RECT)
	{
	}
	vec2 position;
	vec2 dimension;

	void draw(std::vector<DrawCommand>& dr, mat3 global_transform);
};

struct Polygon : SVGElement {
	Polygon() : SVGElement(POLYGON)
	{
	}
	std::vector<vec2> points;

	void draw(std::vector<DrawCommand>& dr, mat3 global_transform);
};

struct SVGImage : SVGElement {
	SVGImage() : SVGElement(IMAGE)
	{
	}
	vec2 position;
	vec2 dimension;
	Texture tex;

	void draw(std::vector<DrawCommand>& dr, mat3 global_transform);
};

struct SVG {
	~SVG();
	float width, height;
	std::vector<SVGElement *> elements;
	std::map<std::string, Texture *> textures;

	void draw(std::vector<DrawCommand>& dr, mat3 global_transform)
	{
		for (int i = 0; i < elements.size(); ++i)
			elements[i]->draw(dr, global_transform);
	}
};