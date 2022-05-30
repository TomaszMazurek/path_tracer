#include <iostream>
#include <ctime>
#include "utils.h"
#include "color.h"
#include "object3d/object3d_list.h"
#include "sphere/sphere.h"
#include "camera.h"
#include "material.h"
#include "moving_sphere.h"

color ray_color(const ray& r, const object3d& world, int depth) {
    ray_hit_point hit;

    if (depth <= 0)
        return color(0,0,0);

    if (world.hit(r, 0.001, inf, hit)) {
        //point3 target = hit.p + random_unit_hit_on_hemisphere(hit.normal);
        //return 0.5 * ray_color(ray(hit.p, target - hit.p), world, depth-1);
        ray scattered;
        color attenuation;
        if (hit.mat_ptr->scatter(r, hit, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth-1);
        return color(0,0,0);
    }
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5*(unit_direction.y() + 1.0);
    return  (1.0-t) * color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

object3d_list random_scene() {
    object3d_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = rng();
            point3 center(a + 0.9*rng(), 0.2, b + 0.9*rng());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    auto center2 = center + vec3(0, rng(0, .5), 0);
                    world.add(make_shared<moving_sphere>(center,center2, 0.0, 1.0, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = rng(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}


int main() {

    //Image 
    //const auto aspect_ratio = 3.0 / 2.0;
    //const int image_width = 1200;
    //const int samples_per_pixel = 500;
    
    auto aspect_ratio = 16.0 / 9.0;
    int image_width = 400;
    int samples_per_pixel = 100;

    //const int image_height = static_cast<int>(image_width/ aspect_ratio);   
    int image_height = static_cast<int>(image_width / aspect_ratio);

    const int max_depth = 50;

    //World
    //auto R = cos(pi/4);
    auto world = random_scene();
    
    point3 lookfrom(13,2,3);
    point3 lookat(0,0,0);
    vec3 vup(0,1,0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;

    //auto material_ground = make_shared<lambertian>(color(0.2, 0.2, 0.2));
    //auto material_center = make_shared<lambertian>(color(0.7, 0.3, 0.5));
    //auto material_left   = make_shared<dielectric>(1.5);
    //auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

    //world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    //world.add(make_shared<sphere>(point3( 0.0,    0.0, -1.0),   0.5, material_center));
    //world.add(make_shared<sphere>(point3(-1.0,    0.0, -2.0),   0.5, material_left));
    //world.add(make_shared<sphere>(point3(-1.0,    0.5, -0.2), -0.4, material_left));
    //world.add(make_shared<sphere>(point3( 1.0,    0.0, -0.5),   0.5, material_right));

    //auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    //auto material_center = make_shared<lambertian>(color(0.7, 0.3, 0.3));
    //auto material_left   = make_shared<metal>(color(0.8, 0.8, 0.8), 0.3);
    //auto material_left   = make_shared<dielectric>(1.5);
    //auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

    //world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    //world.add(make_shared<sphere>(point3( 0.0,    0.0, -1.0),   0.5, material_center));
    //world.add(make_shared<sphere>(point3( R,    0.0, -1.3), R, material_left));
    //world.add(make_shared<sphere>(point3( -R,    0.0, -1.3), R, material_right));

    //auto material_left  = make_shared<lambertian>(color(0,0,1));
    //auto material_right = make_shared<lambertian>(color(1,0,0));

    //world.add(make_shared<sphere>(point3(-R, 0, -1), R, material_left));
    //world.add(make_shared<sphere>(point3( R, 0, -1), R, material_right));

    //camera
    //camera cam(90.0, aspect_ratio);
    //camera cam(point3(-2,2,1), point3(0,0,-1), vec3(0,1,0), 90, aspect_ratio);
    //camera cam(point3(-2,2,1), point3(0,0,-1), vec3(0,1,0), 20, aspect_ratio);

    //camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);
    camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

    //Render
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    
    const clock_t begin_time = clock();

    for (int j = image_height -1; j >=0 ; --j){
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
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