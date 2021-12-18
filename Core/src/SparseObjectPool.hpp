#pragma once

#include <memory>
#include <queue>

template <typename T>
requires std::is_class_v<T>
class SparseObjectPool
{
public:
	struct Deleter
	{
		explicit Deleter(SparseObjectPool<T>& pool) : _pool{ pool } {}

		void operator()(T* ptr)
		{
			auto index = reinterpret_cast<ActivatableObject*>(ptr) - _pool._objects;

			_pool._objects[index].isActive = false;

			if (index == _pool._numActive)
			{
				_pool._numActive--;
				return;
			}
			else
			{
				_pool._holes.push(index);
			}
		}

	private:
		SparseObjectPool<T>& _pool;
	};

	using pool_ptr = std::unique_ptr<T, Deleter>;

protected:
	~SparseObjectPool()
	{
		delete[] _objects;
	}

	pool_ptr acquire()
	{
		uint64_t index;

		if (_holes.empty())
		{
			index = _numActive++;
		}
		else
		{
			index = _holes.front();
			_holes.pop();
		}

		return pool_ptr{ &_objects[index].value, Deleter(*this) };
	}

	SparseObjectPool(uint64_t size)
	{
		_objects = new ActivatableObject[_maxSize = size];
	}

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

		Iterator& operator++()
		{
			auto next = reinterpret_cast<ActivatableObject*>(_ptr) + 1;
			for (; !next->isActive; next++);
			_ptr = &next->value;
			return *this;
		}

		Iterator operator++(int) {
			Iterator tmp = *this;
			++(*this);
			return tmp;
		}

		friend bool operator== (const Iterator& a, const Iterator& b) { return a._ptr == b._ptr; };
		friend bool operator!= (const Iterator& a, const Iterator& b) { return a._ptr != b._ptr; };
	private:
		pointer _ptr;
	};

	//static_assert(std::forward_iterator<Iterator>); // idk why it fails

	Iterator begin() { return Iterator(&_objects[0].value); }
	Iterator end() { return Iterator(&_objects[_numActive].value); }

private:
#pragma pack(push, 1) // benchmarking needed
	struct ActivatableObject
	{
		T value;
		bool isActive : 1 = false;
	};
#pragma pack(pop)

	ActivatableObject* _objects = nullptr;
	uint64_t _maxSize = 0;
	uint64_t _numActive = 0;
	std::queue<uint64_t> _holes;
};