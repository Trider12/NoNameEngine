#include "Node2D.hpp"

#include <assert.h>

#include <SFML/Graphics/Transform.hpp>

#include "Locator.hpp"

namespace
{
	auto& systemManager = Locator::getInstance().getSystemManager();
	auto& node2DManager = Locator::getInstance().getNode2DManager();
}

Node2D::Node2D()
{
	node2DManager.registerNode(this);
	systemManager.addComponent<ComponentType::Transform2D>(_id);
}

Node2D::~Node2D()
{
}

Node2D& Node2D::addChild(Node2D child)
{
	if (child._parent != nullptr)
	{
		fprintf(stderr, "Node %llu already has a parent!", child._id);
		return *this;
	}

	_children.push_back(child);
	child._parent = this;

	return *this;
}

Node2D& Node2D::removeChild(Node2D& child)
{
	if (child._parent != this)
	{
		fprintf(stderr, "Node %llu has no child %llu!", _id, child._id);
		return *this;
	}

	auto it = std::find(_children.begin(), _children.end(), child);

	assert(it != _children.end());

	child._parent = nullptr;
	_children.erase(it);

	return *this;
}

void Node2D::ready()
{
}

void Node2D::input(const sf::Event& event)
{
}

void Node2D::update(float delta)
{
}

void Node2D::transformDeferred(const sf::Transform& transform)
{
	systemManager.getComponent<TransformComponent>(_id).deferredTransform *= transform;
}