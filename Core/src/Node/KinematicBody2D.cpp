#include "Node/KinematicBody2D.hpp"
#include "Core/Locator.hpp"

namespace
{
	auto& systemManager = Locator::getInstance().getSystemManager();
}

KinematicBody2D::KinematicBody2D() : Node2D()
{
	auto component = CollisionComponent(CollisionComponent::CollisionType::Kinematic, CollisionComponent::CollisionShapeType::AABB);
	component.collisionShape.rect = sf::FloatRect({ -50.f,-50.f }, { 100.f, 100.f });
	systemManager.addComponent<CollisionComponent>(*this, component);
}

KinematicBody2D::KinematicBody2D(const sf::Vector2f& position, const sf::Vector2f& dimensions) : Node2D(position)
{
	auto component = CollisionComponent(CollisionComponent::CollisionType::Kinematic, CollisionComponent::CollisionShapeType::AABB);
	component.collisionShape.rect = sf::FloatRect(-dimensions / 2.f, dimensions);
	systemManager.addComponent<CollisionComponent>(*this, component);
}

KinematicBody2D::KinematicBody2D(const sf::Vector2f& position, float radius) : Node2D(position)
{
	auto component = CollisionComponent(CollisionComponent::CollisionType::Kinematic, CollisionComponent::CollisionShapeType::Circle);
	component.collisionShape.center = {};
	component.collisionShape.radius = radius;
	systemManager.addComponent<CollisionComponent>(*this, component);
}