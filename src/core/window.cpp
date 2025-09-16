#include "window.h"

#include <Windows.h>
#include <backends/imgui_impl_opengl3_loader.h>

Window::Window() {
#pragma region // Console
    //::ShowWindow(::GetConsoleWindow(), SW_HIDE);
    //::ShowWindow(::GetConsoleWindow(), SW_MINIMIZE); //Still visible in the task bar
    ::ShowWindow(::GetConsoleWindow(), SW_SHOW);
//::IsWindowVisible(::GetConsoleWindow()) != FALSE;
#pragma endregion

    mainWindow = nullptr;
    for (size_t i = 0; i < 1024; i++) {
        keys[i] = 0;
    }
}

void Window::getMaxMonitorResolution(int *w, int *h, int *refresh) {
    int count;
    const GLFWvidmode *modes = glfwGetVideoModes(glfwGetPrimaryMonitor(), &count);

    int maxWidth = 0;
    int maxHeight = 0;
    int maxRefresh = 0;
    for (int i = 0; i < count; i++) {
        if (modes[i].width > maxWidth)
            maxWidth = modes[i].width;
        if (modes[i].height > maxHeight)
            maxHeight = modes[i].height;
        if (modes[i].refreshRate > maxRefresh)
            maxRefresh = modes[i].refreshRate;
    }

    console.logLn("MaxWidth: " + toString(maxWidth) + "MaxHeight: " + toString(maxHeight) +
                  "MaxRefreshRate: " + toString(maxRefresh));

    *w = maxWidth;
    *h = maxHeight;
    *refresh = maxRefresh;
}

bool Window::initWindow(std::string wName = "Test Name", const int width = 1280, const int height = 720) {
    // Initialise GLFW
    glfwInit();

#ifdef OPENGL
    // Set Version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Forwards Compatabillity
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#elif VULKAN
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // SET GLFW TO NOT WORK WITH OPENGL
    // glfwWindowHint(GLFW_REFRESH_RATE, GLFW_FALSE);
    glfwWindowHint(GLFW_AUTO_ICONIFY, GL_FALSE); // DISABLES AUTOMATIC MINIMIZING
    // glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE); // ENABLE WINDOW RESIZE ABILLITY
#else
    console.error("ERROR : NO GRAPHICS API DEFINED!");
#endif

    if (mainWindow == nullptr) {
        mainWindow = glfwCreateWindow(width, height, wName.c_str(), glfwGetPrimaryMonitor(), NULL);
    }

    if (!mainWindow) {
        console.error("Window creation failed!");
        glfwDestroyWindow(mainWindow);
        return false;
    }

    // mainWindow = glfwCreateWindow(width, height, wName.c_str(), NULL, NULL);
    glfwSetWindowAspectRatio(mainWindow, 16, 9);

    // int monitorCount;
    // GLFWmonitor **monitors = glfwGetMonitors(&monitorCount);

    int maxHeight;
    int maxWidth;
    int maxRefreshRate;

    getMaxMonitorResolution(&maxHeight, &maxWidth, &maxRefreshRate);
    // glfwGetMonitorPhysicalSize(monitors[0], &maxWidth, &maxHeight);
    // std::cout << "Max Monitor Resolution: " << maxHeight << " : " << maxWidth << std::endl;
    // int maxHeight = glfwGetMonitor

    glfwSetWindowSizeLimits(mainWindow, width, height, maxWidth, maxHeight);

    createCallbacks();
    glfwSetCursorPos(mainWindow, 0, 0);
    // glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowUserPointer(mainWindow, this); // Creates pointer on the window handle
    // glfwSetWindowUserPointer(window, this);
    // glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

    glfwMakeContextCurrent(mainWindow); // MIGHT NOT WORK WITH VULKAN // TODO

    if (mainWindow) {
        return true;
    } else {
        return false;
    }
}

void Window::createCallbacks() {
    glfwSetKeyCallback(mainWindow, keyCallback);
    glfwSetCursorPosCallback(mainWindow, cursorPositionCallback);
    glfwSetMouseButtonCallback(mainWindow, mouseButtonCallback);
    glfwSetScrollCallback(mainWindow, scrollCallback);
}

#pragma region // Input Callbacks

double Window::cursorXChange() {
    double change = xChange;
    xChange = 0;
    return change;
}

double Window::cursorYChange() {
    double change = yChange;
    yChange = 0;
    return change;
}

// Keyboard Input
void Window::keyCallback(GLFWwindow *window, int key, int code, int action, int mode) {
    Window *w = static_cast<Window *>(glfwGetWindowUserPointer(window));

    // Exit Application Action [Hold Escape]
    if (key == GLFW_KEY_ESCAPE && action == GLFW_REPEAT) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    // F11 to Toggle Fullscreen Mode
    if (key == GLFW_KEY_F11 && action == GLFW_PRESS) {

        GLFWmonitor *currentMonitor = glfwGetWindowMonitor(w->getWindow());
        int maxWidth;
        int maxHeight;
        int maxRefreshRate;
        w->getMaxMonitorResolution(&maxWidth, &maxHeight, &maxRefreshRate);

        if (currentMonitor == nullptr) {
            console.logLn("MODE - FULLSCREEN");
            glfwSetWindowMonitor(w->getWindow(), glfwGetPrimaryMonitor(), 0, 0, 1920, 1080, maxRefreshRate);
        } else {
            console.logLn("MODE - WINDOWED");

            glfwSetWindowMonitor(w->getWindow(), nullptr, 0, 0, 1280, 720, maxRefreshRate);
            glfwSetWindowAspectRatio(w->getWindow(), 16, 9);

            glfwRestoreWindow(w->getWindow());
            glfwMaximizeWindow(w->getWindow());

            glfwSetWindowSizeLimits(w->getWindow(), 1280, 720, maxWidth, maxHeight);
        }
    }

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            w->keys[key] = true;
        } else if (action == GLFW_RELEASE) {
            w->keys[key] = false;
        }
    }
}

// cursor Position Callback
void Window::cursorPositionCallback(GLFWwindow *window, double xPos, double yPos) {
    // Get Pointer to window because function is static
    Window *w = static_cast<Window *>(glfwGetWindowUserPointer(window));

    /*if (w->mouseFirstMoved)
    {
            w->lastX = xPos;
            w->lastY = yPos;
            w->mouseFirstMoved = false;
    }*/
    if (!w->CursorNormal) {

        if (w->mouseFirstMoved) {
            w->lastX = xPos;
            w->lastY = yPos;
            w->mouseFirstMoved = false; // next move will produce real deltas
            w->xChange = 0.0;
            w->yChange = 0.0;
            return;
        }
        // Position change is = to diffrence between last position and current position
        w->xChange += xPos - w->lastX;
        w->yChange += w->lastY - yPos;

        // glm::vec2 p = glm::clamp(glm::vec2(w->xChange, w->yChange), glm::vec2(-100.0f, -100.0f),
        // glm::vec2(100.0f, 100.0f));

        // Set last position to current position
        w->lastX = xPos;
        w->lastY = yPos;
    } else { // if (w->lastX != xPos)
        w->lastX = xPos;
        w->lastY = yPos;

        w->mouseFirstMoved = true;
        w->xChange = w->yChange = 0.0;
    }
}

void Window::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    Window *w = static_cast<Window *>(glfwGetWindowUserPointer(window));

    // When Right Mouse Button is Down Cursor disapears and we can move around the screen
    if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS) {
        // w->SelectAction = false;

        // if (w->SelectAction == true)
        //{
        //	GLbyte color[4];
        //	GLfloat depth;
        //	GLuint index;

        //	/*glReadPixels(xPos, w->bufferHeight + yPos - 1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, color);
        //	glReadPixels(xPos, w->bufferHeight + yPos - 1, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
        //	glReadPixels(xPos, w->bufferHeight + yPos - 1, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT,
        //&index);*/

        //	glReadPixels(w->bufferWidth / 2, w->bufferHeight / 2, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE,
        // color); 	glReadPixels(w->bufferWidth / 2, w->bufferHeight / 2, 1, 1, GL_DEPTH_COMPONENT,
        // GL_FLOAT, &depth); 	glReadPixels(w->bufferWidth / 2, w->bufferHeight / 2, 1, 1,
        // GL_STENCIL_INDEX, GL_UNSIGNED_INT, &index);

        //	console.logLn("Clicked on pixel %d, %d, color %02hhx%02hhx%02hhx%02hhx, depth %f, stencil index %u\n",
        //		w->bufferWidth / 2, w->bufferHeight / 2, color[0], color[1], color[2], color[3],
        // depth, index);

        //	const glm::vec4 viewport = glm::vec4(0, 0, w->bufferWidth, w->bufferHeight);
        //	const glm::vec3 wincoord = glm::vec3(w->bufferWidth / 2, w->bufferHeight / 2, depth);
        //	GLuint view = w->shaders->getUniformView();
        //	//GLuint projection = w->shaders->getUniformProjection();
        //	const glm::mat4 modelview = w->shaders->GetViewMatrix();
        //	const glm::mat4 projection = glm::perspective(45.0f, (GLfloat)w->bufferWidth /
        //(GLfloat)w->bufferHeight, 0.1f, 100.0f);

        //	glm::vec3 objcoord = glm::unProject(wincoord, modelview, projection, viewport);// wincoord, view,
        // projection, viewport

        //	console.logLn("Coordinates in object space: %f, %f, %f\n",
        //		objcoord.x, objcoord.y, objcoord.z);
        //}

        if (w->CursorNormal == true) {

            console.logLn("Disable Cursor\n");
            //-- Raw mouse motion
            // When the cursor is disabled, raw(unscaled and unaccelerated) mouse motion can be enabled if available.
            // Raw mouse motion is closer to the actual motion of the mouse across a surface.It is not affected by the
            // scalingand acceleration applied to the motion of the desktop cursor.That processing is suitable for a
            // cursor while raw motion is better for controlling for example a 3D camera.Because of this, raw mouse
            // motion is only provided when the cursor is disabled.
            if (glfwRawMouseMotionSupported())
                glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

            // Disable Cursor
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            w->CursorNormal = false;
        }
    }

    if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_RELEASE) {
        if (w->CursorNormal == false) {
            console.logLn("Enable Cursor\n");
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            w->CursorNormal = true;
        }
    }
}

// Scroll input
void Window::scrollCallback(GLFWwindow *window, double xOffset, double yOffset) {
    Window *w = static_cast<Window *>(glfwGetWindowUserPointer(window));

    if (w->scrollPos + yOffset < 10.0f && w->scrollPos + yOffset > 0.0f) {
        w->scrollChanges = yOffset;
        w->scrollPos += yOffset;
    }
}
#pragma endregion