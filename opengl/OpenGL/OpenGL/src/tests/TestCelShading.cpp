#include "TestCelShading.h"
#include "DebugTools.h"
#include "imgui/imgui.h"
#include <OBJ_Loader.h>

namespace Test
{
    TestCelShading::TestCelShading(const std::string& test_name)
        : m_model_world_coordinates{ 0.0f,0.0f,-5.0f },
        TestBase(test_name)
	{
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        GLCall(glBlendEquation(GL_FUNC_ADD));

        GLCall(glEnable(GL_DEPTH_TEST));
        GLCall(glDepthFunc(GL_LESS));

        objl::Loader Robert_Smith_Loader;
        Robert_Smith_Loader.LoadFile("res/meshes/suzanne.obj");
        assert(Robert_Smith_Loader.LoadedMeshes.size() == 1);
        objl::Mesh loaded_mesh = Robert_Smith_Loader.LoadedMeshes[0];
        m_model_vertices_count = loaded_mesh.Vertices.size();
        float* arr = new float[m_model_vertices_count * (3 + 2 + 3) * sizeof(float)];
        const auto& v = loaded_mesh.Vertices;
        for (unsigned int i = 0; i < m_model_vertices_count; i++)
        {
            // position:
            arr[i * (3 + 2 + 3) + 0] = v[i].Position.X;
            arr[i * (3 + 2 + 3) + 1] = v[i].Position.Y;
            arr[i * (3 + 2 + 3) + 2] = v[i].Position.Z;
            // texture coordinates:
            arr[i * (3 + 2 + 3) + 3] = v[i].TextureCoordinate.X;
            arr[i * (3 + 2 + 3) + 4] = v[i].TextureCoordinate.Y;
            // normal:
            arr[i * (3 + 2 + 3) + 5] = v[i].Normal.X;
            arr[i * (3 + 2 + 3) + 6] = v[i].Normal.Y;
            arr[i * (3 + 2 + 3) + 7] = v[i].Normal.Z;
        }

        m_model_vao = std::make_unique<VAO>();

        m_model_vbo = std::make_unique<VBO>(arr, m_model_vertices_count * (3 + 2 + 3) * sizeof(float));

        VBOLayout vbo_layout;
        // position:
        vbo_layout.AddAttribute<float>(3);
        // texture coordinates:
        vbo_layout.AddAttribute<float>(2);
        // normal:
        vbo_layout.AddAttribute<float>(3);

        m_model_vao->LinkVertexBuffer(*m_model_vbo, vbo_layout);

        m_model_shader = std::make_unique<Shader>("res/shaders/CelShading.shader");

        m_view_matrix = glm::translate(glm::mat4{ 1.0f }, glm::vec3{ 0.0f,0.0f,0.0f });
	}

	TestCelShading::~TestCelShading()
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
	}

	void TestCelShading::OnUpdate(float dt)
	{
        m_model_shader->Bind();
        time_passed += 1.0f / ImGui::GetIO().Framerate;
        m_model_shader->SetUniform_1float("u_TimePassed", time_passed);
	}

	void TestCelShading::OnRender()
	{
        ModelPass();
	}

	void TestCelShading::OnImGuiRender()
	{
        ImGui::SliderFloat("FOVy", &m_fovy, 1.0f, 179.0f);
        ImGui::SliderFloat("Model X axis Rotation", &m_model_x_rotation, 0.0f, 360.0f);
        ImGui::SliderFloat("Model Y axis Rotation", &m_model_y_rotation, 0.0f, 360.0f);
        ImGui::SliderFloat("Model Shininess", &m_model_shininess, 2.0f, 50.0f);
        
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

    void TestCelShading::ModelPass()
    {
        m_projection_matrix = glm::perspective(glm::radians(m_fovy), 4.0f / 3.0f, 0.1f, 100.0f); // aspect ratio = 4:3
        m_model_matrix = glm::translate(glm::mat4{ 1.0f }, m_model_world_coordinates) * 
            glm::rotate(glm::mat4{ 1.0f }, glm::radians(m_model_y_rotation), glm::vec3(0, 1, 0)) *
            glm::rotate(glm::mat4{ 1.0f }, glm::radians(m_model_x_rotation), glm::vec3(1, 0, 0));
        m_model_shader->Bind();
        m_model_shader->SetUniform_float_matrix_4_4("u_MVP", m_projection_matrix * m_view_matrix * m_model_matrix);
        m_model_shader->SetUniform_float_matrix_4_4("u_Model", m_model_matrix);
        m_model_shader->SetUniform_1float("u_Shininess", m_model_shininess);

        m_renderer.LinearDraw(*m_model_vao, *m_model_shader, m_model_vertices_count);
    }
}
