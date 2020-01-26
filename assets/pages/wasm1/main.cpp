#include <iostream>
#include <math.h>
#include <float.h>
#include <random>
#include <memory>
#define MAXFLOAT 3.40282347e+38F
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "material.hpp"
#include "sphere.hpp"
#include "camera.hpp"
#include "hitable_list.hpp"
#include "common.hpp"

#include <cstdint>

#ifndef QUALITY
#define QUALITY 2 
#endif

#ifndef BOUNCES
#define BOUNCES 40
#endif

#ifndef FACTOR 
#define FACTOR 2 
#endif

vec3<float> color(ray<float>& r, hitable<float> *world, int depth){
	hit_record<float> rec;
	if (world->hit(r, 0.001, MAXFLOAT,rec)) {
		ray<float> scattered;
		vec3<float> attenuation;

		if(depth < BOUNCES && rec.mat_ptr->scatter(r, rec, attenuation, scattered)){
			
			return attenuation*color(scattered, world, depth+1);
		}
		else {
			return vec3<float>(0.0, 0.0, 0.0);
		}
	}
	else {
		vec3<float> unit_direction = unit_vector(r.direction());
		float t = 0.5 *(unit_direction.y() + 1.0);
		return (float)(1.0-t)*vec3<float>(1.0,1.0,1.0) + t*vec3<float>(0.5,0.7,1.0);
	}
}

void write_image(uint8_t* image, int x, int y) {

	std::cout << "P3\n" << x << " " << y << "\n255\n";
	for (int j = y-1 ; j >= 0 ; j--){
	    for (int i = 0 ; i < x ; i++){
		uint8_t ir = image[j+i*y+0];
		uint8_t ig = image[j+i*y+1];
		uint8_t ib = image[j+i*y+2] ;
		uint8_t ia = 255;
		std::cout << unsigned(ir) << " " << unsigned(ig) << " " << unsigned(ib) << "\n";
	    }
	}
	std::cout << "\n";
}

extern "C" {
void return_image(vec3<uint8_t>* image, int x, int y, uint8_t* dest) {

	//std::cout << "P3\n" << x << " " << y << "\n255\n";
	for (int j = y-1 ; j >= 0 ; j--){
	    for (int i = 0 ; i < x ; i++){
		//int idx = (3*y*j+i*3);
			int idx = (3*x*j+i*3);
		dest[idx+0] = image[idx][0]  ;
		dest[idx+1] = image[idx][1]  ;
		dest[idx+2] = image[idx][2]  ;
		//std::cout << dest[j+i*y] << " " << dest[j+i*y+1] << " " << dest[j+i*y+2] << "\n";
	    }
	}
	//std::cout << "\n";
}
}
void write_image2(int x, int y, uint8_t* dest) {

	std::cout << "P3\n" << x << " " << y << "\n255\n";
	for (int j = y-1 ; j >= 0 ; j--){
	    for (int i = 0 ; i < x ; i++){
		/*
		dest[j+i*y] = (uint8_t)image[j+i*y].r() ;
		dest[j+i*y+1] = (uint8_t)image[j+i*y].g() ;
		dest[j+i*y+2] = (uint8_t)image[j+i*y].b() ;
		*/
		//int idx = (3*y*j+i*3);
			//int idx = (4*x*j+i*4);
			int idx = ((4*x*j) + (i*4));
		std::cout << unsigned(dest[idx+0]) << " " << unsigned(dest[idx+1]) << " " << unsigned(dest[idx+2]) << "\n";
	    }
	}
	std::cout << "\n";
}

//uint8_t* dest;

const float factor = FACTOR;
const int nx = int(200 * factor);
const int ny = int(100 * factor);

extern "C" {
    int d(){
		std::cerr << "nx=" << nx << " ny=" << ny << " uint8_t=" << sizeof(uint8_t) << "\n";
		return sizeof(uint8_t)*(nx*ny*4);
    }

	uint8_t e(uint8_t* dest, int i, int j, int c) {
			int idx = (3*nx*j+i*3)+c;
			std::cout << i << " " << j << " " << c << " " <<  "\n";
			std::cout << idx << ", " << dest << "\n";
			std::cout << *(dest+idx) << "\n";
			std::cout << dest[idx] << "\n";
			std::cout << "----------" << "\n";
		return dest[idx]; 
	}
}

extern "C" {
    int width(){
	return nx;
    }
}
extern "C" {
    int height(){
	return ny;
    }
}
int count = 0;

extern "C"{
int render(uint8_t* dest){
	const float fnx = (float)nx;
	const float fny = (float)ny;
	const int ns = QUALITY * factor * 8 ;
	//std::cout << "P3\n" << nx << " " << ny << "\n255\n";
#ifdef __EMSCRIPTEN__
	std::cout << "nx " << fnx << " ny " << fny << " ns " <<  ns << " quality " << QUALITY << " factor " << factor << " \n";
#endif

	hitable<float> *list[6];

	auto mat1 = new lambertian<float>(vec3<float>(0.8,0.3,0.3));
	auto mat2 = new lambertian<float>(vec3<float>(0.8,0.8,0.0));

	auto mat3 = new metal<float>(vec3<float>(0.8,0.6,0.2), 0.2);
	//auto mat4 = new metal<float>(vec3<float>(0.8,0.8,0.8), 0.1);
	auto mat5 = new dieletric<float>(1.3);
	auto mat6 = new dieletric<float>(0.51);
	auto mat7 = new lambertian<float>(vec3<float>(0.8,0.8,0.8));

	//float R = cos(M_PI/4);

	list[0] = new sphere<float>(vec3<float>(0,0,0), 0.5, mat1);
	list[1] = new sphere<float>(vec3<float>(0,-100.5,0), 100, mat2);
	list[2] = new sphere<float>(vec3<float>(+1,0,0), 0.5, mat3);
	list[3] = new sphere<float>(vec3<float>(-1,0,0), 0.5, mat5);
	list[4] = new sphere<float>(vec3<float>(-1,0,0), -0.48, mat6);
	list[5] = new sphere<float>(vec3<float>(10,50.5,130.5), 100, mat7);

	hitable<float> *world = new hitable_list<float>(list,6);

	vec3<float> lookfrom = vec3<float>(-4.0,0.5,-2.8);
	vec3<float> lookat = vec3<float>(0,0,0);
	camera<float> cam(lookfrom,             // lookfrom
			  lookat,               //lookat
			  vec3<float>(0,1,0),   //view-up
			  20,                   //vfov
			  float(nx)/float(ny),  //aspect
			  0.3,                  //aperture
			  (lookfrom-lookat).length() //dist_to_focus
		     );



	srand(time(0));
	for(int j = 0 ; j < ny ; j++){
		for(int i = 0; i < nx; i++) {
			vec3<float> col(0,0,0);

				// this could be better if not setting atomic write
				// it could an buffer array for each thread and then a reduction
				// maybe it's faster
				// but it would some work which I'm not going into it
				#pragma num_threads(6)
				#pragma omp parallel for schedule(dynamic)
				for(int s = 0; s < ns; s++) {
					auto u = float(i + fRand()) / fnx;
					auto v = float(j + fRand()) / fny;

					ray<float> r = cam.get_ray(u,v);
                                        #pragma atomic write
					col += color(r, world,0);
				}
			col /= float(ns);

			uint8_t ir = uint8_t(255.99*col[0]);
			uint8_t ig = uint8_t(255.99*col[1]);
			uint8_t ib = uint8_t(255.99*col[2]);


			/*
			[ 0,1,2 3,4,5 6,7,8
			  9,0,1 2,3,4 5,6,7 cores * colunas * j + i
			  8,9,0 1,2,3 4,5,6
			  7,8,9 0,1,2 3,4,5 ] */

			//std::cout << ir << " " << ig << " " << ib << "\n";

#ifdef __EMSCRIPTEN__
			int idx = ((4*nx*((ny-1)-j)) + (i*4))-4;
			if (i==0&&j==0) std::cout << "idx " << idx << std::endl;
#else
			int idx = ((4*nx*j) + (i*4));
#endif
			dest[idx++]=ir;	
			dest[idx++]=ig;	
			dest[idx++]=ib;	
			dest[idx++]=255;	
#ifdef __EMSCRIPTEN__
			if (i==0&&j==0) std::cout << "idxe " << idx << std::endl;
#endif
		#pragma omp barrier
		float stat = (float)(-j+ny)/(float)ny ;
		std::cerr << (ny-j) << " " << stat << '\r';
	}
	}
	/*
#ifdef __EMSCRIPTEN__
			std::cout << "idx = " << idx << std::endl;
#endif
*/
	
#ifndef __EMSCRIPTEN__
	write_image2(nx, ny,dest);
#else
	std::cout << "finished img\n";
#endif
	return 0 ;
}
}//extern

int main(){
#ifndef __EMSCRIPTEN__
	//uint8_t* dest = (uint8_t*) malloc(sizeof(uint8_t)*nx*ny);
	d();
	auto dest = new uint8_t[sizeof(uint8_t)*nx*ny*4];
	render(dest);
#else
	std::cout << "ready\n";
#endif
	return 0;

}
