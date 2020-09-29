#define IMAGE_SIZE 3000
#include <iostream>
#include "bitmap_image.hpp"
#include <thread>
using namespace std;

unsigned char pixels[IMAGE_SIZE][IMAGE_SIZE];
unsigned char output[IMAGE_SIZE][IMAGE_SIZE];

void PictureLoading(const bitmap_image& image, unsigned int from, unsigned int to) {
    for (unsigned int i = from; i < to; i++) {
        for (unsigned int j = 0; j < IMAGE_SIZE; j++) {
            pixels[i][j] = image.get_pixel(i, j).blue;
        }
    }
}

void PictureFilter(unsigned int from, unsigned int to) {
    int X = IMAGE_SIZE, Y = IMAGE_SIZE;
    unsigned char filter[9];
    for (int x = from; x < to; x++) {
        for (int y = 0; y < IMAGE_SIZE; y++) {
            if (x != 0 && x != X - 1 && y != Y - 1 && y != 0) {
                filter[0] = pixels[x - 1][y - 1];
                filter[1] = pixels[x][y - 1];
                filter[2] = pixels[x + 1][y - 1];
                filter[3] = pixels[x - 1][y];
                filter[4] = pixels[x][y];
                filter[5] = pixels[x + 1][y];
                filter[6] = pixels[x - 1][y + 1];
                filter[7] = pixels[x][y + 1];
                filter[8] = pixels[x + 1][y + 1];
                output[x][y] = (unsigned char)(255 - (0.25 * (filter[1] + filter[3] + filter[5] + filter[7]) - filter[4]));
            }
        }
    }
}

void PictureInverse(bitmap_image* image, unsigned int from, unsigned int to) {
    for (unsigned int i = from; i < to; i++) {
        for (unsigned int j = 0; j < IMAGE_SIZE; j++) {
            (*image).set_pixel(i, j, 255 - output[i][j], 255 - output[i][j], 255 - output[i][j]);
        }
    }
}

int main() {
    double start = clock();
    int counter = 0;
    bitmap_image image("D:\\Image\\image.bmp");
    thread threads[6];
    //Loading image into matrix
    for (thread& i : threads) {
        i = thread(PictureLoading, image, counter * 500, (counter + 1) * 500);
        counter++;
    }
    for (thread& i : threads) {
        i.join();
    }
    double load = clock();
    cout << "Loading time: " << (load - start) << " ms" << endl;
    //Differential operator II
    counter = 0;
    for (thread& i : threads) {
        i = thread(PictureFilter, counter * 500, (counter + 1) * 500);
        counter++;
    }
    for (thread& i : threads) {
        i.join();
    }
    double fltr = clock();
    cout << "Filter time: " << (fltr - load) << " ms" << endl;
    //Inverse image
    counter = 0;
    for (thread& i : threads) {
        i = thread(PictureInverse, &image, counter * 500, (counter + 1) * 500);
        counter++;
    }
    for (thread& i : threads) {
        i.join();
    }
    double invrs = clock();
    cout << "Inverse time: " << (invrs - fltr) << " ms" << endl;
    double end = clock();
    cout << "Common parallel time: " << (end - start) << " ms" << endl;
    image.save_image("Output.bmp");
    return 0;
}