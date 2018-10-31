#version 330

layout (location = 0) in vec3 vertex;
layout (location = 5) in float radius;

uniform mat4 vertex_model_to_world;
uniform mat4 vertex_world_to_clip;

out VS_OUT {
	float radius;
} vs_out;


void main()
{
	vs_out.radius = radius;
	gl_Position = vertex_world_to_clip * vertex_model_to_world * vec4(vertex, 1.0);
}



