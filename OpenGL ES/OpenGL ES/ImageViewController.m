//
//  ImageViewController.m
//  OpenGL ES
//
//  Created by apple on 2020/6/18.
//  Copyright © 2020 apple. All rights reserved.
//

#import "ImageViewController.h"

@implementation ImageViewController
{
    EAGLContext *context;
    GLKBaseEffect *effect;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    [self setupConfig];
    
    [self setupVertex];
    
    [self setupTexture];
}

- (void)setupConfig
{
    //1.初始化上下文&设置当前上下文
    /*
     EAGLContext 是苹果iOS平台下实现OpenGLES 渲染层.
     kEAGLRenderingAPIOpenGLES1 = 1, 固定管线
     kEAGLRenderingAPIOpenGLES2 = 2,
     kEAGLRenderingAPIOpenGLES3 = 3,
     */
    context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
    if (!context) {
        NSLog(@"create EAGLContext failed");
        return;
    }
    [EAGLContext setCurrentContext:context];
    
    //2.获取GLKView & 设置context
    GLKView *view = (GLKView *)self.view;
    view.context = context;
    /*3.配置视图创建的渲染缓存区.
    
    (1). drawableColorFormat: 颜色缓存区格式.
    简介:  OpenGL ES 有一个缓存区，它用以存储将在屏幕中显示的颜色。你可以使用其属性来设置缓冲区中的每个像素的颜色格式。
    
    GLKViewDrawableColorFormatRGBA8888 = 0,
    默认.缓存区的每个像素的最小组成部分（RGBA）使用8个bit，（所以每个像素4个字节，4*8个bit）。
    
    GLKViewDrawableColorFormatRGB565,
    如果你的APP允许更小范围的颜色，即可设置这个。会让你的APP消耗更小的资源（内存和处理时间）
    
    (2). drawableDepthFormat: 深度缓存区格式
    
    GLKViewDrawableDepthFormatNone = 0,意味着完全没有深度缓冲区
    GLKViewDrawableDepthFormat16,
    GLKViewDrawableDepthFormat24,
    如果你要使用这个属性（一般用于3D游戏），你应该选择GLKViewDrawableDepthFormat16
    或GLKViewDrawableDepthFormat24。这里的差别是使用GLKViewDrawableDepthFormat16
    将消耗更少的资源
    
    */
    view.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat16;
    
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
}

- (void)setupVertex
{
    GLfloat vertexData[] = {
        
        0.5, -0.5, 0.0f,    1.0f, 0.0f, //右下
        0.5, 0.5, -0.0f,    1.0f, 1.0f, //右上
        -0.5, 0.5, 0.0f,    0.0f, 1.0f, //左上
        
        0.5, -0.5, 0.0f,    1.0f, 0.0f, //右下
        -0.5, 0.5, 0.0f,    0.0f, 1.0f, //左上
        -0.5, -0.5, 0.0f,   0.0f, 0.0f, //左下
    };
    
    GLuint VBO = 0;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(GLKVertexAttribPosition);
    glVertexAttribPointer(GLKVertexAttribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void *)NULL);
    
    glEnableVertexAttribArray(GLKVertexAttribTexCoord0);
    glVertexAttribPointer(GLKVertexAttribTexCoord0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void *)(3 * sizeof(GLfloat)));
}

- (void)setupTexture
{
    NSString *path = [[NSBundle mainBundle] pathForResource:@"kunkun" ofType:@"jpg"];
    // 设置纹理参数
    NSDictionary *options = [NSDictionary dictionaryWithObjectsAndKeys:@(YES), GLKTextureLoaderOriginBottomLeft, nil];
    // 加载纹理
    GLKTextureInfo *info = [GLKTextureLoader textureWithContentsOfFile:path options:options error:nil];
    
    // 使用苹果GLKit 提供GLKBaseEffect 完成着色器工作(顶点/片元)
    effect = [[GLKBaseEffect alloc] init];
    effect.texture2d0.enabled = GL_TRUE;
    effect.texture2d0.name = info.name;
}

#pragma mark -- GLKViewDelegate

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    [effect prepareToDraw];
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

@end
