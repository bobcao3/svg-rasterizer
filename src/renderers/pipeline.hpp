#pragma once

#include "application.hpp"
#include "glm/glm.hpp"

#include <image.hpp>

#include <iterator>
#include <memory>

using namespace glm;

#include "texture.h"

struct VertexInput {
	vec2 position;
	vec3 color;
	vec2 texcoord;
};

struct PixelInput {
	vec2 position;
	vec3 color;
	vec2 texcoord;
};

struct PixelOutput {
    vec3 color;
};

struct Constants {
	std::shared_ptr<Texture> texture;
	mat3 transformation;
	int msaa_count;
};

class Renderer {
public:
    virtual void VertexShader(VertexInput& input,
                              PixelInput& output,
                              Constants& constants) = 0;

    virtual void PixelShader(PixelInput &input,
                             PixelOutput &output,
			                 Constants &constants) = 0;

    virtual std::vector<PixelInput> Rasterize(
        std::vector<PixelInput>::iterator first_vertex,
        std::vector<PixelInput>::iterator last_vertex,
        Constants &constants
    ) = 0;

    virtual void render(Image<u8vec4> &target,
			std::vector<VertexInput>::iterator first_vertex,
			std::vector<VertexInput>::iterator last_vertex,
			Constants &constants) = 0;
};

struct DrawCommand {
	Renderer* renderer;
	std::vector<VertexInput> vertex_buffer;
	Constants constants;
};