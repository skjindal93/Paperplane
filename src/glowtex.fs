STRINGIFY(
varying vec2 UV;

uniform int w;
uniform int h;
uniform sampler2D tex;

float sigma = 4.0;

float getGauss(float x, float y)	{
	float twvar = 2 * sigma * sigma;
	return exp(-(x*x + y*y) / twvar);
}

void main()	{
	vec2 tc = UV;
	tc.x *= min(w,h);
	tc.y *= min(w,h);
	tc += (vec2(w,h) - min(w,h))/2.0;
	gl_FragColor = vec4(0.0);
	
	int size = int(ceil(6 * sigma) + 0.1);
	size += mod(size, 2) == 1 ? 0 : 1;
	int size_2 = size/2;
	
	// blur
	gl_FragColor = vec4(0.0);
	float weights = 0.0;
	for(int x = -size_2; x <= size_2; ++x)
		for(int y = -size_2; y <= size_2; ++y)	{
			float weight = getGauss(float(x), float(y));
			gl_FragColor += texture2D(tex, vec2((tc.x + x)/w, (tc.y + y)/h)) * weight;
			weights += weight;
		}
	gl_FragColor /= weights;
	
	// original color (anti-aliased)
	vec4 orig = vec4(0.0);
	size_2 = 2;
	sigma = 0.4;
	weights = 0.0;
	for(int x = -size_2; x <= size_2; ++x)
		for(int y = -size_2; y <= size_2; ++y)	{
			float weight = getGauss(float(x), float(y));
			orig += texture2D(tex, vec2((tc.x + x)/w, (tc.y + y)/h)) * weight;
			weights += weight;
		}
	orig /= weights;
	
	gl_FragColor = vec4(0.4 * gl_FragColor.xyz, 1.1 * gl_FragColor.w) + 0.75 * orig;
	//gl_FragColor.w = min(1.0, gl_FragColor.w);
	
})