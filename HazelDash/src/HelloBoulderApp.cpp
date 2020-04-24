#include "HelloBoulderLayer.h"
#include "Random.h"
#include "Hazel/Core/Application.h"
#include "Hazel/Core/EntryPoint.h"

class HelloBoulderApp : public Hazel::Application
{
public:
	HelloBoulderApp()
	{
		Random::Init();
		PushLayer(new HelloBoulderLayer());
	}

};

Hazel::Application* Hazel::CreateApplication()
{
	return new HelloBoulderApp();
}
