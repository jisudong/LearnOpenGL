//
//  LightViewController.m
//  OpenGL ES
//
//  Created by apple on 2020/6/29.
//  Copyright © 2020 apple. All rights reserved.
//

#import "LightViewController.h"
#import "AGLKVertexAttribArrayBuffer.h"
#import "sceneUtil.h"

@interface LightViewController ()


@property(nonatomic,strong) EAGLContext *mContext;

//基本Effect
@property(nonatomic,strong) GLKBaseEffect *baseEffect;
//额外Effect
@property(nonatomic,strong) GLKBaseEffect *extraEffect;

//顶点缓存区
@property(nonatomic,strong) AGLKVertexAttribArrayBuffer *vertexBuffer;
//法线位置缓存区
@property(nonatomic,strong) AGLKVertexAttribArrayBuffer *extraBuffer;

//是否绘制法线
@property(nonatomic,assign) BOOL shouldDrawNormals;
//中心点的高
@property(nonatomic,assign) GLfloat centexVertexHeight;

@end

@implementation LightViewController
{
    //三角形-8面
    SceneTriangle triangles[NUM_FACES];
}

- (void)dealloc
{
    NSLog(@"");
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self setupES];
    
    [self setupEffect];
    
    [self setupBuffer];
}

- (void)setupES
{
    self.mContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.mContext;
    view.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat16;
    
    [EAGLContext setCurrentContext:self.mContext];
}

- (void)setupEffect
{
    // 金字塔Effect
    self.baseEffect = [[GLKBaseEffect alloc] init];
    self.baseEffect.light0.enabled = GL_TRUE;
    //光的漫射部分 GLKVector4Make(R,G,B,A)
    self.baseEffect.light0.diffuseColor = GLKVector4Make(0.7f, 0.7f, 0.7f, 1.0f);
    //世界坐标中的光的位置。
    self.baseEffect.light0.position = GLKVector4Make(1.0f, 1.0f, 0.5f, 0.0f);
    
    self.extraEffect = [[GLKBaseEffect alloc] init];
    self.extraEffect.useConstantColor = GL_TRUE;
    
    GLKMatrix4 modelViewMatrix = GLKMatrix4MakeRotation(GLKMathDegreesToRadians(-60.0f), 1.0f, 0.0f, 0.0f);
    modelViewMatrix = GLKMatrix4Rotate(modelViewMatrix, GLKMathDegreesToRadians(-30.0f), 0.0f, 0.0f, 1.0f);
    modelViewMatrix = GLKMatrix4Translate(modelViewMatrix, 0.0f, 0.0f, 0.25f);
    self.baseEffect.transform.modelviewMatrix = modelViewMatrix;
    self.extraEffect.transform.modelviewMatrix = modelViewMatrix;
}

- (void)setupBuffer
{
    //确定图形的8个面
    triangles[0] = sceneTriangleMake(vertexA, vertexB, vertexD);
    triangles[1] = sceneTriangleMake(vertexB, vertexC, vertexF);
    triangles[2] = sceneTriangleMake(vertexD, vertexB, vertexE);
    triangles[3] = sceneTriangleMake(vertexE, vertexB, vertexF);
    triangles[4] = sceneTriangleMake(vertexD, vertexE, vertexH);
    triangles[5] = sceneTriangleMake(vertexE, vertexF, vertexH);
    triangles[6] = sceneTriangleMake(vertexG, vertexD, vertexH);
    triangles[7] = sceneTriangleMake(vertexH, vertexF, vertexI);
    
    self.vertexBuffer = [[AGLKVertexAttribArrayBuffer alloc] initWithAttribStride:sizeof(SceneVertex) numberOfVertices:sizeof(triangles) / sizeof(SceneVertex) bytes:triangles usage:GL_DYNAMIC_DRAW];
    
    self.extraBuffer = [[AGLKVertexAttribArrayBuffer alloc] initWithAttribStride:sizeof(SceneVertex) numberOfVertices:0 bytes:NULL usage:GL_DYNAMIC_DRAW];
    
    self.centexVertexHeight = 0.0f;
}

#pragma mark - GLKViewDelegate
- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    glClearColor(0.3f, 0.4f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    [self.baseEffect prepareToDraw];
    
    //准备绘制顶点数据
    [self.vertexBuffer prepareToDrawWithAttrib:AGLKVertexAttribPosition numberOfCoordinates:3 attribOffset:offsetof(SceneVertex, position) shouldEnable:YES];
    //准备绘制光照数据
    [self.vertexBuffer prepareToDrawWithAttrib:AGLKVertexAttribNormal numberOfCoordinates:3 attribOffset:offsetof(SceneVertex, normal) shouldEnable:YES];
    
    [self.vertexBuffer drawArrayWithMode:GL_TRIANGLES startVertexIndex:0 numberOfVertices:sizeof(triangles) / sizeof(SceneVertex)];
    
    if (self.shouldDrawNormals) {
        [self drawNormals];
    }
}

- (void)drawNormals
{
    GLKVector3 normalLineVertices[NUM_LINE_VERTS];
    
    // 以每个顶点的坐标为起点，顶点坐标加上法向量的偏移值作为终点，更新法线显示数组
    //参数1: 三角形数组
    //参数2：光源位置
    //参数3：法线显示的顶点数组
    sceneTrianglesNormalLinesUpdate(triangles, GLKVector3MakeWithArray(self.baseEffect.light0.position.v), normalLineVertices);
    
    // 为extraBuffer重新开辟空间
    [self.extraBuffer reinitWithAttribStride:sizeof(GLKVector3) numberOfVertices:NUM_LINE_VERTS bytes:normalLineVertices];
    
    // 准备绘制数据
    [self.extraBuffer prepareToDrawWithAttrib:GLKVertexAttribPosition numberOfCoordinates:3 attribOffset:0 shouldEnable:YES];
    
    // 修改extraEffect
    /*
    指示是否使用常量颜色的布尔值。
    如果该值设置为gl_true，然后存储在设置属性的值为每个顶点的颜色值。如果该值设置为gl_false，那么你的应用将使glkvertexattribcolor属性提供每顶点颜色数据。默认值是gl_false。
    */
    self.extraEffect.useConstantColor = GL_TRUE;
    //设置光源颜色为绿色，画顶点法线
    self.extraEffect.constantColor = GLKVector4Make(0.0f, 1.0f, 0.0f, 1.0f);
    [self.extraEffect prepareToDraw];
    //绘制线段
    [self.extraBuffer drawArrayWithMode:GL_LINES startVertexIndex:0 numberOfVertices:NUM_NORMAL_LINE_VERTS];
    
    //设置光源颜色为黄色，并且画光源线
    self.extraEffect.constantColor = GLKVector4Make(1.0f, 1.0f, 0.0f, 1.0f);
    [self.extraEffect prepareToDraw];
    [self.extraBuffer drawArrayWithMode:GL_LINES startVertexIndex:NUM_NORMAL_LINE_VERTS numberOfVertices:2];
}

- (void)updateNormals
{
    sceneTrianglesUpdateFaceNormals(triangles);
    
    [self.vertexBuffer reinitWithAttribStride:sizeof(SceneVertex) numberOfVertices:sizeof(triangles)/sizeof(SceneVertex) bytes:triangles];
}

- (void)setCentexVertexHeight:(GLfloat)centexVertexHeight
{
    _centexVertexHeight = centexVertexHeight;
    
    SceneVertex newVertexE = vertexE;
    newVertexE.position.z = _centexVertexHeight;
    
    triangles[2] = sceneTriangleMake(vertexD, vertexB, newVertexE);
    triangles[3] = sceneTriangleMake(newVertexE, vertexB, vertexF);
    triangles[4] = sceneTriangleMake(vertexD, newVertexE, vertexH);
    triangles[5] = sceneTriangleMake(newVertexE, vertexF, vertexH);
    
    [self updateNormals];
}

- (IBAction)enableNormal:(UISwitch *)sender
{
    self.shouldDrawNormals = sender.isOn;
}

- (IBAction)changeCenterVertexHeight:(UISlider *)sender
{
    self.centexVertexHeight = sender.value;
}
@end
