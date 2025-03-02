#pragma once
#ifndef BESPOKECONSTRUCTS_H
#define BESPOKECONSTRUCTS_H

#include <cmath>
#include <sstream>
#include "staticConstants.h"


struct vec3 {
    double x;
    double y;
    double z;

    //default constructor
    vec3() : x(0), y(0), z(0) {}

    vec3(double x, double y, double z) : x(x), y(y), z(z) {}
    vec3(int x, int y, int z) : x(x), y(y), z(z) {}

    inline vec3& operator= (const double* arr) {
        x = arr[0];
        y = arr[1];
        z = arr[2];
        return *this; //avoids copying the object (better for performance)
    }
    inline vec3& operator= (const double value) {
        x = value;
        y = value;
        z = value;
        return *this;
    }
    inline vec3& operator= (const int value) {
        x = value;
        y = value;
        z = value;
        return *this;
    }

    inline vec3 operator+ (const vec3& vals) const {
        return vec3(x + vals.x, y + vals.y, z + vals.z);
    }
    inline vec3 operator+ (const double value) const {
        return vec3(x + value, y + value, z + value);
    }
    inline vec3 operator+ (const int value) const {
        return vec3(x + value, y + value, z + value);
    }

    inline vec3 operator- (const vec3& vals) const {
        return vec3(x - vals.x, y - vals.y, z - vals.z);
    }
    inline vec3 operator- (const double value) const {
        return vec3(x - value, y - value, z - value);
    }
    inline vec3 operator- (const int value) const {
        return vec3(x - value, y - value, z - value);
    }

    inline vec3 operator* (const vec3& vals) const {
        return vec3(x * vals.x, y * vals.y, z * vals.z);
    }
    inline vec3 operator* (const double value) const {
        return vec3(x * value, y * value, z * value);
    }
    inline vec3 operator* (const int value) const {
        return vec3(x * value, y * value, z * value);
    }

    inline vec3 operator/ (const vec3& vals) const {
        return vec3(x / vals.x, y / vals.y, z / vals.z);
    }
    inline vec3 operator/ (const double value) const {
        return vec3(x / value, y / value, z / value);
    }
    inline vec3 operator/ (const int value) const {
        return vec3(x / value, y / value, z / value);
    }

    inline vec3& operator+= (const vec3& vals) {
        x += vals.x;
        y += vals.y;
        z += vals.z;
        return *this;
    }
    inline vec3& operator+= (const double value) {
        x += value;
        y += value;
        z += value;
        return *this;
    }
    inline vec3& operator+= (const int value) {
        x += value;
        y += value;
        z += value;
        return *this;
    }

    inline vec3& operator-= (const vec3& vals) {
        x -= vals.x;
        y -= vals.y;
        z -= vals.z;
        return *this;
    }
    inline vec3& operator-= (const double value) {
        x -= value;
        y -= value;
        z -= value;
        return *this;
    }
    inline vec3& operator-= (const int value) {
        x -= value;
        y -= value;
        z -= value;
        return *this;
    }

    inline vec3& operator*= (const vec3& vals) {
        x *= vals.x;
        y *= vals.y;
        z *= vals.z;
        return *this;
    }
    inline vec3& operator*= (const double value) {
        x *= value;
        y *= value;
        z *= value;
        return *this;
    }
    inline vec3& operator*= (const int value) {
        x *= value;
        y *= value;
        z *= value;
        return *this;
    }

    inline vec3& operator/= (const vec3& vals) {
        x /= vals.x;
        y /= vals.y;
        z /= vals.z;
        return *this;
    }
    inline vec3& operator/= (const double value) {
        x /= value;
        y /= value;
        z /= value;
        return *this;
    }
    inline vec3& operator/= (const int value) {
        x /= value;
        y /= value;
        z /= value;
        return *this;
    }

    inline friend std::istream& operator>>(std::istream& is, vec3& vec) {
        is >> vec.x >> vec.y >> vec.z;
        return is;
    }

    inline friend bool operator! (vec3& vec) {
        return vec.x && vec.y && vec.z;
    }

    inline friend vec3 operator- (vec3& vec) {
        return vec3(-vec.x, -vec.y, -vec.z);
    }
};

inline vec3 operator+ (const double value, const vec3& vec) {
    return vec3(value + vec.x, value + vec.y, value + vec.z);
}
inline vec3 operator+ (const int value, const vec3& vec) {
    return vec3(value + vec.x, value + vec.y, value + vec.z);
}

inline vec3 operator- (const double value, const vec3& vec) {
    return vec3(value - vec.x, value - vec.y, value - vec.z);
}
inline vec3 operator- (const int value, const vec3& vec) {
    return vec3(value - vec.x, value - vec.y, value - vec.z);
}

inline vec3 operator* (const double value, const vec3& vec) {
    return vec3(value * vec.x, value * vec.y, value * vec.z);
}
inline vec3 operator* (const int value, const vec3& vec) {
    return vec3(value * vec.x, value * vec.y, value * vec.z);
}

inline vec3 operator/ (const double value, const vec3& vec) {
    return vec3(value / vec.x, value / vec.y, value / vec.z);
}
inline vec3 operator/ (const int value, const vec3& vec) {
    return vec3(value / vec.x, value / vec.y, value / vec.z);
}


inline double dot(const vec3& a, const vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline vec3 cross(const vec3& a, const vec3& b) {
    return vec3(
        a.y * b.z - a.z * b.y, 
        a.z * b.x - a.x * b.z, 
        a.x * b.y - a.y * b.x
    );
}

inline double length(const vec3& vec) {
    return std::sqrt(dot(vec, vec));
}

inline vec3 normalise(const vec3& vec) {
    return vec / length(vec);
}


struct mat3 {
    double xx, xy, xz, yx, yy, yz, zx, zy, zz;

    mat3(double xx, double xy, double xz, double yx, double yy, double yz, double zx, double zy, double zz):
    xx(xx), xy(xy), xz(xz), yx(yx), yy(yy), yz(yz), zx(zx), zy(zy), zz(zz) {}
};


inline vec3 applyMat3(const mat3& mat, const vec3& vec) {
    return vec3(
        mat.xx * vec.x + mat.xy * vec.y + mat.xz * vec.z,
        mat.yx * vec.x + mat.yy * vec.y + mat.yz * vec.z,
        mat.zx * vec.x + mat.zy * vec.y + mat.zz * vec.z
    );
}


//courtesey of Wikipedia. Again...
inline vec3 applyEulerSet(const vec3& vec, const vec3& eulerSet) {
    double cp = cos(eulerSet.x); //pitch
    double sp = sin(eulerSet.x);
    double cy = cos(eulerSet.y); //yaw
    double sy = sin(eulerSet.y);
    double cr = cos(eulerSet.z); //roll
    double sr = sin(eulerSet.z);

    mat3 rotMat3(
        cy * cp, 
        cy * sp * sr - sy * cr,
        cy * sp * cr + sy * sr, 
        sy * cp, 
        sy * sp * sr + cy * cr, 
        sy * sp * cr - cy * sr, 
        -sp, 
        cp * sr, 
        cp * cr
    );

    return applyMat3(rotMat3, vec);
}



struct Particle {
    double mass, radius, charge, Cd, A;
    vec3 s, v, a;

    inline double calculateCrossSectionalArea() {
        return radius * radius * pi;
    }

    Particle(double mass, const vec3& s, const vec3& v, double radius = 1.0, double charge = 0.0, double Cd = 0.0): 
        mass(mass), radius(radius), charge(charge), Cd(Cd), s(s), v(v) {
        a = 0;

        A = calculateCrossSectionalArea();
    }
};


struct Plane {
    double length, width, charge, A, sigma;
    vec3 s, normal, xUnit, yUnit;

    inline double calculateChargeDensity() {
        return charge / A;
    }

    inline double calculateArea() {
        return length * width;
    }

    Plane(double length, double width, double charge, const vec3& s, const vec3& orientations): 
        length(length), width(width), charge(charge), s(s) {
        
        normal = applyEulerSet(vec3(0, 1, 0), orientations); //upwards facing normal
        xUnit = applyEulerSet(vec3(1, 0, 0), orientations);
        yUnit = applyEulerSet(vec3(0, 0, 1), orientations);

        A = calculateArea();
        sigma = calculateChargeDensity();
    }
};


#endif
