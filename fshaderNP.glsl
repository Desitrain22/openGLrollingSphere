/*****************************
 * File: fshader42.glsl
 *       A simple fragment shader
 *****************************/

#version 150  // YJC: Comment/un-comment this line to resolve compilation errors
              //      due to different settings of the default GLSL version

in  vec4 color;

in	vec4 position;
in	vec4 oPosition;

out vec4 fColor;

uniform int fog;
uniform string fogType;

void main() 
{ 
    fColor = color;
    vec3 fogColor = vec3(0.7, 0.7, 0.7), fColor3;
	float density = 0.09;
	float z = length(ePosition.xyz), f;
	vec4 colorAfterTexM;
	vec4 color_reddish = vec4(0.9, 0.1, 0.1, 1.0);

} 
/*
/* 
File Name: "vshader53.glsl":
Vertex shader:
  - Per vertex shading for a single point light source;
    distance attenuation is Yet To Be Completed.
  - Entire shading computation is done in the Eye Frame.
*/

#version 150  // YJC: Comment/un-comment this line to resolve compilation errors
              //      due to different settings of the default GLSL version

in  vec4 vPosition;
in  vec3 vNormal;
out vec4 color;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform mat4 ModelView;
uniform mat4 Projection;
uniform mat3 Normal_Matrix;
uniform vec4 LightPosition;   // Must be in Eye Frame
uniform float Shininess;

uniform float ConstAtt;  // Constant Attenuation
uniform float LinearAtt; // Linear Attenuation
uniform float QuadAtt;   // Quadratic Attenuation

void main()
{
    // Transform vertex  position into eye coordinates
    vec3 pos = (ModelView * vPosition).xyz;
	
    vec3 L = normalize( LightPosition.xyz - pos );
    vec3 E = normalize( -pos );
    vec3 H = normalize( L + E );

    // Transform vertex normal into eye coordinates
      // vec3 N = normalize( ModelView*vec4(vNormal, 0.0) ).xyz;
    vec3 N = normalize(Normal_Matrix * vNormal);

// YJC Note: N must use the one pointing *toward* the viewer
//     ==> If (N dot E) < 0 then N must be changed to -N
//
   if ( dot(N, E) < 0 ) N = -N;


/*--- To Do: Compute attenuation ---*/
float attenuation = 1.0; 

 // Compute terms in the illumination equation
    vec4 ambient = AmbientProduct;

    float d = max( dot(L, N), 0.0 );
    vec4  diffuse = d * DiffuseProduct;

    float s = pow( max(dot(N, H), 0.0), Shininess );
    vec4  specular = s * SpecularProduct;
    
    if( dot(L, N) < 0.0 ) {
	specular = vec4(0.0, 0.0, 0.0, 1.0);
    } 

    gl_Position = Projection * ModelView * vPosition;

/*--- attenuation below must be computed properly ---*/
    color = attenuation * (ambient + diffuse + specular);
}
*/
