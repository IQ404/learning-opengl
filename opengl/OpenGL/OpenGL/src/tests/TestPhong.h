#ifndef TESTPHONG_H
#define TESTPHONG_H

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
	class TestPhong : public TestBase
	{
		Renderer m_renderer;
		glm::vec3 m_model_world_coordinates;
		glm::mat4 m_model_matrix{1};
		glm::mat4 m_view_matrix;
		glm::mat4 m_projection_matrix;
		std::unique_ptr<VBO> m_vbo;
		std::unique_ptr<VAO> m_vao;
		std::unique_ptr<Shader> m_shader;
		unsigned int m_count = 0;
		float m_fovy = 35.0f;
		float m_y_rotation = 0.0f;
		float m_shininess = 32.0f;
		bool m_blinn = false;

	public:
		TestPhong(const std::string& test_name);
		~TestPhong() override;

		void OnUpdate(float dt) override;
		void OnRender() override;
		void OnImGuiRender() override;

	};

	
}

#endif // !TESTPHONG_H