#include "Cloth.h"
#include <glad/glad.h>
#include <iostream>

Cloth::Cloth(size_t horizontalCount, size_t verticalCount)
{
	if (horizontalCount % 2 != 0) ++horizontalCount;
	if (verticalCount % 2 != 0) ++verticalCount;
	indicesCount = (horizontalCount - 1) * (verticalCount - 1) * 6;
	verticesCount = horizontalCount * verticalCount;
	constructModel(horizontalCount, verticalCount);
}

Cloth::~Cloth()
{
	glDeleteBuffers(1, &ebo);
}

void Cloth::draw() const
{
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

void Cloth::updatePhysics(const Time& t, const Sphere& sphere)
{
	if (glm::abs(t.deltaTime - t.lastDeltaTime) > 0.01f) return;
	for (size_t i = 0; i < particles.size(); ++i)
	{
		///////////////////////////// Applying translations //////////////////////////////////////////////////////////////////////
		Cloth::Particle& p = particles[i];
		const glm::vec3 velocity = translations[i] - p.previousTranslation;
		if (!p.fixed)
		{
			p.tempTranslation = translations[i] + velocity + ((p.forces / particleMass) * ((t.deltaTime + t.lastDeltaTime) / 2.f) * t.deltaTime);

			// Cloth-sphere collision
			const float offset = glm::distance(sphere.getTranslation(), p.transformedPosition + p.tempTranslation) - sphere.getRadius();
			const float additionalOffset = glm::length(particles[0].transformedPosition - particles[1].transformedPosition) / 6.f;
			static bool written = false;
			if (!written) 
			{
				std::cout << additionalOffset << std::endl;
				written = true;
			}
			if (offset < additionalOffset)
			{
				const glm::vec3 direction = glm::normalize(p.transformedPosition + p.tempTranslation - sphere.getTranslation());
				p.tempTranslation += direction * (additionalOffset - offset);
			}

			if (glm::length(p.tempTranslation - translations[i]) < 2.f)
			{
				p.previousTranslation = translations[i];
				translations[i] = p.tempTranslation;
			}
		}

		p.tempTranslation = { 0.f, 0.f, 0.f };
		p.forces = { 0.f, 0.f, 0.f };

		///////////////////////////// New Calculations ////////////////////////////////////////////////////////////////
		const glm::vec3 newVelocity = translations[i] - p.previousTranslation;
		p.transformedPosition = glm::vec3(getTransformMatrix() * glm::vec4(p.initialPosition, 1.f));
		const glm::vec3 currentPosition = p.transformedPosition + translations[i];
		p.forces += particleMass * glm::vec3(0.f, -9.81f, 0.f);
		if (wind) p.forces += generateWindVector(currentPosition, t.runningTime) *  glm::vec3(3.f, 1.f, 3.f);
		p.forces += generateAirResistanceVector(5.f * t.frameRate * t.frameRate, newVelocity);
	}

	for (const auto& spring : springs)
	{
		Cloth::Particle& p1 = particles[spring.particle1];
		Cloth::Particle& p2 = particles[spring.particle2];
		const float initialSpringLen = glm::length(p2.transformedPosition - p1.transformedPosition);
		const glm::vec3 p1Position = p1.transformedPosition + translations[spring.particle1];
		const glm::vec3 p2Position = p2.transformedPosition + translations[spring.particle2];
		const float currentSpringLen = glm::length(p2Position - p1Position);
		const glm::vec3 springForce = springConstants[spring.type] * (currentSpringLen - initialSpringLen) * glm::normalize(p2Position - p1Position);
		p1.forces += springForce;
		p2.forces -= springForce;
	}
}

void Cloth::constructModel(size_t horizontalCount, size_t verticalCount) 
{
	particles.reserve(verticesCount);
	springs.reserve(8 * verticesCount - 5 * horizontalCount - 5 * verticalCount + 4);
	translations.reserve(verticesCount);
	std::vector<GLuint> indices;
	indices.reserve(indicesCount);
	std::vector<float> data;
	data.reserve(verticalCount * 8);
	float xDelta = 1.f / horizontalCount;
	float yDelta = 1.f / verticalCount;

	for (size_t i = 0; i < verticalCount; ++i)
	{
		for (size_t j = 0; j < horizontalCount; ++j)
		{
			// Position
			data.push_back(-0.5f + j * xDelta);
			data.push_back(0.5f - i * yDelta);
			data.push_back(0.f);

			// Normal
			data.push_back(0.f);
			data.push_back(0.f);
			data.push_back(1.f);

			// Texture coordinates
			data.push_back(0.f + j * xDelta);
			data.push_back(1.f - i * yDelta);

			// Cloth vertex
			particles.push_back(Cloth::Particle(glm::vec3(-0.5f + j * xDelta, 0.5f - i * yDelta, 0.f)));

			translations.push_back(glm::vec3(0.f, 0.f, 0.f));
			if (j > 0 && i > 0)
			{
				indices.push_back((i - 1) * horizontalCount + j - 1);
				indices.push_back(i * horizontalCount + j - 1);
				indices.push_back(i * horizontalCount + j);
				indices.push_back(i * horizontalCount + j);
				indices.push_back((i - 1) * horizontalCount + j);
				indices.push_back((i - 1) * horizontalCount + j - 1);

				// Cloth springs --> point connected to each adjacent point and second next vertically and horizontally
				springs.push_back(Cloth::Spring((i - 1) * horizontalCount + j - 1, i * horizontalCount + j - 1, Cloth::Structural));
				springs.push_back(Cloth::Spring((i - 1) * horizontalCount + j - 1, (i - 1) * horizontalCount + j, Cloth::Structural));
				springs.push_back(Cloth::Spring((i - 1) * horizontalCount + j - 1, i * horizontalCount + j, Cloth::Shear));
				springs.push_back(Cloth::Spring((i - 1) * horizontalCount + j, i * horizontalCount + j - 1, Cloth::Shear));
				if (i == verticalCount - 1)
					springs.push_back(Cloth::Spring(i * horizontalCount + j - 1, i * horizontalCount + j, Cloth::Structural));
				if (j == horizontalCount - 1)
					springs.push_back(Cloth::Spring((i - 1) * horizontalCount + j, i * horizontalCount + j, Cloth::Structural));

				if (j > 1 && i > 1)
				{
					springs.push_back(Cloth::Spring((i - 2) * horizontalCount + j - 2, (i - 2) * horizontalCount + j, Cloth::Bending));
					springs.push_back(Cloth::Spring((i - 2) * horizontalCount + j - 2, i * horizontalCount + j - 2, Cloth::Bending));
					if (i == verticalCount - 1 || i == verticalCount - 2)
						springs.push_back(Cloth::Spring(i * horizontalCount + j - 2, i * horizontalCount + j, Cloth::Bending));
					if (j == horizontalCount - 1 || j == horizontalCount - 2)
						springs.push_back(Cloth::Spring((i - 2) * horizontalCount + j, i * horizontalCount + j, Cloth::Bending));
				}
			}
		}
	}

	particles[0].fixed = true;
	particles[49].fixed = true;

	glGenBuffers(1, &ebo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)sizeof(glm::vec3));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)(2 * sizeof(glm::vec3)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

glm::vec3 Cloth::generateWindVector(const glm::vec3& factor, const float time) const
{
	return glm::vec3(glm::sin(time * factor.z), 
				glm::cos(time) * glm::sin(time), 
				glm::sin(time * factor.x));
}

glm::vec3 Cloth::generateAirResistanceVector(const float factor, const glm::vec3& velocity) const
{
	return -factor * velocity * glm::abs(velocity);
}
