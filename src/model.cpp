#include "model.h"

#include <limits>

void Color::shade(float lum) {
    R = static_cast<uint32_t>(lum * R);
    G = static_cast<uint32_t>(lum * G);
    B = static_cast<uint32_t>(lum * B);
}

void Mat4x4::identityMatrix() {
    m[0][0] = 1;
    m[1][1] = 1;
    m[2][2] = 1;
    m[3][3] = 1;
}
void Mat4x4::rotationXMatrix(float fTheta) {
    m[0][0] = 1;
    m[1][1] = cosf(fTheta);
    m[1][2] = sinf(fTheta);
    m[2][1] = -sinf(fTheta);
    m[2][2] = cosf(fTheta);
    m[3][3] = 1;
}
void Mat4x4::rotationYMatrix(float fTheta) {
    m[0][0] = cosf(fTheta);
    m[0][2] = sinf(fTheta);
    m[2][0] = -sinf(fTheta);
    m[1][1] = 1;
    m[2][2] = cosf(fTheta);
    m[3][3] = 1;
}
void Mat4x4::rotationZMatrix(float fTheta) {
    m[0][0] = cosf(fTheta);
    m[0][1] = sinf(fTheta);
    m[1][0] = -sinf(fTheta);
    m[1][1] = cosf(fTheta);
    m[2][2] = 1;
    m[3][3] = 1;
}
void Mat4x4::translationMatrix(float x, float y, float z) {
    m[0][0] = 1.0f;
    m[1][1] = 1.0f;
    m[2][2] = 1.0f;
    m[3][3] = 1.0f;
    m[3][0] = x;
    m[3][1] = y;
    m[3][2] = z;
}
void Mat4x4::projectionMatrix(float fNear, float fFar, float fFovDeg,
                              float fAspectRatio) {

    float fFovRad = 1.0f / tanf(fFovDeg * 0.5f / 180.0f * 3.14159f);

    m[0][0] = fAspectRatio * fFovRad;
    m[1][1] = fFovRad;
    m[2][2] = fFar / (fFar - fNear);
    m[3][2] = (-fFar * fNear) / (fFar - fNear);
    m[2][3] = 1.0f;
    m[3][3] = 0.0f;
}
void Mat4x4::projectionMatrix(float c) {
    m[0][0] = 1.0f;
    m[1][1] = 1.0f;
    m[2][2] = 1.0f;
    m[3][2] = -1.0f / c;
    m[3][3] = 1.0f;
}

Mat4x4 Mat4x4::operator* (const Mat4x4& rhs) {
    Mat4x4 result;
    for (int c=0; c<4; ++c) {
        for (int r=0; r<4; ++r) {
            result.m[r][c] = m[r][0] * rhs.m[0][c] +
                             m[r][1] * rhs.m[1][c] +
                             m[r][2] * rhs.m[2][c] +
                             m[r][3] * rhs.m[3][c];
        }
    }
    return result;
}

Mat4x4& Mat4x4::operator*= (const Mat4x4& rhs) {
    Mat4x4 result;
    for (int c=0; c<4; ++c) {
        for (int r=0; r<4; ++r) {
            result.m[r][c] = m[r][0] * rhs.m[0][c] +
                             m[r][1] * rhs.m[1][c] +
                             m[r][2] * rhs.m[2][c] +
                             m[r][3] * rhs.m[3][c];
        }
    }
    *this = result;
    return *this;
}


bool Mesh::loadObjFile(string sFileName) {
    std::ifstream f(sFileName);
    if (!f.is_open()) {
        std::cout << "Could not open file" << std::endl;
        return false;
    }

    // Local cache of vertices
    vector<Vec3d> vertices;

    while(!f.eof()) {
        char line[128];
        f.getline(line, 128);

        std::stringstream s;
        s << line;

        char junk;
        if (line[0] == 'v') {
            Vec3d v;
            s >> junk >> v.x >> v.y >> v.z;
            vertices.push_back(v);
        }

        if (line[0] == 'f') {
            int f[3];
            s >> junk >> f[0] >> f[1] >> f[2];
            tris.push_back({vertices[f[0] - 1],
                            vertices[f[1] - 1],
                            vertices[f[2] - 1]});
        }
    }
    f.close();
    return true;
}

void Mesh::normiliseCoordinates() {
    float max = std::numeric_limits<float>::min();
    float min = std::numeric_limits<float>::max();
    for (auto& t : tris) {
        for (int i=0; i<3; ++i) {
            if (t.p[i].x > max) max = t.p[i].x;
            if (t.p[i].x < min) min = t.p[i].x;
            if (t.p[i].y > max) max = t.p[i].y;
            if (t.p[i].y < min) min = t.p[i].y;
            if (t.p[i].z > max) max = t.p[i].z;
            if (t.p[i].z < min) min = t.p[i].z;
        }
    }

    if (max < 0) max *= -1.0f;
    if (min < 0) min *= -1.0f;
    float val = max > min ? max : min;

    for (auto& t : tris) {
        for (int i=0; i<3; ++i) {
            t.p[i].x /= val;
            t.p[i].y /= val;
            t.p[i].z /= val;
        }
    }
}

void Vec3d::normilize() {
    float l = length();
    x /= l;
    y /= l;
    z /= l;
}

float Vec3d::dotProduct(const Vec3d& b) {
    return x*b.x + y*b.y + z*b.z;
}

Vec3d Vec3d::crossProduct(const Vec3d& b) {
    return Vec3d(y * b.z - z * b.y,
                 z * b.x - x * b.z,
                 x * b.y - y * b.x);
}

Vec3d Vec3d::schurProduct(const Vec3d& b) {
    return Vec3d(x*b.x, y*b.y, z*b.z);
}

Vec3d Vec3d::multiplyByMat4x4(Mat4x4& m) {
    Vec3d result;
    result.x = x * m.m[0][0] + y * m.m[1][0] + z * m.m[2][0] + w * m.m[3][0];
    result.y = x * m.m[0][1] + y * m.m[1][1] + z * m.m[2][1] + w * m.m[3][1];
    result.z = x * m.m[0][2] + y * m.m[1][2] + z * m.m[2][2] + w * m.m[3][2];
    result.w = x * m.m[0][3] + y * m.m[1][3] + z * m.m[2][3] + w * m.m[3][3];
    return result;
}

Vec3d& Vec3d::operator+= (const Vec3d& rhs) {
    this->x += rhs.x;
    this->y += rhs.y;
    this->z += rhs.z;
    return *this;
}

Vec3d& Vec3d::operator-= (const Vec3d& rhs) {
    this->x -= rhs.x;
    this->y -= rhs.y;
    this->z -= rhs.z;
    return *this;
}

Vec3d& Vec3d::operator*= (const float& rhs) {
    this->x *= rhs;
    this->y *= rhs;
    this->z *= rhs;
    return *this;
}

Vec3d& Vec3d::operator/= (const float& rhs) {
    this->x /= rhs;
    this->y /= rhs;
    this->z /= rhs;
    return *this;
}

Vec3d Vec3d::operator+ (const Vec3d& rhs) {
    return Vec3d(x + rhs.x, y + rhs.y, z + rhs.z);
}

Vec3d Vec3d::operator- (const Vec3d& rhs) {
    return Vec3d(x - rhs.x, y - rhs.y, z - rhs.z);
}

Vec3d Vec3d::operator* (const float& rhs) {
    return Vec3d(x * rhs, y * rhs, z * rhs);
}

Vec3d Vec3d::operator/ (const float& rhs) {
    return Vec3d(x / rhs, y / rhs, z / rhs);
}


Triangle Triangle::multiplyEachByMat4x4(Mat4x4& m) {
    Triangle result;
    result.p[0] = p[0].multiplyByMat4x4(m);
    result.p[1] = p[1].multiplyByMat4x4(m);
    result.p[2] = p[2].multiplyByMat4x4(m);
    return result;
}

void Triangle::normilizeZ() {
    p[0].x /= p[0].w; p[0].y /= p[0].w; p[0].z /= p[0].w;
    p[1].x /= p[1].w; p[1].y /= p[1].w; p[1].z /= p[1].w;
    p[2].x /= p[2].w; p[2].y /= p[2].w; p[2].z /= p[2].w;
}

void Triangle::shiftX(float d) {
    p[0].x += d; 
    p[1].x += d; 
    p[2].x += d; 
}
void Triangle::shiftY(int d) {
    p[0].y += d;
    p[1].y += d;
    p[2].y += d;
}
void Triangle::shiftZ(float d) {
    p[0].z += d;
    p[1].z += d;
    p[2].z += d;
}

void Triangle::scaleTo(int width, int height) {
    p[0].x *= /* 0.5f */ width;
    p[0].y *= /* 0.5f */  height;
    p[1].x *= /* 0.5f */ width;
    p[1].y *= /* 0.5f */  height;
    p[2].x *= /* 0.5f */ width;
    p[2].y *= /* 0.5f */  height;

}












// void multiplyMatrixVector(Vec3d& i, Vec3d& o, Mat4x4& m) {
//     o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
//     o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
//     o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
//     float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

//     if (w != 0.0f) {
//         o.x /= w; o.y /= w; o.z /= w;
//     }
// }