#include "TestGrid.h"
#include "DebugTools.h"
#include "imgui/imgui.h"

namespace Test
{
	TestGrid::TestGrid(const std::string& test_name)
        : m_model_world_coordinates{ 320.0f,240.0f,0.0f },
        TestBase(test_name)
	{
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        GLCall(glBlendEquation(GL_FUNC_ADD));

        float vertices[] =
        {
            //  position:            uv:
                -250.0f,-230.0f,    0.0f,0.0f,    // vertex 1
                 250.0f,-230.0f,    1.0f,0.0f,    // vertex 2
                 250.0f, 230.0f,    1.0f,1.0f,    // vertex 3
                -250.0f, 230.0f,    0.0f,1.0f     // vertex 4
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
        m_mvp_matrix = m_projection_matrix * m_view_matrix * m_model_matrix;

        m_shader = std::make_unique<Shader>("res/shaders/Grid.shader");

        m_shader->Bind();
        m_shader->SetUniform_float_matrix_4_4("u_MVP", m_mvp_matrix);

	}

	TestGrid::~TestGrid()
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

	void TestGrid::OnUpdate(float dt)
	{

	}

	void TestGrid::OnRender()
	{
        m_renderer.Draw(*m_vao, *m_index_buffer, *m_shader);
	}

	void TestGrid::OnImGuiRender()
	{
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}
