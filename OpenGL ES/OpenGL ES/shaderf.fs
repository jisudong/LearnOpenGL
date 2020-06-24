precision highp float;
varying lowp vec2 varyTextureCoord;
uniform sampler2D colorMap;

void main()
{
    // 翻转纹理
    gl_FragColor = texture2D(colorMap, vec2(varyTextureCoord.x, 1.0 - varyTextureCoord.y));
    
//    gl_FragColor = texture2D(colorMap, varyTextureCoord);
}
