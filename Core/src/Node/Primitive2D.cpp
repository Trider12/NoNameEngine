#include "Node/Primitive2D.hpp"
#include "Core/Locator.hpp"

#include <numbers>

#include <SFML/System/Vector2.hpp>

namespace
{
	auto& systemManager = Locator::getInstance().getSystemManager();

	TrianglePrimitiveComponent createRectComponent(sf::Color color, const sf::Vector2f& dimensions);
	TrianglePrimitiveComponent createCircleComponent(sf::Color color, float radius, unsigned numberOfPoints);
}

Primitive2D::Primitive2D()
	: Node2D(), _color{ sf::Color::White }
{
	systemManager.addComponent<TrianglePrimitiveComponent>(*this, createRectComponent(_color, { 100.f, 100.f }));
}

Primitive2D::Primitive2D(const sf::Color& color, const sf::Vector2f& position, const sf::Vector2f& dimensions)
	: Node2D(position), _color{ color }
{
	systemManager.addComponent<TrianglePrimitiveComponent>(*this, createRectComponent(_color, dimensions));
}

Primitive2D::Primitive2D(const sf::Color& color, const sf::Vector2f& position, float radius, unsigned numberOfPoints)
	: Node2D(position), _color{ color }
{
	systemManager.addComponent<TrianglePrimitiveComponent>(*this, createCircleComponent(_color, radius, numberOfPoints));
}

Primitive2D::~Primitive2D()
{
	systemManager.removeComponent<TrianglePrimitiveComponent>(*this);
}

const sf::Color& Primitive2D::getColor() const
{
	return _color;
}

namespace
{
	TrianglePrimitiveComponent createRectComponent(sf::Color color, const sf::Vector2f& dimensions)
	{
		TrianglePrimitiveComponent component;
		component.color = color;

		auto halfDims = dimensions * 0.5f;

		sf::Vector2f points[4] =
		{
			{-halfDims.x, halfDims.y},
			-halfDims,
			{halfDims.x, -halfDims.y},
			halfDims
		};

		component.trianglePointsCount = 6;
		component.trianglePoints = new sf::Vector2f[6]
		{
			points[0],
			points[1],
			points[2],
			points[2],
			points[3],
			points[0]
		};

		return component;
	}

	TrianglePrimitiveComponent createCircleComponent(sf::Color color, float radius, unsigned numberOfPoints)
	{
		TrianglePrimitiveComponent component;
		component.color = color;

		const auto twoPi = std::numbers::pi_v<float> *2.f;
		const auto angleFraction = twoPi / numberOfPoints;

		std::vector<sf::Vector2f> circumferencePoints;
		circumferencePoints.reserve(numberOfPoints);

		for (auto angle = 0.f; angle < twoPi; angle += angleFraction)
		{
			circumferencePoints.push_back({ radius * std::sinf(angle), radius * std::cosf(angle) });
		}

		auto trianglePointsCount = (numberOfPoints - 2) * 3;

		component.trianglePoints = new sf::Vector2f[trianglePointsCount];
		component.trianglePointsCount = trianglePointsCount;

		for (auto i = 0u, middle = (numberOfPoints + 1) / 2, left = middle - 1, right = middle + 1;
			i < trianglePointsCount;
			middle = i % 2 ? left-- : right++)
		{
			component.trianglePoints[i++] = circumferencePoints[left];
			component.trianglePoints[i++] = circumferencePoints[middle];
			component.trianglePoints[i++] = circumferencePoints[right];
		}

		return component;
	}
}