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
template <typename T>
void println(T output);

constexpr float SCREEN_WIDTH = 600;
constexpr float SCREEN_HEIGHT = 600;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec2 circle_one_pos(0.5f,0.5f);
glm::vec2 circle_two_pos(0.0f);
float num = 10.0f;

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

	/*std::vector<Sage::Circle> circle_vector;
	for (int i = 0; i < 10; i++)
	{
		circle_vector.push_back(Sage::Circle(glm::vec4(1.0f,0.0f,0.1f * i,1.0f), glm::vec2((0.1f * i) + 0.05f, 0.0f), 0.1f));
	}*/

	Sage::Circle test_circle = Sage::Circle(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f), 0.1f);
	Sage::Circle test_circle_2 = Sage::Circle(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f), 0.1f);

	constexpr float gravity = 0.1f;
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

		/*for (std::vector<Sage::Circle>::iterator iter = circle_vector.begin();
			iter != circle_vector.end();
			iter++)
		{
			glm::vec2 temp_pos = iter->get_pos(); 
			temp_pos.y += gravity * deltaTime;
			iter->set_pos(temp_pos);
			iter->draw();
		}*/
		test_circle.set_pos_clamp_to_window(circle_one_pos);
		test_circle.set_num_of_points(num);
		test_circle.draw();

		float second_radius = test_circle_2.get_radius();
		circle_two_pos.x = std::clamp(circle_two_pos.x, 0.0f + second_radius, 1.0f - second_radius);
		circle_two_pos.y = std::clamp(circle_two_pos.y, 0.0f + second_radius, 1.0f - second_radius);
		test_circle_2.set_pos(circle_two_pos);
		//test_circle_2.draw();

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
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		circle_one_pos.x -= 0.1f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		circle_one_pos.x += 0.1f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		circle_one_pos.y -= 0.1f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		circle_one_pos.y += 0.1f * deltaTime;


	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		circle_two_pos.x -= 0.1f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		circle_two_pos.x += 0.1f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		circle_two_pos.y -= 0.1f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		circle_two_pos.y += 0.1f * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
		num += 1.0f;
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		num -= 1.0f;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
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
template <typename T>
void println(T output)
{
	std::cout << output << '\n';
}
