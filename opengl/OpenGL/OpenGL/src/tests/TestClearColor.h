#ifndef TESTCLEARCOLOR_H
#define TESTCLEARCOLOR_H

#include "TestBase.h"

namespace Test
{
	class TestClearColor : public TestBase
	{
		float m_BackgroundColor[4];

	public:
		TestClearColor(const std::string& test_name);
		~TestClearColor() override;

		void OnUpdate(float dt) override;
		void OnRender() override;
		void OnImGuiRender() override;

	};

	
}

#endif // !TESTCLEARCOLOR_H