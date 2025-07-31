#ifndef CUSTOMCAMERA_HPP
#define CUSTOMCAMERA_HPP

#include <external/glm/glm.hpp>
#include "raylib.h"

class CustomCamera {
private:
    Camera3D camera;

    glm::vec3 target;
    float distance;
    float yaw; // horizontal rotation (similar to turning head side to side)
    float pitch; // vertical rotation (similar to looking up and down)
    glm::vec3 position;

    float rotateSpeed;
    float zoomSpeed;
    float panSpeed;
public:
    CustomCamera() = default;
    ~CustomCamera() = default;

    [[nodiscard]] const Camera3D* getCamera() const;
    void init();
    void update();

    void changeRotateSpeed(float newRotSpeed);
    void changeZoomSpeed(float newZoomSpeed);
    void changePanSpeed(float newPanSpeed);

    float getDistance() const;
};

#endif //CUSTOMCAMERA_HPP
