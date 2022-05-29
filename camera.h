#ifndef CAMERA_H
#define CAMERA_H

#include "utils.h"

class camera {
    public: 
        camera() {
            auto aspect_ratio = 16.0/9.0;

            auto viewport_height = 2.0;
            auto viewport_width = aspect_ratio * viewport_height;
            auto focal_length = 1.0; //odleglosc obserwatora od viewportu

            origin = point3(0,0,0); // środek viewportu
            horizontal = vec3(viewport_width, 0.0, 0.0); //szerokość viewportu
            vertical = vec3(0.0, viewport_height, 0.0); //wysokość viewportu
            lower_left_corner = origin - horizontal/2 - vertical/2 - vec3(0, 0, focal_length); // początek układu współrzędnych viewportu
        }

        ray cast_ray(double u, double v) const { //u, v współrzędne viewportu
            return ray(origin, lower_left_corner + u*horizontal + v*vertical - origin);
        }

        private:
        point3 origin;
        point3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
};


#endif