#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera() 
    : position(0.0f, 1.0f, 0.0f)
    , front(0.0f, 0.0f, -1.0f)
    , up(0.0f, 1.0f, 0.0f)
    , yaw(-90.0f)
    , pitch(0.0f) {
}

void Camera::processInput(SDL_Window* window, bool& running, float deltaTime) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            running = false;
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
            running = false;
        if (event.type == SDL_MOUSEMOTION) {
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

            front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            front.y = sin(glm::radians(pitch));
            front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            front = glm::normalize(front);
        }
    }

    const float cameraSpeed = 2.5f * deltaTime;
    const Uint8* state = SDL_GetKeyboardState(NULL);
    
    if (state[SDL_SCANCODE_W])
        position += cameraSpeed * front;
    if (state[SDL_SCANCODE_S])
        position -= cameraSpeed * front;
    if (state[SDL_SCANCODE_A])
        position -= glm::normalize(glm::cross(front, up)) * cameraSpeed;
    if (state[SDL_SCANCODE_D])
        position += glm::normalize(glm::cross(front, up)) * cameraSpeed;
    if (state[SDL_SCANCODE_SPACE])
        position += cameraSpeed * up;
    if (state[SDL_SCANCODE_LSHIFT])
        position -= cameraSpeed * up;
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}
