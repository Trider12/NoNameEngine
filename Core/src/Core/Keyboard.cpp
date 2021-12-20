#include "Core/Keyboard.hpp"

#include <SFML/Window/Keyboard.hpp>

bool Keyboard::isKeyPressed(Key key)
{
	return sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(key));
}