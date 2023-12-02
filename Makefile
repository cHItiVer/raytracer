main: main.cpp raytracer.o
	g++ main.cpp raytracer.o -o raytracer -g3 -Ofast -march=native -fopenmp `libpng-config --ldflags`
raytracer.o: raytracer.cpp
	g++ -c raytracer.cpp -o raytracer.o -g3 -Ofast -march=native -fopenmp `libpng-config --cflags`
clean:
	rm *.png raytracer raytracer.o
gif: main
	export OMP_NUM_THREADS=20
	./raytracer
	convert -delay 3.3 *.png output.gif
	rm *.png