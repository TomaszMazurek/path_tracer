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
        virtual double pdf_value(const point3& o, const vec3& v) const {
            return 0.0;
        }

        virtual vec3 random(const vec3& o) const {
            return vec3(1, 0, 0);
        }

};

class translate : public object3d {
    public:
        translate(shared_ptr<object3d> p, const vec3& displacement)
            : ptr(p), offset(displacement) {}

        virtual bool hit(
            const ray& r, double t_min, double t_max, ray_hit_point& hit) const override;

        virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;

    public:
        shared_ptr<object3d> ptr;
        vec3 offset;
};

bool translate::hit(const ray& r, double t_min, double t_max, ray_hit_point& hit) const {
    ray moved_r(r.origin() - offset, r.direction(), r.time());
    if (!ptr->hit(moved_r, t_min, t_max, hit))
        return false;

    hit.p += offset;
    hit.set_normal_dir(moved_r, hit.normal);

    return true;
}

bool translate::bounding_box(double time0, double time1, aabb& output_box) const {
    if (!ptr->bounding_box(time0, time1, output_box))
        return false;

    output_box = aabb(
        output_box.min() + offset,
        output_box.max() + offset);

    return true;
}

class rotate_y : public object3d {
    public:
        rotate_y(shared_ptr<object3d> p, double angle);

        virtual bool hit(
            const ray& r, double t_min, double t_max, ray_hit_point& hit) const override;

        virtual bool bounding_box(double time0, double time1, aabb& output_box) const override {
            output_box = bbox;
            return hasbox;
        }

    public:
        shared_ptr<object3d> ptr;
        double sin_theta;
        double cos_theta;
        bool hasbox;
        aabb bbox;
};

rotate_y::rotate_y(shared_ptr<object3d> p, double angle) : ptr(p) {
    auto radians = degrees_to_radians(angle);
    sin_theta = sin(radians);
    cos_theta = cos(radians);
    hasbox = ptr->bounding_box(0, 1, bbox);

    point3 min( inf,  inf,  inf);
    point3 max(-inf, -inf, -inf);

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                auto x = i*bbox.max().x() + (1-i)*bbox.min().x();
                auto y = j*bbox.max().y() + (1-j)*bbox.min().y();
                auto z = k*bbox.max().z() + (1-k)*bbox.min().z();

                auto newx =  cos_theta*x + sin_theta*z;
                auto newz = -sin_theta*x + cos_theta*z;

                vec3 tester(newx, y, newz);

                for (int c = 0; c < 3; c++) {
                    min[c] = fmin(min[c], tester[c]);
                    max[c] = fmax(max[c], tester[c]);
                }
            }
        }
    }

    bbox = aabb(min, max);
}

bool rotate_y::hit(const ray& r, double t_min, double t_max, ray_hit_point& hit) const {
    auto origin = r.origin();
    auto direction = r.direction();

    origin[0] = cos_theta*r.origin()[0] - sin_theta*r.origin()[2];
    origin[2] = sin_theta*r.origin()[0] + cos_theta*r.origin()[2];

    direction[0] = cos_theta*r.direction()[0] - sin_theta*r.direction()[2];
    direction[2] = sin_theta*r.direction()[0] + cos_theta*r.direction()[2];

    ray rotated_r(origin, direction, r.time());

    if (!ptr->hit(rotated_r, t_min, t_max, hit))
        return false;

    auto p = hit.p;
    auto normal = hit.normal;

    p[0] =  cos_theta*hit.p[0] + sin_theta*hit.p[2];
    p[2] = -sin_theta*hit.p[0] + cos_theta*hit.p[2];

    normal[0] =  cos_theta*hit.normal[0] + sin_theta*hit.normal[2];
    normal[2] = -sin_theta*hit.normal[0] + cos_theta*hit.normal[2];

    hit.p = p;
    hit.set_normal_dir(rotated_r, normal);

    return true;
}

class flip_face : public object3d {
    public:
        flip_face(shared_ptr<object3d> p) : ptr(p) {}

        virtual bool hit(
            const ray& r, double t_min, double t_max, ray_hit_point& hit) const override {

            if (!ptr->hit(r, t_min, t_max, hit))
                return false;

            hit.front_face = !hit.front_face;
            return true;
        }

        virtual bool bounding_box(double time0, double time1, aabb& output_box) const override {
            return ptr->bounding_box(time0, time1, output_box);
        }

    public:
        shared_ptr<object3d> ptr;
};

#endif  