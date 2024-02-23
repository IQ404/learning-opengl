#ifndef TESTOFFSETSCALINGROTATION_H
#define TESTOFFSETSCALINGROTATION_H

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
	class TestOffsetScalingRotation : public TestBase
	{
		Renderer m_renderer;
		glm::vec3 m_model_world_coordinates;
		unsigned int m_model_vertices_count = 0;
		glm::mat4 m_model_matrix{ 1 };
		glm::mat4 m_view_matrix;
		glm::mat4 m_projection_matrix;
		std::unique_ptr<VBO> m_model_vbo;
		std::unique_ptr<VBO> m_skybox_vbo;
		std::unique_ptr<VAO> m_model_vao;
		std::unique_ptr<VAO> m_skybox_vao;
		std::unique_ptr<Shader> m_model_shader;
		std::unique_ptr<Shader> m_skybox_shader;
		std::unique_ptr<Cubemap> m_Cubemap;
		float m_fovy = 35.0f;
		float m_camera_horizontal_rotation = 0.0f;
		float m_camera_vertical_rotation = 0.0f;
		float m_model_y_rotation = 0.0f;
		float m_model_shininess = 32.0f;
		bool m_fresnel_effect_on = true;
		float time_passed = 0.0f;

		void SkyboxPass();
		void ModelPass();

	public:
		TestOffsetScalingRotation(const std::string& test_name);
		~TestOffsetScalingRotation() override;

		void OnUpdate(float dt) override;
		void OnRender() override;
		void OnImGuiRender() override;

	};

	
}

#endif // !TESTOFFSETSCALINGROTATION_H