#ifndef TOYRENDERER_IMAGELOADER_HPP
#define TOYRENDERER_IMAGELOADER_HPP

namespace Stb {
    float* loadImageFloat(const char* path, int* w, int* h, int* nrComponents, int reqComp, bool verticalFlip=false);
    unsigned char* loadImageUChar(const char* path, int* w, int* h, int* nrComponents, int reqComp, bool verticalFlip=false);
    void free(void* data);
};

#endif //TOYRENDERER_IMAGELOADER_HPP
