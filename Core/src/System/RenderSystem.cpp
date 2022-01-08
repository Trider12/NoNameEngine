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
	, _trianglePrimitiveComponents{ ComponentArray<TrianglePrimitiveComponent>(initialNumberOfComponents) }
{
}

void RenderSystem::update(float delta)
{
	if (_transformsDirty)
	{
		std::scoped_lock lock(_systemManager.getSystem<PhysicsSystem>().mutex);

		for (uint64_t i = 0; i < _trianglePrimitiveComponents.activeCount(); ++i)
		{
			auto& primitive = _trianglePrimitiveComponents[i];

			auto newTransform = _systemManager.getComponent<TransformComponent>(_trianglePrimitiveComponents.getNodeId(i)).globalTransform;

			if (primitive.transform == newTransform)
			{
				continue;
			}

			primitive.transform = newTransform;
			primitive.dirty = true;
			_triangleVertexBufferDirty = true;
		}

		_transformsDirty = false;
	}

	if (_triangleVertexBufferNeedsResetting)
	{
		resetBuffer();
	}

	if (_triangleVertexBufferDirty)
	{
		for (uint64_t i = 0; i < _trianglePrimitiveComponents.activeCount(); ++i)
		{
			auto& primitive = _trianglePrimitiveComponents[i];

			if (!primitive.dirty)
			{
				continue;
			}

			updatePrimitiveVertexBufferData(primitive);

			primitive.dirty = false;
		}

		_triangleVertexBufferDirty = false;
	}

	_renderTarget->draw(*_triangleVertexBuffer, 0, _activeTriangleVerticesCount);
}

void RenderSystem::init(sf::RenderTarget& target)
{
	_renderTarget = &target;

	resetBuffer();
}

void RenderSystem::cleanup()
{
	delete _triangleVertexBuffer;
}

void RenderSystem::setTransformsDirty()
{
	_transformsDirty = true;
}

void RenderSystem::resetBuffer()
{
	std::vector<sf::Vertex> vertices;

	for (uint64_t i = 0; i < _trianglePrimitiveComponents.activeCount(); ++i)
	{
		auto& primitive = _trianglePrimitiveComponents[i];

		primitive.bufferOffset = unsigned(vertices.size());
		primitive.dirty = false;

		for (uint64_t j = 0; j < primitive.trianglePointsCount; ++j)
		{
			vertices.push_back({ primitive.transform.transformPoint(primitive.trianglePoints[j]), primitive.color });
		}
	}

	if (_triangleVertexBuffer == nullptr)
	{
		_triangleVertexBuffer = new sf::VertexBuffer(sf::PrimitiveType::Triangles);
		_triangleVertexBuffer->create(vertices.size());
	}

	_triangleVertexBuffer->update(vertices.data(), vertices.size(), 0);
	_activeTriangleVerticesCount = vertices.size();

	_triangleVertexBufferNeedsResetting = false;
	_triangleVertexBufferDirty = false;
}

void RenderSystem::reallocateVertexBufferIfNeeded()
{
	auto ratio = _activeTriangleVerticesCount * 1.f / _triangleVertexBuffer->getVertexCount();

	auto newCapacity =
		ratio > vertexBufferCapacityUpperThreshold || ratio < vertexBufferCapacityLowerThreshold ?
		unsigned(_triangleVertexBuffer->getVertexCount() / vertexBufferCapacityUpperThreshold) : 0;

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

void RenderSystem::updatePrimitiveVertexBufferData(const TrianglePrimitiveComponent& primitive)
{
	std::vector<sf::Vertex> vertices;
	vertices.reserve(primitive.trianglePointsCount);

	for (uint64_t j = 0; j < primitive.trianglePointsCount; ++j)
	{
		vertices.push_back({ primitive.transform.transformPoint(primitive.trianglePoints[j]), primitive.color });
	}

	_triangleVertexBuffer->update(vertices.data(), vertices.size(), unsigned(primitive.bufferOffset));
}