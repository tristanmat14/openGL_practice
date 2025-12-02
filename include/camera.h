#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class CameraMovement {
    Forward,
    Backward,
    Left,
    Right,
};

// Defualt camera options
static constexpr float DEFAULT_SPEED = 2.5f;
static constexpr float DEFAULT_SENSITIVITY = 0.07f;
static constexpr glm::vec3 DEFAULT_DIRECTION{0.0f, 0.0f, -1.0f};
static constexpr glm::vec3 DEFAULT_WORLD_UP{0.0f, 1.0f, 0.0f};
static constexpr glm::vec3 DEFAULT_POSITION{0.0f, 0.0f, 0.0f};

class Camera {
private:
    glm::vec3 position_;
    glm::quat orientation_;
    glm::vec3 worldUp_;

    // pitch and yaw stored as degrees.
    float pitch_;
    float yaw_ = 0.0f;

    float sensitivity = DEFAULT_SENSITIVITY;
    float speed = DEFAULT_SPEED;

public:
    Camera(glm::vec3 position)
        : Camera(position, DEFAULT_DIRECTION, DEFAULT_WORLD_UP)
    {}

    /**
     * Assumes that the direction and worldUp vectors are not parallel.
     */
    Camera(glm::vec3 direction, glm::vec3 worldUp)
        : Camera(DEFAULT_POSITION, direction, worldUp)
    {}

    /**
     * Assumes that the direction and worldUp vectors are not parallel.
     */
    Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 worldUp)
        : position_(position),
          worldUp_(glm::normalize(worldUp))
    {
        pitch_ = getPitch(direction);
        updateOrientation();
    }

    glm::mat4 getViewMatrix() const {
        glm::mat4 rotation = glm::mat4_cast(glm::conjugate(orientation_));
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), -position_);
        return rotation * translation;
    }

    void move(CameraMovement direction, float deltaTime) {
        float distance = speed * deltaTime;

        switch (direction) {
            case CameraMovement::Forward:
                position_ += getCameraForward() * distance;
                break;
            case CameraMovement::Backward:
                position_ -= getCameraForward() * distance;
                break;
            case CameraMovement::Left:
                position_ -= getCameraRight() * distance;
                break;
            case CameraMovement::Right:
                position_ += getCameraRight() * distance;
                break;
        }
    }

    void rotate(float deltaYaw, float deltaPitch, bool constrainPitch = true) {
        deltaYaw *= -sensitivity;
        deltaPitch *= sensitivity;

        yaw_ += deltaYaw;
        pitch_ += deltaPitch;

        if (constrainPitch) {
            pitch_ = glm::clamp(pitch_, -89.0f, 89.0f);
        }

        updateOrientation();
    }

    [[deprecated("marked for removal - need to add getProjectionMatrix()")]]
    float getZoom() const {
        return 45.0f;
    }

    glm::vec3 getPosition() const {
        return glm::vec3(position_);
    }

private:

    glm::vec3 getCameraForward() const {
        return glm::normalize(orientation_ * glm::vec3(0.0f, 0.0f, -1.0f));
    }

    glm::vec3 getCameraUp() const {
        return glm::normalize(orientation_ * glm::vec3(0.0f, 1.0f, 0.0f));
    }

    glm::vec3 getCameraRight() const {
        return glm::normalize(orientation_ * glm::vec3(1.0f, 0.0f, 0.0f));
    }

    float getPitch(glm::vec3 direction) const {
        return glm::degrees(glm::asin(glm::dot(direction, worldUp_)));
    }

    void updateOrientation() {
        // reset orientation to the yaw rotation
        orientation_ = glm::normalize(glm::angleAxis(glm::radians(yaw_), worldUp_));
        // then apply the pitch rotation
        orientation_ = glm::normalize(glm::angleAxis(glm::radians(pitch_), getCameraRight()) * orientation_);
    }

};
