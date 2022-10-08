#include <stdint.h>
#include <cstdio>
#include "CGL/vector3D.h"

enum ImageType {
    PNG, JPG, BMP, TGA
};

struct Image {
    u_int8_t* source = NULL;
    u_int8_t* destination = NULL;
    size_t size = 0;
    int w;
    int h;
    int channels;

    Image(const char* filename);
    Image(int w, int h, int channels);
    Image(const Image& img);
    ~Image();

    bool isTall = true;

    bool read(const char* filename);
    bool write(const char* filename);

    ImageType getFileType(const char* filename);

    void copyColor(CGL::Vector3D& color, size_t index);
    void setColor(CGL::Vector3D& color, size_t index);
    void sampleColor(CGL::Vector3D& color);

    Image& greyscale_avg();
    Image& greyscale_lum();
    Image& inverse();
    Image& liar();
    Image& echo(Image& last, Image& palette);
    Image& colorTo(Image& last);
};
