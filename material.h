#ifndef MATERIAL_H
#define MATERIAL_H

#include "utils.h"
#include "texture.h"

struct ray_hit_point;

class material {
    public:
        virtual color emitted(double u, double v, const point3& p) const {
            return color(0,0,0);
        }
        virtual bool scatter(const ray& r_in, const ray_hit_point& hit, color& attenuation, ray& scattered, double& pdf
        ) const {
            return false;
        };
        
        virtual double scattering_pdf(
            const ray& r_in, const ray_hit_point& hit, const ray& scattered
        ) const {
            return 0;
        }
};

class lambertian : public material {
    public:
        lambertian(const color& a) : albedo(make_shared<solid_color>(a)) {}
        lambertian(shared_ptr<texture> a) : albedo(a) {}

        bool scatter(
            const ray& r_in, const ray_hit_point& hit, color& attenuation, ray& scattered, double& pdf
        ) const {
            auto scatter_direction = hit.normal + random_unit_hit_on_sphere();

            // Wyłapuje  zdegenerowane kierunki odbicia
            if (scatter_direction.near_zero())
                scatter_direction = hit.normal;

            scattered = ray(hit.p, scatter_direction, r_in.time());
            attenuation = albedo->value(hit.u, hit.v, hit.p);
            pdf = dot(hit.normal, scattered.direction()) / pi;
            return true;
        }
        double scattering_pdf(
            const ray& r_in, const ray_hit_point& hit, const ray& scattered
        ) const {
            auto cosine = dot(hit.normal, unit_vector(scattered.direction()));
            return cosine < 0 ? 0 : cosine/pi;
        }

    public:
    shared_ptr<texture> albedo;

};

class metal : public material {
    public:
        metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

       bool scatter(
            const ray& r_in, const ray_hit_point& hit, color& attenuation, ray& scattered
        ) const {
            vec3 reflected = reflect(unit_vector(r_in.direction()), hit.normal);
            scattered = ray(hit.p, reflected + fuzz*random_unit_hit_on_sphere(), r_in.time());
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
            double cos_theta = fmin(dot(-unit_direction, hit.normal), 1.0);
            double sin_theta = sqrt(1.0 - cos_theta*cos_theta);

            bool cannot_refract = refraction_ratio * sin_theta > 1.0;
            vec3 direction;

            if (cannot_refract || reflectance(cos_theta, refraction_ratio) > rng())
                direction = reflect(unit_direction, hit.normal);
            else
                direction = refract(unit_direction, hit.normal, refraction_ratio);

            scattered = ray(hit.p, direction, r_in.time());
            return true;
        }

    public:
        double ir; // współczynnik załamania

    private:
        static double reflectance(double cosine, double ref_idx) {
            // Aproksymacja Schlicka dla odbicia.
            auto r0 = (1-ref_idx) / (1+ref_idx);
            r0 = r0*r0;
            return r0 + (1-r0)*pow((1 - cosine),5);
        }
};

class diffuse_light : public material  {
    public:
        diffuse_light(shared_ptr<texture> a) : emit(a) {}
        diffuse_light(color c) : emit(make_shared<solid_color>(c)) {}

        bool scatter(
            const ray& r_in, const ray_hit_point& hit, color& attenuation, ray& scattered
        ) const {
            return false;
        }

        virtual color emitted(double u, double v, const point3& p) const override {
            return emit->value(u, v, p);
        }

    public:
        shared_ptr<texture> emit;
};

class isotropic : public material {
    public:
        isotropic(color c) : albedo(make_shared<solid_color>(c)) {}
        isotropic(shared_ptr<texture> a) : albedo(a) {}

        bool scatter(
            const ray& r_in, const ray_hit_point& hit, color& attenuation, ray& scattered
        ) const {
            scattered = ray(hit.p, random_unit_hit_on_sphere(), r_in.time());
            attenuation = albedo->value(hit.u, hit.v, hit.p);
            return true;
        }

    public:
        shared_ptr<texture> albedo;
};

#endif