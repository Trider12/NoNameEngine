#include "Node/Sprite.hpp"
#include "Core/Locator.hpp"

#include <SFML/System/Vector2.hpp>

namespace
{
	auto& systemManager = Locator::getInstance().getSystemManager();
}

Sprite::Sprite()
	: Node2D()
	, _dimensions{ sf::Vector2f(100.f, 100.f) }
	, _color{ sf::Color::White }
{
	systemManager.addComponent<QuadVerticesComponent>(*this);
}

Sprite::Sprite(const sf::Color& color, const sf::Vector2f& position, const sf::Vector2f& dimensions)
	: Node2D(position)
	, _dimensions{ dimensions }
	, _color{ color }
{
	systemManager.addComponent<QuadVerticesComponent>(*this);
}

Sprite::~Sprite()
{
	systemManager.removeComponent<QuadVerticesComponent>(*this);
}

const sf::Color& Sprite::getColor() const
{
	return _color;
}

const sf::Vector2f& Sprite::getDimensions() const
{
	return _dimensions;
}