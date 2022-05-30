#ifndef OBJECT3D_H
#define OBJECT3D_H

#include "../utils.h"
#include "../aabb.h"

class material;

struct ray_hit_point {
    point3 p;
    vec3 normal;
    shared_ptr<material> mat_ptr;
    double t;
    double u;
    double v;
    bool front_face;

    inline void set_normal_dir(const ray& r, const vec3& out_n) {
        front_face = dot(r.direction(), out_n) < 0;
        normal = front_face ? out_n : -out_n; 
    }
};

class object3d {
    public:
        virtual bool hit(const ray& r, double t_min, double t_max, ray_hit_point& r_hit) const = 0;
        virtual bool bounding_box(double time0, double time1, aabb& output_box) const = 0;

};
#endif  