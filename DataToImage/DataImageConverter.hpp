#pragma once

#include <string>
#include <iostream>
#include <fstream>

#include "BmpImageHandler.h"


class DataImageConverter {
    static ImageHandler* getImageHandler(const std::string& path, bool markPixels = false);

public:
    enum class DICException { ExtensionNotSupported };

    static std::string getExtension(const std::string& path);

    static void encodeData(const std::string& inPath, const std::string& outPath, const std::vector<char> payload, DataType dataType, bool spreadData, bool markPixels = false);
    static DICDecodedData decodeData(const std::string& path);
};

