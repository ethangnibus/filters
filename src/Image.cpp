#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <algorithm>
#include <iterator>
#include <stdlib.h>

#include "Image.h"
#include "stb_image.h"
#include "stb_image_write.h"




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


void Image::copyColor(CGL::Vector3D& color, size_t index) {
    color.x = source[index + 0];
    color.y = source[index + 1];
    color.z = source[index + 2];
}

void Image::setColor(CGL::Vector3D& color, size_t index) {
    destination[index + 0] = u_int8_t(color.x);
    destination[index + 1] = u_int8_t(color.y);
    destination[index + 2] = u_int8_t(color.z);
}

void Image::sampleColor(CGL::Vector3D& color) {
    size_t index = size_t(rand() % (w * h)) * channels;
    color.x = source[index + 0];
    color.y = source[index + 1];
    color.z = source[index + 2];
}


Image& Image::greyscale_avg() {
    // (r + g + b) / 3
    if (channels < 3) {
        printf("Image %ld has less than 3 channels, it is assumed to already be greyscale", __cpp_capture_star_this);
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
        printf("Image %ld has less than 3 channels, it is assumed to already be greyscale", __cpp_capture_star_this);
    } else {
        for (size_t i = 0; i < size; i += channels) {
            int grey = 0.2126*source[i] + 0.7152*source[i + 1] + 0.0722*source[i + 2];
            memset(destination + i, grey, 3);
        }
    }
    return *this;
}

Image& Image::inverse() {
    // (r + g + b) / 3
    if (channels < 3) {
        printf("Image %ld has less than 3 channels, it is assumed to already be greyscale", __cpp_capture_star_this);
        return *this;
    }
    
    for (size_t i = 0; i < h; i += 1) {
        for (size_t j = 0; j < w; j += 1) {
            size_t index = channels * (i * w + j);
            CGL::Vector3D out_color = CGL::Vector3D(255.0, 255.0, 255.0);
            
            CGL::Vector3D curr_color;
            copyColor(curr_color, index);

            out_color = out_color - curr_color;
            
            

            setColor(out_color, index);
        }
    }
    return *this;
}



Image& Image::liar() {
    if (channels < 3) {
        printf("Image %ld has less than 3 channels, it is assumed to already be greyscale", __cpp_capture_star_this);
        return *this;
    }

    for (size_t i = 0; i < h; i += 1) {
        for (size_t j = 0; j < w; j += 1) {
            size_t index = channels * (i * w + j);
            CGL::Vector3D out_color = CGL::Vector3D(0.0, 0.0, 0.0);
            int num_neighbors = 0;
            CGL::Vector3D curr_color;
            copyColor(curr_color, index);
            
            if (i != 0) {
                num_neighbors += 1;
                CGL::Vector3D color_above;
                copyColor(color_above, index - channels * w);

                out_color.x += abs(color_above.x - curr_color.x);
                out_color.y += abs(color_above.y - curr_color.y);
                out_color.z += abs(color_above.z - curr_color.z);
            }

            if (j != 0) {
                num_neighbors += 1;
                CGL::Vector3D color_left;
                copyColor(color_left, index - channels);

                out_color.x += abs(color_left.x - curr_color.x);
                out_color.y += abs(color_left.y - curr_color.y);
                out_color.z += abs(color_left.z - curr_color.z);
            }

            if (i != 0 && j != 0) {
                num_neighbors += 1;
                CGL::Vector3D color_top_left;
                copyColor(color_top_left, index - channels * (w + 1));

                out_color.x += abs(color_top_left.x - curr_color.x);
                out_color.y += abs(color_top_left.y - curr_color.y);
                out_color.z += abs(color_top_left.z - curr_color.z);
            }

            if (i != h - 1) {
                num_neighbors += 1;
                CGL::Vector3D color_below;
                copyColor(color_below, index + channels * w);

                out_color.x += abs(color_below.x - curr_color.x);
                out_color.y += abs(color_below.y - curr_color.y);
                out_color.z += abs(color_below.z - curr_color.z);
            }

            if (j != w - 1) {
                num_neighbors += 1;
                CGL::Vector3D color_right;
                copyColor(color_right, index + channels);

                out_color.x += abs(color_right.x - curr_color.x);
                out_color.y += abs(color_right.y - curr_color.y);
                out_color.z += abs(color_right.z - curr_color.z);
            }

            if (i != h - 1 && j != w - 1) {
                num_neighbors += 1;
                CGL::Vector3D color_bottom_right;
                copyColor(color_bottom_right, index + channels * (w + 1));

                out_color.x += abs(color_bottom_right.x - curr_color.x);
                out_color.y += abs(color_bottom_right.y - curr_color.y);
                out_color.z += abs(color_bottom_right.z - curr_color.z);
            }

            out_color = (out_color / num_neighbors) * 10;
            out_color.clamp();

            setColor(out_color, index);
        }
    }

    return *this;
}

Image& Image::echo(Image& last, Image& palette) {
    if (channels < 3) {
        printf("Image %ld has less than 3 channels, it is assumed to already be greyscale", __cpp_capture_star_this);
        return *this;
    }

    for (size_t i = 0; i < h; i += 1) {
        for (size_t j = 0; j < w; j += 1) {
            size_t index = channels * (i * w + j);

            CGL::Vector3D out_color = CGL::Vector3D(0.0, 0.0, 0.0);
            CGL::Vector3D curr_color;
            copyColor(curr_color, index);
            
            if (curr_color.addElements() < 300) {
                last.copyColor(curr_color, index);
                if (curr_color.addElements() >= 10) {
                    palette.copyColor(curr_color, index);
                }
            }
            if (curr_color.addElements() < 20) {
                curr_color.x = 0;
                curr_color.y = 0;
                curr_color.z = 0;
            }
            int sub = rand() % 50;
            curr_color.x -= sub;
            curr_color.y -= sub;
            curr_color.z -= sub;

            out_color = curr_color;
            setColor(out_color, index);
        }
    }

    return *this;
}

Image& Image::colorTo(Image& last) {
    if (channels < 3) {
        printf("Image %ld has less than 3 channels, it is assumed to already be greyscale", __cpp_capture_star_this);
        return *this;
    }

    for (size_t i = 0; i < h; i += 1) {
        for (size_t j = 0; j < w; j += 1) {
            size_t index = channels * (i * w + j);

            CGL::Vector3D front_color = CGL::Vector3D(0.0, 0.0, 0.0);
            CGL::Vector3D back_color = CGL::Vector3D(0.0, 0.0, 0.0);
            copyColor(back_color, index);
            last.copyColor(front_color, index);

            if (front_color.x > front_color.y && front_color.x > front_color.z) {
                back_color = back_color + front_color / 2.0;
            }

            setColor(back_color, index);
        }
    }

    return *this;
}