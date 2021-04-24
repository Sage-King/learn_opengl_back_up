
//Need to update equation to not assume v2 equals zero
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind/bind.hpp>
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <filesystem>
#include <map>
#include <random>
#include <ctime>
#include <algorithm>

#include <Shader.h>
#include <Camera.h>
#include <Quad.h>
#include <sage_net_common/message.h>
#include <Circle.h>
#include "Physics_Circle.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void error_callback(int error_code, const char* error_message);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void GLAPIENTRY message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}
template<typename T>
void print(T printable)
{
	std::cout << std::to_string(printable) << '\n';
}
void print(std::string printable)
{
	std::cout << printable << '\n';
}

int32_t float_to_int(float f)
{
	int32_t r;
	memcpy(&r, &f, sizeof(float));
	return r;
}

//from https://bitbashing.io/comparing-floats.html
int32_t ulps_distance(const float a, const float b)
{
	if (a == b) return 0;

	const auto max = std::numeric_limits<int32_t>::max();

	if (isnan(a) || isnan(b)) return max;
	if (isinf(a) || isinf(b)) return max;

	int32_t ia = float_to_int(a);
	int32_t ib = float_to_int(b);

	if ((ia < 0) != (ib < 0)) return max;
	int32_t distance = ia - ib;
	if (distance < 0) distance = -distance;
	return distance;
}

std::pair<float, float> solve_quadratic_equation(float a, float b, float c)
{
	float discriminant = (float)((pow(b, 2)) - (4 * a * c));
	if (discriminant > 0.001f)
	{
		float solution1, solution2;
		float sqrt_term = sqrt(discriminant);
		float temp = 0.0f;
		temp = -b + sqrt_term;
		temp /= (2 * a);
		solution1 = temp;
		temp = -b - sqrt_term;
		temp /= (2 * a);
		solution2 = temp;
		return std::pair<float, float>(solution1, solution2);
	}
	if (fabs(discriminant) <= 0.001f)
	{
		float solution1;
		float sqrt_term = sqrt(discriminant);
		float temp = 0.0f;
		temp = -b + sqrt_term;
		temp /= (2 * a);
		solution1 = temp;

		return std::pair<float, float>(solution1, std::numeric_limits<float>::max());
	}
	if (discriminant < -0.001f)
	{
		return std::pair<float, float>(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	}
	return std::pair<float, float>(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
}

constexpr float SCREEN_WIDTH = 600;
constexpr float SCREEN_HEIGHT = 600;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec4 background_color(0.0f, 0.1f, 0.1f, 1.0f);

int main()
{
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
	{
		std::cout << "GLFW ERROR:: glfw failed to initialize" << '\n';
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow((int)SCREEN_WIDTH, (int)SCREEN_HEIGHT, "Sage Learning OpenGL", NULL, NULL);
	glfwSetWindowPos(window, 800, 180);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	if (!gladLoadGL())
	{
		printf("Something went wrong!\n");
		exit(-1);
	}
	//printf("OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(message_callback, (void*)0);
	glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_OTHER, GL_DEBUG_SEVERITY_NOTIFICATION, 0, 0, GL_FALSE);

	GLFWimage window_icon[1];

	int window_icon_width, window_icon_height, window_icon_nrChannels;

	std::string window_icon_filepath = std::filesystem::current_path().string() + "\\resources\\server_icon.png";
	//std::cout << window_icon_filepath << '\n';
	unsigned char* window_icon_data = stbi_load(window_icon_filepath.c_str(), &window_icon_width, &window_icon_height, &window_icon_nrChannels, 0);

	if (window_icon_data)
	{

		window_icon[0].width = window_icon_width;
		window_icon[0].height = window_icon_height;
		window_icon[0].pixels = window_icon_data;
	}
	else
	{
		std::cout << "Failed to load window icon" << std::endl;
	}

	glfwSetWindowIcon(window, 1, window_icon);

	stbi_image_free(window_icon_data);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, (int)SCREEN_WIDTH, (int)SCREEN_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	Sage::Physics_Circle test_circle = Sage::Physics_Circle(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.1f, 0.1f), 0.1f);
	Sage::Physics_Circle test_circle2 = Sage::Physics_Circle(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), glm::vec2(0.5f, 0.5f), 0.1f);
	test_circle.vel = glm::vec2(0.4f, 0.6f);
	test_circle2.vel = glm::vec2(0.0f, 0.0f);
	test_circle.mass = 10.0f;
	test_circle2.mass = 1.0f;
	bool rate_limited = false;
	float last_rate_limit = 0.0f;
	lastFrame = (float)glfwGetTime();
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////START FRAME RENDERING/////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(background_color.x, background_color.y , background_color.z, background_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		
		if (test_circle.circle.get_pos().x >= 1.0f - test_circle.circle.get_radius() ||
			test_circle.circle.get_pos().x <= 0.0f + test_circle.circle.get_radius())
		{
			test_circle.vel.x = -test_circle.vel.x;
		}
		if (test_circle2.circle.get_pos().x >= 1.0f - test_circle2.circle.get_radius() ||
			test_circle2.circle.get_pos().x <= 0.0f + test_circle2.circle.get_radius())
		{
			test_circle2.vel.x = -test_circle2.vel.x;
		}

		if (test_circle.circle.get_pos().y >= 1.0f - test_circle.circle.get_radius() ||
			test_circle.circle.get_pos().y <= 0.0f + test_circle.circle.get_radius())
		{
			test_circle.vel.y = -test_circle.vel.y;
		}
		if (test_circle2.circle.get_pos().y >= 1.0f - test_circle2.circle.get_radius() ||
			test_circle2.circle.get_pos().y <= 0.0f + test_circle2.circle.get_radius())
		{
			test_circle2.vel.y = -test_circle2.vel.y;
		}
		
		if ((float)glfwGetTime() - last_rate_limit  >= 0.5f)
		{
			rate_limited = false;
		}


		if (test_circle.circle.is_intersecting(test_circle2.circle) && !rate_limited)
		{
			//from the bottom of this page http://www.sciencecalculators.org/mechanics/collisions/
			glm::vec2 test_circle_vel_before = test_circle.vel;
			glm::vec2 test_circle2_vel_before = test_circle2.vel;

			float temp = 2.0f * test_circle2.mass;
			float temp2 = test_circle.mass + test_circle2.mass;
			temp /= temp2;

			glm::vec2 term1 = test_circle.vel - temp;

			glm::vec2 temp3 = test_circle.vel - test_circle2.vel;
			glm::vec2 temp4 = test_circle.circle.get_pos() - test_circle2.circle.get_pos();

			temp = glm::dot(temp3, temp4);
			
			temp3 = test_circle.circle.get_pos() - test_circle2.circle.get_pos();
			temp2 = glm::length(temp3);
			temp2 *= temp2;

			float term2 = temp / temp2;
			
			glm::vec2 term3 = test_circle.circle.get_pos() - test_circle2.circle.get_pos();

			test_circle.vel = term1 * term2 * term3;

			temp = 2.0f * test_circle.mass;
			temp2 = test_circle.mass + test_circle2.mass;
			temp /= temp2;
			
			term1 = test_circle2.vel - temp;

			temp3 = test_circle2.vel - test_circle.vel;
			temp4 = test_circle2.circle.get_pos() - test_circle.circle.get_pos();

			temp = glm::dot(temp3, temp4);

			temp3 = test_circle2.circle.get_pos() - test_circle.circle.get_pos();
			temp2 = glm::length(temp3);
			temp2 *= temp2;

			term2 = temp / temp2;

			term3 = test_circle2.circle.get_pos() - test_circle.circle.get_pos();

			test_circle2.vel = term1 * term2 * term3;
			
			rate_limited = true;
			last_rate_limit = (float)glfwGetTime();
		}

		test_circle.update(deltaTime);
		test_circle2.update(deltaTime);
		test_circle.draw();
		test_circle2.draw();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();

	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void error_callback(int error_code, const char* error_message)
{
	std::cout << "[ERROR] GLFW ERROR - Error code: " << error_code << "|| Error message: " << error_message << '\n';
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
}
