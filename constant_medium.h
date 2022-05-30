#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "utils.h"

#include "object3d/object3d.h"
#include "material.h"
#include "texture.h"

class constant_medium : public object3d {
    public:
        constant_medium(shared_ptr<object3d> b, double d, shared_ptr<texture> a)
            : boundary(b),
              neg_inv_density(-1/d),
              phase_function(make_shared<isotropic>(a))
            {}

        constant_medium(shared_ptr<object3d> b, double d, color c)
            : boundary(b),
              neg_inv_density(-1/d),
              phase_function(make_shared<isotropic>(c))
            {}

        virtual bool hit(
            const ray& r, double t_min, double t_max, ray_hit_point& hit) const override;

        virtual bool bounding_box(double time0, double time1, aabb& output_box) const override {
            return boundary->bounding_box(time0, time1, output_box);
        }

    public:
        shared_ptr<object3d> boundary;
        shared_ptr<material> phase_function;
        double neg_inv_density;
};

bool constant_medium::hit(const ray& r, double t_min, double t_max, ray_hit_point& hit) const {
    // Print occasional samples when debugging. To enable, set enableDebug true.
    const bool enableDebug = false;
    const bool debugging = enableDebug && rng() < 0.00001;

    ray_hit_point hit1, hit2;

    if (!boundary->hit(r, -inf, inf, hit1))
        return false;

    if (!boundary->hit(r, hit1.t+0.0001, inf, hit2))
        return false;

    if (debugging) std::cerr << "\nt_min=" << hit1.t << ", t_max=" << hit2.t << '\n';

    if (hit1.t < t_min) hit1.t = t_min;
    if (hit2.t > t_max) hit2.t = t_max;

    if (hit1.t >= hit2.t)
        return false;

    if (hit1.t < 0)
        hit1.t = 0;

    const auto ray_length = r.direction().length();
    const auto distance_inside_boundary = (hit2.t - hit1.t) * ray_length;
    const auto hit_distance = neg_inv_density * log(rng());

    if (hit_distance > distance_inside_boundary)
        return false;

    hit.t = hit1.t + hit_distance / ray_length;
    hit.p = r.at(hit.t);

    if (debugging) {
        std::cerr << "hit_distance = " <<  hit_distance << '\n'
                  << "hit.t = " <<  hit.t << '\n'
                  << "hit.p = " <<  hit.p << '\n';
    }

    hit.normal = vec3(1,0,0);  // arbitrary
    hit.front_face = true;     // also arbitrary
    hit.mat_ptr = phase_function;

    return true;
}

#endif