#ifndef _MODEL_H_
#define _MODEL_H_

#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iostream>




using std::vector;
using std::string;

struct Color {
    uint32_t R = 255, G = 255, B = 255;

    void shade(float lum);
};

struct Mat4x4 {
    float m[4][4] = {};

    void identityMatrix();
    void rotationXMatrix(float fTheta);
    void rotationYMatrix(float fTheta);
    void rotationZMatrix(float fTheta);
    void translationMatrix(float x, float y, float z);
    void projectionMatrix(float fNear, float fFar, float fFovDeg,
                          float fAspectRatio);
    
    void projectionMatrix(float c);
    

    Mat4x4 operator* (const Mat4x4& rhs);
    Mat4x4& operator*= (const Mat4x4& rhs);
};

struct Vec3d {
    float x, y, z, w;

    Vec3d() { x = y = z = 0; w = 1;}
    Vec3d(float a, float b, float c, float d = 1.0f) { x = a; y = b; z = c; w = d; }

    float length() const { return sqrtf(x*x + y*y + z*z); }
    Vec3d normal() {return *this/length();}
    void normilize();
    float dotProduct(const Vec3d& b);
    Vec3d crossProduct(const Vec3d& b);
    Vec3d schurProduct(const Vec3d& b);
    Vec3d multiplyByMat4x4(Mat4x4& m);

    Vec3d& operator+= (const Vec3d& rhs);
    Vec3d& operator-= (const Vec3d& rhs);
    Vec3d& operator*= (const float& rhs);
    Vec3d& operator/= (const float& rhs);
    Vec3d operator+ (const Vec3d& rhs);
    Vec3d operator- (const Vec3d& rhs);
    Vec3d operator* (const float& rhs);
    Vec3d operator/ (const float& rhs);


};

struct Triangle {
    Vec3d p[3];
    Color color= {255, 255, 255};

    Triangle multiplyEachByMat4x4(Mat4x4& m);
    void normilizeZ();
    void shiftX(float d);
    void shiftY(int d);
    void shiftZ(float d);
    void scaleTo(int width, int height);
};

struct Mesh {
    vector<Triangle> tris;

    bool loadObjFile(string sFilename);
    void normiliseCoordinates();
};

#endif