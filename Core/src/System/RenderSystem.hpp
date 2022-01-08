#pragma once

#include "System/System.hpp"
#include "Component/Component.hpp"
#include "Component/ComponentArray.hpp"
#include "Node/Sprite.hpp"

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

private:
	RenderSystem(SystemManager& manager);

	void resetBuffer();
	void reallocateVertexBufferIfNeeded();
	void updatePrimitiveVertexBufferData(const TrianglePrimitiveComponent& primitive);

	sf::RenderTarget* _renderTarget = nullptr;

	ComponentArray<TrianglePrimitiveComponent> _trianglePrimitiveComponents;
	sf::VertexBuffer* _triangleVertexBuffer = nullptr;

	uint64_t _activeTriangleVerticesCount = 0;
	bool _triangleVertexBufferNeedsUpdating = false;
	bool _triangleVertexBufferNeedsResetting = false;
	bool _transformsDirty = true;

	friend class SystemManager;
};

template <DerivedComponent T>
void RenderSystem::addComponent(const Node2D& node, T component)
{
	if constexpr (std::is_same_v<T, TrianglePrimitiveComponent>)
	{
		if (_renderTarget != nullptr)
		{
			component.bufferOffset = _activeTriangleVerticesCount;
			_activeTriangleVerticesCount += component.trianglePointsCount;

			reallocateVertexBufferIfNeeded();
			updatePrimitiveVertexBufferData(component);
		}

		_trianglePrimitiveComponents.addComponent(node.getId(), component);
	}
	else
	{
		static_assert(always_false<T>, "No specialization!");
	}
}

template <DerivedComponent T>
void RenderSystem::removeComponent(const Node2D& node)
{
	if constexpr (std::is_same_v<T, TrianglePrimitiveComponent>)
	{
		auto& component = _trianglePrimitiveComponents.getComponent(node.getId()); // TODO: improve this
		delete[] component.trianglePoints;
		component.trianglePoints = nullptr;

		_trianglePrimitiveComponents.removeComponent(node.getId());

		_triangleVertexBufferNeedsResetting = true;
	}
	else
	{
		static_assert(always_false<T>, "No specialization!");
	}
}

static_assert (DerivedSystem<RenderSystem>);