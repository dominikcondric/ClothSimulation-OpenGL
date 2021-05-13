#include "Sphere.h"
#include "Cloth.h"
#include "Window.h"
#include <memory>
#include "Camera.h"
#include "Texture.h"
#include "Cube.h"
#include <glm/gtc/matrix_access.hpp>
#include <iostream>

int main()
{
	std::unique_ptr<Window> window(new Window(1024, 768, "Cloth Simulation", 4));
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glClearColor(0.1f, 0.1f, 0.1f, 1.f);

	PerspectiveCamera cam;
	
	// Collision sphere
	std::unique_ptr<Sphere> sphere(new Sphere(50, 30, 0.5f));
	sphere->color = glm::vec3(0.2f, 3.f, 4.f);
	sphere->translate(glm::vec3(0.f, -4.f, 0.f));
	sphere->scale(glm::vec3(4.f));

	// Light source model
	std::unique_ptr<Sphere> lightSphere(new Sphere(10, 5, 1.f));
	lightSphere->scale(glm::vec3(0.1f));
	lightSphere->translate(glm::vec3(1.f, 2.f, 1.f));
	lightSphere->color = glm::vec3(1.f);

	// Cloth
	std::unique_ptr<Cloth> cloth(new Cloth(50, 30));
	cloth->scale(glm::vec3(10.f, 10.f, 1.f));
	cloth->rotate(-90.f, glm::vec3(1.f, 0.f, 0.f));
	cloth->color = glm::vec3(1.0f, 1.f, 0.7f);
	Texture clothTexture("fabric.jpg", GL_TEXTURE_2D, true);
	
	// Skybox
	std::unique_ptr<Cube> skybox(new Cube(glm::vec3(-1.f), glm::vec3(1.f)));
	Texture skyboxCubemap("cubemap.jpg", GL_TEXTURE_CUBE_MAP, true);

	// Shaders setup
	Shader basicShader("BasicVertexShader.glsl", "BasicFragmentShader.glsl");
	Shader lightingShader("LightVertexShader.glsl", "LightFragmentShader.glsl");
	Shader skyboxShader("SkyboxVertexShader.glsl", "SkyboxFragmentShader.glsl");

	// Scene light setup
	lightingShader.use();
	lightingShader.setVec3("light.position", glm::vec3(glm::column(lightSphere->getTransformMatrix(), 3)));
	lightingShader.setVec3("light.color", 1.f * lightSphere->color);
	lightingShader.setFloat("light.constant", 1.f);
	lightingShader.setFloat("light.linear", 0.024f);
	lightingShader.setFloat("light.quadratic", 0.0021f);

	glm::vec3 sphereTranslation = sphere->getTranslation();

	glViewport(0, 0, window->getWindowSize().x, window->getWindowSize().y);
	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skyboxShader.use();
		skyboxShader.setMat4("projection", cam.getProjection());
		skyboxShader.setMat4("view", cam.getView());
		skyboxCubemap.activateAndBind(GL_TEXTURE0);
		skybox->draw();

		glEnable(GL_CULL_FACE);
		basicShader.use();
		basicShader.setMat4("projection", cam.getProjection());
		basicShader.setMat4("view", cam.getView());
		basicShader.setMat4("model", lightSphere->getTransformMatrix());
		basicShader.setVec3("objectColor", lightSphere->color);
		lightSphere->draw();

		lightingShader.use();
		lightingShader.setBool("cloth", false);
		lightingShader.setBool("tex", false);
		lightingShader.setVec3("light.position", glm::vec3(glm::column(lightSphere->getTransformMatrix(), 3)));
		lightingShader.setMat4("projection", cam.getProjection());
		lightingShader.setMat4("view", cam.getView());
		lightingShader.setVec3("eyePosition", cam.getPosition());
		lightingShader.setMat4("model", sphere->getTransformMatrix());
		sphere->updateColorsBasedOnMaterial(lightingShader, Entity::Material::PLASTIC);
		sphere->draw();

		glDisable(GL_CULL_FACE);
		lightingShader.setBool("cloth", true);
		lightingShader.setBool("tex", true);
		clothTexture.activateAndBind(GL_TEXTURE0);
		cloth->updatePhysics(window->getTime(), *sphere);
		lightingShader.setMat4("model", cloth->getTransformMatrix());
		lightingShader.setVec3Array("vertexTranslation", cloth->getTranslations().size(), cloth->getTranslations().data());
		cloth->updateColorsBasedOnMaterial(lightingShader, Entity::Material::FABRIC);
		cloth->draw();

		// Input controls
		const glm::vec3 forwardDirection = glm::cross(glm::vec3(0.f, 1.f, 0.f), cam.getUDirection());
		if (window->isKeyPressed(GLFW_KEY_LEFT)) sphereTranslation -= cam.getUDirection() * window->getTime().deltaTime * 10.f;
		if (window->isKeyPressed(GLFW_KEY_UP)) sphereTranslation += forwardDirection * window->getTime().deltaTime * 10.f;
		if (window->isKeyPressed(GLFW_KEY_RIGHT)) sphereTranslation += cam.getUDirection() * window->getTime().deltaTime * 10.f;
		if (window->isKeyPressed(GLFW_KEY_DOWN)) sphereTranslation -= forwardDirection * window->getTime().deltaTime * 10.f;
		if (window->isKeyPressed(GLFW_KEY_RIGHT_SHIFT)) sphereTranslation.y += window->getTime().deltaTime * 10.f;
		if (window->isKeyPressed(GLFW_KEY_RIGHT_CONTROL)) sphereTranslation.y -= window->getTime().deltaTime * 10.f;
		sphere->translate(sphereTranslation);
		
		if (window->isKeyPressed(GLFW_KEY_W))
			cam.moveCamera(Camera::Directions::FORWARD, window->getTime().deltaTime);
		if (window->isKeyPressed(GLFW_KEY_S))
			cam.moveCamera(Camera::Directions::BACKWARD, window->getTime().deltaTime);
		if (window->isKeyPressed(GLFW_KEY_A))
			cam.moveCamera(Camera::Directions::LEFT, window->getTime().deltaTime);
		if (window->isKeyPressed(GLFW_KEY_D))
			cam.moveCamera(Camera::Directions::RIGHT, window->getTime().deltaTime);
		if (window->isKeyPressed(GLFW_KEY_SPACE))
			cam.moveCamera(Camera::Directions::UP, window->getTime().deltaTime);
		if (window->isKeyPressed(GLFW_KEY_LEFT_SHIFT))
			cam.moveCamera(Camera::Directions::DOWN, window->getTime().deltaTime);

		if (window->isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
			cam.rotateCamera(window->getCursorOffset());

		window->onUpdate();
	} while (!window->shouldClose());
}
