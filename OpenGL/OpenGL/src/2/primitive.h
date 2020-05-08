//
//  primitive.h
//  OpenGL
//
//  Created by apple on 2020/4/28.
//  Copyright © 2020 apple. All rights reserved.
//

#ifndef primitive_h
#define primitive_h

#include "GLTools.h"
#include "GLMatrixStack.h"
#include "GLFrame.h"
#include "GLFrustum.h"
#include "GLBatch.h"
#include "GLGeometryTransform.h"
#include <math.h>

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

/*
GLMatrixStack 变化管线使用矩阵堆栈
GLMatrixStack 构造函数允许指定堆栈的最大深度、默认的堆栈深度为64.这个矩阵堆在初始化时已经在堆栈中包含了单位矩阵。
GLMatrixStack::GLMatrixStack(int iStackDepth = 64);
//通过调用顶部载入这个单位矩阵
void GLMatrixStack::LoadIndentiy(void);
//在堆栈顶部载入任何矩阵
void GLMatrixStack::LoadMatrix(const M3DMatrix44f m);
*/

GLShaderManager     pri_shaderManager;
GLMatrixStack       pri_modelViewMatrix;
GLMatrixStack       pri_projectionMatrix;
GLFrame             pri_cameraFrame;
GLFrame             pri_objectFrame;
GLFrustum           pri_viewFrustum;

GLBatch             pri_pointBatch;
GLBatch             pri_lineBatch;
GLBatch             pri_lineStripBatch;
GLBatch             pri_lineLoopBatch;
GLBatch             pri_triangleBatch;
GLBatch             pri_triangleStripBatch;
GLBatch             pri_triangleFanBatch;

GLGeometryTransform pri_transformPipeline;

GLfloat vGreen[] = { 0.0f, 1.0f, 0.0f, 1.0f };
GLfloat vBlack[] = { 0.0f, 0.0f, 0.0f, 1.0f };

int nStep = 0;

void setupRC()
{
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    pri_shaderManager.InitializeStockShaders();
    glEnable(GL_DEPTH_TEST);
    //设置变换管线以使用两个矩阵堆栈
    pri_transformPipeline.SetMatrixStacks(pri_modelViewMatrix, pri_projectionMatrix);
    //下面2种方式的效果一样
//    pri_cameraFrame.MoveForward(-15.0f);
    pri_objectFrame.MoveForward(15.0f);
    
    GLfloat vCoast[9] = {
        3, 3, 0, 0, 3, 0, 3, 0, 0
    };
    
    //----------- 点的形式 --------------
    pri_pointBatch.Begin(GL_POINTS, 3);
    pri_pointBatch.CopyVertexData3f(vCoast);
    pri_pointBatch.End();
    
    //----------- 线的形式 ---------------
    pri_lineBatch.Begin(GL_LINES, 3);
    pri_lineBatch.CopyVertexData3f(vCoast);
    pri_lineBatch.End();
    
    //----------- 线条的形式 ----------------
    pri_lineStripBatch.Begin(GL_LINE_STRIP, 3);
    pri_lineStripBatch.CopyVertexData3f(vCoast);
    pri_lineStripBatch.End();
    
    //----------- 线环的方式 -----------------
    pri_lineLoopBatch.Begin(GL_LINE_LOOP, 3);
    pri_lineLoopBatch.CopyVertexData3f(vCoast);
    pri_lineLoopBatch.End();
    
    //----------- 通过三角形画金字塔 --------------
    GLfloat vPyramid[12][3] = {
        -2.0f, 0.0f, -2.0f,
        2.0f, 0.0f, -2.0f,
        0.0f, 4.0f, 0.0f,
        
        2.0f, 0.0f, -2.0f,
        2.0f, 0.0f, 2.0f,
        0.0f, 4.0f, 0.0f,
        
        2.0f, 0.0f, 2.0f,
        -2.0f, 0.0f, 2.0f,
        0.0f, 4.0f, 0.0f,
        
        -2.0f, 0.0f, 2.0f,
        -2.0f, 0.0f, -2.0f,
        0.0f, 4.0f, 0.0f
    };
    pri_triangleBatch.Begin(GL_TRIANGLES, 12);
    pri_triangleBatch.CopyVertexData3f(vPyramid);
    pri_triangleBatch.End();
    
    //----------- 三角形扇形 ----------------
    GLfloat vPoints[100][3];
    int nVerts = 0;
    GLfloat r = 5.f;
    // 原点(x,y,z) = (0,0,0)
    vPoints[nVerts][0] = 0.0f;
    vPoints[nVerts][1] = 0.0f;
    vPoints[nVerts][2] = 0.0f;
    
    for (GLfloat angle = 0; angle < M3D_2PI; angle += M3D_2PI / 6.0f) {
        //数组下标自增（每自增1次就表示一个顶点）
        nVerts++;
        //x点坐标 cos(angle) * 半径
        vPoints[nVerts][0] = float(cos(angle)) * r;
        //y点坐标 sin(angle) * 半径
        vPoints[nVerts][1] = float(sin(angle)) * r;
        //z点的坐标
        vPoints[nVerts][2] = -0.5f;
    }
    // 结束扇形 前面一共绘制7个顶点（包括圆心）
    //添加闭合的终点
    nVerts++;
    vPoints[nVerts][0] = r;
    vPoints[nVerts][1] = 0.0f;
    vPoints[nVerts][2] = 0.0f;
    // 加载
    pri_triangleFanBatch.Begin(GL_TRIANGLE_FAN, 8);
    pri_triangleFanBatch.CopyVertexData3f(vPoints);
    pri_triangleFanBatch.End();
    
    //----------- 三角形条带 ------------
    // 顶点下表
    int iCounter = 0;
    GLfloat radius = 5.0f;
    for (GLfloat angle = 0.0f; angle <= (2.0f * M3D_PI); angle += 0.3f) {
        //获取圆形的顶点的X,Y
        GLfloat x = radius * sin(angle);
        GLfloat y = radius * cos(angle);
        //绘制2个三角形（他们的x,y顶点一样，只是z点不一样）
        vPoints[iCounter][0] = x;
        vPoints[iCounter][1] = y;
        vPoints[iCounter][2] = -0.5f;
        iCounter++;
        
        vPoints[iCounter][0] = x;
        vPoints[iCounter][1] = y;
        vPoints[iCounter][2] = 0.5f;
        iCounter++;
    }
    
    // 关闭循环
    printf("三角形带的顶点数：%d\n",iCounter);
    //结束循环，在循环位置生成2个三角形
    vPoints[iCounter][0] = vPoints[0][0];
    vPoints[iCounter][1] = vPoints[0][1];
    vPoints[iCounter][2] = -0.5;
    iCounter++;
    
    vPoints[iCounter][0] = vPoints[1][0];
    vPoints[iCounter][1] = vPoints[1][1];
    vPoints[iCounter][2] = 0.5;
    iCounter++;
    
    // 加载
    pri_triangleStripBatch.Begin(GL_TRIANGLE_STRIP, iCounter);
    pri_triangleStripBatch.CopyVertexData3f(vPoints);
    pri_triangleStripBatch.End();
}

void drawWireFrameBatch(GLBatch *pBatch)
{
    pri_shaderManager.UseStockShader(GLT_SHADER_FLAT, pri_transformPipeline.GetModelViewProjectionMatrix(), vGreen);
    pBatch->Draw();
    
    /*-----------边框部分-------------------*/
    glPolygonOffset(-1.0f, -1.0f);// 偏移深度，在同一位置要绘制填充和边线，会产生z冲突，所以要偏移
    glEnable(GL_POLYGON_OFFSET_LINE);
    
    // 画反锯齿，让黑边好看些
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //绘制线框几何黑色版 三种模式，实心，边框，点，可以作用在正面，背面，或者两面
    //通过调用glPolygonMode将多边形正面或者背面设为线框模式，实现线框渲染
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(2.0f);
    
    pri_shaderManager.UseStockShader(GLT_SHADER_FLAT, pri_transformPipeline.GetModelViewProjectionMatrix(), vBlack);
    pBatch->Draw();
    
    //复原原本的设置
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_POLYGON_OFFSET_LINE);
    glLineWidth(1.0f);
    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);
}

void renderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
//    // 压栈
//    pri_modelViewMatrix.PushMatrix();
//    M3DMatrix44f mCamera;
//    pri_cameraFrame.GetCameraMatrix(mCamera);
//
//    //矩阵乘以矩阵堆栈的顶部矩阵，相乘的结果随后简存储在堆栈的顶部
//    pri_modelViewMatrix.MultMatrix(mCamera);
//
//    M3DMatrix44f mObjectFrame;
//    //只要使用 GetMatrix 函数就可以获取矩阵堆栈顶部的值，这个函数可以进行2次重载。用来使用GLShaderManager 的使用。或者是获取顶部矩阵的顶点副本数据
//    pri_objectFrame.GetMatrix(mObjectFrame);
//
//    //矩阵乘以矩阵堆栈的顶部矩阵，相乘的结果随后简存储在堆栈的顶部
//    pri_modelViewMatrix.MultMatrix(mObjectFrame);
    
    //跟上面注释代码效果一样
    pri_modelViewMatrix.PushMatrix(pri_objectFrame);
    
    pri_shaderManager.UseStockShader(GLT_SHADER_FLAT, pri_transformPipeline.GetModelViewProjectionMatrix(), vBlack);
    
    switch (nStep) {
        case 0: {
            // 设置点的大小
            glPointSize(4.0f);
            pri_pointBatch.Draw();
            glPointSize(1.0f);
            break;
        }
        case 1: {
            glLineWidth(2.0f);
            pri_lineBatch.Draw();
            glLineWidth(1.0f);
            break;
        }
        case 2: {
            glLineWidth(2.0f);
            pri_lineStripBatch.Draw();
            glLineWidth(1.0f);
            break;
        }
        case 3: {
            glLineWidth(2.0f);
            pri_lineLoopBatch.Draw();
            glLineWidth(1.0f);
            break;
        }
        case 4: {
            drawWireFrameBatch(&pri_triangleBatch);
            break;
        }
        case 5: {
            drawWireFrameBatch(&pri_triangleStripBatch);
            break;
        }
        case 6: {
            drawWireFrameBatch(&pri_triangleFanBatch);
            break;
        }
            
        default:
            break;
    }
    
    // 还原到以前的模型视图矩阵（单位矩阵）
    pri_modelViewMatrix.PopMatrix();
    
    glutSwapBuffers();
}

void specialKeys(int key, int x, int y)
{
    if (key == GLUT_KEY_UP) {
        pri_objectFrame.RotateWorld(m3dDegToRad(-5.0f), 1.0f, 0.0f, 0.0f);
    }
    if (key == GLUT_KEY_DOWN) {
        pri_objectFrame.RotateWorld(m3dDegToRad(5.0f), 1.0f, 0.0f, 0.0f);
    }
    if (key == GLUT_KEY_LEFT) {
        pri_objectFrame.RotateWorld(m3dDegToRad(-5.0f), 0.0f, 1.0f, 0.0f);
    }
    if (key == GLUT_KEY_RIGHT) {
        pri_objectFrame.RotateWorld(m3dDegToRad(5.0f), 0.0f, 1.0f, 0.0f);
    }
    
    glutPostRedisplay();
}

//根据空格次数。切换不同的“窗口名称”
void keyPressFunc(unsigned char key, int x, int y)
{
    if(key == 32) {
        nStep++;
        if(nStep > 6) {
            nStep = 0;
        }
    }
    
    switch (nStep) {
        case 0:
            glutSetWindowTitle("GL_POINTS");
            break;
        case 1:
            glutSetWindowTitle("GL_LINES");
            break;
        case 2:
            glutSetWindowTitle("GL_LINE_STRIP");
            break;
        case 3:
            glutSetWindowTitle("GL_LINE_LOOP");
            break;
        case 4:
            glutSetWindowTitle("GL_TRIANGLES");
            break;
        case 5:
            glutSetWindowTitle("GL_TRIANGLE_STRIP");
            break;
        case 6:
            glutSetWindowTitle("GL_TRIANGLE_FAN");
            break;
    }
    
    glutPostRedisplay();
}

void changeSize(int w, int h)
{
    glViewport(0, 0, w, h);
    
    //创建投影矩阵，并将它载入投影矩阵堆栈中
    pri_viewFrustum.SetPerspective(45.f, float(w)/float(h), 1.0f, 100.0f);
    pri_projectionMatrix.LoadMatrix(pri_viewFrustum.GetProjectionMatrix());
    
    //调用顶部载入单元矩阵
    pri_modelViewMatrix.LoadIdentity();
}

void primitive(int argc, char *argv[])
{
    gltSetWorkingDirectory(argv[0]);
    glutInit(&argc, argv);
    //申请一个颜色缓存区、深度缓存区、双缓存区、模板缓存区
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    //设置window 的尺寸
    glutInitWindowSize(800, 600);
    //创建window的名称
    glutCreateWindow("GL_POINTS");
    //注册回调函数（改变尺寸）
    glutReshapeFunc(changeSize);
    //点击空格时，调用的函数
    glutKeyboardFunc(keyPressFunc);
    //特殊键位函数（上下左右）
    glutSpecialFunc(specialKeys);
    //显示函数
    glutDisplayFunc(renderScene);
    
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return;
    }
    //绘制
    setupRC();
    //runloop运行循环
    glutMainLoop();
}

#endif /* primitive_h */
