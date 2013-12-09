#include <vector>
#include <list>
#include <iostream>
#include <fstream>
#include <sstream>

#include <FreeImagePlus.h>

#include "model.h"
#include "gl_context.h"
#include "camera.h"

struct model_data {
   model_data(gl_context const& ctx): _context(ctx) { }
   ~model_data() {
      glDeleteBuffers(1, &_vertexbuffer);
      glDeleteBuffers(1, &_uvbuffer);
      _context.remove_shader(_texture_shader_id);
      _context.remove_shader(_wire_shader_id);
   }
   gl_context const& _context;
   GLuint _vertexbuffer;
   GLuint _uvbuffer;
   GLuint _texture_shader_id;
   GLuint _wire_shader_id;
   GLuint _texture_id;
};

model::model(gl_context const& context, send_mvp_function const& send_mvp,
      std::vector<glm::vec3> const& vertices, std::vector<glm::vec2> const& uvs,
      unsigned width, unsigned height, uint8_t* pixels):
   _context(context),
   _send_mvp(send_mvp),
   _texture_addition(0),
   _filtering_type(filtering_type::NEAREST),
   _vertex_count(vertices.size()),
   _show_mipmap_levels(false),
   _data(new model_data(_context)) {

   glGenBuffers(1, &_data->_vertexbuffer);
   glBindBuffer(GL_ARRAY_BUFFER, _data->_vertexbuffer);
   glBufferData(GL_ARRAY_BUFFER, _vertex_count * sizeof(glm::vec3), &vertices[0],
         GL_STATIC_DRAW);

   glGenBuffers(1, &_data->_uvbuffer);
   glBindBuffer(GL_ARRAY_BUFFER, _data->_uvbuffer);
   glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0],
         GL_STATIC_DRAW);

   glGenTextures(1, &_data->_texture_id);
   glBindTexture(GL_TEXTURE_2D, _data->_texture_id);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR,
         GL_UNSIGNED_BYTE, pixels);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glGenerateMipmap(GL_TEXTURE_2D);

   _data->_texture_shader_id = _context.add_shader("texture.vert", "texture.frag");
   _data->_wire_shader_id = _context.add_shader("wirecolor.vert", "wirecolor.frag");
}

void set_texture_params(filtering_type const& t) {
   switch(t) {
      case filtering_type::NEAREST:
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
         break;
      case filtering_type::LINEAR:
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
         break;
      case filtering_type::NEAREST_MIPMAP_NEAREST:
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
         break;
      case filtering_type::NEAREST_MIPMAP_LINEAR:
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
         break;
      case filtering_type::LINEAR_MIPMAP_NEAREST:
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
         break;
      case filtering_type::LINEAR_MIPMAP_LINEAR:
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
         break;
   }
}

void model::draw() {
   glEnableVertexAttribArray(0);
   glBindBuffer(GL_ARRAY_BUFFER, _data->_vertexbuffer);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
   glEnableVertexAttribArray(1);
   glBindBuffer(GL_ARRAY_BUFFER, _data->_uvbuffer);
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

   glBindTexture(GL_TEXTURE_2D, _data->_texture_id);
   set_texture_params(_filtering_type);

   _context.use_shader(_data->_texture_shader_id);
   glUniform1f(glGetUniformLocation(_data->_texture_shader_id, "multiple"),
         1 + ((float)_texture_addition) / 10);
   glUniform1f(glGetUniformLocation(_data->_texture_shader_id, "show_mipmap"),
         _show_mipmap_levels);
   _send_mvp(glGetUniformLocation(_data->_texture_shader_id, "MVP"));
   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   glPolygonOffset(1, 1);
   glDrawArrays(GL_TRIANGLES, 0, _vertex_count);
   glDisable(GL_POLYGON_OFFSET_FILL);

   //_context.use_shader(_data->_wire_shader_id);
   //_send_mvp(glGetUniformLocation(_data->_wire_shader_id, "MVP"));
   //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
   //glDrawArrays(GL_TRIANGLES, 0, _vertex_count);

   glDisableVertexAttribArray(0);
   glDisableVertexAttribArray(1);
}

filtering_type& operator++(filtering_type& t) {
   if(t == filtering_type::LINEAR_MIPMAP_LINEAR) t = filtering_type::NEAREST;
   else t = (filtering_type)((int)t + 1);
   return t;
}

filtering_type& operator--(filtering_type& t) {
   if(t == filtering_type::NEAREST) t = filtering_type::LINEAR_MIPMAP_LINEAR;
   else t = (filtering_type)((int)t - 1);
   return t;
}

model create_plane_model(gl_context const& ctx, send_mvp_function const& f) {
   std::vector<glm::vec3> vertices;
   std::vector<glm::vec2> uvs;
   vertices.push_back(glm::vec3(-1,-1,0));
   vertices.push_back(glm::vec3(1,-1,0));
   vertices.push_back(glm::vec3(-1,1,0));
   vertices.push_back(glm::vec3(-1,1,0));
   vertices.push_back(glm::vec3(1,-1,0));
   vertices.push_back(glm::vec3(1,1,0));
   uvs.push_back(glm::vec2(0,0));
   uvs.push_back(glm::vec2(1,0));
   uvs.push_back(glm::vec2(0,1));
   uvs.push_back(glm::vec2(0,1));
   uvs.push_back(glm::vec2(1,0));
   uvs.push_back(glm::vec2(1,1));
   fipImage img;
   if(!img.load("lenna_head.jpg"))
      throw std::runtime_error("Can't find lenna_head.jpg texture");
   return model(ctx, f, vertices, uvs, img.getWidth(), img.getHeight(), img.accessPixels());
}

model create_cube_model(gl_context const& ctx, send_mvp_function const& f) {
   std::vector<glm::vec3> vertices;
   std::vector<glm::vec2> uvs;
   glm::vec3 vs[] = {
      glm::vec3(-1,-1,1),
      glm::vec3(1,-1,1),
      glm::vec3(-1,1,1),
      glm::vec3(1,1,1),
      glm::vec3(1,-1,-1),
      glm::vec3(-1,-1,-1),
      glm::vec3(1,1,-1),
      glm::vec3(-1,1,-1)
   };
   auto add_uvs = [&uvs]() {
      uvs.push_back(glm::vec2(0,0));
      uvs.push_back(glm::vec2(1,0));
      uvs.push_back(glm::vec2(0,1));
      uvs.push_back(glm::vec2(0,1));
      uvs.push_back(glm::vec2(1,0));
      uvs.push_back(glm::vec2(1,1));
   };
   // Front face
   vertices.push_back(vs[0]);
   vertices.push_back(vs[1]);
   vertices.push_back(vs[2]);
   vertices.push_back(vs[2]);
   vertices.push_back(vs[1]);
   vertices.push_back(vs[3]);
   add_uvs();
   // Back face
   vertices.push_back(vs[4]);
   vertices.push_back(vs[5]);
   vertices.push_back(vs[6]);
   vertices.push_back(vs[6]);
   vertices.push_back(vs[5]);
   vertices.push_back(vs[7]);
   add_uvs();
   // Up face
   vertices.push_back(vs[2]);
   vertices.push_back(vs[3]);
   vertices.push_back(vs[7]);
   vertices.push_back(vs[7]);
   vertices.push_back(vs[3]);
   vertices.push_back(vs[6]);
   add_uvs();
   // Down face
   vertices.push_back(vs[5]);
   vertices.push_back(vs[4]);
   vertices.push_back(vs[0]);
   vertices.push_back(vs[0]);
   vertices.push_back(vs[4]);
   vertices.push_back(vs[1]);
   add_uvs();
   // Left face
   vertices.push_back(vs[5]);
   vertices.push_back(vs[0]);
   vertices.push_back(vs[7]);
   vertices.push_back(vs[7]);
   vertices.push_back(vs[0]);
   vertices.push_back(vs[2]);
   add_uvs();
   // Right face
   vertices.push_back(vs[1]);
   vertices.push_back(vs[4]);
   vertices.push_back(vs[3]);
   vertices.push_back(vs[3]);
   vertices.push_back(vs[4]);
   vertices.push_back(vs[6]);
   add_uvs();
   fipImage img;
   if(!img.load("lenna_head.jpg"))
      throw std::runtime_error("Can't find lenna_head.jpg texture");
   return model(ctx, f, vertices, uvs, img.getWidth(), img.getHeight(), img.accessPixels());
}

std::ostream& operator<<(std::ostream& ost, glm::vec3 const& v) {
   return ost << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")";
}

std::ostream& operator<<(std::ostream& ost, glm::vec2 const& v) {
   return ost << "(" << v[0] << ", " << v[1] << ")";
}

struct xyzuv {
   xyzuv(glm::vec3 v): xyz(v) {
      uv[0] = (1 + std::atan2(v[0], v[2]) / M_PI) / 2.0;
      uv[1] = 1 - std::acos(v[1]) / M_PI;
      //if(uv[0] > 1 || uv[0] < 0 || uv[1] > 1 || uv[1] < 0) std::cerr << v << ": " << uv << std::endl;
   }
   glm::vec3 xyz;
   glm::vec2 uv;
};

std::ostream& operator<<(std::ostream& ost, xyzuv const& v) {
   return ost << v.xyz << "<->" << v.uv;
}

// This isn't a sum operator.
xyzuv operator+(xyzuv const& a, xyzuv const& b) {
   return xyzuv(glm::normalize(a.xyz + b.xyz));
}

struct triangle {
   triangle(xyzuv a, xyzuv b, xyzuv c): A(a), B(b), C(c) {
      auto dist = [](xyzuv a, xyzuv b, float d) { return std::abs(a.uv[0] + d - b.uv[0]); };
      float a2b = dist(a, b, 0);
      float a2c = dist(a, c, 0);
      float b2c = dist(b, c, 0);
      if(dist(a, b, 1) < a2b && dist(a, c, 1) < a2c) {
         A.uv[0] += 1;
      } else if(dist(a, b, -1) < a2b && dist(a, c, -1) < a2c) {
         A.uv[0] -= 1;
      } else if(dist(b, a, 1) < a2b && dist(b, c, 1) < b2c) {
         B.uv[0] += 1;
      } else if(dist(b, a, -1) < a2b && dist(b, c, -1) < b2c) {
         B.uv[0] -= 1;
      } else if(dist(c, a, 1) < a2c && dist(c, b, 1) < b2c) {
         C.uv[0] += 1;
      } else if(dist(c, a, -1) < a2c && dist(c, b, -1) < b2c) {
         C.uv[0] -= 1;
      }
   }
   xyzuv A;
   xyzuv B;
   xyzuv C;
};

std::ostream& operator<<(std::ostream& ost, triangle const& t) {
   return ost << "triangle { " << t.A << ", " << t.B << ", " << t.C << " }";
}

std::vector<triangle> divide_triangle(triangle const& t) {
   xyzuv f = t.A + t.B;
   xyzuv g = t.B + t.C;
   xyzuv h = t.A + t.C;
   std::vector<triangle> res;
   res.push_back(triangle(t.A, f, h));
   res.push_back(triangle(f, g, h));
   res.push_back(triangle(g, t.C, h));
   res.push_back(triangle(f, t.B, g));
   return res;
}

std::vector<triangle> iteration(std::vector<triangle> const& ts) {
   std::vector<triangle> res;
   for(auto t: ts) {
      auto trs = divide_triangle(t);
      res.insert(res.end(), trs.begin(), trs.end());
   }
   return res;
}

std::vector<triangle> iteration0() {
   float s32 = std::sqrt(3) / 2;
   xyzuv A(glm::vec3(0,0,1));
   xyzuv B(glm::vec3(s32,0,-0.5));
   xyzuv C(glm::vec3(-s32,0,-0.5));
   xyzuv D(glm::vec3(0,1,0));
   xyzuv E(glm::vec3(0,-1,0));
   std::vector<triangle> res;
   res.push_back(triangle(A, B, D));
   res.push_back(triangle(B, C, D));
   res.push_back(triangle(C, A, D));
   res.push_back(triangle(B, A, E));
   res.push_back(triangle(C, B, E));
   res.push_back(triangle(A, C, E));
   return res;
}

void convert_triangles(std::vector<triangle> const& ts,
      std::vector<glm::vec3>& xyzs, std::vector<glm::vec2>& uvs) {
   for(auto t: ts) {
      //std::cerr << t << std::endl;
      xyzs.push_back(t.A.xyz);
      xyzs.push_back(t.B.xyz);
      xyzs.push_back(t.C.xyz);
      uvs.push_back(t.A.uv);
      uvs.push_back(t.B.uv);
      uvs.push_back(t.C.uv);
   }
}

model create_sphere_model(gl_context const& ctx, send_mvp_function const& f) {
   std::vector<glm::vec3> vertices;
   std::vector<glm::vec2> uvs;
   auto ts = iteration0();
   for(int i = 0; i != 5; ++i) ts = iteration(ts);
   convert_triangles(ts, vertices, uvs);
   fipImage img;
   if(!img.load("earth_texture_grid.bmp"))
      throw std::runtime_error("Can't find earth_texture_grid.bmp texture");
   return model(ctx, f, vertices, uvs, img.getWidth(), img.getHeight(), img.accessPixels());
}
