#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

#include "Node/Node2D.hpp"

class Sprite : public Node2D
{
public:
	Sprite();
	Sprite(const sf::Color& color, const sf::Vector2f& position, const sf::Vector2f& dimensions);
	~Sprite();

	const sf::Color& getColor() const;
	const sf::Vector2f& getDimensions() const;

private:
	sf::Color _color;
	sf::Vector2f _dimensions;
};