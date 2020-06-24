//
//  SDView.m
//  OpenGL ES
//
//  Created by apple on 2020/6/23.
//  Copyright © 2020 apple. All rights reserved.
//

#import "SDView.h"
#import <OpenGLES/ES3/gl.h>

@interface SDView ()

@property (nonatomic, strong) CAEAGLLayer *eaglLayer;
@property (nonatomic, strong) EAGLContext *context;

@property (nonatomic, assign) GLuint colorRenderBuffer;
@property (nonatomic, assign) GLuint colorFrameBuffer;

@property (nonatomic, assign) GLuint program;

@end

@implementation SDView

- (void)dealloc
{
    [self deleteBuffers];
}

- (void)layoutSubviews
{
    [self setupLayer];
    
    [self setupContext];
    
    [self deleteBuffers];
    
    [self setupRenderBuffer];
    
    [self setupFrameBuffer];
    
    [self renderLayer];
}

//1.设置图层
- (void)setupLayer
{
    //1.创建特殊图层
    /*
     重写layerClass，将SDView返回的图层从CALayer替换成CAEAGLLayer
     */
    self.eaglLayer = (CAEAGLLayer *)self.layer;
    
    //2.设置scale
    [self setContentScaleFactor:[UIScreen mainScreen].scale];
    
    //3.设置描述属性，这里设置不维持渲染内容以及颜色格式为RGBA8
    /*
     kEAGLDrawablePropertyRetainedBacking  表示绘图表面显示后，是否保留其内容。
     kEAGLDrawablePropertyColorFormat
         可绘制表面的内部颜色缓存区格式，这个key对应的值是一个NSString指定特定颜色缓存区对象。默认是kEAGLColorFormatRGBA8；
     
         kEAGLColorFormatRGBA8：32位RGBA的颜色，4*8=32位
         kEAGLColorFormatRGB565：16位RGB的颜色，
         kEAGLColorFormatSRGBA8：sRGB代表了标准的红、绿、蓝，即CRT显示器、LCD显示器、投影机、打印机以及其他设备中色彩再现所使用的三个基本色素。sRGB的色彩空间基于独立的色彩坐标，可以使色彩在不同的设备使用传输中对应于同一个色彩坐标体系，而不受这些设备各自具有的不同色彩坐标的影响。
     */
    self.eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:@(false), kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
}

+ (Class)layerClass
{
    return [CAEAGLLayer class];
}

// 设置上下文
- (void)setupContext
{
    EAGLContext *context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
    if (!context) {
        NSLog(@"Create context failed!");
        return;
    }
    
    if (![EAGLContext setCurrentContext:context]) {
        NSLog(@"setCurrentContext failed!");
        return;
    }
    
    self.context = context;
}

// 清空缓冲区
- (void)deleteBuffers
{
    glDeleteBuffers(1, &_colorRenderBuffer);
    self.colorRenderBuffer = 0;
    
    glDeleteBuffers(1, &_colorFrameBuffer);
    self.colorFrameBuffer = 0;
}

// 设置renderbuffer
- (void)setupRenderBuffer
{
    GLuint buffer;
    
    glGenRenderbuffers(1, &buffer);
    
    self.colorRenderBuffer = buffer;
    
    // 将缓存id绑定到GL_RENDERBUFFER
    glBindRenderbuffer(GL_RENDERBUFFER, self.colorRenderBuffer);
    
    // 将可绘制对象drawable object's  CAEAGLLayer的存储绑定到OpenGL ES renderBuffer对象
    [self.context renderbufferStorage:GL_RENDERBUFFER fromDrawable:self.eaglLayer];
}

// 设置framebuffer
- (void)setupFrameBuffer
{
    GLuint buffer;
    
    glGenBuffers(1, &buffer);
    
    self.colorFrameBuffer = buffer;
    
    glBindFramebuffer(GL_FRAMEBUFFER, self.colorFrameBuffer);
    
    // 将渲染缓存区myColorRenderBuffer 通过glFramebufferRenderbuffer函数绑定到 GL_COLOR_ATTACHMENT0上。
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, self.colorRenderBuffer);
}

// 加载纹理
- (GLuint)setupTexture:(NSString *)filename
{
    CGImageRef cgimage = [UIImage imageNamed:filename].CGImage;
    if (!cgimage) {
        NSLog(@"Failed to load image %@", filename);
        return -1;
    }
    
    size_t width = CGImageGetWidth(cgimage);
    size_t height = CGImageGetHeight(cgimage);
    
    GLubyte *data = (GLubyte *)calloc(width * height * 4, sizeof(GLubyte));
    
    //4.创建绘图上下文
    /*
     参数1：data,指向要渲染的绘制图像的内存地址
     参数2：width,bitmap的宽度，单位为像素
     参数3：height,bitmap的高度，单位为像素
     参数4：bitPerComponent,内存中像素的每个组件的位数，比如32位RGBA，就设置为8
     参数5：bytesPerRow,bitmap的没一行的内存所占的比特数
     参数6：colorSpace,bitmap上使用的颜色空间
     参数7：bitmapInfo,kCGImageAlphaPremultipliedLast：RGBA
     */
    CGContextRef context = CGBitmapContextCreate(data, width, height, 8, width * 4, CGImageGetColorSpace(cgimage), kCGImageAlphaPremultipliedLast);
    
    CGRect rect = CGRectMake(0, 0, width, height);
    
    // 纹理翻转-图片解压缩的时候翻转
////    CGContextTranslateCTM(context, rect.origin.x, rect.origin.y);
//    CGContextTranslateCTM(context, 0, rect.size.height);
//    CGContextScaleCTM(context, 1, -1);
////    CGContextTranslateCTM(context, -rect.origin.x, -rect.origin.y);
    
    CGContextDrawImage(context, rect, cgimage);
    
    CGContextRelease(context);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    float fw = width, fh = height;
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fw, fh, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    free(data);

    return 0;
}

// 绘制
- (void)renderLayer
{
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    GLfloat scale = [UIScreen mainScreen].scale;
    glViewport(self.frame.origin.x * scale, self.frame.origin.y * scale, self.frame.size.width * scale, self.frame.size.height * scale);
    
    NSString *vertexfile = [[NSBundle mainBundle] pathForResource:@"shaderv" ofType:@"vs"];
    NSString *fragfile = [[NSBundle mainBundle] pathForResource:@"shaderf" ofType:@"fs"];
    
    NSLog(@"vertfile: %@", vertexfile);
    NSLog(@"fragfile: %@", fragfile);
    
    self.program = [self loadShader:vertexfile fragPath:fragfile];
    
    glLinkProgram(self.program);
    GLint linkStatus;
    glGetProgramiv(self.program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE) {
        GLchar message[512];
        glGetProgramInfoLog(self.program, sizeof(message), 0, message);
        NSString *messageString = [NSString stringWithUTF8String:message];
        NSLog(@"Program Link Error: %@", messageString);
        return;
    }
    
    NSLog(@"Program Link Success!");
    
    glUseProgram(self.program);
    
    GLfloat vertexs[] =
    {
        0.5f, -0.5f, -1.0f,     1.0f, 0.0f,
        -0.5f, 0.5f, -1.0f,     0.0f, 1.0f,
        -0.5f, -0.5f, -1.0f,    0.0f, 0.0f,
        
        0.5f, 0.5f, -1.0f,      1.0f, 1.0f,
        -0.5f, 0.5f, -1.0f,     0.0f, 1.0f,
        0.5f, -0.5f, -1.0f,     1.0f, 0.0f,
    };
    
    GLuint attrBuffer;
    glGenBuffers(1, &attrBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, attrBuffer);
    // 把顶点数据从CPU内存复制到GPU上
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexs), vertexs, GL_STATIC_DRAW);
    
    GLuint position = glGetAttribLocation(self.program, "position");
    
    glEnableVertexAttribArray(position);
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, 0);
    
    GLuint textcoord = glGetAttribLocation(self.program, "textureCoord");
    glEnableVertexAttribArray(textcoord);
    glVertexAttribPointer(textcoord, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void *)(sizeof(GLfloat) * 3));
    
    // 加载纹理
    [self setupTexture:@"kunkun"];
    
    // 设置纹理采样器 sampler2D
    glUniform1i(glGetUniformLocation(self.program, "colorMap"), 0);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    // 从渲染缓存区显示到屏幕上
    [self.context presentRenderbuffer:GL_RENDERBUFFER];
}

- (GLuint)loadShader:(NSString *)vertPath fragPath:(NSString *)fragPath
{
    GLuint vertexShader, fragShader;
    
    GLuint program = glCreateProgram();
    
    [self compileShader:&vertexShader type:GL_VERTEX_SHADER file:vertPath];
    [self compileShader:&fragShader type:GL_FRAGMENT_SHADER file:fragPath];
    
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragShader);
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragShader);
    
    return program;
}

- (void)compileShader:(GLuint *)shader type:(GLenum)type file:(NSString *)file
{
    //1.读取文件路径字符串
    NSString *content = [NSString stringWithContentsOfFile:file encoding:NSUTF8StringEncoding error:nil];
    const GLchar * source = (GLchar *)[content UTF8String];
    
    //2.创建一个shader（根据type类型）
    *shader = glCreateShader(type);
    
    //3.将着色器源码附加到着色器对象上。
    //参数1：shader,要编译的着色器对象 *shader
    //参数2：numOfStrings,传递的源码字符串数量 1个
    //参数3：strings,着色器程序的源码（真正的着色器程序源码）
    //参数4：lenOfStrings,长度，具有每个字符串长度的数组，或NULL，这意味着字符串是NULL终止的
    glShaderSource(*shader, 1, &source, NULL);
    
    //4.把着色器源代码编译成目标代码
    glCompileShader(*shader);
}

@end
