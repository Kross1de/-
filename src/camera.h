#pragma once

#include <glm/glm.hpp>
#include <SDL2/SDL.h>

class Camera {
public:
    Camera();
    void processInput(SDL_Window* window, bool& running, float deltaTime);
    glm::mat4 getViewMatrix() const;
    glm::vec3 getPosition() const { return position; }
    
private:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    float yaw;
    float pitch;
};
