#include "Node/Node2DManager.hpp"
#include "Node/Node2D.hpp"
#include "Core/Debug.hpp"

#include <queue>
#include <cassert>

namespace
{
	template<auto Func, typename... Args>
	static void bfs(std::shared_ptr<Node2D> root, Args... args)
	{
		for (std::queue<Node2D*> queue({ root.get() }); !queue.empty();)
		{
			auto node = queue.front();
			queue.pop();

			((*node).*Func)(args...);

			for (auto& child : node->getChildren())
			{
				queue.push(child.get());
			}
		}
	}
}

Node2DManager::Node2DManager()
{
}

void Node2DManager::registerNode(Node2D* node)
{
	assert(node->_id == 0 && "Node already registered!\n");
	assert(_nextId != 0 && "Node overflow!\n");
	node->_id = _nextId++;
}

std::shared_ptr<Node2D> Node2DManager::getRoot()
{
	return _root;
}

void Node2DManager::setRoot(std::shared_ptr<Node2D> root)
{
	if (_root)
	{
		Debug::Warning("replacing existing root %llu with %llu (old root discarded)!", _root->_id, root->_id);
	}

	_root = root;
}

void Node2DManager::ready()
{
	bfs<&Node2D::ready>(_root);
}

void Node2DManager::input(const InputEvent& event)
{
	bfs<&Node2D::input, const InputEvent&>(_root, event);
}

void Node2DManager::update(float delta)
{
	bfs<&Node2D::update, float>(_root, delta);
}