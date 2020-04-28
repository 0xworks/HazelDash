#include "HazelDashLayer.h"
#include "Random.h"
#include "Hazel/Core/Application.h"
#include "Hazel/Core/EntryPoint.h"

class HazelDashApp : public Hazel::Application
{
public:
	HazelDashApp()
	{
		Random::Init();
		PushLayer(new HazelDashLayer());
	}

};

Hazel::Application* Hazel::CreateApplication()
{
	return new HazelDashApp();
}
