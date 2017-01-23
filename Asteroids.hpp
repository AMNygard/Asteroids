//
//  Asteroids.hpp
//  ASTEROIDS
//
//  Created by 'Nathema-Bee on 10/17/16.
//
//

#ifndef Asteroids_hpp
#define Asteroids_hpp

#include "common.h"
#include "./../Bullets.hpp"

class Asteroids{
public:
    
    //START OF ENCLOSED TYPE ASTEROID
    class Asteroid{
    public:        
        Asteroid(float width, float height, int s);
        
        void SetState(float initAngle, vec2 vel, vec2 loc, int s);
        
        void updateLoc();
        
        bool outofbounds(float width, float height);
        
        //Asteroid State
        struct {
            vec2 location;
            vec2 velocity;
            int angle;
            int splitting;
            bool toDelete = false;
        } state;
    };
    //END OF ENCLOSED TYPE ASTEROID
    
    float width, height;
    
    //OpenGL vars
    struct {
        GLuint vao;
        GLuint program;
        GLuint buffer;
        GLuint vertex_shader, fragment_shader;
        GLint vpos_location, vcolor_location;
        GLint M_location;
    } GLvars;
    
    
    vec2 asteroid_verts[11] = {vec2(0.0, 0.0),
        vec2(0.1, 0.25),
        vec2(0.3, 0.0),
        vec2(0.2, -0.3),
        vec2(0.25, -0.4),
        vec2(-0.1, -0.25),
        vec2(-0.2, -0.1),
        vec2(-0.1, 0.1),
        vec2(-0.3, 0.1),
        vec2(-0.2, 0.2),
        vec2(-0.1, 0.2)
    };
    
    vec3 asteroid_colors[11] = {vec3(1.0, 1.0, 1.0),
        vec3(1.0, 1.0, 1.0),
        vec3(1.0, 1.0, 1.0),
        vec3(1.0, 1.0, 1.0),
        vec3(1.0, 1.0, 1.0),
        vec3(1.0, 1.0, 1.0),
        vec3(1.0, 1.0, 1.0),
        vec3(1.0, 1.0, 1.0),
        vec3(1.0, 1.0, 1.0),
        vec3(1.0, 1.0, 1.0),
        vec3(1.0, 1.0, 1.0)};
    
    Asteroids(){}; //will never be called
    
    ~Asteroids() {
        for (unsigned int i = 0; i < asteroids.size(); i++){
            delete asteroids[i];
        }
        asteroids.clear();
    }
    
    std::vector <Asteroid *> asteroids;
    
    void newAsteroid(int s);
    
    void split(Angel::vec2 vel, Angel::vec2 loc, int s);
        
    void update();
    
    void gl_init();
    
    void draw(mat4 proj);
    
    void set_extents(int w, int h){
        width  = w/2;
        height = h/2;
    }
    
    void bulletsCollision(Bullets &bull);
    
    void reset();
    
};


#endif /* Asteroids_hpp */
