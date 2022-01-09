#include <Core/CoreLoop.hpp>
#include <Core/InputEvent.hpp>
#include <Core/Keyboard.hpp>

#include <Node/Primitive2D.hpp>
#include <Node/KinematicBody2D.hpp>
#include <Node/StaticBody2D.hpp>

#include <Utility/Helpers.hpp>

class Player : public KinematicBody2D
{
public:
	Player(const sf::Vector2f& position, float radius) : KinematicBody2D(position, radius)
	{
		_sprite.reset(new Primitive2D(sf::Color::Blue, position, radius));
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
	std::shared_ptr<Primitive2D> _sprite;

	float _speed = 500;
};

class Bullet : public KinematicBody2D
{
public:
	Bullet(const sf::Vector2f& position, float radius) : KinematicBody2D(position, radius)
	{
		_sprite.reset(new Primitive2D(sf::Color::Red, position, radius));
		addChild(_sprite);
	}

	void update(float delta) override
	{
		translateDeferred(_direction * _speed * delta);
	}

	void onCollision(sf::Vector2f normal) override
	{
		_direction -= 2 * VectorHelper::dot(_direction, normal) * normal;
	}

private:
	std::shared_ptr<Primitive2D> _sprite;
	sf::Vector2f _direction = { 1.f, 1.f };
	float _speed = 500;
};

class Obstacle : public StaticBody2D
{
public:
	Obstacle(const sf::Vector2f& position, const sf::Vector2f& dimensions) : StaticBody2D(position, dimensions)
	{
		_sprite = std::make_shared<Primitive2D>(sf::Color::White, position, dimensions);
		addChild(_sprite);
	}

	Obstacle(const sf::Vector2f& position, float radius) : StaticBody2D(position, radius)
	{
		_sprite.reset(new Primitive2D(sf::Color::White, position, radius));
		addChild(_sprite);
	}

private:
	std::shared_ptr<Primitive2D> _sprite;
};

int main()
{
	auto windowSize = sf::Vector2u(1280u, 720u);
	auto center = sf::Vector2f(windowSize / 2u);

	auto root = std::make_shared<Node2D>();

	auto borderWidth = windowSize.y * 0.1f;
	auto offset = sf::Vector2f(windowSize.y * 0.25f, windowSize.y * 0.25f);

	std::vector<std::shared_ptr<Node2D>> obstacles =
	{
		std::make_shared<Obstacle>(sf::Vector2f(0.f, center.y), sf::Vector2f(borderWidth, windowSize.y - borderWidth)),
		std::make_shared<Obstacle>(sf::Vector2f(float(windowSize.x), center.y), sf::Vector2f(borderWidth, windowSize.y - borderWidth)),
		std::make_shared<Obstacle>(sf::Vector2f(center.x, 0.f), sf::Vector2f(windowSize.x - borderWidth, borderWidth)),
		std::make_shared<Obstacle>(sf::Vector2f(center.x, float(windowSize.y)), sf::Vector2f(windowSize.x - borderWidth, borderWidth)),
		std::make_shared<Obstacle>(offset, sf::Vector2f(100.f, 100.f)),
		std::make_shared<Obstacle>(sf::Vector2f(windowSize) - offset, sf::Vector2f(100.f, 100.f)),
		std::make_shared<Obstacle>(sf::Vector2f(windowSize.x - offset.x, offset.y), 50.f),
		std::make_shared<Obstacle>(sf::Vector2f(offset.x, windowSize.y - offset.y), 50.f),
	};

	root->addChildren(obstacles);
	root->addChild(std::make_shared<Bullet>(center, 10.f));

	CORE_LOOP(windowSize, "TDS", root);

	return 0;
}