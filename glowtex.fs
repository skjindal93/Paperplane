#version 120
#define PI 3.1415926535897932384626433832795

varying vec2 UV;

uniform int w, h;
uniform sampler2D tex;
float sigma = 3.0;

float getGauss(float x, float y)	{
	float twvar = 2 * sigma * sigma;
	return exp(-(x*x + y*y) / twvar) / (PI * twvar);
}

void main()	{
	vec2 tc = UV;
	tc.x *= w;
	tc.y *= h;
	gl_FragColor = vec4(0.0);
	
	int size = int(ceil(6 * sigma) + 0.1);
	size += mod(size, 2) == 1 ? 0 : 1;
	int size_2 = size/2;
	
	gl_FragColor = vec4(0.0);
	for(int x = -size_2; x <= size_2; ++x)
		for(int y = -size_2; y <= size_2; ++y)	{
			gl_FragColor += texture2D(tex, vec2((tc.x + x)/w, (tc.y + y)/h)) * getGauss(float(x), float(y));
		}
	vec4 orig = texture2D(tex, vec2((tc.x)/w, (tc.y)/h));
	float alpha = 0.9; //gl_FragColor.w;
	gl_FragColor = alpha * gl_FragColor + (1.0 - alpha) * orig;// - gl_FragColor * orig;
	//gl_FragColor.w = min(1.0, gl_FragColor.w);

}

