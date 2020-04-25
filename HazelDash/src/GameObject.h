#pragma once

#include "Tile.h"
#include "Hazel/Core/Timestep.h"

class Level;

class GameObject {
public:
	GameObject(Tile tile = Tile::Dirt1) : m_Tile(tile) {}

	virtual ~GameObject() = default;

	virtual void FixedUpdate(size_t row, size_t col, Level& level) {}

	virtual void Animate(Hazel::Timestep ts) {}

	virtual void IncreaseFrame() {}
	virtual void DecreaseFrame() {}

	virtual Tile GetTile() const { return m_Tile; }

	bool IsEmpty() const { return ::IsEmpty(GetTile()); }
	bool IsSolid() const { return ::IsSolid(GetTile()); }
	bool IsPlayer() const { return ::IsPlayer(GetTile()); }
	bool IsAmoeba() const { return ::IsAmoeba(GetTile()); }
	bool IsButterfly() const { return ::IsButterfly(GetTile()); }
	bool IsRounded() const { return ::IsRounded(GetTile()); }
	bool IsPushable() const { return ::IsPushable(GetTile()); }
	bool IsExplosive() const { return ::IsExplosive(GetTile()); }
	bool IsExplodable() const { return ::IsExplodable(GetTile()); }
	bool IsCollectable() const { return ::IsCollectable(GetTile()); }
	bool CanBeOccupied() const { return ::CanBeOccupied(GetTile()); }

#ifdef _DEBUG
	virtual void ImGuiRender() {};
#endif

private:
	Tile m_Tile;
};
