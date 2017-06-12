#version 150

in vec3 position;
in vec3 color;
in vec2 uv;

out vec2 frag_uv;

void main(){
	frag_uv = uv;
	gl_Position =  vec4(position, 1.0);
}


#FRAG

#version 150

in vec2 frag_uv;
out vec4 frag_color;


uniform float frame_time;
uniform sampler2D texture_sampler;

void main() {
	vec4 texel = texture(texture_sampler, frag_uv);

	// fill color of transparent background
	if (texel.a < 0.1) {
		float time = frame_time * 2.0;
		texel = vec4(0.3,0.2,0.5,1.0);
	
		// add a little effect
		vec2 nuv = frag_uv * 1.0;

		nuv.x = cos(time) * (nuv.x) - sin(time) * (nuv.y);
		nuv.y = sin(time) * (nuv.x) + cos(time) * (nuv.y); 

		nuv.x *=pow(cos(time),3);
		nuv.y +=sin(time) * nuv.x;

		texel.xy = texel.xy + texture(texture_sampler, nuv).xy * texel.xy;
	}

	frag_color = texel;
}