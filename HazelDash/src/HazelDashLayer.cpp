#include "HazelDashLayer.h"

#include "Components/Amoeba.h"
#include "Components/Animation.h"
#include "Components/Camera.h"
#include "Components/EnemyMovement.h"
#include "Components/Explosion.h"
#include "Components/Mass.h"
#include "Components/PlayerState.h"
#include "Components/Roll.h"

#include "Scripts/CameraController.h"

#include "Random.h"

#include "Hazel/Core/Application.h"
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

struct LevelDefinition {
	int Width;
	int Height;
	int ScoreRequired;
	int TimeAllowed;
	std::string LevelData;
};

static std::vector<LevelDefinition> s_LevelDefinition = {
	{
		10, 10, 1, 0,
		"WWWWWWWWWW"
		"W.......XW"
		"W........W"
		"W........W"
		"W........W"
		"W........W"
		"W........W"
		"W........W"
		"WP......dW"
		"WWWWWWWWWW"
	},
	{
		10, 10, 1, 0,
		"WWWWWWWWWW"
		"W.r...r.XW"
		"W........W"
		"W...    .W"
		"W...    .W"
		"W...    .W"
		"W... B  .W"
		"W........W"
		"WP.......W"
		"WWWWWWWWWW"
	},
	{
		10, 10, 1, 0,
		"WWWWWWWWWW"
		"W.r...r.XW"
		"W........W"
		"W...    .W"
		"W...    .W"
		"W...    .W"
		"W... F  .W"
		"W........W"
		"WP......dW"
		"WWWWWWWWWW"
	},
	{
		40, 22, 9, 150,
		"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"
		"WP.....................................W"
		"W......................................W"
		"W......................................W"
		"W......................................W"
		"W.................r................    W"
		"W....................r.................W"
		"W.......r...........r..................W"
		"W..................r...................W"
		"W....... oooooooooo...........r........W"
		"W....... ..............................W"
		"W....... ..................... ........W"
		"W....... ..................... ........W"
		"W....... ..................... ........W"
		"W.......B..................... ........W"
		"W.......B.....................F........W"
		"W....... ..................... ........W"
		"W......................................W"
		"W......................................W"
		"W......................................W"
		"W.....................................XW"
		"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"
	},
	{
		160, 88, 20, 120,
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
	{
		40, 22, 1, 150,
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
	{
		40, 22, 9, 150,
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
	{
		40, 22, 10, 0,
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


Tile CharToTile(const char ch) {
	static std::unordered_map<char, Tile> tileMap = {
		{'P', Tile::PlayerFirst},
		{'X', Tile::DoorFirst},
		{'A', Tile::AmoebaFirst},
		{'B', Tile::ButterflyFirst},
		{'F', Tile::FireflyFirst},
		{'W', Tile::Metal1},
		{'w', Tile::Brick1},
		{'.', Tile::Dirt1},
		{'r', Tile::BoulderFirst},
		{'d', Tile::DiamondFirst},
		{'o', Tile::BarrelFirst},
		{' ', Tile::Empty}
	};
	std::unordered_map<char, Tile>::const_iterator tile = tileMap.find(ch);
	HZ_ASSERT(tile != tileMap.end(), "ERROR: Unknown character '{}' in level definition", ch);
	return (tile != tileMap.end()) ? tile->second : Tile::Empty;
}


Animation CharToAnimation(const char ch) {
	static std::unordered_map<char, Animation> animationMap = {
		{'P', {{Tile::PlayerIdle0, Tile::PlayerIdle1, Tile::PlayerIdle2, Tile::PlayerIdle3}}},
		{'A', {{Tile::Amoeba0, Tile::Amoeba1, Tile::Amoeba2, Tile::Amoeba3, Tile::Amoeba4, Tile::Amoeba5, Tile::Amoeba6, Tile::Amoeba7}}},
		{'B', {{Tile::Butterfly0, Tile::Butterfly1, Tile::Butterfly2, Tile::Butterfly3}}},
		{'F', {{Tile::Firefly0, Tile::Firefly1, Tile::Firefly2, Tile::Firefly3}}},
		{'d', {{Tile::Diamond0, Tile::Diamond1, Tile::Diamond2, Tile::Diamond3, Tile::Diamond4, Tile::Diamond5, Tile::Diamond6, Tile::Diamond7}}}
	};
	std::unordered_map<char, Animation>::const_iterator animation = animationMap.find(ch);
	if (animation == animationMap.end()) {
		return Animation {};
	} else {
		return animation->second;
	}
}


glm::vec4 CharToColor(const char ch) {
	static std::unordered_map<char, glm::vec4> colorMap = {
		{'P', {1.0f, 1.0f, 1.0f, 1.0f}},
		{'X', {0.8f, 0.8f, 0.8f, 1.0f}},
		{'A', {0.1960f, 0.8039f, 0.1960f, 1.0f}},
		{'B', {0.0f, 1.0f, 1.0f, 1.0f}},
		{'F', {1.0f, 0.0f, 0.0f, 1.0f}},
		{'W', {0.3f, 0.3f, 0.3f, 1.0f}},
		{'w', {0.5f, 0.5f, 0.5f, 1.0f}},
		{'.', {0.8039f, 0.5216f, 0.2470f, 1.0f}},
		{'r', {0.4667f, 0.5333f, 0.6f, 1.0f}},
		{'d', {1.0f, 1.0f, 0.0f, 1.0f}},
		{'o', {0.5450f, 0.2705f, 0.0745f, 1.0f}},
		{' ', {0.0f, 0.0f, 0.0f, 1.0f}}
	};
	std::unordered_map<char, glm::vec4>::const_iterator color = colorMap.find(ch);
	HZ_ASSERT(color != colorMap.end(), "ERROR: Unknown character '{}' in level definition", ch);
	return (color != colorMap.end()) ? color->second : glm::vec4 {};
}


Roll CharToRoll(const char ch) {
	static std::unordered_map<char, Roll> rollMap = {
		{'r', {{Tile::Boulder0, Tile::Boulder1, Tile::Boulder2, Tile::Boulder3}}},
		{'o', {{Tile::Barrel0, Tile::Barrel1, Tile::Barrel2, Tile::Barrel3}}}
	};
	std::unordered_map<char, Roll>::const_iterator roll = rollMap.find(ch);
	if (roll == rollMap.end()) {
		return Roll {};
	} else {
		return roll->second;
	}
}


const Animation& GetPlayerAnimation(PlayerState state, bool lastWasLeft) {
	static std::array<Animation, 6> animations = {
		Animation {{ Tile::PlayerIdle0,  Tile::PlayerIdle0,  Tile::PlayerIdle0,  Tile::PlayerIdle0}},
		Animation {{ Tile::PlayerIdle0, Tile::PlayerIdle1, Tile::PlayerIdle2, Tile::PlayerIdle3}}, // TODO: idle animations
		Animation {{ Tile::PlayerIdle0, Tile::PlayerIdle1, Tile::PlayerIdle2, Tile::PlayerIdle3}}, // TODO: idle animations
		Animation {{ Tile::PlayerIdle0, Tile::PlayerIdle1, Tile::PlayerIdle2, Tile::PlayerIdle3}}, // TODO: idle animations
		Animation {{ Tile::PlayerLeft0, Tile::PlayerLeft1, Tile::PlayerLeft2, Tile::PlayerLeft3}},
		Animation {{ Tile::PlayerRight0, Tile::PlayerRight1, Tile::PlayerRight2, Tile::PlayerRight3}}
	};
	if (state == PlayerState::MovingUp || state == PlayerState::MovingDown) {
		state = lastWasLeft ? PlayerState::MovingLeft : PlayerState::MovingRight;
	}
	return animations[static_cast<int>(state)];
}


PlayerState SetPlayerBlinkState(PlayerState state) {
	switch (state) {
		case PlayerState::Idle:
			state = PlayerState::Blink;
			break;
		case PlayerState::Blink:
			state = PlayerState::Blink;
			break;
		case PlayerState::FootTap:
			state = PlayerState::BlinkFootTap;
			break;
		case PlayerState::BlinkFootTap:
			state = PlayerState::BlinkFootTap;
			break;
	}
	return state;
}


PlayerState ClearPlayerBlinkState(PlayerState state) {
	switch (state) {
		case PlayerState::Idle:
			state = PlayerState::Idle;
			break;
		case PlayerState::Blink:
			state = PlayerState::Idle;
			break;
		case PlayerState::FootTap:
			state = PlayerState::FootTap;
			break;
		case PlayerState::BlinkFootTap:
			state = PlayerState::FootTap;
			break;
	}
	return state;
}


PlayerState SwapPlayerFootTapState(PlayerState state) {
	switch (state) {
		case PlayerState::Idle:
			state = PlayerState::FootTap;
			break;
		case PlayerState::Blink:
			state = PlayerState::BlinkFootTap;
			break;
		case PlayerState::FootTap:
			state = PlayerState::Idle;
			break;
		case PlayerState::BlinkFootTap:
			state = PlayerState::Blink;
			break;
	}
	return state;
}


namespace std {

	template<>
	struct hash<std::pair<int, int>> {
		size_t operator()(const std::pair<int, int>& pr) const {
			std::hash<int> hasher;
			size_t hash = hasher(pr.first);
			hash ^= hasher(pr.second) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
			return hash;
		}
	};

}


HazelDashLayer::HazelDashLayer()
: Layer("HelloBoulder")
, m_FixedTimestep(1.0 / 8.0f)        // game logic runs at 8 fps
, m_AnimationTimestep(1.0f / 25.0f)  // animation runs at 25fps
, m_FixedUpdateAccumulatedTs(0.0f)
, m_AnimatorAccumulatedTs(0.0f)
, m_PushProbability(0.25)
, m_CurrentLevel(STARTING_LEVEL)
, m_Width(0)
, m_Height(0)
, m_Score(0)
, m_ScoreRequired(0)
, m_AmoebaSize(0)
, m_AmoebaPotential(0)
, m_GamePaused(false)
, m_PlayerIsAlive(false)
, m_WonLevel(false)
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

	if (m_WonLevel) {
		LoadScene(++m_CurrentLevel);
	}

	Hazel::Renderer2D::ResetStats();
	Hazel::Renderer2D::StatsBeginFrame();
	Hazel::RenderCommand::Clear();

	m_FixedUpdateAccumulatedTs += ts;
	if (m_FixedUpdateAccumulatedTs > m_FixedTimestep) {
		//
		// Each of these functions is a "system" that operates on a set of game entities.
		// I envisage these will become some sort of scripted game-logic in the future...
		//
		// These systems are updated on a fixed timestep
		PhysicsFixedUpdate();
		PlayerControllerFixedUpdate();
		EnemiesFixedUpdate();
		AmoebaFixedUpdate();
		m_FixedUpdateAccumulatedTs = 0.0;
	}

	// These systems update as fast as they like
	PlayerControllerUpdate(ts);
	ExploderUpdate(ts);
	AnimatorUpdate(ts);
	m_Scene.OnUpdate(ts);

	HZ_PROFILE_FRAMEMARKER();

	Hazel::Renderer2D::StatsEndFrame();

	auto stats = Hazel::Renderer2D::GetStats();
	float averageRenderTime = stats.TotalFrameRenderTime / stats.FrameRenderTime.size(); // nb: wont be accurate until we have gathered at least stats.FrameRenderTime().size() results
	float averageFPS = 1.0f / averageRenderTime;
	char buffer[64];
	sprintf_s(buffer, 64, "Average frame render time: %8.5f (%5.0f fps)", averageRenderTime, averageFPS);
	glfwSetWindowTitle((GLFWwindow*)Hazel::Application::Get().GetWindow().GetNativeWindow(), buffer);
	
}


void HazelDashLayer::OnEvent(Hazel::Event& e) {
	Hazel::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Hazel::KeyPressedEvent>(HZ_BIND_EVENT_FN(HazelDashLayer::OnKeyPressed));
}


bool HazelDashLayer::OnKeyPressed(Hazel::KeyPressedEvent& e) {
	if (e.GetKeyCode() == HZ_KEY_SPACE) {
		if (m_PlayerIsAlive) {
			m_GamePaused = !m_GamePaused;
		} else {
			LoadScene(m_CurrentLevel);
		}
	} else if (e.GetKeyCode() == HZ_KEY_ESCAPE) {
		LoadScene(m_CurrentLevel);
	}
	return true;
}


void HazelDashLayer::LoadScene(int level) {
	const LevelDefinition& definition = s_LevelDefinition[level];
	
	m_Scene.DestroyAllEntities();
	m_Entities.clear();
	m_Width = definition.Width;
	m_Height = definition.Height;
	m_WonLevel = false;

	m_EmptyEntity = m_Scene.CreateEntity();
	m_EmptyEntity.AddComponent<Tile>(Tile::Empty);

	Hazel::Entity playerEntity;
	m_Entities.resize(m_Width * m_Height);
	for (int row = 0; row < m_Height; ++row) {
		for (int col = 0; col < m_Width; ++col) {
			int charIndex = (m_Width * (m_Height - (row + 1))) + col;
			HZ_ASSERT(charIndex < definition.LevelData.size(), "insufficient levelData supplied");
			if (charIndex < definition.LevelData.size()) {
				char ch = definition.LevelData[charIndex];
				Tile tile = CharToTile(ch);
				int index = (m_Width * row) + col;
				auto entity = m_EmptyEntity;
				if (!IsEmpty(tile)) {
					entity = m_Scene.CreateEntity();
					if (IsAmoeba(tile)) {
						entity.AddComponent<Amoeba>();
					} else if (IsBoulder(tile) || IsDiamond(tile)) {
						entity.AddComponent<Mass>();
					} else if (IsBarrel(tile)) {
						entity.AddComponent<Mass>(MassState::Stationary, 0, 5);
					} else if (IsButterfly(tile)) {
						entity.AddComponent<EnemyMovement>(3, false);
					} else if (IsFirefly(tile)) {
						entity.AddComponent<EnemyMovement>(1, true);
					} else if (IsDoor(tile)) {
						m_ExitEntity = entity;
					} else if (IsPlayer(tile)) {
						entity.AddComponent<PlayerState>(PlayerState::Idle);
						playerEntity = entity;
					}
					auto& transform = entity.GetComponent<Hazel::TransformComponent>().Transform;
					transform = glm::translate(glm::identity<glm::mat4>(), {col + 0.5f, row + 0.5f, 0.0f});
					entity.AddComponent<Tile>(tile);
					entity.AddComponent<Hazel::SpriteRendererComponent>(CharToColor(ch)); // TODO: replace with proper "sprite" renderer once we have some sort of asset management
					Animation animation = CharToAnimation(ch);
					if (!animation.Frames.empty()) {
						entity.AddComponent<Animation>(animation);
					}
					Roll roll = CharToRoll(ch);
					if (!roll.Frames.empty()) {
						entity.AddComponent<Roll>(roll);
					}
				}
				m_Entities[index] = entity;
			}
		}
	}
	HZ_ASSERT(playerEntity, "ERROR: level definition does not contain player start point!");

	m_Score = 0;
	m_ScoreRequired = definition.ScoreRequired;

	Hazel::Entity cameraEntity = m_Scene.CreateEntity("Camera");
	auto& cc = cameraEntity.AddComponent<Hazel::CameraComponent>();

	int viewPortWidth = 20;
	int viewPortHeight = 11;
#if BATCHRENDER_TEST
	viewPortWidth = 160;
	viewPortHeight = 88;
#endif
	cc.Camera.SetViewportSize(viewPortWidth, viewPortHeight);  // The magnitude of these numbers doesn't matter, only the ratio between them is important.
	cc.Camera.SetOrthographicSize(static_cast<float>(std::min(m_Height, viewPortHeight)));  // This is what controls the actual size of the viewport.
	cameraEntity.GetComponent<Hazel::TransformComponent>().Transform = glm::translate(glm::identity<glm::mat4>(), {viewPortWidth / 2.0f, viewPortHeight / 2.0f, 0.0f});

	auto& nsc = cameraEntity.AddComponent<Hazel::NativeScriptComponent>();
	nsc.Bind<CameraController>();

	// HACK: instantiate the script component, on cameraEntity
	// HACK: There is currently no way to pass parameters in to the "instance".
	//       This is a bit ugly, but will do for now...
	nsc.InstantiateFunction(cameraEntity);
	auto* instance = static_cast<CameraController*>(nsc.Instance);
	instance->m_TrackEntity = playerEntity;
	instance->m_LevelWidth = static_cast<float>(m_Width);
	instance->m_LevelHeight = static_cast<float>(m_Height);
	instance->m_CameraSpeed = (1.0f / m_FixedTimestep) - 1.0f;
}


void HazelDashLayer::PhysicsFixedUpdate() {
	HZ_PROFILE_FUNCTION();
	// Note: To get the behaviour of the original DB game, the "physics" system must evaluate
	//       the entities in level top-down order.
	//       However, the ECS will not do that.
	//       Instead, entities will just be iterated in whatever order they happen to be stored in the
	//       underlying data structures (and to try and iterate them in any other order kind of defeats
	//       the purpose of the ECS in the first place (which is to iterate in a cache-friendly way))
	//       EnTT does allow for sorting of components (i.e. sort them first, and then iterate them in order)
	//       So that is worth investigating.
	//       However, for now I am just going to ignore it, and iterate the entities in the order that the ECS
	//       has them.

	static const std::pair<int, int> Below = {-1, 0};
	static const std::pair<int, int> Left = {0, -1};
	static const std::pair<int, int> BelowLeft = {-1, -1};
	static const std::pair<int, int> Right = {0, 1};
	static const std::pair<int, int> BelowRight = {-1, 1};

	m_Scene.m_Registry.group<Mass>(entt::get<Hazel::TransformComponent>).each([this] (const auto entityHandle, auto& mass, auto& transformComponent) {
		auto& transform = transformComponent.Transform;
		int row = static_cast<int>(transform[3][1]);
		int col = static_cast<int>(transform[3][0]);
		Hazel::Entity entity(entityHandle, &m_Scene);
		Hazel::Entity entityBelow = GetEntity(row + Below.first, col + Below.second);
		auto tileBelow = entityBelow.GetComponent<Tile>();
		if (IsEmpty(tileBelow)) {
			mass.State = MassState::Falling;
			++mass.HeightFallen;
			SwapEntities(row, col, row + Below.first, col + Below.second);
			transform = glm::translate(transform, {Below.second, Below.first, 0.0f});
		} else {
			if ((mass.State == MassState::Falling) && IsExplosive(tileBelow)) {
				OnExplode(row + Below.first, col + Below.second);
			} else if (mass.HeightFallen > mass.FallLimit) {
				OnExplode(row, col);
			} else {
				if (IsRounded(tileBelow)) {
					Hazel::Entity entityLeft = GetEntity(row + Left.first, col + Left.second);
					Hazel::Entity entityBelowLeft = GetEntity(row + BelowLeft.first, row + BelowLeft.second);
					auto tileLeft = entityLeft.GetComponent<Tile>();
					auto tileBelowLeft = entityBelowLeft.GetComponent<Tile>();
					if (IsEmpty(tileLeft) && IsEmpty(tileBelowLeft)) {
						// bounce left
						mass.State = MassState::Falling;
						SwapEntities(row, col, row + Left.first, col + Left.second);
						transform = glm::translate(transform, {Left.second, Left.first, 0.0f});
						if (entity.HasComponent<Roll>()) {
							auto& roll = entity.GetComponent<Roll>();
							auto& tile = entity.GetComponent<Tile>();
							roll.CurrentFrame = (roll.CurrentFrame - 1) % roll.Frames.size();
							tile = roll.Frames[roll.CurrentFrame];
						}
					} else {
						Hazel::Entity entityRight = GetEntity(row + Right.first, col + Right.second);
						Hazel::Entity entityBelowRight = GetEntity(row + BelowRight.first, col + BelowRight.second);
						auto tileRight = entityRight.GetComponent<Tile>();
						auto tileBelowRight = entityBelowRight.GetComponent<Tile>();
						if (IsEmpty(tileRight) && IsEmpty(tileBelowRight)) {
							// bounce right
							mass.State = MassState::Falling;
							SwapEntities(row, col, row + Right.first, row + Right.second);
							transform = glm::translate(transform, {Right.second, Right.first, 0.0f});
							if (entity.HasComponent<Roll>()) {
								auto& roll = entity.GetComponent<Roll>();
								auto& tile = entity.GetComponent<Tile>();
								roll.CurrentFrame = (roll.CurrentFrame + 1) % roll.Frames.size();
								tile = roll.Frames[roll.CurrentFrame];
							}
						} else {
							mass.State = MassState::Stationary;
							mass.HeightFallen = 0;
						}
					}
				} else {
					mass.State = MassState::Stationary;
					mass.HeightFallen = 0;
				}
			}
		}
	});
}


void HazelDashLayer::PlayerControllerFixedUpdate() {
	HZ_PROFILE_FUNCTION();

	static const std::pair<int, int> Left = {0, -1};
	static const std::pair<int, int> Right = {0, 1};
	static const std::pair<int, int> Up = {1, 0};
	static const std::pair<int, int> Down = {-1, 0};

	static bool lastWasLeft = false; // hack

	m_Scene.m_Registry.group<PlayerState>(entt::get<Hazel::TransformComponent, Animation>).each([this] (auto& state, auto& transformComponent, auto& animation) {
		auto& transform = transformComponent.Transform;
		PlayerState newState = PlayerState::Idle;
		PlayerState secondaryState = PlayerState::Idle;

		if (Hazel::Input::IsKeyPressed(HZ_KEY_LEFT) || Hazel::Input::IsKeyPressed(HZ_KEY_A)) {
			newState = PlayerState::MovingLeft;
			lastWasLeft = true;
		} else if (Hazel::Input::IsKeyPressed(HZ_KEY_RIGHT) || Hazel::Input::IsKeyPressed(HZ_KEY_D)) {
			newState = PlayerState::MovingRight;
			lastWasLeft = false;
		}

		if (Hazel::Input::IsKeyPressed(HZ_KEY_UP) || Hazel::Input::IsKeyPressed(HZ_KEY_W)) {
			secondaryState = newState;
			newState = PlayerState::MovingUp;
		} else if (Hazel::Input::IsKeyPressed(HZ_KEY_DOWN) || Hazel::Input::IsKeyPressed(HZ_KEY_S)) {
			secondaryState = newState;
			newState = PlayerState::MovingDown;
		}

		if (IsIdle(state)) {
			if (!IsIdle(newState)) {
				state = newState;
				animation = GetPlayerAnimation(state, lastWasLeft);
			}
		} else {
			if (state != newState) {
				state = newState;
				animation = GetPlayerAnimation(state, lastWasLeft);
			}
		}

		bool ctrlPressed = Hazel::Input::IsKeyPressed(HZ_KEY_LEFT_CONTROL) || Hazel::Input::IsKeyPressed(HZ_KEY_RIGHT_CONTROL);
		int oldRow = static_cast<int>(transform[3][1]);
		int oldCol = static_cast<int>(transform[3][0]);
		switch (state) {
			case PlayerState::MovingLeft:
				TryMovePlayer(transform, Left, ctrlPressed);
				break;
			case PlayerState::MovingRight:
				TryMovePlayer(transform, Right, ctrlPressed);
				break;
			case PlayerState::MovingUp:
				if (!TryMovePlayer(transform, Up, ctrlPressed)) {
					if (secondaryState == PlayerState::MovingLeft) {
						TryMovePlayer(transform, Left, ctrlPressed);
					} else if (secondaryState == PlayerState::MovingRight) {
						TryMovePlayer(transform, Right, ctrlPressed);
					}
				}
				break;
			case PlayerState::MovingDown:
				if (!TryMovePlayer(transform, Down, ctrlPressed)) {
					if (secondaryState == PlayerState::MovingLeft) {
						TryMovePlayer(transform, Left, ctrlPressed);
					} else if (secondaryState == PlayerState::MovingRight) {
						TryMovePlayer(transform, Right, ctrlPressed);
					}
				}
				break;
		}
		int row = static_cast<int>(transform[3][1]);
		int col = static_cast<int>(transform[3][0]);
		if ((row != oldRow) || (col != oldCol)) {
			Hazel::Entity entityAtNewPos = GetEntity(row, col);
			auto tile = entityAtNewPos.GetComponent<Tile>();
			SwapEntities(oldRow, oldCol, row, col);
			ClearEntity(oldRow, oldCol);
			if (IsDoor(tile)) {
				OnLevelCompleted();
			}
			if (IsCollectable(tile)) {
				OnIncreaseScore();
			}
		}
	});
}


void HazelDashLayer::PlayerControllerUpdate(Hazel::Timestep ts) {
	HZ_PROFILE_FUNCTION();
	m_Scene.m_Registry.group<PlayerState>(entt::get<Animation>).each([this] (auto& state, auto& animation) {
		if (animation.CurrentFrame == (animation.Frames.size() - 1)) {
			if (IsIdle(state)) {
				PlayerState newState = state;
				if (Random::Uniform0_1() < 0.25f) {
					newState = SetPlayerBlinkState(state);
				} else {
					newState = ClearPlayerBlinkState(state);
				}
				if (Random::Uniform0_1() < 1.0f / 16.0f) {
					newState = SwapPlayerFootTapState(state);
				}
				if (state != newState) {
					state = newState;
					animation = GetPlayerAnimation(state, false);
				}
			}
		}
	});
}


bool HazelDashLayer::TryMovePlayer(glm::mat4& transform, const std::pair<int, int> direction, const bool ctrlPressed) {
	bool retVal = false;
	int row = static_cast<int>(transform[3][1]);
	int col = static_cast<int>(transform[3][0]);
	Hazel::Entity entity = GetEntity(row + direction.first, col + direction.second);
	auto& tile = entity.GetComponent<Tile>();
	if (CanBeOccupied(tile)) {
		retVal = true;
		if (!ctrlPressed) {
			transform = glm::translate(transform, {direction.second, direction.first, 0.0f});
		}
	} else if ((direction.first == 0) && IsPushable(tile)) {
		retVal = true;
		if (Random::Uniform0_1() < m_PushProbability) {
			int rowBelow = row - 1;
			int colBelow = col + direction.second;
			int rowAcross = row;
			int colAcross = col + (2 * direction.second);
			Hazel::Entity entityBelow = GetEntity(rowBelow, colBelow);
			Hazel::Entity entityAcross = GetEntity(rowAcross, colAcross);
			const auto tileBelow = entityBelow.GetComponent<Tile>();
			const auto tileAcross = entityAcross.GetComponent<Tile>();
			if (!IsEmpty(tileBelow) && IsEmpty(tileAcross)) {
				SwapEntities(rowAcross, colAcross, row + direction.first, col + direction.second);
				auto& transformPushed = entity.GetComponent<Hazel::TransformComponent>();
				transformPushed.Transform = glm::translate(transformPushed.Transform, {direction.second, direction.first, 0.0f});
				if (entity.HasComponent<Roll>()) {
					auto& roll = entity.GetComponent<Roll>();
					roll.CurrentFrame = (roll.CurrentFrame + direction.second) % roll.Frames.size();
					tile = roll.Frames[roll.CurrentFrame];
				}
				if (!ctrlPressed) {
					transform = glm::translate(transform, {direction.second, direction.first, 0.0f});
				}
			}
		}
	}
	return retVal;
}


void HazelDashLayer::OnPlayerDied() {
	// TODO: placeholder code.  Should be done with "proper" events at some point
	m_PlayerIsAlive = false;
}


void HazelDashLayer::OnLevelCompleted() {
	// TODO: placeholder code.  Should be done with "proper" events at some point
	m_WonLevel = true;
}


void HazelDashLayer::OnIncreaseScore() {
	// TODO: placeholder code.  Should be done with "proper" events at some point
	++m_Score;
	if (m_Score == m_ScoreRequired) {
		Animation animation = {{Tile::Door0, Tile::Door1, Tile::Door2, Tile::Door3}, 0, false};
		m_ExitEntity.AddComponent<Animation>(animation);
	}
}


void HazelDashLayer::EnemiesFixedUpdate() {
	HZ_PROFILE_FUNCTION();
	
	static std::array<std::pair<int, int>, 4> Directions {
		std::pair<int, int>{-1,  0},
		std::pair<int, int>{ 0, -1},
		std::pair<int, int>{ 1,  0},
		std::pair<int, int>{ 0,  1}
	};

	m_Scene.m_Registry.group<EnemyMovement>(entt::get<Hazel::TransformComponent>).each([this] (auto& movement, auto& transformComponent) {
		auto& transform = transformComponent.Transform;
		int row = static_cast<int>(transform[3][1]);
		int col = static_cast<int>(transform[3][0]);

		// If next to player, then explode (and do not move)
		bool move = true;
		for (auto direction : Directions) {
			if (IsPlayer(GetEntity(row + direction.first, col + direction.second).GetComponent<Tile>())) {
				OnExplode(row, col);
				move = false;
				break;
			}
		}
		if (move) {
			// try to turn in preferred direction
			// if that is not possible, go straight ahead
			// if that is not possible either, then don't move, but change to opposite direction for next frame
			int direction = (movement.Direction + (movement.PreferLeftTurn ? -1 : 1)) % Directions.size();
			int preferredRow = row + Directions[direction].first;
			int preferredCol = col + Directions[direction].second;
			if (IsEmpty(GetEntity(preferredRow, preferredCol).GetComponent<Tile>())) {
				SwapEntities(row, col, preferredRow, preferredCol);
				transform = glm::translate(transform, {Directions[direction].second, Directions[direction].first, 0.0f});
				movement.Direction = direction;
			} else {
				int straightRow = row + Directions[movement.Direction].first;
				int straightCol = col + Directions[movement.Direction].second;
				if (IsEmpty(GetEntity(straightRow, straightCol).GetComponent<Tile>())) {
					SwapEntities(row, col, straightRow, straightCol);
					transform = glm::translate(transform, {Directions[movement.Direction].second, Directions[movement.Direction].first, 0.0f});
				} else {
					movement.Direction = (movement.Direction + 2) % Directions.size();
				}
			}
		}
	});
}


void HazelDashLayer::OnExplode(const int row, const int col) {
	HZ_PROFILE_FUNCTION();

	// TODO: placeholder code.  Should be done with "proper" events at some point

	static std::array<std::pair<int, int>, 9> Offsets = {
		std::pair{ 1, -1},
		std::pair{ 1,  0},
		std::pair{ 1,  1},
		std::pair{ 0, -1},
		std::pair{ 0,  0},
		std::pair{ 0,  1},
		std::pair{-1, -1},
		std::pair{-1,  0},
		std::pair{-1,  1}
	};

	static Animation animation1 = {{Tile::Explosion0, Tile::Explosion1, Tile::Explosion2, Tile::Explosion3, Tile::Explosion4, Tile::Explosion5, Tile::Explosion6, Tile::Explosion7}};
	static Animation animation2 = {{Tile::Explosion0, Tile::Explosion1, Tile::Explosion2, Tile::Explosion3,  Tile::ExplosionDiamond4, Tile::ExplosionDiamond5, Tile::ExplosionDiamond6, Tile::Diamond7}};

	auto tile = GetEntity(row, col).GetComponent<Tile>();
	if (IsPlayer(tile)) {
		OnPlayerDied();
	}
	bool explodeToDiamond = IsButterfly(tile);
	//
	// At this point, other systems are still iterating their entities,
	// so we can't go destroying anything just yet.
	// What we'll do here is create an explosion entities at appropriate
	// positions, then when the exploder system gets its Update, we will
	// wreak the destruction there.
	for (auto offset : Offsets) {
		if (!IsExplodable(GetEntity(row + offset.first, col + offset.second).GetComponent<Tile>())) {
			continue;
		}
		Hazel::Entity entity = m_Scene.CreateEntity();
		entity.GetComponent<Hazel::TransformComponent>().Transform = glm::translate(glm::identity<glm::mat4>(), {row + offset.first + 0.5f, col + offset.second + 0.5f, 0.0f});
		entity.AddComponent<Explosion>(Explosion::Ignite);
		if (explodeToDiamond) {
			entity.AddComponent<Animation>(animation2);
			entity.AddComponent<Tile>(animation2.Frames[0]);
		} else {
			entity.AddComponent<Animation>(animation1);
			entity.AddComponent<Tile>(animation1.Frames[0]);
		}
	}
}


void HazelDashLayer::AmoebaFixedUpdate() {
	HZ_PROFILE_FUNCTION();

	static const std::array<std::pair<int, int>, 4> Directions = {
		std::pair{-1,  0},
		std::pair{ 0, -1},
		std::pair{ 1,  0},
		std::pair{ 0,  1}
	};

	auto amoebas = m_Scene.m_Registry.group<Amoeba>(entt::get<Hazel::TransformComponent>);

	m_AmoebaSize = static_cast<int>(amoebas.size());
	if (m_AmoebaSize >= 200) { // TODO: parameterize?
		OnSolidify(Tile::BoulderFirst);
	}

	m_AmoebaPotential = 0;
	std::unordered_set<std::pair<int,int>> growPositions;

	amoebas.each([&] (auto& amoeba, auto& transformComponent) {
		auto& transform = transformComponent.Transform;
		for (auto direction : Directions) {
			int row = static_cast<int>(transform[3][1]);
			int col = static_cast<int>(transform[3][0]);
			Hazel::Entity entityOther = GetEntity(row + direction.first, col + direction.second);
			auto tile = entityOther.GetComponent<Tile>();
			if (IsEmpty(tile) || tile == Tile::Dirt1) {
				++m_AmoebaPotential;
				if (Random::Uniform0_1() < amoeba.GrowthProbability) {
					growPositions.emplace(row + direction.first, col + direction.second);
				}
			} else if (IsExplosive(tile)) {
				OnExplode(row + direction.first, col + direction.second);
			}
			amoeba.GrowthProbability *= 1.0f + static_cast<float>(amoebas.size()) / 200000.0f;
		}
	});

	if (m_AmoebaPotential == 0) {
		OnSolidify(Tile::Diamond0);
	} else {
		for (auto pos : growPositions) {
			Hazel::Entity entity = GetEntity(pos.first, pos.second);
			auto& tileInitial = entity.GetComponent<Tile>();
			if (IsEmpty(tileInitial)) {
				entity = m_Scene.CreateEntity();
				entity.AddComponent<Tile>(tileInitial);
				entity.GetComponent<Hazel::TransformComponent>().Transform = glm::translate(glm::identity<glm::mat4>(), {pos.second + 0.5f, pos.first + 0.5f, 0.0f});
				SetEntity(pos.first, pos.second, entity);
			}
			entity.AddComponent<Amoeba>();
			const Animation& animation = CharToAnimation('A');
			entity.AddComponent<Animation>(animation);
			auto& tile = entity.GetComponent<Tile>();
			tile = animation.Frames[animation.CurrentFrame]; // TODO: it would be nicer to use EnTT "short circuit" to automatically set the tile when Animation component is added
		}
	}
}


void HazelDashLayer::OnSolidify(const Tile solidifyTo) {
	m_Scene.m_Registry.view<Amoeba, Tile>().each([&] (const auto entityHandle, auto& amoeba, auto& tile) {
		Hazel::Entity entity(entityHandle, &m_Scene);
		entity.RemoveComponent<Amoeba>();
		tile = solidifyTo;
		if (IsDiamond(tile)) {
			auto& animation = entity.GetComponent<Animation>(); // we know it has an Animation component because it was an Amoeba, and Amoeba entities have an Animation
			animation = CharToAnimation('d');
		} else {
			entity.RemoveComponent<Animation>();
		}
	});
}


void HazelDashLayer::ExploderUpdate(Hazel::Timestep ts) {
	HZ_PROFILE_FUNCTION();

	// When we get here, other systems are finished iterating.
	// It is now safe to destroy the game entities at position of explosion entities
	m_Scene.m_Registry.group<Explosion>(entt::get<Hazel::TransformComponent, Animation, Tile>).each([this] (const auto entityHandle, auto& explosion, auto& transformComponent, auto& animation, auto& tile) {
		auto& transform = transformComponent.Transform;
		int row = static_cast<int>(transform[3][1]);
		int col = static_cast<int>(transform[3][0]);
		if (explosion == Explosion::Ignite) {
			ClearEntity(row, col);
			SetEntity(row, col, {entityHandle, &m_Scene});
			explosion = Explosion::Burn;
		} else {
			if (animation.CurrentFrame == animation.Frames.size() - 1) {
				if (IsDiamond(animation.Frames.back())) {
					// turn into a diamond
					Hazel::Entity entity(entityHandle, &m_Scene);
					entity.RemoveComponent<Explosion>();
					entity.AddComponent<Mass>();
					animation = CharToAnimation('d');
				} else {
					//HZ_ASSERT(Hazel::Entity(entityHandle, &m_Scene) == GetEntity(row, col), "Something has misplaced an explosion - game logic error!");
					ClearEntity(row, col);
				}
			}
		}
	});
}


void HazelDashLayer::AnimatorUpdate(Hazel::Timestep ts) {
	HZ_PROFILE_FUNCTION();

	m_AnimatorAccumulatedTs += ts;
	if (m_AnimatorAccumulatedTs > m_AnimationTimestep) {
		m_AnimatorAccumulatedTs = 0.0f;
		m_Scene.m_Registry.group<Animation>(entt::get<Tile>).each([this] (const auto entityHandle, auto& animation, auto& tile) {
			if (++animation.CurrentFrame >= animation.Frames.size()) {
				if (animation.Repeat) {
					animation.CurrentFrame = 0;
				} else {
					Hazel::Entity entity(entityHandle, &m_Scene);
					entity.RemoveComponent<Animation>();
					return;
				}
			}
			tile = animation.Frames[animation.CurrentFrame];
		});
	}
}


Hazel::Entity HazelDashLayer::GetEntity(const int row, const int col) {
	return m_Entities[(m_Width * row) + col];
}


void HazelDashLayer::SetEntity(const int row, const int col, Hazel::Entity entity) {
	m_Entities[(m_Width * row) + col] = entity;
}


void HazelDashLayer::ClearEntity(const int row, const int col) {
	int index = (m_Width * row) + col;
	if (m_Entities[index] != m_EmptyEntity) {
		m_Scene.DestroyEntity(m_Entities[index]);
		m_Entities[index] = m_EmptyEntity;
	}
}


void HazelDashLayer::SwapEntities(const int rowA, const int colA, const int rowB, const int colB) {
	std::swap(m_Entities[(m_Width * rowA) + colA], m_Entities[(m_Width * rowB) + colB]);
}


#ifdef _DEBUG
void HazelDashLayer::OnImGuiRender() {
	ImGui::Begin("Game Stats");
	ImGui::Text("Score: %d", m_Score);

	ImGui::Separator();
	ImGui::Text("Amoeba:");
	ImGui::Text("Count: %d", m_AmoebaSize);
	ImGui::Text("Growth Potential: %d", m_AmoebaPotential);

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

	ImGui::End();
}
#endif
