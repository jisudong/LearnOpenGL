//
//  triangle.h
//  OpenGL
//
//  Created by apple on 2020/4/26.
//  Copyright © 2020 apple. All rights reserved.
//

#ifndef triangle_h
#define triangle_h

#include "GLShaderManager.h"
#include "GLTools.h"
#include <GLUT/GLUT.h>


/*
  移入了GLTool着色器管理器（shader Mananger）类。没有着色器，我们就不能在OpenGL（核心框架）进行着色。
  着色器管理器不仅允许我们创建并管理着色器，还提供一组“存储着色器”，他们能够进行一些初步的基本的渲染操作
 */
GLShaderManager tri_shaderManager;

/// 简单的批次容器，是GLTools的一个简单的容器类。
GLBatch triangleBatch;

void tri_changeSize(int w, int h)
{
    glViewport(0, 0, w, h);
}

void tri_renderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    GLfloat vred[] = {1.0, 0.0, 0.0, 1.0};
    tri_shaderManager.UseStockShader(GLT_SHADER_IDENTITY, vred);
    
    triangleBatch.Draw();
    
    glutSwapBuffers();
}

void tri_setup()
{
    // 设置清屏颜色（背景颜色）
    glClearColor(0.5f, 0.40f, 0.7f, 1);
     
    // 初始化一个着色器
    tri_shaderManager.InitializeStockShaders();
    
    // 顶点数组
    GLfloat vVerts[] = {
        -0.5f, 0.0f, 0.0f,
         0.5f, 0.0f, 0.0f,
         0.0f, 0.5f, 0.0f
    };
    
    triangleBatch.Begin(GL_TRIANGLES, 3);
    triangleBatch.CopyVertexData3f(vVerts);
    triangleBatch.End();
}

void drawTriangle(int argc,char *argv[])
{
    glutInit(&argc, argv);
    
    /*
    初始化双缓冲窗口，其中标志GLUT_DOUBLE、GLUT_RGBA、GLUT_DEPTH、GLUT_STENCIL分别指
    双缓冲窗口、RGBA颜色模式、深度测试、模板缓冲区
    
    --GLUT_DOUBLE`：双缓存窗口，是指绘图命令实际上是离屏缓存区执行的，然后迅速转换成窗口视图，这种方式，经常用来生成动画效果；
    --GLUT_DEPTH`：标志将一个深度缓存区分配为显示的一部分，因此我们能够执行深度测试；
    --GLUT_STENCIL`：确保我们也会有一个可用的模板缓存区。
    */
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    
    glutInitWindowSize(800, 600);
    glutCreateWindow("Triangle");
    
    // 注册窗口改变的回调函数
    glutReshapeFunc(tri_changeSize);
    // 注册显示函数
    glutDisplayFunc(tri_renderScene);
    
    GLenum status = glewInit();
    if (GLEW_OK != status) {
        printf("GLEW Error:%s\n",glewGetErrorString(status));
        return;
    }
    
    tri_setup();
    
    glutMainLoop();
    
}


#endif /* triangle_h */
