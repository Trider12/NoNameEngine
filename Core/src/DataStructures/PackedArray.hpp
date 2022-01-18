#pragma once

#include <cstdint>
#include <vector>

template <typename T>
class PackedArray
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

	PackedArray(uint64_t size);

	T& operator [](uint64_t i);

	Iterator begin();
	Iterator end();

	uint64_t size() const;
	uint64_t insert(T element);
	void erase(uint64_t i);
	T& at(uint64_t i);

private:
	std::vector<T> _components;
	uint64_t _activeCount = 0;
};

template <typename T>
PackedArray<T>::PackedArray(uint64_t size)
{
	_components.resize(size);
}

template <typename T>
T& PackedArray<T>::operator[](uint64_t i)
{
	return _components[i];
}

template <typename T>
PackedArray<T>::Iterator PackedArray<T>::begin()
{
	return Iterator(&_components[0]);
}

template <typename T>
PackedArray<T>::Iterator PackedArray<T>::end()
{
	return Iterator(&_components[_activeCount]);
}

template <typename T>
uint64_t PackedArray<T>::size() const
{
	return _activeCount;
}

template <typename T>
uint64_t PackedArray<T>::insert(T element)
{
	auto index = _activeCount++;
	_components[index] = element;

	if (_activeCount == _components.size())
	{
		_components.resize(size_t(_components.size() * 1.5f));
	}

	return index;
}

template <typename T>
void PackedArray<T>::erase(uint64_t i)
{
	_components[i] = _components[_activeCount--];
}

template <typename T>
T& PackedArray<T>::at(uint64_t i)
{
	return _components.at(i);
}