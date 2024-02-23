#include "TestRipplingSphere.h"
#include "DebugTools.h"
#include "imgui/imgui.h"
#include <OBJ_Loader.h>

namespace Test
{
    TestRipplingSphere::TestRipplingSphere(const std::string& test_name)
        : m_model_world_coordinates{ 0.0f,0.0f,-5.0f },
        TestBase(test_name)
	{
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        GLCall(glBlendEquation(GL_FUNC_ADD));

        GLCall(glEnable(GL_DEPTH_TEST));
        GLCall(glDepthFunc(GL_LESS));

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

        objl::Loader Robert_Smith_Loader;
        Robert_Smith_Loader.LoadFile("res/meshes/sphere.obj");
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

        m_skybox_vao = std::make_unique<VAO>();
        m_model_vao = std::make_unique<VAO>();

        m_skybox_vbo = std::make_unique<VBO>(skyboxVertices, 3 * (3 * 2 * 6) * sizeof(float));
        m_model_vbo = std::make_unique<VBO>(arr, m_model_vertices_count * (3 + 2 + 3) * sizeof(float));

        {
            VBOLayout vbo_layout;
            // local position:
            vbo_layout.AddAttribute<float>(3);

            m_skybox_vao->LinkVertexBuffer(*m_skybox_vbo, vbo_layout);
        }

        {
            VBOLayout vbo_layout;
            // position:
            vbo_layout.AddAttribute<float>(3);
            // texture coordinates:
            vbo_layout.AddAttribute<float>(2);
            // normal:
            vbo_layout.AddAttribute<float>(3);

            m_model_vao->LinkVertexBuffer(*m_model_vbo, vbo_layout);
        }
        
        m_skybox_shader = std::make_unique<Shader>("res/shaders/Skybox.shader");
        m_model_shader = std::make_unique<Shader>("res/shaders/RipplingSphere.shader");

        m_Cubemap = std::make_unique<Cubemap>(
            "res/textures/skybox_RiverMountain/+X.png",
            "res/textures/skybox_RiverMountain/-X.png",
            "res/textures/skybox_RiverMountain/+Y.png",
            "res/textures/skybox_RiverMountain/-Y.png",
            "res/textures/skybox_RiverMountain/+Z.png",
            "res/textures/skybox_RiverMountain/-Z.png"
            /*"res/textures/skybox_SkyCloud/+X.png",
            "res/textures/skybox_SkyCloud/-X.png",
            "res/textures/skybox_SkyCloud/+Y.png",
            "res/textures/skybox_SkyCloud/-Y.png",
            "res/textures/skybox_SkyCloud/+Z.png",
            "res/textures/skybox_SkyCloud/-Z.png"*/
        );
        m_Cubemap->Bind(0);
        m_skybox_shader->Bind();
        m_skybox_shader->SetUniform_1int("u_Cubemap", 0);
        m_model_shader->Bind();
        m_model_shader->SetUniform_1int("u_Cubemap", 0);

        delete[] arr;
	}

	TestRipplingSphere::~TestRipplingSphere()
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

	void TestRipplingSphere::OnUpdate(float dt)
	{
        m_model_shader->Bind();
        if (!m_pause)
        {
            m_time_passed += 1.0f / ImGui::GetIO().Framerate;
        }
        m_model_shader->SetUniform_1float("u_TimePassed", m_time_passed);
	}

	void TestRipplingSphere::OnRender()
	{
        SkyboxPass();
        ModelPass();
	}

	void TestRipplingSphere::OnImGuiRender()
	{
        ImGui::SliderFloat("FOVy", &m_fovy, 1.0f, 179.0f);
        ImGui::SliderFloat("Camera Horizontal Rotation", &m_camera_horizontal_rotation, -180.0f, 180.0f);
        ImGui::SliderFloat("Camera Vertical Rotation", &m_camera_vertical_rotation, -89.0f, 89.0f);
        ImGui::SliderFloat("Model X axis Rotation", &m_model_x_rotation, 0.0f, 360.0f);
        ImGui::SliderFloat("Model Shininess", &m_model_shininess, 2.0f, 50.0f);
        
        ImGui::Checkbox("Fresnel Effect", &m_fresnel_effect_on);
        ImGui::Checkbox("Use Screen Space Derivative to Calculate World Normal", &m_screen_derivative_normal);
        ImGui::Checkbox("Pause", &m_pause);
        
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

    void TestRipplingSphere::SkyboxPass()
    {
        GLCall(glDepthMask(GL_FALSE));

        m_projection_matrix = glm::perspective(glm::radians(m_fovy), 4.0f / 3.0f, 0.1f, 100.0f); // aspect ratio = 4:3
        m_view_matrix = glm::mat4(glm::mat3(
            glm::rotate(glm::mat4{ 1.0f }, -glm::radians(m_camera_vertical_rotation), glm::vec3(1, 0, 0)) *
            glm::rotate(glm::mat4{ 1.0f }, glm::radians(m_camera_horizontal_rotation), glm::vec3(0, 1, 0))
        ));
        m_skybox_shader->Bind();
        m_skybox_shader->SetUniform_float_matrix_4_4("u_ProjectionMatrix", m_projection_matrix);
        m_skybox_shader->SetUniform_float_matrix_4_4("u_RotationOnlyViewMatrix", m_view_matrix);

        m_renderer.LinearDraw(*m_skybox_vao, *m_skybox_shader, 36);   // 3 * 2 * 6

        GLCall(glDepthMask(GL_TRUE));
    }

    void TestRipplingSphere::ModelPass()
    {
        m_view_matrix = 
            glm::rotate(glm::mat4{ 1.0f }, -glm::radians(m_camera_vertical_rotation), glm::vec3(1, 0, 0)) *
            glm::rotate(glm::mat4{ 1.0f }, glm::radians(m_camera_horizontal_rotation), glm::vec3(0, 1, 0));
        m_projection_matrix = glm::perspective(glm::radians(m_fovy), 4.0f / 3.0f, 0.1f, 100.0f); // aspect ratio = 4:3
        m_model_matrix = glm::translate(glm::mat4{ 1.0f }, m_model_world_coordinates) * glm::rotate(glm::mat4{ 1.0f }, glm::radians(m_model_x_rotation), glm::vec3(1, 0, 0));
        m_model_shader->Bind();
        m_model_shader->SetUniform_1int("u_FresnelEffect", (int)m_fresnel_effect_on);
        m_model_shader->SetUniform_1int("u_UseScreenSpaceDerivativeNormal", (int)m_screen_derivative_normal);
        m_model_shader->SetUniform_float_matrix_4_4("u_MVP", m_projection_matrix * m_view_matrix * m_model_matrix);
        m_model_shader->SetUniform_float_matrix_4_4("u_Model", m_model_matrix);
        m_model_shader->SetUniform_1float("u_Shininess", m_model_shininess);

        m_renderer.LinearDraw(*m_model_vao, *m_model_shader, m_model_vertices_count);
    }
}
