//
//  sphere.h
//  OpenGL
//
//  Created by apple on 2020/5/27.
//  Copyright © 2020 apple. All rights reserved.
//

#ifndef sphere_h
#define sphere_h


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

//**4、添加附加随机球
#define NUM_SPHERES 50
GLFrame sphereArr[NUM_SPHERES];

GLShaderManager     sphere_shaderManager;
GLMatrixStack       sphere_modelViewMatrix;
GLMatrixStack       sphere_projectionMatrix;
GLFrustum           sphere_viewFrustum;
GLGeometryTransform sphere_transformPipeline;

GLTriangleBatch     sphere_torusBatch;
GLBatch             sphere_floorBatch;

GLTriangleBatch     sphereBatch;

GLFrame             sphere_cameraFrame;

GLuint              uiTextures[3];


bool sphere_loadTGATexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode)
{
    GLbyte *pBits;
    int nWidth, nHeight, nComponents;
    GLenum eFormat;
    
    pBits = gltReadTGABits(szFileName, &nWidth, &nHeight, &nComponents, &eFormat);
    if (pBits == NULL) {
        return false;
    }
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    
    //载入纹理
    //参数1：纹理维度
    //参数2：mip贴图层次
    //参数3：纹理单元存储的颜色成分（从读取像素图是获得）-将内部参数nComponents改为了通用压缩纹理格式GL_COMPRESSED_RGB
    //参数4：加载纹理宽
    //参数5：加载纹理高
    //参数6：加载纹理的深度
    //参数7：像素数据的数据类型（GL_UNSIGNED_BYTE，每个颜色分量都是一个8位无符号整数）
    //参数8：指向纹理图像数据的指针
    glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB, nWidth, nHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBits);
    
    free(pBits);
    
    //只有minFilter 等于以下四种模式，才可以生成Mip贴图
    //GL_NEAREST_MIPMAP_NEAREST具有非常好的性能，并且闪烁现象非常弱
    //GL_LINEAR_MIPMAP_NEAREST常常用于对游戏进行加速，它使用了高质量的线性过滤器
    //GL_LINEAR_MIPMAP_LINEAR 和GL_NEAREST_MIPMAP_LINEAR 过滤器在Mip层之间执行了一些额外的插值，以消除他们之间的过滤痕迹。
    //GL_LINEAR_MIPMAP_LINEAR 三线性Mip贴图。纹理过滤的黄金准则，具有最高的精度。
    if (minFilter == GL_LINEAR_MIPMAP_LINEAR ||
        minFilter == GL_LINEAR_MIPMAP_NEAREST ||
        minFilter == GL_NEAREST_MIPMAP_LINEAR ||
        minFilter == GL_NEAREST_MIPMAP_NEAREST)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    
    return true;
}

void sphere_setupRC()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    sphere_shaderManager.InitializeStockShaders();
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    gltMakeSphere(sphere_torusBatch, 0.4f, 40, 80);
    
    gltMakeSphere(sphereBatch, 0.1f, 26, 13);
    
    GLfloat texSize = 10.0f;
    sphere_floorBatch.Begin(GL_TRIANGLE_FAN, 4, 1);
    sphere_floorBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    sphere_floorBatch.Vertex3f(-20.0f, -0.4f, 20.0f);
    
    sphere_floorBatch.MultiTexCoord2f(0, texSize, 0.0f);
    sphere_floorBatch.Vertex3f(20.0f, -0.4f, 20.0f);
    
    sphere_floorBatch.MultiTexCoord2f(0, texSize, texSize);
    sphere_floorBatch.Vertex3f(20.0f, -0.4f, -20.0f);
    
    sphere_floorBatch.MultiTexCoord2f(0, 0.0f, texSize);
    sphere_floorBatch.Vertex3f(-20.0f, -0.4f, -20.0f);
    sphere_floorBatch.End();
    
    //随机小球球顶点坐标数据
    for (int i = 0; i < NUM_SPHERES; i++) {
        
        //y轴不变，X,Z产生随机值
        GLfloat x = ((GLfloat)((rand() % 400) - 200 ) * 0.1f);
        GLfloat z = ((GLfloat)((rand() % 400) - 200 ) * 0.1f);
        
        //在y方向，将球体设置为0.0的位置，这使得它们看起来是飘浮在眼睛的高度
        //对spheres数组中的每一个顶点，设置顶点数据
        sphereArr[i].SetOrigin(x, 0.0f, z);
    }
    
    glGenTextures(3, uiTextures);
    
    glBindTexture(GL_TEXTURE_2D, uiTextures[0]);
    sphere_loadTGATexture("marble.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);
    
    
    glBindTexture(GL_TEXTURE_2D, uiTextures[1]);
    sphere_loadTGATexture("marslike.tga", GL_LINEAR_MIPMAP_LINEAR,
                   GL_LINEAR, GL_CLAMP_TO_EDGE);
    
    
    glBindTexture(GL_TEXTURE_2D, uiTextures[2]);
    sphere_loadTGATexture("moonlike.tga", GL_LINEAR_MIPMAP_LINEAR,
                   GL_LINEAR, GL_CLAMP_TO_EDGE);
    
}

//删除纹理
void sphere_shutdownRC(void)
{
    glDeleteTextures(3, uiTextures);
}

// 屏幕更改大小或已初始化
void sphere_changeSize(int nWidth, int nHeight)
{
    //1.设置视口
    glViewport(0, 0, nWidth, nHeight);
    
    //2.设置投影方式
    sphere_viewFrustum.SetPerspective(35.0f, float(nWidth)/float(nHeight), 1.0f, 100.0f);
    
    //3.将投影矩阵加载到投影矩阵堆栈,
    sphere_projectionMatrix.LoadMatrix(sphere_viewFrustum.GetProjectionMatrix());
    sphere_modelViewMatrix.LoadIdentity();
    
    //4.将投影矩阵堆栈和模型视图矩阵对象设置到管道中
    sphere_transformPipeline.SetMatrixStacks(sphere_modelViewMatrix, sphere_projectionMatrix);
   
}

void sphere_drawSomething(GLfloat yRot)
{
    //定义光源位置&漫反射颜色
    static GLfloat vWhite[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    static GLfloat vLightPos[] = { 0.0f, 3.0f, 0.0f, 1.0f };
    
    //绘制悬浮小球
    glBindTexture(GL_TEXTURE_2D, uiTextures[2]);
    for (int i = 0; i < NUM_SPHERES; i++) {
        sphere_modelViewMatrix.PushMatrix();
        sphere_modelViewMatrix.MultMatrix(sphereArr[i]);
        sphere_shaderManager.UseStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF,
                                            sphere_modelViewMatrix.GetMatrix(),
                                            sphere_transformPipeline.GetProjectionMatrix(),
                                            vLightPos,
                                            vWhite,
                                            0);
        sphereBatch.Draw();
        sphere_modelViewMatrix.PopMatrix();
    }
    
    //绘制大球
    sphere_modelViewMatrix.Translate(0.0f, 0.2f, -2.5f);
    sphere_modelViewMatrix.PushMatrix();
    sphere_modelViewMatrix.Rotate(yRot, 0.0f, 1.0f, 0.0f);
    glBindTexture(GL_TEXTURE_2D, uiTextures[1]);
    sphere_shaderManager.UseStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF,
                                        sphere_modelViewMatrix.GetMatrix(),
                                        sphere_transformPipeline.GetProjectionMatrix(),
                                        vLightPos,
                                        vWhite,
                                        0);
    sphere_torusBatch.Draw();
    sphere_modelViewMatrix.PopMatrix();
    
    //绘制公转小球（公转自转)
    sphere_modelViewMatrix.PushMatrix();
    sphere_modelViewMatrix.Rotate(yRot * -2.0f, 0.0f, 1.0f, 0.0f);
    sphere_modelViewMatrix.Translate(0.8f, 0.0f, 0.0f);
    glBindTexture(GL_TEXTURE_2D, uiTextures[2]);
    sphere_shaderManager.UseStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF,
                                        sphere_modelViewMatrix.GetMatrix(),
                                        sphere_transformPipeline.GetProjectionMatrix(),
                                        vLightPos,
                                        vWhite,
                                        0);
    sphereBatch.Draw();
    sphere_modelViewMatrix.PopMatrix();
}

void sphere_renderScene()
{
    //地板颜色值
    static GLfloat vFloorColor[] = { 1.0f, 1.0f, 0.0f, 0.75f };
    
    //基于时间动画
    static CStopWatch    rotTimer;
    float yRot = rotTimer.GetElapsedSeconds() * 60.0f;
    
    //清除颜色缓存区和深度缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //压入栈(栈顶)
    sphere_modelViewMatrix.PushMatrix();
    
    //设置观察者矩阵
    M3DMatrix44f mCamera;
    sphere_cameraFrame.GetCameraMatrix(mCamera);
    sphere_modelViewMatrix.MultMatrix(mCamera);
    
    //压栈(镜面)
    sphere_modelViewMatrix.PushMatrix();
    
    //---添加反光效果---
    sphere_modelViewMatrix.Scale(1.0f, -1.0f, 1.0f);
    sphere_modelViewMatrix.Translate(0.0f, 0.8f, 0.0f);
    
    //指定顺时针为正面
    glFrontFace(GL_CW);
    
    //绘制地面以外其他部分(镜面)
    sphere_drawSomething(yRot);
    
    //恢复为逆时针为正面
    glFrontFace(GL_CCW);
    
    //绘制镜面，恢复矩阵
    sphere_modelViewMatrix.PopMatrix();
    
    //开启混合功能(绘制地板)
    glEnable(GL_BLEND);
    //指定glBlendFunc 颜色混合方程式
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //绑定地面纹理
    glBindTexture(GL_TEXTURE_2D, uiTextures[0]);
    
    /*
    纹理调整着色器(将一个基本色乘以一个取自纹理的单元nTextureUnit的纹理)
    参数1：GLT_SHADER_TEXTURE_MODULATE
    参数2：模型视图投影矩阵
    参数3：颜色
    参数4：纹理单元（第0层的纹理单元）
    */
    sphere_shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE,
                                        sphere_transformPipeline.GetModelViewProjectionMatrix(),
                                        vFloorColor,
                                        0);
    sphere_floorBatch.Draw();
    
    //取消混合
    glDisable(GL_BLEND);
    
    //绘制地面以外其他部分
    sphere_drawSomething(yRot);
    
    //绘制完，恢复矩阵
    sphere_modelViewMatrix.PopMatrix();
    
    glutSwapBuffers();
    glutPostRedisplay();
}

// 移动照相机参考帧，来对方向键作出响应
void sphere_speacialKeys(int key,int x,int y)
{
    
    float linear = 0.1f;
    float angular = float(m3dDegToRad(5.0f));
    
    if (key == GLUT_KEY_UP) {
        
        //MoveForward 平移
        sphere_cameraFrame.MoveForward(linear);
    }
    
    if (key == GLUT_KEY_DOWN) {
        sphere_cameraFrame.MoveForward(-linear);
    }
    
    if (key == GLUT_KEY_LEFT) {
        //RotateWorld 旋转
        sphere_cameraFrame.RotateWorld(angular, 0.0f, 1.0f, 0.0f);
    }
    
    if (key == GLUT_KEY_RIGHT) {
        sphere_cameraFrame.RotateWorld(-angular, 0.0f, 1.0f, 0.0f);
    }
}

void sphere(int argc, char *argv[])
{
    gltSetWorkingDirectory(argv[0]);
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800,600);
    
    glutCreateWindow("OpenGL SphereWorld");
    
    glutReshapeFunc(sphere_changeSize);
    glutDisplayFunc(sphere_renderScene);
    glutSpecialFunc(sphere_speacialKeys);
    
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return;
    }
    
    
    sphere_setupRC();
    glutMainLoop();
    sphere_shutdownRC();
}

#endif /* sphere_h */
