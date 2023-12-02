#include "raytracer.hpp"
#include "png++/png.hpp"
#include <cmath>
#include <omp.h>

using namespace std;

namespace raytracer
{
	double Vector3D::length() {return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));}
	Vector3D Vector3D::normalized() {return *this / length();}
	Vector3D operator+(Vector3D u, Vector3D v) {return Vector3D(u.x + v.x, u.y + v.y, u.z + v.z);}
	Vector3D operator-(Vector3D u, Vector3D v) {return Vector3D(u.x - v.x, u.y - v.y, u.z - v.z);}
	Vector3D operator*(double a, Vector3D u) {return Vector3D(a * u.x, a * u.y, a * u.z);}
	Vector3D operator*(Vector3D u, double a) {return Vector3D(a * u.x, a * u.y, a * u.z);}
	Vector3D operator/(Vector3D u, double a) {return Vector3D(u.x / a, u.y / a, u.z / a);}

	void Ray::step(double timestep) {position = position + timestep * velocity;}
	
	void Object::accelerate(Ray &ray, double timestep)
	{
		Vector3D new_velocity = ray.velocity + timestep * acceleration(ray.position);
		if(new_velocity.length() > 1)
			ray.velocity = new_velocity.normalized();
		else
			ray.velocity = new_velocity;
	}

	png::image<png::rgb_pixel> Camera::render(Vector3D position, double theta, double phi, double h_fov, double v_fov)
	{
		png::image<png::rgb_pixel> image(_width, _height);
		#pragma omp parallel for
		for(int y = 0; y < _height; y++)
		{
			for(int x = 0; x < _width; x++)
			{
				double ray_theta = theta - h_fov / 2 + (double)x / (_width - 1) * h_fov;
				double ray_phi = phi - v_fov / 2 + (double)y / (_height - 1) * v_fov;
				Ray ray(position, Vector3D(sin(ray_phi) * cos(ray_theta), sin(ray_phi) * sin(ray_theta), cos(ray_phi)));
				bool collided = false;
				for(double time = 0; time < _time_limit && !collided; time += _time_step)
				{
					for(Object object: _objects)
						object.accelerate(ray, _time_step);
					ray.step(_time_step);
					for(Object object: _objects)
					{
						if(object.inside(ray.position))
						{
							collided = true;
							image[y][x] = object.color;
							double endarkenment = max(1.0, pow((ray.position - position).length(), 2) / 100);
							image[y][x].red = (double)image[y][x].red / endarkenment;
							image[y][x].green = (double)image[y][x].green / endarkenment;
							image[y][x].blue = (double)image[y][x].blue / endarkenment;
						}
					}
				}
			}
		}
		return image;
	}
}