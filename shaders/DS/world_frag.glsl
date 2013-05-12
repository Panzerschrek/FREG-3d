#version 330

uniform sampler2DArray tex;


in vec3 f_tex_coord;
in vec2 f_light;//sun and fire light
in vec3 f_normal; // view space normal

layout( location= 0 ) out vec4 albedo_material_id;
layout( location= 1 ) out vec4 normal_light;

void main( void )
{
    vec4 c= texture( tex, f_tex_coord );
    if( c.a <= 0.5 )discard;

    albedo_material_id.xyz= c.xyz;
    albedo_material_id.a= ( f_tex_coord.z + 0.0015  ) * ( 1.0 / 255.0 );
    albedo_material_id.a+= step( 0.0, f_normal.z ) * ( 128.0 / 255.0 );

    // linear normal convertion
    normal_light.xy= f_normal.xy * 0.5 + vec2( 0.5, 0.5 );

    //stereographic normal convertion
    //normal_light.xy= f_normal.xy/( 1.0 - f_normal.z );

    normal_light.ba= f_light * ( 1.0/16.0 );
}
