#pragma once

#include <vector>
#include <string>
#include <deque>

#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"

#include "shader.h"


struct Cube {
    glm::mat4 model;
    int colorMask;
};

enum class Color { BLUE, GREEN, ORANGE, RED, YELLOW, WHITE, BLACK };

struct CubeState {
    Color front[3][3];
    Color back[3][3];
    Color left[3][3];
    Color right[3][3];
    Color top[3][3];
    Color bottom[3][3];
};

static std::string colorToString(Color c) {
    switch (c) {
    case Color::BLUE: return "B";
    case Color::GREEN: return "G";
    case Color::ORANGE: return "O";
    case Color::RED: return "R";
    case Color::YELLOW: return "Y";
    case Color::WHITE: return "W";
    case Color::BLACK: return "X";
    default: return "?";
    }
}

class RubiksCube {
public:
    struct RotationConfig {
        glm::vec3 axis;
        int side;
        int direction;
    };

public:
    RubiksCube(float rotationSpeed, float cubeSpacing, int shuffleSteps) :
        m_rotationSpeed{ rotationSpeed },
        m_cubeSpacing{ cubeSpacing },
        m_shuffleSteps{ shuffleSteps },
        m_cubes{},
        m_moveQueue{}
    {
    }

    auto init() -> void {
        for (int x = -1; x <= 1; ++x) {
            for (int y = -1; y <= 1; ++y) {
                for (int z = -1; z <= 1; ++z) {
                    auto cube = Cube{};
                    auto pos = glm::vec3(x * m_cubeSpacing, y * m_cubeSpacing, z * m_cubeSpacing);
                    cube.model = glm::translate(glm::mat4(1.0f), pos);

                    // Face 0: Back (Z-), Face 1: Front (Z+), Face 2: Left (X-), Face 3: Right (X+), Face 4: Bottom (Y-), Face 5: Top (Y+)
                    cube.colorMask = 0;
                    if (z == -1) cube.colorMask |= (1 << 0); // Back
                    if (z == 1) cube.colorMask |= (1 << 1); // Front
                    if (x == -1) cube.colorMask |= (1 << 2); // Left
                    if (x == 1) cube.colorMask |= (1 << 3); // Right
                    if (y == -1) cube.colorMask |= (1 << 4); // Bottom
                    if (y == 1) cube.colorMask |= (1 << 5); // Top

                    m_cubes.push_back(cube);
                }
            }
        }
    }

    auto initRotation(RotationConfig cfg) -> void {
        m_isAnimating = true;
        m_rotationAxis = cfg.axis;
        m_rotationSide = cfg.side;
        m_rotationDirection = cfg.direction;
        m_currentAngle = 0.0f;
    }

    auto addMove(const glm::vec3& axis, int side, int direction) -> void {
        m_moveQueue.push_back(
            RotationConfig{
                .axis = axis,
                .side = side,
                .direction = direction
            }
        );
    }

    auto addMove(const RotationConfig& cfg) -> void {
        m_moveQueue.push_back(cfg);
    }

    auto shuffle() -> void {
        // Shuffle cube randomly m_shuffleSteps times
        for (int i = 0; i < m_shuffleSteps; ++i) {
            // generate random rotation config values
            auto cfg = RotationConfig{};

            // axis
            auto randAxis = rand() % 3;
            if (randAxis == 0) cfg.axis = glm::vec3(1.0, 0.0, 0.0);
            else if (randAxis == 1) cfg.axis = glm::vec3(0.0, 1.0, 0.0);
            else cfg.axis = glm::vec3(0.0, 0.0, 1.0);

            // direction
            cfg.direction = (rand() % 2 == 0) ? 1 : -1;

            // side
            cfg.side = (rand() % 2 == 0) ? 1 : -1;

            // queue rotation
            addMove(cfg);
        }
    }

    auto update(float deltaTime) -> void {
        if (!m_isAnimating && !m_moveQueue.empty()) {
            auto queuedRotation = m_moveQueue.front();
            m_moveQueue.pop_front();
            m_rotationAxis = queuedRotation.axis;
            m_rotationDirection = queuedRotation.direction;
            m_rotationSide = queuedRotation.side;
            m_isAnimating = true;
            m_currentAngle = 0.0f;
            m_rotationSpeed = 400.0f;
        }
        else if (!m_isAnimating && m_moveQueue.empty()) {
            m_rotationSpeed = 200.0f;
        }

        if (m_isAnimating) {
            m_currentAngle += m_rotationSpeed * deltaTime;

            // check if rotation complete
            if (m_currentAngle >= m_targetAngle) {
                m_currentAngle = m_targetAngle;

                // complete rotation matrix, target is 90 degrees
                auto axis = m_rotationAxis * float(m_rotationSide);
                auto rotation = glm::rotate(glm::mat4(1.0f), glm::radians(m_targetAngle * m_rotationDirection), axis);

                // update rotating cubes
                for (auto& cube : m_cubes) {
                    auto pos = glm::vec3(cube.model[3]);
                    float dot = glm::dot(pos, m_rotationAxis);

                    // check if cube is rotating
                    if (std::abs(dot - m_rotationSide * m_cubeSpacing) < 0.1f) {
                        cube.model = rotation * cube.model;
                    }
                }

                //fixCubes();
                m_isAnimating = false;
                m_currentAngle = 0.0f;
            }
        }
    }

    auto draw(Shader& shader) -> void {
        for (auto& cube : m_cubes) {
            auto model = cube.model;

            // temporary drawing rotation
            if (m_isAnimating) {
                auto pos = glm::vec3(cube.model[3]);
                float dot = glm::dot(pos, m_rotationAxis);

                // check if cube is rotating
                if (std::abs(dot - m_rotationSide * m_cubeSpacing) < 0.1f) {
                    auto axis = m_rotationAxis * float(m_rotationSide);
                    auto rotation = glm::rotate(glm::mat4(1.0f), glm::radians(m_currentAngle * m_rotationDirection), axis);
                    model = rotation * model;
                }
            }

            shader.setMat4("model", model);
            shader.setInt("colorMask", cube.colorMask);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }

    auto isAnimating() const -> bool { return m_isAnimating; };

    auto getCubeState() const -> CubeState {
        CubeState state;

        // Init black
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                state.front[i][j] = Color::BLACK;
                state.back[i][j] = Color::BLACK;
                state.left[i][j] = Color::BLACK;
                state.right[i][j] = Color::BLACK;
                state.top[i][j] = Color::BLACK;
                state.bottom[i][j] = Color::BLACK;
            }
        }

        // face normals
        glm::vec3 localNormals[6] = {
            glm::vec3(0.0, 0.0, -1.0),  // back
            glm::vec3(0.0, 0.0, 1.0),   // front
            glm::vec3(-1.0, 0.0, 0.0),  // left
            glm::vec3(1.0, 0.0, 0.0),   // right
            glm::vec3(0.0, -1.0, 0.0),  // bottom
            glm::vec3(0.0, 1.0, 0.0),   // top
        };

        // colors
        Color localColors[6] = {
            Color::BLUE, Color::GREEN, Color::ORANGE, Color::RED, Color::YELLOW, Color::WHITE
        };

        for (auto& cube : m_cubes) {
            int x = (int)round(cube.model[3][0] / m_cubeSpacing);
            int y = (int)round(cube.model[3][1] / m_cubeSpacing);
            int z = (int)round(cube.model[3][2] / m_cubeSpacing);

            auto rotation = glm::mat3(cube.model);

            for (int i = 0; i < 6; ++i) {
                if ((cube.colorMask & (1 << i)) == 0) continue;

                auto currentNormal = rotation * localNormals[i];

                // Global Front (Z+)
                if (glm::dot(currentNormal, glm::vec3(0, 0, 1)) > 0.9f) {
                    int col = x + 1;
                    int row = 1 - y;
                    state.front[row][col] = localColors[i];
                }
                // Global Back (Z-)
                else if (glm::dot(currentNormal, glm::vec3(0, 0, -1)) > 0.9f) {
                    int col = 1 - x;
                    int row = 1 - y;
                    state.back[row][col] = localColors[i];
                }
                // Global Right (X+)
                else if (glm::dot(currentNormal, glm::vec3(1, 0, 0)) > 0.9f) {
                    int col = 1 - z;
                    int row = 1 - y;
                    state.right[row][col] = localColors[i];
                }
                // Global Left (X-)
                else if (glm::dot(currentNormal, glm::vec3(-1, 0, 0)) > 0.9f) {
                    int col = z + 1;
                    int row = 1 - y;
                    state.left[row][col] = localColors[i];
                }
                // Global Top (Y+)
                else if (glm::dot(currentNormal, glm::vec3(0, 1, 0)) > 0.9f) {
                    int col = x + 1;
                    int row = z + 1; // z=-1 is top row visually? No, usually z=-1 is back.
                    state.top[row][col] = localColors[i];
                }
                // Global Bottom (Y-)
                else if (glm::dot(currentNormal, glm::vec3(0, -1, 0)) > 0.9f) {
                    int col = x + 1;
                    int row = 1 - z; // z=1 is top row visually
                    state.bottom[row][col] = localColors[i];
                }
            }
        }

        return state;
    }

    static auto printCubeState(const CubeState& s) -> void {
        std::cout << "\nRubik's Cube State:\n";
        std::cout << "      " << colorToString(s.top[0][0]) << colorToString(s.top[0][1]) << colorToString(s.top[0][2]) << "\n";
        std::cout << "      " << colorToString(s.top[1][0]) << colorToString(s.top[1][1]) << colorToString(s.top[1][2]) << "\n";
        std::cout << "      " << colorToString(s.top[2][0]) << colorToString(s.top[2][1]) << colorToString(s.top[2][2]) << "\n";

        for (int i = 0; i < 3; ++i) {
            std::cout << colorToString(s.left[i][0]) << colorToString(s.left[i][1]) << colorToString(s.left[i][2]) << " ";
            std::cout << colorToString(s.front[i][0]) << colorToString(s.front[i][1]) << colorToString(s.front[i][2]) << " ";
            std::cout << colorToString(s.right[i][0]) << colorToString(s.right[i][1]) << colorToString(s.right[i][2]) << " ";
            std::cout << colorToString(s.back[i][0]) << colorToString(s.back[i][1]) << colorToString(s.back[i][2]) << "\n";
        }

        std::cout << "      " << colorToString(s.bottom[0][0]) << colorToString(s.bottom[0][1]) << colorToString(s.bottom[0][2]) << "\n";
        std::cout << "      " << colorToString(s.bottom[1][0]) << colorToString(s.bottom[1][1]) << colorToString(s.bottom[1][2]) << "\n";
        std::cout << "      " << colorToString(s.bottom[2][0]) << colorToString(s.bottom[2][1]) << colorToString(s.bottom[2][2]) << "\n";
    };

private:
    float m_rotationSpeed;
    float m_cubeSpacing;
    int m_shuffleSteps;
    std::vector<Cube> m_cubes;

    std::deque<RotationConfig> m_moveQueue;

    bool m_isAnimating = false;
    float m_currentAngle = 0.0f;
    float m_targetAngle = 90.0f;
    glm::vec3 m_rotationAxis = glm::vec3(0.0f);
    int m_rotationSide = 0;
    int m_rotationDirection = 0;
};
