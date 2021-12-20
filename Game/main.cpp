#include <Core/CoreLoop.hpp>
#include <Node/Sprite.hpp>

int main()
{
	auto windowSize = sf::Vector2u(1280u, 720u);
	auto center = sf::Vector2f(windowSize / 2u);

	auto root = std::make_shared<Node2D>();

	using nodePtr = std::shared_ptr<Node2D>;

	std::vector<nodePtr> children =
	{
		nodePtr(new Sprite(sf::Color::Red, {center.x * 0.5f, center.y}, {100, 100})),
		nodePtr(new Sprite(sf::Color::Green, center, {100, 100})),
		nodePtr(new Sprite(sf::Color::Blue, {center.x * 1.5f, center.y}, {100, 100})),
	};

	root->addChildren(children);

	CORE_LOOP(windowSize, "TDS", root);

	return 0;
}