#include "entity.h"

Entity::Entity() {
    position = {0.0f, 0.0f, 0.0f};
    matrix.transformMatrix = glm::mat4(1.0f);
}

const glm::mat4 Entity::getMatrix() { return matrix.transformMatrix; }

#pragma region // Transformation
// Needs to happen every time main runs to update position for all non static obj
void Entity::updateMatrix() {
    glm::mat4 newMatrix(1.0f);

    if (!localTransform) {

        GLfloat angle = (rotation.x + rotation.y + rotation.z);
        if (angle > 0.0f) {
            newMatrix = glm::scale(glm::mat4(1.0f), scale) * glm::translate(glm::mat4(1.0), position) *
                        glm::rotate(glm::mat4(1.0),
                                    glm::radians(angle),
                                    glm::vec3(rotation.x / 360.0f, rotation.y / 360.0f, rotation.z / 360.0f));
        } else {
            newMatrix = glm::scale(glm::mat4(1.0f), scale) * glm::translate(glm::mat4(1.0), position) *
                        glm::rotate(glm::mat4(1.0), glm::radians(angle), glm::vec3(1.0f, 1.0f, 1.0f));
        }
    } else {
        // newMatrix = glm::rotate(newMatrix, glm::radians())
        newMatrix = glm::rotate(newMatrix, glm::radians(rotation.x), glm::vec3(1, 0, 0));
        newMatrix = glm::rotate(newMatrix, glm::radians(rotation.y), glm::vec3(0, 1, 0));
        newMatrix = glm::rotate(newMatrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));
        newMatrix = glm::translate(newMatrix, position);
        newMatrix = glm::scale(newMatrix, scale);
    }

    if (rotation.x > 360.0f) {
        rotation.x -= 360.0f;
    } else if (rotation.x < 0.0f) {
        rotation.x += 360.0f;
    }

    if (rotation.y > 360.0f) {
        rotation.y -= 360.0f;
    } else if (rotation.y < 0.0f) {
        rotation.y += 360.0f;
    }

    if (rotation.z > 360.0f) {
        rotation.z -= 360.0f;
    } else if (rotation.z < 0.0f) {
        rotation.z += 360.0f;
    }

    setMatrix(newMatrix);
}

void Entity::resetMatrix() {
    localTransform = false;
    position = {0.0f, 0.0f, 0.0f};
    rotation = {0.0f, 0.0f, 0.0f};
    scale = {1.0f, 1.0f, 1.0f};

    updateMatrix();
}

void Entity::setMatrix(glm::mat4 newMatrix) { matrix.transformMatrix = newMatrix; }

//--Set Transform
void Entity::setPosition(GLfloat x, GLfloat y, GLfloat z) {
    position = {x, y, z};
    updateMatrix();
}

void Entity::setRotation(GLfloat x, GLfloat y, GLfloat z) {
    rotation = {x, y, z};
    updateMatrix();
}

void Entity::setScale(GLfloat x, GLfloat y, GLfloat z) {
    scale = {x, y, z};
    updateMatrix();
}
//---

//--Transforming
void Entity::transformPosition(GLfloat x, GLfloat y, GLfloat z) {
    position = {position.x + x, position.y + y, position.z + z};
    updateMatrix();
}

void Entity::transformRotation(GLfloat x, GLfloat y, GLfloat z) {
    rotation = {rotation.x + x, rotation.y + y, rotation.z + z};
    updateMatrix();
}

void Entity::transformScale(GLfloat x, GLfloat y, GLfloat z) {
    scale = {scale.x + x, scale.y + y, scale.z + z};
    updateMatrix();
}
//---

#pragma endregion
