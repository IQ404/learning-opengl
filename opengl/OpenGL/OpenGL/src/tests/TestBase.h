#ifndef TESTBASE_H
#define TESTBASE_H

#include <string>

namespace Test
{

	class TestBase
	{
		std::string m_test_name = "Test";

	protected:

		TestBase(const std::string& test_name)
		{
			m_test_name = test_name;
		}

	public:

		TestBase() = delete;	// we don't want the user to directly construct TestBase

		virtual ~TestBase()
		{

		}

		virtual void OnUpdate(float dt)
		{

		}

		virtual void OnRender()
		{

		}

		virtual void OnImGuiRender()
		{

		}

		std::string GetTestName()	// TODO: should this be virtual?
		{
			return m_test_name;
		}

	};

}

#endif // !TESTBASE_H
