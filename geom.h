#ifndef _GEOM_H_
#define _GEOM_H_

#include <math.h>

struct Vec3 {
	float x,y,z;
	Vec3( float _x, float _y, float _z ) : x(_x), y(_y), z(_z) {}
	Vec3() {}
	Vec3 operator+(const Vec3 &other) const { return Vec3(x+other.x, y+other.y, z+other.z); }
	Vec3 operator-(const Vec3 &other) const { return Vec3(x-other.x, y-other.y, z-other.z); }
	Vec3 operator*(const float &other) const { return Vec3(x*other, y*other, z*other); }
	Vec3& operator+=(const Vec3 &other) { x+=other.x; y+=other.y; z+=other.z; return *this; }
	Vec3& operator-=(const Vec3 &other) { x-=other.x; y-=other.y; z-=other.z; return *this; }
	float dot(const Vec3 &other) const { return x*other.x + y*other.y + z*other.z; }
	float abs() const { return sqrtf(dot(*this)); }
	Vec3 cross(const Vec3 &other) {
		return Vec3( y*other.z - z*other.y,
				z*other.x - x*other.z,
				x*other.y - y*other.x );
	}
	Vec3 normalized() const { return *this * (1.0f / abs()); }
	Vec3 operator-() const { return Vec3(-x, -y, -z); }
};

struct Vec4 {
	float x,y,z,w;
	operator Vec3() const { return Vec3( x, y, z ); }
};

struct Mat44 {
	Vec4 x,y,z,w;
};

struct AABB {
	Vec3 aamin, aamax;
	static const AABB EVERYTHING;
	AABB() {}
	AABB( const Vec3 &minVal, const Vec3 &maxVal ) : aamin(minVal), aamax(maxVal) {}
};

// Vec3 operations
inline float dot( const Vec3 a, const Vec3 b ) { return a.dot(b); }

#endif
