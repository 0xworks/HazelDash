#pragma once

#include <type_traits>

struct Position {
	int Row = 0;
	int Col = 0;

	Position& operator+=(const Position posOther) {
		Row += posOther.Row;
		Col += posOther.Col;
		return *this;
	}

	Position& operator-=(const Position posOther) {
		Row -= posOther.Row;
		Col -= posOther.Col;
		return *this;
	}
};


inline bool operator==(const Position posA, const Position posB) {
	return (posA.Row == posB.Row) && (posA.Col == posB.Col);
}

inline bool operator!=(const Position posA, const Position posB) {
	return (posA.Row != posB.Row) || (posA.Col != posB.Col);
}


inline Position operator+(const Position posA, const Position posB) {
	return Position {posA.Row + posB.Row, posA.Col + posB.Col};
}

inline Position operator-(const Position posA, const Position posB) {
	return Position {posA.Row - posB.Row, posA.Col - posB.Col};
};

namespace std {

	template<>
	struct hash<Position> {
		size_t operator()(const Position& pos) const {
			std::hash<int> hasher;
			size_t hash = hasher(pos.Row);
			hash ^= hasher(pos.Col) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
			return hash;
		}
	};

}
