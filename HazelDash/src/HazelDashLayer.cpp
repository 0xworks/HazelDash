#include "HazelDashLayer.h"

#include "LevelDefinition.h"
#include "Scripts/CameraController.h"
#include "Scripts/Level.h"

#include "Hazel/Core/Application.h"
#include "Hazel/Core/Input.h"
#include "Hazel/Core/KeyCodes.h"
#include "Hazel/Events/KeyEvent.h"
#include "Hazel/Renderer/RenderCommand.h"
#include "Hazel/Renderer/Renderer2D.h"
#include "Hazel/Scene/Components.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iomanip>
#include <sstream>
#include <unordered_set>

#ifdef _DEBUG
#include <imgui.h>
#endif

// If BATCHRENDER_TEST is non-zero, then starting level is always the
// large one with hazel logo, and will use huge viewport.
// Otherwise set STARTING_LEVEL to the level index (from levelDefinition)
// that you want to start on (and will use normal sized viewport)
// Real levels start from index 5, the ones before that are just small tests
#define BATCHRENDER_TEST 0
#define STARTING_LEVEL 0



HazelDashLayer::HazelDashLayer()
: Layer{"HelloBoulder"}
, m_FixedTimestep{1.0 / 8.0f}        // game logic runs at 8 fps
, m_AnimationTimestep{1.0f / 25.0f}  // animation runs at 25fps
, m_CurrentLevel{STARTING_LEVEL}
, m_GamePaused{false}
{
#if BATCHRENDER_TEST
	m_CurrentLevel = 4;
#endif
}


void HazelDashLayer::OnAttach() {
	std::string basePath = "assets/textures/tile";
	std::string extension = ".png";

	for (size_t i = 0; i < m_Tiles.size(); ++i) {
		std::ostringstream os;
		os << std::setfill('0') << std::setw(3) << i;
		std::string path = basePath + os.str() + extension;
		m_Tiles[i] = Hazel::Texture2D::Create(path);
	}

	Hazel::RenderCommand::SetClearColor({0.0f, 0.0f, 0.0f, 1});

	LoadScene(m_CurrentLevel);
}


void HazelDashLayer::OnDetach() {
}


void HazelDashLayer::OnUpdate(Hazel::Timestep ts) {
	HZ_PROFILE_FUNCTION();
	if (m_GamePaused) {
		return;
	}

	Hazel::Renderer2D::ResetStats();
	Hazel::Renderer2D::StatsBeginFrame();
	Hazel::RenderCommand::Clear();

	m_Scene.OnUpdateRuntime(ts);

	HZ_PROFILE_FRAMEMARKER();

	Hazel::Renderer2D::StatsEndFrame();

	auto stats = Hazel::Renderer2D::GetStats();
	float averageRenderTime = stats.TotalFrameRenderTime / stats.FrameRenderTime.size(); // nb: wont be accurate until we have gathered at least stats.FrameRenderTime().size() results
	float averageFPS = 1.0f / averageRenderTime;
	char buffer[64];
	sprintf_s(buffer, 64, "Average frame render time: %8.5f (%5.0f fps)", averageRenderTime, averageFPS);
	glfwSetWindowTitle((GLFWwindow*)Hazel::Application::Get().GetWindow().GetNativeWindow(), buffer);
	
	if (Level::Get()->HasWonLevel()) {
		LoadScene(++m_CurrentLevel);
	}
}


void HazelDashLayer::OnEvent(Hazel::Event& e) {
	Hazel::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Hazel::KeyPressedEvent>(HZ_BIND_EVENT_FN(HazelDashLayer::OnKeyPressed));
}


bool HazelDashLayer::OnKeyPressed(Hazel::KeyPressedEvent& e) {
	if (Hazel::Input::IsEnabled()) {
		if (e.GetKeyCode() == Hazel::Key::Space) {
			if (Level::Get()->IsPlayerAlive()) {
				m_GamePaused = !m_GamePaused;
			} else {
				LoadScene(m_CurrentLevel);
			}
		} else if (e.GetKeyCode() == Hazel::Key::Escape) {
			LoadScene(m_CurrentLevel);
		}
	}
	return true;
}


void HazelDashLayer::LoadScene(int level) {
	uint32_t viewPortWidth = 20;
	uint32_t viewPortHeight = 11;
#if BATCHRENDER_TEST
	viewPortWidth = 160;
	viewPortHeight = 88;
#endif

	m_Scene.DestroyAllEntities();

	// create and instantiate our singleton "Level" script that will manage the overall level state.
	Hazel::Entity levelEntity = m_Scene.CreateEntity();
	levelEntity
		.AddComponent<Hazel::NativeScriptComponent>()
		.GetComponent<Hazel::NativeScriptComponent>()
		.Bind<Level>(level, m_FixedTimestep, m_AnimationTimestep)
		.Instantiate(levelEntity)
		;

	m_Scene.CreateEntity("Camera")
		.AddComponent<Hazel::CameraComponent>()
		.AddComponent<Hazel::NativeScriptComponent>()
		.GetComponent<Hazel::NativeScriptComponent>()
		.Bind<CameraController>(viewPortWidth, viewPortHeight, (1.0f / m_FixedTimestep) - 1.0f)
		;
}


#ifdef _DEBUG
void HazelDashLayer::OnImGuiRender() {
	static float sz = 36.0f;

	ImGui::Begin("Game Stats");
	ImGui::Text("Score: %d", Level::Get()->GetScore());

	ImGui::Separator();
	ImGui::Text("Key:");
	ImGui::Indent();

	glm::vec4 col = CharToColor('P');
	ImGui::ColorEdit3("Player (move with WASD, or arrow keys)", &col.r, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker);

	col = CharToColor('.');
	ImGui::ColorEdit3("Dirt (dig out by moving player)", &col.r, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker);

	col = CharToColor('w');
	ImGui::ColorEdit3("Wall (cannot be dug out)", &col.r, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker);

	col = CharToColor('d');
	ImGui::ColorEdit3("Collectable", &col.r, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker);

	col = CharToColor('r');
	ImGui::ColorEdit3("Boulder (will fall down and kill things, can be pushed)", &col.r, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker);

	col = CharToColor('B');
	ImGui::ColorEdit3("Butterfly (when killed, turns into collectables)", &col.r, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker);

	col = CharToColor('F');
	ImGui::ColorEdit3("Firefly (will kill player)", &col.r, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker);

	col = CharToColor('A');
	ImGui::ColorEdit3("Amoeba (grows, turns into collectables if fully enclosed)", &col.r, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker);

	col = CharToColor('W');
	ImGui::ColorEdit3("Steel Wall (indestructible)", &col.r, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker);

	ImGui::Unindent();

	ImGui::Separator();
	ImGui::Text("Amoeba:");
	ImGui::Text("Count: %d", Level::Get()->GetAmoebaSize());
	ImGui::Text("Growth Potential: %d", Level::Get()->GetAmoebaPotential());

	ImGui::Separator();
	auto stats = Hazel::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
	ImGui::Text("Textures: %d", stats.TextureCount);
	float averageRenderTime = stats.TotalFrameRenderTime / stats.FrameRenderTime.size(); // nb: wont be accurate until we have gathered at least stats.FrameRenderTime().size() results
	float averageFPS = 1.0f / averageRenderTime;
	ImGui::Text("Average frame render time: %8.5f (%5.0f fps)", averageRenderTime, averageFPS);

	Hazel::Input::Enable(!ImGui::IsWindowFocused());

	ImGui::End();
}
#endif
