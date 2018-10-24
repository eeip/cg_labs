#version 330

float t;
vec4 color_deep;
vec4 color_shallow;

uniform vec3 camera_position;
uniform sampler2D diffuse_texture;
uniform samplerCube my_cube_map;

in VS_OUT {
	vec3 vertex_modified;
	vec2 texcoord;
	vec3 normal_modified;
	vec3 binormal;
	vec3 tangent;
} fs_in;

out vec4 frag_color;

void main()
{
	// Calculating the water color.
	color_deep = vec4(0,0,0.1,1);
	color_shallow = vec4(0,0.5,0.5,1);
	vec3 V = normalize(camera_position - fs_in.vertex_modified);
	float facing = 1-max(dot(V,fs_in.normal_modified),0);
	vec4 color_water = mix(color_deep,color_shallow,facing);

	// Adding the reflection mapping.
	vec3 R = reflect(-V, fs_in.normal_modified);
	vec4 reflection = texture(my_cube_map, R);

	// Bump mapping.
	vec2 texScale = vec2(8,4);
	float bumpTime = mod(t,100);
	vec2 bumpSpeed = vec2(-0.05,0);
	vec2 bumpCoord0 = fs_in.texcoord*texScale + bumpTime*bumpSpeed;
	vec2 bumpCoord1 = fs_in.texcoord*texScale*2 + bumpTime*bumpSpeed*4;
	vec2 bumpCoord2 = fs_in.texcoord*texScale*4 + bumpTime*bumpSpeed*8;

	vec4 n0 = texture(diffuse_texture, bumpCoord0)*2-1;
	vec4 n1 = texture(diffuse_texture, bumpCoord1)*2-1;
	vec4 n2 = texture(diffuse_texture, bumpCoord2)*2-1;
	vec4 nBump = normalize(n0+n1+n2);

	mat3 BTN = mat3(fs_in.binormal, fs_in.tangent, fs_in.normal_modified);

	vec3 n_world = BTN * vec3(nBump);


	// Fresnel.
	float R0 = 0.02037;
	float fresnel = R0 + (1-R0)*pow(1-dot(V,fs_in.normal_modified),5);

	// Refraction.
	vec3 r = refract(-V,fs_in.normal_modified,1.33);
	vec4 refraction = texture(my_cube_map, r);

	frag_color = color_water + reflection * fresnel + refraction*(1-fresnel);
}
