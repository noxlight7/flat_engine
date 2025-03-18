//
// Created by curo on 01.02.2025.
//
#include <opengl/opengl_shader.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

OpenGLShader::OpenGLShader(const std::string& vertex_path, const std::string& fragment_path) {
    load(vertex_path, fragment_path);
}

OpenGLShader::~OpenGLShader() {
    glDeleteProgram(m_id);
}

void OpenGLShader::load(const std::string& vertex_code, const std::string& fragment_code) {
    GLuint vertex_id = compileShader(GL_VERTEX_SHADER, vertex_code);
    GLuint fragment_id = compileShader(GL_FRAGMENT_SHADER, fragment_code);

    m_id = glCreateProgram();
    glAttachShader(m_id, vertex_id);
    glAttachShader(m_id, fragment_id);
    glLinkProgram(m_id);

    GLint success;
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        char info[512];
        glGetProgramInfoLog(m_id, sizeof(info), nullptr, info);
        std::cerr << "Shader program linking failed:" << std::endl << info << std::endl;
    }

    glDeleteShader(vertex_id);
    glDeleteShader(fragment_id);
}

void OpenGLShader::loadFromFile(const std::string& vertex_path, const std::string& fragment_path) {
    std::string vertex_code = readFile(vertex_path);
    std::string fragment_code = readFile(fragment_path);

    load(vertex_code, fragment_code);
}

void OpenGLShader::use() const {
    glUseProgram(m_id);
}

GLint OpenGLShader::getUniformLocation(const std::string& name) {
    if (m_uniform_cache.contains(name))
        return m_uniform_cache[name];

    GLint location = glGetUniformLocation(m_id, name.c_str());
    if (location == -1)
        std::cerr << "Warning: uniform '" << name << "' not found" << std::endl;

    m_uniform_cache[name] = location;
    return location;
}

void OpenGLShader::setMat4(const std::string& name, const glm::mat4& matrix) {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

void OpenGLShader::setInt(const std::string& name, const int value) {
    glUniform1i(getUniformLocation(name), value);
}

void OpenGLShader::setFloat(const std::string& name, const float value) {
    glUniform1f(getUniformLocation(name), value);
}

void OpenGLShader::setVec2(const std::string& name, const glm::vec2& vector) {
    glUniform2f(getUniformLocation(name), vector.x, vector.y);
}

GLuint OpenGLShader::compileShader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();

    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        char info[512];
        glGetShaderInfoLog(shader, sizeof(info), nullptr, info);
        std::cerr << "Shader compilation failed (" <<
            (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") <<
                "):" << std::endl << info << std::endl;
    }

    return shader;
}

std::string OpenGLShader::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file " + path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
