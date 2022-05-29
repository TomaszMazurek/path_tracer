#include <iostream>
#include <ctime>
#include "utils.h"
#include "color.h"
#include "object3d/object3d_list.h"
#include "sphere/sphere.h"
#include "camera.h"


color ray_color(const ray& r, const object3d& world, int depth) {
    ray_hit_point hit;

    if (depth <= 0)
        return color(0,0,0);

    if (world.hit(r, 0.001, inf, hit)) {
        point3 target = hit.p + hit.normal + random_unit_hit_on_sphere();
        return 0.5 * ray_color(ray(hit.p, target - hit.p), world, depth-1);
    }
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5*(unit_direction.y() + 1.0);
    return  (1.0-t)* color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0); 
}

int main() {

    //Image 
    const auto aspect_ratio = 16.0 / 9.0;    
    const int image_width = 1024;
    const int image_height = static_cast<int>(image_width/ aspect_ratio);   
    const int samples_per_pixel = 100;
    const int max_depth = 50;

    //World
    object3d_list world;
    world.add(make_shared<sphere>(point3(0,0,-1), 0.5));
    world.add(make_shared<sphere>(point3(0,-100.5,-1), 100));

    //camera
    camera cam;

    //Render
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    
    const clock_t begin_time = clock();

    for (int j = image_height -1; j >=0 ; --j){
        //std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width ; ++i){
            color pixel_color(0,0,0);
            for (int s = 0; s < samples_per_pixel; ++s){
                auto u = (i + rng()) / (image_width-1); //współrzędna horyzontalna u
                auto v = (j + rng()) / (image_height-1); //współrzędna wertykalna v
                ray r = cam.cast_ray(u, v); // ustawienie kierunku promienia względem (piksela) viewportu
                pixel_color += ray_color(r, world, max_depth); //wyznaczenie koloru w punkcie przecięcia promienia z (pikselem) vieportem
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }

    std::cerr << "\nDone in " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << "s\n";
}