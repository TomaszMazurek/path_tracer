#ifndef SPHERE_H
#define SPHERE_H

#include "../object3d/object3d.h"
#include "../vec3.h"
#include "../onb.h"
#include "../pdf.h"

class sphere: public object3d {
private:
    public:
        sphere() {}
        sphere(point3 cp, double r, shared_ptr<material> m) : center(cp), radius(r), mat_ptr(m) {};

        virtual bool hit(const ray& r, double t_min, double t_max, ray_hit_point& r_hit) const override;
        virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;
        virtual double pdf_value(const point3& o, const vec3& v) const override;
        virtual vec3 random(const point3& o) const override;

    public:
        point3 center;
        double radius;
        shared_ptr<material> mat_ptr;

    private:
        static void get_sphere_uv(const point3& p, double& u, double& v) {
            // p: a given point on the sphere of radius one, centered at the origin.
            // u: returned value [0,1] of angle around the Y axis from X=-1.
            // v: returned value [0,1] of angle from Y=-1 to Y=+1.
            //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
            //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
            //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

            auto theta = acos(-p.y());
            auto phi = atan2(-p.z(), p.x()) + pi;

            u = phi / (2*pi);
            v = theta / pi;
        }    
};

double sphere::pdf_value(const point3& o, const vec3& v) const {
    ray_hit_point hit;
    if (!this->hit(ray(o, v), 0.001, inf, hit))
        return 0;

    auto cos_theta_max = sqrt(1 - radius*radius/(center-o).length_squared());
    auto solid_angle = 2*pi*(1-cos_theta_max);

    return  1 / solid_angle;
}

vec3 sphere::random(const point3& o) const {
     vec3 direction = center - o;
     auto distance_squared = direction.length_squared();
     onb uvw;
     uvw.build_from_w(direction);
     return uvw.local(random_to_sphere(radius, distance_squared));
}

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
    get_sphere_uv(out_n, r_hit.u, r_hit.v);
    r_hit.mat_ptr = mat_ptr;
    
    return true;
};

bool sphere::bounding_box(double time0, double time1, aabb& output_box) const {
    output_box = aabb(
        center - vec3(radius, radius, radius),
        center + vec3(radius, radius, radius));
    return true;
};
#endif