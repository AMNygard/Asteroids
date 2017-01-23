//
//  Ship.cpp
//  Asteroids
//
//  Created by Brian Summa on 6/5/15.
//
//

#include "common.h"
#include "Ship.hpp"
#include "..//Bullets.hpp"

float scaleFactor = 100;
mat4 modelView = RotateZ(180)*Scale(scaleFactor, scaleFactor, scaleFactor);

Ship::Ship(){
    state.location = vec2(0.0,0.0);
    state.dir = vec2(0.0,-1.0);
    state.vel = vec2(0.0,0.0);
    state.thruster = false;
    state.angle = 0.0;
};


void Ship::update_state(){
    
    modelView = Translate(-state.location.x, -state.location.y, 0)*modelView;
    
    state.vel = state.vel * _DAMPING;
    
    if(state.thruster){
        state.vel = state.vel + _ACC*state.dir;
    }
    
    if ((state.vel[0]*state.vel[0] + state.vel[1]*state.vel[1]) > _MAX_SPEED*_MAX_SPEED){
        state.vel = normalize(state.vel)*_MAX_SPEED;
    }
    
    state.location = state.location + 0.033 * state.vel;
    
    if(state.location.x < -width || state.location.x > width){
        state.location.x = -state.location.x;
    }
    if(state.location.y < -height ||state.location.y > height){
        state.location.y = -state.location.y;
    }
    
    modelView = Translate(state.location.x, state.location.y, 0)*modelView;
    
}

void Ship::rotateLeft()  {
    state.angle-=_ROT;
    state.dir =  RotateZ(state.angle) * vec2(0.0,-1.0);
    modelView = Translate(state.location.x, state.location.y, 0)*Angel::RotateZ(-_ROT)*Translate(-state.location.x, -state.location.y, 0)*modelView;
}

void Ship::rotateRight() {
    state.angle+=_ROT;
    state.dir =  RotateZ(state.angle) * vec2(0.0,-1.0);
    modelView = Translate(state.location.x, state.location.y, 0)*Angel::RotateZ(_ROT)*Translate(-state.location.x, -state.location.y, 0)*modelView;
}

void Ship::gl_init(){

    
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
    glBufferData( GL_ARRAY_BUFFER, sizeof(ship_vert) + sizeof(ship_color), NULL, GL_STATIC_DRAW );
    //First part of array holds vertices
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(ship_vert), ship_vert );
    //Second part of array hold colors (offset by sizeof(triangle))
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(ship_vert), sizeof(ship_color), ship_color );
    
    glEnableVertexAttribArray(GLvars.vpos_location);
    glEnableVertexAttribArray(GLvars.vcolor_location );
    
    glVertexAttribPointer( GLvars.vpos_location, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    glVertexAttribPointer( GLvars.vcolor_location, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(ship_vert)) );
    
    glBindVertexArray(0);
    
}

void Ship::fire(Bullets &bullets){
    bullets.bulletFired(state.location, state.vel);
}

bool Ship::astCollision(Asteroids &asteroids){
    std::vector<Asteroids::Asteroid*>::iterator astit = asteroids.asteroids.begin();
    vec2 astLoc, vertLoc;
    vec2 shipLoc = state.location;
    float minXAst = 0.3 * 100;
    float maxXAst = 0.3 * 100;
    float maxYAst = 0.4 * 100;
    float minYAst = 0.25 * 100;
    
    float angle = state.angle * 3.14159 / 360;
    vec2 newMinBounds = getMinMinBB(-10, 10, -35, 40, angle);
    vec2 newMaxBounds = getMaxMaxBB(-10, 10, -35, 40, angle);
    
//    std::cout << "Checking for asteroid collisions!\n";
    
    while (astit != asteroids.asteroids.end()){
        astLoc = (*astit)->state.location;
        minXAst = minXAst / (*astit)->state.splitting;
        maxXAst = maxXAst / (*astit)->state.splitting;
        maxYAst = maxYAst / (*astit)->state.splitting;
        minYAst = minYAst / (*astit)->state.splitting;
        
//        std::cout << "Ship Loc: " << shipLoc << " with bounds " << newMinBounds << " and " << newMaxBounds << "\n\tAst Loc: " << astLoc << "with bounds (" << minXAst << ", " << minYAst << ") and (" << maxXAst << ", " << maxYAst << ")\n";
        if (((shipLoc.x + newMinBounds.x > astLoc.x - minXAst && shipLoc.x + newMinBounds.x < astLoc.x + maxXAst) //left edge of ship bounding box within asteroid bounding box
            || (shipLoc.x + newMaxBounds.x > astLoc.x - minXAst && shipLoc.x + newMaxBounds.x < astLoc.x + maxXAst)) //right edge of ship bounding box within asteroid bounding box
            && ((shipLoc.y + newMinBounds.y > astLoc.y - minYAst && shipLoc.y + newMinBounds.y < astLoc.y + maxYAst) //bottom edge of ship bounding box within asteroid bounding box
            || (shipLoc.y + newMaxBounds.y > astLoc.y - minYAst && shipLoc.y + newMaxBounds.y < astLoc.y + maxYAst))) //top edge of ship bounding box within asteroid bounding box
            {
                return true;
        }
        astit++;
        minXAst = 0.3 * 100;
        maxXAst = 0.3 * 100;
        maxYAst = 0.4 * 100;
        minYAst = 0.25 * 100;
    }
    return false;
}

vec2 Ship::getMinMinBB(float minx, float maxx, float miny, float maxy, float theta) {
    vec2 mins;
    
    vec2 a = vec2(minx*cos(theta) + maxy*sin(theta), minx*sin(theta) + maxy*cos(theta));
    vec2 b = vec2(maxx*cos(theta) + maxy*sin(theta), maxx*sin(theta) + maxy*cos(theta));
    vec2 c = vec2(minx*cos(theta) + miny*sin(theta), minx*sin(theta) + miny*cos(theta));
    vec2 d = vec2(maxx*cos(theta) + miny*sin(theta), maxx*sin(theta) + miny*cos(theta));
    
    if (a.x <= b.x) {mins.x = a.x;} else {mins.x = b.x;}
    if (c.x <= mins.x) {mins.x = c.x;}
    if (d.x <= mins.x) {mins.x = d.x;}
    
    if (a.y <= b.y) {mins.y = a.y;} else {mins.y = b.y;}
    if (c.y <= mins.y) {mins.y = c.y;}
    if (d.y <= mins.y) {mins.y = d.y;}
    
    return mins;
}

vec2 Ship::getMaxMaxBB(float minx, float maxx, float miny, float maxy, float theta) {
    vec2 maxes;
    
    vec2 a = vec2(minx*cos(theta) + maxy*sin(theta), minx*sin(theta) + maxy*cos(theta));
    vec2 b = vec2(maxx*cos(theta) + maxy*sin(theta), maxx*sin(theta) + maxy*cos(theta));
    vec2 c = vec2(minx*cos(theta) + miny*sin(theta), minx*sin(theta) + miny*cos(theta));
    vec2 d = vec2(maxx*cos(theta) + miny*sin(theta), maxx*sin(theta) + miny*cos(theta));
    
    if (a.x >= b.x) {maxes.x = a.x;} else {maxes.x = b.x;}
    if (c.x >= maxes.x) {maxes.x = c.x;}
    if (d.x >= maxes.x) {maxes.x = d.x;}
    
    if (a.y >= b.y) {maxes.y = a.y;} else {maxes.y = b.y;}
    if (c.y >= maxes.y) {maxes.y = c.y;}
    if (d.y >= maxes.y) {maxes.y = d.y;}
    
    return maxes;
}

void Ship::reset() {
    state.location = vec2(0.0,0.0);
    state.dir = vec2(0.0,-1.0);
    state.vel = vec2(0.0,0.0);
    state.thruster = false;
    state.angle = 0.0;
    modelView = Angel::RotateZ(180)*Scale(scaleFactor, scaleFactor, scaleFactor);
}

void Ship::draw(mat4 proj){
    
    glUseProgram( GLvars.program );
    glBindVertexArray( GLvars.vao );
    
    glUniformMatrix4fv( GLvars.M_location, 1, GL_TRUE, proj*modelView );
    glDrawArrays(GL_LINE_LOOP, 0, 11);
    
    glBindVertexArray(0);
    glUseProgram(0);
    
}
