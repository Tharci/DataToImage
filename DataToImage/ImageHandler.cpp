#include "ImageHandler.h"



/*
tableSize = 100
payloadSize = 10
dataToSpread = int(payloadSize * 8 / 4)
positions = [int(tableSize/(dataToSpread-1)*i) for i in range(dataToSpread)]

print(positions)
*/


#include <iostream>
void ImageHandler::updateColorTable(const std::vector<char>& payload, DataType dataType, bool spreadData) {
	int headerSize = 0;
	encodeHeader(payload.size(), headerSize, HEADER_COMP_RATE);
	encodeHeader(spreadData, headerSize, HEADER_COMP_RATE);

	unsigned char compRate = getOptimalCompRate(colorTable.size() - headerSize, payload.size());
	if (compRate == 0)
		throw ImageHandler::ImageHandlerException::PayloadOverflow;
	std::cout << "CompRate: " << (int)compRate << "\n";


	SpreadIterator si(colorTable.size(), payload.size() + sizeof(char), headerSize, compRate, spreadData);
	si.first();

	encodeElement((char)dataType, si, compRate);

	for (int j = 0; j < payload.size(); j++) {
		encodeElement(payload[j], si, compRate);
	}
}

DICDecodedData ImageHandler::fetchDataFromColorTable() const {
	DICDecodedData deData;

	unsigned payloadSize;
	int headerSize = 0;
	bool spreadData;
	decodeHeader(payloadSize, headerSize, HEADER_COMP_RATE);
	decodeHeader(spreadData, headerSize, HEADER_COMP_RATE);

	unsigned char compRate = getOptimalCompRate(colorTable.size() - headerSize, payloadSize);
	std::cout << "CompRate: " << (int)compRate << "\n";

	deData.payload.resize(payloadSize);

	SpreadIterator si(colorTable.size(), deData.payload.size() + sizeof(char), headerSize, compRate, spreadData);
	si.first();
	
	char dt;
	decodeElement(dt, si, compRate);
	deData.dataType = (DataType)dt;


	for (int j = 0; j < deData.payload.size(); j++) {
		decodeElement(deData.payload[j], si, compRate);
	}

	return deData;
}

unsigned char ImageHandler::getPixelValue(unsigned char p, unsigned char v, unsigned char compRate) {
	char compRange = (char)pow(2, compRate);
	int p_out = p + (v - p % compRange);

	if (p_out > 255) p_out -= compRange;
	if (p_out < 0) p_out += compRange;

	return (unsigned char)p_out;
}

unsigned char ImageHandler::getOptimalCompRate(const unsigned colorTableSize, const unsigned payloadSize) {
	unsigned char compRateOptions[] = { 1, 2, 4 };

	for (auto r : compRateOptions) {
		if (payloadSize * (8 / r) <= colorTableSize) {
			return r;
		}
	}

	return 0;
}

