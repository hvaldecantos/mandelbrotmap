//1.  Write sequential and multi-core parallel programs for Mandelbrot Set. Measure the
//    execution time as K changes from 1 to 4, and compute speedup and efficiency.

#include <iostream>
#include <fstream>
#include <complex>
#include <thread>

#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <chrono>

using namespace std;

// Thread pool implementation is taken from https://stackoverflow.com/a/29742586/2109534
// and modified for specific porpuses

class ThreadPool
{
    public:

    ThreadPool (int threads) : shutdown_ (false) {
        threads_.reserve (threads);
        for (int i = 0; i < threads; ++i)
            threads_.emplace_back (std::bind (&ThreadPool::threadEntry, this, i));
    }

    void join() {
        {
            // Unblock any threads and tell them to stop
            shutdown_ = true;
            condVar_.notify_all();
        }

        for (auto& thread : threads_)
            thread.join();
    }

    void doJob (std::function <void (void)> func) {
        // Place a job on the queu and unblock a thread
        jobs_.emplace (std::move (func));
        condVar_.notify_one();
    }

    private:

    void threadEntry (int i)
    {
        std::function <void (void)> job;

        while (1) {
            {
                std::unique_lock <std::mutex> l (lock_);

                while (! shutdown_ && jobs_.empty())
                    condVar_.wait (l);

                if (jobs_.empty ()) {
                    return;
                 }

                job = std::move (jobs_.front ());
                jobs_.pop();
            }
            job ();
        }

    }

    std::mutex lock_;
    std::condition_variable condVar_;
    bool shutdown_;
    std::queue <std::function <void (void)>> jobs_;
    std::vector <std::thread> threads_;
};

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

    stringstream results[whole];

    ThreadPool p(nthreads);

    for (int part = 0; part < whole; ++part) {
        p.doJob(std::bind (scan_pixels, &results[part], width, height, max_iter, part, whole));
    }

    p.join();

    for (int part = 0; part < whole; part++) { my_Image << results[part].str(); }
}

// Compile:
//     $ c++ -std=c++11 -pthread -o mandel.pool.exe mandelbrot.threadpool.cpp
// Run passing the number of threads (1 to 4 depending on your hardware):
//     $ ./mandel.pool.exe 1
// Open the picture:
//     $ ./mandel.ppm
// Executes with 1 to 4 cores, 10 times each, to measure time:
//     $ ./measure_times.sh 4 10 ./mandel.pool.exe
