#include "Node/StaticBody2D.hpp"
#include "Core/Locator.hpp"

namespace
{
	auto& systemManager = Locator::getInstance().getSystemManager();
}

StaticBody2D::StaticBody2D() : Node2D()
{
	auto component = CollisionComponent(CollisionComponent::CollisionType::Static, CollisionComponent::CollisionShapeType::AABB);
	component.collisionShape.rect = sf::FloatRect({ -50.f,-50.f }, { 100.f, 100.f });
	systemManager.addComponent<CollisionComponent>(*this, component);
}

StaticBody2D::StaticBody2D(const sf::Vector2f& position, const sf::Vector2f& dimensions) : Node2D(position)
{
	auto component = CollisionComponent(CollisionComponent::CollisionType::Static, CollisionComponent::CollisionShapeType::AABB);
	component.collisionShape.rect = sf::FloatRect(-dimensions / 2.f, dimensions);
	systemManager.addComponent<CollisionComponent>(*this, component);
}

StaticBody2D::StaticBody2D(const sf::Vector2f& position, float radius) : Node2D(position)
{
	auto component = CollisionComponent(CollisionComponent::CollisionType::Static, CollisionComponent::CollisionShapeType::Circle);
	component.collisionShape.center = {};
	component.collisionShape.radius = radius;
	systemManager.addComponent<CollisionComponent>(*this, component);
}