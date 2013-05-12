#version 330

in vec3 coord;

uniform mat4 proj_mat;
uniform vec3 cam_pos;
uniform vec3 sun_vector;

out vec3 view_vector;
out float sun_vec_len;

void main( void )
{
    view_vector.x= coord.x;
    view_vector.y= coord.z;
    view_vector.z= -coord.y;
    gl_Position= proj_mat * vec4( coord + cam_pos, 1.0f );
    sun_vec_len= length( sun_vector );
}   
   