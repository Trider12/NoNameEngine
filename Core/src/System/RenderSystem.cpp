#include "System/RenderSystem.hpp"
#include "System/SystemManager.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/VertexBuffer.hpp>

namespace
{
	const uint8_t initialNumberOfComponents = 64;
}

RenderSystem::RenderSystem(SystemManager& manager)
	: System(manager)
	, _triangulatedPrimitiveComponents{ ComponentArray<TriangulatedPrimitiveComponent>(initialNumberOfComponents) }
{
}

void RenderSystem::update(float delta)
{
	std::scoped_lock lock(mutex);

	if (_transformsDirty)
	{
		if (_triangulatedPrimitiveComponents.update())
		{
			resetBuffer();
		}
		else
		{
			std::scoped_lock lock(_systemManager.getSystem<PhysicsSystem>().mutex);

			for (uint64_t i = 0; i < _triangulatedPrimitiveComponents.activeCount(); ++i)
			{
				auto transform = _systemManager.getComponent<TransformComponent>(_triangulatedPrimitiveComponents.getNodeId(i)).globalTransform;
				updatePrimitiveVertexBufferData(_triangulatedPrimitiveComponents[i], transform);
			}

			_transformsDirty = false;
		}
	}

	_renderTarget->draw(*_triangleVertexBuffer, 0, _activeTrianglesVerticesCount);

#if DEBUG
	for (const auto& data : _debugDrawData)
	{
		_renderTarget->draw(data.vertices.data(), data.vertices.size(), data.type);
	}
#endif // DEBUG
}

void RenderSystem::init(sf::RenderTarget& target)
{
	_renderTarget = &target;

	_triangleVertexBuffer = new sf::VertexBuffer(sf::PrimitiveType::Triangles);

	resetBuffer();
}

void RenderSystem::cleanup()
{
	std::scoped_lock lock(mutex); // just in case

	_triangleVertexBuffer->create(0);
	delete _triangleVertexBuffer;
}

void RenderSystem::setTransformsDirty()
{
	_transformsDirty = true;
}

void RenderSystem::resetBuffer()
{
	static std::vector<sf::Vertex> vertices;
	vertices.clear();

	{
		std::scoped_lock lock(_systemManager.getSystem<PhysicsSystem>().mutex);

		for (uint64_t i = 0; i < _triangulatedPrimitiveComponents.activeCount(); ++i)
		{
			auto& primitive = _triangulatedPrimitiveComponents[i];

			primitive.bufferOffset = unsigned(vertices.size());

			const auto& transform = _systemManager.getComponent<TransformComponent>(_triangulatedPrimitiveComponents.getNodeId(i)).globalTransform;

			for (uint64_t j = 0; j < primitive.trianglePointsCount; ++j)
			{
				vertices.push_back({ transform.transformPoint(primitive.trianglePoints[j]), primitive.color });
			}
		}
	}

	_triangleVertexBuffer->create(0);
	_triangleVertexBuffer->create(vertices.size());

	_triangleVertexBuffer->update(vertices.data(), vertices.size(), 0);
	_activeTrianglesVerticesCount = vertices.size();
	_tempTrianglesVerticesCount = 0;

	_transformsDirty = false;
}

void RenderSystem::updatePrimitiveVertexBufferData(const TriangulatedPrimitiveComponent& primitive, const sf::Transform& transform)
{
	auto vertices = new sf::Vertex[primitive.trianglePointsCount]; // glBufferSubData might not yet be finished, so have to use new memory

	for (auto i = 0u; i < primitive.trianglePointsCount; ++i)
	{
		vertices[i] = { transform.transformPoint(primitive.trianglePoints[i]), primitive.color };
	}

	_triangleVertexBuffer->update(vertices, primitive.trianglePointsCount, unsigned(primitive.bufferOffset));

	delete[] vertices;
}