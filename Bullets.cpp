//
//  Bullets.cpp
//  ASTEROIDS
//
//  Created by 'Nathema-Bee on 10/17/16.
//
//

#include "Bullets.hpp"

//BULLET METHODS
Bullets::Bullet::Bullet(){
    state.location = vec2(0.0, 0.0);
}

bool Bullets::Bullet::outofbounds(float width, float height){
    if ((state.location.x >  width || state.location.x < -width) && (width != 0)){
        return true;
    }
    if ((state.location.y > height || state.location.y < -height) && (height != 0)){
        return true;
    }
    return false;
}

void Bullets::Bullet::updateLoc(){
    state.location += 0.033*state.velocity;
}

void Bullets::Bullet::SetState(Angel::vec2 loc, Angel::vec2 vel) {
    state.location = loc;
    state.velocity = 2*vel + vec2(1.0, 1.0);
}

Bullets::Bullet::Bullet(vec2 loc, vec2 vel){
    state.location = loc;
    state.velocity = 2*vel + vec2(1.0, 1.0);
    
//    std::cout << "Fired bullet from " << state.location << " at speed " << state.velocity << "\n";
}


//BULLETS METHODS
void Bullets::draw(Angel::mat4 proj){
    float scaling = 15;
    mat4 bulView = Scale(scaling, scaling, scaling);
    
    glUseProgram( GLvars.program );
    glBindVertexArray( GLvars.vao );
    
    std::vector<Bullet*>::iterator it = bullets.begin();
    while (it != bullets.end()) {
        vec2 currLoc = (*it)->state.location;
        mat4 currView = Translate(currLoc.x, currLoc.y, 0)*bulView;
        glUniformMatrix4fv( GLvars.M_location, 1, GL_TRUE, proj*currView);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        it++;
    }
    
    glBindVertexArray(0);
    glUseProgram(0);
}

void Bullets::gl_init(){
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
    glBufferData( GL_ARRAY_BUFFER, sizeof(bullet_verts) + sizeof(bullet_colors), NULL, GL_STATIC_DRAW );
    //First part of array holds vertices
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(bullet_verts), bullet_verts );
    //Second part of array hold colors (offset by sizeof(triangle))
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(bullet_verts), sizeof(bullet_colors), bullet_colors );
    
    glEnableVertexAttribArray(GLvars.vpos_location);
    glEnableVertexAttribArray(GLvars.vcolor_location );
    
    glVertexAttribPointer( GLvars.vpos_location, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    glVertexAttribPointer( GLvars.vcolor_location, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(bullet_verts)) );
    
    glBindVertexArray(0);
}

void Bullets::reset() {
    std::vector<Bullet*>::iterator it = bullets.begin();
    while (it != bullets.end()){
        delete *it;
        it = bullets.erase(it);
    }
}

void Bullets::update() {
    std::vector<Bullet*>::iterator it = bullets.begin();
    
    while (it != bullets.end()){
        if ((*it)->outofbounds(width, height)){
            //delete bullet, it is out of bounds
//            std::cout << "Deleting bullet, it is at " << (*it)->state.location << " which exceeds the bounds of " << width << ", " << height << "\n";
            delete *it;
            it = bullets.erase(it);
        } else {
            //bullet still valid
            (*it)->updateLoc();
            it++;
        }
    }
    
    vec2 *locs = new vec2[bullets.size()];
    vec3 *color = new vec3[bullets.size()];
    
    for (int i = 0; i < bullets.size(); i++){
        locs[i] = bullets[i]->state.location;
        color[i] = vec3(0.0, 1.0, 0.0);
    }
    
            //Create GPU buffer to hold vertices and color
    glBufferData( GL_ARRAY_BUFFER, bullets.size()*sizeof(vec2) + bullets.size() *sizeof(vec3), NULL, GL_STATIC_DRAW );
            //First part of array holds vertices
    glBufferSubData( GL_ARRAY_BUFFER, 0, bullets.size()*sizeof(vec2), locs );
            //Second part of array hold colors (offset by sizeof(triangle))
    glBufferSubData( GL_ARRAY_BUFFER, bullets.size()*sizeof(vec2), bullets.size()*sizeof(vec3), color );
    
    glVertexAttribPointer( GLvars.vpos_location, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    glVertexAttribPointer( GLvars.vcolor_location, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(bullets.size()*sizeof(vec2)) );
    
    delete[] locs;
    delete[] color;
    
}