#include "camera.h"

#include <iostream>

Camera::Camera(
    glm::vec3 position,
    glm::vec3 up,
    float yaw,
    float pitch
): 
    m_position{ position },
    m_worldUp{ up },
    m_yaw{ yaw },
    m_pitch{ pitch },
    m_front{ glm::vec3(0.0f, 0.0f, -1.0f) },
    m_movementSpeed{ SPEED },
    m_mouseSensitivity{ SENSITIVITY },
    m_zoom{ ZOOM }
{
    updateCameraVectors();
}

Camera::Camera(
    float posX, float posY, float posZ,
    float upX, float upY, float upZ,
    float yaw, float pitch
) :
    m_position{ glm::vec3(posX, posY, posZ) },
    m_worldUp{ glm::vec3(upX, upY, upZ) },
    m_yaw{ yaw },
    m_pitch{ pitch },
    m_front{ glm::vec3(0.0f, 0.0f, -1.0f) },
    m_movementSpeed{ SPEED },
    m_mouseSensitivity{ SENSITIVITY },
    m_zoom{ ZOOM }
{
    updateCameraVectors();
}

auto Camera::getCustomLookAtMat4(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp) -> glm::mat4
{
    auto direction = glm::normalize(position - target);
    auto right = glm::normalize(glm::cross(glm::normalize(worldUp), direction));
    auto up = glm::cross(direction, right);

    /*auto rotate = glm::mat4{
        right.x, right.y, right.z, 0.0f,
        up.x, up.y, up.z, 0.0f,
        direction.x, direction.y, direction.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };


    auto translate = glm::mat4{
        1.0f, 0.0f, 0.0f, -position.x,
        0.0f, 1.0f, 0.0f, -position.y,
        0.0f, 0.0f, 1.0f, -position.z,
        0.0f, 0.0f, 0.0f, 1.0f
    };*/

    glm::mat4 translation = glm::mat4(1.0f); // Identity matrix by default
    translation[3][0] = -position.x; // Fourth column, first row
    translation[3][1] = -position.y;
    translation[3][2] = -position.z;
    glm::mat4 rotation = glm::mat4(1.0f);
    rotation[0][0] = right.x; // First column, first row
    rotation[1][0] = right.y;
    rotation[2][0] = right.z;
    rotation[0][1] = up.x; // First column, second row
    rotation[1][1] = up.y;
    rotation[2][1] = up.z;
    rotation[0][2] = direction.x; // First column, third row
    rotation[1][2] = direction.y;
    rotation[2][2] = direction.z;

    return rotation * translation;
}

auto Camera::getViewMatrix() -> glm::mat4
{
    //return getCustomLookAtMat4(m_position, m_position + m_front, m_up);
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

auto Camera::processKeyboard(CameraMovement direction, float deltaTime) -> void
{
    float velocity = m_movementSpeed * deltaTime;
    if (direction == CameraMovement::FORWARD) m_position += m_front * velocity;
    if (direction == CameraMovement::BACKWARD) m_position -= m_front * velocity;
    if (direction == CameraMovement::LEFT) m_position -= m_right * velocity;
    if (direction == CameraMovement::RIGHT) m_position += m_right * velocity;

    if (direction == CameraMovement::UP) m_position += m_up * velocity;
    if (direction == CameraMovement::DOWN) m_position -= m_up * velocity;

    //std::cout << "pos: " << m_position.x << ", " << m_position.y << ", " << m_position.z << "\n";

    //m_position.y = 0.0f;
}

auto Camera::processMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch) -> void
{
    xOffset *= m_mouseSensitivity;
    yOffset *= m_mouseSensitivity;

    m_yaw += xOffset;
    m_pitch += yOffset;

    if (constrainPitch) {
        if (m_pitch > 89.0f) m_pitch = 89.0f;
        if (m_pitch < -89.0f) m_pitch = -89.0f;
    }

    updateCameraVectors();
}

auto Camera::processMouseScroll(float yOffset) -> void
{
    m_zoom -= (float)yOffset;
    if (m_zoom < 1.0f) m_zoom = 1.0f;
    if (m_zoom > 45.0f) m_zoom = 45.0f;
}

auto Camera::updateCameraVectors() -> void
{
    glm::vec3 front(
        cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)),
        sin(glm::radians(m_pitch)),
        sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch))
    );
    m_front = glm::normalize(front);

    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}
