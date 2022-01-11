#pragma once

#include <mutex>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>

class Node2D;

class CoreLoop
{
public:
	CoreLoop(const sf::Vector2u& windowSize, const std::string& windowTitle, std::shared_ptr<Node2D> root);
	~CoreLoop();

	bool isRunning() const { return _isRunning; }
	void update();

	static std::shared_ptr<Node2D> getRoot();

private:
	void render();
	void drawUi();
	void updateAi();

	std::atomic_bool _isRunning = false;
	std::condition_variable _aiThreadExecuteCondition;
	std::thread _renderThread;
	std::thread _aiThread;

	sf::RenderWindow _window;
	sf::Clock _updateDeltaClock;
	sf::Clock _renderDeltaClock;

	std::atomic<float> _tickDeltaTime = 0.f;
	float _frameDeltaTime = 0.f;
	float _elapsedFrameSeconds = 0.f;
	float _elapsedFixedTickSeconds = 0.f;

	bool _displayDebugInfo = false;
};

#define CORE_LOOP(dimensions, name, root) for (CoreLoop loop(dimensions, name, root); loop.isRunning(); loop.update())