#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "lodepng.h"

typedef struct vector {
	double x, y, z;
} Vector;

typedef struct ray {
	Vector position, velocity, acceleration;
	double time;
	int collided;
} Ray;

typedef struct color {
	unsigned char r, g, b;
} Color;

typedef struct object {
	int (*collides_with)(Ray *);
	Color (*color)(Ray *);
	struct object *next;
} Object;

typedef struct camera {
	Vector position;
	double phi, theta, fov;
	int width, height;
	Object *objects;
} Camera;

Vector add(Vector a, Vector b)
{
	Vector result = {a.x + b.x, a.y + b.y, a.z + b.z};
	return result;
}

Vector mul(Vector a, double k)
{
	Vector result = {a.x * k, a.y * k, a.z * k};
	return result;
}

void update(Ray *ray, double timestep)
{
	ray->velocity = add(ray->velocity, mul(ray->acceleration, timestep));
	ray->position = add(ray->position, mul(ray->velocity, timestep));
	ray->time += timestep;
}

void push(Object *object_list, Object *object)
{
	Object *current = object_list;
	while(current->next != NULL)
		current = current->next;
	current->next = object;
}

unsigned char *render(Camera camera, double timestep, double time_limit)
{
	unsigned char *image = (unsigned char *)malloc(camera.height * camera.width * 4 * sizeof(unsigned char));
	if(image == NULL)
	{
		printf("Error allocating memory for image.\n");
		exit(1);
	}
	Ray *rays = (Ray *)malloc(camera.height * camera.width * sizeof(Ray));
	if(rays == NULL)
	{
		printf("Error allocating memory for rays.\n");
		exit(1);
	}
	Vector v;
	double phi, theta;
	Object *current;
	Color c;
	for(int i = 0; i < camera.height; i++)
	{
		printf("%d/%d\n", i + 1, camera.height);
		for(int j = 0; j < camera.width; j++)
		{
			phi = camera.phi - camera.fov * camera.height / camera.width / 2 + camera.fov * camera.height / camera.width / (camera.height - 1) * i;
			theta = camera.theta - camera.fov / 2 + camera.fov / (camera.width - 1) * j;
			rays[i * camera.width + j].position = camera.position;
			v.x = sin(phi) * cos(theta);
			v.y = sin(phi) * sin(theta);
			v.z = cos(phi);
			rays[i * camera.width + j].velocity = v;
			rays[i * camera.width + j].collided = rays[i * camera.width + j].time = v.x = v.y = v.z = 0;
			rays[i * camera.width + j].acceleration = v;
			while(!rays[i * camera.width + j].collided && rays[i * camera.width + j].time <= time_limit)
			{
				update(&rays[i * camera.width + j], timestep);
				for(current = camera.objects; current != NULL; current = current->next)
				{
					if(current->collides_with(&rays[i * camera.width + j]))
					{
						rays[i * camera.width + j].collided = 1;
						c = current->color(&rays[i * camera.width + j]);
						image[(i * camera.width + j) * 4] = c.r / (0.01 * pow(rays[i * camera.width + j].time, 2) + 1);
						image[(i * camera.width + j) * 4 + 1] = c.g / (0.01 * pow(rays[i * camera.width + j].time, 2) + 1);
						image[(i * camera.width + j) * 4 + 2] = c.b / (0.01 * pow(rays[i * camera.width + j].time, 2) + 1);
					}
				}
			}
			image[(i * camera.width + j) * 4 + 3] = 255;
			if(!rays[i * camera.width + j].collided)
			{
				for(int k = 0; k < 3; k++)
					image[(i * camera.width + j) * 4 + k] = 0;
			}
		}
	}
	free(rays);
	return image;
}

int floor_collision(Ray *ray)
{
	return ray->position.z < 0;
}

Color floor_color(Ray *ray)
{
	Color result = {255, 0, 0};
	return result;
}

int ceiling_collision(Ray *ray)
{
	return ray->position.z > 5;
}

Color ceiling_color(Ray *ray)
{
	Color result = {0, 255, 0};
	return result;
}

int front_wall_collision(Ray *ray)
{
	return ray->position.x > 10;
}

Color front_color(Ray *ray)
{
	Color result = {20 * fabs(ray->position.x), 20 * fabs(ray->position.y), 20 * fabs(ray->position.z)};
	return result;
}

int back_wall_collision(Ray *ray)
{
	return ray->position.x < -10;
}

Color back_color(Ray *ray)
{
	Color result = {0, 255, 255};
	return result;
}

int left_wall_collision(Ray *ray)
{
	return ray->position.y < -10;
}

Color left_color(Ray *ray)
{
	Color result = {255, 255, 0};
	return result;
}

int right_wall_collision(Ray *ray)
{
	return ray->position.y > 10;
}

Color right_color(Ray *ray)
{
	Color result = {0, 0, 255};
	return result;
}

int event_horizon_collision(Ray *ray)
{
	double distance = sqrt(pow(5 - ray->position.x, 2) + pow(0 - ray->position.y, 2) + pow(2 - ray->position.z, 2));
	Vector acceleration = {5 - ray->position.x, 0 - ray->position.y, 2 - ray->position.z};
	ray->acceleration = mul(mul(acceleration, 1 / distance), pow(distance, -2));
	return 0;
}

int sphere_collision(Ray *ray)
{
	return sqrt(pow(8 - ray->position.x, 2) + pow(-5 - ray->position.y, 2) + pow(1 - ray->position.z, 2)) < 1;
}

Color sphere_color(Ray *ray)
{
	Color result = {32, 32, 32};
	return result;
}

int main(int argc, char *argv[])
{
	Object floor = {floor_collision, floor_color},
	ceiling = {ceiling_collision, ceiling_color},
	front_wall = {front_wall_collision, front_color},
	back_wall = {back_wall_collision, back_color},
	left_wall = {left_wall_collision, left_color},
	right_wall = {right_wall_collision, right_color},
	event_horizon = {event_horizon_collision, NULL},
	sphere = {sphere_collision, sphere_color};
	push(&floor, &ceiling);
	push(&floor, &front_wall);
	push(&floor, &back_wall);
	push(&floor, &left_wall);
	push(&floor, &right_wall);
	push(&floor, &event_horizon);
	push(&floor, &sphere);
	Camera camera = {{0, 0, 2}, M_PI / 2, 0, M_PI / 1, 500, 500, &floor};
	unsigned char *image;
	char filename[20];
	for(int i = 0; i < 100; i += 1)
	{
		camera.position.x += 0.1;
		image = render(camera, 0.01, 100);
		sprintf(filename, "result_%04d.png", i);
		unsigned int error = lodepng_encode32_file(filename, image, camera.width, camera.height);
		if(error)
			printf("Error %u: %s\n", error, lodepng_error_text(error));
		free(image);
	}
	return 0;
}