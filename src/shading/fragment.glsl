#version 330

varying float light;

out vec4 outputColor;

void main()
{
	outputColor = vec4(light, 0, 0, 1.0f);
}
