#pragma once

#include <vector>

namespace sf
{
	class Event;
	class Transform;
}

class Node2D
{
public:
	Node2D();
	~Node2D();

	uint64_t getId() const { return _id; }

	Node2D& addChild(Node2D child);
	Node2D& removeChild(Node2D& child);
	std::vector<Node2D>& getChildren() { return _children; }

	friend bool operator==(const Node2D& a, const Node2D& b) { return std::addressof(a) == std::addressof(b); }

protected:
	virtual void ready();
	virtual void input(const sf::Event& event);
	virtual void update(float delta);

	void transformDeferred(const sf::Transform& transform);

	Node2D* getParent() const { return _parent; }

private:
	std::vector<Node2D> _children;
	Node2D* _parent = nullptr;
	uint64_t _id = 0;

	friend class Node2DManager;
};