#include "TestSmoothstepTexture.h"
#include "DebugTools.h"
#include "imgui/imgui.h"

namespace Test
{
	TestSmoothstepTexture::TestSmoothstepTexture(const std::string& test_name)
        : m_model_world_coordinates{ 320.0f,240.0f,0.0f },
        TestBase(test_name)
	{
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        GLCall(glBlendEquation(GL_FUNC_ADD));

        float vertices[] =
        {
            //  position:            texCoords:
                -250.0f,-166.75f,    0.0f,0.0f,    // vertex 1
                 250.0f,-166.75f,    1.0f,0.0f,    // vertex 2
                 250.0f, 166.75f,    1.0f,1.0f,    // vertex 3
                -250.0f, 166.75f,    0.0f,1.0f     // vertex 4
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
        // texture coordinates:
        vbo_layout.AddAttribute<float>(2);

        m_vao->LinkVertexBuffer(*m_vbo, vbo_layout);

        m_index_buffer = std::make_unique<IndexBuffer>(indices, 6);

        m_view_matrix = glm::translate(glm::mat4{ 1.0f }, glm::vec3{ -320.0f,-240.0f,0.0f });
        m_projection_matrix = glm::ortho(-320.0f, 320.0f, -240.0f, 240.0f, -100.0f, 100.0f); // aspect ratio = 4:3, as we are rendering 640 by 480
        m_model_matrix = glm::translate(glm::mat4{ 1.0f }, m_model_world_coordinates);
        m_mvp_matrix = m_projection_matrix * m_view_matrix * m_model_matrix;

        m_shader = std::make_unique<Shader>("res/shaders/SmoothstepTexture.shader");

        m_Texture = std::make_unique<Texture>("res/textures/squirrel.png");
        m_Texture->Bind(0);

        m_shader->Bind();
        m_shader->SetUniform_1int("u_Texture", 0); // uniform sampler2D is 1 int corresponding to the texture unit (slot)
        m_shader->SetUniform_1int("u_SmoothstepOn", 0);
        m_shader->SetUniform_float_matrix_4_4("u_MVP", m_mvp_matrix);

        // Careful: if we write m_Texture.Unbind(); here, u_Texture will be linked to the default texture object with ID 0!
	}

	TestSmoothstepTexture::~TestSmoothstepTexture()
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

	void TestSmoothstepTexture::OnUpdate(float dt)
	{

	}

	void TestSmoothstepTexture::OnRender()
	{
        m_renderer.Draw(*m_vao, *m_index_buffer, *m_shader);
	}

	void TestSmoothstepTexture::OnImGuiRender()
	{
        if (ImGui::Button("smoothstep"))
        {
            m_smoothstep_on = !m_smoothstep_on;
            if (m_smoothstep_on)
            {
                m_shader->SetUniform_1int("u_SmoothstepOn", 1);
            }
            else
            {
                m_shader->SetUniform_1int("u_SmoothstepOn", 0);
            }
        }

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}
