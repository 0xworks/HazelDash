#include "HazelDashLayer.h"
#include "HazelDashAudio.h"

#include "Components/Amoeba.h"
#include "Components/Animation.h"
#include "Components/Camera.h"
#include "Components/EnemyMovement.h"
#include "Components/Explosion.h"
#include "Components/Mass.h"
#include "Components/PlayerState.h"
#include "Components/Roll.h"

#include "Random.h"

#include "Hazel/Core/Application.h"
#include "Hazel/Core/KeyCodes.h"
#include "Hazel/Events/KeyEvent.h"
#include "Hazel/Renderer/RenderCommand.h"
#include "Hazel/Renderer/Renderer2D.h"
#include "Hazel/Scene/Components.h"

#include "HazelAudio.h"

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


SoundEffect GetSoundEffect(Tile tile) {
	return IsBoulder(tile)? SoundEffect::Boulder : SoundEffect::Diamond;
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


HazelDashLayer::HazelDashLayer()
: Layer("HelloBoulder")
, m_ViewPort(0.0f, 0.0f, 20.0f, 11.0f)
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
	m_ViewPort = {0.0f, 0.0f, 160.0f, 88.0f};
#endif
	m_ViewPort.SetCameraSpeed((1.0f / m_FixedTimestep) - 1.0f);
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
	HazelDashAudio::Init();

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
	CameraControllerUpdate(ts);
	RendererUpdate(ts);

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

	Position playerPosition;
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
					} else if (IsBoulder(tile) || IsBarrel(tile) || IsDiamond(tile)) {
						entity.AddComponent<Mass>(Mass::Stationary);
					} else if (IsButterfly(tile)) {
						entity.AddComponent<EnemyMovement>(3, false);
					} else if (IsFirefly(tile)) {
						entity.AddComponent<EnemyMovement>(1, true);
					} else if (IsDoor(tile)) {
						m_ExitEntity = entity;
					} else if (IsPlayer(tile)) {
						entity.AddComponent<PlayerState>(PlayerState::Idle);
						playerPosition = {row, col};
					}
					entity.AddComponent<Position>(row, col);
					entity.AddComponent<Tile>(tile);
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
	HZ_ASSERT((playerPosition.Row > 0) && (playerPosition.Col >= 0), "ERROR: ({},{}) is not a legal starting position for player (check that level definition contains player start point)", playerPosition.Row, playerPosition.Col);

	m_Score = 0;
	m_ScoreRequired = definition.ScoreRequired;

	m_ViewPort.SetLevelSize(static_cast<float>(m_Width), static_cast<float>(m_Height));
	m_ViewPort.SetPlayerPosition(static_cast<float>(playerPosition.Col), static_cast<float>(playerPosition.Row));

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

	static const Position Below = {-1, 0};
	static const Position Left = {0, -1};
	static const Position BelowLeft = {-1, -1};
	static const Position Right = {0, 1};
	static const Position BelowRight = {-1, 1};

	m_Scene.m_Registry.group<Mass>(entt::get<Position>).each([this] (const auto entityHandle, auto& mass, auto& pos) {
		Hazel::Entity entity(entityHandle, &m_Scene);
		Hazel::Entity entityBelow = GetEntity(pos + Below);
		auto tileBelow = entityBelow.GetComponent<Tile>();
		if (IsEmpty(tileBelow)) {
			mass = Mass::Falling;
			SwapEntities(pos, pos + Below);
			pos += Below;
		} else {
			if ((mass == Mass::Falling) && IsExplosive(tileBelow)) {
				OnExplode(pos + Below);
			} else {
				if (mass == Mass::Falling) {
					HazelDashAudio::PlaySound(GetSoundEffect(tile));
				}
				if (IsRounded(tileBelow)) {
					Hazel::Entity entityLeft = GetEntity(pos + Left);
					Hazel::Entity entityBelowLeft = GetEntity(pos + BelowLeft);
					auto tileLeft = entityLeft.GetComponent<Tile>();
					auto tileBelowLeft = entityBelowLeft.GetComponent<Tile>();
					if (IsEmpty(tileLeft) && IsEmpty(tileBelowLeft)) {
						// bounce left
						mass = Mass::Falling;
						SwapEntities(pos, pos + Left);
						pos += Left;
						if (entity.HasComponent<Roll>()) {
							auto& roll = entity.GetComponent<Roll>();
							auto& tile = entity.GetComponent<Tile>();
							roll.CurrentFrame = (roll.CurrentFrame - 1) % roll.Frames.size();
							tile = roll.Frames[roll.CurrentFrame];
						}
					} else {
						Hazel::Entity entityRight = GetEntity(pos + Right);
						Hazel::Entity entityBelowRight = GetEntity(pos + BelowRight);
						auto tileRight = entityRight.GetComponent<Tile>();
						auto tileBelowRight = entityBelowRight.GetComponent<Tile>();
						if (IsEmpty(tileRight) && IsEmpty(tileBelowRight)) {
							// bounce right
							mass = Mass::Falling;
							SwapEntities(pos, pos + Right);
							pos += Right;
							if (entity.HasComponent<Roll>()) {
								auto& roll = entity.GetComponent<Roll>();
								auto& tile = entity.GetComponent<Tile>();
								roll.CurrentFrame = (roll.CurrentFrame + 1) % roll.Frames.size();
								tile = roll.Frames[roll.CurrentFrame];
							}
						} else {
							mass = Mass::Stationary;
						}
					}
				} else {
					mass = Mass::Stationary;
				}
			}
		}
	});
}


void HazelDashLayer::PlayerControllerFixedUpdate() {
	HZ_PROFILE_FUNCTION();

	static const Position Left = {0, -1};
	static const Position Right = {0, 1};
	static const Position Up = {1, 0};
	static const Position Down = {-1, 0};

	static bool lastWasLeft = false; // hack

	m_Scene.m_Registry.group<PlayerState>(entt::get<Position, Animation>).each([this] (auto& state, auto& pos, auto& animation) {
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
		Position oldPos = pos;
		switch (state) {
			case PlayerState::MovingLeft:
				TryMovePlayer(pos, Left, ctrlPressed);
				break;
			case PlayerState::MovingRight:
				TryMovePlayer(pos, Right, ctrlPressed);
				break;
			case PlayerState::MovingUp:
				if (!TryMovePlayer(pos, Up, ctrlPressed)) {
					if (secondaryState == PlayerState::MovingLeft) {
						TryMovePlayer(pos, Left, ctrlPressed);
					} else if (secondaryState == PlayerState::MovingRight) {
						TryMovePlayer(pos, Right, ctrlPressed);
					}
				}
				break;
			case PlayerState::MovingDown:
				if (!TryMovePlayer(pos, Down, ctrlPressed)) {
					if (secondaryState == PlayerState::MovingLeft) {
						TryMovePlayer(pos, Left, ctrlPressed);
					} else if (secondaryState == PlayerState::MovingRight) {
						TryMovePlayer(pos, Right, ctrlPressed);
					}
				}
				break;
		}
		if (oldPos != pos) {
			Hazel::Entity entityAtNewPos = GetEntity(pos);
			auto tile = entityAtNewPos.GetComponent<Tile>();
			SwapEntities(oldPos, pos);
			ClearEntity(oldPos);
			OnPlayerMoved(pos);
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
	m_Scene.m_Registry.group<PlayerState>(entt::get<Position, Animation>).each([this] (auto& state, auto& pos, auto& animation) {
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


bool HazelDashLayer::TryMovePlayer(Position& pos, Position direction, const bool ctrlPressed) {
	bool retVal = false;
	Hazel::Entity entity = GetEntity(pos + direction);
	auto& tile = entity.GetComponent<Tile>();
	if (CanBeOccupied(tile)) {
		retVal = true;
		if (!ctrlPressed) {
			if (IsEmpty(tile)) {
				HazelDashAudio::PlaySound(SoundEffect::Movement1);
			} else {
				HazelDashAudio::PlaySound(SoundEffect::Movement2);
			}
			pos += direction;
		}
	} else if ((direction.Row == 0) && IsPushable(tile)) {
		retVal = true;
		if (Random::Uniform0_1() < m_PushProbability) {
			Position posBelow = {pos.Row - 1, pos.Col + direction.Col};
			Position posAcross = {pos.Row, pos.Col + (2 * direction.Col)};
			Hazel::Entity entityBelow = GetEntity(posBelow);
			Hazel::Entity entityAcross = GetEntity(posAcross);
			const auto tileBelow = entityBelow.GetComponent<Tile>();
			const auto tileAcross = entityAcross.GetComponent<Tile>();
			if (!IsEmpty(tileBelow) && IsEmpty(tileAcross)) {
				SwapEntities(posAcross, pos + direction);
				auto& posPushed = entity.GetComponent<Position>();
				posPushed += direction;
				HazelDashAudio::PlaySound(SoundEffect::Boulder);
				if (entity.HasComponent<Roll>()) {
					auto& roll = entity.GetComponent<Roll>();
					roll.CurrentFrame = (roll.CurrentFrame + direction.Col) % roll.Frames.size();
					tile = roll.Frames[roll.CurrentFrame];
				}
				if (!ctrlPressed) {
					pos += direction;
				}
			}
		}
	}
	return retVal;
}


void HazelDashLayer::OnPlayerMoved(const Position& pos) {
	// TODO: placeholder code.  Should be done with "proper" events at some point
	m_ViewPort.SetPlayerPosition(static_cast<float>(pos.Col), static_cast<float>(pos.Row));
}


void HazelDashLayer::OnPlayerDied() {
	// TODO: placeholder code.  Should be done with "proper" events at some point
	HazelDashAudio::PlaySound(SoundEffect::PlayerDie);
	m_PlayerIsAlive = false;
}


void HazelDashLayer::OnLevelCompleted() {
	// TODO: placeholder code.  Should be done with "proper" events at some point
	m_WonLevel = true;
}


void HazelDashLayer::OnIncreaseScore() {
	// TODO: placeholder code.  Should be done with "proper" events at some point
	HazelDashAudio::PlaySound(SoundEffect::Collect);
	++m_Score;
	if (m_Score == m_ScoreRequired) {
		Animation animation = {{Tile::Door0, Tile::Door1, Tile::Door2, Tile::Door3}, 0, false};
		m_ExitEntity.AddComponent<Animation>(animation);
	}
}


void HazelDashLayer::EnemiesFixedUpdate() {
	HZ_PROFILE_FUNCTION();
	
	static std::array<Position, 4> Directions {
		Position{-1,  0},
		Position{ 0, -1},
		Position{ 1,  0},
		Position{ 0,  1}
	};

	m_Scene.m_Registry.group<EnemyMovement>(entt::get<Position>).each([this] (auto& movement, auto& pos) {

		// If next to player, then explode (and do not move)
		bool move = true;
		for (auto direction : Directions) {
			if (IsPlayer(GetEntity(pos + direction).GetComponent<Tile>())) {
				OnExplode(pos);
				move = false;
				break;
			}
		}
		if (move) {
			// try to turn in preferred direction
			// if that is not possible, go straight ahead
			// if that is not possible either, then don't move, but change to opposite direction for next frame
			int direction = (movement.Direction + (movement.PreferLeftTurn ? -1 : 1)) % Directions.size();
			Position preferredPos = pos + Directions[direction];
			if (IsEmpty(GetEntity(preferredPos).GetComponent<Tile>())) {
				SwapEntities(pos, preferredPos);
				pos = preferredPos;
				movement.Direction = direction;
			} else {
				Position straightPos = pos + Directions[movement.Direction];
				if (IsEmpty(GetEntity(straightPos).GetComponent<Tile>())) {
					SwapEntities(pos, straightPos);
					pos = straightPos;
				} else {
					movement.Direction = (movement.Direction + 2) % Directions.size();
				}
			}
		}
	});
}


void HazelDashLayer::OnExplode(const Position& pos) {
	HZ_PROFILE_FUNCTION();

	// TODO: placeholder code.  Should be done with "proper" events at some point

	static std::array<Position, 9> Offsets = {
		Position{1,-1},
		Position{1,0},
		Position{1,1},
		Position{0,-1},
		Position{0,0},
		Position{0,1},
		Position{-1,-1},
		Position{-1,0},
		Position{-1,1}
	};

	static Animation animation1 = {{Tile::Explosion0, Tile::Explosion1, Tile::Explosion2, Tile::Explosion3, Tile::Explosion4, Tile::Explosion5, Tile::Explosion6, Tile::Explosion7}};
	static Animation animation2 = {{Tile::Explosion0, Tile::Explosion1, Tile::Explosion2, Tile::Explosion3,  Tile::ExplosionDiamond4, Tile::ExplosionDiamond5, Tile::ExplosionDiamond6, Tile::Diamond7}};

	auto tile = GetEntity(pos).GetComponent<Tile>();
	bool explodeToDiamond = false;

	if (IsPlayer(tile)) {
		OnPlayerDied();
	} else if(IsButterfly(tile)) {
		HazelDashAudio::PlaySound(SoundEffect::Explode2);
		explodeToDiamond = true;
	} else {
		HazelDashAudio::PlaySound(SoundEffect::Explode1);
	}
	//
	// At this point, other systems are still iterating their entities,
	// so we can't go destroying anything just yet.
	// What we'll do here is create an explosion entities at appropriate
	// positions, then when the exploder system gets its Update, we will
	// wreak the destruction there.
	for (auto offset : Offsets) {
		if (!IsExplodable(GetEntity(pos + offset).GetComponent<Tile>())) {
			continue;
		}
		Hazel::Entity entity = m_Scene.CreateEntity();
		entity.AddComponent<Position>(pos + offset);
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

	static const std::array<Position, 4> Directions = {
		Position{-1,  0},
		Position{ 0, -1},
		Position{ 1,  0},
		Position{ 0,  1}
	};

	auto amoebas = m_Scene.m_Registry.group<Amoeba>(entt::get<Position>);

	m_AmoebaSize = static_cast<int>(amoebas.size());
	if (m_AmoebaSize >= 200) { // TODO: parameterize?
		OnSolidify(Tile::BoulderFirst);
	}

	m_AmoebaPotential = 0;
	std::unordered_set<Position> growPositions;

	amoebas.each([&] (auto& amoeba, auto& pos) {
		for (auto direction : Directions) {
			Hazel::Entity entityOther = GetEntity(pos + direction);
			auto tile = entityOther.GetComponent<Tile>();
			if (IsEmpty(tile) || tile == Tile::Dirt1) {
				++m_AmoebaPotential;
				if (Random::Uniform0_1() < amoeba.GrowthProbability) {
					growPositions.emplace(pos + direction);
				}
			} else if (IsExplosive(tile)) {
				OnExplode(pos + direction);
			}
			amoeba.GrowthProbability *= 1.0f + static_cast<float>(amoebas.size()) / 200000.0f;
		}
	});

	if (m_AmoebaPotential == 0) {
		OnSolidify(Tile::Diamond0);
	} else {
		for (auto pos : growPositions) {
			Hazel::Entity entity = GetEntity(pos);
			auto& tileInitial = entity.GetComponent<Tile>();
			if (IsEmpty(tileInitial)) {
				entity = m_Scene.CreateEntity();
				entity.AddComponent<Tile>(tileInitial);
				entity.AddComponent<Position>(pos);
				SetEntity(pos, entity);
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
	// for some reason group<Amoeba>(get<Tile>) does not work here...?
	m_Scene.m_Registry.view<Amoeba, Tile>().each([&] (const auto entityHandle, auto& amoeba, auto& tile) {
		Hazel::Entity entity(entityHandle, &m_Scene);
		entity.RemoveComponent<Amoeba>();  // <-- This removes a component from the entity we are currently iterating.  According to EnTT documentation, this is allowed and will not break anything.
		tile = solidifyTo;
		if (IsDiamond(tile)) {
			auto& animation = entity.GetComponent<Animation>(); // we know it has an Animation component because it was an Amoeba, and Amoeba entities have an Animation
			animation = CharToAnimation('d');
		} else {
			entity.RemoveComponent<Animation>();  // <-- This removes a component from the entity we are currently iterating.  According to EnTT documentation, this is allowed and will not break anything.
		}
	});
}


void HazelDashLayer::ExploderUpdate(Hazel::Timestep ts) {
	HZ_PROFILE_FUNCTION();

	// When we get here, other systems are finished iterating.
	// It is now safe to destroy the game entities at position of explosion entities
	m_Scene.m_Registry.group<Explosion>(entt::get<Position, Animation, Tile>).each([this] (const auto entityHandle, auto& explosion, auto& pos, auto& animation, auto& tile) {
		if (explosion == Explosion::Ignite) {
			ClearEntity(pos);
			SetEntity(pos, {entityHandle, &m_Scene});
			explosion = Explosion::Burn;
		} else {
			if (animation.CurrentFrame == animation.Frames.size() - 1) {
				if (IsDiamond(animation.Frames.back())) {
					// turn into a diamond
					Hazel::Entity entity(entityHandle, &m_Scene);
					entity.RemoveComponent<Explosion>();          // <-- This removes a component from the entity we are currently iterating.  According to EnTT documentation, this is allowed and will not break anything.
					entity.AddComponent<Mass>(Mass::Stationary);  // <-- This adds a component to the entity we are currently iterating.  According to EnTT documentation, this is allowed and will not break anything.
					animation = CharToAnimation('d');
				} else {
					//HZ_ASSERT(Hazel::Entity(entityHandle, &m_Scene) == GetEntity(pos), "Something has misplaced an explosion - game logic error!");
					ClearEntity(pos); // <-- This clears the entity we are currently iterating.  According to EnTT documenation, this is allowed and will  not break anything.
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


void HazelDashLayer::CameraControllerUpdate(Hazel::Timestep ts) {
	HZ_PROFILE_FUNCTION();
	// TODO: placeholder code.  Camera and Viewport may become entities and components at some point
	m_ViewPort.Update(ts);
}


void HazelDashLayer::RendererUpdate(Hazel::Timestep ts) {
	HZ_PROFILE_FUNCTION();

	static glm::vec2 tileSize {1.0f, 1.0f};

	Hazel::RenderCommand::Clear();
	Hazel::Renderer2D::BeginScene(m_ViewPort.GetCamera());

	// Theres a couple of options for how to render here.
	// Ideally, we'd like to just say "for each entity that has a position and a tile, render it"
	// That iteration is cache-friendly, but it renders a lot of entities that it doesn't need to
	// (the ones that are not in the viewport).
	// (either that, or we have to check "is in viewport" for every entity)
	//
	// The other way to do it is use our "index" of entities m_Entities[], since we can efficiently query that
	// for just the entities that are in the view port.
	// However, this is not cache-friendly since it will jump all over the place in the underlying
	// component data.
	//
	// Which is best ??
	// Is there a better way?

	m_Scene.m_Registry.group<Position, Tile>().each([this] (auto& pos, auto& tile) {
		if (m_ViewPort.Overlaps(pos)) {
			glm::vec2 xy = {pos.Col, pos.Row};
			Hazel::Renderer2D::DrawQuad(xy, tileSize, m_Tiles[(int)tile]);
		}
	});
	Hazel::Renderer2D::EndScene();
}


Hazel::Entity HazelDashLayer::GetEntity(const Position pos) {
	return m_Entities[(m_Width * pos.Row) + pos.Col];
}


void HazelDashLayer::SetEntity(Position pos, Hazel::Entity entity) {
	m_Entities[(m_Width * pos.Row) + pos.Col] = entity;
}


void HazelDashLayer::ClearEntity(const Position pos) {
	int index = (m_Width * pos.Row) + pos.Col;
	if (m_Entities[index] != m_EmptyEntity) {
		m_Scene.DestroyEntity(m_Entities[index]);
		m_Entities[index] = m_EmptyEntity;
	}
}


void HazelDashLayer::SwapEntities(const Position posA, const Position posB) {
	size_t indexA = (m_Width * posA.Row) + posA.Col;
	size_t indexB = (m_Width * posB.Row) + posB.Col;

	Hazel::Entity entityA = m_Entities[indexA];
	Hazel::Entity entityB = m_Entities[indexB];

	std::swap(m_Entities[(m_Width * posA.Row) + posA.Col], m_Entities[(m_Width * posB.Row) + posB.Col]);

	Hazel::Entity entityA2 = m_Entities[indexA];
	Hazel::Entity entityB2 = m_Entities[indexB];

}


#ifdef _DEBUG
void HazelDashLayer::OnImGuiRender() {
	ImGui::Begin("Game Stats");
	ImGui::Text("Score: %d", m_Score);

	ImGui::Separator();
	int updateFPS = (int)(1.0f / m_FixedTimestep);
	ImGui::DragInt("Game Speed: ", &updateFPS, 1, 1, 60);
	m_FixedTimestep = 1.0f / updateFPS;
	m_ViewPort.SetCameraSpeed((1.0f / m_FixedTimestep) - 1.0f);

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
