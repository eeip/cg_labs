#version 330

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 texcoord;

uniform mat4 vertex_model_to_world;
uniform mat4 normal_model_to_world;
uniform mat4 vertex_world_to_clip;
uniform vec3 camera_position;

vec2 Amp;
vec2 freq;
vec2 phase;
vec2 sharp;
mat2x2 dir;
float t;
vec3 vertex_modified;

out VS_OUT {
	vec3 vertex_modified;
	vec2 texcoord;
	vec3 normal_modified;
	vec3 binormal;
	vec3 tangent;
} vs_out;


void main()
{
	// Loading the given information.
	Amp = vec2(1,0.5);
	freq = vec2(0.2, 0.4);
	phase = vec2(0.5, 1.3);
	sharp = vec2(2, 2);
	dir = mat2x2(vec2(-1,0), vec2(-0.7,  0.7));
	t = 1;

	// Calcuating the displacement of y.
	float W1 = Amp.x*pow(sin((vertex.x*dir[0][0]+vertex.z*dir[1][0])*freq.x+t*phase.x)*0.5+0.5,sharp.x);
	float W2 = Amp.y*pow(sin((vertex.x*dir[0][1]+vertex.z*dir[1][1])*freq.y+t*phase.y)*0.5+0.5,sharp.y);
	float H = W1 + W2;
	vec3 vertex_modified = vertex;
	vertex_modified.y = H;

	// Calculating the partial derivatives.
	float G1x = Amp.x*sharp.x*pow(sin((vertex.x*dir[0][0]+vertex.z*dir[1][0])*freq.x+t*phase.x)*0.5+0.5,sharp.x-1)*
		cos((vertex.x*dir[0][0]+vertex.z*dir[1][0])*freq.x+t*phase.x)*dir[0][0];
	float G1z = Amp.x*sharp.x*pow(sin((vertex.x*dir[0][0]+vertex.z*dir[1][0])*freq.x+t*phase.x)*0.5+0.5,sharp.x-1)*
		cos((vertex.x*dir[0][0]+vertex.z*dir[1][0])*freq.x+t*phase.x)*dir[1][0];
	float G2x = Amp.y*sharp.x*pow(sin((vertex.x*dir[0][1]+vertex.z*dir[1][1])*freq.y+t*phase.y)*0.5+0.5,sharp.y-1)*
		cos((vertex.x*dir[0][1]+vertex.z*dir[1][1])*freq.y+t*phase.y)*dir[0][1];
	float G2z = Amp.y*sharp.x*pow(sin((vertex.x*dir[0][1]+vertex.z*dir[1][1])*freq.y+t*phase.y)*0.5+0.5,sharp.y-1)*
		cos((vertex.x*dir[0][1]+vertex.z*dir[1][1])*freq.y+t*phase.y)*dir[1][1];
	float dHdx = G1x + G2x;
	float dHdz = G1z + G2z;

	// Calculating the tangent space.
	vec3 normal_modified = vec3(-dHdx, 1, -dHdz);
	vec3 tangent = vec3(1, dHdx, 0);
	vec3 binormal = vec3(0, dHdz, 1);

	vs_out.texcoord = vec2(texcoord.x, texcoord.y);
	vs_out.vertex_modified = vec3(vertex_model_to_world * vec4(vertex_modified, 1.0));
	vs_out.normal_modified = vec3(normal_model_to_world * vec4(normal_modified, 0.0));
	vs_out.binormal = vec3(vertex_model_to_world * vec4(binormal, 0));
	vs_out.tangent = vec3(vertex_model_to_world * vec4(tangent, 0));
	gl_Position = vertex_world_to_clip * vertex_model_to_world * vec4(vertex, 1.0);
}



