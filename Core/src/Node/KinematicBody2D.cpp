#include "Node/KinematicBody2D.hpp"
#include "Core/Locator.hpp"

namespace
{
	auto& systemManager = Locator::getInstance().getSystemManager();
}

KinematicBody2D::KinematicBody2D() : Node2D(), _dimensions{ sf::Vector2f(100.f, 100.f) }
{
	auto component = CollisionComponent(CollisionComponent::CollisionObjectType::Kinematic, CollisionComponent::CollisionShapeType::AABB);
	component.collisionShape.rect = sf::FloatRect({}, _dimensions);
	systemManager.addComponent<CollisionComponent>(*this, component);
}

KinematicBody2D::KinematicBody2D(const sf::Vector2f& position, const sf::Vector2f& dimensions) : Node2D(position), _dimensions{ dimensions }
{
	auto component = CollisionComponent(CollisionComponent::CollisionObjectType::Kinematic, CollisionComponent::CollisionShapeType::AABB);
	component.collisionShape.rect = sf::FloatRect({}, _dimensions);
	systemManager.addComponent<CollisionComponent>(*this, component);
}