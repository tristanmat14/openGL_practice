#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class MovementDirection {
    Forward,
    Backward,
    Left,
    Right,
    Up, 
    Down
};

/**
 * Defines a normalized camera movement direction.
 * Never adding a movement is undefined behaviour
 */
class CameraMovement {
private:
    // direction vector defined as (right, up, forward)
    // defines movement w.r.t. cameras local vectors (as opposed to the world space vectors)
    glm::vec3 direction_{0.0f, 0.0f, 0.0f};

public:
    void addMovement(MovementDirection direction) {
        switch (direction) {
            case MovementDirection::Forward:
                direction_.z += 1.0f;
                break;
            case MovementDirection::Backward:
                direction_.z -= 1.0f;
                break;
            case MovementDirection::Right:
                direction_.x += 1.0f;
                break;
            case MovementDirection::Left:
                direction_.x -= 1.0f;
                break;
            case MovementDirection::Up:
                direction_.y += 1.0f;
                break;
            case MovementDirection::Down:
                direction_.y -= 1.0f;
                break;
        }
    }

    glm::vec3 getMovement() const {
        if (direction_ == glm::vec3(0.0f, 0.0f, 0.0f)) {
            return direction_;
        }
        return glm::normalize(direction_);
    }

    float getForwardMovement() const {
        return getMovement().z;
    }    

    float getRightMovement() const {
        return getMovement().x;
    }

    float getUpMovement() const {
        return getMovement().y;
    }
};

class Camera {
private:
    glm::vec3 position_{0.0f, 0.0f, 0.0f};
    glm::vec3 worldUp_{0.0f, 1.0f, 0.0f};
    glm::vec3 worldForward_{0.0f, 0.0f, -1.0f};
    glm::quat orientation_;

    // defined w.r.t. Cameras local right vector and worldUp vector.
    // stored in degrees, clamped in range [-89.0f, 89.0f].
    float pitch_ = 0.0f;
    // defined w.r.t. Cameras local up vector and worldForward vector.
    // stored in degrees, modulo in range [0.0f, 360.0f).
    float yaw_ = 0.0f;

    float sensitivity_ = 0.07f;
    float speed_ = 2.5f;
    float zoom_ = 45.0f;

public:
    Camera() {
        updateOrientation();
    }

    explicit Camera(glm::vec3 position) 
        : position_(position)
    {
        updateOrientation();
    }

    /**
     * @precondition: the direction and worldUp vectors are not parallel.
     */
    Camera(glm::vec3 position, glm::vec3 direction)
        : position_(position)
    {
        updateOrientation(glm::normalize(direction));
    }

    /**
     * @precondition: the direction and worldUp vectors are not parallel.
     * @precondition: worldForward and worldUp are orthonormal.
     */
    Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 worldUp, glm::vec3 worldForward)
        : position_(position),
          worldUp_(glm::normalize(worldUp)),
          worldForward_(glm::normalize(worldForward))
    {
        updateOrientation(glm::normalize(direction));
    }

    glm::mat4 getViewMatrix() const {
        glm::mat4 rotation = glm::mat4_cast(glm::conjugate(orientation_));
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), -position_);
        return rotation * translation;
    }

    glm::mat4 getProjectionMatrix(float aspectRatio) const {
        return glm::perspective(glm::radians(zoom_), aspectRatio, 0.1f, 100.0f);
    }

    glm::mat4 getViewProjectionMatrix(float aspectRatio) const {
        return getProjectionMatrix(aspectRatio) * getViewMatrix();
    }
    
    glm::vec3 getPosition() const {
        return glm::vec3(position_);
    }

    glm::vec3 getDirection() const {
        return getCameraForward();
    }

    void move(const CameraMovement& movement, float deltaTime) {
        float distance = speed_ * deltaTime;

        position_ += getCameraForward() * movement.getForwardMovement() * distance;
        position_ += getCameraRight() * movement.getRightMovement() * distance;
        position_ += getCameraUp() * movement.getUpMovement() * distance;
    }

    void rotate(float deltaYaw, float deltaPitch) {
        deltaYaw *= sensitivity_;
        deltaPitch *= sensitivity_;

        yaw_ = glm::mod(yaw_ - deltaYaw, 360.0f);
        
        pitch_ += deltaPitch;
        pitch_ = glm::clamp(pitch_, -89.0f, 89.0f);

        updateOrientation();
    }

private:

    glm::vec3 getCameraForward() const {
        return glm::normalize(orientation_ * worldForward_);
    }

    glm::vec3 getCameraUp() const {
        return glm::normalize(orientation_ * worldUp_);
    }

    glm::vec3 getCameraRight() const {
        // need to use -worldForward since we define it as -ve conventionally
        glm::vec3 worldRight = glm::normalize(glm::cross(worldUp_, -worldForward_));
        return glm::normalize(orientation_ * worldRight);
    }

    /**
     * @precondition: direction is normalized
     */
    void setPitch(glm::vec3 direction) {
        float rawPitch = glm::degrees(glm::asin(glm::dot(direction, worldUp_)));
        pitch_ = glm::clamp(rawPitch, -89.0f, 89.0f);
    }

    /**
     * @precondition: direction is normalized
     */
    void setYaw(glm::vec3 direction) {
        // projection onto plane spanned by worldFoward and worldRight
        glm::vec3 projection = direction - glm::dot(direction, worldUp_);
        glm::vec3 worldRight = glm::normalize(glm::cross(worldUp_, -worldForward_));
        float rawYaw = glm::degrees(glm::atan(
            glm::dot(projection, -worldRight),
            glm::dot(projection, worldForward_)
        ));
        yaw_ = glm::mod(rawYaw, 360.0f);
    }

    void updateOrientation() {
        // reset orientation to the yaw rotation
        orientation_ = glm::normalize(glm::angleAxis(glm::radians(yaw_), worldUp_));
        // then apply the pitch rotation
        orientation_ = glm::normalize(glm::angleAxis(glm::radians(pitch_), getCameraRight()) * orientation_);
    }

    /**
     * @precondition: direction is normalized:w
     */
    void updateOrientation(glm::vec3 direction) {
        setYaw(direction);
        setPitch(direction);
        updateOrientation();
    }

};
