#include <Core/CoreLoop.hpp>
#include <Core/InputEvent.hpp>
#include <Core/Keyboard.hpp>
#include <Node/Primitive2D.hpp>
#include <Node/KinematicBody2D.hpp>
#include <Node/StaticBody2D.hpp>
#include <Utility/Helpers.hpp>

#include <random>

class Bullet : public KinematicBody2D
{
public:
	Bullet(const sf::Vector2f& position, const sf::Vector2f& direction, float radius) : KinematicBody2D(position, radius), _direction{ direction }
	{
		addChild(std::make_shared<Primitive2D>(sf::Color::Red, position, radius));
	}

	void update(float delta) override
	{
		translateDeferred(_direction * _speed * delta);
	}

	void onCollision(sf::Vector2f normal) override
	{
		_direction -= 2.f * VectorHelper::dot(_direction, normal) * normal;
	}

private:
	sf::Vector2f _direction;
	float _speed = 500.f;
};

class Player : public Node2D
{
public:
	Player() : Node2D(), _rng{ _device() }, _distribution{ -1.f, 1.f } {}

	void input(const InputEvent& event) override
	{
		if (event.type == InputEvent::EventType::MouseButtonReleased)
		{
			auto bullet = std::make_shared<Bullet>(sf::Vector2f(float(event.mouseButton.x), float(event.mouseButton.y)), VectorHelper::normalized(sf::Vector2f(_distribution(_rng), _distribution(_rng))), 20.f);
			CoreLoop::getRoot()->addChild(bullet);
		}
	}
private:
	std::random_device _device;
	std::mt19937 _rng;
	std::uniform_real_distribution<float> _distribution;
};

class Obstacle : public StaticBody2D
{
public:
	Obstacle(const sf::Vector2f& position, const sf::Vector2f& dimensions, sf::Color color = sf::Color::White) : StaticBody2D(position, dimensions)
	{
		addChild(std::make_shared<Primitive2D>(color, position, dimensions));
	}

	Obstacle(const sf::Vector2f& position, float radius, sf::Color color = sf::Color::White) : StaticBody2D(position, radius)
	{
		addChild(std::make_shared<Primitive2D>(color, position, radius));
	}
};

int main()
{
	auto windowSize = sf::Vector2u(1280u, 720u);
	auto center = sf::Vector2f(windowSize / 2u);
	auto borderWidth = windowSize.y * 0.05f;
	auto offset = sf::Vector2f(windowSize.y * 0.25f, windowSize.y * 0.25f);

	std::vector<std::shared_ptr<Node2D>> obstacles =
	{
		std::make_shared<Obstacle>(sf::Vector2f(borderWidth, center.y), sf::Vector2f(borderWidth, windowSize.y - 3.f * borderWidth)),
		std::make_shared<Obstacle>(sf::Vector2f(float(windowSize.x) - borderWidth, center.y), sf::Vector2f(borderWidth, windowSize.y - 3.f * borderWidth)),
		std::make_shared<Obstacle>(sf::Vector2f(center.x, borderWidth), sf::Vector2f(windowSize.x - 3.f * borderWidth, borderWidth)),
		std::make_shared<Obstacle>(sf::Vector2f(center.x, float(windowSize.y) - borderWidth), sf::Vector2f(windowSize.x - 3.f * borderWidth, borderWidth)),
		std::make_shared<Obstacle>(offset, sf::Vector2f(100.f, 100.f), sf::Color::Green),
		std::make_shared<Obstacle>(sf::Vector2f(windowSize) - offset, sf::Vector2f(100.f, 100.f), sf::Color::Green),
		std::make_shared<Obstacle>(sf::Vector2f(windowSize.x - offset.x, offset.y), 50.f, sf::Color::Blue),
		std::make_shared<Obstacle>(sf::Vector2f(offset.x, windowSize.y - offset.y), 50.f, sf::Color::Blue),
	};

	auto root = std::make_shared<Node2D>();
	root->addChild(std::make_shared<Player>());
	root->addChildren(obstacles);

	CORE_LOOP(windowSize, "TDS", root);

	return 0;
}