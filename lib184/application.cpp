#include "application.hpp"

#include <iostream>

int runApplication(Application& app, int argc, char** argv) {
  // Parse all input parameters and put them into a hashmap
  for (int i = 1; i < argc; i++) {
    // Convert into std::string
    std::string param(argv[i]);

    // Find the '='
    size_t index = param.find_first_of("=");

    if (index == std::string::npos) {
      // If '=' is not found, then push it to params
      app.params.push_back(param);
    } else {
      // If '=' is found, then it is a option.
      app.options.emplace(param.substr(0, index), param.substr(index + 1));
    }
  }

  // Initialize GLFW window
  if (!glfwInit()) {
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  app.window =
      glfwCreateWindow(app.width, app.height, app.name.data(), NULL, NULL);
  if (!app.window) {
    return -1;
  }

  // Create OpenGL context, enables Vsync by default `glfwSwapInterval(1)`
  glfwMakeContextCurrent(app.window);
  gladLoadGL();
  glfwSwapInterval(1);

  glfwGetFramebufferSize(app.window, &app.width, &app.height);
  glViewport(0, 0, app.width, app.height);
  glClearColor(0.45f, 0.55f, 0.60f, 1.00f);

  // Setup IMGUI
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(app.window, true);
  ImGui_ImplOpenGL3_Init("#version 150");
  // Setup Dear ImGui style
  ImGui::StyleColorsLight();

  // Call application init
  app.init();

  // Starts the render loop
  while (!glfwWindowShouldClose(app.window)) {
    // Poll events and prepare for a frame
    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT);

    // feed inputs to dear imgui, start new frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Call the render tick of Application
    double time = glfwGetTime();
    app.tick(time);

    // Render dear imgui into screen
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap buffers
    glfwSwapBuffers(app.window);
    glfwGetFramebufferSize(app.window, &app.width, &app.height);
  }

  // Terminate
  glfwDestroyWindow(app.window);
  glfwTerminate();

  return 0;
}