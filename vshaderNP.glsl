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
uniform int smoothOrFlat;
uniform vec4 ambientProduct, diffuseProduct, specularProduct;
uniform vec4 distantLightVector;

//d
uniform int spotOrPoint;
uniform vec4 pointLight1;
uniform vec4 pointLightVector;
uniform vec4 pointAmbient, pointDiffuse, pointSpecular;
uniform float angle, exp;
uniform float atten1, atten2, atten3;

uniform float shiny;

void main() 
{
    vec4 vPosition4 = vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);
    vec4 vColor4 = vec4(vNormal.r, vNormal.g, vNormal.b, 1.0); 
    float attenuation = 1.0f;
    // YJC: Original, incorrect below:
    //      gl_Position = projection * model_view * vPosition/vPosition.w;

    //gl_Position = projection * model_view * vPosition4;

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

    //part d: repeat this process with the point source vectors, and ADD it to color

    vec3 Lf;    
    //change light vector to be relative from position to light source
    l = normalize(pointLight1.xyz - position);

    if(spotOrPoint==1) //if its a spotlight, we need to factor in the directional vector from the point
    {
        Lf = normalize(pointLightVector.xyz);
    }
    h = normalize(l+e);

    float dis = length(position - pointLight1.xyz); //distance from light to our point we're shading
    attenuation = (1.0f)/(atten1 + atten2*dis + atten3*dis*dis); //factoring attenuation sum as a coefficient (lecture 9-10)

    if(spotOrPoint==1) //if spotlight, we have to check to see if its in the angle, and scale the intensity (coefficient to attenuation)
    {
        if(dot(-l,Lf) > cos(angle)) // lecture 9-10 diagram
        {
			attenuation = attenuation * pow(dot(-l,Lf),exp);
		}
		else
        { //out of range of the light, set coefficient to 0
			attenuation = 0;
		}
    }

    ambient = attenuation * pointAmbient;
    
    dCoeff = max( dot(l, newNormal), 0.0 );
    diffuse = attenuation * dCoeff * pointDiffuse;
    
    sCoeff = pow(max(dot(newNormal,h),0.0), shiny);
    specular = attenuation * (sCoeff*(pointSpecular));
    if (dot(l,newNormal) < 0.0)
    {
        specular = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    color += attenuation * (ambient + diffuse + specular);
    gl_Position = projection * model_view * vPosition4; 
}

