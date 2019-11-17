#pragma once

#include "pipeline.hpp"

class LineRenderer : public Renderer {
    public:
	void VertexShader(VertexInput &input, PixelInput &output,
			  Constants &constants);

	void PixelShader(PixelInput &input, PixelOutput &output,
			 Constants &constants);

	std::vector<PixelInput>
	Rasterize(std::vector<PixelInput>::iterator first_vertex,
		  std::vector<PixelInput>::iterator last_vertex,
		  Constants &constants);

	void render(Image<u8vec4> &target,
		    std::vector<VertexInput>::iterator first_vertex,
		    std::vector<VertexInput>::iterator last_vertex,
		    Constants &constants);
};