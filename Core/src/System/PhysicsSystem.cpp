#include "System/PhysicsSystem.hpp"
#include "System/SystemManager.hpp"
#include "Utility/Helpers.hpp"
#include "Core/Debug.hpp"

#if DEBUG
#include "Core/Locator.hpp"
#endif // DEBUG

namespace
{
	const uint8_t initialNumberOfComponents = 32;

	void preventCollision(const CollisionComponent& c1, const CollisionComponent& c2, TransformComponent& t1, TransformComponent& t2);
}

PhysicsSystem::PhysicsSystem(SystemManager& manager)
	: System(manager)
	, _transformComponents{ ComponentArray<TransformComponent>(initialNumberOfComponents) }
	, _collisionComponents{ ComponentArray<CollisionComponent>(initialNumberOfComponents) }
{
}

void PhysicsSystem::update(float delta)
{
	std::scoped_lock lock(mutex);

	for (auto i = 0; i < _collisionComponents.activeCount(); ++i)
	{
		auto& c1 = _collisionComponents[i];
		auto& t1 = _transformComponents[_collisionComponents.getNodeId(i)];

		for (auto j = i + 1; j < _collisionComponents.activeCount(); ++j)
		{
			auto& c2 = _collisionComponents[j];
			auto& t2 = _transformComponents[_collisionComponents.getNodeId(j)];

			preventCollision(c1, c2, t1, t2);
		}
	}

	for (auto& transformComponent : _transformComponents)
	{
		transformComponent.globalTransform *= transformComponent.deferredTransform;
		transformComponent.deferredTransform = sf::Transform::Identity;
	}

	_systemManager.getSystem<RenderSystem>().setTransformsDirty();
}

void PhysicsSystem::transformDeferred(const Node2D& node, sf::Transform transform)
{
	_transformComponents.getComponent(node.getId()).deferredTransform *= transform;
}

namespace
{
	bool rectsAreNotOverlapping(const sf::FloatRect& rect1, const sf::FloatRect& rect2)
	{
		return rect1.left > rect2.left + rect2.width || rect2.left > rect1.left + rect1.width ||
			rect1.top > rect2.top + rect2.height || rect2.top > rect1.top + rect1.height;
	}

	bool pureStaticCheck(const CollisionComponent& c1, const CollisionComponent& c2)
	{
		if (c1.collisionType == CollisionComponent::CollisionType::Static && c2.collisionType == CollisionComponent::CollisionType::Static)
		{
			Debug::Warning("pure static collisions ignored!");
			return true;
		}

		return false;
	}

	bool pureKinematicCheck(const CollisionComponent& c1, const CollisionComponent& c2)
	{
		if (c1.collisionType == CollisionComponent::CollisionType::Kinematic && c2.collisionType == CollisionComponent::CollisionType::Kinematic)
		{
			Debug::Warning("pure kinematic collisions ignored!");
			return true;
		}

		return false;
	}

	void applyCollisionResolvingTranslationToKinematicBody(const CollisionComponent& c1, const CollisionComponent& c2, TransformComponent& t1, TransformComponent& t2, const sf::Vector2f& translation)
	{
		auto resolvingTransform = sf::Transform::Identity;

		if (c1.collisionType == CollisionComponent::CollisionType::Kinematic)
		{
			resolvingTransform.translate(translation);
			t1.deferredTransform *= resolvingTransform;
		}
		else
		{
			resolvingTransform.translate(-translation);
			t2.deferredTransform *= resolvingTransform;
		}
	}

	void preventAABBxAABBCollision(const CollisionComponent& c1, const CollisionComponent& c2, TransformComponent& t1, TransformComponent& t2)
	{
		auto rect1 = (t1.globalTransform * t1.deferredTransform).transformRect(c1.collisionShape.rect);
		auto rect2 = (t2.globalTransform * t2.deferredTransform).transformRect(c2.collisionShape.rect);

		if (rectsAreNotOverlapping(rect1, rect2))
		{
			return;
		}

		auto dx = (rect1.left + rect1.width / 2) - (rect2.left + rect2.width / 2);
		auto dy = (rect1.top + rect1.height / 2) - (rect2.top + rect2.height / 2);
		auto minDx = rect1.width / 2 + rect2.width / 2;
		auto minDy = rect1.height / 2 + rect2.height / 2;

		if (std::fabsf(dx) >= minDx || std::fabsf(dy) >= minDy || pureStaticCheck(c1, c2) || pureKinematicCheck(c1, c2))
		{
			return;
		}

		auto depthX = std::copysignf(minDx, dx) - dx;
		auto depthY = std::copysignf(minDy, dy) - dy;

		auto translation = std::fabsf(depthX) <= std::fabsf(depthY) ? sf::Vector2f(depthX, 0.f) : sf::Vector2f(0.f, depthY);

		applyCollisionResolvingTranslationToKinematicBody(c1, c2, t1, t2, translation);
	}

	void preventCirclexCircleCollision(const CollisionComponent& c1, const CollisionComponent& c2, TransformComponent& t1, TransformComponent& t2)
	{
		auto center1 = (t1.globalTransform * t1.deferredTransform).transformPoint(c1.collisionShape.center);
		auto center2 = (t2.globalTransform * t2.deferredTransform).transformPoint(c2.collisionShape.center);
		auto radius1 = c1.collisionShape.radius;
		auto radius2 = c2.collisionShape.radius;

		if (rectsAreNotOverlapping({ center1.x - radius1, center1.y - radius1, radius1 * 2.f, radius1 * 2.f }, { center2.x - radius2, center2.y - radius2, radius2 * 2.f, radius2 * 2.f }))
		{
			return;
		}

		auto deltaCenter = center1 - center2;
		auto distSqr = VectorHelper::lengthSqr(deltaCenter);

		if (distSqr - std::powf(radius1 + radius2, 2.f) >= 0.f || pureStaticCheck(c1, c2) || pureKinematicCheck(c1, c2))
		{
			return;
		}

		auto translation = (radius1 + radius2 - std::sqrtf(distSqr)) * VectorHelper::normalized(deltaCenter);

		applyCollisionResolvingTranslationToKinematicBody(c1, c2, t1, t2, translation);
	}

	void preventAABBxCircleCollision(const CollisionComponent& c1, const CollisionComponent& c2, TransformComponent& t1, TransformComponent& t2)
	{
		auto rect = (t1.globalTransform * t1.deferredTransform).transformRect(c1.collisionShape.rect);
		auto circleCenter = (t2.globalTransform * t2.deferredTransform).transformPoint(c2.collisionShape.center);
		auto circleRadius = c2.collisionShape.radius;
		auto circleRect = sf::FloatRect(circleCenter.x - circleRadius, circleCenter.y - circleRadius, circleRadius * 2.f, circleRadius * 2.f);

		if (rectsAreNotOverlapping(rect, circleRect))
		{
			return;
		}

		auto halfDims = sf::Vector2f(rect.width / 2.f, rect.height / 2.f);
		auto rectCenter = sf::Vector2f(rect.left + halfDims.x, rect.top + halfDims.y);
		auto radialDirection = VectorHelper::normalized(rectCenter - circleCenter);
		auto circumferencePoint = circleCenter + circleRadius * radialDirection;

		if (!rect.contains(circumferencePoint) || pureStaticCheck(c1, c2) || pureKinematicCheck(c1, c2))
		{
			return;
		}

		// TODO: I have a big feeling this can be simplified

		auto diagonalTangent = rect.height / rect.width;
		auto tangent = std::fabsf(radialDirection.y / radialDirection.x);

		sf::Vector2f translation;
		sf::Vector2f intersection;

		if (tangent > diagonalTangent)
		{
			if (circleCenter.x > rect.left && circleCenter.x < rect.left + rect.width)
			{
				intersection.x = circleCenter.x;
				intersection.y = circleCenter.y + std::copysignf(circleRadius, radialDirection.y);
			}
			else
			{
				intersection.x = rectCenter.x - std::copysignf(halfDims.x, radialDirection.x);
				intersection.y = circleCenter.y + std::copysignf(std::sqrtf(circleRadius * circleRadius - std::powf(intersection.x - circleCenter.x, 2.f)), radialDirection.y);
			}

			translation.y = std::copysignf(halfDims.y, radialDirection.y) + intersection.y - rectCenter.y;
		}
		else
		{
			if (circleCenter.y > rect.top && circleCenter.y < rect.top + rect.height)
			{
				intersection.y = circleCenter.y;
				intersection.x = circleCenter.x + std::copysignf(circleRadius, radialDirection.x);
			}
			else
			{
				intersection.y = rectCenter.y - std::copysignf(halfDims.y, radialDirection.y);
				intersection.x = circleCenter.x + std::copysignf(std::sqrtf(circleRadius * circleRadius - std::powf(intersection.y - circleCenter.y, 2.f)), radialDirection.x);
			}

			translation.x = std::copysignf(halfDims.x, radialDirection.x) + intersection.x - rectCenter.x;
		}

		applyCollisionResolvingTranslationToKinematicBody(c1, c2, t1, t2, translation);

#if DEBUG & 0
		sf::Vertex lines[] =
		{
			{ rectCenter, sf::Color::Red}, { intersection, sf::Color::Red},
			{ intersection, sf::Color::Green}, { intersection + translation, sf::Color::Green}
		};

		auto& renderSystem = Locator::getInstance().getSystemManager().getSystem<RenderSystem>();
		renderSystem.debugClear();
		renderSystem.debugDraw(lines, 4, sf::Lines);
#endif // DEBUG
	}

	void preventCollision(const CollisionComponent& c1, const CollisionComponent& c2, TransformComponent& t1, TransformComponent& t2)
	{
		if (c1.collisionShapeType == CollisionComponent::CollisionShapeType::AABB)
		{
			if (c2.collisionShapeType == CollisionComponent::CollisionShapeType::AABB)
			{
				preventAABBxAABBCollision(c1, c2, t1, t2);
			}

			if (c2.collisionShapeType == CollisionComponent::CollisionShapeType::Circle)
			{
				preventAABBxCircleCollision(c1, c2, t1, t2);
			}
		}

		if (c1.collisionShapeType == CollisionComponent::CollisionShapeType::Circle)
		{
			if (c2.collisionShapeType == CollisionComponent::CollisionShapeType::Circle)
			{
				preventCirclexCircleCollision(c1, c2, t1, t2);
			}

			if (c2.collisionShapeType == CollisionComponent::CollisionShapeType::AABB)
			{
				preventAABBxCircleCollision(c2, c1, t2, t1);
			}
		}
	}
}