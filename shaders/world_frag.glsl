#version 330

uniform vec3 sun_vector= vec3( 0.05, 0.82, 0.517 );
uniform vec3 fog_color= vec3( 0.8, 0.8, 1.0 );
uniform sampler2DArray tex;
uniform sampler2DShadow shadow_map;
uniform sampler1D material_property;
uniform float direct_sun_light= 20.0;
uniform vec3 cam_pos;


in vec3 f_tex_coord;
in float f_light;
in vec3 f_normal;
in float fog_k;
in vec3 f_position;
in vec3 f_world_position;
out vec4 color;



float SunLight(void)
{
	#ifdef NORMAL_INTERPOLATION
	vec3 normal= normalize( f_normal );
	#else
	vec3 normal = f_normal;
	#endif
	vec3 p= f_position;
	float sun_light_cos= max( dot( normal, sun_vector )- 0.05, 0.0 );
	//p.z-= 0.001 * inversesqrt( sun_light_cos ); //epsilon

	float sun_l= texture( shadow_map, p );
	sun_l= round( sun_l );

	//material property
	vec3 mp= texelFetch( material_property, int( f_tex_coord.z + 0.1 ), 0 ).xyz;

	float diffuse, specular;
	diffuse= sun_light_cos;
	vec3 tocam_pos= normalize( f_world_position - cam_pos );
	tocam_pos= reflect( tocam_pos, normal );
	specular= max( dot( sun_vector, tocam_pos ), 0.0f );
	specular= pow( specular, mp.z * 255.0f );
	

	return direct_sun_light *( ( specular * mp.y + mp.x * diffuse ) * sun_l );
}

void main( void )
{

   	float l= f_light + 0.01;
	vec4 c= texture( tex, f_tex_coord );
	if( c.a <= 0.5 )discard;
	
	l+= SunLight();
	color= mix( l * c, vec4( fog_color, 1.0 ), 0.0 );

}
