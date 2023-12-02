#include "raytracer.hpp"
#include <iostream>
#include <cmath>

using namespace std;
using namespace raytracer;

int main(int argc, char *argv[])
{
	vector<Object> objects;
	objects.push_back(Object([](Vector3D v) {return false;}, png::rgb_pixel(0, 0, 0), [](Vector3D v) {
		Vector3D u = Vector3D(5, 0, 2) - v;
		return u.normalized() / pow(u.length() + 0.01, 2);
	}));
	objects.push_back(Object([](Vector3D v) {return v.z < 0;}, png::rgb_pixel(0, 0, 255)));
	objects.push_back(Object([](Vector3D v) {return v.z > 10;}, png::rgb_pixel(255, 0, 0)));
	objects.push_back(Object([](Vector3D v) {return v.x > 10;}, png::rgb_pixel(0, 255, 0)));
	objects.push_back(Object([](Vector3D v) {return v.y > 10;}, png::rgb_pixel(255, 255, 255)));
	objects.push_back(Object([](Vector3D v) {return v.y < -10;}, png::rgb_pixel(255, 255, 255)));
	Camera camera(objects, 200, 200);
	vector<png::image<png::rgb_pixel>> images;
	int num_images = 1;
	for(int i = 0; i < num_images; i++)
	{
		images.push_back(camera.render(Vector3D(-5, 0, 2), 0, 3.14 / 2, 3.14 / 2, 3.14 / 2));
		cout << (i + 1) << "/" << num_images << endl;
	}
	for(int i = 0; i < images.size(); i++)
		images.at(i).write(string(4 - min(4, (int)to_string(i).size()), '0') + to_string(i) + ".png");
	return 0;
}