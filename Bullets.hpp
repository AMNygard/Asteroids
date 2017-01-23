//
//  Bullets.hpp
//  ASTEROIDS
//
//  Created by 'Nathema-Bee on 10/17/16.
//
//

#ifndef Bullets_hpp
#define Bullets_hpp

#include "common.h"

class Bullets{
public:
    
    //START OF ENCLOSED TYPE BULLET
    class Bullet{
    public:
        Bullet(vec2 loc, vec2 vel);
        
        Bullet();
        
        void SetState(vec2 loc, vec2 vel);
        
        void updateLoc();
        
        bool outofbounds(float width, float height);
        
        //Bullet State
        struct {
            vec2 location;
            vec2 velocity;
            bool canShoot = true;
        } state;
    };
    //END OF ENCLOSED TYPE BULLET
    
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
    
    Bullets(){}; //will never be called
    
    ~Bullets() {
        for (unsigned int i = 0; i < bullets.size(); i++){
            delete bullets[i];
        }
        bullets.clear();
    }
    
    vec2 bullet_verts[4] = {vec2(0.1, 0.1),
        vec2(0.1, -0.1),
        vec2(-0.1, -0.1),
        vec2(-0.1, 0.1),
    };
    
    vec3 bullet_colors[4] = {vec3(1.0, 1.0, 1.0),
        vec3(1.0, 1.0, 1.0),
        vec3(1.0, 1.0, 1.0),
        vec3(1.0, 1.0, 1.0)};
    
    std::vector <Bullet *> bullets;
    
    void bulletFired(vec2 loc, vec2 vel){
//        std::cout << "bulletFired(), bullets.size(): " << bullets.size() << "\n";
        bullets.push_back(new Bullet(loc, vel));
//        std::cout << "bulletsFired(), bullets.size(): " << bullets.size() << "\n";
    }
    
    void update();
    
    void gl_init();
    
    void draw(mat4 proj);
    
    void set_extents(int w, int h){
        width  = w/2;
        height = h/2;
    }
    
    void reset();
    
};

#endif /* Bullets_hpp */
