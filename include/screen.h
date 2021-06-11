#ifndef SCREEN_H_
#define SCREEN_H_

#include <SDL.h>
#include <memory>
#include <cmath>
#include <algorithm>
#include <limits>

#include "model.h"


using std::unique_ptr;
using std::make_unique;
using std::numeric_limits;



struct Screen {
    static const int WIDTH = 1600;
    static const int HEIGHT = 1000;

    unique_ptr<Uint32[]> m_buffer;
    unique_ptr<float[]> m_zBuffer;

    Screen();

    void clear();
    void setPixel(int x, int y, Color color);
    void drawLine(float x1, float y1, float x2, float y2, Color color);
    void drawHLine(float x1, float x2, float y, Color color);
    void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, Color color);
    void drawTriangle(Triangle& t);
    void drawTriangle(Triangle& t, Color color);

    void fillBottomFlatTriangle(float x1, float y1, float x2, float y2, float x3, float y3, Color color);
    void fillTopFlatTriangle(float x1, float y1, float x2, float y2, float x3, float y3, Color color);

    void fillTriangle(float x1, float y1, float x2, float y2, float x3, float y3, Color color);
    void fillTriangle(Triangle& t);
    void fillTriangleO(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, Color color);
    void fillTriangleOZ(int32_t x1, int32_t y1, int32_t z1,
                        int32_t x2, int32_t y2, int32_t z2,
                        int32_t x3, int32_t y3, int32_t z3, Color color);    
    

    void barycentric(Triangle& t, Vec3d& p, float& u, float& v, float& w);
    void boundBox(Triangle& t, int& xMin, int& xMax, int& yMin, int& yMax);
    void fillTriangleB(Triangle& t);



};


#endif