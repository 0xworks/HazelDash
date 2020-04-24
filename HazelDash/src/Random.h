#pragma once

#include <random>

class Random {
public:
	static void Init() {
		s_RandomEngine.seed(std::random_device()());
	}

	static float Uniform0_1() {
		return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
	}

private:
	static std::mt19937 s_RandomEngine;
	static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
};
