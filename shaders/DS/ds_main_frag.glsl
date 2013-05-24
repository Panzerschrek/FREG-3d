#version 330

uniform vec3 sun_vector= vec3( 0.0 ,0.0, 0.0 );
uniform sampler2DShadow shadow_map;
uniform float direct_sun_light= 20.0;

uniform float sky_ambient_light= 0.5f;
uniform float fire_ambient_light= 0.2f;

uniform sampler2D albedo_material_id;
uniform sampler2D normal_light;
uniform sampler2D depth_buffer;
uniform sampler1D material_property;

uniform mat4 shadow_mat;
uniform mat3 normal_mat;


in vec2 f_tex_coord;
in vec2 f_screen_coord;

out vec4 color;

uniform vec3 depth_convert_k;
//depth_convert_k.x= - z_far * z_near / ( z_far - z_near );
//depth_convert_k.y= - z_far / ( z_far - z_near );

float LinearFragDepth()
{
    return depth_convert_k.x / ( texture( depth_buffer , f_tex_coord ).x + depth_convert_k.y );
}


float FlashLight( vec3 coord, vec3 n )
{
    float l= 16.0 / dot( coord, coord );
    coord= normalize( coord );

    l*= max( ( coord.z - 0.9 ), 0.0 ) * 10.0;
    l*= max( 0.0, -dot( coord, n ) );
    return l;
}

void main()
{
    vec3 coord= vec3( f_screen_coord, 1.0 ) * LinearFragDepth(); //screen space frag position
    vec4 nl_texdata= texture( normal_light, f_tex_coord ); //normal.xy ( world space ), light
    vec4 ami_texdata= texture( albedo_material_id, f_tex_coord );// albedo, material id, normal.z.sign
    vec3 mp_data= texelFetch( material_property, int( mod( ami_texdata.a * 255.0, 128.0 ) ), 0 ).xyz; //diffuse, specular, spec_pow
    vec3 normal; // screen space normal

    //linear normal convertion
    normal.xy= nl_texdata.xy * 2.0 - vec2( 1.0, 1.0 );
    normal.z= sqrt( 1.005 - normal.x * normal.x - normal.y * normal.y )
              *( step( ( 128.0/255.0), ami_texdata.a ) * 2.0 - 1.0 );
    normal= normal_mat * normal;

    //stereographic normal convertion
    //float nk= 1.0 / ( 1 + nl_texdata.x * nl_texdata.x + nl_texdata.y * nl_texdata.y );
    //normal.xy= 2.0 * nl_texdata.xy;
    //normal.z= - nl_texdata.x * nl_texdata.x - nl_texdata.y * nl_texdata.y + 1.0;
    //normal*= nk;
    //normal.z *= step( 0.5, ami_texdata.a  ) * 2.0 - 1.0;


    float light= nl_texdata.b * sky_ambient_light + nl_texdata.a * fire_ambient_light;// sun and fire ambient light
    light*= 16.0;
    light+= 0.01; // constant ambient light


    vec3 shadow_coord= ( shadow_mat * vec4( coord, 1.0 ) ).xyz;
    shadow_coord.xy= 1.125 * shadow_coord.xy / ( vec2( 0.125, 0.125 ) + abs( shadow_coord.xy ) );
    shadow_coord*= 0.5;
    shadow_coord+= vec3( 0.5, 0.5, 0.5 ) + noise3( f_tex_coord ) * 0.01;

 
    //light+= FlashLight(coord, normal );

    #ifndef SMOOUTH_SHADOWS
    vec2 delta;
    delta= vec2( 0.125, 0.125 ) + abs( shadow_coord.xy );
    delta= 0.004 * ( 0.125 + 0.125 * 0.125 ) * ( delta * delta );
    float shadow_factor= ( texture( shadow_map, shadow_coord )
			   + texture( shadow_map, shadow_coord + vec3( delta.x, delta.y, 0.0  ) )
			   + texture( shadow_map, shadow_coord + vec3( -delta.x, delta.y, 0.0 ) )
			   + texture( shadow_map, shadow_coord + vec3( -delta.x, -delta.y, 0.0) )
			   + texture( shadow_map, shadow_coord + vec3( delta.x, -delta.y, 0.0 ) ) ) * 0.2;
    #else
    float shadow_factor= round( texture( shadow_map, shadow_coord ) );
    #endif

    vec3 tocam_vec= reflect( normalize( coord ), normal );
    tocam_vec= normalize( tocam_vec );
    float spec= mp_data.y * pow( max( dot( sun_vector, tocam_vec ), 0.0 ), mp_data.z * 255.0 );
    float diff= mp_data.x * max( 0.0, dot( sun_vector, normal ) - 0.05 );
    light+= direct_sun_light * shadow_factor * ( diff + spec );

    color=  vec4( light * ami_texdata.xyz, 1.0 );

    //color= vec4( max( normal, vec3( 0.0, 0.0, 0.0 ) ) * 10.0, 1.0 );

}


