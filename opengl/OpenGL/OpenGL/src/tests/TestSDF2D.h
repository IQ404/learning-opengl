#ifndef TESTSDF2D_H
#define TESTSDF2D_H

#include "TestBase.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "VAO.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Renderer.h"

#include <memory>

namespace Test
{
	class TestSDF2D : public TestBase
	{
		Renderer m_renderer;
		glm::vec3 m_model_world_coordinates;
		glm::mat4 m_model_matrix{1};
		glm::mat4 m_view_matrix;
		glm::mat4 m_projection_matrix;
		std::unique_ptr<VBO> m_vbo;
		std::unique_ptr<VAO> m_vao;
		std::unique_ptr<IndexBuffer> m_index_buffer;
		std::unique_ptr<Shader> m_shader;
		float m_time_passed = 0.0f;
		float m_signed_distance = -100.0f;
		bool m_soft = true;
		float m_movable_circle_x = 200.0f;

	public:
		TestSDF2D(const std::string& test_name);
		~TestSDF2D() override;

		void OnUpdate(float dt) override;
		void OnRender() override;
		void OnImGuiRender() override;

	};

	
}

#endif // !TESTSDF2D_H