#pragma once

#include "DataStructures/PackedArray.hpp"

#include <unordered_map>
#include <cassert>

template <typename T>
class ComponentArray : public PackedArray<T>
{
public:
	ComponentArray(uint64_t size);

	void addComponent(uint64_t nodeId);
	void addComponent(uint64_t nodeId, T component);
	void removeComponent(uint64_t nodeId);
	T& getComponent(uint64_t nodeId);

	uint64_t getNodeId(uint64_t index);

private:
	std::unordered_map<uint64_t, uint64_t> _nodeId2cidMap;
	std::unordered_map<uint64_t, uint64_t> _cid2nodeIdMap;
};

template <typename T>
ComponentArray<T>::ComponentArray(uint64_t size) : PackedArray<T>(size) {}

template <typename T>
void ComponentArray<T>::addComponent(uint64_t nodeId)
{
	addComponent(nodeId, T{});
}

template <typename T>
void ComponentArray<T>::addComponent(uint64_t nodeId, T component)
{
	assert(_nodeId2cidMap.find(nodeId) == _nodeId2cidMap.end());

	auto componentId = this->add(component);
	_nodeId2cidMap[nodeId] = componentId;
	_cid2nodeIdMap[componentId] = nodeId;
}

template <typename T>
void ComponentArray<T>::removeComponent(uint64_t nodeId)
{
	assert(_nodeId2cidMap.find(nodeId) != _nodeId2cidMap.end());

	auto componentId = _nodeId2cidMap[nodeId];

	this->remove(componentId);

	auto exLastNodeId = _cid2nodeIdMap[this->_activeCount];
	_nodeId2cidMap[exLastNodeId] = componentId;
	_cid2nodeIdMap[componentId] = exLastNodeId;

	_nodeId2cidMap.erase(nodeId);
	_cid2nodeIdMap.erase(this->_activeCount);
}

template <typename T>
T& ComponentArray<T>::getComponent(uint64_t nodeId)
{
	assert(_nodeId2cidMap.find(nodeId) != _nodeId2cidMap.end());

	return this->at(_nodeId2cidMap[nodeId]);
}

template <typename T>
uint64_t ComponentArray<T>::getNodeId(uint64_t index)
{
	return _cid2nodeIdMap[index];
}