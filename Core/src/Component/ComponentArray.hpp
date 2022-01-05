#pragma once

#include <unordered_map>

class Node2D;

template <typename T>
class ComponentArray
{
public:
	struct Iterator
	{
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = value_type*;
		using reference = value_type&;

		Iterator() {}
		Iterator(pointer ptr) : _ptr(ptr) {}

		reference operator*() const { return *_ptr; }
		pointer operator->() { return _ptr; }
		Iterator& operator++() { ++_ptr; return *this; }
		Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }
		friend bool operator== (const Iterator& a, const Iterator& b) { return a._ptr == b._ptr; };
		friend bool operator!= (const Iterator& a, const Iterator& b) { return a._ptr != b._ptr; };
	private:
		pointer _ptr = nullptr;
	};

	static_assert(std::forward_iterator<Iterator>);

	ComponentArray(uint64_t size);

	T& operator [](uint64_t i);

	Iterator begin();
	Iterator end();

	uint64_t activeCount() const;

	void addComponent(uint64_t nodeId);
	void addComponent(uint64_t nodeId, T component);
	void removeComponent(uint64_t nodeId);
	T& getComponent(uint64_t nodeId);

	uint64_t getNodeId(uint64_t index);

private:
	std::vector<T> _components;
	std::unordered_map<uint64_t, uint64_t> _nodeId2cidMap;
	std::unordered_map<uint64_t, uint64_t> _cid2nodeIdMap;
	uint64_t _activeCount = 0;
};

#include "ComponentArray.tpp"