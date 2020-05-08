//
//  transform.h
//  OpenGL
//
//  Created by apple on 2020/5/8.
//  Copyright © 2020 apple. All rights reserved.
//

#ifndef transform_h
#define transform_h

#include "GLTools.h"    // OpenGL toolkit
#include "GLMatrixStack.h"
#include "GLFrame.h"
#include "GLFrustum.h"
#include "GLBatch.h"
#include "GLGeometryTransform.h"
#include "StopWatch.h"

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

GLShaderManager     transform_shaderManager;
GLMatrixStack       transform_modelViewMatrix;
GLMatrixStack       transform_projectionMatrix;
//观察者位置
GLFrame             transform_cameraFrame;
//世界坐标位置
GLFrame             transform_objectFrame;
//视景体，用来构造投影矩阵
GLFrustum           transform_viewFrustum;
//球
GLTriangleBatch     transform_sphereBatch;
//环
GLTriangleBatch     transform_torusBatch;
//圆柱
GLTriangleBatch     transform_cylinderBatch;
//锥
GLTriangleBatch     transform_coneBatch;
//磁盘
GLTriangleBatch     transform_diskBatch;

GLGeometryTransform transform_pipeline;
M3DMatrix44f        transform_shadowMatrix;

extern GLfloat vGreen[];
extern GLfloat vBlack[];
extern int nStep;

void transform_setupRC()
{
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    transform_shaderManager.InitializeStockShaders();
    
    glEnable(GL_DEPTH_TEST);
    
    // 将物体向屏幕外移动15.0
    transform_objectFrame.MoveForward(15.f);
    
    //--- 利用三角形批次类构造图形对象 ---
    // 球
    /*
     gltMakeSphere(GLTriangleBatch& sphereBatch, GLfloat fRadius, GLint iSlices, GLint iStacks);
    参数1：sphereBatch，三角形批次类对象
    参数2：fRadius，球体半径
    参数3：iSlices，从球体底部堆叠到顶部的三角形带的数量；其实球体是一圈一圈三角形带组成
    参数4：iStacks，围绕球体一圈排列的三角形对数
    
    建议：一个对称性较好的球体的片段数量是堆叠数量的2倍，就是iStacks = 2 * iSlices;
    绘制球体都是围绕Z轴，这样+z就是球体的顶点，-z就是球体的底部。
    */
    gltMakeSphere(transform_sphereBatch, 3.0f, 15, 30);
    
    // 环
    /*
    gltMakeTorus(GLTriangleBatch& torusBatch, GLfloat majorRadius, GLfloat minorRadius, GLint numMajor, GLint numMinor);
    参数1：torusBatch，三角形批次类对象
    参数2：majorRadius,甜甜圈中心到外边缘的半径
    参数3：minorRadius,甜甜圈中心到内边缘的半径
    参数4：numMajor,沿着主半径的三角形数量
    参数5：numMinor,沿着内部较小半径的三角形数量
    */
    gltMakeTorus(transform_torusBatch, 3.0f, 0.75f, 15, 15);
    
    // 圆柱
    /*
    void gltMakeCylinder(GLTriangleBatch& cylinderBatch, GLfloat baseRadius, GLfloat topRadius, GLfloat fLength, GLint numSlices, GLint numStacks);
    参数1：cylinderBatch，三角形批次类对象
    参数2：baseRadius,底部半径
    参数3：topRadius,头部半径
    参数4：fLength,圆形长度
    参数5：numSlices,围绕Z轴的三角形对的数量
    参数6：numStacks,圆柱底部堆叠到顶部圆环的三角形数量
    */
    gltMakeCylinder(transform_cylinderBatch, 2.0f, 2.0f, 3.0f, 15, 2);
    
    // 圆锥
    //圆锥体，是一端的半径为0，另一端半径可指定。
    gltMakeCylinder(transform_coneBatch, 2.0f, 0.0f, 3.0f, 15, 3);
    
    // 磁盘
    /*
    void gltMakeDisk(GLTriangleBatch& diskBatch, GLfloat innerRadius, GLfloat outerRadius, GLint nSlices, GLint nStacks);
     参数1:diskBatch，三角形批次类对象
     参数2:innerRadius,内圆半径
     参数3:outerRadius,外圆半径
     参数4:nSlices,圆盘围绕Z轴的三角形对的数量
     参数5:nStacks,圆盘外网到内围的三角形数量
     */
    gltMakeDisk(transform_diskBatch, 1.5f, 3.0f, 13, 3);
}

void transform_drawWireFramedBatch(GLTriangleBatch *pBatch)
{
    transform_shaderManager.UseStockShader(GLT_SHADER_FLAT, transform_pipeline.GetModelViewProjectionMatrix(), vGreen);
    pBatch->Draw();
    
    //---画出黑色轮廓---
    // 开启多边形偏移
    glEnable(GL_POLYGON_OFFSET_LINE);
    // 多边形模型(背面、线) 将多边形背面设为线框模式
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // 开启多边形偏移(设置偏移数量)
    glPolygonOffset(-1.0f, -1.0f);
    // 线条宽度
    glLineWidth(2.5f);
    
    // 开启混合功能（颜色混合&抗锯齿功能）
    glEnable(GL_BLEND);
    // 开启处理线段抗锯齿功能
    glEnable(GL_LINE_SMOOTH);
    // 设置颜色混合因子
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // 平面着色器绘制线条
    transform_shaderManager.UseStockShader(GLT_SHADER_FLAT, transform_pipeline.GetModelViewProjectionMatrix(), vBlack);
    pBatch->Draw();
    
    // 恢复多边形模式和深度测试
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_POLYGON_OFFSET_LINE);
    glLineWidth(1.0f);
    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);
}

void transform_renderScene()
{
    // 用当前清除颜色清除窗口背景
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    // 模型视图矩阵栈堆，压栈
    transform_modelViewMatrix.PushMatrix(transform_objectFrame);
    
    switch (nStep) {
        case 0:
            transform_drawWireFramedBatch(&transform_sphereBatch);
            break;
        case 1:
            transform_drawWireFramedBatch(&transform_torusBatch);
            break;
        case 2:
            transform_drawWireFramedBatch(&transform_cylinderBatch);
            break;
        case 3:
            transform_drawWireFramedBatch(&transform_coneBatch);
            break;
        case 4:
            transform_drawWireFramedBatch(&transform_diskBatch);
            break;
        default:
            break;
    }
    
    transform_modelViewMatrix.PopMatrix();
    
    glutSwapBuffers();
}

// 上下左右，移动图形
void transform_specialKeys(int key, int x, int y)
{
    if (key == GLUT_KEY_UP) {
        //移动世界坐标系，而不是去移动物体。
        //将世界坐标系在X方向移动-5.0
        transform_objectFrame.RotateWorld(m3dDegToRad(-5.0f), 1.0f, 0.0f, 0.0f);
    }
    if (key == GLUT_KEY_DOWN) {
        transform_objectFrame.RotateWorld(m3dDegToRad(5.0f), 1.0f, 0.0f, 0.0f);
    }
    if (key == GLUT_KEY_LEFT) {
        transform_objectFrame.RotateWorld(m3dDegToRad(-5.f), 0.0f, 1.0f, 0.0f);
    }
    if (key == GLUT_KEY_RIGHT) {
        transform_objectFrame.RotateWorld(m3dDegToRad(5.0f), 0.0f, 1.0f, 0.0f);
    }
    
    glutPostRedisplay();
}

// 点击空格，切换渲染图形
void transform_keyPressFunc(unsigned char key, int x, int y)
{
    if (key == 32) {
        nStep++;
        if (nStep > 4) {
            nStep = 0;
        }
    }
    
    switch (nStep) {
        case 0:
            glutSetWindowTitle("Sphere");
            break;
        case 1:
            glutSetWindowTitle("Torus");
            break;
        case 2:
            glutSetWindowTitle("Cylinder");
            break;
        case 3:
            glutSetWindowTitle("Cone");
            break;
        case 4:
            glutSetWindowTitle("Disk");
            break;
            
        default:
            break;
    }
    
    glutPostRedisplay();
}

void tranform_changeSize(int w, int h)
{
    glViewport(0, 0, w, h);
    
    //透视投影
    transform_viewFrustum.SetPerspective(45.f, float(w)/float(h), 0.1f, 100.f);
    //projectionMatrix 矩阵堆栈 加载透视投影矩阵
    transform_projectionMatrix.LoadMatrix(transform_viewFrustum.GetProjectionMatrix());
    
    // modelViewMatrix 矩阵堆栈 加载单元矩阵
    transform_modelViewMatrix.LoadIdentity();
    
    //通过GLGeometryTransform管理矩阵堆栈
    //使用transformPipeline 管道管理模型视图矩阵堆栈 和 投影矩阵堆栈
    transform_pipeline.SetMatrixStacks(transform_modelViewMatrix, transform_projectionMatrix);
}

void transform(int argc,char *argv[])
{
    gltSetWorkingDirectory(argv[0]);
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Sphere");
    glutReshapeFunc(tranform_changeSize);
    glutKeyboardFunc(transform_keyPressFunc);
    glutSpecialFunc(transform_specialKeys);
    glutDisplayFunc(transform_renderScene);
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return;
    }
    transform_setupRC();
    glutMainLoop();
}


#endif /* transform_h */
