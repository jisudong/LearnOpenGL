//
//  tunnel.h
//  OpenGL
//
//  Created by apple on 2020/5/26.
//  Copyright © 2020 apple. All rights reserved.
//

#ifndef tunnel_h
#define tunnel_h


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


GLShaderManager     tunnel_shaderManager;
GLMatrixStack       tunnel_modelViewMatrix;
GLMatrixStack       tunnel_projectionMatrix;
GLFrustum           tunnel_viewFrustum;
GLGeometryTransform tunnel_transformPipeline;

GLBatch             tunnel_floorBatch; // 地面
GLBatch             tunnel_ceilingBatch; // 天花板
GLBatch             tunnel_leftWallBatch; // 左墙面
GLBatch             tunnel_rightWallBatch; // 右墙面

GLfloat             viewZ = -65.f;

// 纹理标识符号
#define TEXTURE_BRICK   0 //墙面
#define TEXTURE_FLOOR   1 //地板
#define TEXTURE_CEILING 2 //纹理天花板
#define TEXTURE_COUNT   3 //纹理个数

GLuint  textures[TEXTURE_COUNT];//纹理标记数组
//文件tag名字数组
const char *szTextureFiles[TEXTURE_COUNT] = { "brick.tga", "floor.tga", "ceiling.tga" };

void tunnel_processMenu(int value)
{
    GLint iloop;
    for (iloop = 0; iloop < TEXTURE_COUNT; iloop++) {
        glBindTexture(GL_TEXTURE_2D, textures[iloop]);
        switch (value) {
            case 0:
                //GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER(缩小过滤器)，GL_NEAREST（邻近过滤）
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                break;
                
            case 1:
                //GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER(缩小过滤器)，GL_LINEAR（线性过滤）
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                break;
                
            case 2:
                //GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER(缩小过滤器)，GL_NEAREST_MIPMAP_NEAREST（选择最邻近的Mip层，并执行最邻近过滤）
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
                break;
                
            case 3:
                //GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER(缩小过滤器)，GL_NEAREST_MIPMAP_LINEAR（在Mip层之间执行线性插补，并执行最邻近过滤）
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
                break;
                
            case 4:
                //GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER(缩小过滤器)，GL_NEAREST_MIPMAP_LINEAR（选择最邻近Mip层，并执行线性过滤）
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
                break;
                
            case 5:
                //GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER(缩小过滤器)，GL_LINEAR_MIPMAP_LINEAR（在Mip层之间执行线性插补，并执行线性过滤，又称为三线性过滤）
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                break;
                
            case 6:
                //设置各向异性过滤
                GLfloat fLargest;
                //获取各向异性过滤的最大数量
                glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
                //设置纹理参数(各向异性采样)
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);
                break;
            
            case 7:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
                break;
        }
    }
    
    glutPostRedisplay();
}

void tunnel_setupRC()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    tunnel_shaderManager.InitializeStockShaders();
    
    GLbyte *pBytes;
    GLint iWidth, iHeight, iComponents;
    GLenum eFormat;
    GLint iloop;
    
    glGenTextures(TEXTURE_COUNT, textures);
    
    for (iloop = 0; iloop < TEXTURE_COUNT; iloop++) {
        // 绑定纹理
        glBindTexture(GL_TEXTURE_2D, textures[iloop]);
        // 加载tga文件
        pBytes = gltReadTGABits(szTextureFiles[iloop], &iWidth, &iHeight, &iComponents, &eFormat);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        /**载入纹理 glTexImage2D
        参数1：纹理维度，GL_TEXTURE_2D
        参数2：mip贴图层次
        参数3：纹理单元存储的颜色成分（从读取像素图中获得）
        参数4：加载纹理宽度
        参数5：加载纹理的高度
        参数6：加载纹理的深度
        参数7：像素数据的数据类型,GL_UNSIGNED_BYTE无符号整型
        参数8：指向纹理图像数据的指针
        */
        glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes);
        
        glGenerateMipmap(GL_TEXTURE_2D);
        
        free(pBytes);
    }
    
    // 设置几何图形顶点/纹理坐标(上.下.左.右)
    GLfloat z;
    
    // 地板
    tunnel_floorBatch.Begin(GL_TRIANGLE_STRIP, 28, 1);
    //Z表示深度，隧道的深度
    for(z = 60.0f; z >= 0.0f; z -=10.0f)
    {
        tunnel_floorBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
        tunnel_floorBatch.Vertex3f(-10.0f, -10.0f, z);
        
        tunnel_floorBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
        tunnel_floorBatch.Vertex3f(10.0f, -10.0f, z);
        
        tunnel_floorBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
        tunnel_floorBatch.Vertex3f(-10.0f, -10.0f, z - 10.0f);
        
        tunnel_floorBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
        tunnel_floorBatch.Vertex3f(10.0f, -10.0f, z - 10.0f);
    }
    tunnel_floorBatch.End();
    
    // 天花板
    tunnel_ceilingBatch.Begin(GL_TRIANGLE_STRIP, 28, 1);
     for(z = 60.0f; z >= 0.0f; z -=10.0f)
     {
         tunnel_ceilingBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
         tunnel_ceilingBatch.Vertex3f(-10.0f, 10.0f, z - 10.0f);
         
         tunnel_ceilingBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
         tunnel_ceilingBatch.Vertex3f(10.0f, 10.0f, z - 10.0f);
         
         tunnel_ceilingBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
         tunnel_ceilingBatch.Vertex3f(-10.0f, 10.0f, z);
         
         tunnel_ceilingBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
         tunnel_ceilingBatch.Vertex3f(10.0f, 10.0f, z);
     }
     tunnel_ceilingBatch.End();
     
     // 左墙面
     tunnel_leftWallBatch.Begin(GL_TRIANGLE_STRIP, 28, 1);
     for(z = 60.0f; z >= 0.0f; z -=10.0f)
     {
         tunnel_leftWallBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
         tunnel_leftWallBatch.Vertex3f(-10.0f, -10.0f, z);
         
         tunnel_leftWallBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
         tunnel_leftWallBatch.Vertex3f(-10.0f, 10.0f, z);
         
         tunnel_leftWallBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
         tunnel_leftWallBatch.Vertex3f(-10.0f, -10.0f, z - 10.0f);
         
         tunnel_leftWallBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
         tunnel_leftWallBatch.Vertex3f(-10.0f, 10.0f, z - 10.0f);
     }
     tunnel_leftWallBatch.End();
    
    // 右墙面
     tunnel_rightWallBatch.Begin(GL_TRIANGLE_STRIP, 28, 1);
     for(z = 60.0f; z >= 0.0f; z -=10.0f)
     {
         tunnel_rightWallBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
         tunnel_rightWallBatch.Vertex3f(10.0f, -10.0f, z);
         
         tunnel_rightWallBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
         tunnel_rightWallBatch.Vertex3f(10.0f, 10.0f, z);
         
         tunnel_rightWallBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
         tunnel_rightWallBatch.Vertex3f(10.0f, -10.0f, z - 10.0f);
         
         tunnel_rightWallBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
         tunnel_rightWallBatch.Vertex3f(10.0f, 10.0f, z - 10.0f);
     }
     tunnel_rightWallBatch.End();
}

void tunnel_shutdownRC()
{
    glDeleteTextures(TEXTURE_COUNT, textures);
}

void tunnel_specialKeys(int key, int x, int y)
{
    if (key == GLUT_KEY_UP) {
        viewZ += 0.5f;
    }
    if (key == GLUT_KEY_DOWN) {
        viewZ -= 0.5f;
    }
    
    glutPostRedisplay();
}

void tunnel_changeSize(int w, int h)
{
    if (h == 0) {
        h = 1;
    }
    
    glViewport(0, 0, w, h);
    
    // 生成透视投影
    GLfloat fAspect = GLfloat(w) / GLfloat(h);
    tunnel_viewFrustum.SetPerspective(80.f, fAspect, 1.0f, 100.f);
    tunnel_projectionMatrix.LoadMatrix(tunnel_viewFrustum.GetProjectionMatrix());
    tunnel_transformPipeline.SetMatrixStacks(tunnel_modelViewMatrix, tunnel_projectionMatrix);
    
}

void tunnel_renderScene()
{
    //1.用当前清除色，清除窗口
    glClear(GL_COLOR_BUFFER_BIT);
    
    //2.模型视图压栈
    tunnel_modelViewMatrix.PushMatrix();
    tunnel_modelViewMatrix.Translate(0.0f, 0.0f, viewZ);
    
    //3.纹理替换矩阵着色器
    /*
     参数1：GLT_SHADER_TEXTURE_REPLACE（着色器标签）
     参数2：模型视图投影矩阵
     参数3：纹理层
     */
    tunnel_shaderManager.UseStockShader(GLT_SHADER_TEXTURE_REPLACE, tunnel_transformPipeline.GetModelViewProjectionMatrix(), 0);
    
    //4.绑定纹理
    /*
     参数1：纹理模式，GL_TEXTURE_1D、GL_TEXTURE_2D、GL_TEXTURE_3D
     参数2：需要绑定的纹理
     */
    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_FLOOR]);
    tunnel_floorBatch.Draw();
    
    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_CEILING]);
    tunnel_ceilingBatch.Draw();
    
    glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_BRICK]);
    tunnel_leftWallBatch.Draw();
    tunnel_rightWallBatch.Draw();
    
    //5.pop
    tunnel_modelViewMatrix.PopMatrix();
    
    //6.交换缓存区
    glutSwapBuffers();
}

    
void tunnel(int argc,char *argv[])
{
    gltSetWorkingDirectory(argv[0]);
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Tunnel");
    glutSpecialFunc(tunnel_specialKeys);
    glutDisplayFunc(tunnel_renderScene);
    glutReshapeFunc(tunnel_changeSize);
    
    // 添加菜单入口，改变过滤器
    glutCreateMenu(tunnel_processMenu);
    glutAddMenuEntry("GL_NEAREST", 0);
    glutAddMenuEntry("GL_LINEAR", 1);
    glutAddMenuEntry("GL_NEAREST_MIPMAP_NEAREST", 2);
    glutAddMenuEntry("GL_NEAREST_MIPMAP_LINEAR", 3);
    glutAddMenuEntry("GL_LINEAR_MIPMAP_NEAREST", 4);
    glutAddMenuEntry("GL_LINEAR_MIPMAP_LINEAR", 5);
    glutAddMenuEntry("Anisotropic Filter", 6);
    glutAddMenuEntry("Anisotropic Off", 7);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return;
    }
    tunnel_setupRC();
    glutMainLoop();
    tunnel_shutdownRC();
}

#endif /* tunnel_h */
