#include "application.hpp"

#include "../ui/dashboard.hpp"

#include <stdexcept>

#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>


namespace mercury::app
{

Application::Application()
    :
    gateway_{10000, 100000000},
    engine_thread_{gateway_}

{
}

void Application::run()
{
    if (!glfwInit())
    {
        throw std::runtime_error(
            "Failed to initialize GLFW.");
    }

#ifdef __APPLE__

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MAJOR,
        3);

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MINOR,
        2);

    glfwWindowHint(
        GLFW_OPENGL_FORWARD_COMPAT,
        1);

    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE);

#else

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MAJOR,
        3);

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MINOR,
        3);

    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE);

#endif

    GLFWwindow* window =
        glfwCreateWindow(
            1280,
            720,
            "Mercury Exchange",
            nullptr,
            nullptr);

    if (window == nullptr)
    {
        glfwTerminate();

        throw std::runtime_error(
            "Failed to create GLFW window.");
    }


    glfwMakeContextCurrent(window);

   

    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    ImGuiIO& io =
        ImGui::GetIO();

    (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(
        window,
        true);

    ImGui_ImplOpenGL3_Init(
        "#version 150");

    mercury::ui::Dashboard dashboard(
        gateway_);

    engine_thread_.start();

        while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();

        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();

        dashboard.render();

        ImGui::Render();

        int width;
        int height;

        glfwGetFramebufferSize(
            window,
            &width,
            &height);

        glViewport(
            0,
            0,
            width,
            height);

        glClearColor(
            0.10f,
            0.10f,
            0.12f,
            1.0f);

        glClear(
            GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(
            ImGui::GetDrawData());

        glfwSwapBuffers(
            window);
    }

    engine_thread_.stop();

    ImGui_ImplOpenGL3_Shutdown();

    ImGui_ImplGlfw_Shutdown();

    ImGui::DestroyContext();

    glfwDestroyWindow(
        window);

    glfwTerminate();
}

} // namespace mercury::app