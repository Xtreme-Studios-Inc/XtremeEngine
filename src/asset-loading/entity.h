#pragma once

#include <codeframe>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/ext/matrix_transform.hpp>

struct Matrix {
    glm::mat4 transformMatrix;
};

class Entity {
  public:
    Entity();

    const glm::mat4 getMatrix();
    void updateMatrix();

    void resetMatrix();
    void setMatrix(glm::mat4 newMatrix);

    void setPosition(GLfloat x, GLfloat y, GLfloat z);
    void setRotation(GLfloat x, GLfloat y, GLfloat z);
    void setScale(GLfloat x, GLfloat y, GLfloat z);

    void transformPosition(GLfloat x, GLfloat y, GLfloat z);
    void transformRotation(GLfloat x, GLfloat y, GLfloat z);
    void transformScale(GLfloat x, GLfloat y, GLfloat z);

    bool &getTransformType() { return localTransform; };
    glm::vec3 &getPosition() { return position; };
    glm::vec3 &getRotation() { return rotation; };
    glm::vec3 &getScale() { return scale; };

  private:
    bool localTransform = false;
    glm::vec3 position = {0.0f, 0.0f, 0.0f};
    glm::vec3 rotation = {0.0f, 0.0f, 0.0f};
    glm::vec3 scale = {1.0f, 1.0f, 1.0f};

    Matrix matrix;
};
