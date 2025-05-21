#pragma once

#include <GL/glew.h>
#include <vector>

class Cursor {
public:
    Cursor();
    ~Cursor();
    
    void render();
    
private:
    GLuint VAO, VBO;
    GLuint shaderProgram;
    
    static const char* vertexShaderSource;
    static const char* fragmentShaderSource;
    static const std::vector<float> vertices;
};
