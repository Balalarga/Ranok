#pragma once

#include <glm/glm.hpp>


class Camera
{
public:
	Camera(float fov, float aspect, float near, float far);

	void MoveForward(float distance);
	void MoveRight(float distance);

	void Rotate(glm::vec3 delta);

	glm::mat4 ViewProjectionMatrix() const;

	void SetAspect(float aspect);


private:
	float _fov;
	float _aspect;
	float _near;
	float _far;

	glm::vec3 _forwardVector;
	glm::vec3 _rightVector;
	glm::vec3 _location;
};
