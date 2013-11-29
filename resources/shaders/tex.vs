#version 120

// Output data
varying vec2 UV;

void main()	{
	gl_Position = ftransform();
	UV = gl_Position.xy / gl_Position.w;
	UV = (UV + 1.0) / 2.0;
}
