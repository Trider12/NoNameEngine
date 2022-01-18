#pragma once

#include "FreeList.hpp"

#include <stack>
#include <cassert>

template <typename T>
concept IntegralOrFloating = std::is_integral_v<T> || std::is_floating_point_v<T>;

// not using this concept for now
template <template <typename, typename...> typename C, typename T, typename... Args>
concept RandomAccessContainer = requires (C<T, Args...> container, int index)
{
	{ container.operator[](index) } -> std::same_as<T&>;
};

// TODO: add element removal support
template <IntegralOrFloating T>
class QuadTree
{
public:
	QuadTree();
	QuadTree(T centerX, T centerY, T width, T height, uint8_t depth);

	void init(T centerX, T centerY, T width, T height, uint8_t depth);
	void insert(int32_t elementExternalIndex, T left, T top, T width, T height);
	void clear();
	uint64_t size() { return _elements.size(); }

	void forEachUniqueElementPairInEachLeaf(std::invocable<int, int> auto func); // probably not the best solution performance-wise

private:
	struct QuadNode
	{
		static const uint8_t maxElements = 8;

		union
		{
			int32_t childNodePackIndex = -1;
			int32_t firstElementNodeIndex;
		};

		int8_t elementsCount = -1;
	};

	struct QuadBounds
	{
		T centerX, centerY, halfWidth, halfHeight;
	};

	struct QuadNodeData
	{
		QuadNode* node = nullptr;
		const QuadBounds bounds;
		const uint8_t depth = 0;
	};

	struct QuadNodePack
	{
		QuadNode topLeft, topRight, bottomLeft, bottomRight;
	};

	struct QuadElement
	{
		int32_t elementExternalIndex = -1;
		T left, top, width, height;
	};

	struct QuadElementNode
	{
		int32_t nextElementNodeIndex = -1;
		int32_t elementInternalIndex = -1;
	};

	void insertIntoNode(QuadNodeData& nodeData, int32_t elementNodeIndex);
	void insertIntoLeaf(QuadNodeData& leafData, int32_t elementNodeIndex);

	std::vector<QuadNodeData> findLeavesIntersectingRect(QuadNodeData& rootData, T rectLeft, T rectTop, T rectWidth, T rectHeight);

	QuadNode _root;
	QuadBounds _bounds;
	uint8_t _maxDepth;

	std::vector<QuadNodePack> _nodePacks;
	FreeList<QuadElement> _elements;
	FreeList<QuadElementNode> _elementNodes;
};

template <IntegralOrFloating T>
QuadTree<T>::QuadTree() : _root{ -1, 0 }, _maxDepth{ 0 }, _bounds{}
{
}

template <IntegralOrFloating T>
QuadTree<T>::QuadTree(T centerX, T centerY, T width, T height, uint8_t depth) : _root{ -1, 0 }
{
	init(centerX, centerY, width, height, depth);
}

template <IntegralOrFloating T>
void QuadTree<T>::init(T centerX, T centerY, T width, T height, uint8_t depth)
{
	_bounds = { centerX, centerY, width / 2, height / 2 };
	_maxDepth = depth;
}

template <IntegralOrFloating T>
void QuadTree<T>::insert(int32_t elementExternalIndex, T left, T top, T width, T height)
{
	QuadElementNode node;
	node.elementInternalIndex = static_cast<int32_t>(_elements.insert({ elementExternalIndex, left, top, width, height }));

	QuadNodeData rootData = { &_root, _bounds, 0 };

	insertIntoNode(rootData, static_cast<int32_t>(_elementNodes.insert(node)));
}

template <IntegralOrFloating T>
void QuadTree<T>::clear()
{
	_nodePacks.clear();
	_elementNodes.clear();
	_elements.clear();
	_root = { -1,0 };
}

template <IntegralOrFloating T>
void QuadTree<T>::forEachUniqueElementPairInEachLeaf(std::invocable<int, int> auto func)
{
	std::stack<QuadNode> toVisit;
	toVisit.push(_root);

	while (!toVisit.empty())
	{
		const auto node = std::move(toVisit.top());
		toVisit.pop();

		if (node.elementsCount == -1)
		{
			const auto& pack = _nodePacks[node.childNodePackIndex];

			toVisit.push(pack.topLeft);
			toVisit.push(pack.topRight);
			toVisit.push(pack.bottomLeft);
			toVisit.push(pack.bottomRight);

			continue;
		}

		for (auto index1 = node.firstElementNodeIndex; index1 != -1; index1 = _elementNodes[index1].nextElementNodeIndex)
		{
			for (auto index2 = _elementNodes[index1].nextElementNodeIndex; index2 != -1; index2 = _elementNodes[index2].nextElementNodeIndex)
			{
				auto i = _elements[_elementNodes[index1].elementInternalIndex].elementExternalIndex;
				auto j = _elements[_elementNodes[index2].elementInternalIndex].elementExternalIndex;

				func(i, j);
			}
		}
	}
}

template <IntegralOrFloating T>
void QuadTree<T>::insertIntoNode(QuadNodeData& nodeData, int32_t elementNodeIndex)
{
	const auto& element = _elements[_elementNodes[elementNodeIndex].elementInternalIndex];
	auto leaves = findLeavesIntersectingRect(nodeData, element.left, element.top, element.width, element.height);

	for (auto& leafData : leaves)
	{
		insertIntoLeaf(leafData, elementNodeIndex);
	}
}

template <IntegralOrFloating T>
void QuadTree<T>::insertIntoLeaf(QuadNodeData& leafData, int32_t elementNodeIndex)
{
	auto& leaf = *leafData.node;
	auto& elementNode = _elementNodes[elementNodeIndex];

	assert(leaf.elementsCount != -1);

	elementNode.nextElementNodeIndex = leaf.firstElementNodeIndex;
	leaf.firstElementNodeIndex = elementNodeIndex;
	leaf.elementsCount++;

	if (leaf.elementsCount > QuadNode::maxElements && leafData.depth < _maxDepth)
	{
		std::vector<int32_t> elementNodesIndexes;
		elementNodesIndexes.reserve(leaf.elementsCount);

		for (; leaf.firstElementNodeIndex != -1; leaf.firstElementNodeIndex = _elementNodes[leaf.firstElementNodeIndex].nextElementNodeIndex)
		{
			elementNodesIndexes.push_back(leaf.firstElementNodeIndex);

			assert(elementNodesIndexes.size() <= leaf.elementsCount);
		}

		leaf.elementsCount = -1;
		leaf.childNodePackIndex = static_cast<int32_t>(_nodePacks.size());
		_nodePacks.push_back({ {-1, 0}, {-1, 0}, {-1, 0}, {-1, 0} });

		// just WTF

		if (leaf.elementsCount != -1)
		{
			__debugbreak(); // am I loosing my mind or is this a bug of MSVC???
		}

		for (auto index : elementNodesIndexes)
		{
			insertIntoNode(leafData, index);
		}
	}
}

template <IntegralOrFloating T>
std::vector<typename QuadTree<T>::QuadNodeData> QuadTree<T>::findLeavesIntersectingRect(QuadNodeData& rootData, T rectLeft, T rectTop, T rectWidth, T rectHeight)
{
	std::vector<QuadNodeData> leaves;
	std::stack<QuadNodeData> toVisit;
	toVisit.push(rootData);

	while (!toVisit.empty())
	{
		const auto nodeData = std::move(toVisit.top());
		toVisit.pop();

		const auto& node = *nodeData.node;

		if (node.elementsCount != -1)
		{
			leaves.push_back(nodeData);
		}
		else
		{
			const auto rectRight = rectLeft + rectWidth, rectBottom = rectTop + rectHeight;
			const uint8_t childDepth = nodeData.depth + 1;
			const auto& nodeBounds = nodeData.bounds;
			const auto childBoundsHalfWidth = nodeBounds.halfWidth / 2, childBoundsHalfHeight = nodeBounds.halfHeight / 2;
			const auto
				childBoundsLeft = nodeBounds.centerX - childBoundsHalfWidth,
				childBoundsRight = nodeBounds.centerX + childBoundsHalfWidth,
				childBoundsTop = nodeBounds.centerY - childBoundsHalfHeight,
				childBoundsBottom = nodeBounds.centerY + childBoundsHalfHeight;

			auto& pack = _nodePacks[node.childNodePackIndex];

			if (rectTop <= nodeBounds.centerY)
			{
				if (rectLeft <= nodeBounds.centerX)
					toVisit.push({ &pack.topLeft, { childBoundsLeft, childBoundsTop, childBoundsHalfWidth, childBoundsHalfHeight }, childDepth });

				if (rectRight > nodeBounds.centerX)
					toVisit.push({ &pack.topRight, { childBoundsRight, childBoundsTop, childBoundsHalfWidth, childBoundsHalfHeight }, childDepth });
			}

			if (rectBottom > nodeBounds.centerY)
			{
				if (rectLeft <= nodeBounds.centerX)
					toVisit.push({ &pack.bottomLeft, { childBoundsLeft, childBoundsBottom, childBoundsHalfWidth, childBoundsHalfHeight}, childDepth });

				if (rectRight > nodeBounds.centerX)
					toVisit.push({ &pack.bottomRight, {childBoundsRight, childBoundsBottom, childBoundsHalfWidth, childBoundsHalfHeight}, childDepth });
			}
		}
	}

	return leaves;
}