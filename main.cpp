#include "color.h"
#include "vec3.h"
#include "ray.h"

#include <iostream>

double hit_sphere(const point3& center, double radius, const ray& r) {
    vec3 oc = r.origin() - center; // origin center - środek układu współrzędnych
    auto a = dot(r.direction(), r.direction()); // A we funkcji liniowej długości promienia 
    auto b = 2.0 * dot(oc, r.direction()); // kierunek padania promienia
    auto c = dot(oc, oc) - radius*radius; // sfera
    auto discriminant = b*b -4*a*c; // pierwiastek równania kwadratowego dla przecięcia sfery przez promień
    if (discriminant < 0){ //jesli pierwiastek równania przecięcia promienia i sfery jest ujemny
        return -1.0;
    } else {
        return (-b - sqrt(discriminant)/2.0 * a); // zwracana jest wartosc pierwiastka
    }

}

color ray_color(const ray& r) {
    auto t = (hit_sphere(point3(0,0,-1), 0.5, r)); // wartość t dla przecięcia sfery przez promień liczac od obserwatora
    if (t > 0.0) {
        vec3 N = unit_vector(r.at(t) - vec3(0,0,-1));
        return 0.5*color(N.x()+1, N.y()+1, N.z()+1);
    }
    vec3 unit_direction = unit_vector(r.direction());
    t = 0.5*(unit_direction.y() + 1.0);
    return  (1.0-t)* color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0); 
}

int main() {

    //Image 
    const auto aspect_ratio = 16.0 / 9.0;    
    const int image_width = 1024;
    const int image_height = static_cast<int>(image_width/ aspect_ratio);   

    //Camera
    auto viewport_height = 2.0;
    auto viewport_width = aspect_ratio * viewport_height;
    auto focal_length = 1.0; //odleglosc obserwatora od viewportu

    auto origin = point3(0,0,0); // środek viewportu
    auto horizontal = vec3(viewport_width, 0, 0); //szerokość viewportu
    auto vertical = vec3(0, viewport_height, 0); //wysokość viewportu
    auto lower_left_corner = origin - horizontal/2 - vertical/2 - vec3(0, 0, focal_length); // początek układu współrzędnych viewportu


    //Render
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height -1; j >=0 ; --j){
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width ; ++i){
            auto u = double(i) / (image_width-1); //współrzędna horyzontalna u
            auto v = double(j) / (image_height-1); //współrzędna wertykalna v
            ray r(origin, lower_left_corner + u*horizontal + v*vertical - origin); // ustawienie kierunku promienia względem (piksela) viewportu  
            color pixel_color = ray_color(r); //wyznaczenie koloru w punkcie przecięcia promienia z (pikselem) vieportem
            write_color(std::cout, pixel_color);
        }
    }
    
    std::cerr << "\nDone.\n";
}