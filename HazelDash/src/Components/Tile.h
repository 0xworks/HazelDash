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
	Metal2,        // not currently in use
	Dirt2,         // not currently in use
	Brick2,        // not currently in use
	Background2,   // not currently in use
	Spare0,
	Spare1,
	Spare2,
	Spare3,
	Metal3,      // not currently in use
	Dirt3,       // not currently in use
	Brick3,      // not currently in use
	Background3, // not currently in use
	Spare4,
	Spare5,
	Spare6,
	Spare7,
	Metal1,
	Dirt1,
	Brick1,
	Empty,
	Spare8,
	Spare9,
	Spare10,
	Spare11,
	Random0,
	Random1,
	Random2,
	Random3,
	DoorFirst,
	Door0 = DoorFirst,
	Door1,
	Door2,
	Door3,
	DoorLast = Door3,
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
	ExplosionFirst,
	Explosion0 = ExplosionFirst,
	Explosion1,
	Explosion2,
	Explosion3,
	Explosion4,
	Explosion5,
	Explosion6,
	Explosion7,
	ExplosionLast = Explosion7,
	ExplosionDiamond4,
	ExplosionDiamond5,
	ExplosionDiamond6,
	Spare12,

	NumTiles
};

inline bool IsEmpty(Tile tile) {
	return tile == Tile::Empty; // || (tile == Tile::Background2) || (tile == Tile::Background2) || (tile == Tile::Background3);
}

inline bool IsSolid(Tile tile) {
	return (!IsEmpty(tile));
}

inline bool IsPlayer(Tile tile) {
	return ((tile >= Tile::PlayerFirst) && (tile <= Tile::PlayerLast));
}

inline bool IsAmoeba(Tile tile) {
	return ((tile >= Tile::AmoebaFirst) && (tile <= Tile::AmoebaLast));
}

inline bool IsBarrel(Tile tile) {
	return ((tile >= Tile::BarrelFirst) && (tile <= Tile::BarrelLast));
}

inline bool IsBoulder(Tile tile) {
	return ((tile >= Tile::BoulderFirst) && (tile <= Tile::BoulderLast));
}

inline bool IsButterfly(Tile tile) {
	return ((tile >= Tile::ButterflyFirst) && (tile <= Tile::ButterflyLast));
}

inline bool IsDiamond(Tile tile) {
	return ((tile >= Tile::DiamondFirst) && (tile <= Tile::DiamondLast));
}

inline bool IsDoor(Tile tile) {
	return ((tile >= Tile::DoorFirst) && (tile <= Tile::DoorLast));
}

inline bool IsExplosion(Tile tile) {
	return (tile >= Tile::ExplosionFirst) && (tile <= Tile::ExplosionLast);
}

inline bool IsFirefly(Tile tile) {
	return ((tile >= Tile::FireflyFirst) && (tile <= Tile::FireflyLast));
}

inline bool IsRounded(Tile tile) {
	return IsBoulder(tile) || IsBarrel(tile) || IsDiamond(tile) || (tile == Tile::Brick1);
}

inline bool IsPushable(Tile tile) {
	return IsBoulder(tile) || IsBarrel(tile);
}

// true if this tile will itself explode if caught in an explosion
// (i.e. chain reaction)
inline bool IsExplosive(Tile tile) {
	return IsPlayer(tile) || IsButterfly(tile) || IsFirefly(tile) || IsBarrel(tile);
}

inline bool IsExplodable(Tile tile) {
	return (tile != Tile::Metal1) && !IsDoor(tile) && !IsExplosion(tile); // && (tile != Tile::Metal2) && (tile != Tile::Metal3)
}

inline bool IsCollectable(Tile tile) {
	return IsDiamond(tile);
}

inline bool CanBeOccupied(Tile tile) {
	return IsEmpty(tile) || (tile == Tile::Dirt1) || (tile == Tile::DoorLast) || IsCollectable(tile); // || (tile == Tile::Dirt2) || (tile == Tile::Dirt3)
}
