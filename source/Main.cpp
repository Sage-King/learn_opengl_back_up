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

#include <Shader.h>
#include <Camera.h>
#include <quad.h>
#include <sage_net_common/message.h>

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
void reset();
void handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred);
constexpr float SCREEN_WIDTH = 600;
constexpr float SCREEN_HEIGHT = 600;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Quad paddle_one, paddle_two, ball;

glm::vec3 clear_color = glm::vec3(0.0f, 0.1f, 0.1f);
float paddle_one_speed, paddle_two_speed;
Game_State local_game_state;
std::chrono::steady_clock::time_point send_rate_limit_last;

boost::asio::io_context io_context;
boost::array<Sage_Message, 1> receive_buffer;
boost::asio::ip::udp::endpoint sender_endpoint;
boost::system::error_code ec;
boost::asio::ip::udp::socket our_socket(io_context);

unsigned int rally = 0;
bool rate_limit = false;
double last_limit_time = 0.0;
unsigned int player_number;
bool connecting = true;
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
	if (!gladLoadGL()) {
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
	////////////////////////////////////////////////////////////////INIT GAME///////////////////////////////////////////////////
	std::string ip_address_input;
	boost::asio::ip::udp::resolver resolver(io_context);
	boost::asio::ip::udp::endpoint server_endpoint;
	our_socket = boost::asio::ip::udp::socket(io_context);

	/*std::cout << "Enter IP to connect to: ";
	std::cin >> ip_address_input;*/
	ip_address_input = "96.19.83.18";
	server_endpoint = *resolver.resolve(boost::asio::ip::udp::v4(), ip_address_input.c_str(), "daytime").begin();
	our_socket.open(boost::asio::ip::udp::v4());
	our_socket.native_non_blocking(true);
	our_socket.non_blocking(true);

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
	glm::vec2 ball_speed = glm::vec2(0.00f,0.0f);
	last_limit_time = glfwGetTime();
	glfwFocusWindow(window);
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
		glClearColor(background_color.x, background_color.y, background_color.z, background_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		///////////////////////////////networking///////////////////////////////

		if (connecting)
		{
			boost::array<Sage_Message, 1> connect_request;
			connect_request[0].message_type = CLIENT_CONNECT;
			our_socket.send_to(boost::asio::buffer(connect_request), server_endpoint, 0, ec);
			connecting = false;
		}
		our_socket.receive_from(boost::asio::buffer(receive_buffer), sender_endpoint,0, ec);

		if (!(ec == boost::asio::error::would_block))
		{
			switch (receive_buffer[0].message_type)
			{
			case CLIENT_TYPE:
				memcpy(&player_number, &receive_buffer[0].payload, sizeof(player_number));
				std::cout << "Waiting for other player to connect...\n";
				break;
			case GAME_STARTED:
				std::cout << "Game started!\n";
				background_color = glm::vec4(0.0f, 0.1f, 0.1f, 1.0f);
				local_game_state.game_started = true;
				break;
			case GAME_STATE:
				memcpy(&local_game_state, &receive_buffer[0].payload, sizeof(Game_State));
				ball.x = local_game_state.ball_x;
				ball.y = local_game_state.ball_y;
				paddle_one.x = local_game_state.paddle_one_x;
				paddle_two.x = local_game_state.paddle_two_x;
				if (player_number == 1)
					paddle_two_speed = local_game_state.paddle_two_vel;
				if (player_number == 2)
					paddle_one_speed = local_game_state.paddle_one_vel;
				ball_speed.x = local_game_state.ball_x_vel;
				ball_speed.y = local_game_state.ball_y_vel;

				boost::array<Sage_Message, 1> player_input;
				player_input[0].message_type = PLAYER_INPUT;
				memcpy(&player_input[0].payload[0], &player_number, sizeof(player_number));
				if (player_number == 1)
				{
					memcpy(&player_input[0].payload[sizeof(player_number)], &paddle_one_speed, sizeof(paddle_one_speed));
				}
				if (player_number == 2)
				{
					memcpy(&player_input[0].payload[sizeof(player_number)], &paddle_two_speed, sizeof(paddle_two_speed));
				}
				our_socket.send_to(boost::asio::buffer(player_input), server_endpoint, 0, ec);

				break;
			case GAME_ENDED:
				memcpy(&local_game_state, &receive_buffer[0].payload[0], sizeof(Game_State));
				reset();
				break;
			default:
				break;
			}
		}
		///////////////////////////////////////CLIENT PREDICTION///////////////////////////////
		//if (local_game_state.game_started)
		//{
		//	auto time_since_last_limit = std::chrono::steady_clock::now() - send_rate_limit_last;
		//	if (time_since_last_limit >= std::chrono::milliseconds(100))
		//	{
		//		boost::array<Sage_Message, 1> player_input;
		//		player_input[0].message_type = PLAYER_INPUT;
		//		memcpy(&player_input[0].payload[0], &player_number, sizeof(player_number));
		//		if (player_number == 1)
		//		{
		//			memcpy(&player_input[0].payload[sizeof(player_number)], &paddle_one_speed, sizeof(paddle_one_speed));
		//		}
		//		if (player_number == 2)
		//		{
		//			memcpy(&player_input[0].payload[sizeof(player_number)], &paddle_two_speed, sizeof(paddle_two_speed));
		//		}
		//		our_socket.send_to(boost::asio::buffer(player_input), server_endpoint, 0, ec);
		//		send_rate_limit_last = std::chrono::steady_clock::now();
		//	}
		//}
		//boost::array<Sage_Message, 1> send_buf;
		//send_buf[0].message_type = STATE_QUERY;
		//if (player_number == 1)
		//{
		//	memcpy(&send_buf[0].payload, &paddle_one_speed, sizeof(paddle_one_speed));
		//	memcpy(&send_buf[0].payload[sizeof(paddle_one_speed)], &player_number, sizeof(player_number));
		//}
		//if (player_number == 2)
		//{
		//	memcpy(&send_buf[0].payload, &paddle_two_speed, sizeof(paddle_two_speed));
		//	memcpy(&send_buf[0].payload[sizeof(paddle_two_speed)], &player_number, sizeof(player_number));
		//}
		//boost::system::error_code ec;
		//our_socket.send_to(boost::asio::buffer(send_buf), server_endpoint, 0, ec);

		//size_t len;
		//if (ec == boost::asio::error::would_block)
		//	goto START_DRAWING;
		//

		//boost::array<Sage_Message, 1> recv_buf;
		//len = socket.receive_from(boost::asio::buffer(recv_buf), this_client_endpoint, 0, ec); //i think this is server_endpoint

		//if (ec == boost::asio::error::would_block)
		//	goto START_DRAWING;

		//if (recv_buf[0].message_type == STATE_RESPONSE)
		//{
		//	memcpy(&paddle_one.x, &recv_buf[0].payload[0 * sizeof(float)], sizeof(float));
		//	memcpy(&paddle_two.x, &recv_buf[0].payload[1 * sizeof(float)], sizeof(float));
		//	memcpy(&ball.x,		  &recv_buf[0].payload[2 * sizeof(float)], sizeof(float));
		//	memcpy(&ball.y,		  &recv_buf[0].payload[3 * sizeof(float)], sizeof(float));
		//}
		//if (recv_buf[0].message_type == GAME_OVER)
		//{
		//	bool green_won = false, white_won = false;
		//	memcpy(&green_won, &recv_buf[0].payload, sizeof(green_won));
		//	memcpy(&white_won, &recv_buf[0].payload[sizeof(green_won)], sizeof(white_won));
		//	if (green_won)
		//	{
		//		std::cout << "GREEN WON!\n";
		//		system("pause");
		//		return 0;
		//	}
		//	else if (white_won)
		//	{
		//		std::cout << "WHITE WON!\n";
		//		system("pause");
		//		return 0;
		//	}
		//}
		//
		/////////////////////////////////game logic//////////////////////////////
		//network side now
 		/*if (reset)
		{
			rally = 0;
			std::cout << "----NEW RALLY----\n";
			rate_limit = false;
			ball.x = 0.5f;
			ball.y = 0.5f;
			ball_speed = glm::vec2(0.05f, (r2 * 0.5) + 0.2f);
			reset = false;
		}
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
			ball.y = 1.0f - (paddle_one.getHeight() + ball.getHeight());
			ball_speed.x += paddle_one_speed;
			std::cout << "Rally: " << ++rally << '\n';
		}

		if (ball.isIntersecting(paddle_two) && !rate_limit)
		{
			rate_limit = true;
			last_limit_time = glfwGetTime();
			ball_speed.y = -ball_speed.y;
			ball.y = (paddle_two.getHeight()+ ball.getHeight());
			ball_speed.x += paddle_two_speed;
			std::cout << "Rally: " << ++rally << '\n';
		}

		if (ball.y > 1.2f)
		{
			std::cout << "Green Won! Rally count: " << rally << '\n';
		}

		if (ball.y < -0.2f)
		{
			std::cout << "White Won! Rally count: " << rally << '\n';
		}*/

		///////////////////////////draw///////////////////////////////////////
		paddle_one.draw();
		paddle_two.draw();
		ball.draw();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	boost::array<Sage_Message, 1> disconnect;
	disconnect[0].message_type = PLAYER_DISCONNECT;
	our_socket.send_to(boost::asio::buffer(disconnect), server_endpoint);
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
		//paddle_one.x -= 1.0f * deltaTime;
		if (player_number == 1)
		{
			paddle_one_speed = -1.0f;
		}
		if (player_number == 2)
		{
			paddle_two_speed = -1.0f;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE)
	{
		if (player_number == 1)
		{
			paddle_one_speed = 0.0f;
		}
		if (player_number == 2)
		{
			paddle_two_speed = 0.0f;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		//paddle_one.x += 1.0f * deltaTime;
		if (player_number == 1)
		{
			paddle_one_speed = 1.0f;
		}
		if (player_number == 2)
		{
			paddle_two_speed = 1.0f;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE)
	{
		//paddle_one_speed = 0.0f;
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

void reset()
{
	if (!local_game_state.green_won && !local_game_state.white_won)
	{
		std::cout << "SOMEONE DISCONNECTED\n";
	}else if (local_game_state.green_won)
	{
		background_color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		std::cout << "GREEN WON\n";
	}else if (local_game_state.white_won)
	{
		background_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		std::cout << "WHITE WON\n";
	}
	connecting = true;
}

void handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred)
{
 	
}