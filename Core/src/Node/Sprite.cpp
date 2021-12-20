#include "Node/Sprite.hpp"
#include "Core/Locator.hpp"

#include <SFML/System/Vector2.hpp>

namespace
{
	auto& systemManager = Locator::getInstance().getSystemManager();
}

Sprite::Sprite() : Node2D(), _dimensions{ sf::Vector2f(100.f, 100.f) }
{
	systemManager.addComponent<ComponentType::ColorRect>(*this);
}

Sprite::Sprite(const sf::Color& color, const sf::Vector2f& position, const sf::Vector2f& dimensions) : Node2D(), _dimensions{ dimensions }, _color{ color }
{
	systemManager.addComponent<ComponentType::ColorRect>(*this);
	translate(position);
}

Sprite::~Sprite()
{
	systemManager.removeComponent<ComponentType::ColorRect>(*this);
}

const sf::Color& Sprite::getColor() const
{
	return _color;
}

const sf::Vector2f& Sprite::getDimensions() const
{
	return _dimensions;
}