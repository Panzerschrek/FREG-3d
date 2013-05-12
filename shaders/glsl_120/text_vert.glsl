#version 120
uniform sampler2D tex;
uniform vec3 inv_tex_size= vec3( 1.0/512.0, 1.0/512.0, 0.0 );

attribute vec2 coord;
attribute vec2 tex_coord;
attribute vec3 color;

varying vec3 f_color;
varying vec2 f_tex_coord;

void main()
{
    f_tex_coord= tex_coord * inv_tex_size.xy;
    f_color= color;
    gl_Position= vec4( coord, -1.0, 1.0 );
}
