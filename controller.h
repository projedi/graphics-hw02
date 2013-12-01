#pragma once

#include <functional>

struct GLFWwindow;

struct controller {
   controller(controller const&) = delete;
   controller(controller&&) = delete;
   controller& operator=(controller const&) = delete;
   controller& operator=(controller&&) = delete;
   static controller* instance() { return INSTANCE; }
   static void init(GLFWwindow*);
   static void deinit();

   void mouseButtonCallback(int button, int action, int mods);
   void scrollCallback(double xoffset, double yoffset);
   void keyCallback(int key, int scancode, int action, int mods);
   void cursorPosCallback(double xpos, double ypos);

   typedef std::function<void (int, int, int)> mouse_button_callback;
   void mouse_button(mouse_button_callback const& cb) { _mouse_button_callback = cb; }
   typedef std::function<void (double, double)> scroll_callback;
   void scroll(scroll_callback const& cb) { _scroll_callback = cb; }
   typedef std::function<void (int, int, int, int)> key_callback;
   void key(key_callback const& cb) { _key_callback = cb; }
   typedef std::function<void (double, double)> cursor_pos_callback;
   void cursor_pos(cursor_pos_callback const& cb) { _cursor_pos_callback = cb; }
private:
   controller(GLFWwindow*);
   ~controller() { }
private:
   static controller* INSTANCE;
   GLFWwindow* _window;
   mouse_button_callback _mouse_button_callback;
   scroll_callback _scroll_callback;
   key_callback _key_callback;
   cursor_pos_callback _cursor_pos_callback;
};
