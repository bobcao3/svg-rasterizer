#define NANOSVG_IMPLEMENTATION

#include "rasterizer.hpp"
#include "application.hpp"

#include <iostream>

void RasterizerApp::init() {
    // Initialize & read parameters
    this->name = "CS184/284A Project 1: Rasterizer";

    // Create image output for every image in parameter
    for (auto f : params) {
	    images.emplace_back(f);
    }
}

void RasterizerApp::tick(double time) {
    for (RasterSVG& im : images) {
        im.render();
    }
}

int main(int argc, char** argv) {
  // The entry point to the program.
  RasterizerApp app;

  return runApplication(app, argc, argv);
}