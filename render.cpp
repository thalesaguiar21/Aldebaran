#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "utils/vec3.h"
#include "utils/ray.h"
#include "utils/image.h"
#include "utils/file_reader.h"

using namespace utility;
using std::string;

/*
 *  In the near future we'll want to refactor our project to become
 *  something like the code below.
 */
#ifdef TRADITIONAL_RT
Image Raytrace (Camera cam, Scene scene, int width, int height)
{
    Image image = new Image (width, height) ;
    for (int i = 0 ; i < height ; i++)
        for (int j = 0 ; j < width ; j++) {
            Ray ray = RayThruPixel (cam, i, j) ;
            Intersection hit = Intersect (ray, scene) ;
            image[i][j] = FindColor (hit) ;
            }
    return image ;
}
#endif

bool hit_sphere(const point3 & center, float radius, const Ray & ray){

    vec3 oc = ray.get_origin() - center;
    float A = dot(ray.get_direction(), ray.get_direction());
    float B = 2.0 * dot(oc, ray.get_direction());
    float C = dot(oc, oc) - (radius * radius);

    float delta = (B * B) - 4 * A * C;
    return delta > 0;
}

Ray hit_sphere_with_light(const point3 & center, float radius, const Ray & ray, bool root1 = false) {
    
    Ray result;
    vec3 oc = ray.get_origin() - center;
    float A = dot(ray.get_direction(), ray.get_direction());
    float B = 2.0 * dot(oc, ray.get_direction());
    float C = dot(oc, oc) - (radius * radius);
    
    float delta = (B * B) - 4 * A * C;
    if (delta > 0){
        float t1 = (-B + sqrt(delta)) / (2.0 * A);
        float t2 = (-B - sqrt(delta)) / (2.0 * A);
        float max = (root1) ? (t1) : (t2);
        vec3 unit = unit_vector(ray.point_at(max) - center);
        vec3 normal = 0.5 * vec3(unit.x()+1, unit.y()+1, unit.z()+1);
        result = Ray (center, normal);
    }
    return result;
}


rgb color( const Ray & r_, Image img)
{

    point3 centerA = point3 (0, 0, -2.0);
    float radiusA = 0.5;

    point3 centerB = point3 (0.3, 0.3, -3.0);
    float radiusB = 0.7;

    point3 centerC = point3 (-0.8, 0.3, -4.0);
    float radiusC = 0.9;

    rgb color;

    if (hit_sphere(centerA, radiusA, r_)) {
        Ray norm = hit_sphere_with_light(centerA, radiusA, r_, true);
        color = norm.get_direction();
    } else if (hit_sphere(centerB, radiusB, r_)) {
        Ray norm = hit_sphere_with_light(centerB, radiusB, r_);
        color = norm.get_direction();
    } else if (hit_sphere(centerC, radiusC, r_)) {
        Ray norm = hit_sphere_with_light(centerC, radiusC, r_);
        color = norm.get_direction();
    } else {
        auto unit = utility::unit_vector( r_.get_direction() );

        auto tx = (unit.x() + 1.0) / 2.0;
        auto ty = (unit.y() + 1.0) / 2.0;

        rgb upper = (1 - tx) * img.upper_left + tx * img.upper_right;
        rgb lower = (1 - tx) * img.lower_left + tx * img.lower_right;
        color = (1 - ty) * lower + ty * upper;
    } 

    return color;
}

int main( int argc, char *argv[]  )
{
    std::ofstream myfile;

    if(argc < 2){
        std::cout << "No input file name was given! Closing the program." <<std::endl;
        return 1;
    } else {
        std::vector<string> content = utility::read_file(argv[1]);
        Image img;
        img.from(content);
        std::ofstream imgFile("imgs/" + img.name);

        imgFile << "P3" << "\n";
        imgFile << img.width << " " << img.height << "\n";
        imgFile << "255\n";

        //=== Defining our 'camera'
        point3 lower_left_corner( -2, -1, -1 ); // lower left corner of the view plane.
        vec3 horizontal( 4, 0, 0 ); // Horizontal dimension of the view plane.
        vec3 vertical(0, 2, 0); // Vertical dimension of the view plane.
        point3 origin(0, 0, 0); // the camera's origin.

         // NOTICE: We loop rows from bottom to top.
        for ( auto row = img.height - 1 ; row >= 0 ; --row ) // Y
        {
            for( auto col = 0 ; col < img.width ; col++ ) // X
            {
                auto u = float(col) / float(img.width); 
                auto v = float(row) / float(img.height); 
                point3 end_point = lower_left_corner + u*horizontal + v*vertical ;
                Ray r( origin, end_point - origin );

                rgb c = color (r, img);

                int ir = int( 255.99f * c[rgb::R] );
                int ig = int( 255.99f * c[rgb::G] );
                int ib = int( 255.99f * c[rgb::B] );

                imgFile << ir << " " << ig << " " << ib << "\n";
            }
        }
    }

    return 0;
}
