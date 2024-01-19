#ifndef TESTMENU_H
#define TESTMENU_H

#include "TestBase.h"
#include <vector>
#include <functional>
#include <string>

namespace Test
{

	class TestMenu : public TestBase
	{
		std::vector< std::pair< std::string, std::function< TestBase* () > > > m_Tests;
		TestBase*& m_CurrentTest;

	public:

		TestMenu(TestBase*& currentTestPointer);

		// We currently aren't overriding the virtual destructor because all data members are on stack

		void OnImGuiRender() override;

		template <typename T>	// T must be a kind of TestBase
		void AddTest(const std::string& test_name)
		{
			m_Tests.push_back(std::make_pair(test_name, []() { return new T{}; }));  // Note that we are NOT creating the test here!
			// TODO: is this using templated copy constructor or is this using any sort of move semantic?
			// TODO: how many mandatory copy elision happens here?
		}

	};
}

#endif // !TESTMENU_H