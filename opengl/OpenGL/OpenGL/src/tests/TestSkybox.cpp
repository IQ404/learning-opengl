#include "TestSkybox.h"
#include "DebugTools.h"
#include "imgui/imgui.h"

namespace Test
{
	TestSkybox::TestSkybox(const std::string& test_name)
        : TestBase(test_name)
	{
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        GLCall(glBlendEquation(GL_FUNC_ADD));

        GLCall(glEnable(GL_DEPTH_TEST));
        GLCall(glDepthFunc(GL_LESS));

        GLCall(glDepthMask(GL_FALSE));

        float skyboxVertices[3 * (3 * 2 * 6)] = {
            
            // local positions:

            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f
        };

        m_vao = std::make_unique<VAO>();

        m_vbo = std::make_unique<VBO>(skyboxVertices, 3 * (3 * 2 * 6) * sizeof(float));

        VBOLayout vbo_layout;
        // local position:
        vbo_layout.AddAttribute<float>(3);

        m_vao->LinkVertexBuffer(*m_vbo, vbo_layout);

        m_rotation_only_view_matrix = glm::mat4(glm::mat3(
            glm::rotate(glm::mat4{ 1.0f }, -glm::radians(m_vertical_rotation), glm::vec3(1, 0, 0)) *
            glm::rotate(glm::mat4{ 1.0f }, glm::radians(m_horizontal_rotation), glm::vec3(0, 1, 0))
        ));
        m_projection_matrix = glm::perspective(glm::radians(m_fovy), 4.0f / 3.0f, 0.1f, 100.0f);
        
        m_shader = std::make_unique<Shader>("res/shaders/Skybox.shader");

        m_Cubemap = std::make_unique<Cubemap>(
            /*"res/textures/skybox_RiverMountain/+X.png",
            "res/textures/skybox_RiverMountain/-X.png",
            "res/textures/skybox_RiverMountain/+Y.png",
            "res/textures/skybox_RiverMountain/-Y.png",
            "res/textures/skybox_RiverMountain/+Z.png",
            "res/textures/skybox_RiverMountain/-Z.png"*/
            "res/textures/skybox_SkyCloud/+X.png",
            "res/textures/skybox_SkyCloud/-X.png",
            "res/textures/skybox_SkyCloud/+Y.png",
            "res/textures/skybox_SkyCloud/-Y.png",
            "res/textures/skybox_SkyCloud/+Z.png",
            "res/textures/skybox_SkyCloud/-Z.png"
        );
        m_Cubemap->Bind(0);

        m_shader->Bind();
        m_shader->SetUniform_1int("u_Cubemap", 0);
        m_shader->SetUniform_float_matrix_4_4("u_ProjectionMatrix", m_projection_matrix);
        m_shader->SetUniform_float_matrix_4_4("u_RotationOnlyViewMatrix", m_rotation_only_view_matrix);

        // Careful: if we write m_Cubemap.Unbind(); here, u_Cubemap will be linked to the default texture object with ID 0!

	}

	TestSkybox::~TestSkybox()
	{
        // recover the default settings:
        GLCall(glBlendFunc(GL_ONE, GL_ZERO));
        GLCall(glBlendEquation(GL_FUNC_ADD));
        /*
        Remember to disable blending with glDisable(GL_BLEND) when it's no longer needed, as leaving it enabled when drawing opaque objects
        can unnecessarily hurt performance.
        */
        GLCall(glDisable(GL_BLEND));

        GLCall(glDisable(GL_DEPTH_TEST));

        GLCall(glDepthMask(GL_TRUE));
	}

	void TestSkybox::OnUpdate(float dt)
	{
        m_projection_matrix = glm::perspective(glm::radians(m_fovy), 4.0f / 3.0f, 0.1f, 100.0f); // aspect ratio = 4:3
        m_rotation_only_view_matrix = glm::mat4(glm::mat3(
            glm::rotate(glm::mat4{ 1.0f }, -glm::radians(m_vertical_rotation), glm::vec3(1, 0, 0)) * 
            glm::rotate(glm::mat4{ 1.0f }, glm::radians(m_horizontal_rotation), glm::vec3(0, 1, 0))
        ));
        m_shader->SetUniform_float_matrix_4_4("u_ProjectionMatrix", m_projection_matrix);
        m_shader->SetUniform_float_matrix_4_4("u_RotationOnlyViewMatrix", m_rotation_only_view_matrix);
	}

	void TestSkybox::OnRender()
	{
        m_renderer.LinearDraw(*m_vao, *m_shader, 36);   // 3 * 2 * 6
	}

	void TestSkybox::OnImGuiRender()
	{
        ImGui::SliderFloat("FOVy", &m_fovy, 1.0f, 179.0f);
        ImGui::SliderFloat("Horizontal Rotation", &m_horizontal_rotation, -180.0f, 180.0f);
        ImGui::SliderFloat("Vertical Rotation", &m_vertical_rotation, -89.0f, 89.0f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}
