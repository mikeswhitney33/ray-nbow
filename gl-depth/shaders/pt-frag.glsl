#version 330
in vec3 raydir;

uniform mat4 view;
uniform vec3 ambient;
uniform vec3 background;

out vec4 f_color;

// Shapes
#define SHAPE_SPHERE 0
#define SHAPE_TRIANGLE 1

// Material types
#define MAT_TYPE_DIFF 0
#define MAT_TYPE_SPEC 1
#define MAT_TYPE_REFL 2
#define MAT_TYPE_REFR 3
#define MAT_TYPE_LIGHT 4


struct Sphere {
    vec3 center;
    float rad2;
};

struct Triangle {
    vec3 a;
    vec3 b;
    vec3 c;
};

struct Material {
    int matType;
    vec3 ka;
    vec3 kd;
    vec3 ks;
    float alpha;
};

struct Shape {
    int shapeType;
    Material mat;
    Sphere sphere;
    Triangle triangle;
};

Sphere makeSphere(vec3 center, float rad) {
    return Sphere(center, rad*rad);
}

Triangle makeTriangle(vec3 a, vec3 b, vec3 c) {
    return Triangle(a, b, c);
}

Shape makeShape(Material mat, Sphere sphere) {
    return Shape(SHAPE_SPHERE, mat, sphere, makeTriangle(vec3(0,0,0), vec3(0,0,0), vec3(0,0,0)));
}

Shape makeShape(Material mat, Triangle triangle) {
    return Shape(SHAPE_TRIANGLE, mat, makeSphere(vec3(0,0,0), 0), triangle);
}

vec3 getShapeDirection(Sphere sphere, vec3 orig) {
    return normalize(sphere.center - orig);
}

vec3 getShapeDirection(Triangle triangle, vec3 orig) {
    vec3 center = (triangle.a + triangle.b + triangle.c) / 3;
    return normalize(center - orig);
}

vec3 getShapeDirection(Shape shape, vec3 orig) {
    if(shape.shapeType == SHAPE_SPHERE) {
        return getShapeDirection(shape.sphere, orig);
    }
    else if(shape.shapeType == SHAPE_TRIANGLE) {
        return getShapeDirection(shape.triangle, orig);
    }
    else {
        return vec3(0, 0, 0);
    }
}

vec3 getNormal(Sphere sphere, vec3 pt) {
    return normalize(sphere.center - pt);
}

vec3 getNormal(Triangle triangle, vec3 pt, vec3 dir) {
    vec3 AB = triangle.b - triangle.a;
    vec3 AC = triangle.c - triangle.a;
    vec3 normal = normalize(cross(AB, AC));
    if(dot(dir, normal) > 0) {
        normal = -normal;
    }
    return normal;
}

vec3 getNormal(Shape shape, vec3 pt, vec3 dir) {
    if(shape.shapeType == SHAPE_SPHERE) {
        return getNormal(shape.sphere, pt);
    }
    else if(shape.shapeType == SHAPE_TRIANGLE) {
        return getNormal(shape.triangle, pt, dir);
    }
    else {
        return vec3(0, 0, 0);
    }
}

float getDistance(Sphere sphere, vec3 pt) {
    return distance(sphere.center, pt);
}

float getDistance(Triangle triangle, vec3 pt) {
    vec3 center = (triangle.a + triangle.b + triangle.c) / 3;
    return distance(center, pt);
}

float getDistance(Shape shape, vec3 pt) {
    if(shape.shapeType == SHAPE_SPHERE) {
        return getDistance(shape.sphere, pt);
    }
    else if(shape.shapeType == SHAPE_TRIANGLE) {
        return getDistance(shape.triangle, pt);
    }
    else {
        return 9999999.0;
    }
}

{%% shapes %%}

bool intersect(vec3 orig, vec3 dir, vec3 A, vec3 B, vec3 C, inout float t) {
    vec3 AB = B - A;
    vec3 AC = C - A;
    vec3 pvec = cross(dir, AC);
    float det = dot(AB, pvec);
    if(abs(det) < 0.0000001) return false;
    float idet = 1 / det;
    vec3 tvec = orig - A;
    float u = dot(tvec, pvec) * idet;
    if(u < 0 || u > 1) return false;
    vec3 qvec = cross(tvec, AB);
    float v = dot(dir, qvec) * idet;
    if(v < 0 || u + v > 1) return false;
    float t0 = dot(AC, qvec) * idet;
    if(t0 > t) return false;
    t = t0;
    return true;
}

bool intersect(vec3 orig, vec3 dir, vec3 center, float rad2, inout float t) {
    vec3 L = center - orig;
    float tca = dot(L, dir);
    if(tca < 0) return false;
    float d2 = dot(L, L) - tca * tca;
    if(d2 > rad2) return false;
    float thc = sqrt(rad2 - d2);
    float t0 = tca - thc;
    float t1 = tca + thc;
    if(t0 > t1) {
        float tmp = t0;
        t0 = t1;
        t1 = tmp;
    }
    if(t0 < 0) {
        t0 = t1;
        if(t0 < 0) {
            return false;
        }
    }
    if(t0 > t) return false;
    t = t0;
    return true;
}

bool intersect(vec3 orig, vec3 dir, Shape shape, inout float t) {
    if(shape.shapeType == SHAPE_SPHERE) {
        return intersect(orig, dir, shape.sphere.center, shape.sphere.rad2, t);
    }
    else if(shape.shapeType == SHAPE_TRIANGLE) {
        return intersect(orig, dir, shape.triangle.a, shape.triangle.b, shape.triangle.c, t);
    }
    else {
        return false;
    }
}

bool intersect(vec3 orig, vec3 dir, inout int id, inout float t) {
    id = -1;
    t = 99999;
    for (int i = 0;i < NUM_SHAPES;i++) {
        if(intersect(orig, dir, shapes[i], t)) {
            id = i;
        }
    }
    return id > -1;
}

vec3 trace(vec3 orig, vec3 dir) {
    int id;
    float t;
    if(!intersect(orig, dir, id, t)) {
        return background;
    }
    Shape shape = shapes[id];
    vec3 pt = orig + dir * t;
    vec3 N = getNormal(shape, pt, dir);

    if(shape.mat.matType == MAT_TYPE_LIGHT) {
        return shape.mat.kd;
    }
    else if(shape.mat.matType == MAT_TYPE_DIFF) {
        vec3 color = ambient * shape.mat.ka;
        vec3 V = normalize(-dir);
        for(int i = 0;i < NUM_SHAPES;i++) {
            if(i == id || shapes[i].mat.matType != MAT_TYPE_LIGHT) {
                continue;
            }
            vec3 L = -getShapeDirection(shapes[i], pt);
            int _id;
            float _t;
            if(intersect(pt + N * 0.0000001, -L, _id, _t) && _id != i) {
                continue;
            }

            vec3 R = reflect(L, N);

            vec3 diff = shape.mat.kd * max(dot(L, N), 0) * shapes[i].mat.kd;
            vec3 spec = shape.mat.ks * pow(max(dot(R, V), 0), shape.mat.alpha) * shapes[i].mat.ks;
            color += diff + spec;
        }
        return color;
    };
}

void main() {
    initShapes();
    vec3 dir = normalize((view * vec4(raydir, 0)).xyz);
    vec3 eye = (view * vec4(0, 0, 0, 1)).xyz;
    vec3 color = trace(eye, dir);
    f_color = vec4(color, 1.0);
}