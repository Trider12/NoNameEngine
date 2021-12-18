#pragma once

#include <mutex>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>

#include "Node2D.hpp"

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

	float _elapsedSeconds = 0.f;
};

#define CORE_LOOP(width, height, name, root) for (CoreLoop loop({width, height}, name, root); loop.isRunning(); loop.update())