#version 330 core

in vec4 finalColor;
out vec4 fragCol;

void main()
{
	fragCol = finalColor;
}
