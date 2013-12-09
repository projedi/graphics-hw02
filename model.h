#include <functional>
#include <vector>
#include <memory>

#include <GL/glew.h>
#include <glm/glm.hpp>

struct gl_context;

typedef std::function<void (GLuint)> send_mvp_function;

enum class filtering_type {
   NEAREST = 0,
   LINEAR,
   NEAREST_MIPMAP_NEAREST,
   NEAREST_MIPMAP_LINEAR,
   LINEAR_MIPMAP_NEAREST,
   LINEAR_MIPMAP_LINEAR
};

filtering_type& operator++(filtering_type&);
filtering_type& operator--(filtering_type&);

struct model_data;

struct model {
   model(gl_context const&, send_mvp_function const&,
         std::vector<glm::vec3> const& vertices,
         std::vector<glm::vec2> const& uvs,
         unsigned width, unsigned height, uint8_t* pixels);
   void draw();
   void next_filtering() { ++_filtering_type; }
   void prev_filtering() { --_filtering_type; }
   void increase_multiple() { ++_texture_addition; }
   void decrease_multiple() { if(_texture_addition > -9) --_texture_addition; }
   void toggle_show_mipmap_levels() { _show_mipmap_levels = !_show_mipmap_levels; }
private:
   gl_context const& _context;
   send_mvp_function _send_mvp;
   int _texture_addition; // actual multiplicator is 1 + _texture_addition / 10
   filtering_type _filtering_type;
   GLsizei _vertex_count;
   bool _show_mipmap_levels;
   std::shared_ptr<model_data> _data;
};


model create_plane_model(gl_context const&, send_mvp_function const&);
model create_cube_model(gl_context const&, send_mvp_function const&);
model create_sphere_model(gl_context const&, send_mvp_function const&);
