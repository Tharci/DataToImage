#include "DataImageConverter.hpp"

std::string DataImageConverter::getExtension(const std::string& path) {
    unsigned dotIdx = path.rfind('.') + 1;
    if (dotIdx >= 0)
        return path.substr(dotIdx);
    else
        return "";
}

ImageHandler* DataImageConverter::getImageHandler(const std::string& path, bool markPixels) {
    std::string fileExt = getExtension(path);

    if (fileExt == "bmp" || fileExt == "BMP") {
        return new BmpImageHandler(markPixels);
    }
    else {
        return nullptr;
    }
}

void DataImageConverter::encodeData(const std::string& inPath, const std::string& outPath, const std::vector<char> payload, DataType dataType, bool spreadData, bool markPixels) {
    ImageHandler* ih = getImageHandler(inPath, markPixels);

    if (ih == nullptr) {
        throw DICException::ExtensionNotSupported;
    }

    ih->loadImage(inPath);

    try {
        ih->updateColorTable(payload, dataType, spreadData);
    }
    catch (ImageHandler::ImageHandlerException e) {
        switch (e) {
            case ImageHandler::ImageHandlerException::PayloadOverflow:
                throw ImageHandler::ImageHandlerException::PayloadOverflow;
                break;
            default:
                break;
        }
    }
    
    ih->saveImage(outPath);

    delete ih;
}

DICDecodedData DataImageConverter::decodeData(const std::string& path) {
    ImageHandler* ih = getImageHandler(path);

    if (ih == nullptr) {
        throw DataImageConverter::DICException::ExtensionNotSupported;
    }

    ih->loadImage(path);
    DICDecodedData deData = ih->fetchDataFromColorTable();

    delete ih;
    return deData;
}
