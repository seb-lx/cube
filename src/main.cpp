#include <iostream>
#include <string>
#include <array>
#include <cmath>

#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"

#include "shader.h"
#include "camera.h"
#include "rubiks_cube.h"


// Config
const int windowWidth = 1400;
const int windowHeight = 1000;
const std::string windowTitle = "cube";

// Delta Time
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Camera
Camera camera(glm::vec3(2.0f, 2.0f, 8.0f));

// Mouse movement, camera direction
bool firstMouse = true;
float lastX = (float)windowWidth / 2.0f;
float lastY = (float)windowHeight / 2.0f;

// Rubiks Cube
const float rotationSpeed = 200.0f;
const float cubeSpacing = 1.02f;
const int shuffleSteps = 50;
RubiksCube rubiksCube(rotationSpeed, cubeSpacing, shuffleSteps);


auto process_input(GLFWwindow* window) -> void;
auto framebuffer_size_callback(GLFWwindow* window, int width, int height) -> void;
auto mouse_callback(GLFWwindow* window, double xposIn, double yposIn) -> void;
auto key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) -> void;
auto scroll_callback(GLFWwindow* window, double xoffset, double yoffset) -> void;


auto main() -> int {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        windowWidth,
        windowHeight,
        windowTitle.c_str(),
        nullptr,
        nullptr
    );

    if (!window) {
        std::cout << "failed to create window!\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "failed to initialize glad!\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // Create shader
    Shader shader("rubiks_cube.vert", "rubiks_cube.frag");

    // Initialize rubiks cube object
    rubiksCube.init();

    float vertices[] = {
        // pos.x, pos.y, pos.z, face index
        -0.5f, -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f,
        0.5f,  0.5f, -0.5f, 0.0f,
        0.5f,  0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f,

        -0.5f, -0.5f,  0.5f, 1.0f,
        0.5f, -0.5f,  0.5f, 1.0f,
        0.5f,  0.5f,  0.5f, 1.0f,
        0.5f,  0.5f,  0.5f, 1.0f,
        -0.5f,  0.5f,  0.5f, 1.0f,
        -0.5f, -0.5f,  0.5f, 1.0f,

        -0.5f,  0.5f,  0.5f, 2.0f,
        -0.5f,  0.5f, -0.5f, 2.0f,
        -0.5f, -0.5f, -0.5f, 2.0f,
        -0.5f, -0.5f, -0.5f, 2.0f,
        -0.5f, -0.5f,  0.5f, 2.0f,
        -0.5f,  0.5f,  0.5f, 2.0f,

        0.5f,  0.5f,  0.5f, 3.0f,
        0.5f,  0.5f, -0.5f, 3.0f,
        0.5f, -0.5f, -0.5f, 3.0f,
        0.5f, -0.5f, -0.5f, 3.0f,
        0.5f, -0.5f,  0.5f, 3.0f,
        0.5f,  0.5f,  0.5f, 3.0f,

        -0.5f, -0.5f, -0.5f, 4.0f,
        0.5f, -0.5f, -0.5f, 4.0f,
        0.5f, -0.5f,  0.5f, 4.0f,
        0.5f, -0.5f,  0.5f, 4.0f,
        -0.5f, -0.5f,  0.5f, 4.0f,
        -0.5f, -0.5f, -0.5f, 4.0f,

        -0.5f,  0.5f, -0.5f, 5.0f,
        0.5f,  0.5f, -0.5f, 5.0f,
        0.5f,  0.5f,  0.5f, 5.0f,
        0.5f,  0.5f,  0.5f, 5.0f,
        -0.5f,  0.5f,  0.5f, 5.0f,
        -0.5f,  0.5f, -0.5f, 5.0f,
    };

    unsigned int VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // configure cube
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // set mode
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        process_input(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        // projection matrix
        glm::mat4 projection = glm::perspective(
            glm::radians(camera.m_zoom),
            (float)windowWidth / (float)windowHeight,
            0.1f,
            100.0f
        );
        shader.setMat4("projection", projection);

        // view matrix
        glm::mat4 view = camera.getViewMatrix();
        shader.setMat4("view", view);

        glBindVertexArray(VAO);

        // update and draw rubiks cube
        rubiksCube.update(deltaTime);
        rubiksCube.draw(shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    shader.deleteShader();

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

auto process_input(GLFWwindow* window) -> void {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.processKeyboard(CameraMovement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.processKeyboard(CameraMovement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.processKeyboard(CameraMovement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.processKeyboard(CameraMovement::RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) camera.processKeyboard(CameraMovement::UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) camera.processKeyboard(CameraMovement::DOWN, deltaTime);
}

auto framebuffer_size_callback(GLFWwindow* window, int width, int height) -> void
{
    glViewport(0, 0, width, height);
}

auto mouse_callback(GLFWwindow* window, double xposIn, double yposIn) -> void
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.processMouseMovement(xoffset, yoffset);
}

auto key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) -> void
{
    if (action == GLFW_PRESS) {
        if (rubiksCube.isAnimating()) return;

        if (key == GLFW_KEY_1) {  // rotate front
            auto cfg = RubiksCube::RotationConfig{ .axis = glm::vec3(0, 0, 1), .side = 1, .direction = -1 };
            rubiksCube.initRotation(cfg);
        }
        if (key == GLFW_KEY_F1) { // rotate front inverted
            auto cfg = RubiksCube::RotationConfig{ .axis = glm::vec3(0, 0, 1), .side = 1, .direction = 1 };
            rubiksCube.initRotation(cfg);
        }
        if (key == GLFW_KEY_2) { // rotate back
            auto cfg = RubiksCube::RotationConfig{ .axis = glm::vec3(0, 0, 1), .side = -1, .direction = -1 };
            rubiksCube.initRotation(cfg);
        }
        if (key == GLFW_KEY_F2) { // rotate back inverted
            auto cfg = RubiksCube::RotationConfig{ .axis = glm::vec3(0, 0, 1), .side = -1, .direction = 1 };
            rubiksCube.initRotation(cfg);
        }
        if (key == GLFW_KEY_3) { // rotate left
            auto cfg = RubiksCube::RotationConfig{ .axis = glm::vec3(1, 0, 0), .side = -1, .direction = -1 };
            rubiksCube.initRotation(cfg);
        }
        if (key == GLFW_KEY_F3) { // rotate left inverted
            auto cfg = RubiksCube::RotationConfig{ .axis = glm::vec3(1, 0, 0), .side = -1, .direction = 1 };
            rubiksCube.initRotation(cfg);
        }
        if (key == GLFW_KEY_4) { // rotate right
            auto cfg = RubiksCube::RotationConfig{ .axis = glm::vec3(1, 0, 0), .side = 1, .direction = -1 };
            rubiksCube.initRotation(cfg);
        }
        if (key == GLFW_KEY_F4) { // rotate right inverted
            auto cfg = RubiksCube::RotationConfig{ .axis = glm::vec3(1, 0, 0), .side = 1, .direction = 1 };
            rubiksCube.initRotation(cfg);
        }
        if (key == GLFW_KEY_5) { // rotate up
            auto cfg = RubiksCube::RotationConfig{ .axis = glm::vec3(0, 1, 0), .side = 1, .direction = -1 };
            rubiksCube.initRotation(cfg);
        }
        if (key == GLFW_KEY_F5) { // rotate up inverted
            auto cfg = RubiksCube::RotationConfig{ .axis = glm::vec3(0, 1, 0), .side = 1, .direction = 1 };
            rubiksCube.initRotation(cfg);
        }
        if (key == GLFW_KEY_6) { // rotate down
            auto cfg = RubiksCube::RotationConfig{ .axis = glm::vec3(0, 1, 0), .side = -1, .direction = -1 };
            rubiksCube.initRotation(cfg);
        }
        if (key == GLFW_KEY_F6) { // rotate down inverted
            auto cfg = RubiksCube::RotationConfig{ .axis = glm::vec3(0, 1, 0), .side = -1, .direction = 1 };
            rubiksCube.initRotation(cfg);
        }
        if (key == GLFW_KEY_Q) { // shuffle cube randomly
            rubiksCube.shuffle();
        }
        if (key == GLFW_KEY_L) {
            CubeState s = rubiksCube.getCubeState();
            RubiksCube::printCubeState(s);
        }
    }
}

auto scroll_callback(GLFWwindow* window, double xoffset, double yoffset) -> void
{
    camera.processMouseScroll(static_cast<float>(yoffset));
}
