#ifndef TESTCELSHADING_H
#define TESTCELSHADING_H

#include "TestBase.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "VAO.h"
#include "Shader.h"
#include "Renderer.h"

#include <memory>

namespace Test
{
	class TestCelShading : public TestBase
	{
		Renderer m_renderer;
		glm::vec3 m_model_world_coordinates;
		unsigned int m_model_vertices_count = 0;
		glm::mat4 m_model_matrix{ 1 };
		glm::mat4 m_view_matrix;
		glm::mat4 m_projection_matrix;
		std::unique_ptr<VBO> m_model_vbo;
		std::unique_ptr<VAO> m_model_vao;
		std::unique_ptr<Shader> m_model_shader;
		float m_fovy = 35.0f;
		float m_model_x_rotation = 0.0f;
		float m_model_y_rotation = 0.0f;
		float m_model_shininess = 32.0f;

		void ModelPass();

	public:
		TestCelShading(const std::string& test_name);
		~TestCelShading() override;

		void OnUpdate(float dt) override;
		void OnRender() override;
		void OnImGuiRender() override;

	};

	
}

#endif // !TESTCELSHADING_H