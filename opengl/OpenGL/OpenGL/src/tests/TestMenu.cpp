#include "TestMenu.h"
#include "imgui/imgui.h"

namespace Test
{
	TestMenu::TestMenu(TestBase*& currentTestPointer)
		: m_CurrentTest{ currentTestPointer }
	{

	}

	void TestMenu::OnImGuiRender()
	{
		bool test_created = false;	// to avoid potential memory leak from creating multiple tests in one frame
		for (const auto& test : m_Tests)
		{
			if (ImGui::Button(test.first.c_str()) && (!test_created))
			/*
			Note that we cannot have two tests with the same name:
			in that case, the return value of the ImGui::Button will reveal the state for the first button with the name (although that ImGui::Button draws for the second botton)
			in which I think this reveals a sloppy implementation of ImGui...
			*/
			{
				m_CurrentTest = test.second();
				test_created = true;
			}
		}
	}
	
}
