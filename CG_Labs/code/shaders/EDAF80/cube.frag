#version 330

uniform samplerCube my_cube_map;

in VS_OUT {
	vec3 normal;
} fs_in;

out vec4 frag_color;

void main()
{
	frag_color = texture(my_cube_map, fs_in.normal);
}
