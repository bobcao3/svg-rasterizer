#include "rasterizer.hpp"

#include <iostream>

RasterSVG::RasterSVG(std::string file, double dpi)
{
	// Use nanosvg to parse SVG file
	std::cout << "Reading SVG file: " << file << std::endl;
	svg = nsvgParseFromFile(file.data(), "px", dpi);

	filename = file;

	// Prepare render target buffer (host side)
	image = new Image<u8vec4>(ceil(svg->width), ceil(svg->height), u8vec4(0));

	std::cout << image->xsize() << "x" << image->ysize() << std::endl;

	// Render into render target buffer
	LineRenderer line;
	Constants constants;

	for (auto shape = svg->shapes; shape != NULL; shape = shape->next) {
		std::vector<VertexInput> vecs;
		uint32_t stroke_color_packed = shape->stroke.color;
		vec3 stroke_color = vec3(
			(float)((stroke_color_packed      ) & 0xFF) / 256.0f,
			(float)((stroke_color_packed >>  8) & 0xFF) / 256.0f,
			(float)((stroke_color_packed >> 16) & 0xFF) / 256.0f);

		for (auto path = shape->paths; path != NULL;
		     path = path->next) {
			for (int i = 0; i < path->npts - 1; i ++) {
				float *p = &path->pts[i * 2];
				vecs.push_back({ vec2(p[0], p[1]), stroke_color, vec2(0) });
				vecs.push_back({ vec2(p[2], p[3]), stroke_color, vec2(0) });
			}
		}
		line.render(*image, vecs.begin(), vecs.end(), constants);
	}

	// Create texture on GPU if it does not exist,
	// This texture is for rendering our rasterization output
	glGenTextures(1, &image_texture);
	glBindTexture(GL_TEXTURE_2D, image_texture);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// Transfer buffer to GPU as texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image->xsize(), image->ysize(),
		     0, GL_RGBA, GL_UNSIGNED_BYTE, image->data());
}

RasterSVG::~RasterSVG()
{
	glDeleteTextures(1, &image_texture);
}

void RasterSVG::render()
{
	// Create the output panel
	ImGui::Begin(filename.data());
	ImGui::Image((void *)(intptr_t)image_texture,
		     ImVec2(image->xsize(), image->ysize()));
	ImGui::End();
}