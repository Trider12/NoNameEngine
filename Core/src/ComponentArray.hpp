#pragma once

#include <unordered_map>

#include "Node2D.hpp"
#include "Component.hpp"

template <typename T>
concept DerivedComponent = std::is_base_of_v<Component, T>;

template <DerivedComponent T>
class ComponentArray
{
protected:
	struct Iterator
	{
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = value_type*;
		using reference = value_type&;

		Iterator(pointer ptr) : _ptr(ptr) {}

		reference operator*() const { return *_ptr; }
		pointer operator->() { return _ptr; }

		Iterator& operator++() { ++_ptr; return *this; }
		Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }

		friend bool operator== (const Iterator& a, const Iterator& b) { return a._ptr == b._ptr; };
		friend bool operator!= (const Iterator& a, const Iterator& b) { return a._ptr != b._ptr; };
	private:
		pointer _ptr;
	};

	//static_assert(std::forward_iterator<Iterator>); // idk why it fails

	ComponentArray(uint64_t size)
	{
		_components.resize(size);
	}

	Iterator begin() { return Iterator(&_components[0]); }
	Iterator end() { return Iterator(&_components[_numActive]); }

	void addComponent(std::shared_ptr<Node2D> node, T component)
	{
		addComponent(node->getId(), component);
	}

	void addComponent(uint64_t nodeId, T component)
	{
		if (_numActive == _components.size())
		{
			_components.resize(int(_components.size() * 1.6f));
		}

		auto cid = _numActive++;
		_components[cid] = component;

		_nodeId2cidMap[nodeId] = cid;
		_cid2nodeIdMap[cid] = nodeId;
	}

	void removeComponent(std::shared_ptr<Node2D> node)
	{
		removeComponent(node->getId());
	}

	void removeComponent(uint64_t nodeId)
	{
		assert(_nodeId2cidMap.find(nodeId) != _nodeId2cidMap.end());

		_numActive--;

		auto cid = _nodeId2cidMap[nodeId];
		_components[cid] = _components[_numActive];

		auto lastNodeid = _cid2nodeIdMap[_numActive];
		_nodeId2cidMap[lastNodeid] = cid;
		_cid2nodeIdMap[cid] = lastNodeid;

		_nodeId2cidMap.erase(nodeId);
		_cid2nodeIdMap.erase(_numActive);
	}

	T& getComponent(std::shared_ptr<Node2D> node)
	{
		return getComponent(node->getId());
	}

	T& getComponent(uint64_t nodeId)
	{
		assert(_nodeId2cidMap.find(nodeId) != _nodeId2cidMap.end());

		return _components[_nodeId2cidMap[nodeId]];
	}

private:
	std::vector<T> _components;
	std::unordered_map<uint64_t, uint64_t> _nodeId2cidMap;
	std::unordered_map<uint64_t, uint64_t> _cid2nodeIdMap;
	uint64_t _numActive = 0;
};

class TransformComponentArray : public ComponentArray<TransformComponent>
{
protected:
	TransformComponentArray(uint64_t size) : ComponentArray<TransformComponent>(size) {}
private:
	friend class ComponentManager;
};

class Graphics2DComponentArray : public ComponentArray<Graphics2DComponent>
{
protected:
	Graphics2DComponentArray(uint64_t size) : ComponentArray<Graphics2DComponent>(size) {}
private:
	friend class ComponentManager;
};

class CollisionComponentArray : public ComponentArray<CollisionComponent>
{
protected:
	CollisionComponentArray(uint64_t size) : ComponentArray<CollisionComponent>(size) {}
private:
	friend class ComponentManager;
};