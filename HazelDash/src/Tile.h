#pragma once

enum class Tile {
	ButterflyFirst,
	Butterfly0 = ButterflyFirst,
	Butterfly1,
	Butterfly2,
	Butterfly3,
	ButterflyLast = Butterfly3,
	ButterflySpare0,
	ButterflySpare1,
	ButterflySpare2,
	ButterflySpare3,
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
	PlayerFirst,
	PlayerDown0 = PlayerFirst,
	PlayerDown1,
	PlayerDown2,
	PlayerDown3,
	PlayerDownSpare0,
	PlayerDownSpare1,
	PlayerDownSpare2,
	PlayerDownSpare3,
	PlayerUp0,
	PlayerUp1,
	PlayerUp2,
	PlayerUp3,
	PlayerUpSpare0,
	PlayerUpSpare1,
	PlayerUpSpare2,
	PlayerUpSpare3,
	PlayerLeft0,
	PlayerLeft1,
	PlayerLeft2,
	PlayerLeft3,
	PlayerLeftSpare0,
	PlayerLeftSpare1,
	PlayerLeftSpare2,
	PlayerLeftSpare3,
	PlayerRight0,
	PlayerRight1,
	PlayerRight2,
	PlayerRight3,
	PlayerRightSpare0,
	PlayerRightSpare1,
	PlayerRightSpare2,
	PlayerRightSpare3,
	PlayerIdle0,
	PlayerIdle1,
	PlayerIdle2,
	PlayerIdle3,
	PlayerIdleSpare0,
	PlayerIdleSpare1,
	PlayerIdleSpare2,
	PlayerIdleSpare3,
	PlayerLast = PlayerIdleSpare3,
	FireflyFirst,
	Firefly0 = FireflyFirst,
	Firefly1,
	Firefly2,
	Firefly3,
	FireflySpare0,
	FireflySpare1,
	FireflySpare2,
	FireflySpare3,
	FireflyLast = FireflySpare3,
	BoulderFirst,
	Boulder0 = BoulderFirst,
	Boulder1,
	Boulder2,
	Boulder3,
	BoulderSpare0,
	BoulderSpare1,
	BoulderSpare2,
	BoulderSpare3,
	BoulderLast = BoulderSpare3,
	BarrelFirst,
	Barrel0 = BarrelFirst,
	Barrel1,
	Barrel2,
	Barrel3,
	BarrelSpare0,
	BarrelSpare1,
	BarrelSpare2,
	BarrelSpare3,
	BarrelLast = BarrelSpare3,
	GoldMetal,
	GoldDirt,
	GoldBricks,
	GoldMud,
	GoldSpare0,
	GoldSpare1,
	GoldSpare2,
	GoldSpare3,
	BlueMetal,
	BlueDirt,
	BlueBricks,
	BlueMud,
	BlueSpare0,
	BlueSpare1,
	BlueSpare2,
	BlueSpare3,
	Metal,
	Dirt,
	Brick,
	Mud,
	Spare0,
	Spare1,
	Spare2,
	Spare3,
	Random0,
	Random1,
	Random2,
	Random3,
	Spare4,
	Spare5,
	Spare6,
	Spare7,
	DoorFirst,
	Door0 = DoorFirst,
	Door1,
	Door2,
	Door3,
	DoorLast = Door3,
	Spare8,
	Spare9,
	Spare10,
	Spare11,
	SmokeFirst,
	Smoke0 = SmokeFirst,
	Smoke1,
	Smoke2,
	Smoke3,
	SmokeLast = Smoke3,
	Spare12,
	Spare13,
	Spare14,
	Empty
};

inline bool IsEmpty(Tile tile) {
	return (tile == Tile::Empty);
}

inline bool IsSolid(Tile tile) {
	return (tile != Tile::Empty);
}

inline bool IsPlayer(Tile tile) {
	return ((tile >= Tile::PlayerFirst) && (tile <= Tile::PlayerLast));
}

inline bool IsAmoeba(Tile tile) {
	return ((tile >= Tile::SmokeFirst) && (tile <= Tile::SmokeLast));
}

inline bool IsButterfly(Tile tile) {
	return ((tile >= Tile::ButterflyFirst) && (tile <= Tile::ButterflyLast));
}

inline bool IsFirefly(Tile tile) {
	return ((tile >= Tile::FireflyFirst) && (tile <= Tile::FireflyLast));
}

inline bool IsRounded(Tile tile) {
	return ((tile >= Tile::DiamondFirst) && (tile <= Tile::DiamondLast)) || ((tile >= Tile::BoulderFirst) && (tile <= Tile::BoulderLast)) || ((tile >= Tile::BarrelFirst) && (tile <= Tile::BarrelLast));
}

inline bool IsPushable(Tile tile) {
	return ((tile >= Tile::BoulderFirst) && (tile <= Tile::BoulderLast)) || ((tile >= Tile::BarrelFirst) && (tile <= Tile::BarrelLast));
}

// true if this tile will itself explode if caught in an explosion
// (i.e. chain reaction)
inline bool IsExplosive(Tile tile) {
	return IsPlayer(tile) || IsButterfly(tile) || IsFirefly(tile);
}

inline bool IsExplodable(Tile tile) {
	return (tile != Tile::GoldMetal) && (tile != Tile::BlueMetal) && (tile != Tile::Metal) && !((tile >= Tile::DoorFirst && tile <= Tile::DoorLast));
}

inline bool IsCollectable(Tile tile) {
	return (tile >= Tile::DiamondFirst) && (tile <= Tile::DiamondLast);
}

inline bool CanBeOccupied(Tile tile) {
	return (tile == Tile::Empty) || (tile == Tile::Dirt) || (tile >= Tile::DoorFirst && tile <= Tile::DoorLast) || IsCollectable(tile);
}