#pragma once

#include "System/System.hpp"
#include "Component/Component.hpp"
#include "Component/ComponentArray.hpp"
#include "Node/Primitive2D.hpp"
#include "Core/Debug.hpp"

#if DEBUG
#include <SFML/Graphics/PrimitiveType.hpp>
#endif // DEBUG

namespace sf
{
	class RenderTarget;
	class VertexBuffer;
}

class RenderSystem : public System
{
public:
	void init(sf::RenderTarget& target);

	void cleanup();

	void update(float delta);

	void setTransformsDirty();

	template <DerivedComponent T>
	void addComponent(const Node2D& node, T component);

	template <DerivedComponent T>
	void removeComponent(const Node2D& node);

#if DEBUG
	struct DebugDrawData
	{
		std::vector<sf::Vertex> vertices;
		sf::PrimitiveType type;
	};

	std::vector<DebugDrawData> _debugDrawData;

#pragma warning(disable : 26812) // FFS Microsoft
	void debugDraw(const sf::Vertex* vertices, std::size_t vertexCount, sf::PrimitiveType type)
	{
		_debugDrawData.push_back({ {vertices, vertices + vertexCount}, type });
	}

	void debugClear()
	{
		_debugDrawData.clear();
	}
#endif // DEBUG

private:
	RenderSystem(SystemManager& manager);

	void resetBuffer();
	void reallocateVertexBufferIfNeeded();
	void updatePrimitiveVertexBufferData(const TriangulatedPrimitiveComponent& primitive, const sf::Transform& transform);

	sf::RenderTarget* _renderTarget = nullptr;

	ComponentArray<TriangulatedPrimitiveComponent> _triangulatedPrimitiveComponents;
	sf::VertexBuffer* _triangleVertexBuffer = nullptr;

	uint64_t _activeTrianglesVerticesCount = 0;
	bool _resetVertexBuffer = false;
	bool _transformsDirty = true;

	friend class SystemManager;
};

template <DerivedComponent T>
void RenderSystem::addComponent(const Node2D& node, T component)
{
	if constexpr (std::is_same_v<T, TriangulatedPrimitiveComponent>)
	{
		if (_renderTarget != nullptr)
		{
			component.bufferOffset = _activeTrianglesVerticesCount;
			_activeTrianglesVerticesCount += component.trianglePointsCount;

			reallocateVertexBufferIfNeeded();
		}

		_triangulatedPrimitiveComponents.addComponent(node.getId(), component);
	}
	else
	{
		static_assert(always_false<T>, "No specialization!");
	}
}

template <DerivedComponent T>
void RenderSystem::removeComponent(const Node2D& node)
{
	if constexpr (std::is_same_v<T, TriangulatedPrimitiveComponent>)
	{
		auto& component = _triangulatedPrimitiveComponents.getComponent(node.getId()); // TODO: improve this
		delete[] component.trianglePoints;
		component.trianglePoints = nullptr;

		_triangulatedPrimitiveComponents.removeComponent(node.getId());

		_resetVertexBuffer = true;
	}
	else
	{
		static_assert(always_false<T>, "No specialization!");
	}
}

static_assert (DerivedSystem<RenderSystem>);