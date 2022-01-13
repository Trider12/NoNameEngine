#include "Node/Node2D.hpp"
#include "Core/Locator.hpp"
#include "Core/InputEvent.hpp"

#include <assert.h>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Transform.hpp>

namespace
{
	auto& systemManager = Locator::getInstance().getSystemManager();
	auto& nodeManager = Locator::getInstance().getNode2DManager();
}

Node2D::Node2D()
{
	nodeManager.registerNode(this);
	TransformComponent component;
	component.node2D = this;
	systemManager.addComponent<TransformComponent>(*this, component);
}

Node2D::Node2D(const sf::Vector2f& translation)
{
	nodeManager.registerNode(this);
	TransformComponent component;
	component.globalTransform.translate(translation);
	component.node2D = this;
	systemManager.addComponent<TransformComponent>(*this, component);
}

Node2D::~Node2D()
{
	systemManager.removeComponent<TransformComponent>(*this);
}

Node2D& Node2D::addChild(std::shared_ptr<Node2D> child)
{
	if (child->_parent != nullptr)
	{
		fprintf(stderr, "Node %llu already has a parent!", child->_id);
		return *this;
	}

	_children.push_back(child);
	child->_parent = this;

	return *this;
}

Node2D& Node2D::removeChild(std::shared_ptr<Node2D> child)
{
	if (child->_parent != this)
	{
		fprintf(stderr, "Node %llu has no child %llu!", _id, child->_id);
		return *this;
	}

	auto it = std::find(_children.begin(), _children.end(), child);

	assert(it != _children.end());

	child->_parent = nullptr;
	_children.erase(it);

	return *this;
}

Node2D& Node2D::addChildren(std::vector<std::shared_ptr<Node2D>>& children)
{
	_children.reserve(_children.size() + children.size());

	for (auto& child : children)
	{
		addChild(child);
	}

	return *this;
}

void Node2D::ready()
{
}

void Node2D::input(const InputEvent& event)
{
}

void Node2D::update(float delta)
{
}

void Node2D::transformDeferred(const sf::Transform& transform)
{
	std::queue<Node2D*> nodes;
	nodes.push(this);

	while (!nodes.empty())
	{
		auto node = nodes.front();
		nodes.pop();

		systemManager.getSystem<PhysicsSystem>().transformDeferred(*node, transform);

		for (auto& child : node->getChildren())
		{
			nodes.push(child.get());
		}
	}
}

void Node2D::translateDeferred(const sf::Vector2f& translation)
{
	auto transform = sf::Transform::Identity;
	transformDeferred(transform.translate(translation));
}

void Node2D::rotateDeferred(float rotationDegrees)
{
	auto transform = sf::Transform::Identity;
	transformDeferred(transform.rotate(rotationDegrees));
}