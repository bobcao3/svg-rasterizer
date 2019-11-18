#pragma once

#include <glm/glm.hpp>

#include <ostream>
#include <sstream>
#include <string>

using namespace glm;

struct Color {
	static vec3 fromHex(const char *s);
	static std::string toHex(const vec3 c);

	static const vec3 Black;
	static const vec3 White;
};