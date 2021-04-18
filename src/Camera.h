#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext\matrix_float4x4.hpp>
#include "Window.h"

class Camera {
public:
	enum class Directions { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };
	Camera();
	~Camera() {}
	void update(const Window& win);
	const glm::vec3& getPosition() const { return eye; }
	const glm::vec3& getUDirection() const { return uDirection; }
	const glm::vec3& getVDirection() const { return vDirection; }
	const glm::vec3& getWDirection() const { return wDirection; }
	const glm::mat4& getView() const { return view; }
	const glm::mat4& getProjection() const { return projection; }

private:
	void moveCamera(Directions direction, float deltaTime);
	void rotateCamera(const glm::vec2& cursorOffset);
	glm::vec3 eye;
	glm::vec3 center;
	glm::vec3 lookUp;
	glm::vec3 wDirection;
	glm::vec3 uDirection;
	glm::vec3 vDirection;
	glm::mat4 view;
	glm::mat4 projection;
	float roll = 0.f;
	float yaw = 3 * glm::pi<float>() / 2;
	float pitch = -glm::pi<float>() / 4;
	const float mouseSensitivity{ 2e-3f };

};
