#include "System/RenderSystem.hpp"
#include "System/SystemManager.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/VertexBuffer.hpp>

namespace
{
	const uint8_t initialNumberOfComponents = 64;

	const float vertexBufferCapacityLowerThreshold = 0.6f;
	const float vertexBufferCapacityUpperThreshold = 0.8f;
}

RenderSystem::RenderSystem(SystemManager& manager)
	: System(manager)
	, _triangulatedPrimitiveComponents{ ComponentArray<TriangulatedPrimitiveComponent>(initialNumberOfComponents) }
{
}

void RenderSystem::update(float delta)
{
	if (_resetVertexBuffer)
	{
		resetBuffer();

		_transformsDirty = false;
	}
	else if (_transformsDirty)
	{
		std::scoped_lock lock(_systemManager.getSystem<PhysicsSystem>().mutex);

		for (uint64_t i = 0; i < _triangulatedPrimitiveComponents.activeCount(); ++i)
		{
			auto& transform = _systemManager.getComponent<TransformComponent>(_triangulatedPrimitiveComponents.getNodeId(i)).globalTransform;

			updatePrimitiveVertexBufferData(_triangulatedPrimitiveComponents[i], transform);
		}

		_transformsDirty = false;
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

	resetBuffer();
}

void RenderSystem::cleanup()
{
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

	if (_triangleVertexBuffer == nullptr)
	{
		_triangleVertexBuffer = new sf::VertexBuffer(sf::PrimitiveType::Triangles);
		_triangleVertexBuffer->create(vertices.size());
	}

	assert(vertices.size() <= _triangleVertexBuffer->getVertexCount());

	_triangleVertexBuffer->update(vertices.data(), vertices.size(), 0);
	_activeTrianglesVerticesCount = vertices.size();

	_resetVertexBuffer = false;
}

void RenderSystem::reallocateVertexBufferIfNeeded()
{
	auto ratio = _activeTrianglesVerticesCount * 1.f / _triangleVertexBuffer->getVertexCount();

	auto newCapacity =
		ratio > vertexBufferCapacityUpperThreshold || ratio < vertexBufferCapacityLowerThreshold ?
		unsigned(_activeTrianglesVerticesCount / vertexBufferCapacityUpperThreshold) : 0;

	if (newCapacity == 0)
	{
		return;
	}

#pragma warning(disable : 26812) // FFS Microsoft
	auto newBuffer = new sf::VertexBuffer(_triangleVertexBuffer->getPrimitiveType());
	newBuffer->create(newCapacity);
	newBuffer->update(*_triangleVertexBuffer);

	_triangleVertexBuffer->create(0);
	delete _triangleVertexBuffer;

	_triangleVertexBuffer = newBuffer;
}

void RenderSystem::updatePrimitiveVertexBufferData(const TriangulatedPrimitiveComponent& primitive, const sf::Transform& transform)
{
	std::vector<sf::Vertex> vertices; // glBufferSubData might not yet be finished, so have to use new memory

	vertices.reserve(primitive.trianglePointsCount);

	for (uint64_t j = 0; j < primitive.trianglePointsCount; ++j)
	{
		vertices.push_back({ transform.transformPoint(primitive.trianglePoints[j]), primitive.color });
	}

	assert(vertices.size() <= _triangleVertexBuffer->getVertexCount());

	_triangleVertexBuffer->update(vertices.data(), vertices.size(), unsigned(primitive.bufferOffset));
}