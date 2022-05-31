#include <iostream>
#include <ctime>
#include "utils.h"
#include "color.h"
#include "object3d/object3d_list.h"
#include "sphere/sphere.h"
#include "camera.h"
#include "material.h"
#include "moving_sphere.h"
#include "aarect.h"
#include "box.h"
#include "constant_medium.h"
#include "object3d/bvh.h"
#include "pdf.h"

color ray_color(const ray& r, const color& background, const object3d& world, shared_ptr<object3d>& lights, int depth) {
    ray_hit_point hit;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0,0,0);

    // If the ray hits nothing, return the background color.
    if (!world.hit(r, 0.001, inf, hit)){
    //std::cerr << "\rray color hits nothing: " << std::flush;
        return background;
    }

    scatter_record srec;
    color emitted = hit.mat_ptr->emitted(r, hit, hit.u, hit.v, hit.p);

    if (!hit.mat_ptr->scatter(r, hit, srec)) {
        //std::cerr << "\rray color hits material emitter: " << std::flush;
        return emitted;
    }

    if (srec.is_specular) {
        return srec.attenuation
             * ray_color(srec.specular_ray, background, world, lights, depth-1);
    }

    auto light_ptr =make_shared<object3d_pdf>(lights, hit.p);
    mixture_pdf mixed_pdf(light_ptr, srec.pdf_ptr);
    ray scattered = ray(hit.p, mixed_pdf.generate(), r.time());
    auto pdf = mixed_pdf.value(scattered.direction());
    //std::cerr << "\rray color hits material diffuse: " << std::flush;
    return emitted
         + srec.attenuation * hit.mat_ptr->scattering_pdf(r, hit, scattered)
                  * ray_color(scattered, background, world, lights, depth-1) / pdf;
}

object3d_list cornell_box() {
    object3d_list objects;

    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    //light
    objects.add(make_shared<flip_face>(make_shared<xz_rect>(213, 343, 227, 332, 554, light)));

    //walls
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

    //boxes
    shared_ptr<material> aluminum = make_shared<metal>(color(0.8, 0.85, 0.88), 0.0);
    shared_ptr<object3d> box1 = make_shared<box>(point3(0,0,0), point3(165,330,165), aluminum);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265,0,295));
    objects.add(box1);

    auto glass = make_shared<dielectric>(1.5);
    objects.add(make_shared<sphere>(point3(190,90,190), 90 , glass));

    //shared_ptr<object3d> box2 = make_shared<box>(point3(0,0,0), point3(165,165,165), white);
    //box2 = make_shared<rotate_y>(box2, -18);
    //box2 = make_shared<translate>(box2, vec3(130,0,65));
    //objects.add(box2);
    return objects;
}

int main() {

    //Image 
    auto aspect_ratio = 1.0 / 1.0;
    int image_width = 600;
    int image_height = static_cast<int>(image_width / aspect_ratio);
    int samples_per_pixel = 100;
    const int max_depth = 50;



    point3 lookfrom = point3(278, 278, -800);
    point3 lookat = point3(278, 278, 0);
    auto vfov = 40.0;
    auto aperture = 0.0;
    color background(0,0,0);

    //World
    object3d_list world = cornell_box();
//    shared_ptr<object3d> lights = make_shared<xz_rect>(213, 343, 227, 332, 554, shared_ptr<material>());
    shared_ptr<object3d> lights = make_shared<sphere>(point3(190, 90, 190), 90, shared_ptr<material>());
    // Camera
    vec3 vup(0,1,0);
    auto dist_to_focus = 10.0;

    //camera
    camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

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
                pixel_color += ray_color(r, background, world, lights, max_depth); //wyznaczenie koloru w punkcie przecięcia promienia z (pikselem) vieportem
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }

    std::cerr << "\nDone in " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << "s\n";
}