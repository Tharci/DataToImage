#include <iostream>
#include <iterator>
#include <cstring>
#include <fstream>

#include "DataImageConverter.hpp"


enum class Mode { Nothing, Encode, Decode };


int main(int argc, const char* argv[]) {
    Mode mode = Mode::Nothing;
    bool spreadData = true;
    bool markData = false;
    std::string inPath      = "";
    std::string outPath     = "";
    std::string dataPath    = "";
    std::string textData    = "";

    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--mode") == 0) {
            if (strcmp(argv[i + 1], "encode") == 0)
                mode = Mode::Encode;
            else if (strcmp(argv[i + 1], "decode") == 0)
                mode = Mode::Decode;
            else {
                std::cout << "Error: Unknown mode.\n";
                return 1;
            }
        }
        else if (strcmp(argv[i], "-sd") == 0 || strcmp(argv[i], "--spreaddata") == 0) {
            if (strcmp(argv[i + 1], "0") == 0)
                spreadData = false;
            else if (strcmp(argv[i + 1], "1") == 0)
                spreadData = true;
            else {
                std::cout << "Error: Unknown value for spreaddata.\n";
                return 1;
            }
        }
        else if (strcmp(argv[i], "-md") == 0 || strcmp(argv[i], "--markdata") == 0) {
            if (strcmp(argv[i + 1], "0") == 0)
                markData = false;
            else if (strcmp(argv[i + 1], "1") == 0)
                markData = true;
            else {
                std::cout << "Error: Unknown value for markdata.\n";
                return 1;
            }
        }
        else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--inpath") == 0) {
            inPath = std::string(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-o") == 0 || 
                strcmp(argv[i], "--outpath") == 0) {
            outPath = argv[i + 1];
        }
        else if (strcmp(argv[i], "-d") == 0 || 
                strcmp(argv[i], "--data") == 0) {
            dataPath = argv[i + 1];
        }
        else if (strcmp(argv[i], "-t") == 0 || 
                strcmp(argv[i], "--text") == 0) {
            textData = argv[i + 1];
        }
        else {
            std::cout << "Error: Unknown flag '" << argv[i] << "'\n";
            return 1;
        }
    }


    if (mode == Mode::Nothing) {
        std::cout << "Error: Mode not specified.\n";
        return 1;
    }

    if (inPath == "") {
        std::cout << "Error: Input file not specified.\n";
        return 1;
    }

    if (mode == Mode::Encode) {
        if (outPath == "") {
            std::cout << "Error: Output file not specified.\n";
            return 1;
        }


        auto payload = std::vector<char>();
        DataType dataType = DataType::Binary;
        if (dataPath == "" && textData == "") {
            std::cout << "Text to encode: \n";
            std::cin >> textData;
        }
        else if (dataPath != "" && textData == "") {
            std::ifstream inFile;
            inFile.open(dataPath, std::ios::binary);
            if (!inFile.fail()) {
                payload = std::vector<char>(std::istreambuf_iterator<char>(inFile), {});
                inFile.close();
            }
            else {
                std::cout << "Error: Could not open input file.\n";
                return 1;
            }

            if (DataImageConverter::getExtension(dataPath) == "txt")
                dataType = DataType::Text;
            else
                dataType = DataType::Binary;
        }
        else if (dataPath == "" && textData != "") {
            dataType = DataType::Text;

            payload.resize(textData.size() + 1);
            std::memcpy(payload.data(), textData.c_str(), textData.size());
        }
        else { /* dataPath != "" && textData != "" */
            std::cout << "Error: Cannot specify both datapath and textdata.\n";
            return 1;
        }

        try {
            DataImageConverter::encodeData(inPath, outPath, payload, dataType, spreadData, markData);
        }
        catch (DataImageConverter::DICException e) {
            switch (e) {
            case DataImageConverter::DICException::ExtensionNotSupported:
                std::cout << "Error: Extension not supported.\n";
                return 1;
                break;
            default:
                std::cout << "Error: Unknown error occured.\n";
                return 1;
                break;
            }
        }

        std::cout << "Data successfully encoded.\n";
    }
    else {
        DICDecodedData deData;
        try {
            deData = DataImageConverter::decodeData(inPath);
        }
        catch (DataImageConverter::DICException e) {
            switch (e)
            {
            case DataImageConverter::DICException::ExtensionNotSupported:
                std::cout << "Error: Extension not supported.\n";
                return 1;
                break;
            default:
                std::cout << "Error: Unknown error occured.\n";
                return 1;
                break;
            }
        }

        if (outPath != "") {
            std::ofstream outFile;
            outFile.open(outPath, std::ios::binary);
            if (!outFile.fail()) {
                outFile.write(deData.payload.data(), deData.payload.size());
                outFile.close();
            }
            else {
                std::cout << "Error: Could not open output file.\n";
                return 1;
            }
        }
        else {
            if (deData.dataType == DataType::Text) {
                std::cout << "\nDecoded data: " << deData.payload.data() << "\n";
            }
            else {
                std::cout << "Error: Data format is not text and no output file was specified.\n";
                return 1;
            }
        }
    }

    return 0;
}



/*
enum Color {RED, BLUE, GREEN};

void generateImg() {
    int w, h;
    w = h = 150;

    unsigned char* img = NULL;
    unsigned imgSize = 3 * w * h;
    int filesize = 54 + imgSize;

    img = (unsigned char*)malloc(imgSize);
    if (img) {
        memset(img, 0, imgSize);

        int x, y;
        for (int i = 0; i < w; i++) {
            for (int j = 0; j < h; j++) {
                x = i; y = (h - 1) - j;
                img[(x + y * w) * 3 + BLUE  ] = 100;
                img[(x + y * w) * 3 + GREEN ] = 100;
                img[(x + y * w) * 3 + RED   ] = 100;
            }
        }

        unsigned char bmpfileheader[14] = { 'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0 };
        unsigned char bmpinfoheader[40] = { 40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0 };
        unsigned char bmppad[3] = { 0,0,0 };

        bmpfileheader[2] = (unsigned char)(filesize);
        bmpfileheader[3] = (unsigned char)(filesize >> 8);
        bmpfileheader[4] = (unsigned char)(filesize >> 16);
        bmpfileheader[5] = (unsigned char)(filesize >> 24);

        bmpinfoheader[4] = (unsigned char)(w);
        bmpinfoheader[5] = (unsigned char)(w >> 8);
        bmpinfoheader[6] = (unsigned char)(w >> 16);
        bmpinfoheader[7] = (unsigned char)(w >> 24);
        bmpinfoheader[8] = (unsigned char)(h);
        bmpinfoheader[9] = (unsigned char)(h >> 8);
        bmpinfoheader[10] = (unsigned char)(h >> 16);
        bmpinfoheader[11] = (unsigned char)(h >> 24);

        std::ofstream file("img.bmp", std::ios::out | std::ios::binary);
        file.write((char*)bmpfileheader, sizeof(bmpfileheader));
        file.write((char*)bmpinfoheader, sizeof(bmpinfoheader));
        for (int i = 0; i < h; i++) {
            file.write((char*)img + (w * (h - i - 1) * 3), 3 * w);
            file.write((char*)bmppad, (4 - (w * 3) % 4) % 4);
        }

        free(img);
        file.close();
    }
}
*/


