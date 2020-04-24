#pragma once

enum class Tile {
	PlayerIdle = 0,
	Explode0,
	Explode1,
	Explode2,
	White,
	Empty,
	Spare0,
	Spare1,
	PlayerFirst,
	PlayerAnim1_0 = PlayerFirst,
	PlayerAnim1_1,
	PlayerAnim1_2,
	PlayerAnim1_3,
	PlayerAnim1_4,
	PlayerAnim1_5,
	PlayerAnim1_6,
	PlayerAnim1_7,
	PlayerAnim2_0,
	PlayerAnim2_1,
	PlayerAnim2_2,
	PlayerAnim2_3,
	PlayerAnim2_4,
	PlayerAnim2_5,
	PlayerAnim2_6,
	PlayerAnim2_7,
	PlayerAnim3_0,
	PlayerAnim3_1,
	PlayerAnim3_2,
	PlayerAnim3_3,
	PlayerAnim3_4,
	PlayerAnim3_5,
	PlayerAnim3_6,
	PlayerAnim3_7,
	PlayerLeft0,
	PlayerLeft1,
	PlayerLeft2,
	PlayerLeft3,
	PlayerLeft4,
	PlayerLeft5,
	PlayerLeft6,
	PlayerLeft7,
	PlayerRight0,
	PlayerRight1,
	PlayerRight2,
	PlayerRight3,
	PlayerRight4,
	PlayerRight5,
	PlayerRight6,
	PlayerRight7,
	PlayerLast = PlayerRight7,
	Spare4,
	SteelWall,
	Exit,
	BrickWall,
	MagicWall0,
	MagicWall1,
	MagicWall2,
	MagicWall3,
	Boulder,
	Dirt,
	BlankDiamond,
	DiamondExplode0,
	DiamondExplode1,
	DiamondExplode2,
	DiamondExplode3,
	DiamondExplode4,
	AmoebaFirst,
	Amoeba0 = AmoebaFirst,
	Amoeba1,
	Amoeba2,
	Amoeba3,
	Amoeba4,
	Amoeba5,
	Amoeba6,
	Amoeba7,
	AmoebaLast = Amoeba7,
	FireflyFirst,
	Firefly0 = FireflyFirst,
	Firefly1,
	Firefly2,
	Firefly3,
	Firefly4,
	Firefly5,
	Firefly6,
	Firefly7,
	FireflyLast = Firefly7,
	DiamondFirst,
	Diamond0 = DiamondFirst,
	Diamond1,
	Diamond2,
	Diamond3,
	Diamond4,
	Diamond5,
	Diamond6,
	Diamond7,
	DiamondLast = Diamond7,
	ButterflyFirst,
	Butterfly0 = ButterflyFirst,
	Butterfly1,
	Butterfly2,
	Butterfly3,
	Butterfly4,
	Butterfly5,
	Butterfly6,
	Butterfly7,
	ButterflyLast = Butterfly7
};

inline bool IsEmpty(Tile tile) {
	return (tile == Tile::Empty);
}

inline bool IsSolid(Tile tile) {
	return (tile != Tile::Empty);
}

inline bool IsPlayer(Tile tile) {
	return (tile == Tile::PlayerIdle) || ((tile >= Tile::PlayerFirst) && (tile <= Tile::PlayerLast));
}

inline bool IsAmoeba(Tile tile) {
	return ((tile >= Tile::AmoebaFirst) && (tile <= Tile::AmoebaLast));
}

inline bool IsButterfly(Tile tile) {
	return ((tile >= Tile::ButterflyFirst) && (tile <= Tile::ButterflyLast));
}

inline bool IsRounded(Tile tile) {
	return (tile == Tile::Boulder) || ((tile >= Tile::DiamondFirst) && (tile <= Tile::DiamondLast));
}

inline bool IsPushable(Tile tile) {
	return (tile == Tile::Boulder);
}

// true if this tile will itself explode if caught in an explosion
// (i.e. chain reaction)
inline bool IsExplosive(Tile tile) {
	return IsPlayer(tile) || IsButterfly(tile);
}

inline bool IsExplodable(Tile tile) {
	return (tile != Tile::SteelWall) && (tile != Tile::Exit);
}

inline bool IsCollectable(Tile tile) {
	return (tile >= Tile::DiamondFirst) && (tile <= Tile::DiamondLast);
}

inline bool CanBeOccupied(Tile tile) {
	return (tile == Tile::Empty) || (tile == Tile::Dirt) || (tile == Tile::Exit) || IsCollectable(tile);
}
