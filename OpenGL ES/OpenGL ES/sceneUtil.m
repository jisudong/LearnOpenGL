//
//  sceneUtil.m
//  OpenGL ES
//
//  Created by apple on 2020/6/29.
//  Copyright © 2020 apple. All rights reserved.
//

#import "sceneUtil.h"

/**
*  创建一个三角形
*
*  @param vertexA 顶点A
*  @param vertexB 顶点B
*  @param vertexC 顶点C
*
*  @return 三角形
*/
SceneTriangle sceneTriangleMake(const SceneVertex vertexA, const SceneVertex vertexB, const SceneVertex vertexC)
{
    SceneTriangle result;
    result.vertices[0] = vertexA;
    result.vertices[1] = vertexB;
    result.vertices[2] = vertexC;
    return result;
}

/**
*  以点0为出发点，通过叉积计算平面法向量
*
*  @param triangle 三角形
*
*  @return 平面法向量
*/
GLKVector3 sceneTriangleFaceNormal(const SceneTriangle triangle)
{
    //vectorA =  v1 - v0
    GLKVector3 vectorA = GLKVector3Subtract(triangle.vertices[1].position, triangle.vertices[0].position);
    //vectorB =  v2 - v0
    GLKVector3 vectorB = GLKVector3Subtract(triangle.vertices[2].position, triangle.vertices[0].position);
    
    return sceneVector3UnitNormal(vectorA, vectorB);
}

/**
*  计算三角形平面法向量，更新每个点的平面法向量
*
*  @param someTriangles 三角形数组
*/
void sceneTrianglesUpdateFaceNormals(SceneTriangle someTriangles[NUM_FACES])
{
    for (int i = 0; i < NUM_FACES; i++) {
        //计算平面法向量
        GLKVector3 faceNormal = sceneTriangleFaceNormal(someTriangles[i]);
        
        //更新每个点的平面法向量
        someTriangles[i].vertices[0].normal = faceNormal;
        someTriangles[i].vertices[1].normal = faceNormal;
        someTriangles[i].vertices[2].normal = faceNormal;
    }
}

/**
*  计算各三角形的法向量，通过平均值求出每个点的法向量
*
*  @param someTriangles 三角形数组
*/
void sceneTrianglesUpdateVertexNormals(SceneTriangle someTriangles[NUM_FACES])
{
    //获取图形上的每个顶点
     SceneVertex newVertexA = vertexA;
     SceneVertex newVertexB = vertexB;
     SceneVertex newVertexC = vertexC;
     SceneVertex newVertexD = vertexD;
     
     //屏幕中心点，E（0,0,0）
     SceneVertex newVertexE = someTriangles[3].vertices[0];
     NSLog(@"%f",someTriangles[3].vertices[0].position.x);
     NSLog(@"%f",someTriangles[3].vertices[0].position.y);
     NSLog(@"%f",someTriangles[3].vertices[0].position.z);

     SceneVertex newVertexF = vertexF;
     SceneVertex newVertexG = vertexG;
     SceneVertex newVertexH = vertexH;
     SceneVertex newVertexI = vertexI;
     
     //8个面  平面法向量
     GLKVector3 faceNormals[NUM_FACES];
     
     for (int i=0; i<NUM_FACES; i++)
     {
         //将3个顶点-> 求法向量函数
         faceNormals[i] = sceneTriangleFaceNormal(someTriangles[i]);
     }
     

     //通过平均值求出每个点的法向量
     newVertexA.normal = faceNormals[0];
     /*
      //向量 * value
      GLKVector3  GLKVector3MultiplyScalar(GLKVector3 vector, float value)
    
      //A + B = C
      GLKVector3 GLKVector3Add(GLKVector3 vectorLeft, GLKVector3 vectorRight)

      //获取新的法线向量 * 倍数
      
      */
     newVertexB.normal = GLKVector3MultiplyScalar(GLKVector3Add(GLKVector3Add(GLKVector3Add(faceNormals[0],faceNormals[1]),faceNormals[2]),faceNormals[3]), 0.25);
     newVertexC.normal = faceNormals[1];
     newVertexD.normal = GLKVector3MultiplyScalar(GLKVector3Add(GLKVector3Add(GLKVector3Add(faceNormals[0],faceNormals[2]),faceNormals[4]),faceNormals[6]), 0.25);
     newVertexE.normal = GLKVector3MultiplyScalar(GLKVector3Add(GLKVector3Add(GLKVector3Add(faceNormals[2],faceNormals[3]),faceNormals[4]),faceNormals[5]), 0.25);
     newVertexF.normal = GLKVector3MultiplyScalar(GLKVector3Add(GLKVector3Add(GLKVector3Add(faceNormals[1],faceNormals[3]),faceNormals[5]),faceNormals[7]), 0.25);
     newVertexG.normal = faceNormals[6];
     newVertexH.normal = GLKVector3MultiplyScalar(GLKVector3Add(GLKVector3Add(GLKVector3Add(faceNormals[4],faceNormals[5]),faceNormals[6]),faceNormals[7]), 0.25);
     newVertexI.normal = faceNormals[7];
     
     //用新的点创建三角形
     someTriangles[0] = sceneTriangleMake(newVertexA, newVertexB, newVertexD);
     someTriangles[1] = sceneTriangleMake(newVertexB, newVertexC, newVertexF);
     someTriangles[2] = sceneTriangleMake(newVertexD, newVertexB, newVertexE);
     someTriangles[3] = sceneTriangleMake(newVertexE, newVertexB, newVertexF);
     someTriangles[4] = sceneTriangleMake(newVertexD, newVertexE, newVertexH);
     someTriangles[5] = sceneTriangleMake(newVertexE, newVertexF, newVertexH);
     someTriangles[6] = sceneTriangleMake(newVertexG, newVertexD, newVertexH);
     someTriangles[7] = sceneTriangleMake(newVertexH, newVertexF, newVertexI);
}

/**
*  以每个顶点的坐标为起点，顶点坐标加上法向量的偏移值作为终点，更新法线显示数组
*  最后一条线是光源
*
*  @param someTriangles          三角型数组
*  @param lightPosition          光源位置
*  @param someNormalLineVertices 法线显示顶点数组
*/
void sceneTrianglesNormalLinesUpdate(const SceneTriangle someTriangles[NUM_FACES], GLKVector3 lightPosition, GLKVector3 someNormalLineVertices[NUM_LINE_VERTS])
{
    int trianglesIndex;
    int lineVetexIndex = 0;
    

    for (trianglesIndex = 0; trianglesIndex < NUM_FACES;
         trianglesIndex++)
    {
        someNormalLineVertices[lineVetexIndex++] =
        someTriangles[trianglesIndex].vertices[0].position;
        someNormalLineVertices[lineVetexIndex++] =
        GLKVector3Add(someTriangles[trianglesIndex].vertices[0].position, GLKVector3MultiplyScalar(someTriangles[trianglesIndex].vertices[0].normal,0.5));
        someNormalLineVertices[lineVetexIndex++] = someTriangles[trianglesIndex].vertices[1].position;
        someNormalLineVertices[lineVetexIndex++] = GLKVector3Add(someTriangles[trianglesIndex].vertices[1].position,GLKVector3MultiplyScalar(someTriangles[trianglesIndex].vertices[1].normal,0.5));
        someNormalLineVertices[lineVetexIndex++] = someTriangles[trianglesIndex].vertices[2].position;
        someNormalLineVertices[lineVetexIndex++] = GLKVector3Add(someTriangles[trianglesIndex].vertices[2].position,GLKVector3MultiplyScalar(someTriangles[trianglesIndex].vertices[2].normal,0.5));
    }
    
    // 添加一行以指示光的方向。
    someNormalLineVertices[lineVetexIndex++] = lightPosition;
    
    someNormalLineVertices[lineVetexIndex] = GLKVector3Make( 0.0, 0.0,-0.5);
}


/**
 *  通过向量A和向量B的叉积求出平面法向量，单元化后返回
 *
 *  @param vectorA 向量A
 *  @param vectorB 向量B
 *
 *  @return 单元平面法向量
 *  通过叉积求单位法向量函数
 */
GLKVector3 sceneVector3UnitNormal(const GLKVector3 vectorA, const GLKVector3 vectorB)
{
    return GLKVector3Normalize(GLKVector3CrossProduct(vectorA, vectorB));
}
