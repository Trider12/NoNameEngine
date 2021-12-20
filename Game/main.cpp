#include <Core/CoreLoop.hpp>
#include <Core/Keyboard.hpp>
#include <Node/Sprite.hpp>
#include <Utility/Math.hpp>

class Player : public Sprite
{
public:
	Player() : Sprite() {}

	void update(float delta) override
	{
		sf::Vector2f input;

		if (Keyboard::isKeyPressed(Keyboard::Key::A))
		{
			input.x += -1.f;
		}
		if (Keyboard::isKeyPressed(Keyboard::Key::D))
		{
			input.x += 1.f;
		}
		if (Keyboard::isKeyPressed(Keyboard::Key::W))
		{
			input.y += -1.f;
		}
		if (Keyboard::isKeyPressed(Keyboard::Key::S))
		{
			input.y += 1.f;
		}

		if (!areEqualApprox(input, sf::Vector2f(0.f, 0.f)))
			translateDeferred(normalized(input) * _speed * delta);
	}
public:
	float _speed = 200;
};

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
	root->addChild(nodePtr(new Player()));

	CORE_LOOP(windowSize, "TDS", root);

	return 0;
}