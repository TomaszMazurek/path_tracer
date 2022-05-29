#ifndef CAMERA_H
#define CAMERA_H

#include "utils.h"

class camera {
    public: 
        camera(
            point3 lookfrom,
            point3 lookat,
            vec3   vup,
            double vfov, //wertykalne pole widzenia
            double aspect_ratio
            ) {
            auto theta = degrees_to_radians(vfov);
            auto h = tan(theta/2);
            auto viewport_height = 2.0 * h;
            auto viewport_width = aspect_ratio * viewport_height;
            
            auto w = unit_vector(lookfrom - lookat);
            auto u = unit_vector(cross(vup, w));
            auto v = cross(w, u);

            origin = lookfrom; // środek viewportu
            horizontal = viewport_width * u; //szerokość viewportu
            vertical = viewport_height * v; //wysokość viewportu
            lower_left_corner = origin - horizontal/2 - vertical/2 - w; // początek układu współrzędnych viewportu
        }

        ray cast_ray(double s, double t) const { //s, t 
            return ray(origin, lower_left_corner + s*horizontal + t*vertical - origin);
        }

        private:
        point3 origin;
        point3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
};


#endif