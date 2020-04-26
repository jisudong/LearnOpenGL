//
//  rectangle.h
//  OpenGL
//
//  Created by apple on 2020/4/26.
//  Copyright © 2020 apple. All rights reserved.
//

#ifndef rectangle_h
#define rectangle_h

#include "GLShaderManager.h"
#include "GLTools.h"
#include <GLUT/GLUT.h>

GLShaderManager rec_shaderManager;
GLBatch rec_batch;

GLfloat blockSize = 0.1f;

GLfloat vVerts[] = {
    -blockSize,-blockSize,0.0f,
    blockSize,-blockSize,0.0f,
    blockSize,blockSize,0.0f,
    -blockSize,blockSize,0.0f
};

void rec_changeSize(int w, int h)
{
    glViewport(0, 0, w, h);
}

void rec_renderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    GLfloat vred[] = {1.0, 0.0, 0.0, 1.0};
    
    rec_shaderManager.UseStockShader(GLT_SHADER_IDENTITY, vred);
    
    rec_batch.Draw();
    
    glutSwapBuffers();
}

void rec_setup()
{
    glClearColor(0.5f, 0.40f, 0.7f, 1);
    
    rec_shaderManager.InitializeStockShaders();
    
    rec_batch.Begin(GL_TRIANGLE_FAN, 4);
    rec_batch.CopyVertexData3f(vVerts);
    rec_batch.End();
}

void rec_specialKeys(int key, int x, int y)
{
    GLfloat stepSize = 0.025f;
    GLfloat blockX = vVerts[0];
    GLfloat blockY = vVerts[10];
    
    printf("v[0] = %f\n",blockX);
    printf("v[10] = %f\n",blockY);
    
    if (key == GLUT_KEY_UP) {
        blockY += stepSize;
    }
    if (key == GLUT_KEY_DOWN) {
        blockY -= stepSize;
    }
    if (key == GLUT_KEY_LEFT) {
        blockX -= stepSize;
    }
    if (key == GLUT_KEY_RIGHT) {
        blockX += stepSize;
    }
    
    //触碰到边界（4个边界）的处理
    
    //当正方形移动超过最左边的时候
    if (blockX < -1.0f) {
        blockX = -1.0f;
    }
    
    //当正方形移动到最右边时
    //1.0 - blockSize * 2 = 总边长 - 正方形的边长 = 最左边点的位置
    if (blockX > (1.0 - blockSize * 2)) {
        blockX = 1.0f - blockSize * 2;
    }
    
    //当正方形移动到最下面时
    //-1.0 - blockSize * 2 = Y（负轴边界） - 正方形边长 = 最下面点的位置
    if (blockY < -1.0f + blockSize * 2 ) {
        
        blockY = -1.0f + blockSize * 2;
    }
    
    //当正方形移动到最上面时
    if (blockY > 1.0f) {
        
        blockY = 1.0f;
        
    }

    printf("blockX = %f\n",blockX);
    printf("blockY = %f\n",blockY);
    
    // Recalculate vertex positions
    vVerts[0] = blockX;
    vVerts[1] = blockY - blockSize*2;
    
    printf("(%f,%f)\n",vVerts[0],vVerts[1]);
    
    
    vVerts[3] = blockX + blockSize*2;
    vVerts[4] = blockY - blockSize*2;
    printf("(%f,%f)\n",vVerts[3],vVerts[4]);
    
    vVerts[6] = blockX + blockSize*2;
    vVerts[7] = blockY;
    printf("(%f,%f)\n",vVerts[6],vVerts[7]);
    
    vVerts[9] = blockX;
    vVerts[10] = blockY;
    printf("(%f,%f)\n",vVerts[9],vVerts[10]);
    
    rec_batch.CopyVertexData3f(vVerts);
    
    glutPostRedisplay();
}

void drawRectangle(int argc,char *argv[])
{
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    
    glutInitWindowSize(800, 600);
    glutCreateWindow("Rectangle");
    
    glutReshapeFunc(rec_changeSize);
    glutDisplayFunc(rec_renderScene);
    glutSpecialFunc(rec_specialKeys);
    
    GLenum status = glewInit();
    if (GLEW_OK != status) {
        printf("GLEW Error:%s\n",glewGetErrorString(status));
        return;
    }
    
    rec_setup();
    
    glutMainLoop();
    
}


#endif /* rectangle_h */
