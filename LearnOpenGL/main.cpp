//
//  main.cpp
//  LearnOpenGL
//
//  Created by apple on 2019/8/2.
//  Copyright © 2019 apple. All rights reserved.
//

#include <iostream>
#include "triangles.hpp"
#include "shader_c.hpp"
#include "texture.hpp"
#include "transform.hpp"
#include "coordinate_systems.hpp"
#include "camera_c.hpp"
#include "colors.hpp"
#include "basic_lighting.hpp"
#include "materials.hpp"
#include "lighting_map.hpp"
#include "light_casters.hpp"
#include "multiple_light.hpp"
#include "model_loading.hpp"
#include "depth_testing.hpp"
#include "stencil_testing.hpp"
#include "blending.hpp"
#include "face_culling.hpp"
#include "framebuffers.hpp"
#include "cubemaps.hpp"
#include "advanced_glsl.hpp"
#include "geometry_shader.hpp"
#include "instancing.hpp"
#include "anti_aliasing.hpp"
#include "advanced_lighting.hpp"

int main(int argc, const char * argv[]) {
    
    // -------------------- 1.getting_started ---------------------
    // 1.1
//    drawTriangle();
//    drawRectangle();
//    practice1();
//    practice2();
//    practice3();
    
    // 1.3
//    inout();
    
    // 1.4
//    texture();
//    texture2();
    
    // 1.5
//    transform();
    
    // 1.6
//    coordinate();
//    more3D();
//    tenBox();
    
    // 1.7
//    func_camera();
    
    
    // ----------------------- 2.lighting ---------------------
    // 2.1
//    colors();
    
    // 2.2
//    lighting();
    
    // 2.3
//    materials();
    
    // 2.4
//    lightingMaps();
    
    // 2.5
//    parallel_light();
//    attenuation();
//    spotlight();
    
    // 2.6
//    multiple();
    
    
    // ---------------------- 3.model_loading ----------------------
//    model_load();
    
    
    // ---------------------- 4.advanced_opengl ----------------------
    // 4.1
//    depth_test();
    
    // 4.2
//    stencil();
    
    // 4.3
//    discard();
//    blend();
    
    // 4.4
//    facecull();
    
    // 4.5
//    framebuffer();
    
    // 4.6
//    skybox();
//    reflect();
//    refract();
    
    // 4.8
//    advancedGlsl();
    
    // 4.9
//    geometry();
//    explode();
//    normal_visualization();
    
    // 4.10
//    instanceArray();
//    planet();
    
    // 4.11
//    MSAA();
    
    
    // ---------------------- 5.advanced_lighting ----------------------
    // 5.1
    blinnPhong();
    
    return 0;
}
