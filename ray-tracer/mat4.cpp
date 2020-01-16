#include "mat4.hpp"


namespace rt
{
    std::ostream &operator<<(std::ostream &os, const mat4 &m)
    {
        for(int i = 0;i< 4;i++)
        {
            if(i > 0) os << "\n";
            for(int j = 0;j < 4;j++)
            {
                if(j > 0) os << ",";
                os << m.m[i][j];
            }
        }
        return os;
    }

    mat4 identity()
    {
        return mat4{
            {
                {1, 0, 0, 0},
                {0, 1, 0, 0},
                {0, 0, 1, 0},
                {0, 0, 0, 1}
            }
        };
    }

    mat4 translate(const float &dx, const float &dy, const float &dz)
    {
        return mat4{
            {
                {1, 0, 0, dx},
                {0, 1, 0, dy},
                {0, 0, 1, dz},
                {0, 0, 0, 1}
            }
        };
    }

    mat4 translate(const vec3 &d)
    {
        return translate(d.x, d.y, d.z);
    }

    mat4 scale(const float &dx, const float &dy, const float &dz)
    {
        return mat4{
            {
                {dx, 0, 0, 0},
                {0, dy, 0, 0},
                {0, 0, dz, 0},
                {0, 0, 0, 1}
            }
        };
    }

    mat4 scale(const vec3 &d)
    {
        return scale(d.x, d.y, d.z);
    }

    mat4 scale(const float &s)
    {
        return scale(s, s, s);
    }

    mat4 rotate(const float &angle, const vec3 &axis)
    {
        float c = cos(angle);
        float s = sin(angle);
        vec3 ax = norm(axis);
        vec3 tmp = ax * (1 - c);

        return mat4{
            {
                {c + tmp.x * ax.x, tmp.x * ax.y + s * ax.z, tmp.x * ax.z - s * ax.y, 0},
                {tmp.y * ax.x - s * ax.z, c + tmp.y * ax.y, tmp.y * ax.z + s * ax.x, 0},
                {tmp.z * ax.x + s * ax.y, tmp.z * ax.y - s * ax.x, c + tmp.z * ax.z, 0},
                {0, 0, 0, 1}
            }
        };
    }

    mat4 lookAt(const vec3 &eye, const vec3 &center, const vec3 &up)
    {
        vec3 f = norm(eye - center);
        vec3 s = norm(cross(f, up));
        vec3 u = norm(cross(s, f));
        return mat4{
            {
                {s.x, s.y, s.z, -eye.x},
                {u.x, u.y, u.z, -eye.y},
                {f.x, f.y, f.z, -eye.z},
                {0, 0, 0, 1}
            }
        };
    }

    vec3 transformPt(const mat4 &m, const vec3 &v)
    {
        return vec3{
            m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3],
            m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3],
            m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3]
        };
    }

    vec3 transformDir(const mat4 &m, const vec3 &v)
    {
        return vec3{
            m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z,
            m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z,
            m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z
        };
    }

    mat4 matmul(const mat4 &m1, const mat4 &m2)
    {
        mat4 res;
        for(int i =0;i < 4;i++)
        {
            for(int j = 0;j < 4;j++)
            {
                res.m[i][j] = 0;
                for(int k = 0;k < 4;k++)
                {
                    res.m[i][j] += m1.m[i][k] * m2.m[j][k];
                }
            }
        }
        return res;
    }
};
 