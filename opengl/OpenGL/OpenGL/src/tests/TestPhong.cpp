#include "TestPhong.h"
#include "DebugTools.h"
#include "imgui/imgui.h"
#include <OBJ_Loader.h>

namespace Test
{
	TestPhong::TestPhong(const std::string& test_name)
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
        m_count = loaded_mesh.Vertices.size();
        float* arr = new float[m_count * (3 + 2 + 3) * sizeof(float)];
        const auto& v = loaded_mesh.Vertices;
        for (unsigned int i = 0; i < m_count; i++)
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

        m_vao = std::make_unique<VAO>();

        m_vbo = std::make_unique<VBO>(arr, m_count * (3 + 2 + 3) * sizeof(float));

        VBOLayout vbo_layout;
        // position:
        vbo_layout.AddAttribute<float>(3);
        // texture coordinates:
        vbo_layout.AddAttribute<float>(2);
        // normal:
        vbo_layout.AddAttribute<float>(3);

        m_vao->LinkVertexBuffer(*m_vbo, vbo_layout);

        m_view_matrix = glm::translate(glm::mat4{ 1.0f }, glm::vec3{ 0.0f,0.0f,0.0f });

        m_shader = std::make_unique<Shader>("res/shaders/Phong.shader");

        m_shader->Bind();
        m_shader->SetUniform_4floats("u_CameraWorldPosition", 0.0f, 0.0f, 0.0f, 1.0f);  // right now this is hard-coded

        delete[] arr;
	}

	TestPhong::~TestPhong()
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

	void TestPhong::OnUpdate(float dt)
	{
        m_projection_matrix = glm::perspective(glm::radians(m_fovy), 4.0f / 3.0f, 0.1f, 100.0f); // aspect ratio = 4:3
        m_model_matrix = glm::translate(glm::mat4{ 1.0f }, m_model_world_coordinates) * glm::rotate(glm::mat4{ 1.0f }, glm::radians(m_y_rotation), glm::vec3(0, 1, 0));
        m_shader->SetUniform_float_matrix_4_4("u_MVP", m_projection_matrix * m_view_matrix * m_model_matrix);
        m_shader->SetUniform_float_matrix_4_4("u_Model", m_model_matrix);
        m_shader->SetUniform_1float("u_Shininess", m_shininess);
        m_shader->SetUniform_1int("u_Blinn", (int)m_blinn);

        time_passed += 1.0f / ImGui::GetIO().Framerate;
        m_shader->SetUniform_1float("u_TimePassed", time_passed);
	}

	void TestPhong::OnRender()
	{
        m_renderer.LinearDraw(*m_vao, *m_shader, m_count);
	}

	void TestPhong::OnImGuiRender()
	{
        ImGui::SliderFloat("FOVy", &m_fovy, 1.0f, 179.0f);
        ImGui::SliderFloat("Y axis Rotation", &m_y_rotation, 0.0f, 360.0f);
        ImGui::SliderFloat("Shininess", &m_shininess, 2.0f, 50.0f);

        ImGui::Checkbox("Blinn Model", &m_blinn);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}
