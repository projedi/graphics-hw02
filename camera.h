#include <GL/glew.h>
#include <glm/glm.hpp>

struct GLFWwindow;

struct camera {
   camera(GLFWwindow* window, float near, float far);
   glm::mat4 const& mvp() const { return _mvp; }
   GLfloat far() const { return _far; }
   GLfloat near() const { return _near; }
   void update_matrices();
   void set_angles(float up_angle, float right_angle);
   void zoom_out();
   void zoom_in();
private:
   GLfloat _fov;
   GLfloat _width;
   GLfloat _height;
   GLfloat _near;
   GLfloat _far;
   glm::vec3 _direction;
   glm::vec3 _right;
   glm::mat4 _mvp;
   float _distance;
};

void send_matrix(GLuint id, glm::mat4 const&);
