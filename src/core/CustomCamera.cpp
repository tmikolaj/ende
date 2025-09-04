#include "CustomCamera.hpp"

#include <imgui.h>
#include <iostream>
#include <ostream>

const Camera3D* CustomCamera::getCamera() const {
    return &camera;
}

void CustomCamera::init() {
    camera = { 0 };
    camera.position = { 23.2637, 13.3959, 19.1924 };
    camera.target = { 0, 0, 0 };
    camera.up = { 0, 1, 0 };
    camera.fovy = 45;
    camera.projection = CAMERA_PERSPECTIVE;

    position = { camera.position.x, camera.position.y, camera.position.z };
    target = { 0, 0, 0 };

    rotateSpeed = 0.002f;
    zoomSpeed = 2.0f;
    panSpeed = 0.001f;

    distance = 33;
    pitch = 0.418002;
    yaw = 0.881;
}

void CustomCamera::update() {
    // -----------------
    // Scroll handling
    // -----------------

    if (!ImGui::GetIO().WantCaptureMouse) {
        bool shouldUpdate = false;
        float prevDistance = distance;

        distance -= GetMouseWheelMove() * zoomSpeed;
        if (distance != prevDistance) shouldUpdate = true;

        // Measuring mouse move between frames
        Vector2 mouseDelta = GetMouseDelta();

        // ------------------
        // Rotation handling
        // ------------------
        if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON) && !IsKeyDown(KEY_LEFT_SHIFT)) {
            yaw += -mouseDelta.x * rotateSpeed;
            pitch += mouseDelta.y * rotateSpeed;

            const float maxPitch = glm::radians(89.0f);
            pitch = glm::clamp(pitch, -maxPitch, maxPitch);

            shouldUpdate = true;
        }

        // -----------------
        // Panning handling
        // -----------------
        if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON) && IsKeyDown(KEY_LEFT_SHIFT)) {
            glm::vec3 direction = glm::normalize(target - position);
            // returns which way is right
            glm::vec3 right = glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f)));
            // returns which way is up
            glm::vec3 up = glm::normalize(glm::cross(right, direction));

            target += -right * mouseDelta.x * panSpeed * distance;
            target += up * mouseDelta.y * panSpeed * distance;

            shouldUpdate = true;
        }

        if (shouldUpdate) {
            // POSITION = TARGET + OFFSET

            // cosf(pitch) = horizontal radius from target
            // sinf(pitch) = vertical offset from target
            // sinf(yaw) and cosf(yaw) = circular orbit around target in XZrrr
            position.x = target.x + distance * cosf(pitch) * sinf(yaw);
            position.y = target.y + distance * sinf(pitch);
            position.z = target.z + distance * cosf(pitch) * cosf(yaw);

            camera.position = { position.x, position.y, position.z };
            camera.target = { target.x, target.y, target.z };
        }
    }
}

void CustomCamera::changeRotateSpeed(float newRotSpeed) {
    rotateSpeed = newRotSpeed;
}

void CustomCamera::changeZoomSpeed(float newZoomSpeed) {
    zoomSpeed = newZoomSpeed;
}

void CustomCamera::changePanSpeed(float newPanSpeed) {
    panSpeed = newPanSpeed;
}

float CustomCamera::getDistance() const {
    return distance;
}