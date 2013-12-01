#pragma once

#include <string>
#include <functional>

#include <GL/glew.h>

struct GLFWwindow;

struct gl_context {
   gl_context(int width, int height);
   ~gl_context();
   void main_loop(std::function<void()> body) const;
   GLuint add_shader(std::string const& vertex_file_path,
                   std::string const& fragment_file_path) const;
   GLFWwindow* main_window() const { return _main_window; }
   void use_shader(GLuint shader_id) const;
   void remove_shader(GLuint shader_id) const;
private:
   void init_glfw() const;
   void create_glfw_window(int width, int height);
   void init_glew() const;
   int compile_shader(std::string const& shader_path, GLuint shader_id) const;
private:
   GLuint _vertex_array_id;
   GLFWwindow* _main_window;
};
