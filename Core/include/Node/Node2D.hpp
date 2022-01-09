#pragma once

#include <vector>
#include <memory>

namespace sf
{
	class Transform;

	template <typename T>
	class Vector2;

	using Vector2f = Vector2<float>;
}

class InputEvent;

class Node2D
{
public:
	Node2D();
	~Node2D();

	uint64_t getId() const { return _id; }

	Node2D& addChild(std::shared_ptr<Node2D> child);
	Node2D& removeChild(std::shared_ptr<Node2D> child);
	Node2D& addChildren(std::vector<std::shared_ptr<Node2D>>& children);
	std::vector<std::shared_ptr<Node2D>>& getChildren() { return _children; }

	friend bool operator==(const Node2D& a, const Node2D& b) { return std::addressof(a) == std::addressof(b); }

protected:
	Node2D(const sf::Vector2f& translation);

	virtual void ready();
	virtual void input(const InputEvent& event);
	virtual void update(float delta);

	void translateDeferred(const sf::Vector2f& translation);
	void transformDeferred(const sf::Transform& transform);

	Node2D* getParent() const { return _parent; }

private:
	void translate(const sf::Vector2f& translation);

	std::vector<std::shared_ptr<Node2D>> _children;
	Node2D* _parent = nullptr;
	uint64_t _id = 0;

	friend class Node2DManager;
	friend class PhysicsSystem;
};