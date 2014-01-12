#version 330

in vec3 normal_cam;
in vec3 position_cam;
in vec3 light_dir;

uniform float power;

out vec4 outputColor;


void main()
{
	float light = dot(normal_cam, light_dir);
	light = clamp(light, 0, 1);

	vec3 view_dir = -normalize(position_cam);
	vec3 half = normalize(light_dir + view_dir);
	float blinn = dot(normal_cam, half);
	blinn = clamp(blinn, 0, 1);
	
	blinn = light > 0.00001 ? blinn : 0.0;
	blinn = pow(blinn, power);

	light = light + blinn;

	outputColor = vec4(light, 0, 0, 1.0f);
}
