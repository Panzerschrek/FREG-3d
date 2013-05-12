#version 330

layout( location= 0 ) out vec4 albedo_material_id;
layout( location= 1 ) out vec4 normal_light;

in vec2 f_tex_coord;
in vec3 f_color;
in float f_light;
uniform sampler2D tex;

void main()
{
	vec4 c= texture( tex, f_tex_coord );
	if( c.a < 0.01 || // alpha test
	 ( mod( gl_FragCoord.x + gl_FragCoord.y, 2.0 ) < 1.0 && c.a < 0.5 ) )// simulate transparency
		discard;

	albedo_material_id= vec4( c.xyz, 0.5 );
	normal_light= vec4( 0.0, 0.0, 1.0, 1.0 );
	
}