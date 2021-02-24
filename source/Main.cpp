
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

#include <Shader.h>
#include <Camera.h>
#include <quad.h>

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

constexpr float SCREEN_WIDTH = 600;
constexpr float SCREEN_HEIGHT = 600;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Quad paddle_one, paddle_two, ball;

glm::vec3 clear_color = glm::vec3(0.0f, 0.1f, 0.1f);
float paddle_one_speed, paddle_two_speed;

unsigned int rally = 0;
bool rate_limit = false;
double last_limit_time = 0.0;

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
	if (!gladLoadGL()) {
		printf("Something went wrong!\n");
		exit(-1);
	}
	//printf("OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(message_callback, (void*)0);
	glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_OTHER, GL_DEBUG_SEVERITY_NOTIFICATION, 0, 0, GL_FALSE);

	GLFWimage window_icon[3];

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

	glfwSetWindowIcon(window, 3, window_icon);

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
	////////////////////////////////////////////////////////////////INIT GAME///////////////////////////////////////////////////
	paddle_two = Quad(
		glm::vec3(0.0f, 1.0f, 0.0f), 
		0.2f,
		0.02f,
		0.5f,
		0.01f
	);

	paddle_one = Quad(
		glm::vec3(1.0f,1.0f,1.0f),
		0.2f,
		0.02f,
		0.5f,
		0.99f
	);

	ball = Quad(
		glm::vec3(1.0f, 1.0f, 1.0f),
		0.03f,
		0.03f,
		0.5f,
		0.5f
	);
	srand(static_cast <unsigned> (time(0)));
	float r2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	glm::vec2 ball_speed = glm::vec2(0.05f,(r2 * 0.5) + 0.2f);
	last_limit_time = glfwGetTime();
	glfwFocusWindow(window);
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
		glClearColor(0.0f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		/////////////////////////////////game logic//////////////////////////////
		ball.x = std::clamp(ball.x, (ball.getWidth() / 2), 1.0f - (ball.getWidth() / 2));
		if (ball.x >= 1.0f - (ball.getWidth() / 2) || ball.x <= (ball.getWidth() / 2))
		{
			ball_speed.x = -ball_speed.x;
		}
		ball_speed.x = std::clamp(ball_speed.x, -0.9f, 0.9f);
		ball.x += ball_speed.x * deltaTime;
		ball.y += ball_speed.y * deltaTime;

		if (glfwGetTime() - last_limit_time >= 0.2)
		{
			rate_limit = false;
		}

		if (ball.isIntersecting(paddle_one) && !rate_limit)
		{
			rate_limit = true;
			last_limit_time = glfwGetTime();
			ball_speed.y = -ball_speed.y;
			ball_speed.x += paddle_one_speed;
			std::cout << "Rally: " << ++rally << '\n';
		}

		if (ball.isIntersecting(paddle_two) && !rate_limit)
		{
			rate_limit = true;
			last_limit_time = glfwGetTime();
			ball_speed.y = -ball_speed.y;
			ball_speed.x += paddle_two_speed;
			std::cout << "Rally: " << ++rally << '\n';
		}

		if (ball.y > 1.0f)
		{
			std::cout << "Green Won! Rally count: " << rally << '\n';
		}

		if (ball.y < 0.0f)
		{
			std::cout << "White Won! Rally count: " << rally << '\n';

		}

		///////////////////////////draw///////////////////////////////////////
		paddle_one.draw();
		paddle_two.draw();
		ball.draw();

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
	{
		paddle_one.x -= 1.0f * deltaTime;
		paddle_one_speed = -1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE)
	{
		paddle_one_speed = 0.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		paddle_one.x += 1.0f * deltaTime;
		paddle_one_speed = 1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE)
	{
		paddle_one_speed = 0.0f;
	}

	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
	{
		paddle_two.x -= 1.0f * deltaTime;
		paddle_two_speed = -1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_RELEASE)
	{
		paddle_two_speed = 0.0f;
	}

	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		paddle_two.x += 1.0f * deltaTime;
		paddle_two_speed = 1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE)
	{
		paddle_two_speed = 0.0f;
	}
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