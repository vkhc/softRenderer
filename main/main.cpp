#include <iostream>

#include "window.h"
#include "screen.h"
#include "model.h"

using std::cout;
using std::endl;


int main(int argc, char *argv[]) {

    Window window;
    window.init(); // Needs to be handled if  fails
    Screen screen;
    
    Vec3d vCamera = {0, 0, 0};
    Vec3d light = {0, 0, -1.0f};

    Mesh model;
    bool load = model.loadObjFile("models\\cow.obj");
    if (!load) {
        window.quit();
        cout << "could not load file" << endl;
        return 7;
    }
    model.normiliseCoordinates();


    float fTheta = 0;
    
    
    

    Mat4x4 projMat;
    projMat.projectionMatrix(0.1f, 1000.0f, 90.0f,
                            (float)Screen::HEIGHT / (float)Screen::WIDTH);


    Mat4x4 matRotZ, matRotX, matRotY;
    // fTheta = 0.0005f * SDL_GetTicks();

    float xTheta = 0.0f, yTheta = 0.0f, zTheta = 0.0f;

    int run = 1;  
    while (run) {
        screen.clear();
        

        matRotZ.rotationZMatrix(zTheta*0.1);
        matRotX.rotationXMatrix(xTheta*0.1);
        matRotY.rotationYMatrix(yTheta*0.1);

        Mat4x4 matTrans;
        matTrans.translationMatrix(0.0f, 0.0f, 3.0f);

        Mat4x4 matWorld;
        matWorld.identityMatrix();

        matWorld *= matRotY;
        matWorld *= matRotX;
        matWorld *= matRotZ;
        matWorld *= matTrans;
        

        for (Triangle& t : model.tris) {
            
            Triangle transformed = t.multiplyEachByMat4x4(matWorld);
            // if (transformed.p[0].z > 0) transformed.color = {255,0,0};
            


            Vec3d one = transformed.p[1] - transformed.p[0];
            Vec3d two = transformed.p[2] - transformed.p[0];
            Vec3d normal = one.crossProduct(two);
            normal.normilize();
            
    
            float intensity = normal.dotProduct(light);
            
            
            if (intensity > 0) {
            // if (normal.dotProduct(transformed.p[0] - vCamera) < 0.0f) {


                Triangle tr;
                tr = transformed;
                

                tr = transformed.multiplyEachByMat4x4(projMat);
                tr.normilizeZ();
                
                
                tr.color.shade(intensity);


                tr.scaleTo(Screen::WIDTH, Screen::HEIGHT);
                tr.shiftX(Screen::WIDTH / 2);
                tr.shiftY(Screen::HEIGHT / 2.5);
                
                
                screen.fillTriangleB(tr);
                // screen.drawTriangle(tr, {255,0,0});

            }

        }

        run = window.handleEvents();
        window.update(screen);

        if (window.m_keys[SDL_SCANCODE_RIGHT]) yTheta++;
        if (window.m_keys[SDL_SCANCODE_LEFT]) yTheta--;

        if (window.m_keys[SDL_SCANCODE_UP]) xTheta++;
        if (window.m_keys[SDL_SCANCODE_DOWN]) xTheta--;

    
    }

    window.quit();
    return 0;
}
