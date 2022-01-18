#pragma once

#include <cstdint>
#include <vector>

template <class T>
class FreeList
{
public:
	FreeList();

	T& operator [](uint64_t i);
	const T& operator[](uint64_t i) const;

	uint64_t size() const;
	uint64_t insert(T element);
	void erase(uint64_t i);
	void clear();

private:
	union FreeElement
	{
		T element;
		uint64_t next;
	};

	std::vector<FreeElement> _data;
	uint64_t _size;
	int64_t _firstFree;
};

template <class T>
FreeList<T>::FreeList() : _firstFree(-1), _size(0)
{
}

template <class T>
T& FreeList<T>::operator[](uint64_t i)
{
	return _data[i].element;
}

template <class T>
const T& FreeList<T>::operator[](uint64_t i) const
{
	return _data[i].element;
}

template <class T>
uint64_t FreeList<T>::size() const
{
	return _size;
}

template <class T>
uint64_t FreeList<T>::insert(T element)
{
	_size++;

	if (_firstFree != -1)
	{
		const auto index = _firstFree;
		_firstFree = _data[_firstFree].next;
		_data[index].element = element;

		return index;
	}
	else
	{
		_data.push_back({ element });

		return static_cast<uint64_t>(_data.size() - 1);
	}
}

template <class T>
void FreeList<T>::erase(uint64_t i)
{
	_data[i].next = _firstFree;
	_firstFree = i;
	_size--;
}

template <class T>
void FreeList<T>::clear()
{
	_data.clear();
	_firstFree = -1;
	_size = 0;
}