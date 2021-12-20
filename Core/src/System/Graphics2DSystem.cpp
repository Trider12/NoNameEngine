#include "System/Graphics2DSystem.hpp"
#include "System/SystemManager.hpp"

namespace
{
	const uint8_t initialNumberOfComponents = 64;
}

Graphics2DSystem::Graphics2DSystem(SystemManager& manager)
	: System(manager)
	, _globalTransforms{ ComponentArray<sf::Transform>(initialNumberOfComponents) }
	, _streamQuadVertices{ ComponentArray<QuadVerticesComponent>(initialNumberOfComponents) }
{
}

void Graphics2DSystem::update(float delta)
{
	std::scoped_lock lock(_systemManager.getSystem<PhysicsSystem>().mutex);

	for (uint64_t i = 0; i < _globalTransforms.activeCount(); ++i)
	{
		_globalTransforms[i] = _systemManager.getComponent<TransformComponent>(_globalTransforms.getNodeId(i)).globalTransform;
	}
}

void Graphics2DSystem::setRenderTarget(sf::RenderTarget* target)
{
	_renderTarget = target;
}