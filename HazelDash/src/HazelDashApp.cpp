#include "HazelDashLayer.h"
#include "Random.h"

#include "Hazel/Core/Application.h"
#include "Hazel/Core/EntryPoint.h"

class HazelDashApp : public Hazel::Application {
public:
	HazelDashApp(Hazel::ApplicationCommandLineArgs args) {
		Random::Init();
		PushLayer(new HazelDashLayer());
	}
};


Hazel::Application* Hazel::CreateApplication(Hazel::ApplicationCommandLineArgs args) {
	return new HazelDashApp(args);
}
