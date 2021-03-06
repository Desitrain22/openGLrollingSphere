/***************************
 * File: vshader42.glsl:
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

in  vec4 vPosition;
in  vec4 vColor;
in vec2 vTexCoord;

out vec2 textCoords;
out vec4 color;

//assignment 4a; reference: https://stackoverflow.com/questions/4421261/vertex-shader-vs-fragment-shader

out vec4 eye; // position relative to eye fram
out vec4 truePosition; // position relative to 0 


uniform mat4 model_view;
uniform mat4 projection;

void main() 
{
    vec4 vPosition4 = vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);
    vec4 vColor4 = vec4(vColor.r, vColor.g, vColor.b, vColor.w);
    textCoords = vTexCoord;

    eye = model_view * vPosition;
    truePosition = vPosition; //
    
    // YJC: Original, incorrect below:
    //      gl_Position = projection*model_view*vPosition/vPosition.w;

    gl_Position = projection * model_view * vPosition4;

    color = vColor4;
} 
