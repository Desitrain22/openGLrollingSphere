/*****************************
 * File: fshader42.glsl
 *       A simple fragment shader
 *****************************/

#version 150  // YJC: Comment/un-comment this line to resolve compilation errors
              //      due to different settings of the default GLSL version

in  vec4 color;
in vec4 eye;
in vec4 positionOutput; //vPosition stolen from vshaderNP on it's way out
in vec2 textCoords;

out vec4 fColor;

//4a
uniform int fogOn;
uniform int fogType;

//4b reference: Checker-new handout
uniform int textureFlag;
uniform sampler2D texture_2D;

void main() 
{
    vec4 tempColor4 = color;

    float fogClamp = 1.0f;
    vec3 fogBaseColor = vec3(0.7f, 0.7f, 0.7f);
    //part c
    
    if(textureFlag ==1)
    {
        tempColor4 = color * texture( texture_2D, textCoords);
    }

    //part a -- now moved after part c
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
        tempColor4 = vec4(mix(fogBaseColor, tempColor4.xyz, fogClamp), tempColor4.w);
    }
    fColor = tempColor4;

    
} 

