#include "Camera.h"
#include <iostream>

#define ROTATIONAL_MOVE deltaTime
#define SPACE_MOVE (deltaTime * 10)
#define FULL_CIRCLE (2 * glm::pi<float>())

Camera::Camera() : eye(glm::vec3(0.f, 7.f, 10.f)), center(glm::vec3(0.f, 0.f, 0.f)), lookUp(glm::vec3(0.f, 1.f, 0.f)), view(glm::lookAt(eye, center, lookUp))
{
	wDirection = glm::normalize(center - eye);
	uDirection = glm::cross(wDirection, lookUp);
	vDirection = glm::cross(uDirection, wDirection);
	if (wDirection.z < 0.0f) pitch = acos(glm::dot(glm::vec3(0.f, 0.f, -1.f), wDirection));
	else pitch = acos(glm::dot(glm::vec3(0.f, 0.f, 1.f), wDirection));
	if (wDirection.y < 0) pitch *= -1;


	if (wDirection.z < 0.0f) yaw = FULL_CIRCLE - acos(glm::dot(glm::vec3(1.f, 0.f, 0.f), wDirection));
	else yaw = acos(glm::dot(glm::vec3(1.f, 0.f, 0.f), wDirection));

	wDirection.x = 0;
	wDirection.z = sin(yaw) * cos(pitch);
	wDirection.y = sin(pitch);
	center = eye + wDirection;
	view = glm::lookAt(eye, center, lookUp);

	projection = glm::perspective(glm::radians(45.f), 1024.f / 768.f, 1.f, 100.f);

}

void Camera::update(const Window& win)
{
	if (win.isKeyPressed(GLFW_KEY_W)) moveCamera(Directions::FORWARD, win.getTime().deltaTime);
	if (win.isKeyPressed(GLFW_KEY_A)) moveCamera(Directions::LEFT, win.getTime().deltaTime);
	if (win.isKeyPressed(GLFW_KEY_S)) moveCamera(Directions::BACKWARD, win.getTime().deltaTime);
	if (win.isKeyPressed(GLFW_KEY_D)) moveCamera(Directions::RIGHT, win.getTime().deltaTime);
	if (win.isKeyPressed(GLFW_KEY_SPACE)) moveCamera(Directions::UP, win.getTime().deltaTime);
	if (win.isKeyPressed(GLFW_KEY_LEFT_SHIFT)) moveCamera(Directions::DOWN, win.getTime().deltaTime);

	if (win.isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) rotateCamera(win.getCursorOffset());
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	projection = glm::perspective(45.f, (float)viewport[2] / viewport[3], 0.1f, 100.f);
}

void Camera::rotateCamera(const glm::vec2& cursorOffset)
{
	float mouseOffsetX = 0.f;
	float mouseOffsetY = 0.f;
	mouseOffsetX = cursorOffset.x;
	mouseOffsetY = cursorOffset.y;

	yaw += mouseOffsetX * mouseSensitivity;
	if (yaw < 0.f)
		yaw += FULL_CIRCLE;
	else if (yaw > FULL_CIRCLE)
		yaw -= FULL_CIRCLE;

	if (pitch + (mouseOffsetY * mouseSensitivity) < glm::pi<float>() / 2 && pitch + (mouseOffsetY * mouseSensitivity) > -glm::pi<float>() / 2)
		pitch += mouseOffsetY * mouseSensitivity;

	wDirection.x = cos(yaw) * cos(pitch);
	wDirection.y = sin(pitch);
	wDirection.z = sin(yaw) * cos(pitch);
	wDirection = glm::normalize(wDirection);
	center = eye + wDirection;
	uDirection = glm::cross(wDirection, lookUp);
	vDirection = glm::cross(uDirection, wDirection);

	view = glm::lookAt(eye, center, lookUp);
}

void Camera::moveCamera(Directions direction, float deltaTime)
{
	switch (direction)
	{
	case Directions::FORWARD:
		eye += wDirection * SPACE_MOVE;
		center += wDirection * SPACE_MOVE;
		break;

	case Directions::BACKWARD:
		eye -= wDirection * SPACE_MOVE;
		center -= wDirection * SPACE_MOVE;
		break;

	case Directions::LEFT:
		eye -= uDirection * SPACE_MOVE;
		center -= uDirection * SPACE_MOVE;
		break;

	case Directions::RIGHT:
		eye += uDirection * SPACE_MOVE;
		center += uDirection * SPACE_MOVE;
		break;

	case Directions::UP:
		eye += lookUp * SPACE_MOVE;
		center += lookUp * SPACE_MOVE;
		break;

	case Directions::DOWN:
		eye -= lookUp * SPACE_MOVE;
		center -= lookUp * SPACE_MOVE;
		break;
	}

	view = glm::lookAt(eye, center, lookUp);
}
