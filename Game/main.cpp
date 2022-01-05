#include <Core/CoreLoop.hpp>
#include <Core/InputEvent.hpp>
#include <Core/Keyboard.hpp>

#include <Node/Sprite.hpp>
#include <Node/KinematicBody2D.hpp>
#include <Node/StaticBody2D.hpp>

#include <Utility/Math.hpp>

class Player : public KinematicBody2D
{
public:
	Player() : KinematicBody2D()
	{
		_sprite.reset(new Sprite(sf::Color::Blue, {}, { 100.f, 100.f }));
		addChild(_sprite);
	}

	void input(const InputEvent& event) override
	{
		if (event.type == InputEvent::EventType::MouseButtonReleased)
		{
			printf("sdf\n");
		}
	}

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
private:
	std::shared_ptr<Sprite> _sprite;

	float _speed = 200;
};

class Obstacle : public StaticBody2D
{
public:
	Obstacle(const sf::Vector2f& position, const sf::Vector2f& dimensions) : StaticBody2D(position, dimensions)
	{
		_sprite.reset(new Sprite(sf::Color::White, position, dimensions));
		addChild(_sprite);
	}
private:
	std::shared_ptr<Sprite> _sprite;
};

int main()
{
	auto windowSize = sf::Vector2u(1280u, 720u);
	auto center = sf::Vector2f(windowSize / 2u);

	auto root = std::make_shared<Node2D>();

	using nodePtr = std::shared_ptr<Node2D>;

	std::vector<nodePtr> children =
	{
		nodePtr(new Obstacle({center.x * 0.5f, center.y}, {100, 100})),
		nodePtr(new Obstacle(center, {100, 100})),
		nodePtr(new Obstacle({center.x * 1.5f, center.y}, {100, 100})),
	};

	root->addChildren(children);
	root->addChild(nodePtr(new Player()));

	CORE_LOOP(windowSize, "TDS", root);

	return 0;
}