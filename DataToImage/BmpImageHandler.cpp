#include "BmpImageHandler.h"

#include <fstream>

void BmpImageHandler::loadImage(const std::string& path) {
    std::ifstream f_in;
    f_in.open(path, std::ios::in | std::ios::binary);

    if (!f_in.fail()) {
        f_in.read((char*)fileHeader, sizeof(fileHeader));
        f_in.read((char*)infoHeader, sizeof(infoHeader));

        BMPData bmpData(fileHeader, infoHeader);

        const unsigned int colorTableSize = bmpData.row_padded * bmpData.height;
        colorTable.resize(colorTableSize);
        f_in.read((char*)colorTable.data(), colorTableSize);

        f_in.close();
    }
    else {
        throw ImageHandler::ImageHandlerException::InputFileNotFound;
    }
}

void BmpImageHandler::saveImage(const std::string& path) const {
    std::ofstream f_out;
    f_out.open(path, std::ios::out | std::ios::binary);

    if (!f_out.fail()) {
        f_out.write((char*)fileHeader, sizeof(fileHeader));
        f_out.write((char*)infoHeader, sizeof(infoHeader));
        f_out.write((char*)colorTable.data(), colorTable.size());
        f_out.close();
    }
    else {
        throw BmpImageHandler::ImageHandlerException::OutputFileError;
    }
}
