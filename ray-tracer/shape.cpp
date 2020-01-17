#include "shape.hpp"

namespace rt
{
    Triangle::Triangle(const vec3 &a, const vec3 &b, const vec3 &c):a(a),b(b),c(c)
    {}

    bool Triangle::intersect(const Ray &ray, float &t) const
    {
        vec3 ab = b - a;
        vec3 ac = c - a;
        vec3 pvec = cross(ray.dir, ac);
        float det = dot(ab, pvec);
        if(fabs(det) < std::numeric_limits<float>::epsilon()) return false;
        float idet = 1 / det;
        vec3 tvec = ray.orig - a;
        float u = dot(tvec, pvec) * idet;
        if(u < 0 || u > 1) return false;
        vec3 qvec = cross(tvec, ab);
        float v = dot(ray.dir, qvec) * idet;
        if(v < 0 || u + v > 1) return false;
        float t0 = dot(ac, qvec) * idet;
        if(t0 > t) return false;
        t = t0;
        return true;
    }



    void Triangle::extents(vec3 &emin, vec3 &emax) const
    {
        vec3 abcx = {a.x, b.x, c.x};
        vec3 abcy = {a.y, b.y, c.y};
        vec3 abcz = {a.z, b.z, c.z};
        emin = 
        {
            min(abcx),
            min(abcy),
            min(abcz)
        };
        emax = 
        {
            max(abcx), max(abcy), max(abcz)
        };
    }

    Sphere::Sphere(const vec3 &c, const float &r):center(c), radius(r),radius2(r*r)
    {}

    bool Sphere::intersect(const Ray &ray, float &t) const 
    {
        vec3 L = center - ray.orig;
        float tca = dot(L, ray.dir);
        if(tca < 0) return false;  // ray is facing the wrong way
        float d2 = dot(L, L) - tca * tca;
        if(d2 > radius2) return false;  // ray misses the sphere
        float thc = sqrt(radius2 - d2);
        float t0 = tca - thc;
        float t1 = tca + thc;
        if(t0 > t1) std::swap(t0, t1); 
        if(t0 < 0) t0 = t1; // we are inside the sphere
        if(t0 < 0 || t0 > t) return false; // the sphere is behind us or another shape is infront of it.
        t = t0;
        return true;
    }

    void Sphere::extents(vec3 &emin, vec3 &emax) const
    {
        emin = {center.x - radius, center.y - radius, center.z - radius};
        emax = {center.x + radius, center.y + radius, center.z + radius};
    }


    BoundingBox::BoundingBox(Shape *s):shape(s)
    {
        shape->extents(bounds[0], bounds[1]);
    }
    bool BoundingBox::intersect(const Ray &ray, float &t) const
    {
        if(raybox(ray, bounds))
        {
            return shape->intersect(ray, t);
        }
        return false;
    }
    void BoundingBox::extents(vec3 &emin, vec3 &emax) const
    {
        emin = bounds[0];
        emax = bounds[1];
    }



    bool raybox(const Ray &ray, const vec3 bounds[2], float &t)
    {
        float tmin, tmax, tymin, tymax, tzmin, tzmax;
        tmin = (bounds[ray.sign[0]].x - ray.orig.x) * ray.invdir.x;
        tmax = (bounds[1 - ray.sign[0]].x - ray.orig.x) * ray.invdir.x;
        tymin = (bounds[ray.sign[1]].y - ray.orig.y) * ray.invdir.y;
        tymax = (bounds[1 - ray.sign[1]].y - ray.orig.y) * ray.invdir.y;

        if((tmin > tymax) || tymin > tmax)
        {
            return false;
        }
        if(tymin > tmin)
        {
            tmin = tymin;
        }
        if(tymax < tmax)
        {
            tmax = tymax;
        }

        tzmin = (bounds[ray.sign[2]].z - ray.orig.z) * ray.invdir.z;
        tzmax = (bounds[1 - ray.sign[2]].z - ray.orig.z) * ray.invdir.z;

        if((tmin > tzmax) || (tzmin > tmax))
        {
            return false;
        }

        if(tzmin > tmin)
        {
            tmin = tzmin;
        }
        if(tzmax < tmax)
        {
            tmax = tzmax;
        }
        float t0 = tmin;
        if(t0 < 0)
        {
            t0 = tmax;
            if(t0 < 0) return false;
        }
        if(t0 > t) return false;
        t = t0;
        return true; 
    }

    bool raybox(const Ray &ray, const vec3 bounds[2])
    {
        float t = std::numeric_limits<float>::max();
        return raybox(ray, bounds, t);   
    }

    bool LinearContainer::intersect(const Ray &ray, float &t) const
    {
        bool hit = false;
        for(auto s : shapes)
        {
            if(s->intersect(ray, t))
            {
                hit = true;
            }
        }
        return hit;
    }
    void LinearContainer::addShape(Shape *shape)
    {
        shapes.push_back(shape);
    }
    size_t LinearContainer::size() const 
    {
        return shapes.size();
    }

    bool MassBoxContainer::intersect(const Ray &ray, float &t) const
    {
        if(size() == 0) return false;
        if(!raybox(ray, bounds, t)) return false;
        t = std::numeric_limits<float>::max();
        return shapes.intersect(ray, t);
    }
    void MassBoxContainer::addShape(Shape *shape)
    {
        if(size() == 0)
        {
            shape->extents(bounds[0], bounds[1]);
        }
        else
        {
            vec3 emin, emax;
            shape->extents(emin, emax);
            bounds[0].x = std::min(emin.x, bounds[0].x);
            bounds[0].y = std::min(emin.y, bounds[0].y);
            bounds[0].z = std::min(emin.z, bounds[0].z);
            bounds[1].x = std::max(emax.x, bounds[1].x);
            bounds[1].y = std::max(emax.y, bounds[1].y);
            bounds[1].z = std::max(emax.z, bounds[1].z);
        }
        shapes.addShape(shape);
    }
    size_t MassBoxContainer::size() const
    {
        return shapes.size();
    }

    constexpr size_t OctreeNode::MAX_SIZE;
    constexpr size_t OctreeNode::MAX_DEPTH;

    OctreeNode::OctreeNode(const vec3 &emin, const vec3 &emax, size_t depth):depth(depth)
    {
        center = (emin + emax) / 2;
        bounds[0] = emin;
        bounds[1] = emax;
    }

    bool boxbox(const vec3 bounds1[2], const vec3 bounds2[2])
    {
        return 
            bounds1[0].x <= bounds2[1].x && bounds1[1].x >= bounds2[0].x &&
            bounds1[0].y <= bounds2[1].y && bounds1[1].y >= bounds2[0].y &&
            bounds1[0].z <= bounds2[1].z && bounds1[1].z >= bounds2[0].z;
    }

    bool OctreeNode::intersect(const Ray &ray, float &t) const
    {
        bool hit = false;
        if(children.size() == 0)
        {
            for(size_t i = 0;i < content.size();i++)
            {
                if(content[i]->intersect(ray, t))
                {
                    hit = true;
                }
            }
        }

        for(size_t i = 0;i < children.size();i++)
        {
            if(children[i].intersect(ray, t))
            {
                hit = true;
            }
        }
        return hit;
    }
    bool OctreeNode::intersect(Shape *shape) const
    {
        vec3 emin, emax;
        shape->extents(emin, emax);
        vec3 bounds2[2] = {emin, emax};
        return boxbox(bounds, bounds2);
    }

    void OctreeNode::addShape(Shape *shape)
    {
        if(children.size() == 0)
        {
            content.push_back(shape);
            if(content.size() >= MAX_SIZE && depth < MAX_DEPTH)
            {
                split();
            }
        }
        else 
        {
            for(auto &child : children)
            {
                if(child.intersect(shape))
                {
                    child.addShape(shape);
                }
            }
        }
    }

    void OctreeNode::split()
    {
        vec3 qpppnear, qnppnear, qpnpnear, qppnnear, qnnpnear, qnpnnear, qpnnnear, qnnnnear;
        vec3 qpppfar, qnppfar, qpnpfar, qppnfar, qnnpfar, qnpnfar, qpnnfar, qnnnfar;
  
        qpppnear = center;
        qnppnear = {bounds[0].x, center.y, center.z};
        qpnpnear = {center.x, bounds[0].y, center.z};
        qppnnear = {center.x, center.y, bounds[0].z};
        qnnpnear = {bounds[0].x, bounds[0].y, center.z};
        qnpnnear = {bounds[0].x, center.y, bounds[0].z};
        qpnnnear = {center.x, bounds[0].y, bounds[0].z};
        qnnnnear = {bounds[0].x, bounds[0].y, bounds[0].z};

        qpppfar = bounds[1];
        qnnnfar = center;
        qnppfar = {center.x, bounds[1].y, bounds[1].z};
        qpnpfar = {bounds[1].x, center.y, bounds[1].z};
        qppnfar = {bounds[1].x, bounds[1].y, center.z};
        qnnpfar = {center.x, center.y, bounds[1].z};
        qnpnfar = {center.x, bounds[1].y, center.z};
        qpnnfar = {bounds[1].x, center.y, center.z};

        children.push_back(OctreeNode(qpppnear, qpppfar, depth+1));
        children.push_back(OctreeNode(qnppnear, qnppfar, depth+1));
        children.push_back(OctreeNode(qpnpnear, qpnpfar, depth+1));
        children.push_back(OctreeNode(qppnnear, qppnfar, depth+1));
        children.push_back(OctreeNode(qnnpnear, qnnpfar, depth+1));
        children.push_back(OctreeNode(qnpnnear, qnpnfar, depth+1));
        children.push_back(OctreeNode(qpnnnear, qpnnfar, depth+1));
        children.push_back(OctreeNode(qnnnnear, qnnnfar, depth+1));
        for(auto shape : content)
        {
            vec3 emin, emax;
            shape->extents(emin, emax);
            vec3 ebounds[2] = {emin, emax};
            for(auto &child : children)
            {
                if(boxbox(child.bounds, ebounds))
                {
                    child.addShape(shape);
                }
            }
        }
    }
};