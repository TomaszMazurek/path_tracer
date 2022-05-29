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

#endif