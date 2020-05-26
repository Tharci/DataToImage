#pragma once

#include "ImageHandler.h"

struct BMPData {
    BMPData(unsigned char* fileHeader, unsigned char* infoHeader) {
        width = height = 0;

        width = width | infoHeader[4];
        width = width | infoHeader[5] << 8;
        width = width | infoHeader[6] << 16;
        width = width | infoHeader[7] << 24;

        height = height | infoHeader[8];
        height = height | infoHeader[9] << 8;
        height = height | infoHeader[10] << 16;
        height = height | infoHeader[11] << 24;

        row_padded = (width * 3 + 3) & (~3u);
    }

    unsigned width, height, row_padded;
};


class BmpImageHandler : public ImageHandler {
    unsigned char fileHeader[14] = { 0 };
    unsigned char infoHeader[40] = { 0 };

public:
    BmpImageHandler() {}
    BmpImageHandler(bool markPixels) : ImageHandler(markPixels) {}

    void loadImage(const std::string& path) override final;
    void saveImage(const std::string& path) const override final;

};

