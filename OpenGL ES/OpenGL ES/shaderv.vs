attribute vec4 position;
attribute vec2 textureCoord;
varying lowp vec2 varyTextureCoord;

void main()
{
    
    
    varyTextureCoord = textureCoord;
    // 翻转纹理
//    varyTextureCoord = vec2(textureCoord.x, 1.0 - textureCoord.y);
    gl_Position = position;
}
