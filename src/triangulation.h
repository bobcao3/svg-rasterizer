// Original file Copyright CMU462 Fall 2015: 
// Kayvon Fatahalian, Keenan Crane,
// Sky Gao, Bryce Summers, Michael Choquette.
#pragma once

#include "svg.h"
#include <glm/glm.hpp>

using namespace glm;

// triangulates a polygon and save the result as a triangle list
void triangulate(const Polygon& polygon, std::vector<vec2>& triangles );
