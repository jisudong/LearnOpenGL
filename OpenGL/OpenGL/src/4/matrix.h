//
//  matrix.h
//  OpenGL
//
//  Created by apple on 2020/5/9.
//  Copyright © 2020 apple. All rights reserved.
//

#ifndef matrix_h
#define matrix_h

#include "GLTools.h"
#include "GLMatrixStack.h"
#include "GLFrame.h"
#include "GLFrustum.h"
#include "GLGeometryTransform.h"
#include "GLBatch.h"
#include "StopWatch.h"

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

GLFrustum           maxtrix_viewFrustum;
GLShaderManager     maxtrix_shaderManager;
GLTriangleBatch     maxtrix_torusBatch;
GLGeometryTransform maxtrix_tranformPipeline;

void maxtrix_changeSize(int w, int h)
{
    if (h == 0) {
        h = 1;
    }
    
    glViewport(0, 0, w, h);
    
    maxtrix_viewFrustum.SetPerspective(45.f, float(w)/float(h), 0.1f, 100.f);
}

void maxtrix_renderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    // 建立基于时间变化的动画
    static CStopWatch rotTimer;
    // 当前时间 * 60s
    float yRot = rotTimer.GetElapsedSeconds() * 60.f;
    
    M3DMatrix44f mTranslate, mRotate, mModelView, mModelViewProjection;
    
    //创建一个4*4矩阵变量，将花托沿着Z轴负方向移动2.5个单位长度
    m3dTranslationMatrix44(mTranslate, 0.0f, 0.0f, -2.5f);
    
    m3dRotationMatrix44(mRotate, m3dDegToRad(yRot), 0.0f, 1.0f, 0.0f);
    
    m3dMatrixMultiply44(mModelView, mTranslate, mRotate);
    
    m3dMatrixMultiply44(mModelViewProjection, maxtrix_viewFrustum.GetProjectionMatrix(), mModelView);
    
    GLfloat vBlack[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    
    maxtrix_shaderManager.UseStockShader(GLT_SHADER_FLAT, mModelViewProjection, vBlack);
    maxtrix_torusBatch.Draw();
    
    glutSwapBuffers();
    glutPostRedisplay();
}

void maxtrix_setupRC()
{
    glClearColor(0.8, 0.8, 0.8, 1.f);
    maxtrix_shaderManager.InitializeStockShaders();
    glEnable(GL_DEPTH_TEST);
    
    gltMakeSphere(maxtrix_torusBatch, 0.4f, 10, 20);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void testMatrix(int argc,char *argv[])
{
    
    gltSetWorkingDirectory(argv[0]);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(800, 600);
    glutCreateWindow("ModelViewProjection Maxtrix");
    glutReshapeFunc(maxtrix_changeSize);
    glutDisplayFunc(maxtrix_renderScene);
    
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return;
    }
    
    maxtrix_setupRC();
    glutMainLoop();
}

#endif /* matrix_h */
