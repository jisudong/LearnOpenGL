//
//  CubeViewController.m
//  OpenGL ES
//
//  Created by apple on 2020/6/18.
//  Copyright © 2020 apple. All rights reserved.
//

#import "CubeViewController.h"
#import <GLKit/GLKit.h>

typedef struct {
    GLKVector3 position;
    GLKVector2 textureCoord;
    GLKVector3 normal;
} Vertex;

static NSInteger const kVertexCount = 36;

@interface CubeViewController () <GLKViewDelegate>

@property (nonatomic, strong) GLKView *glkView;
@property (nonatomic, strong) GLKBaseEffect *baseEffect;
@property (nonatomic, assign) Vertex *vertices;

@property (nonatomic, strong) CADisplayLink *displayLink;
@property (nonatomic, assign) NSInteger angle;
@property (nonatomic, assign) GLuint vertexBuffer;

@end

@implementation CubeViewController

- (void)dealloc
{
    if ([EAGLContext currentContext] == self.glkView.context) {
        [EAGLContext setCurrentContext:nil];
    }
    if (_vertices) {
        free(_vertices);
        _vertices = nil;
    }
    
    if (_vertexBuffer) {
        glDeleteBuffers(1, &_vertexBuffer);
        _vertexBuffer = 0;
    }
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self setupConfig];
    
    [self setupVertex];
    
    [self addDisplayLink];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];
    
    [self.displayLink invalidate];
    self.displayLink = nil;
}

- (void)setupConfig
{
    EAGLContext *context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
    [EAGLContext setCurrentContext:context];
    
    self.glkView = [[GLKView alloc] initWithFrame:CGRectMake(0, 100, self.view.frame.size.width, self.view.frame.size.width) context:context];
    self.glkView.backgroundColor = [UIColor clearColor];
    self.glkView.delegate = self;
    self.glkView.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    glDepthRangef(1.0f, 0.0f);
    [self.view addSubview:self.glkView];
    
    NSString *path = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"kunkun.jpg"];
    UIImage *image = [UIImage imageWithContentsOfFile:path];
    
    NSDictionary *options = @{GLKTextureLoaderOriginBottomLeft : @(YES)};
    GLKTextureInfo *info = [GLKTextureLoader textureWithCGImage:image.CGImage options:options error:nil];
    
    self.baseEffect = [[GLKBaseEffect alloc] init];
    self.baseEffect.texture2d0.name = info.name;
    self.baseEffect.texture2d0.target = info.target;
    
    glEnable(GL_DEPTH_TEST);
}

- (void)setupVertex
{
    self.vertices = malloc(sizeof(Vertex) * kVertexCount);
    
    // 前面
    self.vertices[0] = (Vertex){{-0.5, 0.5, 0.5},  {0, 1}};
    self.vertices[1] = (Vertex){{-0.5, -0.5, 0.5}, {0, 0}};
    self.vertices[2] = (Vertex){{0.5, 0.5, 0.5},   {1, 1}};
    
    self.vertices[3] = (Vertex){{-0.5, -0.5, 0.5}, {0, 0}};
    self.vertices[4] = (Vertex){{0.5, 0.5, 0.5},   {1, 1}};
    self.vertices[5] = (Vertex){{0.5, -0.5, 0.5},  {1, 0}};
    
    // 上面
    self.vertices[6] = (Vertex){{0.5, 0.5, 0.5},    {1, 1}};
    self.vertices[7] = (Vertex){{-0.5, 0.5, 0.5},   {0, 1}};
    self.vertices[8] = (Vertex){{0.5, 0.5, -0.5},   {1, 0}};
    self.vertices[9] = (Vertex){{-0.5, 0.5, 0.5},   {0, 1}};
    self.vertices[10] = (Vertex){{0.5, 0.5, -0.5},  {1, 0}};
    self.vertices[11] = (Vertex){{-0.5, 0.5, -0.5}, {0, 0}};
    
    // 下面
    self.vertices[12] = (Vertex){{0.5, -0.5, 0.5},    {1, 1}};
    self.vertices[13] = (Vertex){{-0.5, -0.5, 0.5},   {0, 1}};
    self.vertices[14] = (Vertex){{0.5, -0.5, -0.5},   {1, 0}};
    self.vertices[15] = (Vertex){{-0.5, -0.5, 0.5},   {0, 1}};
    self.vertices[16] = (Vertex){{0.5, -0.5, -0.5},   {1, 0}};
    self.vertices[17] = (Vertex){{-0.5, -0.5, -0.5},  {0, 0}};
    
    // 左面
    self.vertices[18] = (Vertex){{-0.5, 0.5, 0.5},    {1, 1}};
    self.vertices[19] = (Vertex){{-0.5, -0.5, 0.5},   {0, 1}};
    self.vertices[20] = (Vertex){{-0.5, 0.5, -0.5},   {1, 0}};
    self.vertices[21] = (Vertex){{-0.5, -0.5, 0.5},   {0, 1}};
    self.vertices[22] = (Vertex){{-0.5, 0.5, -0.5},   {1, 0}};
    self.vertices[23] = (Vertex){{-0.5, -0.5, -0.5},  {0, 0}};
    
    // 右面
    self.vertices[24] = (Vertex){{0.5, 0.5, 0.5},    {1, 1}};
    self.vertices[25] = (Vertex){{0.5, -0.5, 0.5},   {0, 1}};
    self.vertices[26] = (Vertex){{0.5, 0.5, -0.5},   {1, 0}};
    self.vertices[27] = (Vertex){{0.5, -0.5, 0.5},   {0, 1}};
    self.vertices[28] = (Vertex){{0.5, 0.5, -0.5},   {1, 0}};
    self.vertices[29] = (Vertex){{0.5, -0.5, -0.5},  {0, 0}};
    
    // 后面
    self.vertices[30] = (Vertex){{-0.5, 0.5, -0.5},   {0, 1}};
    self.vertices[31] = (Vertex){{-0.5, -0.5, -0.5},  {0, 0}};
    self.vertices[32] = (Vertex){{0.5, 0.5, -0.5},    {1, 1}};
    self.vertices[33] = (Vertex){{-0.5, -0.5, -0.5},  {0, 0}};
    self.vertices[34] = (Vertex){{0.5, 0.5, -0.5},    {1, 1}};
    self.vertices[35] = (Vertex){{0.5, -0.5, -0.5},   {1, 0}};
    
    glGenBuffers(1, &_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, self.vertexBuffer);
    GLsizeiptr bufferSize = sizeof(Vertex) * kVertexCount;
    glBufferData(GL_ARRAY_BUFFER, bufferSize, self.vertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(GLKVertexAttribPosition);
    glVertexAttribPointer(GLKVertexAttribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, position));
    
    glEnableVertexAttribArray(GLKVertexAttribTexCoord0);
    glVertexAttribPointer(GLKVertexAttribTexCoord0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, textureCoord));
    
    
}

- (void)addDisplayLink
{
    self.angle = 0;
    self.displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(updateRender)];
    [self.displayLink addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSRunLoopCommonModes];
}

- (void)updateRender
{
    self.angle+=5;
    
    self.baseEffect.transform.modelviewMatrix = GLKMatrix4MakeRotation(GLKMathDegreesToRadians(self.angle), 0.3f, 1.0f, 0.5f);
    
    // 重新渲染
    [self.glkView display];
}

#pragma mark - GLKViewDelegate

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    [self.baseEffect prepareToDraw];
    
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
