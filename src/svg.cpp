// Original file Copyright CMU462 Fall 2015:
// Kayvon Fatahalian, Keenan Crane,
// Sky Gao, Bryce Summers, Michael Choquette.
#include "svg.h"

#include "triangulation.h"
#include <iostream>

#include "lodepng.h"

#include "renderers/line.hpp"

LineRenderer line_renderer;

Group::~Group()
{
	for (size_t i = 0; i < elements.size(); i++) {
		delete elements[i];
	}
	elements.clear();
}

SVG::~SVG()
{
	for (size_t i = 0; i < elements.size(); i++) {
		delete elements[i];
	}
	elements.clear();
}

// Draw routines //

void Triangle::draw(std::vector<DrawCommand> &dr, mat3 global_transform)
{
	global_transform = global_transform * transform;
	/*
  vec2 p0_scr = global_transform * p0_svg;
  vec2 p1_scr = global_transform * p1_svg;
  vec2 p2_scr = global_transform * p2_svg;

  // draw fill. Here the color field is empty, since children
  // export their own more sophisticated color() method.
  dr->rasterize_triangle( p0_scr.x, p0_scr.y, p1_scr.x, p1_scr.y, p2_scr.x, p2_scr.y, vec3(), this );
  */
}

void Group::draw(std::vector<DrawCommand> &dr, mat3 global_transform)
{
	global_transform = global_transform * transform;

	for (int i = 0; i < elements.size(); ++i)
		elements[i]->draw(dr, global_transform);
}

void Point::draw(std::vector<DrawCommand> &dr, mat3 global_transform)
{
	global_transform = global_transform * transform;
	vec2 p = global_transform * vec3(position, 1.0);
	//dr->rasterize_point(p.x, p.y, style.fillColor);
}

void Line::draw(std::vector<DrawCommand> &dr, mat3 global_transform)
{
	global_transform = global_transform * transform;

	if (style.strokeVisible) {
		DrawCommand cmd = { &line_renderer,
							{ { from, style.strokeColor, vec2(0.0) },
							  { to, style.strokeColor, vec2(0.0) } },
							{ nullptr, global_transform, 1 } };
		dr.push_back(cmd);
	}
}

void Polyline::draw(std::vector<DrawCommand> &dr, mat3 global_transform)
{
	global_transform = global_transform * transform;

	vec3 c = style.strokeColor;

	int nPoints = points.size();
	for (int i = 0; i < nPoints - 1; i++) {
		vec2 p0(points[(i + 0) % nPoints]);
		vec2 p1(points[(i + 1) % nPoints]);
		DrawCommand cmd = { &line_renderer,
							{ { p0, style.strokeColor, vec2(0.0) },
							  { p1, style.strokeColor, vec2(0.0) } },
							{ nullptr, global_transform, 1 } };
		dr.push_back(cmd);
	}
}

void Rect::draw(std::vector<DrawCommand> &dr, mat3 global_transform)
{
	global_transform = global_transform * transform;

	vec3 c;

	// draw as two triangles
	float x = position.x, y = position.y;
	float w = dimension.x, h = dimension.y;

	vec2 p0 = global_transform * vec3(x, y, 1.0);
	vec2 p1 = global_transform * vec3(x + w, y, 1.0);
	vec2 p2 = global_transform * vec3(x, y + h, 1.0);
	vec2 p3 = global_transform * vec3(x + w, y + h, 1.0);

	// draw fill
	/*
  c = style.fillColor;
  dr->rasterize_triangle( p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, c );
  dr->rasterize_triangle( p2.x, p2.y, p1.x, p1.y, p3.x, p3.y, c );
  */

	// draw outline
	if (style.strokeVisible) {
		DrawCommand cmd = { &line_renderer,
							{
								{ vec2(p0.x, p0.y), c, vec2(0.0) },
								{ vec2(p1.x, p1.y), c, vec2(0.0) },
								{ vec2(p1.x, p1.y), c, vec2(0.0) },
								{ vec2(p3.x, p3.y), c, vec2(0.0) },
								{ vec2(p3.x, p3.y), c, vec2(0.0) },
								{ vec2(p2.x, p2.y), c, vec2(0.0) },
								{ vec2(p2.x, p2.y), c, vec2(0.0) },
								{ vec2(p0.x, p0.y), c, vec2(0.0) },
							},
							{ nullptr, global_transform, 1 } };
		dr.push_back(cmd);
	}
}

void Polygon::draw(std::vector<DrawCommand> &dr, mat3 global_transform)
{
	global_transform = global_transform * transform;

	vec3 c;

	// draw fill
	c = style.fillColor;

	// triangulate
	std::vector<vec2> triangles;
	triangulate(*this, triangles);

	// draw as triangles
	for (size_t i = 0; i < triangles.size(); i += 3) {
		/*
    vec2 p0 = global_transform * triangles[i + 0];
    vec2 p1 = global_transform * triangles[i + 1];
    vec2 p2 = global_transform * triangles[i + 2];
    dr->rasterize_triangle( p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, c );
    */
	}

	// draw outline
	if (style.strokeVisible) {
		c = style.strokeColor;
		int nPoints = points.size();
		for (int i = 0; i < nPoints; i++) {
			vec2 p0(points[(i + 0) % nPoints]);
			vec2 p1(points[(i + 1) % nPoints]);
			DrawCommand cmd = { &line_renderer,
								{ { p0, style.strokeColor, vec2(0.0) },
								  { p1, style.strokeColor, vec2(0.0) } },
								{ nullptr, global_transform, 1 } };
			dr.push_back(cmd);
		}
	}
}

void SVGImage::draw(std::vector<DrawCommand> &dr, mat3 global_transform)
{
	global_transform = global_transform * transform;
	vec2 p0 = global_transform * vec3(position, 1.0);
	vec2 p1 = global_transform * vec3(position + dimension, 1.0);

	for (int x = floor(p0.x); x <= floor(p1.x); ++x) {
		for (int y = floor(p0.y); y <= floor(p1.y); ++y) {
			/*
      vec3 col = tex.sample_bilinear(vec2((x+.5-p0.x)/(p1.x-p0.x+1), (y+.5-p0.y)/(p1.y-p0.y+1)));
      dr->rasterize_point(x,y,col);
      */
		}
	}
}
