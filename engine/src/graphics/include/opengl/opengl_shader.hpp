//
// Created by curo on 01.02.2025.
//

#ifndef OPENGL_SHADER_H
#define OPENGL_SHADER_H
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

class OpenGLShader {
    GLuint m_id = 0;
    std::unordered_map<std::string, GLint> m_uniform_cache;

    GLint getUniformLocation(const std::string &name);
    GLuint compileShader(GLenum type, const std::string &source);
    std::string readFile(const std::string &path);
public:
    OpenGLShader() = default;
    OpenGLShader(const std::string& vertex_str, const std::string& fragment_str);
    ~OpenGLShader();

    void loadFromFile(const std::string& vertex_path, const std::string& fragment_path);
    void load(const std::string& vertex_str, const std::string& fragment_str);
    void use() const;

    void setMat4(const std::string& name, const glm::mat4& matrix);
    void setInt(const std::string& name, int value);
    void setFloat(const std::string& name, float value);
    void setVec2(const std::string& name, const glm::vec2& vector);

    GLuint getId() const { return m_id; }
};
#endif //OPENGL_SHADER_H
