#include "rasterizer.hpp"
#include "svgparser.h"

#include <iostream>
#include <ratio>
#include <chrono>

using namespace std::chrono;

RasterSVG::RasterSVG(std::string file)
{
	// Use nanosvg to parse SVG file
	std::cout << "Reading SVG file: " << file << std::endl;

	SVGParser::load(file.data(), &svg);

	filename = file;

	// Prepare render target buffer (host side)
	image = new Image<u8vec4>(ceil(svg.width), ceil(svg.height), u8vec4(0));

	// Render into render target buffer
	std::vector<DrawCommand> cmds;
	svg.draw(cmds, mat3(1.0));

	auto start = high_resolution_clock::now();
	for (DrawCommand &cmd : cmds) {
		cmd.renderer->render(*image, cmd.vertex_buffer.begin(),
							 cmd.vertex_buffer.end(), cmd.constants);
	}
	std::cout << "Render time: "
			  << duration_cast<duration<double>>(
					 high_resolution_clock::now() - start).count() * 1e3
			  << " ms" << std::endl;

	// Create texture on GPU if it does not exist,
	// This texture is for rendering our rasterization output
	glGenTextures(1, &image_texture);
	glBindTexture(GL_TEXTURE_2D, image_texture);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// Transfer buffer to GPU as texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image->xsize(), image->ysize(), 0,
				 GL_RGBA, GL_UNSIGNED_BYTE, image->data());
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