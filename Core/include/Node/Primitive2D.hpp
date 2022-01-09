#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

#include "Node/Node2D.hpp"

class Primitive2D : public Node2D
{
public:
	Primitive2D();
	Primitive2D(const sf::Color& color, const sf::Vector2f& position, const sf::Vector2f& dimensions);
	Primitive2D(const sf::Color& color, const sf::Vector2f& position, float radius, unsigned numberOfPoints = 24);
	~Primitive2D();

	const sf::Color& getColor() const;

private:
	sf::Color _color;
};