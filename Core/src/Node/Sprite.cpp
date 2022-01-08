#include "Node/Sprite.hpp"
#include "Core/Locator.hpp"

#include <SFML/System/Vector2.hpp>

namespace
{
	auto& systemManager = Locator::getInstance().getSystemManager();

	TrianglePrimitiveComponent initPrimitiveComponent(sf::Color color, const sf::Vector2f& dimensions)
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
}

Sprite::Sprite()
	: Node2D()
	, _dimensions{ sf::Vector2f(100.f, 100.f) }
	, _color{ sf::Color::White }
{
	systemManager.addComponent<TrianglePrimitiveComponent>(*this, initPrimitiveComponent(_color, _dimensions));
}

Sprite::Sprite(const sf::Color& color, const sf::Vector2f& position, const sf::Vector2f& dimensions)
	: Node2D(position)
	, _dimensions{ dimensions }
	, _color{ color }
{
	systemManager.addComponent<TrianglePrimitiveComponent>(*this, initPrimitiveComponent(_color, _dimensions));
}

Sprite::~Sprite()
{
	systemManager.removeComponent<TrianglePrimitiveComponent>(*this);
}

const sf::Color& Sprite::getColor() const
{
	return _color;
}

const sf::Vector2f& Sprite::getDimensions() const
{
	return _dimensions;
}