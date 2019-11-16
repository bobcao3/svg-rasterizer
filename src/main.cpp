#define NANOSVG_IMPLEMENTATION

#include "rasterizer.hpp"
#include "application.hpp"

#include <iostream>

void RasterizerApp::readSVG(std::string file) {
    std::cout << "Reading SVG file: " << file << std::endl;
    svg = nsvgParseFromFile(file.data(), "px", dpi);

    std::cout << "Image size: " << svg->width << "x" << svg->height << std::endl;

    image.resize(svg->width, svg->height);

    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, svg->width, svg->height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, &image[0]);
}

void RasterizerApp::init() {
    this->name = "CS184/284A Project 1: Rasterizer";

    if (options.find("dpi") != options.end()) {
        dpi = stof(options["dpi"]);
        std::cout << "Setting DPI = " << options["dpi"] << std::endl;
    }

    if (params.size() > 0) {
        readSVG(params[0]);
    }
}

void RasterizerApp::tick(double time) {
    ImGui::Text("DPI=%f", dpi);

    ImGui::Begin("SVG Raster Output");
    ImGui::Image((void *)(intptr_t)image_texture,
                 ImVec2(svg->width, svg->height));
    ImGui::End();
}

// The entry point to the program.
int main(int argc, char** argv) {
  RasterizerApp app;

  return runApplication(app, argc, argv);
}