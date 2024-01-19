#ifndef TESTBASE_H
#define TESTBASE_H

namespace Test
{

	class TestBase
	{

	public:

		TestBase()
		{

		}

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

	};

}

#endif // !TESTBASE_H
