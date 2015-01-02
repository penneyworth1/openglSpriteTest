//
//  core.c
//  SpriteTest
//
//  Created by Steven Stewart on 1/1/15.
//  Copyright (c) 2015 Maaz Kamani. All rights reserved.
//

#include "core.h"

#define pi 3.14159265358979323846264338327
#define PI_OVER_360 0.0087266

GLuint _positionSlot;
GLuint _colorSlot;
GLuint _texCoordSlot;
GLuint uniform_mvpMatrix;
GLuint uniform_texture;

GLfloat screenWidthInPixels;
GLfloat screenHeightInPixels;

float timeDiff = 0;
float lastTimeStamp = 0;

float modelMatrix[16] = {0};
float perspectiveMatrix[16] = {0};
float translationMatrix[16] = {0};
float rotationMatrix[16] = {0};
float transRotMatrix[16] = {0}; //To store the result of the translation matrix multiplied by the rotation matrix
float angle = 0;
float rotAxis[3] = {1,.1,.5};
float quat[4] = {0}; //rotation quaternion
float xx,yy,zz,xy,xz,yz,wx,wy,wz;

//testing
GLuint textureReference;

typedef struct
{
    float Position[3];
    float Color[4];
    float TexCoord[2];
} Vertex;

const Vertex Vertices[] = {
    {{-1,-1,-10},{1,1,1,1},{0,0}},
    {{-1,1,-10},{1,1,1,1},{0,1}},
    {{1,-1,-10},{1,1,1,1},{1,0}},
    {{1,1,-10},{1,1,1,1},{1,1}}
};

const GLuint Indices[] = {
    0,1,2,
    2,1,3
};

//testing
void setTextureReference(GLuint textureReferencePar)
{
    textureReference = textureReferencePar;
}



void buildPerspProjMat(float* m, float fov, float aspect, float znear, float zfar)
{
    float xymax = znear * tan(fov* PI_OVER_360);
    float depth = zfar - znear;
    float q = -(zfar + znear) / depth;
    float qn = -2 * (zfar * znear) / depth;
    float w = znear / (xymax * aspect);
    float h = znear / xymax;
    m[0]  = w; m[1]  = 0; m[2]  = 0;  m[3]  =  0;
    m[4]  = 0; m[5]  = h; m[6]  = 0;  m[7]  =  0;
    m[8]  = 0; m[9]  = 0; m[10] = q;  m[11] = -1;
    m[12] = 0; m[13] = 0; m[14] = qn; m[15] =  0;
}
void setTranslationMatrix(float* m, float tx, float ty, float tz)
{
    m[0]  = 1; m[4]  = 0; m[8]  = 0;  m[12] = tx;
    m[1]  = 0; m[5]  = 1; m[9]  = 0;  m[13] = ty;
    m[2]  = 0; m[6]  = 0; m[10] = 1;  m[14] = tz;
    m[3]  = 0; m[7]  = 0; m[11] = 0;  m[15] =  1;
}
void setRotationMatrx(float* m, const float* q)
{
    xx = q[0] * q[0];
    yy = q[1] * q[1];
    zz = q[2] * q[2];
    xy = q[0] * q[1];
    xz = q[0] * q[2];
    yz = q[1] * q[2];
    wx = q[3] * q[0];
    wy = q[3] * q[1];
    wz = q[3] * q[2];
    
    m[0]  = 1 - 2 * (yy + zz); m[4] =     2 * (xy - wz); m[8]  =     2 * (xz + wy);  m[12] = 0;
    m[1]  =     2 * (xy + wz); m[5] = 1 - 2 * (xx + zz); m[9]  =     2 * (yz - wx);  m[13] = 0;
    m[2]  =     2 * (xz - wy); m[6] =     2 * (yz + wx); m[10] = 1 - 2 * (xx + yy);  m[14] = 0;
    m[3]  =                 0; m[7] =                 0; m[11] = 0                ;  m[15] = 1;
}
void matMult4x4by4x4(float* m, const float* a, const float* b)
{
    m[0]  = a[0]*b[0]  + a[4]*b[1]  + a[8]* b[2]  + a[12]*b[3];
    m[1]  = a[1]*b[0]  + a[5]*b[1]  + a[9]* b[2]  + a[13]*b[3];
    m[2]  = a[2]*b[0]  + a[6]*b[1]  + a[10]*b[2]  + a[14]*b[3];
    m[3]  = a[3]*b[0]  + a[7]*b[1]  + a[11]*b[2]  + a[15]*b[3];
    m[4]  = a[0]*b[4]  + a[4]*b[5]  + a[8]* b[6]  + a[12]*b[7];
    m[5]  = a[1]*b[4]  + a[5]*b[5]  + a[9]* b[6]  + a[13]*b[7];
    m[6]  = a[2]*b[4]  + a[6]*b[5]  + a[10]*b[6]  + a[14]*b[7];
    m[7]  = a[3]*b[4]  + a[7]*b[5]  + a[11]*b[6]  + a[15]*b[7];
    m[8]  = a[0]*b[8]  + a[4]*b[9]  + a[8]* b[10] + a[12]*b[11];
    m[9]  = a[1]*b[8]  + a[5]*b[9]  + a[9]* b[10] + a[13]*b[11];
    m[10] = a[2]*b[8]  + a[6]*b[9]  + a[10]*b[10] + a[14]*b[11];
    m[11] = a[3]*b[8]  + a[7]*b[9]  + a[11]*b[10] + a[15]*b[11];
    m[12] = a[0]*b[12] + a[4]*b[13] + a[8]* b[14] + a[12]*b[15];
    m[13] = a[1]*b[12] + a[5]*b[13] + a[9]* b[14] + a[13]*b[15];
    m[14] = a[2]*b[12] + a[6]*b[13] + a[10]*b[14] + a[14]*b[15];
    m[15] = a[3]*b[12] + a[7]*b[13] + a[11]*b[14] + a[15]*b[15];
}

GLuint compileShader(const GLchar* shaderSourceString, GLenum shaderType)
{
    GLuint shaderHandle = glCreateShader(shaderType);
    glShaderSource(shaderHandle, 1, &shaderSourceString, NULL);
    glCompileShader(shaderHandle);
    GLint compileSuccess;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compileSuccess);
    if (compileSuccess == GL_FALSE)
    {
        GLchar messages[256];
        glGetShaderInfoLog(shaderHandle, sizeof(messages), 0, &messages[0]);
        exit(1);
    }
    return shaderHandle;
}
void compileShaders()
{
    GLchar vertexShaderString[] =
    "attribute vec3 a_position; \n"
    "attribute vec4 a_color; \n"
    "varying vec4 v_color; \n"
    "uniform mat4 u_mvpMatrix; \n"
    "attribute vec2 a_texCoordIn;  \n"
    "varying vec2 v_texCoordOut;  \n"
    "void main(void) \n"
    "{ \n"
    "    v_texCoordOut = a_texCoordIn; \n"
    "    v_color = a_color; \n"
    "    gl_Position = u_mvpMatrix * vec4(a_position,1); \n"
    "} \0";
    
    GLchar fragmentShaderString[] =
    "varying lowp vec4 v_color; \n"
    "varying lowp vec2 v_texCoordOut; \n"
    "uniform sampler2D u_texture; \n"
    "void main(void) \n"
    "{ \n"
    "    gl_FragColor = v_color * texture2D(u_texture, v_texCoordOut); \n" //; \n"//
    "} \0";
    
    GLuint compiledVertexShaderReference = compileShader(vertexShaderString, GL_VERTEX_SHADER);
    GLuint compiledFragmentShaderReference = compileShader(fragmentShaderString, GL_FRAGMENT_SHADER);
    GLuint programHandle = glCreateProgram();
    glAttachShader(programHandle, compiledVertexShaderReference);
    glAttachShader(programHandle, compiledFragmentShaderReference);
    glLinkProgram(programHandle);
    GLint linkSuccess;
    glGetProgramiv(programHandle, GL_LINK_STATUS, &linkSuccess);
    if (linkSuccess == GL_FALSE)
    {
        GLchar messages[256];
        glGetProgramInfoLog(programHandle, sizeof(messages), 0, &messages[0]);
        exit(1);
    }
    glUseProgram(programHandle);
    _positionSlot = glGetAttribLocation(programHandle, "a_position");
    _colorSlot = glGetAttribLocation(programHandle, "a_color");
    uniform_mvpMatrix = glGetUniformLocation(programHandle, "u_mvpMatrix");
    
    _texCoordSlot = glGetAttribLocation(programHandle, "a_texCoordIn");
    glEnableVertexAttribArray(_texCoordSlot);
    uniform_texture = glGetUniformLocation(programHandle, "u_texture");
    
    glEnableVertexAttribArray(_positionSlot);
    glEnableVertexAttribArray(_colorSlot);
}

void setupVBOs()
{
    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    
    GLuint indexBuffer;
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
}

void initView(float screenWidthInPixelsPar, float screenHeightInPixelsPar)
{
    compileShaders();
    setupVBOs();
    
    screenWidthInPixels = screenWidthInPixelsPar;
    screenHeightInPixels = screenHeightInPixelsPar;
    
    float fov=30.0f; // in degrees
    float aspect=((float)screenWidthInPixels)/screenHeightInPixels;
    float znear=10.0f;
    float zfar=1000.0f;
    buildPerspProjMat(perspectiveMatrix, fov, aspect, znear, zfar);
    glViewport(0, 0, screenWidthInPixels, screenHeightInPixels);
    
    glClearColor(0, 0.1, 0.4, 1.0);
    glEnable(GL_DEPTH_TEST);
    
    //Normalize rot axis
    float axisNorm = sqrtf(rotAxis[0]*rotAxis[0] + rotAxis[1]*rotAxis[1] + rotAxis[2]*rotAxis[2]);
    rotAxis[0] =  rotAxis[0] / axisNorm;
    rotAxis[1] =  rotAxis[1] / axisNorm;
    rotAxis[2] =  rotAxis[2] / axisNorm;
    
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
}

void renderScene(int timeDiffMillies)
{
    angle += 0.001 * timeDiffMillies; if(angle > 2*pi) angle = 0;
    
    setTranslationMatrix(translationMatrix, 0,0,-1);//10*cos(angle), 2*sin(angle), -100 + 100*sin(angle));
    matMult4x4by4x4(modelMatrix, perspectiveMatrix, translationMatrix);
    glUniformMatrix4fv(uniform_mvpMatrix, 1, 0, modelMatrix);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureReference);
    glUniform1i(uniform_texture, 0);
    
    glVertexAttribPointer(_positionSlot, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(_colorSlot, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) (sizeof(float) * 3));
    glVertexAttribPointer(_texCoordSlot, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) (sizeof(float) * 7));
    
    
    
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, sizeof(Indices)/sizeof(Indices[0]), GL_UNSIGNED_INT, 0);
}




