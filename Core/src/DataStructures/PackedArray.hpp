#pragma once

#include <numbers>
#include <algorithm>

template <typename T, std::unsigned_integral uint>
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

	PackedArray(uint size);
	~PackedArray();

	T& operator [](uint i);

	Iterator begin();
	Iterator end();

	uint activeCount() const;

	uint add(T element);
	T remove(uint i);
	T& at(uint i);

private:
	void reallocate();

	T* _elements = nullptr;
	uint _size = 0;
	uint _activeCount = 0;
};

template <typename T, std::unsigned_integral uint>
PackedArray<T, uint>::PackedArray(uint size)
{
	_elements = new T[_size = size];
}

template <typename T, std::unsigned_integral uint>
PackedArray<T, uint>::~PackedArray()
{
	delete _elements;
}

template <typename T, std::unsigned_integral uint>
T& PackedArray<T, uint>::operator[](uint i)
{
	return _elements[i];
}

template <typename T, std::unsigned_integral uint>
PackedArray<T, uint>::Iterator PackedArray<T, uint>::begin()
{
	return Iterator(&_elements[0]);
}

template <typename T, std::unsigned_integral uint>
PackedArray<T, uint>::Iterator PackedArray<T, uint>::end()
{
	return Iterator(&_elements[_activeCount]);
}

template <typename T, std::unsigned_integral uint>
uint PackedArray<T, uint>::activeCount() const
{
	return _activeCount;
}

template <typename T, std::unsigned_integral uint>
uint PackedArray<T, uint>::add(T element)
{
	auto index = _activeCount++;
	_elements[index] = element;

	if (_activeCount == _size)
	{
		reallocate();
	}

	return index;
}

template <typename T, std::unsigned_integral uint>
T PackedArray<T, uint>::remove(uint i)
{
	auto element = _elements[i];
	_elements[i] = _elements[--_activeCount];

	return element;
}

template <typename T, std::unsigned_integral uint>
T& PackedArray<T, uint>::at(uint i)
{
	return _elements[i];
}

template <typename T, std::unsigned_integral uint>
void PackedArray<T, uint>::reallocate()
{
	auto newPtr = new T[std::max<uint>(_size + 1, uint(_size * std::numbers::phi_v<float>))];
	std::move(_elements, _elements + _size, newPtr);
	delete _elements;
	_elements = newPtr;
}