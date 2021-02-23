#include "Camera.h"

Camera::Camera(float screen_width, float screen_height)
	:screen_width(screen_width), screen_height(screen_height)
{

}

glm::mat4 Camera::getProjectionMatrix()
{
	return glm::perspective(glm::radians(45.0f), screen_width / screen_height, 0.1f, 100.0f);
}

glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(pos, pos + front, up);
}