#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


enum class CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f; // fov

class Camera {
public:
    Camera(
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = YAW,
        float pitch = PITCH
    );

    Camera(
        float posX, float posY, float posZ,
        float upX, float upY, float upZ,
        float yaw, float pitch
    );

    auto getCustomLookAtMat4(glm::vec3 position, glm::vec3 target, glm::vec3 up) -> glm::mat4;
    auto getViewMatrix() -> glm::mat4;
    auto processKeyboard(CameraMovement direction, float deltaTime) -> void;
    auto processMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch = true) -> void;
    auto processMouseScroll(float yOffset) -> void;

public:
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;

    float m_yaw;
    float m_pitch;

    float m_movementSpeed;
    float m_mouseSensitivity;
    float m_zoom;

private:
    auto updateCameraVectors() -> void;
};