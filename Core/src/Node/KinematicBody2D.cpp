#include "Node/KinematicBody2D.hpp"
#include "Core/Locator.hpp"

namespace
{
	auto& systemManager = Locator::getInstance().getSystemManager();
}

KinematicBody2D::KinematicBody2D() : PhysicsBody2D()
{
	auto component = CollisionComponent(CollisionComponent::CollisionType::Kinematic, PhysicsBody2D::CollisionShapeType::AABB);
	component.collisionShape.rect = sf::FloatRect({ -50.f,-50.f }, { 100.f, 100.f });
	component.physicsBody = this;
	systemManager.addComponent<CollisionComponent>(*this, component);
}

KinematicBody2D::KinematicBody2D(const sf::Vector2f& position, const sf::Vector2f& dimensions, PhysicsBody2D::CollisionShapeType collisionShapeType) : PhysicsBody2D(position)
{
	if (collisionShapeType != PhysicsBody2D::CollisionShapeType::AABB && collisionShapeType != PhysicsBody2D::CollisionShapeType::Rectangle)
	{
		throw std::exception("Wrong PhysicsBody2D::CollisionShapeType provided to KinematicBody2D!"); // might be not such a good idea
	}

	auto component = CollisionComponent(CollisionComponent::CollisionType::Kinematic, collisionShapeType);
	component.collisionShape.rect = sf::FloatRect(-dimensions / 2.f, dimensions);
	component.physicsBody = this;
	systemManager.addComponent<CollisionComponent>(*this, component);
}

KinematicBody2D::KinematicBody2D(const sf::Vector2f& position, float radius) : PhysicsBody2D(position)
{
	auto component = CollisionComponent(CollisionComponent::CollisionType::Kinematic, PhysicsBody2D::CollisionShapeType::Circle);
	component.collisionShape.center = {};
	component.collisionShape.radius = radius;
	component.physicsBody = this;
	systemManager.addComponent<CollisionComponent>(*this, component);
}