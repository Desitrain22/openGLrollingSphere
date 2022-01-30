/*****************************
 * File: fshader42.glsl
 *       A simple fragment shader
 *****************************/

#version 150  // YJC: Comment/un-comment this line to resolve compilation errors
              //      due to different settings of the default GLSL version

in  vec4 color;
in vec4 eye;
in vec4 position;

out vec4 fColor;

uniform int fogOn;
uniform int fogType;

void main() 
{
    fColor = color;
    float fogClamp = 1.0f;
    vec3 fogBaseColor = vec3(0.7f, 0.7f, 0.7f);

    if (fogOn==1)
    {
        vec3 eyeBase3 = vec3(eye.x, eye.y, eye.z);
        if (fogType == 1) //Linear
        {
            //recall: eye is at (7.0, 3.0, -10.0), floor runs from 5,0,8 to -5,0,-4 (10x12)
            //from lecture 12: end - distance from pixel to viewer / end - start ()
            fogClamp = (18.0f) - length(eyeBase3);
            fogClamp = fogClamp/(18.0f); //still just 18.0f
        }
        else if (fogType == 2) //Exponential
        {
            fogClamp = exp((-0.09)*length(eyeBase3));
        }
        else if (fogType == 3) //"EXPOQUAD"
        {
            
            fogClamp = exp(-pow((-0.09*length(eyeBase3)),2));
        }
        fogClamp = clamp(fogClamp, 0.0f, 1.0f);

        //from the document: mix only gives a vec3, so we gotta fenagle while mantaining the alpha value 
        fColor = vec4(mix(fogBaseColor, color.xyz, fogClamp), color.w);
    }
} 

