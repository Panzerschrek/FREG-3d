#version 120

attribute vec3 pos;
uniform mat4 proj_mat;
uniform vec3 cube_pos;
void main()
{
	gl_Position= proj_mat * vec4( cube_pos + pos, 1.0 );
}