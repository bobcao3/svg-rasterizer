#define NANOSVG_IMPLEMENTATION

#include "rasterizer.hpp"
#include "application.hpp"

#include <iostream>

void RasterizerApp::init() {
    // Initialize & read parameters
    this->name = "CS184/284A Project 1: Rasterizer";

    if (options.find("dpi") != options.end()) {
        dpi = stof(options["dpi"]);
        std::cout << "Setting DPI = " << options["dpi"] << std::endl;
    }

    // Create image output for every image in parameter
    for (auto f : params) {
	    images.emplace_back(f, dpi);
    }
}

void RasterizerApp::tick(double time) {
    // Create the Rasterization Info & Control panel
    ImGui::Begin("Rasterization Info & Control");
    ImGui::Text("DPI=%f", dpi);
    ImGui::End();

    for (RasterSVG& im : images) {
        im.render();
    }
}

int main(int argc, char** argv) {
  // The entry point to the program.
  RasterizerApp app;

  return runApplication(app, argc, argv);
}