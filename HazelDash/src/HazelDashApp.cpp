#include "HazelDashLayer.h"
#include "Random.h"

#include "Hazel/Core/Application.h"
#include "Hazel/Core/EntryPoint.h"

#include "HazelAudio.h"

class HazelDashApp : public Hazel::Application
{
public:
	HazelDashApp()
	{
		Random::Init();
		Hazel::Audio::Init();
		PushLayer(new HazelDashLayer());
	}

};

Hazel::Application* Hazel::CreateApplication()
{
	return new HazelDashApp();
}
