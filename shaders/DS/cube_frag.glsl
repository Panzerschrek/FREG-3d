#version 330

out vec4 albedo_material_id;
out vec4 normal_light;
void main()
{
	albedo_material_id= vec4( 0.0, 0.0, 0.0, 0.0 );
	normal_light= vec4( 0.0, 0.0, 0.0, 0.0 );	
}