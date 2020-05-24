#include "HazelAudio.h"
#include "SoundEffect.h"

#include "Hazel/Core/Log.h"

#include <memory>
#include <unordered_map>

class HazelDashAudio {
public:

	static void Init() {
		m_SoundEffects[SoundEffect::Movement1] = std::make_unique<Hazel::AudioSource>(Hazel::AudioSource::LoadFromFile("assets/audio/Movement.ogg"));
		m_SoundEffects[SoundEffect::Movement2] = std::make_unique<Hazel::AudioSource>(Hazel::AudioSource::LoadFromFile("assets/audio/Movement.ogg"));
		m_SoundEffects[SoundEffect::Collect] = std::make_unique<Hazel::AudioSource>(Hazel::AudioSource::LoadFromFile("assets/audio/Collect.ogg"));
		m_SoundEffects[SoundEffect::Barrel] = std::make_unique<Hazel::AudioSource>(Hazel::AudioSource::LoadFromFile("assets/audio/Barrel.ogg"));
		m_SoundEffects[SoundEffect::Boulder] = std::make_unique<Hazel::AudioSource>(Hazel::AudioSource::LoadFromFile("assets/audio/Boulder.ogg"));
		m_SoundEffects[SoundEffect::Diamond] = std::make_unique<Hazel::AudioSource>(Hazel::AudioSource::LoadFromFile("assets/audio/Diamond.ogg"));
		m_SoundEffects[SoundEffect::Explode1] = std::make_unique<Hazel::AudioSource>(Hazel::AudioSource::LoadFromFile("assets/audio/Explode1.ogg"));
		m_SoundEffects[SoundEffect::Explode2] = std::make_unique<Hazel::AudioSource>(Hazel::AudioSource::LoadFromFile("assets/audio/Explode2.ogg"));
		m_SoundEffects[SoundEffect::PlayerDie] = std::make_unique<Hazel::AudioSource>(Hazel::AudioSource::LoadFromFile("assets/audio/PlayerDie.ogg"));
		m_SoundEffects[SoundEffect::Movement1]->SetGain(0.1f);
		m_SoundEffects[SoundEffect::Movement2]->SetGain(0.1f);
		m_SoundEffects[SoundEffect::Collect]->SetGain(0.1f);

		m_BackgroundMusic = std::make_unique<Hazel::AudioSource>(Hazel::AudioSource::LoadFromFile("assets/audio/BackgroundMusic.mp3"));
		m_BackgroundMusic->SetLoop(true);
		Hazel::Audio::Play(*m_BackgroundMusic);
	}

	static void PlaySound(SoundEffect sound) {
		HZ_ASSERT(m_SoundEffects[sound], "ERROR: SoundEffect has not been initialised");
		Hazel::Audio::Play(*m_SoundEffects[sound]);
	}


private:
	HazelDashAudio() = delete;
	~HazelDashAudio() = delete;

	static std::unordered_map<SoundEffect, std::unique_ptr<Hazel::AudioSource>> m_SoundEffects;
	static std::unique_ptr<Hazel::AudioSource> m_BackgroundMusic;
};
