//
//  core.h
//  SpriteTest
//
//  Created by Steven Stewart on 1/1/15.
//  Copyright (c) 2015 Maaz Kamani. All rights reserved.
//

#ifndef __SpriteTest__core__
#define __SpriteTest__core__

/////////iOS-specific inludes
#include <OpenGLES/ES2/gl.h>
/////////

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <time.h>

#endif /* defined(__SpriteTest__core__) */

void buildPerspProjMat(float *m, float fov, float aspect, float znear, float zfar);
void initView(float screenWidthInPixelsPar, float screenHeightInPixelsPar);
void renderScene(int timeDiffMillies);
void compileShaders();
GLuint compileShader(const GLchar* shaderSourceString, GLenum shaderType);

//testing
void setTextureReference(GLuint textureReferencePar);
