#pragma once

#include "DataStructures/PackedArray.hpp"

#include <unordered_map>
#include <queue>
#include <cassert>

template <typename T>
class ComponentArray : public PackedArray<T, uint64_t>
{
public:
	ComponentArray(uint64_t size);

	bool update();

	void addComponent(uint64_t nodeId);
	void addComponent(uint64_t nodeId, T component);
	void removeComponent(uint64_t nodeId);
	T& getComponent(uint64_t nodeId);

	uint64_t getNodeId(uint64_t  componentId);

	mutable std::mutex mutex;

private:
	std::unordered_map<uint64_t, uint64_t> _nodeId2cidMap;
	std::unordered_map<uint64_t, uint64_t> _cid2nodeIdMap;
	std::deque<std::pair<uint64_t, T>> _componentsToAdd;
	std::queue<uint64_t> _componentsToRemove;
};

template <typename T>
bool ComponentArray<T>::update()
{
	std::scoped_lock lock(mutex);

	bool updated = !_componentsToAdd.empty() || !_componentsToRemove.empty();

	while (!_componentsToAdd.empty())
	{
		auto pair = std::move(_componentsToAdd.front());
		auto nodeId = pair.first;
		_componentsToAdd.pop_front();
		assert(_nodeId2cidMap.find(nodeId) == _nodeId2cidMap.end());

		auto componentId = this->add(std::move(pair.second));

		_nodeId2cidMap[nodeId] = componentId;
		_cid2nodeIdMap[componentId] = nodeId;
	}

	while (!_componentsToRemove.empty())
	{
		auto nodeId = _componentsToRemove.front();
		assert(_nodeId2cidMap.find(nodeId) != _nodeId2cidMap.end());
		auto componentId = _nodeId2cidMap[nodeId];

		auto component = this->remove(componentId);

		auto exLastNodeId = _cid2nodeIdMap[this->activeCount()];
		_nodeId2cidMap[exLastNodeId] = componentId;
		_cid2nodeIdMap[componentId] = exLastNodeId;
		_nodeId2cidMap.erase(nodeId);
		_cid2nodeIdMap.erase(this->activeCount());
	}

	return updated;
}

template <typename T>
ComponentArray<T>::ComponentArray(uint64_t size) : PackedArray<T, uint64_t>(size) {}

template <typename T>
void ComponentArray<T>::addComponent(uint64_t nodeId)
{
	addComponent(nodeId, T{});
}

template <typename T>
void ComponentArray<T>::addComponent(uint64_t nodeId, T component)
{
	std::scoped_lock lock(mutex);
	_componentsToAdd.push_back({ nodeId, component });
}

template <typename T>
void ComponentArray<T>::removeComponent(uint64_t nodeId)
{
	std::scoped_lock lock(mutex);
	_componentsToRemove.push(nodeId);
}

template <typename T>
T& ComponentArray<T>::getComponent(uint64_t nodeId)
{
	std::scoped_lock lock(mutex);

	if (auto it = _nodeId2cidMap.find(nodeId); it != _nodeId2cidMap.end())
	{
		return this->at(it->second);
	}

	for (auto pair : _componentsToAdd)
	{
		if (pair.first == nodeId)
		{
			return pair.second;
		}
	}

	throw std::out_of_range("Invalid Node2D id!");
}

template <typename T>
uint64_t ComponentArray<T>::getNodeId(uint64_t  componentId)
{
	std::scoped_lock lock(mutex);
	assert(_cid2nodeIdMap.find(componentId) != _cid2nodeIdMap.end());
	return _cid2nodeIdMap[componentId];
}