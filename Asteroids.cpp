//
//  Asteroids.cpp
//  ASTEROIDS
//
//  Created by 'Nathema-Bee on 10/17/16.
//
//

#include "Asteroids.hpp"

void Asteroids::newAsteroid(int s) {
    asteroids.push_back(new Asteroid(height, width, s));
}

void Asteroids::draw(mat4 proj){
    float scaling = 100;
    mat4 astView = RotateZ(180)*Scale(scaling, scaling, scaling);
    
    glUseProgram( GLvars.program );
    glBindVertexArray( GLvars.vao );
    
    std::vector<Asteroid*>::iterator it = asteroids.begin();
    while (it != asteroids.end()) {
        vec2 currLoc = (*it)->state.location;
        int currAngle = (*it)->state.angle;
        int split = (*it)->state.splitting;
        mat4 currView = Scale((float)1/split, (float)1/split, 1)*astView;
        currView = RotateZ(currAngle)*currView;
        currView = Translate(currLoc.x, currLoc.y, 0)*currView;
        glUniformMatrix4fv( GLvars.M_location, 1, GL_TRUE, proj*currView);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 11);
        it++;
    }
    
    glBindVertexArray(0);
    glUseProgram(0);
    
}

void Asteroids::gl_init() {
    std::string vshader = shader_path + "vshader_Ship.glsl";
    std::string fshader = shader_path + "fshader_Ship.glsl";
    
    GLchar* vertex_shader_source = readShaderSource(vshader.c_str());
    GLchar* fragment_shader_source = readShaderSource(fshader.c_str());
    
    GLvars.vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(GLvars.vertex_shader, 1, (const GLchar**) &vertex_shader_source, NULL);
    glCompileShader(GLvars.vertex_shader);
    check_shader_compilation(vshader, GLvars.vertex_shader);
    
    GLvars.fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(GLvars.fragment_shader, 1, (const GLchar**) &fragment_shader_source, NULL);
    glCompileShader(GLvars.fragment_shader);
    check_shader_compilation(fshader, GLvars.fragment_shader);
    
    GLvars.program = glCreateProgram();
    glAttachShader(GLvars.program, GLvars.vertex_shader);
    glAttachShader(GLvars.program, GLvars.fragment_shader);
    
    glLinkProgram(GLvars.program);
    check_program_link(GLvars.program);
    
    glBindFragDataLocation(GLvars.program, 0, "fragColor");
    
    GLvars.vpos_location   = glGetAttribLocation(GLvars.program, "vPos");
    GLvars.vcolor_location = glGetAttribLocation(GLvars.program, "vColor" );
    GLvars.M_location = glGetUniformLocation(GLvars.program, "M" );
    
    // Create a vertex array object
    glGenVertexArrays( 1, &GLvars.vao );
    //Set GL state to use vertex array object
    glBindVertexArray( GLvars.vao );
    
    //Generate buffer to hold our vertex data
    glGenBuffers( 1, &GLvars.buffer );
    //Set GL state to use this buffer
    glBindBuffer( GL_ARRAY_BUFFER, GLvars.buffer );
    
    //Create GPU buffer to hold vertices and color
    glBufferData( GL_ARRAY_BUFFER, sizeof(asteroid_verts) + sizeof(asteroid_colors), NULL, GL_STATIC_DRAW );
    //First part of array holds vertices
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(asteroid_verts), asteroid_verts );
    //Second part of array hold colors (offset by sizeof(triangle))
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(asteroid_verts), sizeof(asteroid_colors), asteroid_colors );
    
    glEnableVertexAttribArray(GLvars.vpos_location);
    glEnableVertexAttribArray(GLvars.vcolor_location );
    
    glVertexAttribPointer( GLvars.vpos_location, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    glVertexAttribPointer( GLvars.vcolor_location, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(asteroid_verts)) );
    
    glBindVertexArray(0);

}

void Asteroids::update() {
    std::vector<Asteroid*>::iterator it = asteroids.begin();
    
    while (it != asteroids.end()){
        if ((*it)->outofbounds(width, height) || (*it)->state.splitting > 3 || (*it)->state.toDelete == true){
//            std::cout << "Asteroid at " << (*it)->state.location << ", which exceeds the bounds of " << width << ", " << height << "\n";
            //delete
            delete *it;
            it = asteroids.erase(it);
            
        } else {
            //asteroid is still valid
            (*it)->updateLoc();
            it++;
        }
    }
    
    
    //Create GPU buffer to hold vertices and color
    glBufferData( GL_ARRAY_BUFFER, 11*sizeof(vec2) + 11*sizeof(vec3), NULL, GL_STATIC_DRAW );
    //First part of array holds vertices
    glBufferSubData( GL_ARRAY_BUFFER, 0, 11*sizeof(vec2), asteroid_verts );
    //Second part of array hold colors (offset by sizeof(triangle))
    glBufferSubData( GL_ARRAY_BUFFER, 11*sizeof(vec2), 11*sizeof(vec3), asteroid_colors );
    
    glVertexAttribPointer( GLvars.vpos_location, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    glVertexAttribPointer( GLvars.vcolor_location, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(11*sizeof(vec2)) );
    
}

Asteroids::Asteroid::Asteroid(float width, float height, int s) {
    int side = (rand() % 4); //determines which edge the asteroid will be coming from
    //0 indicates height, 1 indicates width, 2 indicates -height, 3 indicates -width
    float initMagnitude = 20;//(rand() % 4);
    float initAngle = (rand() % 180); //each wall has exactly 180 degrees, but the range depends on which wall is selected
    float otherSide;
    
    switch (side) {
        case 0:
            otherSide = (rand() % (int)(2*width)) - width;
            state.location = vec2(height - 2, otherSide);
            initAngle = initAngle + 180;
            break;
        case 1:
            otherSide = (rand() % (int)(2*height)) - height;
            state.location = vec2(otherSide, width - 2);
            initAngle = initAngle + 90;
            break;
        case 2:
            otherSide = (rand() % (int)(2*width)) - width;
            state.location = vec2(-height + 2, otherSide);
            break;
        case 3:
            otherSide = (rand() % (int)(2*height)) - height;
            state.location = vec2(otherSide, -width + 2);
            initAngle = (int)(initAngle + 270) % 360;
            break;
    }
    
    
    initAngle = initAngle * 3.14159 / 180;
    state.splitting = s;
    state.angle = initAngle;
    state.velocity = normalize(vec2(sin(initAngle), cos(initAngle)))*initMagnitude;
//    std::cout << "Asteroid state: \n\tCase: " << side << "\n\tLocation: " << state.location << "\n\tVelocity: " << state.velocity << "\n\tAngle: " << state.angle << "\n\tSplit " << state.splitting << " times.\n";
}

void Asteroids::Asteroid::updateLoc() {
    state.location += 0.033*state.velocity;
}

void Asteroids::split(Angel::vec2 vel, Angel::vec2 loc, int s) {
//    std::cout << "Called split function\n";
    
    if (s > 2) { return; } //abort before we get too many asteroids

    float initAngle = (rand() % 360) * 3.14159 / 360;
    vec2 newVel = vel*2;
    int split = s + 1;
    vec2 newLoc = loc + newVel;
    
//    std::cout << "\tCurrent asteroids.size: " << asteroids.size() << "\n";
    
    Asteroid* newast = new Asteroid(height, width, split);
    newast->SetState(initAngle, newVel, newLoc, split);
    
//    std::cout << "newast State: " << newast->state.location << "; " << newast->state.velocity << "; " << newast->state.angle << "; " << newast->state.splitting << "\n";
    asteroids.push_back(newast);
    
//    std::cout << "\tCurrent asteroids.size: " << asteroids.size() << "\n";
    
    initAngle = initAngle*2;
    
    Asteroid* newerast = new Asteroid(height, width, split);
    newerast->SetState(initAngle, newVel, newLoc, split);
    
//    std::cout << "newerast State: " << newerast->state.location << "; " << newerast->state.velocity << "; " << newerast->state.angle << "; " << newerast->state.splitting << "\n";

    
    asteroids.push_back(newerast);
    
//    std::cout << "\tCurrent asteroids.size: " << asteroids.size() << "\n";
}


void Asteroids::Asteroid::SetState(float initAngle, Angel::vec2 vel, Angel::vec2 loc, int s){
        state.angle = initAngle;
        state.velocity = vel;
        state.location = loc;
        state.splitting = s;
}

void Asteroids::bulletsCollision(Bullets &bull) {
//    std::cout << "New call to bulletsCollision\n";
    std::vector<Asteroid*>::iterator astit = asteroids.begin();
    std::vector<Bullets::Bullet*>::iterator bullit = bull.bullets.begin();
    vec2 bullLoc, astLoc, astVel;
    int astSplit;
    float minX = 0.3 * 100;
    float maxX = 0.3 * 100;
    float maxY = 0.4 * 100;
    float minY = 0.25 * 100;
    
    int iter = asteroids.size();
//    std::cout << "Should iterate " << iter << "times through this call\n";
    
    for (int i = 0; i < iter; i++) {
        astLoc = (*astit)->state.location;
        astVel = (*astit)->state.velocity;
        astSplit = (*astit)->state.splitting;
        
//        std::cout << "Testing state assignments: \n\t\tLocation: " << astLoc << "\n\t\tVelocity: " << astVel << "\n\t\tSplitting: " << astSplit << "\n";
        
        while (bullit != bull.bullets.end()){
            bullLoc = (*bullit)->state.location;
            
//            std::cout << "Bullet Loc: " << bullLoc << "\n";
            
            if ((*bullit)->state.canShoot && bullLoc.x > (astLoc.x - (minX/astSplit)) && bullLoc.x < (astLoc.x + (maxX/astSplit)) && bullLoc.y > (astLoc.y - (minY/astSplit)) && bullLoc.y < (astLoc.y + (maxY/astSplit))){
//                std::cout << "Found a bullet within the box!\n";
                (*astit)->state.toDelete = true;
                split(astVel, astLoc, astSplit);
                (*bullit)->state.canShoot = false;
                delete *bullit;
                bullit = bull.bullets.erase(bullit);
                
                //todo: add way to keep bullets from hanging around long enough to delete new asteroids
                
            } else {
                bullit++;
            }
        }
        
        bullit = bull.bullets.begin();
        astit++;
    }
}

void Asteroids::reset() {
    std::vector<Asteroid*>::iterator it = asteroids.begin();
    while (it != asteroids.end()){
        delete *it;
        it = asteroids.erase(it);
    }
}

bool Asteroids::Asteroid::outofbounds(float width, float height) {
    if ((state.location.x > width || state.location.x < -width) && (width != 0)){
        return true;
    }
    if ((state.location.y > height || state.location.y < -height) && (height != 0)){
        return true;
    }
    return false;
}