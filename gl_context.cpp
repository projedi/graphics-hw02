#include <iostream>
#include <fstream>
#include <stdexcept>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "gl_context.h"

gl_context::gl_context(int width, int height) {
   init_glfw();
   create_glfw_window(width, height);
   glfwMakeContextCurrent(_main_window);
   init_glew();
   glGenVertexArrays(1, &_vertex_array_id);
   glBindVertexArray(_vertex_array_id);
}

gl_context::~gl_context() {
   glfwDestroyWindow(_main_window);
   glfwTerminate();
   glDeleteVertexArrays(1, &_vertex_array_id);
}

GLuint gl_context::add_shader(std::string const& vertex_file_path,
                std::string const& fragment_file_path) const {
   GLuint vertex_id = glCreateShader(GL_VERTEX_SHADER);
   GLuint fragment_id = glCreateShader(GL_FRAGMENT_SHADER);

   compile_shader(vertex_file_path, vertex_id);
   compile_shader(fragment_file_path, fragment_id);

   // Link the program
   std::cout << "Linking program" << std::endl;
   GLuint shader_id = glCreateProgram();
   glAttachShader(shader_id, vertex_id);
   glAttachShader(shader_id, fragment_id);
   glLinkProgram(shader_id);

   // Check the program
   GLint result = GL_FALSE;
   int info_log_length;
   glGetProgramiv(shader_id, GL_LINK_STATUS, &result);
   glGetProgramiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
   if(info_log_length > 1) {
      std::string err_message;
      err_message.resize(info_log_length);
      glGetProgramInfoLog(shader_id, info_log_length, NULL, &err_message[0]);
      std::cerr << "SHADER ERROR: " << err_message << std::endl;
   }

   glDeleteShader(vertex_id);
   glDeleteShader(fragment_id);

   return shader_id;
}

void gl_context::init_glfw() const {
   glfwSetErrorCallback([](int code, const char* msg) {
         std::cerr << "GLFW Error: " << msg << std::endl;
   });
   if(!glfwInit())
      throw std::runtime_error("Failed to initialize GLFW");
}

void gl_context::create_glfw_window(int width, int height) {
   glfwWindowHint(GLFW_SAMPLES, 4);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

   if(!(_main_window = glfwCreateWindow(width, height, "HW01", NULL, NULL))) {
      glfwTerminate();
      throw std::runtime_error("Failed to open GLFW window");
   }
}

void gl_context::init_glew() const {
   glewExperimental = GL_TRUE;
   GLenum err = glewInit();
   auto glerr = glGetError();
   // Can get GL_INVALID_ENUM according to
   // http://stackoverflow.com/questions/10857335/opengl-glgeterror-returns-invalid-enum-after-call-to-glewinit
   if(glerr != GL_NO_ERROR && glerr != GL_INVALID_ENUM)
      std::cerr << "init_glew: GOT ERROR: " << glerr << std::endl;
   if(err != GLEW_OK)
      throw std::runtime_error("Failed to initialize GLEW: " +
            std::string((char*)glewGetErrorString(err)));
}

int gl_context::compile_shader(std::string const& shader_path, GLuint shader_id) const {
   std::string code;
   std::ifstream file(shader_path.c_str(), std::ios::in | std::ios::binary);
   if(!file) {
      std::cerr << "Impossible to open " << shader_path << std::endl;
      return -1;
   }
   file.seekg(0, std::ios::end);
   code.resize(file.tellg());
   file.seekg(0, std::ios::beg);
   file.read(&code[0], code.size());
   std::cout << "Compiling shader: " << shader_path << std::endl;
   char const* source_ptr = code.c_str();
   glShaderSource(shader_id, 1, &source_ptr , NULL);
   glCompileShader(shader_id);
   // Check Vertex Shader
   GLint result = GL_FALSE;
   int info_log_length;
   glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
   glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
   if(info_log_length > 1) {
      std::string err_message;
      err_message.resize(info_log_length);
      glGetShaderInfoLog(shader_id, info_log_length, NULL, &err_message[0]);
      std::cerr << err_message << std::endl;
   }
   return 0;
}

void gl_context::main_loop(std::function<void()> body) const {
   do {
      body();

      glfwSwapBuffers(_main_window);
      glfwPollEvents();
   } while(glfwGetKey(_main_window, GLFW_KEY_ESCAPE) != GLFW_PRESS
          && !glfwWindowShouldClose(_main_window));
}

void gl_context::use_shader(GLuint shader_id) const {
   glUseProgram(shader_id);
}

void gl_context::remove_shader(GLuint shader_id) const {
   glDeleteProgram(shader_id);
}
