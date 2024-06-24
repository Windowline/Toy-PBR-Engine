#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#define STBI_ASSERT(x)
#include <stb_image.h>
#include <iostream>

#include "ImageLoader.hpp"


namespace Stb {

    float *loadImageFloat(const char *path, int *w, int *h, int *nrComponents, int reqComp, bool verticalFlip) {
        if (verticalFlip) {
            stbi_set_flip_vertically_on_load(true);
        }

        float *data = stbi_loadf(path, w, h, nrComponents, reqComp);

        if (!data) {
            std::cout << "Failed to load image." << std::endl;
            std::cout << stbi_failure_reason() << std::endl;
        }

        return data;
    }

    unsigned char* loadImageUChar(const char* path, int *w, int *h, int *nrComponents, int reqComp, bool verticalFlip) {
        if (verticalFlip) {
            stbi_set_flip_vertically_on_load(true);
        }

        unsigned char *data = stbi_load(path, w, h, nrComponents, reqComp);

        if (!data) {
            std::cout << "Failed to load image." << std::endl;
            std::cout << stbi_failure_reason() << std::endl;
        }

        return data;
    }



    void free(void *data) {
        stbi_image_free(data);
    }

};

