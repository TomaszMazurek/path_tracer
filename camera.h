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
            double aspect_ratio,
            double aperture,
            double focus_dist, // depth of field 
            double _time0 = 0,
            double _time1 = 0
            ) {
            auto theta = degrees_to_radians(vfov);
            auto h = tan(theta/2);
            auto viewport_height = 2.0 * h;
            auto viewport_width = aspect_ratio * viewport_height;
            
            w = unit_vector(lookfrom - lookat);
            u = unit_vector(cross(vup, w));
            v = cross(w, u);

            origin = lookfrom; // środek viewportu
            horizontal = focus_dist * viewport_width * u; //szerokość viewportu
            vertical = focus_dist * viewport_height * v; //wysokość viewportu
            lower_left_corner = origin - horizontal/2 - vertical/2 - focus_dist*w; // początek układu współrzędnych viewportu

            lens_radius = aperture / 2;
            time0 = _time0;
            time1 = _time1; 
        }

        ray cast_ray(double s, double t) const { //s, t 
            vec3 rd = lens_radius * random_in_unit_disk();
            vec3 offset = u * rd.x() + v * rd.y();
            
            return ray(
                origin + offset, 
                lower_left_corner + s*horizontal + t*vertical - origin - offset,
                rng(time0, time1));
        }

        private:
        point3 origin;
        point3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
        vec3 u, v, w;
        double lens_radius;
        double time0, time1;  // czas otwarcia i zamkniecia "przeslony"

};


#endif