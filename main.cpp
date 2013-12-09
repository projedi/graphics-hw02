#include <vector>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "camera.h"
#include "controller.h"
#include "gl_context.h"
#include "model.h"

// TODO: Draw a status text somewhere

struct model_switcher {
   model_switcher(model const& m) {
      _models.push_back(m);
      _idx = 0;
   }
   void add_model(model const& m) { _models.push_back(m); }
   model& current_model() { return _models[_idx]; }
   void prev() {
      if(!_idx) _idx = _models.size();
      --_idx;
   }
   void next() {
      _idx = (_idx + 1) % _models.size();
   }
private:
   size_t _idx;
   std::vector<model> _models;
};

void setup_controller(gl_context& context, camera& main_camera, model_switcher& ms) {
   // Since controller is a singleton anyway it will do.
   static bool _rotation_mode = false;
   static double _mouse_x = 0;
   static double _mouse_y = 0;
   controller::instance()->mouse_button([&](int button, int action, int) {
      if(button == GLFW_MOUSE_BUTTON_LEFT) {
         _rotation_mode = (action == GLFW_PRESS);
         if(_rotation_mode) {
            glfwGetCursorPos(context.main_window(), &_mouse_x, &_mouse_y);
         }
      }
   });
   controller::instance()->scroll([&](double xoffset, double yoffset) {
      if(yoffset < 0) main_camera.zoom_out();
      else if(yoffset > 0) main_camera.zoom_in();
   });
   controller::instance()->key([&](int key, int, int action, int) {
      if(key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT))
         main_camera.zoom_in();
      else if(key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT))
         main_camera.zoom_out();
      else if(key == GLFW_KEY_SPACE && action == GLFW_PRESS)
         ms.next();
      else if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
         ms.current_model().next_filtering();
      else if(key == GLFW_KEY_LEFT && action == GLFW_PRESS)
         ms.current_model().prev_filtering();
      else if(key == GLFW_KEY_UP && action == GLFW_PRESS)
         ms.current_model().increase_multiple();
      else if(key == GLFW_KEY_DOWN && action == GLFW_PRESS)
         ms.current_model().decrease_multiple();
      else if(key == GLFW_KEY_M && action == GLFW_PRESS)
         ms.current_model().toggle_show_mipmap_levels();
   });
   controller::instance()->cursor_pos([&](double xpos, double ypos) {
      if(!_rotation_mode) return;
      double dx = xpos - _mouse_x;
      double dy = ypos - _mouse_y;
      float up_angle = -dx;
      float right_angle = dy;
      main_camera.set_angles(up_angle, right_angle);
      _mouse_x = xpos;
      _mouse_y = ypos;
   });
}

int main() {
   gl_context context(800, 600);
   camera main_camera(context.main_window(), 0.1, 100);
   auto sendmvp = [&](GLuint id) { send_matrix(id, main_camera.mvp()); };
   model_switcher ms(create_plane_model(context, sendmvp));
   ms.add_model(create_cube_model(context, sendmvp));
   ms.add_model(create_sphere_model(context, sendmvp));
   controller::init(context.main_window());

   setup_controller(context, main_camera, ms);

   glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LEQUAL);

   context.main_loop([&]() {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      main_camera.update_matrices();
      ms.current_model().draw();
   });

   controller::deinit();
   return 0;
}
