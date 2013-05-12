#version 330
layout(points, invocations= 1) in;
layout( triangle_strip, max_vertices = 6 ) out;

in float g_light[];

out vec2 f_tex_coord;
out vec3 f_color;
out float f_light;
uniform vec3 particle_size;


void main()
{
	f_color= vec3( 1.0, 1.0, 1.0 );
	f_light= g_light[0];
	vec4 pos= gl_in[0].gl_Position;
	
	gl_Position= pos + vec4( particle_size.x * 1.0, particle_size.y * 1.0, 0.0, 0.0 );
	f_tex_coord= vec2( 1.0, 1.0 );
	EmitVertex();

	gl_Position= pos + vec4( particle_size.x * 1.0, particle_size.y * (-1.0), 0.0, 0.0 );
	f_tex_coord= vec2( 1.0, 0.0 );
	EmitVertex();

	gl_Position= pos + vec4( particle_size.x * (-1.0), particle_size.y * (-1.0), 0.0, 0.0 );
	f_tex_coord= vec2( 0.0, 0.0 );
	EmitVertex();
	EndPrimitive();

	gl_Position= pos + vec4( particle_size.x * 1.0, particle_size.y * 1.0, 0.0, 0.0 );
	f_tex_coord= vec2( 1.0, 1.0 );
	EmitVertex();

	gl_Position= pos + vec4( particle_size.x * (-1.0), particle_size.y * (-1.0), 0.0, 0.0 );
	f_tex_coord= vec2( 0.0, 0.0 );
	EmitVertex();

	gl_Position= pos + vec4( particle_size.x * (-1.0), particle_size.y * 1.0, 0.0, 0.0 );
	f_tex_coord= vec2( 0.0, 1.0 );
	EmitVertex();
	EndPrimitive();

}	