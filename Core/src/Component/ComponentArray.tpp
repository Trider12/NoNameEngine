#pragma once

#include <cassert>

#include "Node/Node2D.hpp"

template <typename T>
ComponentArray<T>::ComponentArray(uint64_t size)
{
	_components.resize(size);
}

template <typename T>
T& ComponentArray<T>::operator[](uint64_t i)
{
	return _components[i];
}

template <typename T>
ComponentArray<T>
::Iterator ComponentArray<T>::begin()
{
	return Iterator(&_components[0]);
}

template <typename T>
ComponentArray<T>
::Iterator ComponentArray<T>::end()
{
	return Iterator(&_components[_activeCount]);
}

template <typename T>
uint64_t ComponentArray<T>::activeCount() const
{
	return _activeCount;
}

template <typename T>
void ComponentArray<T>::addComponent(const Node2D& node)
{
	addComponent(node.getId());
}

template <typename T>
void ComponentArray<T>::addComponent(uint64_t nodeId)
{
	addComponent(nodeId, T{});
}

template <typename T>
void ComponentArray<T>::addComponent(const Node2D& node, T component)
{
	addComponent(node.getId(), component);
}

template <typename T>
void ComponentArray<T>::addComponent(uint64_t nodeId, T component)
{
	if (_activeCount == _components.size())
	{
		_components.push_back(component);
	}

	auto cid = _activeCount++;
	_components[cid] = component;

	_nodeId2cidMap[nodeId] = cid;
	_cid2nodeIdMap[cid] = nodeId;
}

template <typename T>
void ComponentArray<T>::removeComponent(const Node2D& node)
{
	removeComponent(node.getId());
}

template <typename T>
void ComponentArray<T>::removeComponent(uint64_t nodeId)
{
	assert(_nodeId2cidMap.find(nodeId) != _nodeId2cidMap.end());

	_activeCount--;

	auto cid = _nodeId2cidMap[nodeId];
	_components[cid] = _components[_activeCount];

	auto lastNodeid = _cid2nodeIdMap[_activeCount];
	_nodeId2cidMap[lastNodeid] = cid;
	_cid2nodeIdMap[cid] = lastNodeid;

	_nodeId2cidMap.erase(nodeId);
	_cid2nodeIdMap.erase(_activeCount);
}

template <typename T>
T& ComponentArray<T>::getComponent(const Node2D& node)
{
	return getComponent(node.getId());
}

template <typename T>
T& ComponentArray<T>::getComponent(uint64_t nodeId)
{
	assert(_nodeId2cidMap.find(nodeId) != _nodeId2cidMap.end());

	return _components[_nodeId2cidMap[nodeId]];
}

template <typename T>
uint64_t ComponentArray<T>::getNodeId(uint64_t index)
{
	return _cid2nodeIdMap[index];
}