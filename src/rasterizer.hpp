#pragma once

#include "application.hpp"

#include "nanosvg.h"

#include "renderers/line.hpp"

#include <image.hpp>
#include <glm/glm.hpp>

class RasterSVG {
private:
	NSVGimage *svg = NULL;

	Image<u8vec4> *image;
	GLuint image_texture = 0;

	std::string filename = "SVG output";

public:
	RasterSVG(std::string file, double dpi);
	~RasterSVG();

	void rasterize();
	void render();
};

class RasterizerApp : public Application {
private:
    float dpi = 96.0f;

    std::vector<RasterSVG> images;

public:
    void init();
    void tick(double time);
    void readSVG(std::string file);
};