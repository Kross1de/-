#pragma once

#include <GL/glew.h>
#include <string>
#include <iostream>

class Shader {
public:
    Shader(const char* vertexSource, const char* fragmentSource);
    ~Shader();
    
    void use();
    GLuint getProgram() const { return program; }
    
private:
    GLuint program;
    GLuint compileShader(const char* source, GLenum type);
};