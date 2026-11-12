#pragma once
#include <GL/glew.h>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

// Interfaz compatible con los ayudantes Model y Mesh del laboratorio.
// Liberar Program antes de destruir el contexto OpenGL.
class Shader {
    static std::string read(const char* path) {
        std::ifstream input(path);
        if (!input) throw std::runtime_error(std::string("No se pudo abrir: ") + path);
        std::ostringstream result;
        result << input.rdbuf();
        return result.str();
    }
    static GLuint compile(GLenum type, const std::string& source) {
        GLuint shader = glCreateShader(type);
        const char* text = source.c_str();
        glShaderSource(shader, 1, &text, nullptr);
        glCompileShader(shader);
        GLint ok = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
        if (!ok) {
            char log[2048] = {};
            glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
            glDeleteShader(shader);
            throw std::runtime_error(log);
        }
        return shader;
    }
public:
    GLuint Program = 0;
    Shader(const char* vertexPath, const char* fragmentPath) {
        const auto vertexText = read(vertexPath);
        const auto fragmentText = read(fragmentPath);
        const GLuint vertex = compile(GL_VERTEX_SHADER, vertexText);
        GLuint fragment = 0;
        try { fragment = compile(GL_FRAGMENT_SHADER, fragmentText); }
        catch (...) { glDeleteShader(vertex); throw; }
        Program = glCreateProgram();
        glAttachShader(Program, vertex);
        glAttachShader(Program, fragment);
        glLinkProgram(Program);
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        GLint ok = GL_FALSE;
        glGetProgramiv(Program, GL_LINK_STATUS, &ok);
        if (!ok) {
            char log[2048] = {};
            glGetProgramInfoLog(Program, sizeof(log), nullptr, log);
            glDeleteProgram(Program);
            Program = 0;
            throw std::runtime_error(log);
        }
    }
    void Use() const { glUseProgram(Program); }
    GLint getColorLocation() const { return glGetUniformLocation(Program, "color"); }
};
