//
//  skill.h
//  OpenGL
//
//  Created by apple on 2020/4/30.
//  Copyright © 2020 apple. All rights reserved.
//

#ifndef skill_h
#define skill_h

//演示了OpenGL背面剔除，深度测试，和多边形模式
#include "GLTools.h"
#include "GLMatrixStack.h"
#include "GLFrame.h"
#include "GLFrustum.h"
#include "GLGeometryTransform.h"

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

GLFrame             sk_viewFrame;
GLFrustum           sk_viewFrustum;
GLTriangleBatch     sk_torusBatch;
GLMatrixStack       sk_modelViewMatrix;
GLMatrixStack       sk_projectionMatrix;
GLGeometryTransform sk_transformPipeline;
GLShaderManager     sk_shaderManager;

//标记：背面剔除、深度测试
int iCull = 0;
int iDepth = 0;

void sk_renderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    if (iCull) {
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
    } else {
        glDisable(GL_CULL_FACE);
    }
    
    if (iDepth) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
    
    //把摄像机矩阵压入模型矩阵中
    sk_modelViewMatrix.PushMatrix(sk_viewFrame);
    
    GLfloat vred[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    
    //使用默认光源着色器
    //通过光源、阴影效果跟提现立体效果
    //参数1：GLT_SHADER_DEFAULT_LIGHT 默认光源着色器
    //参数2：模型视图矩阵
    //参数3：投影矩阵
    //参数4：基本颜色值
    sk_shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, sk_transformPipeline.GetModelViewMatrix(), sk_transformPipeline.GetProjectionMatrix(), vred);
    
    sk_torusBatch.Draw();
    
    sk_modelViewMatrix.PopMatrix();
    
    glutSwapBuffers();
}

void sk_setupRC()
{
    glClearColor(0.3f, 0.4f, 0.4f, 1.0f);
    
    sk_shaderManager.InitializeStockShaders();
    
    sk_viewFrame.MoveForward(7.0f);
    
    //创建一个甜甜圈
    //void gltMakeTorus(GLTriangleBatch& torusBatch, GLfloat majorRadius, GLfloat minorRadius, GLint numMajor, GLint numMinor);
    //参数1：GLTriangleBatch 容器帮助类
    //参数2：外边缘半径
    //参数3：内边缘半径
    //参数4、5：主半径和从半径的细分单元数量
    gltMakeTorus(sk_torusBatch, 1.0f, 0.3f, 52, 26);
    
    glPointSize(4.0f);
}

void sk_specialKeys(int key, int x, int y)
{
    if (key == GLUT_KEY_UP) {
        sk_viewFrame.RotateWorld(m3dDegToRad(-5.0f), 1.0f, 0.0f, 0.0f);
    }
    if (key == GLUT_KEY_DOWN) {
        sk_viewFrame.RotateWorld(m3dDegToRad(5.0f), 1.0f, 0.0f, 0.0f);
    }
    if (key == GLUT_KEY_LEFT) {
        sk_viewFrame.RotateWorld(m3dDegToRad(-5.0f), 0.0f, 1.0f, 0.0f);
    }
    if (key == GLUT_KEY_RIGHT) {
        sk_viewFrame.RotateWorld(m3dDegToRad(5.0f), 0.0f, 1.0f, 0.0f);
    }
    
    glutPostRedisplay();
}

void sk_processMenu(int value)
{
    switch (value) {
        case 1:
        {
            iDepth = !iDepth;
            break;
        }
        case 2:
        {
            iCull = !iCull;
            break;
        }
        case 3:
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
        }
        case 4:
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
        }
        case 5:
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            break;
        }
            
        default:
            break;
    }
    
    glutPostRedisplay();
}

void sk_changeSize(int w, int h)
{
    if (h == 0) {
        h = 1;
    }
    
    glViewport(0, 0, w, h);
    // 设置透视模式，初始化其透视矩阵
    sk_viewFrustum.SetPerspective(45.f, float(w)/float(h), 1.0f, 100.0f);
    // 把透视矩阵加载到透视矩阵对阵中
    sk_projectionMatrix.LoadMatrix(sk_viewFrustum.GetProjectionMatrix());
    // 初始化渲染管线
    sk_transformPipeline.SetMatrixStacks(sk_modelViewMatrix, sk_projectionMatrix);
}

void skill(int argc,char *argv[])
{
    gltSetWorkingDirectory(argv[0]);
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Geometry Test Program");
    glutReshapeFunc(sk_changeSize);
    glutSpecialFunc(sk_specialKeys);
    glutDisplayFunc(sk_renderScene);
    
    glutCreateMenu(sk_processMenu);
    glutAddMenuEntry("Toggle depth test",1);
    glutAddMenuEntry("Toggle cull backface",2);
    glutAddMenuEntry("Set Fill Mode", 3);
    glutAddMenuEntry("Set Line Mode", 4);
    glutAddMenuEntry("Set Point Mode", 5);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return;
    }
    
    sk_setupRC();
    
    glutMainLoop();
}

#endif /* skill_h */
