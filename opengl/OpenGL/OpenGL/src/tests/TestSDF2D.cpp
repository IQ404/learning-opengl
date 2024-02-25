#include "TestSDF2D.h"
#include "DebugTools.h"
#include "imgui/imgui.h"

namespace Test
{
	TestSDF2D::TestSDF2D(const std::string& test_name)
        : m_model_world_coordinates{ 0.0f,0.0f,0.0f },
        TestBase(test_name)
	{
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        GLCall(glBlendEquation(GL_FUNC_ADD));

        float vertices[] =
        {
            //  position:       uv:
                0.0f,0.0f,      0.0f,0.0f,    // vertex 1
                640.0f,0.0f,    1.0f,0.0f,    // vertex 2
                640.0f,480.0f,  1.0f,1.0f,    // vertex 3
                0.0f, 480.0f,   0.0f,1.0f     // vertex 4
        };

        unsigned int indices[] =    // OpenGL requires index buffer to store unsigned data!!!
        {
            0,1,2,  // lower-half triangle for the rectangle
            0,2,3   // upper-half triangle for the rectangle
        };

        m_vao = std::make_unique<VAO>();

        m_vbo = std::make_unique<VBO>(vertices, 4 * 4 * sizeof(float));

        VBOLayout vbo_layout;
        // position:
        vbo_layout.AddAttribute<float>(2);
        // uv coordinates: (we will draw the texture in shader)
        vbo_layout.AddAttribute<float>(2);

        m_vao->LinkVertexBuffer(*m_vbo, vbo_layout);

        m_index_buffer = std::make_unique<IndexBuffer>(indices, 6);

        m_view_matrix = glm::translate(glm::mat4{ 1.0f }, glm::vec3{ -320.0f,-240.0f,0.0f });
        m_projection_matrix = glm::ortho(-320.0f, 320.0f, -240.0f, 240.0f, -100.0f, 100.0f); // aspect ratio = 4:3, as we are rendering 640 by 480
        m_model_matrix = glm::translate(glm::mat4{ 1.0f }, m_model_world_coordinates);

        m_shader = std::make_unique<Shader>("res/shaders/SDF2D.shader");

        m_shader->Bind();
        m_shader->SetUniform_float_matrix_4_4("u_MVP", m_projection_matrix * m_view_matrix * m_model_matrix);
        m_shader->SetUniform_2floats("u_WindowResolution", 640 * 3.0f, 480 * 3.0f);    // TODO: right now this is hard-coded
	}

	TestSDF2D::~TestSDF2D()
	{
        // recover the default settings:
        GLCall(glBlendFunc(GL_ONE, GL_ZERO));
        GLCall(glBlendEquation(GL_FUNC_ADD));
        /*
        Remember to disable blending with glDisable(GL_BLEND) when it's no longer needed, as leaving it enabled when drawing opaque objects
        can unnecessarily hurt performance.
        */
        GLCall(glDisable(GL_BLEND));
	}

	void TestSDF2D::OnUpdate(float dt)
	{
        //m_shader->Bind();
        m_time_passed += 1.0f / ImGui::GetIO().Framerate;
        m_shader->SetUniform_1float("u_TimePassed", m_time_passed);
        m_shader->SetUniform_1float("u_SignedDistance", m_signed_distance);
        m_shader->SetUniform_1int("u_Soft", (int)m_soft);
        m_shader->SetUniform_1float("u_MovableCircleX", m_movable_circle_x);
	}

	void TestSDF2D::OnRender()
	{
        m_renderer.Draw(*m_vao, *m_index_buffer, *m_shader);
	}

	void TestSDF2D::OnImGuiRender()
	{
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        ImGui::SliderFloat("Movable Circle x-value", &m_movable_circle_x, -800.0f, 800.0f);
        ImGui::SliderFloat("Signed Distance", &m_signed_distance, -250.0f, 800.0f);
        ImGui::Checkbox("Soft Merge", &m_soft);
	}
}
