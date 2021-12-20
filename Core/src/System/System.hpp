#pragma once

#include <mutex>

class SystemManager;

class System
{
public:
	mutable std::mutex mutex;

protected:
	System(SystemManager& manager) : _systemManager{ manager } {}

	SystemManager& _systemManager;
};

template <typename...> inline constexpr bool always_false = false;

template <typename S>
concept DerivedSystem = requires(S s, float f, uint64_t nodeId)
{
	requires std::is_base_of_v<System, S>;

	s.update(f);
	// idk why it fails
	//s.template addComponent<ComponentType>(nodeId);
	//s.template getComponent<>(nodeId);
};