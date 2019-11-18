#include "line.hpp"

#include <iostream>

void LineRenderer::VertexShader(VertexInput &input, PixelInput &output,
				Constants &constants)
{
	output.position = vec3(input.position, 1.0) * constants.transformation;
	output.color = input.color;
	output.texcoord = input.texcoord;
}

void LineRenderer::PixelShader(PixelInput &input, PixelOutput &output,
			       Constants &constants)
{
	output.color = input.color;
}

std::vector<PixelInput>
LineRenderer::Rasterize(std::vector<PixelInput>::iterator first_vertex,
			std::vector<PixelInput>::iterator last_vertex,
			Constants &constants)
{
	if (last_vertex - first_vertex != 2) {
		std::cerr << "Line rasterization error: #vertices != 2"
			  << std::endl;
		return {};
	}

	PixelInput p0 = *first_vertex;
	PixelInput p1 = *(first_vertex + 1);

	p0.position = round(p0.position);
	p1.position = round(p1.position);

	vec2 direction = p1.position - p0.position;

	std::vector<PixelInput> fragments;

	// calculate steps required for generating pixels
	int steps = abs(direction.x) > abs(direction.y) ?
				abs(direction.x) :
				abs(direction.y);

	// calculate increment in x & y for each steps
	vec2 inc = direction / (float)steps;

	// Put pixel for each step
	vec2 pos = p0.position;
	for (int i = 0; i <= steps; i++) {
		fragments.push_back({ pos, p0.color, p0.texcoord });
		pos += inc;
	}

	return fragments;
}

void LineRenderer::render(Image<u8vec4> &target,
			  std::vector<VertexInput>::iterator first_vertex,
			  std::vector<VertexInput>::iterator last_vertex,
			  Constants &constants)
{
	for (auto vertex = first_vertex; vertex < last_vertex; vertex += 2) {
		std::vector<PixelInput> points = { {}, {} };

		VertexShader(*vertex, points[0], constants);
		VertexShader(*(vertex + 1), points[1], constants);

		// TODO: MSAA for lines
		auto fragments =
			Rasterize(points.begin(), points.end(), constants);

		#pragma omp parallel for
		for (auto pixel_input : fragments) {
			PixelOutput output;
			PixelShader(pixel_input, output, constants);
			target.set(pixel_input.position.x,
				   pixel_input.position.y,
				   u8vec4(output.color * 255.0f, 255));
		}
	}
}