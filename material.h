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

class dielectric : public material {
    public:
        dielectric(double index_of_refraction) : ir(index_of_refraction) {}

       bool scatter(
            const ray& r_in, const ray_hit_point& hit, color& attenuation, ray& scattered
        ) const {
            attenuation = color(1.0, 1.0, 1.0);
            double refraction_ratio = hit.front_face ? (1.0/ir) : ir;

            vec3 unit_direction = unit_vector(r_in.direction());
            vec3 refracted = refract(unit_direction, hit.normal, refraction_ratio);

            scattered = ray(hit.p, refracted);
            return true;
        }

    public:
        double ir; // Index of Refraction
};

#endif