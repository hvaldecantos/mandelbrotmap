#include <iostream>
#include <fstream>
#include <complex>
#include <thread>

using namespace std;

int mandelbrot(int x, int y, int width, int height, int max)  {
    complex<float> point((float) (y - height/2.0) * 4.0/width, (float) (x - width/2.0) * 4.0/width);
    complex<float> z(0, 0);
    unsigned int iteration = 0;

    while (abs(z) < 4 && iteration < max) {
           z = z * z + point;
           iteration++;
    }
    return iteration;
}

void scan_pixels(stringstream* image, const int width, const int height, const int max, const int part, const int whole) {
    int start = part * height / whole;
    int end = ((part+1) * height / whole);

    for (int row = start; row < end; row++) {
        for (int col = 0; col < width; col++) {

            int iteration = mandelbrot(row, col, width, height, max);

            if (iteration < max && row !=start) (*image) << iteration * 255 << ' ' << iteration * 20 << ' ' << iteration * 5 << "\n";
            else (*image) << 0 << ' ' << 0 << ' ' << 0 << "\n";
        }
    }
}

int main(int argc, char** argv) {

    int width = 400, height = 400, part, max_iter = 1000, nthreads = atoi(argv[1]);
    cout << "Using " << nthreads << " threads." << endl;
    string name = "mandel.ppm";

    ofstream my_Image(name);
    my_Image << "P3\n" << width << " " << height << " 255\n";

    int whole = 100;

    for (int part = 0; part < whole; part = part + nthreads) {

        thread threads[nthreads];
        stringstream ss[nthreads];

        for (int t = 0; t < nthreads; ++t) {
            threads[t] = thread(scan_pixels, &ss[t], width, height, max_iter, part + t, whole);
        }
        for (int t = 0; t < nthreads; ++t) { threads[t].join(); }
        for (int t = 0; t < nthreads; ++t) { my_Image << ss[t].str(); }
    }

    return 0;
}

// Compile:
//     $ c++ -std=c++17 -pthread -o mandel.mult.exe mandelbrot.multithread.cpp
// Run passing the number of threads (1 to 4 depending on your hardware):
//     $ ./mandel.mult.exe 1
// Open the picture:
//     $ ./mandel.ppm
// Executes with 1 to 4 cores, 10 times each, to measure time:
//     $ ./measure_times.sh 4 10 ./mandel.mult.exe
