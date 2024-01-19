#include "TestClearColor.h"
#include "DebugTools.h"
#include "imgui/imgui.h"

namespace Test
{
	TestClearColor::TestClearColor()
		: m_BackgroundColor{ 0.2f,0.3f,0.8f,1.0f }
	{

	}

	TestClearColor::~TestClearColor()
	{

	}

	void TestClearColor::OnUpdate(float dt)
	{

	}
	void TestClearColor::OnRender()
	{
		GLCall(glClearColor(m_BackgroundColor[0], m_BackgroundColor[1], m_BackgroundColor[2], m_BackgroundColor[3]));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));
	}

	void TestClearColor::OnImGuiRender()
	{
		ImGui::ColorEdit4("Background Color", m_BackgroundColor);
	}
}
