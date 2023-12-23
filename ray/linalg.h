#include <math.h>

namespace linalg
{
    struct vec3 {
        struct {float x, y, z;};

        vec3(float x, float y, float z) : x(x), y(y), z(z) {
        }
        vec3(float k) : x(k), y(k), z(k) {
        }
        vec3() : x(0), y(0), z(0) {
        }
        vec3 scale(float s) {
            return vec3{x * s, y * s, z * s};
        }
        vec3 multiply(vec3 v) {
            return vec3{x * v.x, y * v.y, z * v.z};
        }
        vec3 add(vec3 v) {
            return vec3{x + v.x, y + v.y, z + v.z};
        }
        vec3 subtract(vec3 v) {
            return vec3{x - v.x, y - v.y, z - v.z};
        }
        float dot(vec3 v) {
            return x * v.x + y * v.y + z * v.z;
        }
        vec3 cross(vec3 v) {
            return vec3{y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x};
        }
    };
    
    struct vec4 {
        float x, y, z, w;

        vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {
        }
        vec4(float k) : x(k), y(k), z(k), w(k) {
        }
        vec4() : x(0), y(0), z(0), w(0) {
        }
        vec4(vec3 v): x(v.x), y(v.y), z(v.z), w(1) {
        } 
        vec4 scale(float s) {
            return vec4{x * s, y * s, z * s, w * s};
        }
        vec4 multiply(vec4 v) {
            return vec4{x * v.x, y * v.y, z * v.z, w * v.w};
        }
        vec4 add(vec4 v) {
            return vec4{x + v.x, y + v.y, z + v.z, w + v.w};
        }
        vec4 subtract(vec4 v) {
            return vec4{x - v.x, y - v.y, z - v.z, w - v.w};
        }
        float dot(vec4 v) {
            return x * v.x + y * v.y + z * v.z + w * v.w;
        }
        vec3 unhomo() {
            return vec3{x / w, y / w, z / w};
        }
    };

    struct mat3x3 {
        vec3 r1;
        vec3 r2;
        vec3 r3;
        vec3 c1() {
            return vec3{r1.x, r2.x, r3.x};
        }
        vec3 c2() {
            return vec3{r1.y, r2.y, r3.y};
        }
        vec3 c3() {
            return vec3{r1.z, r2.z, r3.z};
        }
        mat3x3() : 
        r1(vec3{1,0,0}),
        r2(vec3{0,1,0}),
        r3(vec3{0,0,1}) {
        }
        mat3x3(float k) :
        r1(vec3{k}),
        r2(vec3{k}),
        r3(vec3{k}) {
        }
        mat3x3(float x11, float x12, float x13,
               float x21, float x22, float x23,
               float x31, float x32, float x33) : 
               r1(vec3{x11, x12, x13}),
               r2(vec3{x21, x22, x23}),
               r3(vec3{x31, x32, x33}) {
        }
        mat3x3(vec3 r1, vec3 r2, vec3 r3) : r1(r1), r2(r2), r3(r3) {
        }
        mat3x3 transpose() {
            return mat3x3(c1(), c2(), c3());
        }
        mat3x3 mult(mat3x3 A) {
            return mat3x3{
                mult(A.c1()),
                mult(A.c2()),
                mult(A.c3())
                }.transpose();
        }
        vec3 mult(vec3 v) {
            return vec3{
                r1.dot(v),
                r2.dot(v),
                r3.dot(v)
            };
        }
        float determinant() {
            return r1.x * r2.y * r3.z 
                 + r1.y * r2.z * r3.x 
                 + r1.z * r2.x * r3.y 
                 - r1.z * r2.y * r3.x 
                 - r1.y * r2.x * r3.z 
                 - r1.x * r2.z * r3.y;
        }
    };

    struct mat4x4 {
        vec4 r1;
        vec4 r2;
        vec4 r3;
        vec4 r4;
        vec4 c1() {
            return vec4{r1.x, r2.x, r3.x, r4.w};
        }
        vec4 c2() {
            return vec4{r1.y, r2.y, r3.y, r4.w};
        }
        vec4 c3() {
            return vec4{r1.z, r2.z, r3.z, r4.w};
        }
        vec4 c4() {
            return vec4{r1.w, r2.w, r3.w, r4.w};
        }
        mat4x4() : 
        r1(vec4{1,0,0,0}),
        r2(vec4{0,1,0,0}),
        r3(vec4{0,0,1,0}),
        r4(vec4{0,0,0,1}) {
        }
        mat4x4(float k) :
        r1(vec4{k}),
        r2(vec4{k}),
        r3(vec4{k}),
        r4(vec4{k}) {
        }
        mat4x4(float x11, float x12, float x13, float x14,
               float x21, float x22, float x23, float x24,
               float x31, float x32, float x33, float x34,
               float x41, float x42, float x43, float x44) : 
               r1(vec4{x11, x12, x13, x14}),
               r2(vec4{x21, x22, x23, x24}),
               r3(vec4{x31, x32, x33, x34}),
               r4(vec4{x41, x42, x43, x44}) {
        }
        mat4x4(vec4 r1, vec4 r2, vec4 r3, vec4 r4) : r1(r1), r2(r2), r3(r3), r4(r4) {
        }
        mat4x4 transpose() {
            return mat4x4(c1(), c2(), c3(), c4());
        }
        mat4x4 mult(mat4x4 A) {
            return mat4x4{
                mult(A.c1()),
                mult(A.c2()),
                mult(A.c3()),
                mult(A.c4())
                }.transpose();
        }
        vec4 mult(vec4 v) {
            return vec4{
                r1.dot(v),
                r2.dot(v),
                r3.dot(v),
                r4.dot(v)
            };
        }
        float determinant() {
            return
                  r4.x * r3.y * r2.z * r1.w
                - r3.x * r4.y * r2.z * r1.w
                - r4.x * r2.y * r3.z * r1.w
                + r2.x * r4.y * r3.z * r1.w
                + r3.x * r2.y * r4.z * r1.w
                - r2.x * r3.y * r4.z * r1.w
                - r4.x * r3.y * r1.z * r2.w
                + r3.x * r4.y * r1.z * r2.w
                + r4.x * r1.y * r3.z * r2.w
                - r1.x * r4.y * r3.z * r2.w
                - r3.x * r1.y * r4.z * r2.w
                + r1.x * r3.y * r4.z * r2.w
                + r4.x * r2.y * r1.z * r3.w
                - r2.x * r4.y * r1.z * r3.w
                - r4.x * r1.y * r2.z * r3.w
                + r1.x * r4.y * r2.z * r3.w
                + r2.x * r1.y * r4.z * r3.w
                - r1.x * r2.y * r4.z * r3.w
                - r3.x * r2.y * r1.z * r4.w
                + r2.x * r3.y * r1.z * r4.w
                + r3.x * r1.y * r2.z * r4.w
                - r1.x * r3.y * r2.z * r4.w
                - r2.x * r1.y * r3.z * r4.w
                + r1.x * r2.y * r3.z * r4.w;
        }
    };
}

