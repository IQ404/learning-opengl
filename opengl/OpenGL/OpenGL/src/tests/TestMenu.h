#ifndef TESTMENU_H
#define TESTMENU_H

#include "TestBase.h"
#include <vector>
#include <functional>
#include <string>

#include <iostream>

namespace Test
{

	class TestMenu : public TestBase
	{
		std::vector< std::pair< std::string, std::function< TestBase* () > > > m_Tests;
		TestBase*& m_CurrentTest;	// we want to directly modify an external pointer

	public:

		TestMenu(TestBase*& currentTestPointer);

		virtual ~TestMenu() override
		{
			std::cout << "\nDestroying the menu...\n";
		}

		void OnImGuiRender() override;

		template <typename T>	// T must be a kind of TestBase
		void AddTest(const std::string& test_name)
		{
			// Note that we captured test_name by value: the parameter dies when AddTest returns
			m_Tests.push_back(std::make_pair(test_name, [test_name]() { return new T{ test_name }; }));  // Note that we are NOT creating the test here!
			// TODO: is this using templated copy constructor or is this using any sort of move semantic?
			// TODO: how many mandatory copy elision happens here?
		}

	};
}

#endif // !TESTMENU_H