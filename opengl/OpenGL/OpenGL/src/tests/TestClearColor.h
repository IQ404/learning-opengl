#ifndef TESTCLEARCOLOR_H
#define TESTCLEARCOLOR_H

#include "TestBase.h"

namespace Test
{
	class TestClearColor : public TestBase
	{
	public:
		TestClearColor();
		~TestClearColor() override;

		void OnUpdate(float dt) override;
		void OnRender() override;
		void OnImGuiRender() override;

	private:
		float m_BackgroundColor[4];
	};

	
}

#endif // !TESTCLEARCOLOR_H