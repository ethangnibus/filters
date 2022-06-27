#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <algorithm>
#include <iterator>

#include "Image.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include "CGL/vector3D.h"

Image::Image(const char* filename) {
    if (read(filename)) {
        printf("Read %s\n", filename);
        size = w * h * channels;
    } else {
        printf("Failed to read %s\n", filename);
    }

}

Image::Image(int w, int h, int channels) : w(w), h(h), channels(channels) {
    size = w * h * channels;
    source = new u_int8_t[size];
    destination = new u_int8_t[size];
}

Image::Image(const Image& img) : Image(img.w, img.h, img.channels) {
    memcpy(source, img.source, size);
    memcpy(destination, img.source, size);
}

Image::~Image() {
    stbi_image_free(source);
    stbi_image_free(destination);
}

bool Image::read(const char* filename) {
    source = stbi_load(filename, &w, &h, &channels, 0);
    destination = stbi_load(filename, &w, &h, &channels, 0);
    return source != NULL && destination != NULL;
}

bool Image::write(const char* filename) {
    ImageType type = getFileType(filename);
    int success;

    switch (type) {
        case PNG:
            success = stbi_write_png(filename, w, h, channels, destination, w * channels);
            break;
        case BMP:
            success = stbi_write_bmp(filename, w, h, channels, destination);
            break;
        case JPG:
            success = stbi_write_jpg(filename, w, h, channels, destination, 100);
            break;
        case TGA:
            success = stbi_write_tga(filename, w, h, channels, destination);
            break;
    }
    return success != 0;
}

ImageType Image::getFileType(const char* filename) {
    const char* ext = strrchr(filename, '.');

    if (ext != nullptr) {
        if (strcmp(ext, ".png") == 0) {
            return PNG;
        }
        else if (strcmp(ext, ".jpg") == 0) {
            return JPG;
        }
        else if (strcmp(ext, ".bmp") == 0) {
            return BMP;
        }
        else if (strcmp(ext, ".tga") == 0) {
            return TGA;
        }
    }
    return PNG;
}


Image& Image::greyscale_avg() {
    // (r + g + b) / 3
    if (channels < 3) {
        printf("Image %p has less than 3 channels, it is assumed to already be greyscale", __cpp_capture_star_this);
    } else {
        for (size_t i = 0; i < size; i += channels) {
            int grey = (source[i] + source[i + 1] + source[i + 2]) / 3;
            memset(destination + i, grey, 3);
        }
    }
    return *this;
}

Image& Image::greyscale_lum() {
    if (channels < 3) {
        printf("Image %p has less than 3 channels, it is assumed to already be greyscale", __cpp_capture_star_this);
    } else {
        for (size_t i = 0; i < size; i += channels) {
            int grey = 0.2126*source[i] + 0.7152*source[i + 1] + 0.0722*source[i + 2];
            memset(destination + i, grey, 3);
        }
    }
    return *this;
}

Image& Image::liar() {
    if (channels < 3) {
        printf("Image %p has less than 3 channels, it is assumed to already be greyscale", __cpp_capture_star_this);
        return *this;
    }

    for (size_t i = 0; i < h; i += 1) {
        for (size_t j = 0; j < w; j += 1) {
            size_t index = channels * (i * w + j);
            CGL::Vector3D out_color = CGL::Vector3D(0.0, 0.0, 0.0);
            int num_neighbors = 0;
            CGL::Vector3D curr_color;
            curr_color.x = source[index + 0];
            curr_color.y = source[index + 1];
            curr_color.z = source[index + 2];
            
            if (i != 0) {
                num_neighbors += 1;
                CGL::Vector3D color_above;
                color_above.x = source[index + 0 - channels * w];
                color_above.y = source[index + 1 - channels * w];
                color_above.z = source[index + 2 - channels * w];

                out_color.x += abs(color_above.x - curr_color.x);
                out_color.y += abs(color_above.y - curr_color.y);
                out_color.z += abs(color_above.z - curr_color.z);
            }

            if (j != 0) {
                num_neighbors += 1;
                CGL::Vector3D color_left;
                color_left.x = source[index + 0 - channels];
                color_left.y = source[index + 1 - channels];
                color_left.z = source[index + 2 - channels];

                out_color.x += abs(color_left.x - curr_color.x);
                out_color.y += abs(color_left.y - curr_color.y);
                out_color.z += abs(color_left.z - curr_color.z);
            }

            if (i != 0 && j != 0) {
                num_neighbors += 1;
                CGL::Vector3D color_top_left;
                color_top_left.x = source[index + 0 - channels * (w + 1)];
                color_top_left.y = source[index + 1 - channels * (w + 1)];
                color_top_left.z = source[index + 2 - channels * (w + 1)];

                out_color.x += abs(color_top_left.x - curr_color.x);
                out_color.y += abs(color_top_left.y - curr_color.y);
                out_color.z += abs(color_top_left.z - curr_color.z);
            }

            if (i != h - 1) {
                num_neighbors += 1;
                CGL::Vector3D color_below;
                color_below.x = source[index + 0 + channels * w];
                color_below.y = source[index + 1 + channels * w];
                color_below.z = source[index + 2 + channels * w];

                out_color.x += abs(color_below.x - curr_color.x);
                out_color.y += abs(color_below.y - curr_color.y);
                out_color.z += abs(color_below.z - curr_color.z);
            }

            if (j != w - 1) {
                num_neighbors += 1;
                CGL::Vector3D color_right;
                color_right.x = source[index + 0 + channels];
                color_right.y = source[index + 1 + channels];
                color_right.z = source[index + 2 + channels];

                out_color.x += abs(color_right.x - curr_color.x);
                out_color.y += abs(color_right.y - curr_color.y);
                out_color.z += abs(color_right.z - curr_color.z);
            }

            if (i != h - 1 && j != w - 1) {
                num_neighbors += 1;
                CGL::Vector3D color_bottom_right;
                color_bottom_right.x = source[index + 0 + channels * (w + 1)];
                color_bottom_right.y = source[index + 1 + channels * (w + 1)];
                color_bottom_right.z = source[index + 2 + channels * (w + 1)];

                out_color.x += abs(color_bottom_right.x - curr_color.x);
                out_color.y += abs(color_bottom_right.y - curr_color.y);
                out_color.z += abs(color_bottom_right.z - curr_color.z);
            }

            out_color = (out_color / num_neighbors) * 10;
            out_color.clamp();

            destination[index + 0] = u_int8_t(out_color.x);
            destination[index + 1] = u_int8_t(out_color.y);
            destination[index + 2] = u_int8_t(out_color.z);
        }
    }
    

    return *this;
}