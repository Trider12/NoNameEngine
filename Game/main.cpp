#include <Core/CoreLoop.hpp>
#include <Core/InputEvent.hpp>
#include <Core/Keyboard.hpp>

#include <Node/Sprite.hpp>
#include <Node/KinematicBody2D.hpp>
#include <Node/StaticBody2D.hpp>

#include <Utility/Helpers.hpp>

class Player : public KinematicBody2D
{
public:
	Player(const sf::Vector2f& position, const sf::Vector2f& dimensions) : KinematicBody2D(position, dimensions)
	{
		_sprite.reset(new Sprite(sf::Color::Blue, position, dimensions));
		addChild(_sprite);
	}

	Player(const sf::Vector2f& position, float radius) : KinematicBody2D(position, radius)
	{
		_sprite.reset(new Sprite(sf::Color::Blue, position, { radius * 2.f, radius * 2.f }));
		addChild(_sprite);
	}

	void input(const InputEvent& event) override
	{
		if (event.type == InputEvent::EventType::MouseButtonReleased)
		{
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

		if (!VectorHelper::areEqualApprox(input, {}))
			translateDeferred(VectorHelper::normalized(input) * _speed * delta);
	}
private:
	std::shared_ptr<Sprite> _sprite;

	float _speed = 500;
};

class Obstacle : public StaticBody2D
{
public:
	Obstacle(const sf::Vector2f& position, const sf::Vector2f& dimensions) : StaticBody2D(position, dimensions)
	{
		_sprite = std::make_shared<Sprite>(sf::Color::White, position, dimensions);
		addChild(_sprite);
	}

	Obstacle(const sf::Vector2f& position, float radius) : StaticBody2D(position, radius)
	{
		_sprite.reset(new Sprite(sf::Color::White, position, { radius * 2.f, radius * 2.f }));
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

	std::vector<std::shared_ptr<Node2D>> children =
	{
		std::make_shared<Obstacle>(sf::Vector2f(center.x * 0.5f, center.y), sf::Vector2f(100.f, 100.f)),
		std::make_shared<Obstacle>(center, 50.f),
		std::make_shared<Obstacle>(sf::Vector2f(center.x * 1.5f, center.y), sf::Vector2f(100.f, 100.f))
	};

	root->addChildren(children);
	//root->addChild(std::make_shared<Player>(sf::Vector2f(), sf::Vector2f(100.f, 100.f)));
	root->addChild(std::make_shared<Player>(sf::Vector2f(), 50.f));

	CORE_LOOP(windowSize, "TDS", root);

	return 0;
}