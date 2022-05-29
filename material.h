#ifndef MATERIAL_H
#define MATERIAL_H

#include "utils.h"

struct ray_hit_point;

class material {
    public:
        virtual bool scatter(const ray& r_in, const ray_hit_point& hit, color& attenuation, ray& scattered) const = 0;
};

class lambertian : public material {
    public:
        lambertian(const color& a) : albedo(a) {}

        bool scatter(
            const ray& r_in, const ray_hit_point& hit, color& attenuation, ray& scattered
        ) const {
            auto scatter_direction = hit.normal + random_unit_hit_on_sphere();

            // Wyłapuje  zdegenerowane kierunki odbicia
            if (scatter_direction.near_zero())
                scatter_direction = hit.normal;

            scattered = ray(hit.p, scatter_direction);
            attenuation = albedo;
            return true;
        }

    public:
        color albedo;
};

class metal : public material {
    public:
        metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

       bool scatter(
            const ray& r_in, const ray_hit_point& hit, color& attenuation, ray& scattered
        ) const {
            vec3 reflected = reflect(unit_vector(r_in.direction()), hit.normal);
            scattered = ray(hit.p, reflected + fuzz*random_unit_hit_on_sphere());
            attenuation = albedo;
            return (dot(scattered.direction(), hit.normal) > 0);
        }

    public:
        color albedo;
        double fuzz;
};

#endif