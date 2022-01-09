#pragma once

#include <SFML/Window/Keyboard.hpp>

// TODO: better proxy

class Keyboard : public sf::Keyboard
{
public:
	static bool isKeyPressed(Keyboard::Key key);
};