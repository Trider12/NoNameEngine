#pragma once

#include <SFML/Graphics/VertexBuffer.hpp>

#include "System/System.hpp"
#include "Component/Component.hpp"
#include "Component/ComponentArray.hpp"

class Graphics2DSystem : public System
{
public:
	void update(float delta);

	void setRenderTarget(sf::RenderTarget* target);

	template <ComponentType type>
	void addComponent(uint64_t nodeId)
	{
		if constexpr (type == ComponentType::ColorRect)
		{
			_globalTransforms.addComponent(nodeId);
			_streamQuadVertices.addComponent(nodeId);
		}
		else
		{
			static_assert(always_false<type>, "No specialization!");
		}
	}

private:
	Graphics2DSystem(SystemManager& manager);

	ComponentArray<sf::Transform> _globalTransforms;
	ComponentArray<QuadVerticesComponent> _streamQuadVertices;

	//sf::VertexBuffer _streamBuffer;

	sf::RenderTarget* _renderTarget = nullptr;

	friend class SystemManager;
};

static_assert (DerivedSystem<Graphics2DSystem>);