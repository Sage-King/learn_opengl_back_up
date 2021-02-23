
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

#include <Shader.h>
#include <Model.h>
#include <Window.h>

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

constexpr float SCREEN_WIDTH = 1920;
constexpr float SCREEN_HEIGHT = 1080;

float lastMouseX = SCREEN_WIDTH / 2;
float lastMouseY = SCREEN_HEIGHT / 2;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);
float pitch;
float yaw = -90.0f;

unsigned int VAO;
unsigned int VBO;
unsigned int EBO;
unsigned int diffuseMap;
unsigned int specularMap;
unsigned char* data;

unsigned int lightVAO;

glm::vec3 lightPos(1.2f, 3.0f, 2.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
glm::vec3 lightSize(0.5f,0.5f,0.5f);

glm::vec3 cubePos(0.0f, 0.0f, 0.0f);
glm::vec3 cubeDir(0.0f, 0.0f, 0.0f);
bool cubeFirst = true;
float cubeSubX = 0.0f, cubeSubY = 0.0f, cubeSubZ = 0.0f;
float cubeSpeed = 20.0f;

constexpr int numOfPointLights = 4;

bool dynamic = false;
bool line_mode = false;
bool firstMouse = true;

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
	printf("OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(message_callback, (void*)0);
	glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_OTHER, GL_DEBUG_SEVERITY_NOTIFICATION, 0, 0, GL_FALSE);

	GLFWimage window_icon[3];

	int window_icon_width, window_icon_height, window_icon_nrChannels;
	//std::string window_icon_filepath = std::filesystem::current_path().string() + "\\resources\\party_streamers_16_16.png";
	////std::cout << window_icon_filepath << '\n';
	//unsigned char* window_icon_data = stbi_load(window_icon_filepath.c_str(), &window_icon_width, &window_icon_height, &window_icon_nrChannels, 0);

	//if (window_icon_data)
	//{
 //
	//	window_icon[0].width = window_icon_width;
	//	window_icon[0].height = window_icon_height;
	//	window_icon[0].pixels = window_icon_data;
	//}
	//else
	//{
	//	std::cout << "Failed to load window icon" << std::endl;
	//}

	//window_icon_filepath = std::filesystem::current_path().string() + "\\resources\\party_streamers_32_32.png";
	////std::cout << window_icon_filepath << '\n';
	//window_icon_data = stbi_load(window_icon_filepath.c_str(), &window_icon_width, &window_icon_height, &window_icon_nrChannels, 0);
	//if (window_icon_data)
	//{

	//	window_icon[1].width = window_icon_width;
	//	window_icon[1].height = window_icon_height;
	//	window_icon[1].pixels = window_icon_data;
	//}
	//else
	//{
	//	std::cout << "Failed to load window icon" << std::endl;
	//}

	//window_icon_filepath = std::filesystem::current_path().string() + "\\resources\\party_streamers_48_48.png";
	////std::cout << window_icon_filepath << '\n';
	//window_icon_data = stbi_load(window_icon_filepath.c_str(), &window_icon_width, &window_icon_height, &window_icon_nrChannels, 0);
	//if (window_icon_data)
	//{

	//	window_icon[2].width = window_icon_width;
	//	window_icon[2].height = window_icon_height;
	//	window_icon[2].pixels = window_icon_data;
	//}
	//else
	//{
	//	std::cout << "Failed to load window icon" << std::endl;
	//}

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

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	float vertices[] = {
		// positions(3f) // normals(3f) // texture coords(2f)
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,		0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f,	0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
		0.5f, 0.5f, -0.5f,	0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
		-0.5f, 0.5f, -0.5f,		0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f,	0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f,	0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f,	0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		-0.5f, 0.5f, 0.5f,	0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f,	-1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, -0.5f,		-1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,	-1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,	-1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f,		-1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f,	-1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f,	1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f,	1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.5f,	 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.5f,	 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,	 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,	 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, 0.5f,	 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.5f,	 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.5f,		0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,	 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, -0.5f,	0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f,	0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f,	0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, 0.5f,	0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f, 0.0f, 1.0f
	};
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	unsigned int indices[] =
	{

		0,1,2,
		0,2,3,

		4,5,6,
		4,6,7,

		1,4,7,
		1,7,2,

		0,5,4,
		0,4,1,

		5,0,3,
		5,3,6,

		6,3,2,
		6,2,7
	};
	//textured cubes
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);					
													
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);					
													
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//light source
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	Shader ourShader("shaders\\shader.vs", "shaders\\shader.fs");
	Shader lightSourceShader("shaders\\lightSource.vs", "shaders\\lightSource.fs");

	glGenTextures(1, &diffuseMap);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	std::string filepath = std::filesystem::current_path().string() + "\\resources\\container2.png";
	data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	glGenTextures(1, &specularMap);
	glBindTexture(GL_TEXTURE_2D, specularMap);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	filepath = std::filesystem::current_path().string() + "\\resources\\container2_specular.png";
	data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	//unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
	unsigned int lightTransformLoc = glGetUniformLocation(lightSourceShader.ID, "transform");

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(2.0f, 5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f, 3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f, 2.0f, -2.5f),
		glm::vec3(1.5f, 0.2f, -1.5f),
		glm::vec3(-1.3f, 1.0f, -1.5f)
	};

	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f, 0.2f, 2.0f),
		glm::vec3(5.3f, 3.3f, -4.0f),
		glm::vec3(-4.0f, 2.0f, -12.0f),
		glm::vec3(0.0f, 0.0f, -3.0f)
	}; 

	Model backpack = Model("C:\\Learn_OpenGL_-_Joey_de_Vries\\Learn_OpenGL\\x64\\Debug\\resources\\backpack.obj");
	Shader backpackShader = Shader("shaders\\backpack.vs", "shaders\\backpack.fs");
	Shader solidColor = Shader("shaders\\backpack.vs", "shaders\\solidColor.fs");
	Window myWindow = Window();
	Shader windowShader = Shader("shaders\\window.vs", "shaders\\window.fs");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);

	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	unsigned int frame_buf_texture;
	glGenTextures(1, &frame_buf_texture);
	glBindTexture(GL_TEXTURE_2D, frame_buf_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)SCREEN_WIDTH, (GLsizei)SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frame_buf_texture, 0);

	unsigned int render_buf;
	glGenRenderbuffers(1, &render_buf);
	glBindRenderbuffer(GL_RENDERBUFFER, render_buf);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (GLsizei)SCREEN_WIDTH, (GLsizei)SCREEN_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, render_buf);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete\n";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	unsigned int quad_vao, quad_vbo;
	glGenVertexArrays(1, &quad_vao);
	glBindVertexArray(quad_vao);

	glGenBuffers(1, &quad_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
	float plane[] =
	{
		-1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f,-1.0f, 0.0f, 0.0f,
		 1.0f,-1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f,
		 1.0f,-1.0f, 1.0f, 0.0f,
		 1.0f, 1.0f, 1.0f, 1.0f
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(plane), plane, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	unsigned int backwards_framebuffer;
	glGenFramebuffers(1,&backwards_framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER,backwards_framebuffer);

	unsigned int backwards_framebuffer_texture;
	glGenTextures(1, &backwards_framebuffer_texture);
	glBindTexture(GL_TEXTURE_2D, backwards_framebuffer_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)SCREEN_WIDTH, (GLsizei)SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, backwards_framebuffer_texture, 0);

	unsigned int backwards_depth_and_stencil_renderbuffer;
	glGenRenderbuffers(1, &backwards_depth_and_stencil_renderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, backwards_depth_and_stencil_renderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (GLsizei)SCREEN_WIDTH, (GLsizei)SCREEN_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, backwards_depth_and_stencil_renderbuffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR:FRAMEBUFFER::Framebuffer not complete.\n";
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	unsigned int backwards_quad_vao, backwards_quad_vbo;
	glGenVertexArrays(1, &backwards_quad_vao);
	glBindVertexArray(backwards_quad_vao);
	
	glGenBuffers(1, &backwards_quad_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, backwards_quad_vbo);

	float backwards_plane[] =
	{
		-1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f,-1.0f, 0.0f, 0.0f,
		 1.0f,-1.0f, 1.0f, 0.0f,

		-1.0f, 1.0f, 0.0f, 1.0f,
		 1.0f,-1.0f, 1.0f, 0.0f,
		 1.0f, 1.0f, 1.0f, 1.0f
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(backwards_plane), backwards_plane, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,0);

	Shader screenShader("shaders\\screen_shader.vs", "shaders\\screen_shader.fs");
	Shader backwards_screen_shader("shaders\\backwards_screen_shader.vs", "shaders\\screen_shader.fs");

	unsigned int skybox_texture;
	glGenTextures(1, &skybox_texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture);

	int skybox_width, skybox_height, skybox_number_of_channels;
	unsigned char* skybox_data;
	std::vector<std::string> skybox_texture_faces
	{
		"\\resources\\skybox_right.jpg",
		"\\resources\\skybox_left.jpg",
		"\\resources\\skybox_top.jpg",
		"\\resources\\skybox_bottom.jpg",
		"\\resources\\skybox_front.jpg",
		"\\resources\\skybox_back.jpg"
	};
	for (unsigned int i = 0; i < skybox_texture_faces.size(); i++)
	{
		std::string skybox_filepath = std::filesystem::current_path().string() + skybox_texture_faces[i];
		skybox_data = stbi_load(skybox_filepath.c_str(), &skybox_width, &skybox_height, &skybox_number_of_channels, 0);
		std::cout << skybox_filepath << "\n";
		if (skybox_data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, skybox_width, skybox_height, 0, GL_RGB, GL_UNSIGNED_BYTE, skybox_data);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		}
		else
		{
			std::cout << "ERROR::STBI::Failed to load skybox texture\n";
		}
		stbi_image_free(skybox_data);
	}


	unsigned int skybox_vao;
	glGenVertexArrays(1, &skybox_vao);
	glBindVertexArray(skybox_vao);

	unsigned int skybox_vbo;
	glGenBuffers(1, &skybox_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo);
	float skybox_vertices[] =
	{
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, 0.5f, -0.5f,
		0.5f, 0.5f, -0.5f,
		-0.5f, 0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, 0.5f,
		0.5f, -0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f, 0.5f,
		-0.5f, -0.5f, 0.5f,
		-0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, 0.5f,
		-0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, 0.5f,
		0.5f, -0.5f, 0.5f,
		-0.5f, -0.5f, 0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, 0.5f, -0.5f,
		0.5f, 0.5f, -0.5f,
		0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f, -0.5f,
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), skybox_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	Shader skybox_shader("shaders\\skybox.vs", "shaders\\skybox.fs");

	glDepthFunc(GL_LEQUAL);

	unsigned int cube_vao;
	glGenVertexArrays(1, &cube_vao);
	glBindVertexArray(cube_vao);

	unsigned int cube_vbo;
	glGenBuffers(1, &cube_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);

	float cube_vertices[36*8] =
	{
	   //position (3f)    normal(3f)        texture(2f)
	   //positive z face
		-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,

		-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		 1.0f,-1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		-1.0f,-1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

	   //negative z face
		-1.0f, 1.0f, -1.0f, 0.0f, 0.0f,-1.0f, 0.0f, 1.0f,
		 1.0f,-1.0f, -1.0f, 0.0f, 0.0f,-1.0f, 1.0f, 0.0f,
		 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,-1.0f, 1.0f, 1.0f,
									
		-1.0f, 1.0f, -1.0f, 0.0f, 0.0f,-1.0f, 0.0f, 1.0f,
		-1.0f,-1.0f, -1.0f, 0.0f, 0.0f,-1.0f, 0.0f, 0.0f,
		 1.0f,-1.0f, -1.0f, 0.0f, 0.0f,-1.0f, 1.0f, 0.0f,
		
	   //positive x face
		 1.0f, 1.0f,-1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
						  
		 1.0f, 1.0f,-1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		 1.0f,-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		 1.0f,-1.0f,-1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

	   //negative x face
		-1.0f, 1.0f, -1.0f,-1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-1.0f,-1.0f,  1.0f,-1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-1.0f, 1.0f,  1.0f,-1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
									
		-1.0f, 1.0f, -1.0f,-1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-1.0f,-1.0f, -1.0f,-1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-1.0f,-1.0f,  1.0f,-1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

	   //positive y face
		 1.0f, 1.0f,-1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
								   
		 1.0f, 1.0f,-1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		-1.0f, 1.0f,-1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

	   //negative y face
		 1.0f, -1.0f,-1.0f, 0.0f,-1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 1.0f, 0.0f,-1.0f, 0.0f, 1.0f, 0.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,-1.0f, 0.0f, 1.0f, 1.0f,
									
		 1.0f, -1.0f,-1.0f, 0.0f,-1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f,-1.0f, 0.0f,-1.0f, 0.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, 1.0f, 0.0f,-1.0f, 0.0f, 1.0f, 0.0f,
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0 * sizeof(float)));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,0);

	Shader cube_shader("shaders\\cube_shader.vs", "shaders\\cube_shader.fs");
	
	unsigned int cube_color_vao;
	glGenVertexArrays(1,&cube_color_vao);
	glBindVertexArray(cube_color_vao);
	
	unsigned int cube_color_vbo;
	glGenBuffers(1, &cube_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, cube_color_vbo);
	float cube_color_vertices[36 * 3] =
	{
		//position (3f)    normal(3f)        texture(2f)
		//positive z face
		 -1.0f, 1.0f,  1.0f, 
		  1.0f, 1.0f,  1.0f, 
		  1.0f,-1.0f,  1.0f, 
					   
		 -1.0f, 1.0f,  1.0f, 
		  1.0f,-1.0f,  1.0f, 
		 -1.0f,-1.0f,  1.0f, 

		//negative z face
		 -1.0f, 1.0f, -1.0f, 
		  1.0f,-1.0f, -1.0f, 
		  1.0f, 1.0f, -1.0f, 

		 -1.0f, 1.0f, -1.0f, 
		 -1.0f,-1.0f, -1.0f, 
		  1.0f,-1.0f, -1.0f, 

		//positive x face
		  1.0f, 1.0f, -1.0f, 
		  1.0f, 1.0f,  1.0f, 
		  1.0f,-1.0f,  1.0f, 
		  			  
		  1.0f, 1.0f, -1.0f, 
		  1.0f,-1.0f,  1.0f, 
		  1.0f,-1.0f, -1.0f, 

		//negative x face
		 -1.0f, 1.0f, -1.0f,
		 -1.0f,-1.0f,  1.0f,
		 -1.0f, 1.0f,  1.0f,
		 
		 -1.0f, 1.0f, -1.0f,
		 -1.0f,-1.0f, -1.0f,
		 -1.0f,-1.0f,  1.0f,

		//positive y face
       	  1.0f, 1.0f, -1.0f, 
       	  1.0f, 1.0f,  1.0f, 
         -1.0f, 1.0f,  1.0f, 
       				  
       	  1.0f, 1.0f, -1.0f, 
         -1.0f, 1.0f,  1.0f, 
         -1.0f, 1.0f, -1.0f, 

		//negative y face
		  1.0f, -1.0f, -1.0f,
		 -1.0f, -1.0f,  1.0f,
		  1.0f, -1.0f,  1.0f,
		 			   
		  1.0f, -1.0f, -1.0f,
		 -1.0f, -1.0f, -1.0f,
		 -1.0f, -1.0f,  1.0f,
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_color_vertices), cube_color_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	Shader cube_red("shaders\\cube_color_shader.vs", "shaders\\cube_red.fs");
	Shader cube_green("shaders\\cube_color_shader.vs", "shaders\\cube_green.fs");
	Shader cube_blue("shaders\\cube_color_shader.vs", "shaders\\cube_blue.fs");
	Shader cube_yellow("shaders\\cube_color_shader.vs", "shaders\\cube_yellow.fs");

	unsigned int red = glGetUniformBlockIndex(cube_red.ID, "matrices");
	unsigned int green = glGetUniformBlockIndex(cube_green.ID, "matrices");
	unsigned int blue = glGetUniformBlockIndex(cube_blue.ID, "matrices");
	unsigned int yellow = glGetUniformBlockIndex(cube_yellow.ID, "matrices");

	glUniformBlockBinding(cube_red.ID, red, 0);
	glUniformBlockBinding(cube_green.ID, green, 0);
	glUniformBlockBinding(cube_blue.ID, blue, 0);
	glUniformBlockBinding(cube_yellow.ID, yellow, 0);

	unsigned int ubo_matrices;
	glGenBuffers(1, &ubo_matrices);

	glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo_matrices, 0, 2 * sizeof(glm::mat4));

	cube_red.use();
	glm::mat4 model(1.0f);
	model = glm::translate(model, glm::vec3(3.0f, 0.0f, 0.0f));
	cube_red.setMat4("model", model);

	cube_green.use();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0f));
	cube_green.setMat4("model", model);

	cube_blue.use();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
	cube_blue.setMat4("model", model);

	cube_yellow.use();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0f));
	cube_yellow.setMat4("model", model);

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
		glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);
		const float radius = 10.0f;
		float camX = sin((float)glfwGetTime()) * radius;
		float camZ = cos((float)glfwGetTime()) * radius;
		glm::mat4 view = glm::mat4(1.0f);
		glm::vec3 cameraPosPlusCameraFront = cameraPos + cameraFront;
		view = glm::lookAt(cameraPos, cameraPosPlusCameraFront, cameraUp);

		glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		//colored_cubes
		cube_red.use();
		glBindVertexArray(cube_color_vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		cube_green.use();
		glBindVertexArray(cube_color_vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		cube_blue.use();
		glBindVertexArray(cube_color_vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		cube_yellow.use();
		glBindVertexArray(cube_color_vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glDepthMask(GL_FALSE);
		skybox_shader.use();
		glm::mat4 skybox_view = glm::mat4(glm::mat3(view));
		glm::mat4 skybox_MVP = projection * skybox_view;
		skybox_shader.setMat4("MVP_transform", skybox_MVP);
		glBindVertexArray(skybox_vao);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(GL_TRUE);

		glEnable(GL_DEPTH_TEST);

		glm::vec3 pointlightPos;
		glm::vec3 pointlightColor = glm::vec3(1.0f,1.0f,1.0f);
		glm::vec3 pointlightSize = glm::vec3(0.2f, 0.2f, 0.2f);
		const float lightRadius = 15.0f;
		float lightX = sin((float)glfwGetTime())* lightRadius;
		float lightZ = cos((float)glfwGetTime()) * lightRadius;
		pointlightPos.x = lightX;
		pointlightPos.z = lightZ;
		pointlightPos.y = 5.0f;

		lightSourceShader.use();
		lightSourceShader.setVec3("lightColor", pointlightColor);
		//lightSourceShader.setVec3("lightColor", 1.0f,1.0f,1.0f);
		glm::mat4 lightModel = glm::mat4(1.0f);
		glm::mat4 MVP(1.0f);
		lightModel = glm::scale(lightModel, pointlightSize);
		lightModel = glm::translate(lightModel, pointlightPos);
		MVP = projection * view * lightModel;
		glUniformMatrix4fv(lightTransformLoc, 1, GL_FALSE, glm::value_ptr(MVP));
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		if (!cubeFirst)
		{
			cube_shader.use();
			cube_shader.setVec3("lightColor", glm::vec3(0.4f, 0.0f, 0.4f));
			glm::mat4 cubeModelToWorld = glm::mat4(1.0f);
			cubePos += cubeDir * cubeSpeed * deltaTime;
			cubeModelToWorld = glm::translate(cubeModelToWorld, cubePos);
			cubeModelToWorld = glm::scale(cubeModelToWorld, glm::vec3(0.5f, 0.5f, 0.5f));
			MVP = projection * view * cubeModelToWorld;
			cube_shader.setMat4("transform", MVP);
			cube_shader.setMat4("modelToWorldTransform", cubeModelToWorld);
			glBindVertexArray(lightVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		}
		backpackShader.use();
		backpackShader.setVec3("pointLight.position", pointlightPos);

		backpackShader.setVec3("pointLight.ambient", pointlightColor * 0.2f);
		backpackShader.setVec3("pointLight.diffuse", pointlightColor);
		backpackShader.setVec3("pointLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

		backpackShader.setFloat("pointLight.constant", 1.0f); 
		backpackShader.setFloat("pointLight.linear", 0.022f);
		backpackShader.setFloat("pointLight.quadratic", 0.0019f);

		glm::vec3 flashlightColor = glm::vec3(0.0f, 1.0f, 1.0f);
		backpackShader.setVec3("flashlight.position", cameraPos);
		backpackShader.setVec3("flashlight.direction", cameraFront);
		backpackShader.setFloat("flashlight.innerCutOffAngle", glm::cos(glm::radians(12.5f)));
		backpackShader.setFloat("flashlight.outerCutOffAngle", glm::cos(glm::radians(17.5f)));

		backpackShader.setVec3("flashlight.ambient", flashlightColor * 0.01f);
		backpackShader.setVec3("flashlight.diffuse", flashlightColor);
		backpackShader.setVec3("flashlight.specular", flashlightColor);

		backpackShader.setFloat("flashlight.constant", 1.0f);
		backpackShader.setFloat("flashlight.linear", 0.07f);
		backpackShader.setFloat("flashlight.quadratic", 0.017f);

		backpackShader.setVec3("cameraPos", cameraPos);
		glm::mat4 model = glm::mat4(1.0f);
		//glm::mat4 MVP(1.0f);
		model = glm::translate(model, glm::vec3(0.0f,0.0f,0.0f));
		MVP = projection * view * model;
		backpackShader.setMat4("modelToWorldTransform",model);
		backpackShader.setMat4("transform", MVP);

		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		backpack.Draw(backpackShader);
		model = glm::mat4(1.0f);
		//glm::mat4 MVP(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0f));
		MVP = projection * view * model;
		backpackShader.setMat4("modelToWorldTransform", model);
		backpackShader.setMat4("transform", MVP);
		backpack.Draw(backpackShader);
		glStencilFunc(GL_NEVER, 1, 0xFF);

		solidColor.use();
		model = glm::mat4(1.0f);
		//glm::mat4 MVP(1.0f);
		model = glm::scale(model, glm::vec3(1.005f,1.005f,1.005f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		MVP = projection * view * model;
		solidColor.setMat4("modelToWorldTransform", model);
		solidColor.setMat4("transform", MVP);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		//backpack.Draw(solidColor);

		model = glm::mat4(1.0f);
		//glm::mat4 MVP(1.0f)
		model = glm::scale(model, glm::vec3(1.005f, 1.005f, 1.005f));
		model = glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0f));
		MVP = projection * view * model;
		solidColor.setMat4("modelToWorldTransform", model);
		solidColor.setMat4("transform", MVP);
		backpack.Draw(solidColor);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glDisable(GL_STENCIL_TEST);

		std::map<float, glm::vec3> sorted_by_depth;
		for (int i = 0; i < 10; i++)
		{
			float distance = glm::length(cubePositions[i] - cameraPos);
			sorted_by_depth[distance] = cubePositions[i];
		}

		for (std::map<float, glm::vec3>::reverse_iterator it = sorted_by_depth.rbegin(); it != sorted_by_depth.rend(); it++)
		{
			windowShader.use();
			model = glm::mat4(1.0f);
			MVP = glm::mat4(1.0f);
			model = glm::translate(model, it->second);
			MVP = projection * view * model;
			windowShader.setMat4("MVPtransform", MVP);
			myWindow.draw(windowShader);
		}

		//cube
		cube_shader.use();
		glBindVertexArray(cube_vao);

		//vertex shader info
		glm::mat4 cube_model_transform(1.0f);
		glm::mat4 cube_mvp_transform(1.0f);
		cube_mvp_transform = projection * view * cube_model_transform;
		cube_shader.setMat4("mvp_transform", cube_mvp_transform);
		cube_shader.setMat4("model_transform", cube_model_transform);

		//fragment shader info
		cube_shader.setVec3("camera_pos", cameraPos);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture);
		cube_shader.setInt("skybox", 0);

		//draw cube
		glDrawArrays(GL_TRIANGLES, 0, 36);
		//backpack
		backpack.Draw(cube_shader);

		////////////////////////////////////////////////////////////START BACKWARDS FRAME RENDERING////////////////////////////////////////////////////////
		glBindFramebuffer(GL_FRAMEBUFFER, backwards_framebuffer);

		glClearColor(0.0f, 0.25f, 0.25f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		if (dynamic)
		{
			view = glm::lookAt(
				glm::vec3(camX, camX * camZ, camZ),
				//glm::vec3(0.0f, 0.0f, 3.0f),
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f)
			);
		}
		else
		{
			glm::vec3 cameraPosPlusCameraFront = cameraPos - cameraFront;
			view = glm::lookAt(cameraPos, cameraPosPlusCameraFront, cameraUp);
		}
		glDepthMask(GL_FALSE);
		skybox_shader.use();
		skybox_view = glm::mat4(glm::mat3(view));
		skybox_MVP = projection * skybox_view;
		skybox_shader.setMat4("MVP_transform", skybox_MVP);
		glBindVertexArray(skybox_vao);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);

		pointlightPos.x = lightX;
		pointlightPos.z = lightZ;
		pointlightPos.y = 5.0f;

		lightSourceShader.use();
		lightSourceShader.setVec3("lightColor", pointlightColor);
		//lightSourceShader.setVec3("lightColor", 1.0f,1.0f,1.0f);
		lightModel = glm::scale(lightModel, pointlightSize);
		lightModel = glm::translate(lightModel, pointlightPos);
		MVP = projection * view * lightModel;
		glUniformMatrix4fv(lightTransformLoc, 1, GL_FALSE, glm::value_ptr(MVP));
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		if (!cubeFirst)
		{
			lightSourceShader.use();
			lightSourceShader.setVec3("lightColor", glm::vec3(0.4f, 0.0f, 0.4f));
			glm::mat4 cubeModelToWorld = glm::mat4(1.0f);
			cubePos += cubeDir * cubeSpeed * deltaTime;
			cubeModelToWorld = glm::translate(cubeModelToWorld, cubePos);
			cubeModelToWorld = glm::scale(cubeModelToWorld, glm::vec3(0.5f, 0.5f, 0.5f));
			MVP = projection * view * cubeModelToWorld;
			lightSourceShader.setMat4("transform", MVP);
			glBindVertexArray(lightVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		}
		backpackShader.use();
		backpackShader.setVec3("pointLight.position", pointlightPos);

		backpackShader.setVec3("pointLight.ambient", pointlightColor * 0.2f);
		backpackShader.setVec3("pointLight.diffuse", pointlightColor);
		backpackShader.setVec3("pointLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

		backpackShader.setFloat("pointLight.constant", 1.0f);
		backpackShader.setFloat("pointLight.linear", 0.022f);
		backpackShader.setFloat("pointLight.quadratic", 0.0019f);

		backpackShader.setVec3("flashlight.position", cameraPos);
		backpackShader.setVec3("flashlight.direction", cameraFront);
		backpackShader.setFloat("flashlight.innerCutOffAngle", glm::cos(glm::radians(12.5f)));
		backpackShader.setFloat("flashlight.outerCutOffAngle", glm::cos(glm::radians(17.5f)));

		backpackShader.setVec3("flashlight.ambient", flashlightColor * 0.01f);
		backpackShader.setVec3("flashlight.diffuse", flashlightColor);
		backpackShader.setVec3("flashlight.specular", flashlightColor);

		backpackShader.setFloat("flashlight.constant", 1.0f);
		backpackShader.setFloat("flashlight.linear", 0.07f);
		backpackShader.setFloat("flashlight.quadratic", 0.017f);

		backpackShader.setVec3("cameraPos", cameraPos);
		//glm::mat4 MVP(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		MVP = projection * view * model;
		backpackShader.setMat4("modelToWorldTransform", model);
		backpackShader.setMat4("transform", MVP);

		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		backpack.Draw(backpackShader);
		model = glm::mat4(1.0f);
		//glm::mat4 MVP(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0f));
		MVP = projection * view * model;
		backpackShader.setMat4("modelToWorldTransform", model);
		backpackShader.setMat4("transform", MVP);
		backpack.Draw(backpackShader);
		glStencilFunc(GL_NEVER, 1, 0xFF);

		solidColor.use();
		model = glm::mat4(1.0f);
		//glm::mat4 MVP(1.0f);
		model = glm::scale(model, glm::vec3(1.005f, 1.005f, 1.005f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		MVP = projection * view * model;
		solidColor.setMat4("modelToWorldTransform", model);
		solidColor.setMat4("transform", MVP);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		//backpack.Draw(solidColor);

		model = glm::mat4(1.0f);
		//glm::mat4 MVP(1.0f)
		model = glm::scale(model, glm::vec3(1.005f, 1.005f, 1.005f));
		model = glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0f));
		MVP = projection * view * model;
		solidColor.setMat4("modelToWorldTransform", model);
		solidColor.setMat4("transform", MVP);
		backpack.Draw(solidColor);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glDisable(GL_STENCIL_TEST);

		std::map<float, glm::vec3> sorted_by_depth_backwards;
		for (int i = 0; i < 10; i++)
		{
			float distance = glm::length(cubePositions[i] - cameraPos);
			sorted_by_depth_backwards[distance] = cubePositions[i];
		}

		for (std::map<float, glm::vec3>::reverse_iterator it = sorted_by_depth.rbegin(); it != sorted_by_depth.rend(); it++)
		{
			windowShader.use();
			model = glm::mat4(1.0f);
			MVP = glm::mat4(1.0f);
			model = glm::translate(model, it->second);
			MVP = projection * view * model;
			windowShader.setMat4("MVPtransform", MVP);
			myWindow.draw(windowShader);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		screenShader.use();
		glBindVertexArray(quad_vao);
		glBindTexture(GL_TEXTURE_2D, frame_buf_texture);
		screenShader.setInt("screen_texture", 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		backwards_screen_shader.use();
		glBindVertexArray(backwards_quad_vao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, backwards_framebuffer_texture);
		backwards_screen_shader.setInt("backwards_screen_texture", 0);
		constexpr float scale_factor = 0.25f;
		glm::mat4 backwards_transform(1.0f);
		backwards_transform = glm::translate(backwards_transform, glm::vec3((1.0f - ((SCREEN_WIDTH * scale_factor)/SCREEN_WIDTH)), (1.0f - ((SCREEN_HEIGHT * scale_factor) / SCREEN_HEIGHT)), 0.0f));
		backwards_transform = glm::scale(backwards_transform, glm::vec3(scale_factor, scale_factor, 0.0f));
		backwards_screen_shader.setMat4("transform", backwards_transform);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glEnable(GL_DEPTH_TEST);

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

	float cameraSpeed = 6.0f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		cameraPos += cameraFront * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		cameraPos -= cameraFront * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		cameraPos -= cameraRight * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		
		cameraPos += cameraRight * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		cameraPos -= cameraUp * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		cameraPos += cameraUp * cameraSpeed;
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		dynamic = !dynamic;
	}
	if (key == GLFW_KEY_G && action == GLFW_PRESS)
	{
		if (line_mode)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			line_mode = false;
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			line_mode = true;
		}

	}
	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
		cubeSubX += 1.0f;
	if (key == GLFW_KEY_H && action == GLFW_PRESS)
		cubeSubY += 1.0f;
	if (key == GLFW_KEY_N && action == GLFW_PRESS)
		cubeSubZ += 1.0f;
	if (key == GLFW_KEY_U && action == GLFW_PRESS)
		cubeSubX -= 1.0f;
	if (key == GLFW_KEY_J && action == GLFW_PRESS)
		cubeSubY -= 1.0f;
	if (key == GLFW_KEY_M && action == GLFW_PRESS)
		cubeSubZ -= 1.0f;
}

void error_callback(int error_code, const char* error_message)
{
	std::cout << "[ERROR] GLFW ERROR - Error code: " << error_code << "|| Error message: " << error_message << '\n';
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastMouseX = (float)xpos;
		lastMouseY = (float)ypos;
		firstMouse = false;
	}
	
	float xoffset = (float)xpos - lastMouseX;
	float yoffset = lastMouseY - (float)ypos;
	lastMouseX = (float)xpos;
	lastMouseY = (float)ypos;

	float sensitivity = 0.03f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	cameraFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront.y = sin(glm::radians(pitch));
	cameraFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(cameraFront);
	cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		cubeFirst = false;
		cubeDir = cameraFront;
		cubePos.x = cameraPos.x + cameraFront.x;
		cubePos.y = cameraPos.y + cameraFront.y;
		cubePos.z = cameraPos.z + cameraFront.z;
	}
}