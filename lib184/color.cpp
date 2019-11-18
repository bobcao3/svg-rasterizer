#include "color.hpp"

vec3 Color::fromHex(const char *s)
{
	// Ignore leading hashmark.
	if (s[0] == '#') {
		s++;
	}

	// Set stream formatting to hexadecimal.
	std::stringstream ss;
	ss << std::hex;

	// Convert to integer.
	unsigned int rgb;
	ss << s;
	ss >> rgb;

	// Extract 8-byte chunks and normalize.
	vec3 c;
	c.r = (float)((rgb & 0xFF0000) >> 16) / 255.0;
	c.g = (float)((rgb & 0x00FF00) >> 8) / 255.0;
	c.b = (float)((rgb & 0x0000FF) >> 0) / 255.0;

	return c;
}

std::string Color::toHex(const vec3 c)
{
	int R = (unsigned char)max(0., min(255.0, 255.0 * c.r));
	int G = (unsigned char)max(0., min(255.0, 255.0 * c.g));
	int B = (unsigned char)max(0., min(255.0, 255.0 * c.b));

	std::stringstream ss;
	ss << std::hex;

	ss << R << G << B;
	return ss.str();
}

const vec3 Color::Black = vec3(0.0);
const vec3 Color::White = vec3(1.0);
