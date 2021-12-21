#include "Core/CoreLoop.hpp"
#include "Core/Locator.hpp"
#include "Core/InputEvent.hpp"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif // _WIN32

#include <imconfig-SFML.h>
#include <imgui-SFML.h>
#include <imgui.h>
#include <SFML/Window/Event.hpp>

namespace
{
	constexpr float fixedDelta = 1.f / 60.f;
	const sf::Color clearColorDefault = sf::Color(100, 100, 100, 255);

	auto& systemManager = Locator::getInstance().getSystemManager();
	auto& nodeManager = Locator::getInstance().getNode2DManager();
}

CoreLoop::CoreLoop(const sf::Vector2u& windowSize, const std::string& windowTitle, std::shared_ptr<Node2D> root) : _isRunning{ true }
{
#ifdef _WIN32
#ifdef _DEBUG
	::ShowWindow(::GetConsoleWindow(), SW_SHOW);
#else
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);
#endif // _DEBUG
#endif // _WIN32
	_window.create(sf::VideoMode(windowSize.x, windowSize.y), windowTitle, sf::Style::Titlebar | sf::Style::Close);
	_window.setVerticalSyncEnabled(true);
	_window.setActive(false);

	systemManager.setRenderTarget(_window);
	nodeManager.setRoot(root);
	nodeManager.ready();

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

		nodeManager.input(static_cast<InputEvent>(event));
	}

	auto delta = _updateDeltaClock.restart().asSeconds();

	for (_elapsedSeconds += delta; _elapsedSeconds - fixedDelta > std::numeric_limits<float>::epsilon(); _elapsedSeconds -= fixedDelta)
	{
		systemManager.update<PhysicsSystem>(fixedDelta);
	}

	nodeManager.update(delta);
}

std::shared_ptr<Node2D> CoreLoop::getRoot()
{
	return nodeManager.getRoot();
}

void CoreLoop::render()
{
	const auto dt = _renderDeltaClock.restart();

	ImGui::SFML::Update(_window, dt);

	_window.clear(clearColorDefault);

	systemManager.update<RenderSystem>(dt.asSeconds());

	ImGui::SFML::Render(_window);
	_window.display();
}

void CoreLoop::drawUi()
{
	// guess no UI this time, huh?
}

void CoreLoop::updateAi()
{
	// TODO
}