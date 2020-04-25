#include "HelloBoulderLayer.h"
#include "Amoeba.h"
#include "Boulder.h"
#include "Diamond.h"
#include "Player.h"

#include "Hazel/Core/Application.h"
#include "Hazel/Core/KeyCodes.h"
#include "Hazel/Events/KeyEvent.h"
#include "Hazel/Renderer/RenderCommand.h"
#include "Hazel/Renderer/Renderer2D.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iomanip>
#include <sstream>

#ifdef _DEBUG
#include <imgui.h>
#endif

static std::vector<LevelDefinition> levelDefinition = {
	{ 40, 22, 12, 150,
		"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"
		"WP........r.............r...rr..r......W"
		"Wr.....r..........r.............r...r.dW"
		"W....r..........r..wwwwwwwwwwwwwwwwwwwwW"
		"W..rr........r..w..r.r....r............W"
		"W..rd.rr.rr.....w......r.........r.r..rW"
		"W.rr..r......r..w..rr....rr..r..rw.r..rW"
		"Wwwwwww.........w....r.rr..r.....w.rr..W"
		"W.........rr.r..w.r..............wrr...W"
		"W......r.r.r....wrr.......r.....rw.d.r.W"
		"W.rrr...........wrdr........r....wdd...W"
		"Wr..r.r......r.rw.....rr.rr.r.r..wdd.r.W"
		"Wr.r...w....r...w..rrr....rr...r.w....rW"
		"W....r.w........w......r..rr.r...w.....W"
		"W......w...r..r.w.....r.r......rrw....rW"
		"W..r.r.wrr...rdrw..r..r...r......w...rrW"
		"Wr.....w..r.....w.r....r..rr..r.rw..r.rW"
		"W.rr...w....r..rw...r.wwwwwwwwwwwwwwwwwW"
		"W......wwwwwwwwww.r..r...r...r......r..W"
		"W..rr.....r.rr....d....r.r.r....r....rrW"
		"W.r....r.....r...dd...rrr.r...r.r.....XW"
		"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"
	},
	{ 40, 22, 9, 150,
		"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"
		"WP..........r.....w.......r............W"
		"W.................w....................W"
		"W.................w.w.......w.w........W"
		"W.......w.w.......w.wwwwwwwwwwwwwwwwww.W"
		"Wwwwwwwww.wwwwwwwww.w       ..w        W"
		"W       w.w       w.w       w.w        W"
		"W       w.w       w..       w.r       dW"
		"WFd     r.r       wwwwwwwwwww.w.wwwwwwwW"
		"Wwwwwwwww.wwwwwwwww.w       ..w        W"
		"W       w.w       w.w       w.w        W"
		"W       w.w       w.r       w.wd       W"
		"W d     r.r       w.wwwwwwwww.wwwwwwww.W"
		"Wwwwwwwww.wwwwwwwww.w       w.w        W"
		"W       w.w       w.w       w.w        W"
		"W       w.w       ..r       w.w       dW"
		"W d     r.r       w.wwwwwwwww.w.wwwwwwwW"
		"Wwwwwwwww.wwwwwwwww.w       ..w        W"
		"W       w.w       w.w       w.w        W"
		"W       w.w       w.r       w.wd      .W"
		"W d     r.r      dwwwwwwwwwwwwwwwwwwwwXW"
		"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"
	},
	{ 160, 88, 20, 120,
		"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"
		"WP.............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W........................................................................                             .........................................................W"
		"W........................................................................ ........................... .........................................................W"
		"W........................................................................ .  B                      . .........................................................W"
		"W........................................................................ . ....................... . .........................................................W"
		"W........................................................................ . ....................... . .........................................................W"
		"W........................................................................ . ....................... . .........................................................W"
		"W........................................................................ . ....................... . .........................................................W"
		"W........................................................................ . ....................... . .........................................................W"
		"W............          ..............         ........................... . ....................... . .........................................................W"
		"W............          ..............         ........................... .                         . .........................................................W"
		"W............          ..............         ........................... ........................... .........................................................W"
		"W............F   d     ..............F   d    ...........................                        B    .........................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W............................................................................................................................................................. W"
		"W............................................................................................................................................................. W"
		"W............................................................................................................................................................. W"
		"W............................................................................................................................................................. W"
		"W............................................................................................................................................................. W"
		"W............................................................................................................................................................. W"
		"W....................................................................r........................................................................................ W"
		"W...................................................................rr......................r................................................................. W"
		"W..................................................................rrr.....................rr................................................................. W"
		"W.................................................................rrrr....................rrr................................................................. W"
		"W................................................................rrrrr...................rrrr................................................................. W"
		"W...................rwwwwwwwwwwwwwwwwwwwwwwr.....................rrrr...................rrrrr....................rwwwwwwwwwwwwwwwwwwwwwwr..................... W"
		"W....................w....................w......................rrr...................rrrrrr.....................w....................w...................... W"
		"W....................w....................w......................rr..r.................rrrrrr.....................w....................w...................... W"
		"W....................w....................w......................r..rr.................rrrrrr.....................w....................w...................... W"
		"W....................w....................w........................rrr.................rrrrrr.....................w....................w...................... W"
		"W....................w....................w.......................rrrrrrrrrrrrrrrrrrrrrrrrrrr.....................w....................w...................... W"
		"W....................w....................w......................rrrrrrrrrrrrrrrrrrrrrrrrrrrr.....................w....................w...................... W"
		"W....................w....................w......................rrrrrrrrrrrrrrrrrrrrrrrrrrr......................w....................w...................... W"
		"W....................w....................w......................rrrrr.................rrrr.......................w....................w...................... W"
		"W....................wAA..................w......................rrrrr.................rrr..r.....................w..................AAw...................... W"
		"W...................rwwwwwwwwwwwwwwwwwwwwwwr.....................rrrrr.................rr..rr....................rwwwwwwwwwwwwwwwwwwwwwwr..................... W"
		"W................................................................rrrrr.................r..rrr................................................................. W"
		"W................................................................rrrrr...................rrrr..................................................................W"
		"W...................      F                 .....................rrrr...................rrrrr....................        F               ......................W"
		"W................................................................rrr...................rrrrr...................................................................W"
		"W................................................................rr....................rrrr....................................................................W"
		"W................................................................r.....................rr......................................................................W"
		"W......................................................................................r.......................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W..............................................................................................................................................................W"
		"W.............................................................................................................................................................XW"
		"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"
	},
	{ 40, 22, 10, 0,
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"
	"WWWWWW  WWWWWW  W        WW  WWWW  WWWWW"
	"WWWWWWW  WWWW PWW  WWWW  WW  WWWW  WWWWW"
	"WWWWWWWW  WW  WWW  WWWW  WW  WWWW  WWWWW"
	"WWWWWWWWWW  WWWWW  WWWW  WW  WWWW  WWWWW"
	"WWWWWWWWWW  WWWWW  WWWW  WW  WWWW  WWWWW"
	"WWWWWWWWWW  WWWWW        WWW      WWWWWW"
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"
	"WWWW  WWWWWW  WW      WWW  WWW  WWWWWWWW"
	"WWWW  WWWWWW  WWWW  WWWWW   WW  WWWWWWWW"
	"WWWWW  WWWW  WWWWW  WWWWW    W  WWWWWWWW"
	"WWWWW  W  W  WWWWW  WWWWW  W    WWWWWWWW"
	"WWWWWW      WWWWWW  WWWWW  WW   WWWWWWWW"
	"WWWWWW  WW  WWWW      WWW  WWW  WWWWWWWW"
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"
	}
};

HelloBoulderLayer::HelloBoulderLayer()
: Layer("HelloBoulder")
, m_FixedTimestep(0.125)
, m_ViewPort(0.0f, 0.0f, 20.0f, 11.0f)
, m_TileSize(1.0f, 1.0f)
, m_AccumulatedTs(0.0f)
, m_PlayerIsAlive(false)
, m_GamePaused(false)
, m_CurrentLevel(1)
, m_WonLevel(false)
{
#if 0
	m_CurrentLevel = levelDefinition.size() - 2;
	m_ViewPort = {0.0f, 0.0f, 160.0f, 88.0f};
#endif
	m_ViewPort.SetCameraSpeed((1.0f / m_FixedTimestep) - 1.0f);
}

void HelloBoulderLayer::OnAttach() {
	// Load tiles
	std::string basePath = "assets/textures/tile";
	std::string extension = ".png";

	for (size_t i = 0; i < m_Tiles.size(); ++i) {
		std::ostringstream os;
		os << std::setfill('0') << std::setw(3) << i;
		std::string path = basePath + os.str() + extension;
		m_Tiles[i] = Hazel::Texture2D::Create(path);
	}

	// Load level
	LoadLevel(m_CurrentLevel);
}

void HelloBoulderLayer::OnDetach() {
}

void HelloBoulderLayer::OnUpdate(Hazel::Timestep ts) {

	if (m_GamePaused) {
		return;
	}

	if (m_WonLevel) {
		LoadLevel(++m_CurrentLevel);
	}


	// Update game level on fixed timestep
	m_AccumulatedTs += ts;
	if (m_AccumulatedTs > m_FixedTimestep) {
		m_Level.ClearUpdated();
		for (size_t row = 0; row < m_Level.GetHeight(); ++row) {
			for (size_t col = 0; col < m_Level.GetWidth(); ++col) {
				
				// the "physics" work better if we go from top down.
				size_t flipRow = m_Level.GetHeight() - (row + 1);
				if(!m_Level.IsUpdated(flipRow, col)) {
					m_Level.GetGameObject(flipRow, col).FixedUpdate(flipRow, col, m_Level);
				}
			}
		}
		m_AccumulatedTs = 0.0;
	}

	// Animate
	for (size_t row = 0; row < m_Level.GetHeight(); ++row) {
		for (size_t col = 0; col < m_Level.GetWidth(); ++col) {
			m_Level.GetGameObject(row, col).Animate(ts);
		}
	}

	m_ViewPort.Update(ts);

	Hazel::Renderer2D::ResetStats();
	Hazel::Renderer2D::StatsBeginFrame();

	// Render
	Hazel::RenderCommand::SetClearColor({0.0f, 0.0f, 0.0f, 1});
	Hazel::RenderCommand::Clear();
	Hazel::Renderer2D::BeginScene(m_ViewPort.GetCamera());

	size_t minRow = std::max((size_t)0, (size_t)std::floor(m_ViewPort.GetBottom()));
	size_t maxRow = std::min(m_Level.GetHeight(), (size_t)std::ceil(m_ViewPort.GetTop()));
	size_t minCol = std::max((size_t)0, (size_t)std::floor(m_ViewPort.GetLeft()));
	size_t maxCol = std::min(m_Level.GetWidth(), (size_t)std::ceil(m_ViewPort.GetRight()));

	m_PlayerIsAlive = false;
	for (size_t row = minRow; row < maxRow; ++row) {
		for (size_t col = minCol; col < maxCol; ++col) {
			GameObject& object = m_Level.GetGameObject(row, col);
			//if (!object.IsEmpty()) {
				glm::vec3 position(col - m_ViewPort.GetLeft() + 0.5f, row - m_ViewPort.GetBottom() + 0.5f, 0.0f);
				Hazel::Renderer2D::DrawQuad(position, m_TileSize, m_Tiles[(int)object.GetTile()]);
				if (object.IsPlayer()) {
					m_ViewPort.SetPlayerPosition((float)col, (float)row);
					m_PlayerIsAlive = true;
					if ((row == m_Level.GetExitRow()) && (col == m_Level.GetExitCol())) {
						m_WonLevel = true;
					}
				}
			//}
		}
	}

	Hazel::Renderer2D::EndScene();
	Hazel::Renderer2D::StatsEndFrame();

	auto stats = Hazel::Renderer2D::GetStats();
	float averageRenderTime = stats.TotalFrameRenderTime / stats.FrameRenderTime.size(); // nb: wont be accurate until we have gathered at least stats.FrameRenderTime().size() results
	float averageFPS = 1.0f / averageRenderTime;
	char buffer[64];
	sprintf(buffer, "Average frame render time: %8.5f (%5.0f fps)", averageRenderTime, averageFPS);
	glfwSetWindowTitle((GLFWwindow*)Hazel::Application::Get().GetWindow().GetNativeWindow(), buffer);
}

void HelloBoulderLayer::OnEvent(Hazel::Event& e) {
	Hazel::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Hazel::KeyPressedEvent>(HZ_BIND_EVENT_FN(HelloBoulderLayer::OnKeyPressed));
}


bool HelloBoulderLayer::OnKeyPressed(Hazel::KeyPressedEvent& e) {
	if (e.GetKeyCode() == HZ_KEY_SPACE) {
		if (m_PlayerIsAlive) {
			m_GamePaused = !m_GamePaused;
		} else {
			LoadLevel(m_CurrentLevel);
		}
	} else if (e.GetKeyCode() == HZ_KEY_ESCAPE) {
		LoadLevel(m_CurrentLevel);
	}
	return true;
}


void HelloBoulderLayer::LoadLevel(int level) {
	m_Level.Init(levelDefinition[level]);
	m_WonLevel = false;
	m_ViewPort.SetLevelSize((float)m_Level.GetWidth(), (float)m_Level.GetHeight());
	m_ViewPort.SetPlayerPosition((float)m_Level.GetPlayerInitialCol(), (float)m_Level.GetPlayerInitialRow());
}

#ifdef _DEBUG
void HelloBoulderLayer::OnImGuiRender() {
	{
		ImGui::Begin("Game Settings");
		ImGui::Text("Score: %d", m_Level.GetScore());
		int updateFPS = (int)(1.0f / m_FixedTimestep);
		ImGui::DragInt("Movement FPS", &updateFPS, 1, 1, 60);
		ImGui::End();
		m_FixedTimestep = 1.0f / updateFPS;
		m_ViewPort.SetCameraSpeed((1.0f / m_FixedTimestep) - 1.0f);
	}

	{
		ImGui::Begin("Amoeba Stats");
		ImGui::Text("Amoeba Count: %d", m_Level.GetAmoebaCount());
		ImGui::Text("Amoeba Growth Potential: %d", m_Level.GetAmoebaPotential());
		ImGui::End();
	}

	for (size_t row = 0; row < m_Level.GetHeight(); ++row) {
		for (size_t col = 0; col < m_Level.GetWidth(); ++col) {
			if (m_Level.GetGameObject(row, col).IsPlayer()) {
				m_Level.GetGameObject(row, col).ImGuiRender();
			}
		}
	}

	{
		ImGui::Begin("Statistics");
		auto stats = Hazel::Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
		ImGui::Text("Textures: %d", stats.TextureCount);
		ImGui::Text("Frame count: %d", stats.FrameCount);

		float averageRenderTime = stats.TotalFrameRenderTime / stats.FrameRenderTime.size(); // nb: wont be accurate until we have gathered at least stats.FrameRenderTime().size() results
		float averageFPS = 1.0f / averageRenderTime;
		ImGui::Text("Average frame render time: %8.5f (%5.0f fps)", averageRenderTime, averageFPS);
		ImGui::End();
	}
}
#endif

