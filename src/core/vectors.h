/*
    Copyright (c) 2009 Andrew Caudwell (acaudwell@gmail.com)
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. The name of the author may not be used to endorse or promote products
       derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
    IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
    OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
    NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
    THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef VECTORS_H
#define VECTORS_H

#include <cmath>

template<class T> class vec2 {
public:
    T x;
    T y;

    vec2(const vec2<T>& vec) {
        this->x = vec.x;
        this->y = vec.y;
    }

    vec2(T x = 0, T y = 0) {
        this->x = x;
        this->y = y;
    }

    vec2<T> perpendicular() const {
        return vec2<T>(y * -1.0f, x);
    }


    T dot(const vec2<T> &vec) const {
        return x*vec.x + y*vec.y;
    }

    T operator* (const vec2<T> &vec) const {
        return x*vec.x + y*vec.y;
    }

    vec2<T> operator+ (const vec2<T> &vec) const {
        return vec2<T>(x+vec.x, y+vec.y);
    }

    vec2<T> operator- (const vec2<T> &vec) const {
        return vec2<T>(x-vec.x, y-vec.y);
    }

    vec2<T> operator* (const T n) const {
        return vec2<T>(x*n, y*n);
    }

    vec2<T> operator/ (const T n) const {
        return vec2<T>(x/n, y/n);
    }

    friend vec2<T> operator* (T n, const vec2<T>& vec) {
        return vec2<T>(vec.x*n, vec.y*n);
    }

    bool operator== (vec2<T> &vec) const {
        return (vec.x==x && vec.y==y);
    }

    bool operator!= (vec2<T> &vec) const {
        return (vec.x!=x || vec.y!=y);
    }

    T length2() const {
        return x*x + y*y;
    }

    T length() const {
        return sqrt(x*x + y*y);
    }

    void normalize() {
        T len = length();
        if(len == 0) return;

        *this *= (1.0/len);
    }

    vec2<T> normal() const {
        vec2<T> v(x,y);
        v.normalize();
        return v;
    }

    operator T*() const {
        return (T*) &x;
    }

    vec2<T>& operator= (const vec2<T> &vec) {
        x = vec.x;
        y = vec.y;
        return *this;
    }

    void operator+= (const vec2<T> &vec) {
        x += vec.x;
        y += vec.y;
    }

    void operator-= (const vec2<T> &vec) {
        x -= vec.x;
        y -= vec.y;
    }

    void operator*= (T n) {
        x *= n;
        y *= n;
    }

    void operator/= (T n) {
        x /= n;
        y /= n;
    }
};

template<class T> class vec3 {
public:
    T x;
    T y;
    T z;

    vec3(const vec3<T>& vec) {
        this->x = vec.x;
        this->y = vec.y;
        this->z = vec.z;
    }

    vec3(T x = 0, T y = 0, T z = 0) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    vec3<T> cross(const vec3<T>& vec) const {
        return vec3<T>(y*vec.z-z*vec.y, z*vec.x-x*vec.z, x*vec.y-y*vec.x);
    }

    T dot(const vec3<T>& vec) const {
        return x*vec.x + y*vec.y + z*vec.z;
    }

    T operator* (const vec3<T> &vec) const {
        return x*vec.x + y*vec.y + z*vec.z;
    }

    vec3<T> operator+ (const vec3<T> &vec) const {
        return vec3<T>(x+vec.x,y+vec.y,z+vec.z);
    }

    vec3<T> operator- (const vec3<T> &vec) const {
        return vec3<T>(x-vec.x,y-vec.y,z-vec.z);
    }

    vec3<T> operator* (T n) const {
		return vec3<T>(x*n,y*n,z*n);
	}

	vec3<T> operator/ (const T n) const {
		return vec3<T>(x/n, y/n, z/n);
	}

	friend vec3<T> operator* (T n, const vec3<T>& vec) {
		return vec3<T>(vec.x*n,vec.y*n,vec.z*n);
	}

    bool operator== (vec3<T> &vec) const {
        return (vec.x==x && vec.y==y && vec.z==z);
    }

    bool operator!= (vec3<T> &vec) const {
        return (vec.x!=x || vec.y!=y || vec.z!=z);
    }

	T length2() const {
		return x*x + y*y + z*z;
	}

	T length() const {
        return sqrt(x*x + y*y + z*z);
	}

    void normalize() {
        T len = length();
        if(len == 0) return;

        *this *= (1.0/len);
    }

    vec3<T> normal() const {
        vec3<T> v(x,y,z);
        v.normalize();
        return v;
    }

    vec2<T> truncate() const {
        return vec2<T>(x, y);
    }

    operator T*() const {
        return (T*) &x;
    }

    vec3<T>& operator= (const vec3<T> &vec) {
        x = vec.x;
        y = vec.y;
        z = vec.z;
        return *this;
    }

    void operator+= (const vec3<T> &vec) {
        x += vec.x;
        y += vec.y;
        z += vec.z;
    }

    void operator-= (const vec3<T> &vec) {
        x -= vec.x;
        y -= vec.y;
        z -= vec.z;
    }

    void operator*= (T n) {
        x *= n;
        y *= n;
        z *= n;
    }

    void operator/= (T n) {
        x /= n;
        y /= n;
        z /= n;
    }
};

template<class T> class vec4 {
public:
    T x;
    T y;
    T z;
    T w;

    vec4(T x = 0, T y = 0, T z = 0, T w = 0) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

    vec4(const vec4<T>& vec) {
        this->x = vec.x;
        this->y = vec.y;
        this->z = vec.z;
        this->w = vec.w;
    }

    vec4(vec3<T>& vec, T w) {
        this->x = vec.x;
        this->y = vec.y;
        this->z = vec.z;
        this->w = w;
    }

    vec4<T>& operator= (const vec4<T> &vec) {
        x = vec.x;
        y = vec.y;
        z = vec.z;
        w = vec.w;
        return *this;
    }

    T dot(const vec4<T>& vec) const {
        return x*vec.x + y*vec.y + z*vec.z + w*vec.w;
    }

    vec4<T> operator+ (const vec4<T> &vec) const {
        return vec4<T>(x+vec.x, y+vec.y, z+vec.z, w+vec.w);
    }

    vec4<T> operator- (const vec4<T> &vec) const {
        return vec4<T>(x-vec.x, y-vec.y, z-vec.z, w-vec.w);
    }

    vec4<T> operator* (T n) const {
        return vec4<T>(x*n, y*n, z*n, w*n);
    }

	vec4<T> operator/ (const T n) const {
		return vec4<T>(x/n, y/n, z/n, w/n);
	}

    T operator* (const vec4<T> &vec) const {
        return x*vec.x + y*vec.y + z*vec.z + w*vec.w;
    }

    friend vec4<T> operator* (T n, const vec4<T>& vec) {
        return vec4<T>(vec.x*n, vec.y*n, vec.z*n, vec.w*n);
    }

    bool operator== (vec4<T> &vec) const {
        return (vec.x==x && vec.y==y && vec.z==z && vec.w==w);
    }

    bool operator!= (vec4<T> &vec) const {
        return (vec.x!=x || vec.y!=y || vec.z!=z || vec.w==w);
    }

    T length2() const {
        return  x*x + y*y + z*z + w*w;
    }

    T length() const {
        return sqrt(x*x + y*y + z*z + w*w);
    }

    void normalize() {
        T len = length();
        if(len == 0) return;

        *this *= (1.0/len);
    }

    vec4<T> normal() const {
        vec4<T> v(x,y,z,w);
        v.normalize();
        return v;
    }

    vec3<T> truncate() const {
        return vec3<T>(x, y, z);
    }

    operator T*() const {
        return (T*) &x;
    }

    void operator+= (const vec4<T> &vec) {
        x += vec.x;
        y += vec.y;
        z += vec.z;
        w += vec.w;
    }

    void operator-= (const vec4<T> &vec) {
        x -= vec.x;
        y -= vec.y;
        z -= vec.z;
        w -= vec.w;
    }

    void operator*= (T n) {
        x *= n;
        y *= n;
        z *= n;
        w *= n;
    }

    void operator/= (T n) {
        x /= n;
        y /= n;
        z /= n;
        w /= n;
    }
};

typedef vec2<float> vec2f;
typedef vec3<float> vec3f;
typedef vec4<float> vec4f;

typedef vec2<int> vec2i;
typedef vec3<int> vec3i;
typedef vec4<int> vec4i;

#endif
