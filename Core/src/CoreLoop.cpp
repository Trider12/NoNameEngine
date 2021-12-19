#include "CoreLoop.hpp"

#if defined(_WIN32) && !defined(_DEBUG)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <imconfig-SFML.h>
#include <imgui-SFML.h>
#include <imgui.h>
#include <SFML/Window/Event.hpp>

#include "Locator.hpp"

namespace
{
	constexpr float fixedDelta = 1.f / 60.f;
	const sf::Color clearColorDefault = sf::Color(100, 100, 100, 255);

	auto& systemManager = Locator::getInstance().getSystemManager();
	auto& node2DManager = Locator::getInstance().getNode2DManager();
}

CoreLoop::CoreLoop(const sf::Vector2u& windowSize, const std::string& windowTitle, std::shared_ptr<Node2D> root) : _isRunning{ true }
{
#if defined(_WIN32) && !defined(_DEBUG)
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);
#endif
	_window.create(sf::VideoMode(windowSize.x, windowSize.y), windowTitle, sf::Style::Titlebar | sf::Style::Close);
	_window.setVerticalSyncEnabled(true);
	_window.setActive(false);

	systemManager.setRenderTarget(_window);

	ImGui::SFML::Init(_window);
	ImGui::GetIO().IniFilename = nullptr;

	_aiThread = std::thread([this]() { for (; _isRunning; updateAi()); });
	_renderThread = std::thread([this]() { for (_window.setActive(true); _isRunning || !_window.setActive(false); render()); });
}

CoreLoop::~CoreLoop()
{
	_aiThread.join();
	_renderThread.join();

	_window.close();
	ImGui::SFML::Shutdown();
}

void CoreLoop::update()
{
	for (sf::Event event; _window.pollEvent(event);)
	{
		ImGui::SFML::ProcessEvent(event);

		if (event.type == sf::Event::Closed)
		{
			_isRunning = false;
			return;
		}
	}

	for (_elapsedSeconds += _updateDeltaClock.restart().asSeconds(); _elapsedSeconds - fixedDelta > std::numeric_limits<float>::epsilon(); _elapsedSeconds -= fixedDelta)
	{
		systemManager.update<PhysicsSystem>(fixedDelta);
	}
}

std::shared_ptr<Node2D> CoreLoop::getRoot()
{
	return node2DManager.getRoot();
}

void CoreLoop::render()
{
	ImGui::SFML::Update(_window, _renderDeltaClock.restart());

	_window.clear(clearColorDefault);

	ImGui::SFML::Render(_window);
	_window.display();
}

void CoreLoop::drawUi()
{
}

void CoreLoop::updateAi()
{
}