//
//  blend.h
//  OpenGL
//
//  Created by apple on 2020/4/30.
//  Copyright © 2020 apple. All rights reserved.
//

#ifndef blend_h
#define blend_h

#include "GLTools.h"
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

GLShaderManager bl_shaderManager;
GLBatch redBatch;
GLBatch greenBatch;
GLBatch blueBatch;
GLBatch blackBatch;
GLBatch squareBatch;

GLfloat bl_blockSize = 0.2f;
GLfloat vertex[] = {
    -bl_blockSize, -bl_blockSize, 0.0f,
    bl_blockSize,  -bl_blockSize, 0.0f,
    bl_blockSize,   bl_blockSize, 0.0f,
    -bl_blockSize,  bl_blockSize, 0.0f,
};

void bl_setupRC()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f );
    bl_shaderManager.InitializeStockShaders();

    //绘制1个移动矩形
    squareBatch.Begin(GL_TRIANGLE_FAN, 4);
    squareBatch.CopyVertexData3f(vVerts);
    squareBatch.End();
    
    //绘制4个固定矩形
    GLfloat vBlock[] = { 0.25f, 0.25f, 0.0f,
        0.75f, 0.25f, 0.0f,
        0.75f, 0.75f, 0.0f,
        0.25f, 0.75f, 0.0f};
    
    greenBatch.Begin(GL_TRIANGLE_FAN, 4);
    greenBatch.CopyVertexData3f(vBlock);
    greenBatch.End();
    
    
    GLfloat vBlock2[] = { -0.75f, 0.25f, 0.0f,
        -0.25f, 0.25f, 0.0f,
        -0.25f, 0.75f, 0.0f,
        -0.75f, 0.75f, 0.0f};
    
    redBatch.Begin(GL_TRIANGLE_FAN, 4);
    redBatch.CopyVertexData3f(vBlock2);
    redBatch.End();
    
    
    GLfloat vBlock3[] = { -0.75f, -0.75f, 0.0f,
        -0.25f, -0.75f, 0.0f,
        -0.25f, -0.25f, 0.0f,
        -0.75f, -0.25f, 0.0f};
    
    blueBatch.Begin(GL_TRIANGLE_FAN, 4);
    blueBatch.CopyVertexData3f(vBlock3);
    blueBatch.End();
    
    
    GLfloat vBlock4[] = { 0.25f, -0.75f, 0.0f,
        0.75f, -0.75f, 0.0f,
        0.75f, -0.25f, 0.0f,
        0.25f, -0.25f, 0.0f};
    
    blackBatch.Begin(GL_TRIANGLE_FAN, 4);
    blackBatch.CopyVertexData3f(vBlock4);
    blackBatch.End();
}

void bl_specialKeys(int key, int x, int y)
{
    GLfloat stepsize = 0.025f;

    GLfloat blockX = vertex[0];
    GLfloat blockY = vertex[7];

    if (key == GLUT_KEY_UP) {
        blockY += stepsize;
    }
    if (key == GLUT_KEY_DOWN) {
        blockY -= stepsize;
    }
    if (key == GLUT_KEY_LEFT) {
        blockX -= stepsize;
    }
    if (key == GLUT_KEY_RIGHT) {
        blockX += stepsize;
    }

    if (blockX < -1.0f) {
        blockX = -1.0f;
    }
    if (blockX > (1.0f - bl_blockSize * 2)) {
        blockX = 1.0f - bl_blockSize * 2;
    }
    if (blockY < -1.0f + bl_blockSize * 2) {
        blockY = -1.0f + bl_blockSize * 2;
    }
    if (blockY > 1.0f) {
        blockY = 1.0f;
    }

    vertex[0] = blockX;
    vertex[1] = blockY - bl_blockSize * 2;

    vertex[3] = blockX + bl_blockSize * 2;
    vertex[4] = blockY - bl_blockSize * 2;

    vertex[6] = blockX + bl_blockSize * 2;
    vertex[7] = blockY;

    vertex[9] = blockX;
    vertex[10] = blockY;

    squareBatch.CopyVertexData3f(vertex);

    glutPostRedisplay();
}

void bl_renderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    //定义4种颜色
    GLfloat vRed[] = { 1.0f, 0.0f, 0.0f, 0.5f };
    GLfloat vGreen[] = { 0.0f, 1.0f, 0.0f, 1.0f };
    GLfloat vBlue[] = { 0.0f, 0.0f, 1.0f, 1.0f };
    GLfloat vBlack[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    
    bl_shaderManager.UseStockShader(GLT_SHADER_IDENTITY, vGreen);
    greenBatch.Draw();
    
    bl_shaderManager.UseStockShader(GLT_SHADER_IDENTITY, vRed);
    redBatch.Draw();
    
    bl_shaderManager.UseStockShader(GLT_SHADER_IDENTITY, vBlue);
    blueBatch.Draw();
    

    bl_shaderManager.UseStockShader(GLT_SHADER_IDENTITY, vBlack);
    blackBatch.Draw();


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    bl_shaderManager.UseStockShader(GLT_SHADER_IDENTITY, vRed);
    squareBatch.Draw();
    glDisable(GL_BLEND);
    
    glutSwapBuffers();
}

void bl_changeSize(int w, int h)
{
    glViewport(0, 0, w, h);
}

void blend(int argc,char *argv[])
{
    gltSetWorkingDirectory(argv[0]);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("移动矩形，观察颜色");
    
    
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        return;
    }
    
    glutReshapeFunc(bl_changeSize);
    glutDisplayFunc(bl_renderScene);
    glutSpecialFunc(bl_specialKeys);
    
    bl_setupRC();
    glutMainLoop();
}

#endif /* blend_h */
