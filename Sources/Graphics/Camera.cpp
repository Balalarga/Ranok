#include "Camera.h"


#include <glm/gtc/matrix_transform.hpp>


Camera::Camera(float fov, float aspect, float near, float far):
	_fov(fov),
	_aspect(aspect),
	_near(near),
	_far(far),
	_forwardVector({0, 0, 1}),
	_rightVector({0, 1, 0}),
	_location(0.f)
{

}

void Camera::MoveForward(float distance)
{
	_location.z += distance;
	// _location += normalize(_forwardVector) * distance;
}

void Camera::MoveRight(float distance)
{
	_location.x += distance;
	// _location += normalize(_rightVector) * distance;
}

void Camera::Rotate(glm::vec3 delta)
{
	_forwardVector += delta;
	_rightVector += delta;
}

glm::mat4 Camera::ViewProjectionMatrix() const
{
	glm::mat4 m(1.f);
	m = translate(m, _location);
	m = rotate(m, _forwardVector.x, {1, 0, 0});
	m = rotate(m, _forwardVector.y, {0, 1, 0});
	m = rotate(m, _forwardVector.z, {0, 0, 1});
	return glm::perspective(_fov, _aspect, _near, _far) * m;
}

void Camera::SetAspect(float aspect)
{
	_aspect = aspect;
}
