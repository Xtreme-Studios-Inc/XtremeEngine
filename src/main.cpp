#include "definitions.h"
#include "renderer/GUI_Renderer.h"

#ifdef VULKAN
#include "renderer/vulkan-renderer/vulkan-renderer.h"
VulkanRenderer Renderer;
#elif OPENGL
#include "renderer/OpenGL_Renderer/OpenGL_Renderer.h"
OpenGL_Renderer Renderer;
#else
#endif

#include "core/time.h"

Window window;
Time appTime;
Camera camera;

/**
 * The Entry Point
 */
int main() {
    // CREATE WINDOW
    if (!window.initWindow("WIN", 1920, 1080)) { // 1280, 720
        return EXIT_FAILURE;
    }

#ifdef VULKAN
    // CREATE VULKAN INSTANCE -- needs the window so we get the window
    if (Renderer.initVulkanRenderer(window.getWindow()) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }
#elif OPENGL
    // CREATE VULKAN INSTANCE -- needs the window so we get the window
    if (Renderer.init_OpenGL_Renderer(window.getWindow()) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }
#endif

    float deltaTime = 0.0f;

    // camera needs to be set as active to change the vulkan view matrix
    camera.setAsActive(Renderer.getViewMatrix());

    int counter = 0;

    // PROGRAM LOOP
    while (!window.shouldClose()) {
        glfwPollEvents();

        appTime.startTimeTracker();

        deltaTime = appTime.getDeltaTime();
        appTime.getFps();

        camera.checkInputs(window.getCursorMode(), window.getKeys(), window.getScroll(), deltaTime);
        camera.setCamera(window.getCursorMode(), window.cursorXChange(), window.cursorYChange(), deltaTime);

        Renderer.draw();
    }

    // CLEAN UP
    Renderer.shutdownRenderer();

    glfwDestroyWindow(window.getWindow());
    glfwTerminate();
}
