#ifndef OBJECT3D_LIST_H
#define OBJECT3D_LIST_H

#include "object3d.h"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

class object3d_list : public object3d {
    public:
        object3d_list() {};
        object3d_list(shared_ptr<object3d> obj) { add(obj); }

        void clear() { objects.clear(); }
        void add(shared_ptr<object3d> obj) { objects.push_back(obj); }

        virtual bool hit(const ray& r, double t_min, double t_max, ray_hit_point& r_hit) const override;
        virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;
        virtual double pdf_value(const vec3 &o, const vec3 &v) const override;
        virtual vec3 random(const vec3 &o) const override;

    public:
        std::vector<shared_ptr<object3d>> objects;

};

bool object3d_list::hit(const ray& r, double t_min, double t_max, ray_hit_point& r_hit) const {
    ray_hit_point cur_hit;
    bool hit_any = false;
    auto closest = t_max;

    for(const auto& obj: objects) {
        if(obj->hit(r, t_min, closest, cur_hit)) {
            hit_any = true;
            closest = cur_hit.t;
            r_hit = cur_hit;
        }
    }
    return hit_any;
}

bool object3d_list::bounding_box(double time0, double time1, aabb& output_box) const {
    if (objects.empty()) return false;

    aabb temp_box;
    bool first_box = true;

    for (const auto& object : objects) {
        if (!object->bounding_box(time0, time1, temp_box)) return false;
        output_box = first_box ? temp_box : surrounding_box(output_box, temp_box);
        first_box = false;
    }

    return true;
}

double object3d_list::pdf_value(const point3& o, const vec3& v) const {
    auto weight = 1.0/objects.size();
    auto sum = 0.0;

    for (const auto& object : objects)
        sum += weight * object->pdf_value(o, v);

    return sum;
}

vec3 object3d_list::random(const vec3& o) const {
    auto int_size = static_cast<int>(objects.size());
    return objects[random_int(0, int_size-1)]->random(o);
}

#endif