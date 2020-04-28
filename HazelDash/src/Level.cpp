#include "Level.h"
#include "Amoeba.h"
#include "Barrel.h"
#include "Boulder.h"
#include "Butterfly.h"
#include "Diamond.h"
#include "Exit.h"
#include "Explosion.h"
#include "Firefly.h"
#include "Player.h"

#include "Hazel/Core/Core.h"
#include "Hazel/Core/Log.h"

void Level::Init(const LevelDefinition& definition) {

	m_SoundEffects[SoundEffect::Movement1] = std::make_unique<Hazel::AudioSource>(Hazel::AudioSource::LoadFromFile("assets/audio/Movement.ogg"));
	m_SoundEffects[SoundEffect::Movement2] = std::make_unique<Hazel::AudioSource>(Hazel::AudioSource::LoadFromFile("assets/audio/Movement.ogg"));
	m_SoundEffects[SoundEffect::Boulder] = std::make_unique<Hazel::AudioSource>(Hazel::AudioSource::LoadFromFile("assets/audio/Boulder.ogg"));
	m_SoundEffects[SoundEffect::Explode1] = std::make_unique<Hazel::AudioSource>(Hazel::AudioSource::LoadFromFile("assets/audio/Explode1.ogg"));
	m_SoundEffects[SoundEffect::Explode2] = std::make_unique<Hazel::AudioSource>(Hazel::AudioSource::LoadFromFile("assets/audio/Explode2.ogg"));
	m_SoundEffects[SoundEffect::PlayerDie] = std::make_unique<Hazel::AudioSource>(Hazel::AudioSource::LoadFromFile("assets/audio/PlayerDie.ogg"));
	m_SoundEffects[SoundEffect::Movement1]->SetGain(0.1f);
	m_SoundEffects[SoundEffect::Movement2]->SetGain(0.1f);


	m_Width = definition.Width;
	m_Height = definition.Height;
	m_Objects.resize(m_Width * m_Height);
	m_Updated.resize(m_Width * m_Height);
	for (size_t row = 0; row < m_Height; ++row) {
		for (size_t col = 0; col < m_Width; ++col) {
			size_t index = (m_Width * (m_Height - (row + 1))) + col;
			HZ_ASSERT(index < definition.LevelData.size(), "insufficient levelData supplied");
			if (index < definition.LevelData.size()) {
				char ch = definition.LevelData[index];
				switch (ch) {
					case 'P':
						SetGameObject(row, col, std::make_unique<Player>());
						m_PlayerInitialRow = row;
						m_PlayerInitialCol = col;
						break;
					case 'X':
						SetGameObject(row, col, std::make_unique<Exit>());
						m_ExitRow = row;
						m_ExitCol = col;
						break;
					case 'A':
						SetGameObject(row, col, std::make_unique<Amoeba>());
						break;
					case 'B':
						SetGameObject(row, col, std::make_unique<Butterfly>());
						break;
					case 'F':
						SetGameObject(row, col, std::make_unique<Firefly>());
						break;
					case 'W':
						SetGameObject(row, col, std::make_unique<GameObject>(Tile::Metal1));
						break;
					case 'w':
						SetGameObject(row, col, std::make_unique<GameObject>(Tile::Brick1));
						break;
					case '.':
						SetGameObject(row, col, std::make_unique<GameObject>(Tile::Dirt1));
						break;
					case 'r':
						SetGameObject(row, col, std::make_unique<Boulder>());
						break;
					case 'd':
						SetGameObject(row, col, std::make_unique<Diamond>());
						break;
					case 'o':
						SetGameObject(row, col, std::make_unique<Barrel>());
						break;
					default:
						HZ_ASSERT(ch == ' ', "unknown character in level data");
						SetGameObject(row, col, std::make_unique<GameObject>(Tile::Empty));
				}
			}
		}
	}

	for (size_t row = 0; row < m_Height; ++row) {
		for (size_t col = 0; col < m_Width; ++col) {
			GameObject& object = GetGameObject(row, col);
			if (object.IsAmoeba()) {
				++m_AmoebaCount;
				for (const auto& [rowOffset, colOffset] : Amoeba::Directions) {
					if (GetGameObject(row + rowOffset, col + colOffset).CanBeOccupied()) {
						auto potentialAmoeba = std::make_pair(row + rowOffset, col + colOffset);
						if (m_PotentialAmoebaPositions.find(potentialAmoeba) == m_PotentialAmoebaPositions.end()) {
							m_PotentialAmoebaPositions.insert(potentialAmoeba);
						}
					}
				}
			}
		}
	}

	m_Score = 0;
	m_ScoreRequired = definition.ScoreRequired;
}


void Level::ClearUpdated() {
	std::fill(m_Updated.begin(), m_Updated.end(), false);
}

void Level::SwapObjects(size_t rowA, size_t colA, size_t rowB, size_t colB) {
	std::swap(m_Objects[(m_Width * rowA) + colA], m_Objects[(m_Width * rowB) + colB]);
}


void Level::Explode(size_t row, size_t col) {
	for (int rowOffset = -1; rowOffset <= 1; ++rowOffset) {
		for (int colOffset = -1; colOffset <= 1; ++colOffset) {
			GameObject& object = GetGameObject(row + rowOffset, col + colOffset);
			if (object.IsExplodable()) {
				Tile explodeTo = object.IsButterfly()? Tile::Diamond0 : Tile::Empty;
				if (object.IsPlayer()) {
					PlaySound(SoundEffect::PlayerDie);
				}
				SetGameObject(row + rowOffset, col + colOffset, std::make_unique<Explosion>(explodeTo));
				SetUpdated(row + rowOffset, col + colOffset, true);
			}
		}
	}
}


void Level::GrowAmoeba(size_t row, size_t col) {
	SetGameObject(row, col, std::make_unique<Amoeba>());
	SetUpdated(row, col, true);
	++m_AmoebaCount;
	if (m_AmoebaCount >= 200) {
		SolidifyAmoeba(Tile::Boulder0);
	} else {
		m_PotentialAmoebaPositions.erase(std::make_pair(row, col));
	
		for (const auto& [rowOffset, colOffset] : Amoeba::Directions) {
			if (GetGameObject(row + rowOffset, col + colOffset).CanBeOccupied()) {
				auto potentialAmoeba = std::make_pair(row + rowOffset, col + colOffset);
				if (m_PotentialAmoebaPositions.find(potentialAmoeba) == m_PotentialAmoebaPositions.end()) {
					m_PotentialAmoebaPositions.insert(potentialAmoeba);
				}
			}
		}
		if (m_PotentialAmoebaPositions.size() == 0) {
			SolidifyAmoeba(Tile::Diamond0);
		}
	}
}


void Level::SolidifyAmoeba(const Tile solidifyTo) {
	for (size_t row = 0; row < m_Height; ++row) {
		for (size_t col = 0; col < m_Width; ++col) {
			GameObject& object = GetGameObject(row, col);
			if (object.IsAmoeba()) {
				if (solidifyTo == Tile::Boulder0) {
					SetGameObject(row, col, std::make_unique<Boulder>());
				} else if (solidifyTo == Tile::Diamond0) {
					SetGameObject(row, col, std::make_unique<Diamond>());
				}
			}
		}
	}
}


void Level::IncreaseScore() {
	++m_Score;
}

#ifdef _DEBUG
size_t Level::GetAmoebaCount() {
	return m_AmoebaCount;
}

size_t Level::GetAmoebaPotential() {
	return m_PotentialAmoebaPositions.size();
}
#endif

