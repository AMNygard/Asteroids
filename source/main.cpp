#include "common.h"
#include "Ship.hpp"
#include "./../Bullets.hpp"
#include "./../Asteroids.hpp"

using namespace Angel;

Ship ship;
Bullets bullets;
Asteroids asteroids;
bool reset;

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT))
        ship.rotateLeft();
    if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT))
        ship.rotateRight();
    if (key == GLFW_KEY_SPACE){
        if(action == GLFW_PRESS){
            ship.start_thruster();
        }
        if(action == GLFW_RELEASE){
            ship.stop_thruster();
        }
    }
    if (key == GLFW_KEY_Z && action == GLFW_PRESS){
        ship.fire(bullets);
    }
}


void init(){
    
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint (GL_POINT_SMOOTH_HINT, GL_NICEST);
    ship.gl_init();
    bullets.gl_init();
    asteroids.gl_init();
}

void animate(){
    if(glfwGetTime() > 0.033){
        glfwSetTime(0.0);
        ship.update_state();
        bullets.update();
        asteroids.update();
        if (bullets.bullets.size() > 0 && asteroids.asteroids.size() > 0){
            asteroids.bulletsCollision(bullets);
        }
        reset = ship.astCollision(asteroids);
    }
}



int main(void)
{
    GLFWwindow* window;
    
    glfwSetErrorCallback(error_callback);
    
    if (!glfwInit())
        exit(EXIT_FAILURE);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    glfwWindowHint(GLFW_SAMPLES, 10);
    
    
    window = glfwCreateWindow(1024, 768, "Asteroids!", NULL, NULL);
    if (!window){
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    glfwSetKeyCallback(window, key_callback);
    
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);
    
    //srand(time(NULL));
    //TODO: uncomment this line to return randomness to the game!
    
    init();
    
    int count = 1;
    
    while (!glfwWindowShouldClose(window)){
        
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        
        ship.set_extents(width, height);
        bullets.set_extents(width, height);
        asteroids.set_extents(width, height);
        
        mat4 proj = Ortho2D(-width/2, width/2, height/2, -height/2);
        
        if (count % 100 == 0) {
            //generate an asteroid
            asteroids.newAsteroid(1);
            count = 1;
        }
        count++;
        
        
        animate();
        
        glClear(GL_COLOR_BUFFER_BIT);
        
        ship.draw(proj);
        bullets.draw(proj);
        asteroids.draw(proj);
        
        if (reset){
            ship.reset();
            bullets.reset();
            asteroids.reset();
            reset = false;
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        
    }
    
    glfwDestroyWindow(window);
    
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
