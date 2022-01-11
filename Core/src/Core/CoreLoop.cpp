#include "Core/CoreLoop.hpp"
#include "Core/Locator.hpp"
#include "Core/InputEvent.hpp"
#include "Core/Debug.hpp"

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
	Debug::SwitchConsole();

	_window.create(sf::VideoMode(windowSize.x, windowSize.y), windowTitle, sf::Style::Titlebar | sf::Style::Close);
	_window.setVerticalSyncEnabled(true);
	_window.setActive(false);

	nodeManager.setRoot(root);
	nodeManager.ready();

	systemManager.getSystem<RenderSystem>().init(_window);

	ImGui::SFML::Init(_window);
	ImGui::GetIO().IniFilename = nullptr;

	_aiThread = std::thread([this]() { for (; _isRunning; updateAi()); });
	_renderThread = std::thread([this]() { for (_window.setActive(true); _isRunning || !_window.setActive(false); render()); });
}

CoreLoop::~CoreLoop()
{
	_aiThread.join();
	_renderThread.join();

	systemManager.getSystem<RenderSystem>().cleanup();

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

		if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::F1)
		{
			_displayDebugInfo = !_displayDebugInfo;
		}

		nodeManager.input(static_cast<InputEvent>(event));
	}

	_tickDeltaTime = _updateDeltaClock.restart().asSeconds();

	for (_elapsedFixedTickSeconds += _tickDeltaTime; _elapsedFixedTickSeconds - fixedDelta > std::numeric_limits<float>::epsilon(); _elapsedFixedTickSeconds -= fixedDelta)
	{
		systemManager.update<PhysicsSystem>(fixedDelta);
	}

	nodeManager.update(_tickDeltaTime);
}

std::shared_ptr<Node2D> CoreLoop::getRoot()
{
	return nodeManager.getRoot();
}

void CoreLoop::render()
{
	const auto dt = _renderDeltaClock.restart();
	_frameDeltaTime = dt.asSeconds();
	_elapsedFrameSeconds += _frameDeltaTime;

	ImGui::SFML::Update(_window, dt);

	_window.clear(clearColorDefault);

	systemManager.update<RenderSystem>(_frameDeltaTime);

	drawUi();

	ImGui::SFML::Render(_window);

	_window.display();
}

void CoreLoop::drawUi()
{
	if (_displayDebugInfo)
	{
		ImGui::SetNextWindowPos({}, ImGuiCond_Always);
		ImGui::SetNextWindowSize({ 200.f, 100.f });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 10.f, 10.f });
		ImGui::Begin("Counter", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus);

		static auto frameTime = 0.f;
		static auto tickTime = 0.f;
		static bool vSyncCheckboxValue = true;

		if (_elapsedFrameSeconds > 0.5f)
		{
			_elapsedFrameSeconds = 0.f;
			frameTime = 1.f / _frameDeltaTime;
			tickTime = 1.f / _tickDeltaTime;
		}

		ImGui::PushStyleColor(ImGuiCol_Text, { 0.f, 1.f, 0.f, 1.f });
		ImGui::Text("FPS: %.2f", frameTime);
		ImGui::Text("TPS: %.2f", tickTime);

		if (ImGui::Checkbox("VSync", &vSyncCheckboxValue))
		{
			_window.setVerticalSyncEnabled(vSyncCheckboxValue);
		}

		ImGui::PopStyleColor();
		ImGui::End();
		ImGui::PopStyleVar();
	}
}

void CoreLoop::updateAi()
{
	// TODO
}