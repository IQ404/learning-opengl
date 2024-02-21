#ifndef TESTSKYBOX_H
#define TESTSKYBOX_H

#include "TestBase.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "VAO.h"
#include "Shader.h"
#include "Cubemap.h"
#include "Renderer.h"

#include <memory>

namespace Test
{
	class TestSkybox : public TestBase
	{
		Renderer m_renderer;
		glm::mat4 m_rotation_only_view_matrix;
		glm::mat4 m_projection_matrix;
		std::unique_ptr<VBO> m_vbo;
		std::unique_ptr<VAO> m_vao;
		std::unique_ptr<Shader> m_shader;
		std::unique_ptr<Cubemap> m_Cubemap;
		float m_fovy = 35.0f;
		float m_horizontal_rotation = 0.0f;
		float m_vertical_rotation = 0.0f;

	public:
		TestSkybox(const std::string& test_name);
		~TestSkybox() override;

		void OnUpdate(float dt) override;
		void OnRender() override;
		void OnImGuiRender() override;

	};

	
}

#endif // !TESTSKYBOX_H