#version 330

in vec3 normal_cam;
in vec3 position_cam;
in vec3 light_dir;

out vec4 outputColor;

uniform vec4 diffuse_color;
uniform vec4 specular_color;
uniform float power;
uniform vec4 light_pos;
uniform float atten_factor;
uniform float ambient_factor;

float apply_atten(in vec3 position_cam)
{
	vec3 span = vec3(light_pos) - position_cam;
	float span_sqr = dot(span, span);

	return (1.0 / ( 1.0 + atten_factor * sqrt(span_sqr)));
}

void main()
{
	float atten = apply_atten(position_cam);
	float light = dot(normal_cam, light_dir);
	light = clamp(light, 0, 1);

	vec3 view_dir = -normalize(position_cam);
	vec3 half = normalize(light_dir + view_dir);
	float blinn = dot(normal_cam, half);
	blinn = clamp(blinn, 0, 1);
	
	blinn = light > 0.00001 ? blinn : 0.0;
	blinn = pow(blinn, power);

	outputColor = diffuse_color * light * atten + specular_color * blinn * atten + diffuse_color * ambient_factor;
}
