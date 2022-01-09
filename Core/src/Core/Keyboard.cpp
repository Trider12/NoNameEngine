#include "Core/Keyboard.hpp"

#pragma warning(disable : 26812) // FFS Microsoft
bool Keyboard::isKeyPressed(Keyboard::Key key)
{
	return sf::Keyboard::isKeyPressed(key);
}