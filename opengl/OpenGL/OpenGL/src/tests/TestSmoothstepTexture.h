#ifndef TESTSMOOTHSTEOTEXTURE_H
#define TESTSMOOTHSTEOTEXTURE_H

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
	class TestSmoothstepTexture : public TestBase
	{
		Renderer m_renderer;
		glm::vec3 m_model_world_coordinates;
		glm::mat4 m_model_matrix{1};
		glm::mat4 m_view_matrix;
		glm::mat4 m_projection_matrix;
		glm::mat4 m_mvp_matrix{1};
		std::unique_ptr<VBO> m_vbo;
		std::unique_ptr<VAO> m_vao;
		std::unique_ptr<IndexBuffer> m_index_buffer;
		std::unique_ptr<Shader> m_shader;
		std::unique_ptr<Texture> m_Texture;
		bool m_smoothstep_on = false;

	public:
		TestSmoothstepTexture(const std::string& test_name);
		~TestSmoothstepTexture() override;

		void OnUpdate(float dt) override;
		void OnRender() override;
		void OnImGuiRender() override;

	};

	
}

#endif // !TESTSMOOTHSTEOTEXTURE_H