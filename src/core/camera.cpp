#include "camera.h"

#include <algorithm>

// Making a camera with defaul position and rotation
Camera::Camera() {
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    yaw = -90.0f;
    pitch = 0.0f;
    roll = 0.0f;

    front = glm::vec3(0.0f, 0.0f, -1.0f);
}

// Making a camera with a new position and rotation
Camera::Camera(glm::vec3 startPosition,
               glm::vec3 startUp,
               GLfloat startYaw,
               GLfloat startPitch,
               GLfloat startMoveSpeed,
               GLfloat startTurnSpeed) {
    position = startPosition;
    worldUp = startUp;
    yaw = startYaw;
    pitch = startPitch;

    front = glm::vec3(0.0f, 0.0f, -1.0f);

    moveSpeed = startMoveSpeed;
    turnSpeed = startTurnSpeed;

    update();
}

// Calculating the view Matrix for the camera
bool Camera::setAsActive(glm::mat4 *currentViewMatrix) {
    if (currentViewMatrix != nullptr) {
        viewMatrix = currentViewMatrix;
        return true;
    }

    return false;
}

// change the direction values and then run the update function to calculate the new direction
void Camera::setCamera(bool cursorMode, GLfloat xChange, GLfloat yChange, float deltaTime) {
    // if cursor mode this means cursor is active and we dont want to move
    if (cursorMode != false)
        return;

    // // Smoothing
    // // Optional tiny smoothing (EMA); keeps feel crisp without lag
    // static float sx = 0.0f, sy = 0.0f;
    // const float alpha = 0.2f; // 0.15–0.3 is typical
    // sx = (1.0f - alpha) * sx + alpha * xChange;
    // sy = (1.0f - alpha) * sy + alpha * yChange;

    // // Optional tiny deadzone to kill sensor noise
    // auto dz = [](float v) { return (std::fabs(v) < 0.02f) ? 0.0f : v; };
    // float dx = dz(sx);
    // float dy = dz(sy);

    // // ✅ No dt here — use pure sensitivity
    // yaw += dx * mouseSensitivity;
    // pitch += dy * mouseSensitivity;

    // ✅ Use raw deltas directly
    yaw += xChange * mouseSensitivity;
    pitch += yChange * mouseSensitivity;

    // Clamp pitch, wrap yaw (prevents precision drift)
    pitch = std::clamp(pitch, -89.0f, 89.0f);
    if (yaw > 360.0f)
        yaw -= 360.0f;
    if (yaw < 0.0f)
        yaw += 360.0f;

    update();
}

// WASD movement Code
void Camera::checkInputs(bool cursorMode, bool *keys, float scroll, GLfloat deltaTime) {
    // if cursor mode this means cursor is active and we dont want to move
    if (cursorMode != false) {
        return;
    }
    glm::vec3 newPos = glm::vec3(0.0f, 0.0f, 0.0f);
    // move forward
    if (keys[GLFW_KEY_W])
        newPos += front;

    // move back
    if (keys[GLFW_KEY_S])
        newPos -= front;

    // move left
    if (keys[GLFW_KEY_A])
        newPos -= right;

    // move right
    if (keys[GLFW_KEY_D])
        newPos += right;

    // move down
    if (keys[GLFW_KEY_Q])
        newPos -= worldUp;
    // move up
    if (keys[GLFW_KEY_E])
        newPos += worldUp;

    position += (newPos * moveSpeed * scroll) * deltaTime;
}

// update camera direction
void Camera::update() {
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);

    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));

    // Updating the given view Matrix
    if (viewMatrix != nullptr) {
        glm::mat4 newViewMatrix = lookAt(position, position + front, up);
        *viewMatrix = newViewMatrix;
    }
}
