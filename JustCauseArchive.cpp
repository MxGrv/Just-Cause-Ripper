#include "stdafx.h"

#if 0

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <cassert>

static const char* posBeg = nullptr;
static const char* posEnd = nullptr;
static size_t fileSize = 0;

unsigned int readDword(const char*& pos)
{
	unsigned int count = 0;
	count += static_cast<unsigned int>(static_cast<unsigned char>(*pos++));
	count += static_cast<unsigned int>((static_cast<unsigned char>(*pos++)) << 8);
	count += static_cast<unsigned int>((static_cast<unsigned char>(*pos++)) << 16);
	count += static_cast<unsigned int>((static_cast<unsigned char>(*pos++)) << 24);
	return count;
}

int actionTab(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cout << "Format:\n"
			"JustCauseRipper.exe <file_name>.tab" << std::endl;
		return -1;
	}

	std::string inputFilePath = argv[1];
	std::ifstream ifs(inputFilePath, std::ios_base::binary);
	if (!ifs)
	{
		std::cout << "ERROR: unable to open the tab file!" << std::endl;
		return -1;
	}

	size_t foundExt = inputFilePath.find_last_of(".");
	if (foundExt == std::string::npos)
	{
		std::cout << "Output file error!" << std::endl;
		return -1;
	}
	std::string outputFilePath = inputFilePath.substr(0, foundExt) + ".txt";
	std::ofstream ofs(outputFilePath);
	if (!ofs)
	{
		std::cout << "ERROR: unable to open the result file!" << std::endl;
		return -1;
	}

	ifs.seekg(0, std::ios_base::end);
	size_t size = ifs.tellg();
	ifs.seekg(0, std::ios_base::beg);
	size -= ifs.tellg();

	std::vector<char> data(size);
	ifs.read(data.data(), data.size());
	fileSize = size;

	const char* posBeg = data.data();
	posEnd = posBeg + size;

	if (size % 12 != 0)
	{
		std::cout << "Size % 12 != 0: " << std::dec << size << std::endl;
		return -1;
	}

	size_t cumulativeSumFirst = 0;
	size_t cumulativeSumSecond = 0;
	size_t cumulativeSumThird = 0;

	const char* pos = posBeg;
	for (size_t i = 0, j = 0; i < size; i += 12, ++j)
	{
//		std::cout << "[0x" << std::hex << std::setw(8) << std::setfill('0') << static_cast<int>(i) << "] ";
		ofs << "[0x" << std::hex << std::setw(8) << std::setfill('0') << static_cast<int>(i) << "] ";
		int first = static_cast<int>(readDword(pos));
		int second = static_cast<int>(readDword(pos));
		int third = static_cast<int>(readDword(pos));
//		std::cout << "0x" << std::hex << std::setw(8) << std::setfill('0') << first << " ";
		ofs << "0x" << std::hex << std::setw(8) << std::setfill('0') << first << " ";
//		std::cout << "0x" << std::hex << std::setw(8) << std::setfill('0') << second << " ";
		ofs << "0x" << std::hex << std::setw(8) << std::setfill('0') << second << " ";
//		std::cout << "0x" << std::hex << std::setw(8) << std::setfill('0') << third << " ";
		ofs << "0x" << std::hex << std::setw(8) << std::setfill('0') << third << " ";

		if (i != 0)
		{
			ofs << "| ";
			cumulativeSumFirst += static_cast<size_t>(first);
			ofs << "0x" << std::hex << std::setw(8) << std::setfill('0') << cumulativeSumFirst << " ";
			cumulativeSumSecond += static_cast<size_t>(second);
			ofs << "0x" << std::hex << std::setw(8) << std::setfill('0') << cumulativeSumSecond << " ";
			cumulativeSumThird += static_cast<size_t>(third);
			ofs << "0x" << std::hex << std::setw(8) << std::setfill('0') << cumulativeSumThird << " ";

//			double rateSecondToThird = static_cast<double>(second) / static_cast<double>(third);
//			ofs << "| ";
//			ofs << std::dec << std::fixed << std::setprecision(6) << rateSecondToThird << " ";
		}

//		std::cout << std::endl;
		ofs << std::endl;
	}

	return 0;
}

int saveFile(const char* filePos, const size_t fileSize, std::ostream &os, const std::string& outputFileName)
{
	std::ofstream ofs(outputFileName, std::ios_base::binary);
	if (!ofs)
	{
		os << "ERROR: unable to open the output file: \"" << outputFileName.c_str() << "\"!" << std::endl;
		return -1;
	}

	for (size_t i = 0; i < fileSize; ++i)
		ofs << *filePos;

	return 0;
}

static const unsigned int sarcSignature = 0x43524153; // "SARC"

int readSarc(const char*& pos, std::ostream& os)
{
	os << "Reading SARC from position 0x" << std::hex << std::setfill('0') << std::setw(8) << static_cast<size_t>(pos - posBeg) << std::endl;

	const char* posBegDuplicate = pos;

	unsigned int version = readDword(pos);
	assert(version == 0x00000004);
	unsigned int signature = readDword(pos);
	assert(signature == sarcSignature);
	unsigned int unk1 = readDword(pos); // Size of what?...
	unsigned int unk2 = readDword(pos); // Offset of the first file?...
	const char* borderPos = posBegDuplicate + static_cast<size_t>(unk2);

	os << "SARC correct, unk1 = " << std::hex << std::setfill('0') << std::setw(8) << unk1 <<
		", unk2 = " << std::hex << std::setfill('0') << std::setw(8) << unk2 << std::endl;

	static const unsigned int maxFileNameLength = 40;

	static std::string outputDirectoryPath = "F:\\- JCA\\1 init\\pc4";
	size_t foundDelim = outputDirectoryPath.find_last_of("\\/");
	if (foundDelim == std::string::npos)
	{
		std::cout << "Delimiter error!" << std::endl;
		return -1;
	}
	char delimiter = outputDirectoryPath[foundDelim];

	unsigned int recordsCount = 0;
	// One-by-one record
	while (pos < borderPos)
	{
		os << " ";
		os << "File record: ";
		unsigned int fileNameSize = readDword(pos);
		assert(fileNameSize > 0 && fileNameSize <= 80);
		std::string fileName(fileNameSize + 1, '\0');
		char* stringData = const_cast<char*>(fileName.data());
		for (unsigned int i = 0; i < fileNameSize; ++i)
			*stringData++ = *pos++;
		os << "file name = " << fileName.c_str();
		for (unsigned int i = fileNameSize; i < maxFileNameLength; ++i) os << " ";
		os << ", ";

		unsigned int fileOffset = readDword(pos);
		os << "offset = " << std::hex << std::setfill('0') << std::setw(8) << fileOffset;
		os << ", ";
		unsigned int fileSize = readDword(pos);
		os << "size = " << std::hex << std::setfill('0') << std::setw(8) << fileSize;
		os << std::endl;

		++recordsCount;

		std::string outputFileName = outputDirectoryPath;
		outputFileName += delimiter + fileName;
		const char* filePos = posBegDuplicate + static_cast<size_t>(fileOffset);
		os << "TRYING TO SAVE THE FILE [" << std::dec << recordsCount << "]: \"" << outputFileName.c_str()
			<< "\" from offset in pc4.arc = 0x" << std::hex << std::setfill('\0') << std::setw(8) <<
			static_cast<size_t>(filePos - posBegDuplicate) << " of size = " << std::dec << fileSize << std::endl;

		int result = 0;
		result = saveFile(filePos, fileSize, os, outputFileName);
		if (result != 0)
			return result;
	}

	os << "Found " << std::dec << recordsCount << " records!" << std::endl;
	return 0;
}

int actionArcFile(std::ifstream &ifs, std::ostream& os)
{
	ifs.seekg(0, std::ios_base::end);
	size_t size = ifs.tellg();
	ifs.seekg(0, std::ios_base::beg);
	size -= ifs.tellg();

	size = 0x20'0000; // ### TEMP DEBUG

	std::vector<char> data(size);
	ifs.read(data.data(), data.size());
	fileSize = size;

	posBeg = data.data();
	posEnd = posBeg + size;

	const char* pos = posBeg;

	int result = readSarc(pos, os);

	return result;
}

int main(int argc, char* argv[])
{
//    return actionTab(argc, argv);

	if (argc != 2)
	{
		std::cout << "Format:\nJustCauseRipper.exe <arc_file_path>" << std::endl;
		char c;
		std::cin >> c;
		return -1;
	}

	std::ifstream ifs(argv[1], std::ios_base::binary);
	if (!ifs)
	{
		std::cout << "Unable to open the arc file: \"" << argv[2] << "\"!" << std::endl;
		char c;
		std::cin >> c;
		return -1;
	}

	int result = actionArcFile(ifs, std::cout);
	char c;
	std::cin >> c;
	return result;
}

#endif

#include "JustCauseRipper.h"

// ### MAX LAZY
int main(int argc, char* argv[])
{
	std::list<std::string> list = {
//		"F:\\- JCA\\1 init\\pc.tab",
//		"F:\\- JCA\\1 init\\pc0.arc",
//		"F:\\- JCA\\1 init\\pc1.arc",
//		"F:\\- JCA\\1 init\\pc2.arc",
//		"F:\\- JCA\\1 init\\pc3.arc",
//		"F:\\- JCA\\1 init\\pc4.arc",
		"F:\\- JCA\\1 init\\pc_rus.tab",
		"F:\\- JCA\\1 init\\pc4_rus.arc",
	};
	std::string outDirPath = argv[1];

	return static_cast<int>(JustCauseRipper(list, outDirPath).run());
}
