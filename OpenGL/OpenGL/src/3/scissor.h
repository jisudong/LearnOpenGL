//
//  scissor.h
//  OpenGL
//
//  Created by apple on 2020/4/30.
//  Copyright © 2020 apple. All rights reserved.
//

#ifndef scissor_h
#define scissor_h

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

void sc_renderScene()
{
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    //1.现在剪成小红色分区
    //(1)设置裁剪区颜色为红色
    glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
    //(2)设置裁剪尺寸
    glScissor(100, 100, 600, 400);
    //(3)开启裁剪测试
    glEnable(GL_SCISSOR_TEST);
    //(4)开启清屏，执行裁剪
    glClear(GL_COLOR_BUFFER_BIT);
    
    // 2.裁剪一个绿色的小矩形
    //(1).设置清屏颜色为绿色
    glClearColor(0.0f, 1.0f, 0.0f, 0.0f);
    //(2).设置裁剪尺寸
    glScissor(200, 200, 400, 200);
    //(3).开始清屏执行裁剪
    glClear(GL_COLOR_BUFFER_BIT);
    
    //关闭裁剪测试
    glDisable(GL_SCISSOR_TEST);
    
    //强制执行缓存区
    glutSwapBuffers();
    
}

void sc_changeSize(int w, int h)
{
    if (h == 0) {
        h = 1;
    }
    
    glViewport(0, 0, w, h);
}

void scissor(int argc,char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Scissor");
    glutReshapeFunc(sc_changeSize);
    glutDisplayFunc(sc_renderScene);
    glutMainLoop();
}

#endif /* scissor_h */
