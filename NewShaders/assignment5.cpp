#include "assignment5.hpp"
#include "parametric_shapes.hpp"

//#include "random_sphere.hpp"
#include <vector>
#include "config.hpp"
#include "core/Bonobo.h"
#include "core/FPSCamera.h"
#include "core/helpers.hpp"
#include "core/Log.h"
#include "core/LogView.h"
#include "core/Misc.h"
#include "core/ShaderProgramManager.hpp"
#include "core/node.hpp"
#include <stdlib.h>

#include <imgui.h>
#include <external/imgui_impl_glfw_gl3.h>
#include <tinyfiledialogs.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdexcept>

bool testSphereSphere(glm::vec4 p1, float r1, glm::vec4 p2, float r2) {
	float r = r1 + r2;
	float p = distance(p1, p2);
	if (p < r) {
		return true;
	}
	return false;
}



edaf80::Assignment5::Assignment5() :
	mCamera(0.5f * glm::half_pi<float>(),
		static_cast<float>(config::resolution_x) / static_cast<float>(config::resolution_y),
		0.01f, 1000.0f),
	inputHandler(), mWindowManager(), window(nullptr)
{
	Log::View::Init();

	WindowManager::WindowDatum window_datum{ inputHandler, mCamera, config::resolution_x, config::resolution_y, 0, 0, 0, 0 };

	window = mWindowManager.CreateWindow("EDAF80: Assignment 5", window_datum, config::msaa_rate);
	if (window == nullptr) {
		Log::View::Destroy();
		throw std::runtime_error("Failed to get a window: aborting!");
	}
}

edaf80::Assignment5::~Assignment5()
{
	Log::View::Destroy();
}

void
edaf80::Assignment5::run()
{
	// Set up the camera
	mCamera.mWorld.SetTranslate(glm::vec3(0.0f, 0.0f, 6.0f));
	mCamera.mMouseSensitivity = 0.003f;
	mCamera.mMovementSpeed = 0.025;

	// Create the shader programs
	ShaderProgramManager program_manager;
	GLuint fallback_shader = 0u;
	program_manager.CreateAndRegisterProgram({ { ShaderType::vertex, "EDAF80/fallback.vert" },
	{ ShaderType::fragment, "EDAF80/fallback.frag" } },
		fallback_shader);
	if (fallback_shader == 0u) {
		LogError("Failed to load fallback shader");
		return;
	}

	GLuint cube_shader = 0u;
	program_manager.CreateAndRegisterProgram({ { ShaderType::vertex, "EDAF80/cube.vert" },
	{ ShaderType::fragment, "EDAF80/cube.frag" } },
		cube_shader);
	if (cube_shader == 0u)
		LogError("Failed to load cube shader");

	GLuint normal_shader = 0u;
	program_manager.CreateAndRegisterProgram({ { ShaderType::vertex, "EDAF80/normal.vert" },
	{ ShaderType::fragment, "EDAF80/normal.frag" } },
		normal_shader);
	if (normal_shader == 0u)
		LogError("Failed to load normal shader");

	GLuint sphere_shader = 0u;
	program_manager.CreateAndRegisterProgram({ { ShaderType::vertex, "EDAF80/sphere.vert" },
	{ ShaderType::fragment, "EDAF80/sphere.frag" } },
		sphere_shader);
	if (sphere_shader == 0u)
		LogError("Failed to load sphere shader");

	auto const light_position = glm::vec3(-2.0f, 4.0f, 2.0f);
	// The initial radius of the sphere of the player.
	auto r1 = 0.3f;
	auto const set_uniforms = [&light_position, &r1](GLuint program) {
		glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
		glUniform1f(glGetUniformLocation(program, "player_radius"), r1);
	};

	//
	// Todo: Insert the creation of other shader programs.
	//       (Check how it was done in assignment 3.)
	//

	//
	// Todo: Load your geometry
	//
	auto const shape = parametric_shapes::createSphere(60u, 60u, 1.0f);

	if (shape.vao == 0u)
		return;

	Node player_sphere;
	player_sphere.set_geometry(shape);
	player_sphere.set_scaling(glm::vec3(r1, r1, r1));
	player_sphere.set_program(&fallback_shader, set_uniforms);

	Node background_sphere;
	background_sphere.set_geometry(shape);
	auto my_cube_map_id = bonobo::loadTextureCubeMap("cloudyhills/posx.png", "cloudyhills/negx.png", "cloudyhills/posy.png", "cloudyhills/negy.png", "cloudyhills/posz.png", "cloudyhills/negz.png", true);
	background_sphere.add_texture("my_cube_map", my_cube_map_id, GL_TEXTURE_CUBE_MAP);
	background_sphere.set_program(&cube_shader, set_uniforms);
	float s = 500;
	background_sphere.set_scaling(glm::vec3(s, s, s));

	float points = 0.0f;

	struct rsphere {
		Node sphere;
		float radius;
	};

	rsphere other_sphere;
	std::vector<rsphere> sphere_list;
	srand(time(NULL));
	for (int i = 1; i < 10; i++) {
		float j = rand() % 10 - 5;
		float k = rand() % 10 - 5;
		float l = rand() % 10 - 5;
		other_sphere.sphere.set_geometry(shape);
		other_sphere.radius = (rand() % 10 * r1) * 0.1;
		other_sphere.sphere.set_scaling(glm::vec3(other_sphere.radius, other_sphere.radius, other_sphere.radius));
		other_sphere.sphere.set_translation(glm::vec3(j, k, l));
		other_sphere.sphere.set_program(&sphere_shader, set_uniforms);
		sphere_list.push_back(other_sphere);
	}


	glEnable(GL_DEPTH_TEST);

	// Enable face culling to improve performance:
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);
	//glCullFace(GL_BACK);


	f64 ddeltatime;
	size_t fpsSamples = 0;
	double nowTime, lastTime = GetTimeMilliseconds();
	double fpsNextTick = lastTime + 1000.0;

	bool show_logs = true;
	bool show_gui = true;
	bool shader_reload_failed = false;

	/* mCamera.mWorld.GetFront //riktning kamera
	mCamera.mWorld.GetTranslate //var kameran är */

	/*float x = 0.0;
	float y = 0.0;
	float z = 0.0;*/

	float speed = 0.01f;

	while (!glfwWindowShouldClose(window)) {
		nowTime = GetTimeMilliseconds();
		ddeltatime = nowTime - lastTime;
		if (nowTime > fpsNextTick) {
			fpsNextTick += 1000.0;
			fpsSamples = 0;
		}
		fpsSamples++;

		auto& io = ImGui::GetIO();
		inputHandler.SetUICapture(io.WantCaptureMouse, io.WantCaptureKeyboard);

		glfwPollEvents();
		inputHandler.Advance();
		mCamera.Update(ddeltatime, inputHandler);

		glm::vec3 move(mCamera.mWorld.GetTranslation() + mCamera.mWorld.GetFront() * 5.0f + mCamera.mWorld.GetDown());
		player_sphere.set_translation(move);

		//kamera +=  riktningsvektor * (hastighet * ddeltatime/1000.0)
		//rikningsvektor = front


		//mCamera.mWorld.Translate(mCamera.mWorld.GetFront() * (speed * (ddeltatime * 0.0001f)));
		mCamera.mWorld.Translate((mCamera.mWorld.GetFront() * glm::vec3(speed)));
		//*(ddeltatime * 0.0001f)

		if (inputHandler.GetKeycodeState(GLFW_KEY_F3) & JUST_RELEASED)
			show_logs = !show_logs;
		if (inputHandler.GetKeycodeState(GLFW_KEY_F2) & JUST_RELEASED)
			show_gui = !show_gui;
		if (inputHandler.GetKeycodeState(GLFW_KEY_R) & JUST_PRESSED) {
			shader_reload_failed = !program_manager.ReloadAllPrograms();
			if (shader_reload_failed)
				tinyfd_notifyPopup("Shader Program Reload Error",
					"An error occurred while reloading shader programs; see the logs for details.\n"
					"Rendering is suspended until the issue is solved. Once fixed, just reload the shaders again.",
					"error");
		}

		ImGui_ImplGlfwGL3_NewFrame();

		//
		// Todo: If you need to handle inputs, you can do it here
		//

	/*	if (inputHandler.GetKeycodeState(GLFW_KEY_LEFT) & PRESSED) {
			x = -speed;
			y = 0;
			z = 0;
		}

		if (inputHandler.GetKeycodeState(GLFW_KEY_RIGHT) & PRESSED) {
			x = speed;
			y = 0;
			z = 0;
		}

		if (inputHandler.GetKeycodeState(GLFW_KEY_UP) & PRESSED) {
			x = 0;
			y = speed;
			z = 0;
		}

		if (inputHandler.GetKeycodeState(GLFW_KEY_DOWN) & PRESSED) {
			x = 0;
			y = -speed;
			z = 0;
		}

		if (inputHandler.GetKeycodeState(GLFW_KEY_Z) & PRESSED) {
			x = 0;
			y = 0;
			z = -speed;
		}

		if (inputHandler.GetKeycodeState(GLFW_KEY_X) & PRESSED) {
			x = 0;
			y = 0;
			z = speed;
		} */

		//Check if collision
		float r2;
		glm::vec4 p1 = glm::column(player_sphere.get_transform(), 3);
		glm::vec4 p2;
		bool gameover = false;
		for (auto& temp : sphere_list) {
			r2 = temp.radius;
			p2 = glm::column(temp.sphere.get_transform(), 3);

			if (testSphereSphere(p1, r1, p2, r2)) {
				if (r1 > r2) { //yay :D
					float j = rand() % 10 - 5;
					float k = rand() % 10 - 5;
					float l = rand() % 10 - 5;
					temp.sphere.set_translation(glm::vec3(j, k, l));
					points += r2 * 10;
					printf("Points: %f\n", points);
					r1 += 0.1;
					temp.radius = (rand() % 20 * r1) * 0.1;
					player_sphere.set_scaling(glm::vec3(r1, r1, r1));
					temp.sphere.set_scaling(glm::vec3(temp.radius, temp.radius, temp.radius));
					speed += 0.005;
				}
				else { //nooes :(
					printf("GAME OVER\nFinal score: %f\n", points);
					gameover = true;
					break;
				}
			}
		}
		if (gameover) {
			r1 = 0.3;
			player_sphere.set_scaling(glm::vec3(r1, r1, r1));
			player_sphere.set_translation(glm::vec3(0, 0, 0));

			points = 0;
			speed = 0.01;

			mCamera.mWorld.SetTranslate(glm::vec3(0.0f, 0.0f, 0.0f));

			for (auto& temp : sphere_list) {
				float j = rand() % 10 - 5;
				float k = rand() % 10 - 5;
				float l = rand() % 10 - 5;
				temp.radius = (rand() % 10 * r1) * 0.1;
				temp.sphere.set_scaling(glm::vec3(temp.radius, temp.radius, temp.radius));
				temp.sphere.set_translation(glm::vec3(j, k, l));
			}

			gameover = false;
		}

		int framebuffer_width, framebuffer_height;
		glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);
		glViewport(0, 0, framebuffer_width, framebuffer_height);
		glClearDepthf(1.0f);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


		if (!shader_reload_failed) {
			//
			// Todo: Render all your geometry here.
			//
			player_sphere.render(mCamera.GetWorldToClipMatrix(), player_sphere.get_transform());
			background_sphere.render(mCamera.GetWorldToClipMatrix(), background_sphere.get_transform());
			//other_sphere.render(mCamera.GetWorldToClipMatrix(), other_sphere.get_transform());

			for (auto it = sphere_list.cbegin(); it != sphere_list.cend(); it++) {
				rsphere temp = *it;
				temp.sphere.render(mCamera.GetWorldToClipMatrix(), temp.sphere.get_transform());
			}


		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//
		// Todo: If you want a custom ImGUI window, you can set it up
		//       here
		//

		bool const opened = ImGui::Begin("Score", nullptr, ImVec2(300, 100), -1.0f, 0);
		if (opened) {
			ImGui::Text("%.1f", points);
		}
		ImGui::End();

		if (show_logs)
			Log::View::Render();
		if (show_gui)
			ImGui::Render();

		glfwSwapBuffers(window);
		lastTime = nowTime;
	}
}

int main()
{
	Bonobo::Init();
	try {
		edaf80::Assignment5 assignment5;
		assignment5.run();
	}
	catch (std::runtime_error const& e) {
		LogError(e.what());
	}
	Bonobo::Destroy();
}
