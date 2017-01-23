//
//  Ship.h
//  Asteroids
//
//  Created by Brian Summa on 6/5/15.
//


#ifndef __Asteroids__Ship__
#define __Asteroids__Ship__

#include "common.h"
#include "./../Bullets.hpp"
#include "./../Asteroids.hpp"

#define _MAX_SPEED 100
#define _DAMPING 0.98
#define _ACC 5
#define _ROT 15
#define _DT = 0.033;

class Ship{
    
    vec2 ship_vert[11] = { vec2(0.0, 0.4),
        vec2(0.05, 0.2),
        vec2(0.05, 0.0),  //wing
        vec2(0.1, -0.1), //points
        vec2(0.1, -0.35), //here
        vec2(0.05, -0.25),
        vec2(-0.05, -0.25),
        vec2(-0.1, -0.35), //here
        vec2(-0.1, -0.1), //points
        vec2(-0.05, 0.0),  //wing
        vec2(-0.05, 0.2)};
    
    vec3 ship_color[11] = {vec3(0.0, 1.0, 0.0),
        vec3(0.0, 1.0, 0.0),
        vec3(0.0, 1.0, 0.0),
        vec3(0.0, 1.0, 0.0),
        vec3(0.0, 1.0, 0.0),
        vec3(0.0, 1.0, 0.0),
        vec3(0.0, 1.0, 0.0),
        vec3(0.0, 1.0, 0.0),
        vec3(0.0, 1.0, 0.0),
        vec3(0.0, 1.0, 0.0),
        vec3(0.0, 1.0, 0.0)};
    
    float width, height;
    
    
    //Ship State
    struct {
        vec2 location;
        vec2 vel;
        vec2 dir;
        bool thruster;
        float angle;
    } state;
    
    //OpenGL vars
    struct {
        GLuint vao;
        GLuint program;
        GLuint buffer;
        GLuint vertex_shader, fragment_shader;
        GLint vpos_location, vcolor_location;
        GLint M_location;
    } GLvars;
    
    
    mat2 RotateZ( const GLfloat theta ){
        GLfloat angle = DegreesToRadians * theta;
        mat2 c;
        c[0][0] = c[1][1] = cos(angle);
        c[1][0] = sin(angle);
        c[0][1] = -c[1][0];
        return c;
    }
    
public:
    
    
    Ship();
    
    inline void start_thruster(){ state.thruster= true;}
    inline void stop_thruster() { state.thruster= false;}
        
    void rotateLeft();
    void rotateRight();
    
    void update_state();
    
    void gl_init();
    
    void draw(mat4 proj);
    
    void fire(Bullets &bullets);
    
    bool astCollision(Asteroids &asteroids);
    
    vec2 getMinMinBB(float minx, float maxx, float miny, float maxy, float theta);
    
    vec2 getMaxMaxBB(float minx, float maxx, float miny, float maxy, float theta);
    
    void set_extents(int w, int h){
        width  = w/2;
        height = h/2;
    }
    
    void reset();
};


#endif /* defined(__Asteroids__Ship__) */