#pragma once
#include "png++/png.hpp"
#include <functional>

using namespace std;

namespace raytracer
{
	struct Vector3D
	{
		double x, y, z;
		Vector3D(): x(0), y(0), z(0) {}
		Vector3D(double x_i, double y_i, double z_i): x(x_i), y(y_i), z(z_i) {}
		double length();
		Vector3D normalized();
	};
	Vector3D operator+(Vector3D, Vector3D);
	Vector3D operator-(Vector3D, Vector3D);
	Vector3D operator*(double, Vector3D);
	Vector3D operator*(Vector3D, double);
	Vector3D operator/(Vector3D, double);
	struct Ray
	{
		Vector3D position, velocity;
		Ray(Vector3D p, Vector3D v): position(p), velocity(v) {}
		void step(double);
	};
	struct Object
	{
		function<bool(Vector3D)> inside;
		png::rgb_pixel color;
		function<Vector3D(Vector3D)> acceleration;
		Object(function<bool(Vector3D)> i, png::rgb_pixel c, function<Vector3D(Vector3D)> a = [](Vector3D v) {return Vector3D(0, 0, 0);}): inside(i), color(c), acceleration(a) {}
		void accelerate(Ray &, double);
	};
	class Camera
	{
		vector<Object> _objects;
		int _width, _height;
		double _time_step, _time_limit;
		public:
			Camera(vector<Object> objects, int width = 100, int height = 100, double time_step = 0.01, double time_limit = 100): _objects(objects), _width(width), _height(height), _time_step(time_step), _time_limit(time_limit) {}
			png::image<png::rgb_pixel> render(Vector3D, double, double, double, double);
	};
}