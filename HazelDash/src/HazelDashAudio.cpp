#include "HazelDashAudio.h"

std::unordered_map<SoundEffect, std::unique_ptr<Hazel::AudioSource>> HazelDashAudio::m_SoundEffects;
std::unique_ptr<Hazel::AudioSource> HazelDashAudio::m_BackgroundMusic;
