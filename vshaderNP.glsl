/***************************
 * File: vshaderNP.glsl:
 *   A simple vertex shader.
 *
 * - Vertex attributes (positions & colors) for all vertices are sent
 *   to the GPU via a vertex buffer object created in the OpenGL program.
 *
 * - This vertex shader uses the Model-View and Projection matrices passed
 *   on from the OpenGL program as uniform variables of type mat4.
 ***************************/

#version 150  // YJC: Comment/un-comment this line to resolve compilation errors
              //      due to different settings of the default GLSL version

in  vec3 vPosition;
in vec3 vNormal;
out vec4 color;

uniform mat3 normal;
uniform mat4 model_view;
uniform mat4 projection;

//c
uniform int spotOrPoint;
uniform int smoothOrFlat;
uniform vec4 ambientProduct, diffuseProduct, specularProduct;
uniform vec4 distantLightVector;

//d
uniform vec4 pointLightVector;
uniform vec4 pointAmbientProduct, pointDiffuseProduct, pointSpecularProduct;

uniform float shiny;

void main() 
{
    vec4 vPosition4 = vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);
    vec4 vColor4 = vec4(vNormal.r, vNormal.g, vNormal.b, 1.0); 
   
    // YJC: Original, incorrect below:
    //      gl_Position = projection * model_view * vPosition/vPosition.w;

    gl_Position = projection * model_view * vPosition4;

    vec3 position = (model_view * vPosition4).xyz;

    vec3 l = normalize(-distantLightVector.xyz ); 
    vec3 e = normalize(-position ); 
    vec3 h = normalize (l + e);

    vec3 newNormal;
    if (smoothOrFlat==0)
    {
       newNormal = normal * vNormal;
       newNormal = normalize(newNormal);
    }
    else
    {
        newNormal = normal * vPosition;
        newNormal = normalize(newNormal);
    }
    
    /*if ( dot(newNormal, e) < 0 ) 
    {
        newNormal = -newNormal;
    }*/
    
    vec4 ambient = ambientProduct;
   
    float dCoeff = max( dot(l, newNormal), 0.0); 
    vec4 diffuse = dCoeff * diffuseProduct;
    
    float sCoeff = pow( max(dot(newNormal, h), 0.0), shiny);
    vec4 specular = sCoeff*specularProduct;

    if (dot(l,newNormal) < 0.0)
    {
        specular = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
    color = (ambient+diffuse+specular);

    
    /*
    float dis = length(position - pointLightVector.xyz);
    float attenuation = 1.0/(atten[0]+ (atten[1]*dis) + (atten[2]*dis*dis));

    L = normalize(positionalposition.xyz - pos);
    if(point_light == 1){
        vec3 Lf = normalize(positionaldirection.xyz);

        if(dot(-L,Lf)>cos(positionalAng)){ attenuation *= pow(dot(-L,Lf),positionalExp); }
        else{ attenuation = 0; }
    }

    ambient = positionalambientproduct;

    d = max(dot(L, N), 0.0);
    diffuse = d * positionaldiffuseproduct;

    H = normalize(L + E);
    s = pow( max( dot(N, H), 0.0), Shininess);
    specular = s * positionalspecularproduct;
    if(dot(L,N)<0.0){specular = vec4(0,0,0,1);}

    color += attenuation * (ambient + diffuse + specular);


    gl_Position = projection * model_view * vPosition;
    */
}

