#pragma once

#include "application.hpp"

#include "nanosvg.h"

class RasterizerApp : public Application {
private:
    NSVGimage* svg = NULL;
    float dpi = 96.0f;

    std::vector<uint8_t> image;
    GLuint image_texture;

public:
    void init();
    void tick(double time);
    void readSVG(std::string file);
};