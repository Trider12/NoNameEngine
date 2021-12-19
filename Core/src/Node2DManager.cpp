#include "Node2DManager.hpp"

#include <deque>
#include <cassert>

#include "Node2D.hpp"

namespace
{
	template<auto func>
	static void bfs(std::shared_ptr<Node2D> root)
	{
		for (std::deque<std::shared_ptr<Node2D>> deque(root); !deque.empty();)
		{
			auto node = deque.front();
			deque.pop_front();

			func(node);

			const auto& children = node->getChildren();
			deque.insert(deque.end(), children.begin(), children.end());
		}
	}
}

std::shared_ptr<Node2D> Node2DManager::getRoot()
{
	return _root;
}

void Node2DManager::setRoot(std::shared_ptr<Node2D> root)
{
	if (_root)
	{
		fprintf(stderr, "Warning: replacing existing root %llu with %llu (old root discarded)!", _root->_id, root->_id);
	}

	_root = root;
}

Node2DManager::Node2DManager()
{
}

void Node2DManager::registerNode(Node2D* node)
{
	assert(node->_id == 0 && "Node already registered!");
	assert(_nextId != 0 && "Node overflow!");
	node->_id = _nextId++;
}