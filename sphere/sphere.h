#ifndef SPHERE_H
#define SPHERE_H

#include "../object3d/object3d.h"
#include "../vec3.h"

class sphere: public object3d {
private:
    public:
        sphere() {}
        sphere(point3 cp, double r, shared_ptr<material> m) : center(cp), radius(r), mat_ptr(m) {};

        virtual bool hit(const ray& r, double t_min, double t_max, ray_hit_point& r_hit) const override;
    public:
        point3 center;
        double radius;
        shared_ptr<material> mat_ptr;
};

bool sphere::hit(const ray& r, double t_min, double t_max, ray_hit_point& r_hit) const {
    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius*radius;

    auto discriminant = half_b*half_b - a*c; // wyróżnik wielomianu
    if(discriminant < 0) return false;
    auto sqrted = sqrt(discriminant);

    // Znajdowanie najbliższego pierwiastka
    auto root = (-half_b - sqrted) / a;
    if(root < t_min || t_max < root) {
        root = (-half_b + sqrted) / a;
        if (root < t_min || t_max)
            return false;
    }

    r_hit.t = root;
    r_hit.p = r.at(r_hit.t);
    vec3 out_n = (r_hit.p - center) / radius;
    r_hit.set_normal_dir(r, out_n);
    r_hit.mat_ptr = mat_ptr;
    
    return true;
};
#endif