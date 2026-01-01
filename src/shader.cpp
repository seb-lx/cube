#include "shader.h"

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>


Shader::Shader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath)
{
    std::string vertexShaderSourceStr = readTextFile(vertexPath);
    std::string fragmentShaderSourceStr = readTextFile(fragmentPath);

    if (vertexShaderSourceStr.empty()) throw std::runtime_error("vertexShaderSourceStr is empty!");
    if (fragmentShaderSourceStr.empty()) throw std::runtime_error("fragmentShaderSourceStr is empty!");

    // Vertex shader
    unsigned int vertexShaderHandle = glCreateShader(GL_VERTEX_SHADER);
    int vertexShaderSuccess;
    char vertexShaderInfoLog[512];

    const char* vertexShaderSource = vertexShaderSourceStr.c_str();
    glShaderSource(vertexShaderHandle, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShaderHandle);

    glGetShaderiv(vertexShaderHandle, GL_COMPILE_STATUS, &vertexShaderSuccess);
    if (!vertexShaderSuccess) {
        glGetShaderInfoLog(vertexShaderHandle, 512, nullptr, vertexShaderInfoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << vertexShaderInfoLog << "\n";

        glDeleteShader(vertexShaderHandle);
        throw std::runtime_error("vertex shader could not be compiled!");
    }

    // Fragment shader
    unsigned int fragmentShaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
    int fragmentShaderSuccess;
    char fragmentShaderInfoLog[512];

    const char* fragmentShaderSource = fragmentShaderSourceStr.c_str();
    glShaderSource(fragmentShaderHandle, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShaderHandle);

    glGetShaderiv(fragmentShaderHandle, GL_COMPILE_STATUS, &fragmentShaderSuccess);
    if (!fragmentShaderSuccess) {
        glGetShaderInfoLog(fragmentShaderHandle, 512, nullptr, fragmentShaderInfoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << fragmentShaderInfoLog << "\n";

        glDeleteShader(fragmentShaderHandle);
        throw std::runtime_error("fragment shader could not be compiled!");
    }

    // Create shader program
    int programSuccess;
    char programInfoLog[512];

    ID = glCreateProgram();
    glAttachShader(ID, vertexShaderHandle);
    glAttachShader(ID, fragmentShaderHandle);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &programSuccess);
    if (!programSuccess) {
        glGetProgramInfoLog(ID, 512, nullptr, programInfoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << programInfoLog << "\n";

        glDeleteShader(vertexShaderHandle);
        glDeleteShader(fragmentShaderHandle);
        throw std::runtime_error("shader program could not be linked!");
    }

    glDeleteShader(vertexShaderHandle);
    glDeleteShader(fragmentShaderHandle);
}

void Shader::use()
{
    glUseProgram(ID);
}

void Shader::deleteShader()
{
    glDeleteProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setVec2(const std::string& name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void Shader::setMat2(const std::string& name, const glm::mat2& value) const
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setMat3(const std::string& name, const glm::mat3& value) const
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}
