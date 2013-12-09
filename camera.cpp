#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "camera.h"

camera::camera(GLFWwindow* window, float near, float far):
   _fov(45),
   _near(near),
   _far(far),
   _direction(0,0,1),
   _right(-1,0,0),
   _distance(10) {
   int w, h;
   glfwGetWindowSize(window, &w, &h);
   _width = w;
   _height = h;
   update_matrices();
}

void camera::update_matrices() {
   glm::vec3 position = _distance * _direction;
   glm::vec3 up = glm::cross(_right, _direction);
   glm::mat4 projection = glm::perspectiveFov(_fov, _width, _height, _near, _far);
   glm::mat4 view = glm::lookAt(position, glm::vec3(0,0,0), up);
   glm::mat4 model(1.0);
   _mvp = projection * view * model;
}

void camera::set_angles(float up_angle, float right_angle) {
   glm::vec3 up = glm::cross(_right, _direction);
   _direction = glm::rotate(glm::rotate(_direction, up_angle, up), right_angle, _right);
   _right = glm::rotate(glm::rotate(_right, up_angle, up), right_angle, _right);
}

void camera::zoom_in() {
   _distance = glm::max(_distance - 1, 0.1f);
}

void camera::zoom_out() {
   _distance = glm::min(_distance + 1, 1000.0f);
}

void send_matrix(GLuint id, glm::mat4 const& m) {
   glUniformMatrix4fv(id, 1, GL_FALSE, &m[0][0]);
}
