#pragma once

#include <string>
#include <vector>
#include <math.h>
#include <stdexcept>


#define HEADER_COMP_RATE 4

enum class DataType { Binary = 0, Text = 1 };

struct DICDecodedData {
	std::vector<char> payload;
	DataType dataType;
};


class SpreadIterator {
	int i = 0;
	int spreadIdx = 0;
	const unsigned char compRate;
	unsigned headerSize;
	const unsigned tableSize, partsToSpread;
	bool spreadData;

public:
	SpreadIterator(unsigned tableSize, unsigned payloadSize, unsigned headerSize, unsigned char compRate, bool spreadData = true)
		: tableSize(tableSize - headerSize), compRate(compRate), headerSize(headerSize), spreadData(spreadData), partsToSpread(payloadSize * 8 / compRate) { }

	void first() {
		i = 0;
		spreadIdx = 0;
	}

	bool end() const {
		return i > partsToSpread;
	}

	void next() {
		i++;
		if (end())
			throw std::out_of_range("Iterator index out of range.");
		spreadIdx = (int)(tableSize / (partsToSpread - 1) * i);
		int a = 0;
	}

	int current() const {
		return (spreadData ? spreadIdx : i) + headerSize;
	}
};


class ImageHandler {
	bool markPixels = false;

	static unsigned char getPixelValue(unsigned char p, unsigned char v, unsigned char compRate);

	template<typename T>
	void encodeHeader(T value, int& i, unsigned char compRate) {
		if (compRate != 1 && compRate != 2 && compRate != 4)
			throw ImageHandlerException::InvalidCompressionRate;

		const unsigned char parts = sizeof(T) * 8 / compRate;
		unsigned char compRange = (unsigned char)pow(2, compRate);

		for (int j = 0; j < parts; j++) {
			unsigned char valuePart = compRange - 1 & value >> (parts - j - 1) * compRate;
			colorTable[i] = getPixelValue(colorTable[i], valuePart, compRate);
			i++;
		}
	}

	template<typename T>
	void encodeElement(T value, SpreadIterator& si, unsigned char compRate) {
		if (compRate != 1 && compRate != 2 && compRate != 4)
			throw ImageHandlerException::InvalidCompressionRate;

		const unsigned char parts = sizeof(T) * 8 / compRate;
		unsigned char compRange = (unsigned char)pow(2, compRate);

		for (int j = 0; j < parts; j++) {
			unsigned char valuePart = compRange - 1 & value >> (parts - j - 1) * compRate;
			colorTable[si.current()] = getPixelValue(colorTable[si.current()], valuePart, compRate);

			/* Mark changed pixels */
			if (markPixels) {
				if (si.current() < colorTable.size() - 1)
					colorTable[si.current() + 1] = 255;
			}
			
			si.next();
		}
	}
	
	template<typename T>
	void decodeHeader(T& value, int& i, unsigned char compRate) const {
		if (compRate != 1 && compRate != 2 && compRate != 4)
			throw ImageHandlerException::InvalidCompressionRate;

		const unsigned char parts = sizeof(T) * 8 / compRate;
		unsigned char compRange = (unsigned char)pow(2, compRate);

		value = 0;
		for (int j = 0; j < parts; j++) {
			value = value | (colorTable[i] % compRange << (parts - j - 1) * compRate);
			i++;
		}
	}

	template<typename T>
	void decodeElement(T& value, SpreadIterator& si, unsigned char compRate) const {
		if (compRate != 1 && compRate != 2 && compRate != 4)
			throw ImageHandlerException::InvalidCompressionRate;

		const unsigned char parts = sizeof(T) * 8 / compRate;
		unsigned char compRange = (unsigned char)pow(2, compRate);

		value = 0;
		for (int j = 0; j < parts; j++) {
			value = value | (colorTable[si.current()] % compRange << (parts - j - 1) * compRate);
			si.next();
		}
	}



protected:
	std::vector<unsigned char> colorTable;

	static unsigned char getOptimalCompRate(const unsigned colorTableSize, const unsigned payloadSize);

public:
	ImageHandler() {}
	ImageHandler(bool markPixels) : markPixels(markPixels) {}

	enum class ImageHandlerException { InvalidCompressionRate, PayloadOverflow, InputFileNotFound, OutputFileError };

	virtual void loadImage(const std::string& path) = 0;
	virtual void saveImage(const std::string& path) const = 0;

	void updateColorTable(const std::vector<char>& payload, DataType dataType, bool spreadData);
	DICDecodedData fetchDataFromColorTable() const;

	// unsigned getMaxPayloadSize() const;
};

