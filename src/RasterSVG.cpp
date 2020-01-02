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
	image = new Image<u8vec4>(ceil(svg.width), ceil(svg.height), u8vec4(255, 255, 255, 255));

	// Render into render target buffer
	std::vector<DrawCommand> cmds;
	svg.draw(cmds, mat3(1.0));

	auto start = high_resolution_clock::now();
	for (DrawCommand &cmd : cmds) {
		cmd.renderer->render(*image, cmd.vertex_buffer.begin(),
							 cmd.vertex_buffer.end(), cmd.constants);
	}
	std::cout << "Render time: "
			  << duration_cast<duration<double> >(high_resolution_clock::now() -
												  start)
						 .count() *
					 1e3
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

	if (ImGui::TreeNodeEx("Pixel Inspector")) {
		ImVec2 cursor_pos = ImGui::GetMousePos();
		ImVec2 window_pos = ImGui::GetWindowPos();
		ImVec2 window_content_min = ImGui::GetWindowContentRegionMin();
		cursor_pos.x -= window_pos.x + window_content_min.x;
		cursor_pos.y -= window_pos.y + window_content_min.y;
		inspecting_pos.x = glm::clamp(cursor_pos.x - inspecting_area.x / 2, .0f,
									  (float)image->xsize());
		inspecting_pos.y = glm::clamp(cursor_pos.y - inspecting_area.y / 2, .0f,
									  (float)image->ysize());
		ImGui::Text("Inspecting Position %f %f", inspecting_pos.x,
					inspecting_pos.y);

		ImGui::Image(
			(void *)(intptr_t)image_texture,
			ImVec2(inspecting_area.x * zoomScale,
				   inspecting_area.y * zoomScale),
			ImVec2(inspecting_pos.x / image->xsize(),
				   inspecting_pos.y / image->ysize()),
			ImVec2((inspecting_pos.x + inspecting_area.x) / image->xsize(),
				   (inspecting_pos.y + inspecting_area.y) / image->ysize()));

		ImGui::TreePop();
	}

	ImGui::End();
}