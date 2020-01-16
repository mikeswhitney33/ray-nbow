#include "transform.hpp"

namespace rt
{
    Transform::Transform(const vec3 &t, const vec3 &r, const vec3 &s):translation(t), rotation(r),scaling(s)
    {}
    mat4 Transform::mat() const
    {
        mat4 t = translate(translation);
        mat4 s = scale(scaling);
        mat4 rx = rotate(rotation.x, {1, 0, 0});
        mat4 ry = rotate(rotation.y, {0, 1, 0});
        mat4 rz = rotate(rotation.z, {0, 0, 1});
        return matmul(t, matmul(rx, matmul(ry, matmul(rz, s))));
    }

}; // namespace