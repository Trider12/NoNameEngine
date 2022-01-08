#pragma once

#include <SFML/Graphics/VertexBuffer.hpp>

#include "System/System.hpp"
#include "Component/Component.hpp"
#include "Component/ComponentArray.hpp"
#include "Node/Sprite.hpp"

class RenderSystem : public System
{
public:
	void update(float delta);

	void setRenderTarget(sf::RenderTarget* target);

	void setTransformsDirty();

	template <DerivedComponent T>
	void addComponent(const Node2D& node)
	{
		if constexpr (std::is_same_v<T, QuadVerticesComponent>)
		{
			const auto& sprite = static_cast<const Sprite&>(node);

			_globalTransforms.addComponent(node.getId());

			auto halfDims = sprite.getDimensions() * 0.5f;
			auto& color = sprite.getColor();

			auto quad = QuadVerticesComponent
			(
				sf::Vertex(-halfDims, color),
				sf::Vertex({ halfDims.x, -halfDims.y }, color),
				sf::Vertex(halfDims, color),
				sf::Vertex({ -halfDims.x, halfDims.y }, color)
			);

			_streamQuadVertices.addComponent(node.getId(), quad);
		}
		else
		{
			static_assert(always_false<T>, "No specialization!");
		}
	}

	template <DerivedComponent T>
	void removeComponent(const Node2D& node)
	{
		if constexpr (std::is_same_v<T, QuadVerticesComponent>)
		{
			_globalTransforms.removeComponent(node.getId());
			_streamQuadVertices.removeComponent(node.getId());
		}
		else
		{
			static_assert(always_false<T>, "No specialization!");
		}
	}

private:
	RenderSystem(SystemManager& manager);

	ComponentArray<sf::Transform> _globalTransforms;
	ComponentArray<QuadVerticesComponent> _streamQuadVertices;

	sf::RenderTarget* _renderTarget = nullptr;
	std::atomic_bool _transformsDirty = true;

	friend class SystemManager;
};

static_assert (DerivedSystem<RenderSystem>);