#version 330

uniform vec3 inColor;

out vec3 color;

void main(void) {
    
    color.xyz = inColor;
    
    /*
    color = normalize(gl_FragCoord.xyz);
    
    if (color.x < 0) color.x *= -1;
    if (color.y < 0) color.y *= -1;
    if (color.z < 0) color.z *= -1;
    
    if (gl_PrimitiveID % 4 == 0) {
    	float tmp = color.x;
    	color.x = color.y;
    	color.y = color.x;
    }
    if (gl_PrimitiveID % 4 == 1) {
    	float tmp = color.y;
    	color.y = color.z;
    	color.z = color.y;
    }
    if (gl_PrimitiveID % 4 == 2) {
    	float tmp = color.x;
    	color.x = color.z;
    	color.z = color.x;
    }
    */
    
}
