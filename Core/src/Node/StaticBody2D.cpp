#include "Node/StaticBody2D.hpp"
#include "Core/Locator.hpp"

namespace
{
	auto& systemManager = Locator::getInstance().getSystemManager();
}

StaticBody2D::StaticBody2D() : PhysicsBody2D()
{
	auto component = CollisionComponent(CollisionComponent::CollisionType::Static, PhysicsBody2D::CollisionShapeType::AABB);
	component.collisionShape.rect = sf::FloatRect({ -50.f,-50.f }, { 100.f, 100.f });
	systemManager.addComponent<CollisionComponent>(*this, component);
}

StaticBody2D::StaticBody2D(const sf::Vector2f& position, const sf::Vector2f& dimensions, PhysicsBody2D::CollisionShapeType collisionShapeType) : PhysicsBody2D(position)
{
	if (collisionShapeType != PhysicsBody2D::CollisionShapeType::AABB && collisionShapeType != PhysicsBody2D::CollisionShapeType::Rectangle)
	{
		throw std::exception("Wrong PhysicsBody2D::CollisionShapeType provided to StaticBody2D!"); // might be not such a good idea
	}

	auto component = CollisionComponent(CollisionComponent::CollisionType::Static, collisionShapeType);
	component.collisionShape.rect = sf::FloatRect(-dimensions / 2.f, dimensions);
	systemManager.addComponent<CollisionComponent>(*this, component);
}

StaticBody2D::StaticBody2D(const sf::Vector2f& position, float radius) : PhysicsBody2D(position)
{
	auto component = CollisionComponent(CollisionComponent::CollisionType::Static, PhysicsBody2D::CollisionShapeType::Circle);
	component.collisionShape.center = {};
	component.collisionShape.radius = radius;
	systemManager.addComponent<CollisionComponent>(*this, component);
}