#pragma once

#include "GameObject.h"
#include "HazelAudio.h"

#include <array>
#include <memory>
#include <set>
#include <string>
#include <vector>

struct LevelDefinition {
	size_t Width;
	size_t Height;
	int ScoreRequired;
	int TimeAllowed;
	std::string LevelData;
};

class Level {
public:
	void Init(const LevelDefinition& definition);

	size_t GetWidth() const { return m_Width; }
	size_t GetHeight() const { return m_Height; }

	size_t GetPlayerInitialRow() const { return m_PlayerInitialRow; }
	size_t GetPlayerInitialCol() const { return m_PlayerInitialCol; }

	size_t GetExitRow() const { return m_ExitRow; }
	size_t GetExitCol() const { return m_ExitCol; }

	const Hazel::AudioSource& GetMovementSound() { return *m_MovementSound; }
	const Hazel::AudioSource& GetPlayerDieSound() { return *m_PlayerDieSound; }

	// don't call this "GetObject" - because that conflicts with a #define on windows platform...quite annoying!
	GameObject& GetGameObject(size_t row, size_t col) { return *(m_Objects[(m_Width * row) + col].get()); }
	void SetGameObject(size_t row, size_t col, std::unique_ptr<GameObject> object) { m_Objects[(m_Width * row) + col] = std::move(object); }

	bool IsUpdated(size_t row, size_t col) { return m_Updated[(m_Width * row) + col]; }
	void SetUpdated(size_t row, size_t col, bool updated) { m_Updated[(m_Width * row) + col] = updated; }
	void ClearUpdated();

	void SwapObjects(size_t rowA, size_t colA, size_t rowB, size_t colB);

	void Explode(size_t row, size_t col);

	void GrowAmoeba(size_t row, size_t col);
	void SolidifyAmoeba(const Tile solidifyTo);

	int GetScoreRequired() { return m_ScoreRequired; }

	int GetScore() { return m_Score; }
	void IncreaseScore();

#ifdef _DEBUG
	size_t GetAmoebaCount();
	size_t GetAmoebaPotential();
#endif

private:
	size_t m_Width = 0;
	size_t m_Height = 0;
	size_t m_PlayerInitialRow = 1;
	size_t m_PlayerInitialCol = 1;
	size_t m_ExitRow = 2;
	size_t m_ExitCol = 2;

	std::vector<std::unique_ptr<GameObject>> m_Objects;
	std::vector<bool> m_Updated;
	std::unique_ptr<Hazel::AudioSource> m_MovementSound;
	std::unique_ptr<Hazel::AudioSource> m_PlayerDieSound;

	int m_ScoreRequired = 100;
	int m_Score = 0;

	size_t m_AmoebaCount = 0;
	std::set<std::pair<size_t, size_t>> m_PotentialAmoebaPositions;

};
