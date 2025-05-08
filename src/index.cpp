#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const char *vertexShaderSrc = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec2 aTexCoord;
    out vec2 TexCoord;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    void main() {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        TexCoord = aTexCoord;
    }
    )";

const char *fragmentShaderSrc = R"(
    #version 330 core
    in vec2 TexCoord;
    out vec4 FragColor;
    uniform sampler2D texture1;
    void main() {
        FragColor = texture(texture1, TexCoord);
    }
    )";

std::vector<float> platformVertices = {
    -10.0f, 0.0f, -10.0f,  0.0f, 0.0f, 
    10.0f, 0.0f, -10.0f,  1.0f, 0.0f, 
    10.0f, 0.0f,  10.0f,  1.0f, 1.0f, 
    -10.0f, 0.0f, -10.0f,  0.0f, 0.0f, 
    10.0f, 0.0f,  10.0f,  1.0f, 1.0f,
    -10.0f, 0.0f,  10.0f,  0.0f, 1.0f 
};

glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float yaw = -90.0f;
float pitch = 0.0f;
float verticalVelocity = 0.0f;
const float gravity = -9.8f;
const float jumpStrength = 4.5f;
bool isJumping = false;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void processInput(SDL_Window *window, bool &running)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
            running = false;
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
            running = false;
        if (event.type == SDL_MOUSEMOTION)
        {
            float xoffset = event.motion.xrel;
            float yoffset = -event.motion.yrel;
            float sensitivity = 0.1f;
            xoffset *= sensitivity;
            yoffset *= sensitivity;
            yaw += xoffset;
            pitch += yoffset;

            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;

            glm::vec3 front;
            front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            front.y = sin(glm::radians(pitch));
            front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            cameraFront = glm::normalize(front);
        }
    }

    const float cameraSpeed = 2.5f * deltaTime;
    glm::vec3 newPos = cameraPos;
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_W])
        newPos += cameraSpeed * glm::vec3(cameraFront.x, 0.0f, cameraFront.z);
    if (state[SDL_SCANCODE_S])
        newPos -= cameraSpeed * glm::vec3(cameraFront.x, 0.0f, cameraFront.z);
    if (state[SDL_SCANCODE_A])
        newPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (state[SDL_SCANCODE_D])
        newPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (state[SDL_SCANCODE_SPACE] && !isJumping)
    {
        verticalVelocity = jumpStrength;
        isJumping = true;
    }

    verticalVelocity += gravity * deltaTime;
    newPos.y += verticalVelocity * deltaTime;

    if (newPos.y <= 1.0f)
    {
        newPos.y = 1.0f;
        verticalVelocity = 0.0f;
        isJumping = false;
    }

    newPos.x = glm::clamp(newPos.x, -10.0f, 10.0f);
    newPos.z = glm::clamp(newPos.z, -10.0f, 10.0f);
    cameraPos = newPos;
}

GLuint compileShader(const char *source, GLenum type)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Shader compilation failed: " << infoLog << std::endl;
    }
    return shader;
}

GLuint createShaderProgram()
{
    GLuint vertexShader = compileShader(vertexShaderSrc, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentShaderSrc, GL_FRAGMENT_SHADER);
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Program linking failed: " << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}

GLuint loadTexture(const char *path)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cerr << "Failed to load texture: " << path << std::endl;
    }
    stbi_image_free(data);
    return textureID;
}

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window *window = SDL_CreateWindow("Сгусток", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
    if (!window)
    {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext)
    {
        std::cerr << "GL context creation failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "GLEW initialization failed" << std::endl;
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, platformVertices.size() * sizeof(float), platformVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLuint shaderProgram = createShaderProgram();
    GLuint grassTexture = loadTexture("src/textures/grass.png");

    bool running = true;
    while (running)
    {
        float currentFrame = SDL_GetTicks() / 1000.0f;
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, running);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, grassTexture);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);

        glBindVertexArray(VAO);
        glm::mat4 model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        SDL_GL_SwapWindow(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &grassTexture);
    glDeleteProgram(shaderProgram);
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}