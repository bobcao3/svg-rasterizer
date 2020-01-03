#pragma once

#include "application.hpp"

#include "svg.h"

#include "renderers/line.hpp"

#include <image.hpp>
#include <glm/glm.hpp>

class RasterSVG {
private:
	SVG svg;

	Image<u8vec4> *image;
	GLuint image_texture = 0;

	std::string filename = "SVG output";

public:
	glm::vec2 inspecting_pos = glm::vec2(0);
	glm::vec2 inspecting_area = glm::vec2(32);
	float zoomScale = 4;

	const std::string getFilename() { return filename; };

	RasterSVG(std::string file);
	~RasterSVG();

	void rasterize();
	void render();
};

class RasterizerApp : public Application {
private:
    std::vector<RasterSVG*> images;

public:
    void init();
    void tick(double time);
    void readSVG(std::string file);
};