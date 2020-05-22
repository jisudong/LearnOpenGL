//
//  texture.h
//  OpenGL
//
//  Created by apple on 2020/5/22.
//  Copyright © 2020 apple. All rights reserved.
//

#ifndef texture_h
#define texture_h


#include "GLTools.h"
#include "GLShaderManager.h"
#include "GLFrustum.h"
#include "GLBatch.h"
#include "GLFrame.h"
#include "GLMatrixStack.h"
#include "GLGeometryTransform.h"

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

GLShaderManager     texture_shaderManager;
GLMatrixStack       texture_modelViewMatrix;
GLMatrixStack       texture_projectionMatrix;
GLFrame             texture_cameraFrame;
GLFrame             texture_objectFrame;
GLFrustum           texture_viewFrustum;
GLBatch             texture_pyramidBatch;
GLuint              textureID;
GLGeometryTransform texture_transformPipeline;
M3DMatrix44f        texture_shadowMatrix;

void texture_makePyramid(GLBatch &pyramidBatch)
{
    /* 通过pyramidBatch组建三角形批次
     参数1：类型
     参数2：顶点数
     参数3：这个批次中将会应用1个纹理
     注意：如果不写这个参数，默认为0。
    */
    pyramidBatch.Begin(GL_TRIANGLES, 18, 1);
    
    /***前情导入
    
    1)设置法线
    void Normal3f(GLfloat x, GLfloat y, GLfloat z);
    Normal3f：添加一个表面法线（法线坐标 与 Vertex顶点坐标中的Y轴一致）
    表面法线是有方向的向量，代表表面或者顶点面对的方向（相反的方向）。在多数的关照模式下是必须使用。后面的课程会详细来讲法线的应用
    
    pyramidBatch.Normal3f(X,Y,Z);
    
    2)设置纹理坐标
    void MultiTexCoord2f(GLuint texture, GLclampf s, GLclampf t);
    参数1：texture，纹理层次，对于使用存储着色器来进行渲染，设置为0
    参数2：s：对应顶点坐标中的x坐标
    参数3：t:对应顶点坐标中的y
    (s,t,r,q对应顶点坐标的x,y,z,w)
    
    pyramidBatch.MultiTexCoord2f(0,s,t);
    
    3)void Vertex3f(GLfloat x, GLfloat y, GLfloat z);
     void Vertex3fv(M3DVector3f vVertex);
    向三角形批次类添加顶点数据(x,y,z);
     pyramidBatch.Vertex3f(-1.0f, -1.0f, -1.0f);
    
    
    4)获取从三点找到一个法线坐标(三点确定一个面)
    void m3dFindNormal(result,point1, point2,point3);
    参数1：结果
    参数2-4：3个顶点数据
    */
    
    // 塔顶
    M3DVector3f vApex = { 0.0f, 1.0f, 0.0f };
    M3DVector3f vFrontLeft = { -1.0f, -1.0f, 1.0f };
    M3DVector3f vFrontRight = { 1.0f, -1.0f, 1.0f };
    M3DVector3f vBackLeft = { -1.0f, -1.0f, -1.0f };
    M3DVector3f vBackRight = { 1.0f, -1.0f, -1.0f };
    M3DVector3f n;
    
    // 金字塔底部（2个三角形）
    // 找到三角形1的法线
    m3dFindNormal(n, vBackLeft, vBackRight, vFrontRight);
    // 映射纹理坐标
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
    pyramidBatch.Vertex3fv(vBackLeft);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackRight);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
    pyramidBatch.Vertex3fv(vFrontRight);
    
    // 找到三角形2的法线
    m3dFindNormal(n, vFrontLeft, vBackLeft, vFrontRight);
    // 映射纹理坐标
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
    pyramidBatch.Vertex3fv(vFrontLeft);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackLeft);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
    pyramidBatch.Vertex3fv(vFrontRight);
    
    
    // 金子塔前面
    m3dFindNormal(n, vApex, vFrontLeft, vFrontRight);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
    pyramidBatch.Vertex3fv(vApex);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3fv(vFrontLeft);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidBatch.Vertex3fv(vFrontRight);
    
    // 金字塔左面
    m3dFindNormal(n, vApex, vBackLeft, vFrontLeft);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
    pyramidBatch.Vertex3fv(vApex);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackLeft);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidBatch.Vertex3fv(vFrontLeft);
    
    // 金字塔右面
    m3dFindNormal(n, vApex, vFrontRight, vBackRight);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
    pyramidBatch.Vertex3fv(vApex);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3fv(vFrontRight);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackRight);
    
    // 金字塔后面
    m3dFindNormal(n, vApex, vBackRight, vBackLeft);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
    pyramidBatch.Vertex3fv(vApex);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.0, 0.0);
    pyramidBatch.Vertex3fv(vBackRight);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackLeft);
    
    
    pyramidBatch.End();
}

bool texture_loadTGATexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode)
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
    
    glTexImage2D(GL_TEXTURE_2D, 0, nComponents, nWidth, nHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBits);
    free(pBits);
    
    glGenerateMipmap(GL_TEXTURE_2D);
    
    return true;
}

void texture_setupRC()
{
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    texture_shaderManager.InitializeStockShaders();
    
    glEnable(GL_DEPTH_TEST);
    
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    texture_loadTGATexture("stone.tga", GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR, GL_CLAMP_TO_EDGE);
    
    texture_makePyramid(texture_pyramidBatch);
    
    texture_cameraFrame.MoveForward(-10);
}

void texture_renderScene()
{
    static GLfloat vLightPos[] = { 1.0f, 1.0f, 0.0f };
    static GLfloat vWhite[] = { 1.0f, 1.0f, 1.0f };
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    texture_modelViewMatrix.PushMatrix();
    
    M3DMatrix44f mCamera;
    texture_cameraFrame.GetCameraMatrix(mCamera);
    //矩阵乘以矩阵堆栈顶部矩阵，相乘结果存储到堆栈的顶部 将照相机矩阵 与 当前模型矩阵相乘 压入栈顶
    texture_modelViewMatrix.MultMatrix(mCamera);
    
    M3DMatrix44f mObjectFrame;
    texture_objectFrame.GetMatrix(mObjectFrame);
    texture_modelViewMatrix.MultMatrix(mObjectFrame);
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    /*5.点光源着色器
    参数1：GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF（着色器标签）
    参数2：模型视图矩阵
    参数3：投影矩阵
    参数4：视点坐标系中的光源位置
    参数5：基本漫反射颜色
    参数6：图形颜色（用纹理就不需要设置颜色。设置为0）
    */
    texture_shaderManager.UseStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF, texture_transformPipeline.GetModelViewMatrix(), texture_transformPipeline.GetProjectionMatrix(), vLightPos, vWhite, 0);
    
    texture_pyramidBatch.Draw();
    
    texture_modelViewMatrix.PopMatrix();
    
    glutSwapBuffers();
}

void texture_specialKeys(int key, int x, int y)
{
    if(key == GLUT_KEY_UP)
        texture_objectFrame.RotateWorld(m3dDegToRad(-5.0f), 1.0f, 0.0f, 0.0f);
    
    if(key == GLUT_KEY_DOWN)
        texture_objectFrame.RotateWorld(m3dDegToRad(5.0f), 1.0f, 0.0f, 0.0f);
    
    if(key == GLUT_KEY_LEFT)
        texture_objectFrame.RotateWorld(m3dDegToRad(-5.0f), 0.0f, 1.0f, 0.0f);
    
    if(key == GLUT_KEY_RIGHT)
        texture_objectFrame.RotateWorld(m3dDegToRad(5.0f), 0.0f, 1.0f, 0.0f);
    
    glutPostRedisplay();
}

void texture_changeSize(int w, int h)
{
    glViewport(0, 0, w, h);
    
    texture_viewFrustum.SetPerspective(35.f, float(w)/float(h), 1.0f, 500.f);
    
    texture_projectionMatrix.LoadMatrix(texture_viewFrustum.GetProjectionMatrix());
    
    texture_transformPipeline.SetMatrixStacks(texture_modelViewMatrix, texture_projectionMatrix);
}

void texture1(int argc,char *argv[])
{
    gltSetWorkingDirectory(argv[0]);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Pyramid");
    glutReshapeFunc(texture_changeSize);
    glutSpecialFunc(texture_specialKeys);
    glutDisplayFunc(texture_renderScene);
    
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return;
    }
    
    texture_setupRC();
    
    glutMainLoop();
    
}

#endif /* texture_h */
