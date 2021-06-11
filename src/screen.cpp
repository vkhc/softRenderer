#include "screen.h"


Screen::Screen() {
	int size = WIDTH * HEIGHT;
	m_buffer = make_unique<Uint32[]>(size);
	m_zBuffer = make_unique<float[]>(size);
	for (int i=0; i<size; ++i) {
        m_zBuffer[i] = numeric_limits<float>::max();
    }

}

void Screen::clear() {
    memset(m_buffer.get(), 0, WIDTH*HEIGHT*sizeof(Uint32));
	// reset Z buffer
	int size = WIDTH * HEIGHT;
	for (int i=0; i<size; ++i) {
        m_zBuffer[i] = numeric_limits<float>::max();
    }
}

void Screen::setPixel(int x, int y, Color color) {
    if (x<0 || x >= WIDTH || y<0 || y>=HEIGHT) return;
    Uint32& pixel = m_buffer[WIDTH * (HEIGHT - y - 1) + x];
    pixel = 0; //
    pixel += color.R << 24;
    pixel += color.G << 16;
    pixel += color.B << 8;
}

// Bresenham's line algorithm
void Screen::drawLine(float x1, float y1, float x2, float y2, Color color) {
    const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
    if(steep) {
        std::swap(x1, y1);
        std::swap(x2, y2);
    }

    if(x1 > x2) {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }

    const float dx = x2 - x1;
    const float dy = fabs(y2 - y1);

    float error = dx / 2.0f;
    const int ystep = (y1 < y2) ? 1 : -1;
    int y = (int)y1;

    const int maxX = (int)x2;

    for(int x=(int)x1; x<=maxX; x++) {
        if(steep) {
            setPixel(y, x, color);
        } else {
            setPixel(x, y, color);
        }

        error -= dy;
        if(error < 0) {
            y += ystep;
            error += dx;
        }
    }    
}

void Screen::drawHLine(float x1, float x2, float y, Color color) {
    if (x1 > x2) {
    	for (int i=x2; i<=x1; ++i) setPixel(i, y, color);
	} else {
		for (int i=x1; i<=x2; ++i) setPixel(i, y, color);
	}
}

void Screen::drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, Color color) {
    drawLine(x1, y1, x2, y2, color);
    drawLine(x1, y1, x3, y3, color);
    drawLine(x2, y2, x3, y3, color);
}

void Screen::drawTriangle(Triangle& t) {
	drawLine(t.p[0].x, t.p[0].y, t.p[1].x, t.p[1].y, t.color);
	drawLine(t.p[0].x, t.p[0].y, t.p[2].x, t.p[2].y, t.color);
	drawLine(t.p[1].x, t.p[1].y, t.p[2].x, t.p[2].y, t.color);
}

void Screen::drawTriangle(Triangle& t, Color color) {
	drawLine(t.p[0].x, t.p[0].y, t.p[1].x, t.p[1].y, color);
	drawLine(t.p[0].x, t.p[0].y, t.p[2].x, t.p[2].y, color);
	drawLine(t.p[1].x, t.p[1].y, t.p[2].x, t.p[2].y, color);
}


void Screen::fillBottomFlatTriangle(float x1, float y1, float x2, float y2, float x3, float y3, Color color) {
    // y1 < y2 < y3

    float invslope1 = (x2 - x1) / (y2 - y1);
    float invslope2 = (x3 - x1) / (y3 - y1);

    float curx1 = x2;
    float curx2 = x3;

    // When iterating from top to bottom horizontal lines span outside triangles
    // when triangles become very narrow.
    // The same is not the case for fillTopFlatTriangle
    for (int scanLineY = y3; scanLineY > y1; --scanLineY) {
        drawHLine(curx1, curx2, scanLineY, color);
        curx1 -= invslope1;
        curx2 -= invslope2;
    }
}

void Screen::fillTopFlatTriangle(float x1, float y1, float x2, float y2, float x3, float y3, Color color) {
    float invslope1 = (x3 - x1) / (y3 - y1);
    float invslope2 = (x3 - x2) / (y3 - y2);

    float curx1 = x3;
    float curx2 = x3;

    for (int scanLineY = y3; scanLineY > y1; --scanLineY) {
        drawHLine(curx1, curx2, scanLineY, color);
        curx1 -= invslope1;
        curx2 -= invslope2;
    }
}

void Screen::fillTriangle(float x1, float y1, float x2, float y2, float x3, float y3, Color color) {
    // sort points so that y1 <= y2 <= y3
    if (y1 > y2) {std::swap(y1, y2); std::swap(x1, x2);}
    if (y2 > y3) {std::swap(y2, y3); std::swap(x2, x3);}
    if (y1 > y2) {std::swap(y1, y2); std::swap(x1, x2);}

    if (y2 == y3) fillBottomFlatTriangle(x1, y1, x2, y2, x3, y3, color);
    else if (y1 == y2) fillTopFlatTriangle(x1, y1, x2, y2, x3, y3, color);
    else {
        // float slope = (x3 - x1) / (y3 - y1);
        // float x4 = slope * (y2 - y1) + x1;
        float x4 = (x1 + ((y2 -y1) / (y3 - y1)) * (x3 - x1));
        float y4 = y2;
        fillTopFlatTriangle(x2, y2, x4, y4, x3, y3, color);
        fillBottomFlatTriangle(x1, y1, x2, y2, x4, y4, color);
        
    }
}

// Source : https://github.com/OneLoneCoder/olcPixelGameEngine/blob/master/olcPixelGameEngine.h
void Screen::fillTriangleO(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, Color color) {

	int t1x, t2x, y, minx, maxx, t1xp, t2xp;
	bool changed1 = false;
	bool changed2 = false;
	int signx1, signx2, dx1, dy1, dx2, dy2;
	int e1, e2;
	// Sort vertices
	if (y1 > y2) { std::swap(y1, y2); std::swap(x1, x2); }
	if (y1 > y3) { std::swap(y1, y3); std::swap(x1, x3); }
	if (y2 > y3) { std::swap(y2, y3); std::swap(x2, x3); }

	t1x = t2x = x1; y = y1;   // Starting points
	dx1 = (int)(x2 - x1);
	if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
	else signx1 = 1;
	dy1 = (int)(y2 - y1);

	dx2 = (int)(x3 - x1);
	if (dx2 < 0) { dx2 = -dx2; signx2 = -1; }
	else signx2 = 1;
	dy2 = (int)(y3 - y1);

	if (dy1 > dx1) { std::swap(dx1, dy1); changed1 = true; }
	if (dy2 > dx2) { std::swap(dy2, dx2); changed2 = true; }

	e2 = (int)(dx2 >> 1);
	// Flat top, just process the second half
	if (y1 == y2) goto next;
	e1 = (int)(dx1 >> 1);

	for (int i = 0; i < dx1;) {
		t1xp = 0; t2xp = 0;
		if (t1x < t2x) { minx = t1x; maxx = t2x; }
		else { minx = t2x; maxx = t1x; }
		// process first line until y value is about to change
		while (i < dx1) {
			i++;
			e1 += dy1;
			while (e1 >= dx1) {
				e1 -= dx1;
				if (changed1) t1xp = signx1;//t1x += signx1;
				else          goto next1;
			}
			if (changed1) break;
			else t1x += signx1;
		}
		// Move line
	next1:
		// process second line until y value is about to change
		while (1) {
			e2 += dy2;
			while (e2 >= dx2) {
				e2 -= dx2;
				if (changed2) t2xp = signx2;//t2x += signx2;
				else          goto next2;
			}
			if (changed2)     break;
			else              t2x += signx2;
		}
	next2:
		if (minx > t1x) minx = t1x;
		if (minx > t2x) minx = t2x;
		if (maxx < t1x) maxx = t1x;
		if (maxx < t2x) maxx = t2x;
		drawHLine(minx, maxx, y, color);    // Draw line from min to max points found on the y
		// Now increase y
		if (!changed1) t1x += signx1;
		t1x += t1xp;
		if (!changed2) t2x += signx2;
		t2x += t2xp;
		y += 1;
		if (y == y2) break;
	}
next:
	// Second half
	dx1 = (int)(x3 - x2); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
	else signx1 = 1;
	dy1 = (int)(y3 - y2);
	t1x = x2;

	if (dy1 > dx1) {   // swap values
		std::swap(dy1, dx1);
		changed1 = true;
	}
	else changed1 = false;

	e1 = (int)(dx1 >> 1);

	for (int i = 0; i <= dx1; i++) {
		t1xp = 0; t2xp = 0;
		if (t1x < t2x) { minx = t1x; maxx = t2x; }
		else { minx = t2x; maxx = t1x; }
		// process first line until y value is about to change
		while (i < dx1) {
			e1 += dy1;
			while (e1 >= dx1) {
				e1 -= dx1;
				if (changed1) { t1xp = signx1; break; }//t1x += signx1;
				else          goto next3;
			}
			if (changed1) break;
			else   	   	  t1x += signx1;
			if (i < dx1) i++;
		}
	next3:
		// process second line until y value is about to change
		while (t2x != x3) {
			e2 += dy2;
			while (e2 >= dx2) {
				e2 -= dx2;
				if (changed2) t2xp = signx2;
				else          goto next4;
			}
			if (changed2)     break;
			else              t2x += signx2;
		}
	next4:

		if (minx > t1x) minx = t1x;
		if (minx > t2x) minx = t2x;
		if (maxx < t1x) maxx = t1x;
		if (maxx < t2x) maxx = t2x;
		drawHLine(minx, maxx, y, color);
		if (!changed1) t1x += signx1;
		t1x += t1xp;
		if (!changed2) t2x += signx2;
		t2x += t2xp;
		y += 1;
		if (y > y3) return;
	}
}

void Screen::fillTriangle(Triangle& t) {
	fillTriangleO(t.p[0].x, t.p[0].y,
                  t.p[1].x, t.p[1].y,
                  t.p[2].x, t.p[2].y, t.color);
}



void Screen::barycentric(Triangle& t, Vec3d& p, float& u, float& v, float& w) {
	Vec3d v0 = t.p[1] - t.p[0], v1 = t.p[2] - t.p[0], v2 = p - t.p[0];
	// float d00 = v0.dotProduct(v0);
	// float d01 = v0.dotProduct(v1);
	// float d11 = v1.dotProduct(v1);
	// float d20 = v2.dotProduct(v0);
	// float d21 = v2.dotProduct(v1);
	// float denom = d00 * d11 - d01 * d01;
	// v = (d11 * d20 - d01 * d21) / denom;
	// w = (d00 * d21 - d01 * d20) / denom;
	// u = 1.0f - v - w;
		// Ignore Z dimension
	float den = 1.0f / (v0.x * v1.y - v1.x * v0.y);
	v = (v2.x * v1.y - v1.x * v2.y) * den;
	w = (v0.x * v2.y - v2.x * v0.y) * den;
	u = 1.0f - v - w;
	// Ceck for 0 division
}

void Screen::boundBox(Triangle& t, int& xMin, int& xMax, int& yMin, int& yMax) {
	xMin = std::floor(std::min(std::min(t.p[0].x, t.p[1].x), t.p[2].x));
	xMax = std::ceil(std::max(std::max(t.p[0].x, t.p[1].x), t.p[2].x));
	yMin = std::floor(std::min(std::min(t.p[0].y, t.p[1].y), t.p[2].y));
	yMax = std::ceil(std::max(std::max(t.p[0].y, t.p[1].y), t.p[2].y));
	if (xMax >= WIDTH) xMax = WIDTH-1;
	if (xMin < 0) xMin = 0;
	if (yMax >= HEIGHT) yMax = HEIGHT-1;
	if (yMin < 0) yMin = 0;
}


void Screen::fillTriangleB(Triangle& t) {
	float u, v, w; // Barycentric coordinates
	int xMin, xMax, yMin, yMax; // bounding box of a triangle
	boundBox(t, xMin, xMax, yMin, yMax);
	for (int i=xMin; i<xMax; ++i) {
		for (int j=yMin; j<yMax; ++j) {
			Vec3d p = {(float)i, (float)j, 0}; // Need int vec here
			barycentric(t, p, u, v, w);
			if (u < 0 || v < 0 || w < 0) continue;
			
			p.z = u * t.p[0].z + v * t.p[1].z + w * t.p[2].z;
			if (m_zBuffer[i + j * WIDTH] > p.z) {
				m_zBuffer[i + j * WIDTH] = p.z;
				setPixel(i, j, t.color);
			}
		}
	}
}