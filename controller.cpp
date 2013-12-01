#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "controller.h"

controller* controller::INSTANCE;

void mouseButtonCallback(GLFWwindow*, int button, int action, int mods) {
   controller::instance()->mouseButtonCallback(button, action, mods);
}

void scrollCallback(GLFWwindow*, double xoffset, double yoffset) {
   controller::instance()->scrollCallback(xoffset, yoffset);
}

void keyCallback(GLFWwindow*, int key, int scancode, int action, int mods) {
   controller::instance()->keyCallback(key, scancode, action, mods);
}

void cursorPosCallback(GLFWwindow*, double xpos, double ypos) {
   controller::instance()->cursorPosCallback(xpos, ypos);
}

controller::controller(GLFWwindow* window): _window(window) {
}

void controller::init(GLFWwindow* window) {
   if(INSTANCE) return;
   glfwSetMouseButtonCallback(window, ::mouseButtonCallback);
   glfwSetScrollCallback(window, ::scrollCallback);
   glfwSetKeyCallback(window, ::keyCallback);
   glfwSetCursorPosCallback(window, ::cursorPosCallback);
   INSTANCE = new controller(window);
}

void controller::deinit() {
   if(!INSTANCE) return;
   glfwSetMouseButtonCallback(INSTANCE->_window, nullptr);
   glfwSetScrollCallback(INSTANCE->_window, nullptr);
   glfwSetKeyCallback(INSTANCE->_window, nullptr);
   glfwSetCursorPosCallback(INSTANCE->_window, nullptr);
   delete INSTANCE;
}

void controller::mouseButtonCallback(int button, int action, int mods) {
   if(_mouse_button_callback)
      _mouse_button_callback(button, action, mods);
}

void controller::scrollCallback(double xoffset, double yoffset) {
   if(_scroll_callback)
      _scroll_callback(xoffset, yoffset);
}

void controller::keyCallback(int key, int scancode, int action, int mods) {
   if(_key_callback)
      _key_callback(key, scancode, action, mods);
}

void controller::cursorPosCallback(double xpos, double ypos) {
   if(_cursor_pos_callback)
      _cursor_pos_callback(xpos, ypos);
}
