//
//  exercise.h
//  OpenGL
//
//  Created by apple on 2020/5/14.
//  Copyright © 2020 apple. All rights reserved.
//

#ifndef exercise_h
#define exercise_h

#include "GLTools.h"
#include "GLShaderManager.h"
#include "GLFrustum.h"
#include "GLBatch.h"
#include "GLMatrixStack.h"
#include "GLGeometryTransform.h"
#include "StopWatch.h"

#include <math.h>
#include <stdio.h>

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif


GLShaderManager     exercise_shaderManager;
GLMatrixStack       exercise_modelViewMatrix;
GLMatrixStack       exercise_projectionMatrix;
GLFrustum           exercise_viewFrustum;
GLGeometryTransform exercise_transformPipeline;

GLTriangleBatch     exercise_torusBatch;
GLTriangleBatch     exercise_sphereBatch;
GLBatch             exercise_floorBatch;

GLFrame             exercise_cameraFrame;

#define NUM_SPHERES 50
GLFrame spheres[NUM_SPHERES];

void exercise_setupRC()
{
    glClearColor(0.0, 0.0f, 0.0f, 1.0f);
    exercise_shaderManager.InitializeStockShaders();
    
    glEnable(GL_DEPTH_TEST);
    // 设置地板顶点数据
    exercise_floorBatch.Begin(GL_LINES, 324);
    for (GLfloat x = -20; x < 20; x += 0.5) {
        exercise_floorBatch.Vertex3f(x, -0.55f, 20.f);
        exercise_floorBatch.Vertex3f(x, -0.55f, -20.f);
        exercise_floorBatch.Vertex3f(20.f, -0.55f, x);
        exercise_floorBatch.Vertex3f(20.f, -0.55f, x);
    }
    exercise_floorBatch.End();
    // 设置大球模型
    gltMakeSphere(exercise_torusBatch, 0.4f, 40, 80);
    // 设置小球球模型
    gltMakeSphere(exercise_sphereBatch, 0.1f, 26, 13);
    // 随机位置放置小球球
    for (int i = 0; i < NUM_SPHERES; i++) {
        GLfloat x = ((GLfloat)((rand() % 400) - 200) * 0.1f);
        GLfloat z = ((GLfloat)((rand() % 400) - 200 ) * 0.1f);
        
        spheres[i].SetOrigin(x, 0.0f, z);
    }
}

void exercise_renderScene()
{
    //1.颜色值(地板,大球,小球颜色)
    static GLfloat vFloorColor[] = { 0.0f, 1.0f, 0.0f, 1.0f };
    static GLfloat vTorusColor[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    static GLfloat vSphereColor[] = { 0.0f, 0.0f, 1.0f, 1.0f };
    
    //2.基于时间动画
    static CStopWatch rotTimer;
    float yRot = rotTimer.GetElapsedSeconds() * 60.0f;
    
    //3.清除颜色缓存区和深度缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //4.加入观察者 平移10步(地板,大球,小球,小小球)
    M3DMatrix44f mCamera;
    exercise_cameraFrame.GetCameraMatrix(mCamera);
    exercise_modelViewMatrix.PushMatrix(mCamera);
    
    //5.获取光源位置
    M3DVector4f vLightPos = { 0.0f, 10.0f, 5.0f, 1.0f };
    
    //6.绘制地面
    exercise_shaderManager.UseStockShader(GLT_SHADER_FLAT, exercise_transformPipeline.GetModelViewProjectionMatrix(), vFloorColor);
    exercise_floorBatch.Draw();
    
    //大球默认位置(0,0,0)Z深度(3.0) 正负指的是方向, 数字指的移动距离
    //7.使得大球位置平移(3.0)向屏幕里面  只移动1次
    exercise_modelViewMatrix.Translate(0.0f, 0.0f, -3.0f);
    
    //8.压栈(复制栈顶)
    exercise_modelViewMatrix.PushMatrix();
    
    //9.大球自转
    exercise_modelViewMatrix.Rotate(yRot, 0.0f, 1.0f, 0.0f);
    
    //10.指定合适的着色器(点光源着色器)
    exercise_shaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF, exercise_transformPipeline.GetModelViewMatrix(), exercise_transformPipeline.GetProjectionMatrix(), vLightPos, vTorusColor);
    exercise_torusBatch.Draw();
    
    //11.绘制完毕则Pop
    exercise_modelViewMatrix.PopMatrix();
    
    //12.画小球
    for (int i = 0; i < NUM_SPHERES; i++) {
        exercise_modelViewMatrix.PushMatrix();
        exercise_modelViewMatrix.MultMatrix(spheres[i]);
        exercise_shaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF, exercise_transformPipeline.GetModelViewMatrix(), exercise_transformPipeline.GetProjectionMatrix(), vLightPos, vSphereColor);
        exercise_sphereBatch.Draw();
        exercise_modelViewMatrix.PopMatrix();
    }
    
    //13. 让一个小篮球围绕大球公众自转
    exercise_modelViewMatrix.Rotate(yRot * -2.0f, 0.0f, 1.0f, 0.0f);
    exercise_modelViewMatrix.Translate(0.8f, 0.0f, 0.0f);
    exercise_shaderManager.UseStockShader(GLT_SHADER_FLAT, exercise_transformPipeline.GetModelViewProjectionMatrix(), vSphereColor);
    exercise_sphereBatch.Draw();
    
    exercise_modelViewMatrix.PopMatrix();
    
    glutSwapBuffers();
    
    glutPostRedisplay();
}

void exercise_changeSize(int w, int h)
{
    glViewport(0, 0, w, h);
    
    // 创建投影矩阵
    exercise_viewFrustum.SetPerspective(35.f, float(w)/float(h), 0.1f, 100.f);
    
    exercise_projectionMatrix.LoadMatrix(exercise_viewFrustum.GetProjectionMatrix());
    
    exercise_transformPipeline.SetMatrixStacks(exercise_modelViewMatrix, exercise_projectionMatrix);
}

void exercise_specialKeys(int key, int x, int y)
{
    float linear = 0.1f;
    
    float angular = float(m3dDegToRad(5.0f));
    
    if (key == GLUT_KEY_UP) {
        exercise_cameraFrame.MoveForward(linear);
    }
    if (key == GLUT_KEY_DOWN) {
        exercise_cameraFrame.MoveForward(-linear);
    }
    if (key == GLUT_KEY_LEFT) {
        exercise_cameraFrame.RotateWorld(angular, 0.0f, 1.0f, 0.0f);
    }
    if (key == GLUT_KEY_RIGHT) {
        exercise_cameraFrame.RotateWorld(-angular, 0.0f, 1.0f, 0.0f);
    }
}

void excrcise(int argc,char *argv[])
{
    gltSetWorkingDirectory(argv[0]);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Sphere world");
    glutReshapeFunc(exercise_changeSize);
    glutDisplayFunc(exercise_renderScene);
    glutSpecialFunc(exercise_specialKeys);
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return;
    }
    exercise_setupRC();
    glutMainLoop();
}

#endif /* exercise_h */
