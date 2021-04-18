#pragma once
#include "Entity.h"
#include "Sphere.h"
#include "Window.h"
#include <vector>

class Cloth : public Entity {
public:
	Cloth(size_t horizontalCount, size_t verticalCount);
	~Cloth() override;
	void draw() const override;
	void updatePhysics(const Time& t, const Sphere& s);
	const std::vector<glm::vec3>& getTranslations() const { return translations; }
	enum SpringConstantType { Structural, Shear, Bending };

private:
	struct Particle {
		Particle(const glm::vec3& initPosition) : initialPosition(initPosition) {}
		const glm::vec3 initialPosition;
		glm::vec3 tempTranslation{ 0.f, 0.f, 0.f };
		glm::vec3 previousTranslation{ 0.f, 0.f, 0.f };
		glm::vec3 forces{ 0.f, 0.f, 0.f };
		glm::vec3 transformedPosition{ 0.f, 0.f, 0.f };
		bool fixed = false;
	};

	struct Spring {
		Spring(size_t p1, size_t p2, SpringConstantType _type) : particle1(p1), particle2(p2), type(_type) {}
		size_t particle1, particle2;
		SpringConstantType type;
	};

	void constructModel(size_t horizontalCount, size_t verticalCount);
	glm::vec3 generateWindVector(const glm::vec3& factor, const float time) const;
	glm::vec3 generateAirResistanceVector(const float factor, const glm::vec3& velocity) const;
	std::vector<Spring> springs;
	std::vector<Particle> particles;
	std::vector<glm::vec3> translations;
	float particleMass = 1.f;
	bool wind = true;
	float springConstants[3] = { 6000.f, 2000.f, 100.f };
	GLuint ebo = 0;
	size_t indicesCount = 0;
};