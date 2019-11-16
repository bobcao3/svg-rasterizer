#pragma once

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <imgui.h>

#include <imgui/examples/imgui_impl_opengl3.h>
#include <imgui/examples/imgui_impl_glfw.h>

#include <unordered_map>
#include <string>
#include <vector>

class Application {
public:
    // Options are parameters in the form of: Option=XXX
    std::unordered_map<std::string, std::string> options;
    // Params are parameters that is not a option
    std::vector<std::string> params;

    // Application name
    std::string name;

    int width = 1024, height = 600;

    // GLFW window
    GLFWwindow* window;

    virtual void init() = 0;
    virtual void tick(double time) = 0;
};

int runApplication(Application& app, int argc, char** argv);