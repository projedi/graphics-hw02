#version 330 core

in vec2 UV;

out vec3 color;

uniform sampler2D my_texture;
uniform bool show_mipmap;

// Shamelessly taken from https://www.opengl.org/discussion_boards/showthread.php/177520-Mipmap-level-calculation-using-dFdx-dFdy
float mip_map_level(in vec2 texture_coordinate)
{
    // The OpenGL Graphics System: A Specification 4.2
    //  - chapter 3.9.11, equation 3.21
 
 
    vec2  dx_vtc        = dFdx(texture_coordinate);
    vec2  dy_vtc        = dFdy(texture_coordinate);
    float delta_max_sqr = max(dot(dx_vtc, dx_vtc), dot(dy_vtc, dy_vtc));
 
 
    //return max(0.0, 0.5 * log3(delta_max_sqr) - 1.0); // == log2(sqrt(delta_max_sqr));
    return 0.5 * log2(delta_max_sqr); // == log2(sqrt(delta_max_sqr));
}

vec3 color_from_level(float lvl) {
   int max_lvl = 10;
   if(-lvl >= max_lvl) {
      return vec3(0,0,0);
   } else if(-lvl >= max_lvl - 1) {
      return vec3(1,0,0);
   } else if(-lvl >= max_lvl - 2) {
      return vec3(0,0,1);
   } else if(-lvl >= max_lvl - 3) {
      return vec3(1,1,0);
   } else if(-lvl >= max_lvl - 4) {
      return vec3(0,1,1);
   } else if(-lvl >= max_lvl - 5) {
      return vec3(1,0,1);
   } else if(-lvl >= max_lvl - 6) {
      return vec3(1,1,1);
   } else {
      return vec3(0.5,0.5,0.5);
   }
}

void main() {
   if(show_mipmap)
      color = mix(texture(my_texture, UV).rgb, color_from_level(mip_map_level(UV)), 0.3);
   else
      color = texture(my_texture, UV).rgb;
}
