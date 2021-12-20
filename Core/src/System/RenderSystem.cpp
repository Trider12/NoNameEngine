#include "System/RenderSystem.hpp"
#include "System/SystemManager.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace
{
	const uint8_t initialNumberOfComponents = 64;
}

RenderSystem::RenderSystem(SystemManager& manager)
	: System(manager)
	, _globalTransforms{ ComponentArray<sf::Transform>(initialNumberOfComponents) }
	, _streamQuadVertices{ ComponentArray<QuadVerticesComponent>(initialNumberOfComponents) }
{
}

void RenderSystem::update(float delta)
{
	if (_transformsDirty)
	{
		std::scoped_lock lock(_systemManager.getSystem<PhysicsSystem>().mutex);

		for (uint64_t i = 0; i < _globalTransforms.activeCount(); ++i)
		{
			_globalTransforms[i] = _systemManager.getComponent<TransformComponent>(_globalTransforms.getNodeId(i)).globalTransform;
		}

		_transformsDirty = false;
	}

	for (uint64_t i = 0; i < _globalTransforms.activeCount(); ++i)
	{
		const auto& transform = _globalTransforms[i];
		const auto& quad = _streamQuadVertices[i];

		_renderTarget->draw(quad.vertices, 4, sf::PrimitiveType::Quads, sf::RenderStates(transform));
	}
}

void RenderSystem::setRenderTarget(sf::RenderTarget* target)
{
	_renderTarget = target;
}

void RenderSystem::setTransformsDirty()
{
	_transformsDirty = true;
}