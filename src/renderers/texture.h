#pragma once

#include <glm/glm.hpp>
using namespace glm;

#include <iostream>
#include <vector>

#include "image.hpp"

typedef enum PixelSampleMethod { P_NEAREST = 0, P_LINEAR = 1 } PixelSampleMethod;
typedef enum LevelSampleMethod { L_ZERO = 0, L_NEAREST = 1, L_LINEAR = 2 } LevelSampleMethod;

struct SampleParams {
  vec2 p_uv;
  vec2 p_dx_uv, p_dy_uv;
  PixelSampleMethod psm;
  LevelSampleMethod lsm;
};

static const int kMaxMipLevels = 14;

struct MipLevel {
	size_t width;
	size_t height;
  // RGB color values
  std::vector<unsigned char> texels;

  vec3 get_texel(int tx, int ty);
};

struct Texture {
  size_t width;
  size_t height;
  std::vector<MipLevel> mipmap;

  void init(const std::vector<unsigned char>& pixels, const size_t& w, const size_t& h) {
    width = w; height = h;

    // A fancy C++11 feature. emplace_back constructs the element in place,
    // and in this case it uses the new {} list constructor syntax.
    mipmap.emplace_back(MipLevel{width, height, pixels});

    generate_mips();
  }

  // Generates up to kMaxMipLevels of mip maps. Level 0 contains
  // the unfiltered original pixels.
  void generate_mips(int startLevel = 0);

  vec3 sample(const SampleParams &sp);
  float get_level(const SampleParams &sp);

  vec3 sample_nearest(vec2 uv, int level = 0);

  vec3 sample_bilinear(vec2 uv, int level = 0);
};
