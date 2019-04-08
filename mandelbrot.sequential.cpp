//1.  Write sequential and multi-core parallel programs for Mandelbrot Set. Measure the
//    execution time as K changes from 1 to 4, and compute speedup and efficiency.

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

int main(int argc, char** argv) {

    int width = 400, height = 400, max_iter = 1000;
    cout << "Using sequential version." << endl;
    string name = "mandel.ppm";

    ofstream my_Image(name);
    my_Image << "P3\n" << width << " " << height << " 255\n";

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {

            int iteration = mandelbrot(row, col, width, height, max_iter);

            if (iteration < max_iter && row !=0) my_Image << iteration * 255 << ' ' << iteration * 20 << ' ' << iteration * 5 << "\n";
            else my_Image << 0 << ' ' << 0 << ' ' << 0 << "\n";
        }
    }

    return 0;
}

// Compile:
//     $ c++ -std=c++11 -o mandel.seq.exe mandelbrot.sequential.cpp
// Run (it runs in 1 core):
//     $ ./mandel.seq.exe
// Open the picture:
//     $ ./mandel.ppm
// Executes with 1 core 10 times to measure time:
//     $ ./measure_times.sh 1 10 ./mandel.seq.exe
