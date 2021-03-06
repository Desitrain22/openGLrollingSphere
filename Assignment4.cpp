#include "iostream"
#include "fstream"
#include "Angel-yjc.h"
#include "vector"
#define PI 3.14159265
using namespace std;

typedef Angel::vec3  color3;
typedef Angel::vec3  point3;

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

GLuint Angel::InitShader(const char* vShaderFile, const char* fShaderFile);

GLuint program[2];       /* shader program object id */
GLuint floor_buffer;  /* vertex buffer object id for floor */
GLuint sphere_buffer;	/*vertex buffer object id for sphere */
GLuint axis_buffer;
GLuint shadow_buffer;

// Projection transformation parameters
GLfloat  fovy = 55.0;  // Field-of-view in Y direction angle (in degrees)
GLfloat  aspect;       // Viewport aspect ratio
GLfloat  zNear = 0.5, zFar = 50.0;

GLfloat angle = 0.0; // rotation angle in degrees
vec4 init_eye(7.0, 3.0, -10.0, 1.0); // initial viewer position
vec4 eye = init_eye;               // current viewer position

int animationFlag = 1; // 1: animation; 0: non-animation. Toggled by key 'a' or 'A'

int sphereFlag = 0;   // 1: solid cube; 0: wireframe cube. Toggled by key 'c' or 'C'
int floorFlag = 1;  // 1: solid floor; 0: wireframe floor. Toggled by key 'f' or 'F'
int rolling = 0; // Whether or not B has been pressed to start the ball rolling

mat4 totalRolled = mat4(); //track rolling status. Thank you Carmine Guida's 3113 class for teaching me this mechanic
float rollingSpeed = 0.005;
float tick = 0.0f; //ticks keeps track of how many updates/frames since the starting one
float translateTick = 0.0f;

vec3 a = vec3(3.0f, 1.0f, 5.0f);
vec3* startPoint = &a;
vec3 b = vec3(-2.0f, 1.0f, -2.5f);
vec3 c = vec3(2.0f, 1.0f, -4.0f);
vec3 rollingVec = vec3(b - a); //the vector direction in which we are moving
vec3 rollingPoint = normalize(cross(rollingVec, vec3(0.0f, 1.0f, 0.0f))); //the axis on which we're rotating

int totalShadows = 0;
mat4 shadows = mat4();
vec4 shadowColor = vec4(0.25f, 0.25f, 0.25f, 0.65f);
vec4 yPlane = vec4(0.0f, 1.0f, 0.0f, 0.0f); //where the shadows are cast on
bool sphereShadow = false;

const int floor_NumVertices = 6; //(1 face)*(2 triangles/face)*(3 vertices/triangle)
point4 floor_points[floor_NumVertices]; // positions for all vertices
color4 floor_colors[floor_NumVertices]; // colors for all vertices
vec3 floorNormals[6];
void updateShadows();

//Assignment3
bool programSelect = false;
vec4 light1 = vec4(-14.0f, 12.0f, -3.0f, 1.0f);
vec4 light1Color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
vec4 ambientWhite = vec4(1.0f, 1.0f, 1.0f, 1.0f); //ambient white
vec4 ambientBlack = vec4(0.0f, 0.0f, 0.0f, 1.0f); //directional light black ambient color

//part c global and directional
vec4 getProduct(vec4 surface, vec4 light)
{
    return light * surface;
}
int smoothOrFlat = 1;
int spotOrPoint = 0;

vec4 globalAmbientWhite = ambientWhite;

vec4 globalDirectional = vec4(0.1, 0.0, -1.0, 0.0); //vector, distanct directional light
vec4 globalDirectionalAmbient = vec4(0.0f, 0.0f, 0.0f, 1.0f);
vec4 globalDiffuseColor = vec4(0.8f, 0.8f, 0.8f, 1.0f); //diffusion color
vec4 globalSpecularColor = vec4(0.2, 0.2, 0.2, 1.0f); //specular light color
float k = 125.0f; //ambient light coefficient, shininess coeff

vec4 justLight = vec4(globalAmbientWhite + globalDirectionalAmbient);
vec4 globalAtts[3] = { globalDiffuseColor, justLight, globalSpecularColor };

vec4 groundDiffuse = vec4(0.0f, 1.0f, 0.0f, 1.0f);
vec4 groundAmbient = vec4(0.2, 0.2, 0.2, 1.0f);
vec4 groundSpecular = vec4(0.0f, 0.0f, 0.0f, 1.0f);
vec4 groundAtts[3] = { groundDiffuse, groundAmbient, groundSpecular };

vec4 sphereDiffuse = vec4(1.0, 0.84f, 0.0, 1.0);
vec4 sphereAmbient = vec4(0.2, 0.2, 0.2, 1.0f);
vec4 sphereSpecular = vec4(0.2, 0.2, 0.2, 1.0f);
vec4 sphereAtts[3] = { sphereDiffuse, sphereAmbient, sphereSpecular };

bool sphereOrGround = true; //true if sphere, false if ground

//part d spot/dir
vec4 spotLightDirection = vec4(-6.0f, 0.0f, -4.5f, 1.0f); // - light1;
vec4 positionalDiffuse = ambientWhite;
vec4 positionalSpecular = ambientWhite;
vec4 positionalAmbient = ambientBlack;
vec4 positionAtts[3] = { positionalDiffuse, positionalAmbient, positionalSpecular };

float attenuation[3] = { 2.0f, 0.01f, 0.001f };
float spotLightAngle = (20.0f / 180.0f) * PI;
float spotExponent = 15.0f;
int lightOn = 0;
int pointLightOn = false;

//assignment 4
int fog = 0;
int fogType = 1;
color4 fogColor = color4(0.7, 0.7, 0.7, 0.5);
//4b
bool blend = false;
//4c
int groundTexture = 0;
vec2 textCoords[6] = { vec2(5.0f, 8.0f), vec2(5.0f, 2.0f), vec2(0.0, 2.0), vec2(0.0, 2.0), vec2(5.0f, 8.0f), vec2(0.0f, 8.0f) };
static GLuint textureSet1;
static GLuint textureSet2;

#define ImageWidth  64
#define ImageHeight 64
GLubyte Image[ImageHeight][ImageWidth][4];
#define	stripeImageWidth 32
GLubyte stripeImage[4 * stripeImageWidth];
/*************************************************************
void image_set_up(void):
  generate checkerboard and stripe images.

* Inside init(), call this function and set up texture objects
  for texture mapping.
  (init() is called from main() before calling glutMainLoop().)
***************************************************************/
void image_set_up(void)
{
    int i, j, c;

    /* --- Generate checkerboard image to the image array ---*/
    for (i = 0; i < ImageHeight; i++)
        for (j = 0; j < ImageWidth; j++)
        {
            c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0));

            if (c == 1) /* white */
            {
                c = 255;
                Image[i][j][0] = (GLubyte)c;
                Image[i][j][1] = (GLubyte)c;
                Image[i][j][2] = (GLubyte)c;
            }
            else  /* green */
            {
                Image[i][j][0] = (GLubyte)0;
                Image[i][j][1] = (GLubyte)150;
                Image[i][j][2] = (GLubyte)0;
            }

            Image[i][j][3] = (GLubyte)255;
        }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    /*--- Generate 1D stripe image to array stripeImage[] ---*/
    for (j = 0; j < stripeImageWidth; j++) {
        /* When j <= 4, the color is (255, 0, 0),   i.e., red stripe/line.
           When j > 4,  the color is (255, 255, 0), i.e., yellow remaining texture
         */
        stripeImage[4 * j] = (GLubyte)255;
        stripeImage[4 * j + 1] = (GLubyte)((j > 4) ? 255 : 0);
        stripeImage[4 * j + 2] = (GLubyte)0;
        stripeImage[4 * j + 3] = (GLubyte)255;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    /*----------- End 1D stripe image ----------------*/

    /*--- texture mapping set-up is to be done in
          init() (set up texture objects),
          display() (activate the texture object to be used, etc.)
          and in shaders.
     ---*/

} /* end function */

point4 axis[9] =
{ point4(0.0,0.0,0.0, 1.0f), point4(10.0, 0.0, 0.0, 1.0f), point4(20.0, 0.0, 0.0, 1.0f),
  point4(0.0,0.0,0.0, 1.0f), point4(0.0, 10.0, 0.0, 1.0f), point4(0.0, 20.0, 0.0, 1.0f),
  point4(0.0,0.0,0.0, 1.0f), point4(0.0, 0.0, 10.0, 1.0f), point4(0.0, 0.0, 20.0, 1.0f) };

color4 colors[9] = { color4(1.0, 0.0, 0.0, 1.0f),color4(1.0, 0.0, 0.0, 1.0f),  color4(1.0, 0.0, 0.0, 1.0f),
    color4(1.0, 0.0, 1.0, 1.0f),color4(1.0, 0.0, 1.0, 1.0f), color4(1.0, 0.0, 1.0, 1.0f),
    color4(0.0, 0.0, 1.0, 1.0f), color4(0.0, 0.0, 1.0, 1.0f),color4(0.0, 0.0, 1.0, 1.0f) };

void floor()
{

    floor_colors[0] = color4(0.0, 1.0, 0.0, 1.0f); floor_points[0] = point4(5.0, 0.0, 8.0, 1.0f); floorNormals[0] = (0.0f, 120.0f, 0.0f);
    floor_colors[1] = color4(0.0, 1.0, 0.0, 1.0f); floor_points[1] = point4(5.0, 0.0, -4.0, 1.0f); floorNormals[1] = (0.0f, 120.0f, 0.0f);
    floor_colors[2] = color4(0.0, 1.0, 0.0, 1.0f); floor_points[2] = point4(-5.0, 0.0, -4.0, 1.0f); floorNormals[2] = (0.0f, 120.0f, 0.0f);

    floor_colors[3] = color4(0.0, 1.0, 0.0, 1.0f);  floor_points[3] = point4(5.0, 0.0, 8.0, 1.0f); floorNormals[3] = (0.0f, 120.0f, 0.0f);
    floor_colors[4] = color4(0.0, 1.0, 0.0, 1.0f);  floor_points[4] = point4(-5.0, 0, -4.0, 1.0f); floorNormals[4] = (0.0f, 120.0f, 0.0f);
    floor_colors[5] = color4(0.0, 1.0, 0.0, 1.0f);  floor_points[5] = point4(-5.0, 0, 8.0, 1.0f); floorNormals[5] = (0.0f, 120.0f, 0.0f);
}


point4 sphere_points[1024 * 3];
color4 sphere_color[1024 * 3];
vec4 shadow_points[1024 * 3];
vec4 shadow_color[1024 * 3];
vec3 normal[1024 * 3];

int numberOfTriangles;
void init() {
    //part 4c
    image_set_up(); //taken from 'texmap.c' handout to generate textures
    //following taken from checker-new.cpp handout
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    /*--- Create and Initialize a texture object ---*/
    glGenTextures(1, &textureSet1);      // Generate texture obj name(s)

    glActiveTexture(GL_TEXTURE0);  // Set the active texture unit to be 0 
    glBindTexture(GL_TEXTURE_2D, textureSet1); // Bind the texture to this texture unit
   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ImageWidth, ImageHeight,
        0, GL_RGBA, GL_UNSIGNED_BYTE, Image);

    glGenTextures(1, &textureSet2);      // Generate texture obj name(s)

    glActiveTexture(GL_TEXTURE1);  // Set the active texture unit to be 1 

    glBindTexture(GL_TEXTURE_1D, textureSet2); // Bind the texture to this texture unit

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, stripeImageWidth,
        0, GL_RGBA, GL_UNSIGNED_BYTE, stripeImage);
    //part 2a
    fstream myFile;
    string name;
    cout << "Enter a file name" << endl;
    cin >> name;

    string skip;

    int numTri;
    myFile.open(name);
    float x, y, z;
    myFile >> numberOfTriangles;

    vector<vec4*> coordinates;
    vector<vec3> normals;
    vec4* pointer1;
    vec4* pointer2;
    vec4* pointer3;
    vec3 normalPointer;
    //First, lets load up our coordinates
    for (int i = 0; i < numberOfTriangles * 3; i += 3) //3 for the number of coordinates in a triangle
    {
        myFile >> skip;

        myFile >> x >> y >> z;
        pointer1 = new vec4(x, y, z, 1.0f);
        coordinates.push_back(pointer1);

        myFile >> x >> y >> z;
        pointer2 = new vec4(x, y, z, 1.0f);
        coordinates.push_back(pointer2);

        myFile >> x >> y >> z;
        pointer3 = new vec4(x, y, z, 1.0f);
        coordinates.push_back(pointer3);

        vec3 norm1 = vec3(pointer3->x - pointer2->x, pointer3->y - pointer2->y, pointer3->z - pointer2->z);
        vec3 norm2 = vec3(pointer3->x - pointer1->x, pointer3->y - pointer1->y, pointer3->z - pointer1->z);
        normalPointer = cross(norm2, norm1);
        normalPointer = normalize(normalPointer);
        normals.push_back(normalPointer);
        normals.push_back(normalPointer);
        normals.push_back(normalPointer);
    }
    myFile.close();

    for (int i = 0; i < (numberOfTriangles * 3); i += 1)
    {
        sphere_points[i] = *(coordinates[i]);
        sphere_color[i] = color4(1.0, 0.84, 0.0f, 1.0f);
        shadow_points[i] = vec3(coordinates[i]->x, coordinates[i]->y, coordinates[i]->z);
        shadow_color[i] = color4(0.25f, 0.25f, 0.25f, 0.65f);
        normal[i] = (normals[i]);
    }

    glGenBuffers(1, &sphere_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, sphere_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(point4) * numberOfTriangles * 3 + sizeof(point4) * numberOfTriangles * 3 +
        sizeof(vec3) * numberOfTriangles * 3, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(point4) * numberOfTriangles * 3, sphere_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(point4) * numberOfTriangles * 3, (sizeof(point4) * numberOfTriangles * 3), sphere_color);
    glBufferSubData(GL_ARRAY_BUFFER, 2 * (sizeof(point4) * numberOfTriangles * 3), (sizeof(point3) * numberOfTriangles * 3), normal);

    glGenBuffers(1, &shadow_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, shadow_buffer);
    updateShadows();
    glBufferData(GL_ARRAY_BUFFER, (sizeof(point4) * numberOfTriangles * 3) * 2, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(point4) * numberOfTriangles * 3, shadow_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(point4) * numberOfTriangles * 3, (sizeof(point4) * numberOfTriangles * 3), shadow_color);
    

    floor();
    glGenBuffers(1, &floor_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, floor_buffer);
    //4c add texture to buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(point4) * 6 + sizeof(point4) * 6 + sizeof(point3) * 6 + sizeof(vec2) * 6, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(point4) * 6, floor_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(point4) * 6, sizeof(point4) * 6, floor_colors);
    glBufferSubData(GL_ARRAY_BUFFER, 2 * sizeof(point4) * 6, sizeof(point3) * 6, floorNormals);
    glBufferSubData(GL_ARRAY_BUFFER, (sizeof(point3)*6) + (2 * sizeof(point4) * 6), sizeof(vec2) * 6, textCoords);

    glGenBuffers(1, &axis_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, axis_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors) + sizeof(axis), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(axis), axis);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(colors), sizeof(colors), colors);

    // Load shaders and create a shader program (to be used in display())
    //program = InitShader("vshader42.glsl", "fshader42.glsl");
    program[0] = InitShader("vshader42.glsl", "fshaderNP.glsl");
    program[1] = InitShader("vshaderNP.glsl", "fshaderNP.glsl");

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.529, 0.807, 0.92, 0.0);
    glLineWidth(1.0);

}

void SetUp_Lighting_Uniform_Vars(mat4 mv)
{
    vec4 ambientProducts;
    vec4 diffuseProducts;
    vec4 specularProducts;
    vec4 pointDiffuse, pointSpecular, pointAmbient;
    if (sphereOrGround)
    {
        diffuseProducts = getProduct(sphereAtts[0], globalAtts[0]);
        ambientProducts = getProduct(sphereAtts[1], globalAtts[1]);
        specularProducts = getProduct(sphereAtts[2], globalAtts[2]);
        k = 125.0f;

        pointDiffuse = getProduct(sphereAtts[0], positionAtts[0]);
        pointAmbient = getProduct(sphereAtts[1], positionAtts[1]);
        specularProducts = getProduct(sphereAtts[2], positionAtts[2]);
        glLineWidth(1.0);
    }
    else
    {
        diffuseProducts = getProduct(groundAtts[0], globalAtts[0]);
        ambientProducts = getProduct(groundAtts[1], globalAtts[1]);
        specularProducts = getProduct(groundAtts[2], globalAtts[2]);
        k = 1.0;
        pointDiffuse = getProduct(groundAtts[0], positionAtts[0]);
        pointAmbient = getProduct(groundAtts[1], positionAtts[1]);
        specularProducts = getProduct(groundAtts[2], positionAtts[2]);
        glLineWidth(1.0);
    }

    glUniform4fv(glGetUniformLocation(program[1], "ambientProduct"), 1, ambientProducts);
    glUniform4fv(glGetUniformLocation(program[1], "specularProduct"), 1, specularProducts);
    glUniform4fv(glGetUniformLocation(program[1], "diffuseProduct"), 1, diffuseProducts);
    glUniform4fv(glGetUniformLocation(program[1], "distantLightVector"), 1, globalDirectional);
    glUniform1i(glGetUniformLocation(program[1], "smoothOrFlat"), smoothOrFlat);
    glUniform1f(glGetUniformLocation(program[1], "shiny"), k);
    glUniform1i(glGetUniformLocation(program[1], "spotOrPoint"), spotOrPoint);
    vec4 light_position_eyeFrame = mv * light1;
    glUniform4fv(glGetUniformLocation(program[1], "pointLight1"), 1, light_position_eyeFrame);
    
    //part d
    mat4 normal4Matrix = mat4WithUpperLeftMat3(NormalMatrix(mv, 1));

    glUniform4fv(glGetUniformLocation(program[1], "pointLightVector"), 1, normal4Matrix * (spotLightDirection - light1));
    glUniform4fv(glGetUniformLocation(program[1], "pointAmbient"), 1, pointAmbient);
    glUniform4fv(glGetUniformLocation(program[1], "pointDiffuse"), 1, pointDiffuse);
    glUniform4fv(glGetUniformLocation(program[1], "pointSpecular"), 1, pointSpecular);
    glUniform1f(glGetUniformLocation(program[1], "atten"), attenuation[0]);
    glUniform1f(glGetUniformLocation(program[1], "atten2"), attenuation[1]);
    glUniform1f(glGetUniformLocation(program[1], "atten3"), attenuation[2]);
    glUniform1f(glGetUniformLocation(program[1], "angle"), spotLightAngle);
    glUniform1f(glGetUniformLocation(program[1], "exp"), spotExponent);
    glUniform1i(glGetUniformLocation(program[1], "pointLightOn"), pointLightOn);
    //project 4a
    glUniform1i(glGetUniformLocation(program[programSelect], "fogOn"), fog);
    glUniform1i(glGetUniformLocation(program[programSelect], "fogType"), fogType);
    //4c
    glUniform1i(glGetUniformLocation(program[programSelect], "textureFlag"), groundTexture);

}


bool drawingShadow = 0;

void drawObj(GLuint buffer, int num_vertices, mat4& mv)
{
    //--- Activate the vertex buffer object to be drawn ---//
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    /*----- Set up vertex attribute arrays for each vertex attribute -----*/
    GLuint vPosition = glGetAttribLocation(program[programSelect], "vPosition");
    if (drawingShadow)
    {
        vPosition = glGetAttribLocation(program[0], "vPosition");
    }
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    if (groundTexture && !sphereOrGround)
    {
        GLuint vTexCoord = glGetAttribLocation(program[1], "vTexCoord");
        glEnableVertexAttribArray(vTexCoord);
        glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,//point4 points, point4 color, point3 normals,
            BUFFER_OFFSET(sizeof(point4) * num_vertices + sizeof(point4) * num_vertices + sizeof(color3) * num_vertices));
    }

    if (drawingShadow || (programSelect == 0))
    {

        GLuint vColor = glGetAttribLocation(program[0], "vColor");
        glEnableVertexAttribArray(vColor);
        glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
            BUFFER_OFFSET(sizeof(point4) * num_vertices));
        glDrawArrays(GL_TRIANGLES, 0, num_vertices);
        glDisableVertexAttribArray(vPosition);
        glDisableVertexAttribArray(vColor);

    }
    else
    {
        //mat3 normalMatrix = NormalMatrix(mv, 1);
        //glUniformMatrix3fv(glGetUniformLocation(program[1], "normal"),1, GL_TRUE, normalMatrix);
        GLuint vNormal = glGetAttribLocation(program[1], "vNormal");
        glEnableVertexAttribArray(vNormal);
        glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
            BUFFER_OFFSET(sizeof(point4) * num_vertices + sizeof(point4) * num_vertices));
        glDrawArrays(GL_TRIANGLES, 0, num_vertices);
        glDisableVertexAttribArray(vPosition);
        glDisableVertexAttribArray(vNormal);

    }

    // the offset is the (total) size of the previous vertex attribute array(s)
}
//----------------------------------------------------------------------------

void display(void)
{
    GLuint  model_view;  // model-view matrix uniform shader variable location
    GLuint  projection;  // projection matrix uniform shader variable location

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.529, 0.807, 0.92, 1.0);

    glUseProgram(program[programSelect]); // Use the shader program
    model_view = glGetUniformLocation(program[programSelect], "model_view");
    projection = glGetUniformLocation(program[programSelect], "projection");

    glUniform1i(glGetUniformLocation(program[programSelect], "texture_2D"), 0);
    mat4  p = Perspective(fovy, aspect, zNear, zFar);
    glUniformMatrix4fv(projection, 1, GL_TRUE, p); // GL_TRUE: matrix is row-major

    vec4    at(-7.0, -3.0, 10.0, 1.0);
    at = normalize(at);
    vec4    up(0.0, 1.0, 0.0, 0.0);
    mat4  mv = LookAt(eye, at, up);

    sphereOrGround = true;
    SetUp_Lighting_Uniform_Vars(mv);

    if (rolling)
    {
        totalRolled = (Rotate(rollingSpeed * float(180.0 / PI), rollingPoint.x, rollingPoint.y, rollingPoint.z)) * totalRolled;
    }

    mv = mv * Translate(vec3(*startPoint + (normalize(rollingVec) * (rollingSpeed * translateTick))))
        * totalRolled;

    glUniformMatrix4fv(model_view, 1, GL_TRUE, mv); // GL_TRUE: matrix is row-major

    mat3 normal_matrix = NormalMatrix(mv, 1);
    glUniformMatrix3fv(glGetUniformLocation(program[programSelect], "normal"), 1, GL_TRUE, normal_matrix);

    if (sphereFlag == 1) // Filled cube
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else              // Wireframe cube
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(1.0);
    glUniform1i(glGetUniformLocation(program[programSelect], "textureFlag"), 0);

    drawObj(sphere_buffer, numberOfTriangles * 3, mv);  // draw the cube

    glUniform1i(glGetUniformLocation(program[programSelect], "textureFlag"), groundTexture);
    //Draw floor first
    glDepthMask(GL_FALSE);
    sphereOrGround = false;
    mv = LookAt(eye, at, up) * Translate(0.3f, 0.0f, 0.0f); //* Scale(1.6f, 1.5f, 3.3f);

    glUniformMatrix4fv(projection, 1, GL_TRUE, p); // GL_TRUE: matrix is row-major
    glUniformMatrix4fv(model_view, 1, GL_TRUE, mv); // GL_TRUE: matrix is row-major

    SetUp_Lighting_Uniform_Vars(mv);
    normal_matrix = NormalMatrix(mv, 1);
    glUniform1i(glGetUniformLocation(program[programSelect], "smoothOrFlat"), 1);
    glUniformMatrix3fv(glGetUniformLocation(program[programSelect], "normal"), 1, GL_TRUE, normal_matrix);

    if (floorFlag == 1) // Filled floor
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else              // Wireframe floor
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    drawObj(floor_buffer, floor_NumVertices, mv);  // draw the floor

    if (sphereShadow && eye.y > 0)
    {
        //4b
        if (blend)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        glUseProgram(program[0]); // Use the shader program
        model_view = glGetUniformLocation(program[0], "model_view");
        projection = glGetUniformLocation(program[0], "projection");
        glUniformMatrix4fv(projection, 1, GL_TRUE, p); // GL_TRUE: matrix is row-major
        mat4  mv = LookAt(eye, at, up);
        if (rolling)
        {
            mv = mv * shadows * Translate(vec3(*startPoint + (normalize(rollingVec) * (rollingSpeed * translateTick))))
                * totalRolled;
        }
        else
        {
            mv = mv * shadows * Translate(a) * totalRolled;
        }
        glUniformMatrix4fv(model_view, 1, GL_TRUE, mv); // GL_TRUE: matrix is row-major
        if (sphereFlag == 1) // Filled cube
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        else              // Wireframe cube
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(1.0);

        drawingShadow = true;
        sphereOrGround = true;
        glDepthMask(GL_TRUE);
        glUniform1i(glGetUniformLocation(program[programSelect], "textureFlag"), 0);
        drawObj(shadow_buffer, numberOfTriangles * 3, mv);
        drawingShadow = false;
        glUseProgram(program[programSelect]); // Use the shader program
        model_view = glGetUniformLocation(program[programSelect], "model_view");
        projection = glGetUniformLocation(program[programSelect], "projection");
        if (blend)
        {
            glDisable(GL_BLEND);    
        }
    }
    //draw floor again
    sphereOrGround = false;

    glUniform1i(glGetUniformLocation(program[programSelect], "textureFlag"), groundTexture);
    glDepthMask(GL_TRUE);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    mv = LookAt(eye, at, up) * Translate(0.3f, 0.0f, 0.0f); //* Scale(1.6f, 1.5f, 3.3f);
    glUniformMatrix4fv(projection, 1, GL_TRUE, p); // GL_TRUE: matrix is row-major
    glUniformMatrix4fv(model_view, 1, GL_TRUE, mv); // GL_TRUE: matrix is row-major
    SetUp_Lighting_Uniform_Vars(mv);
    normal_matrix = NormalMatrix(mv, 1);
    glUniform1i(glGetUniformLocation(program[programSelect], "smoothOrFlat"), 1);
    glUniformMatrix3fv(glGetUniformLocation(program[programSelect], "normal"), 1, GL_TRUE, normal_matrix);

    if (floorFlag == 1) // Filled floor
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else              // Wireframe floor
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    drawObj(floor_buffer, floor_NumVertices, mv);  // draw the floor
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    glDepthMask(GL_TRUE);
    glUseProgram(program[0]); // Use the shader program
    //axis despawn when light is on! so this is my cheap fix (Suggested by Andrew Qu, Tandon 2021)

    model_view = glGetUniformLocation(program[0], "model_view");
    projection = glGetUniformLocation(program[0], "projection");
    glUniformMatrix4fv(projection, 1, GL_TRUE, p); // GL_TRUE: matrix is row-major
    glUniformMatrix4fv(model_view, 1, GL_TRUE, mv); // GL_TRUE: matrix is row-major
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(1.0); //cant see axis from below like in example 
    drawObj(axis_buffer, 9, mv);
    glutSwapBuffers();
}

//----------------------------------------------------------------------------

void roll()
{
    tick += 1.0f;
    translateTick += 1.0f;
    if (*startPoint == a)
    {
        rollingVec = vec3(b - a);
        if (rollingSpeed * translateTick >= 9.01388) //distance from a to b
        {
            startPoint = &b;
            rollingVec = vec3(c - b);
            translateTick = 0.0f;
        }
    }
    else if (*startPoint == b)
    {
        rollingVec = vec3(c - b);
        if (rollingSpeed * translateTick >= 4.272) //distance b to c
        {
            startPoint = &c;
            rollingVec = vec3(a - c);
            translateTick = 0.0f;
        }
    }
    else //startPoint = c
    {
        rollingVec = vec3(a - c);
        if (rollingSpeed * translateTick >= 9.05539) //distance c to a
        {
            startPoint = &a;
            rollingVec = vec3(b - a);
            translateTick = 0.0f;
        }
    }

    rollingPoint = normalize(cross(vec3(0.0f, 1.0f, 0.0f), rollingVec));//we take the cross product with the y axis vector since that's orthogonal to the plane we roll on
}

void updateShadows() //Reference: https://sites.cs.ucsb.edu/~yfwang/courses/cs180/discussion/Shadows.pdf
{
    float d = dot(light1, yPlane); //distance from light to plane (dot product) squared
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (i == j)
            {
                shadows[i][j] = d - light1[i] * yPlane[j];  //intensity is distance between light and object 
            }
            else
            {
                shadows[i][j] = 0.0f - light1[i] * yPlane[j]; //intensity is just distance between object and plane
            }
        }
    }
    return;
}

//---------------------------------------------------------------------------
void idle(void)
{

    if (rolling)
    {
        roll();
        updateShadows();
    }

    // angle += 0.03f;
    // angle += 1.0f;    //YJC: change this value to adjust the cube rotation speed.
    glutPostRedisplay();
}
void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        if (rolling) {
            animationFlag = 1 - animationFlag;
            if (animationFlag == 1) glutIdleFunc(idle);
            else                    glutIdleFunc(NULL);
        }
    }
}
void leftMouseMenu(int id) {
    switch (id) {
    case 1:
        eye = init_eye; //reset to initial eye given by PDF
        animationFlag = 1;
        glutIdleFunc(idle);
        break;
    case 2:
        exit(EXIT_SUCCESS);
        break;
    case 3:
        blend = 1 - blend;
        break;
    case 4:
        groundTexture = 1 - groundTexture;
    }
    glutPostRedisplay();
}

void shadowMenu(int id)
{
    switch (id)
    {
    case 1:
        sphereShadow = true;
        cout << "Shadow: on" << endl;
        break;
    case 2:
        sphereShadow = false;
        cout << "Shadow: off" << endl;
        break;
    }
}

void lightMenu(int id)
{
    switch (id)
    {
    case 1:
        lightOn = false;
        programSelect = 0;
        cout << "Light off" << endl;
        break;
    case 2:
        lightOn = true;
        programSelect = 1;
        cout << "Light on" << endl;
        break;
    }
}

void shadingMenu(int id)
{
    switch (id)
    {
    case 1:
        smoothOrFlat = 0;
        cout << "Flat Shading";
        break;
    case 2:
        smoothOrFlat = 1;
        cout << "Smooth Shading";
        break;
    }
}

void sourceMenu(int id)
{
    switch (id)
    {
    case 1:
        pointLightOn = 1;
        spotOrPoint = 1;
        cout << "Spot Light" << endl;
        break;

    case 2:
        spotOrPoint = 0;
        pointLightOn = 1;
        cout << "Point Source Light" << endl;
        break;
    case 3:
        pointLightOn = 0;
    }
}

void frameMenu(int id)
{
    switch (id)
    {
    case 1:
        sphereFlag = false;
        cout << "Sphere: wired" << endl;
        break;
    case 2:
        sphereFlag = true;
        cout << "Sphere: Solid" << endl;
        break;
    }
}

void fogMenu(int id)
{
    switch (id)
    {
    case 1:
        fog = 1 - fog;
        break;
    case 2:
        fog = 1;
        fogType = 1;
        break;
    case 3:
        fog = 1;
        fogType = 2;
        break;
    case 4:
        fog = 1;
        fogType = 3;
        break;
    }
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 033: // Escape Key
    case 'q': case 'Q':
        exit(EXIT_SUCCESS);
        break;

    case 'X': eye[0] += 1.0; break;
    case 'x': eye[0] -= 1.0; break;
    case 'Y': eye[1] += 1.0; break;
    case 'y': eye[1] -= 1.0; break;
    case 'Z': eye[2] += 1.0; break;
    case 'z': eye[2] -= 1.0; break;

    case 'b': case 'B': // Toggle between animation and non-animation
        rolling = 1;
        break;

    case 'c': case 'C': // Toggle between filled and wireframe cube
        sphereFlag = 1 - sphereFlag;
        break;

    case 'f': case 'F': // Toggle between filled and wireframe floor
        floorFlag = 1 - floorFlag;
        break;

    case ' ':  // reset to initial viewer/eye position
        eye = init_eye;
        break;
    }
    glutPostRedisplay();
}
//----------------------------------------------------------------------------
void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    aspect = (GLfloat)width / (GLfloat)height;
    glutPostRedisplay();
}
//----------------------------------------------------------------------------
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
#ifdef __APPLE__ // Enable core profile of OpenGL 3.2 on macOS.
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);
#else
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif
    glutInitWindowSize(512, 512);
    glutCreateWindow("npp288 Rolling Sphere");

#ifdef __APPLE__ // on macOS
    // Core profile requires to create a Vertex Array Object (VAO).
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
#else           // on Linux or Windows, we still need glew
    /* Call glewInit() and error checking */
    int err = glewInit();
    if (GLEW_OK != err)
    {
        printf("Error: glewInit failed: %s\n", (char*)glewGetErrorString(err));
        exit(1);
    }
#endif

    // Get info of GPU and supported OpenGL version
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("OpenGL version supported %s\n", glGetString(GL_VERSION));

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);

    glutMouseFunc(mouse);

    GLuint subMenuPointer = glutCreateMenu(shadowMenu);
    glutAddMenuEntry("Yes", 1);
    glutAddMenuEntry("No", 2);


    GLuint subFramePointer = glutCreateMenu(frameMenu);
    glutAddMenuEntry("Wired", 1);
    glutAddMenuEntry("Solid", 2);

    GLuint subShadingPointer = glutCreateMenu(shadingMenu);
    glutAddMenuEntry("Flat Shading", 1);
    glutAddMenuEntry("Smooth Shading", 2);


    GLuint subLightPointer = glutCreateMenu(lightMenu);
    glutAddMenuEntry("No", 1);
    glutAddMenuEntry("Yes", 2);

    GLuint subSourcePointer = glutCreateMenu(sourceMenu);
    glutAddMenuEntry("Spot Light", 1);
    glutAddMenuEntry("Point Source", 2);
    glutAddMenuEntry("None", 3);

    GLuint subFogPointer = glutCreateMenu(fogMenu);
    glutAddMenuEntry("Fog On/Off", 1);
    glutAddMenuEntry("Fog Linear", 2);
    glutAddMenuEntry("Fog Expo", 3);
    glutAddMenuEntry("Fog Expo-Quad", 4);

    glutCreateMenu(leftMouseMenu);

    glutAddMenuEntry("Default View", 1);
    glutAddMenuEntry("Quit", 2);
    glutAddMenuEntry("Blending on/off", 3);
    glutAddMenuEntry("Floor Texture", 4);

    glutAddSubMenu("Shadows", subMenuPointer);
    glutAddSubMenu("Enable Lighting", subLightPointer);
    glutAddSubMenu("Wire Frame Sphere", subFramePointer);
    glutAddSubMenu("Shading", subShadingPointer);
    glutAddSubMenu("Light Source", subSourcePointer);
    glutAddSubMenu("Fog Menu", subFogPointer);
    glutAttachMenu(GLUT_LEFT_BUTTON);

    init();
    glutMainLoop();
    return 0;
}
