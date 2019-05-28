#include "stdafx.h"
#include "ArcParser.h"
#include <iomanip>
#include <cassert>
#include <vector>
#include <fstream>
#include <sstream>

#include <direct.h> // MSVS C++ directories (folders) operations

#define VERBOSE_REPORT
#undef  VERBOSE_REPORT

const char ArcParser::defaultArcSarcHeaderSignatureStr[5] = "SARC";
const int ArcParser::defaultArcSarcHeaderUnk1Str[4] = { 4, 0, 0, 0 };

static Dword readDword(const char*& pos)
{
	Dword dword = 0;
	dword += static_cast<Dword>(static_cast<unsigned char>(*pos++));
	dword += static_cast<Dword>((static_cast<unsigned char>(*pos++)) << 8);
	dword += static_cast<Dword>((static_cast<unsigned char>(*pos++)) << 16);
	dword += static_cast<Dword>((static_cast<unsigned char>(*pos++)) << 24);
	return dword;
}

static inline char getDelimiter(const std::string& str)
{
	size_t foundDelim = str.find_last_of("\\/");
	if (foundDelim == std::string::npos)
		return '\\'; // ### IDK WHY
	return str[foundDelim];
}

static inline bool doesExist(const std::string& fileName)
{
	std::ifstream ifs(fileName);
	return ifs.good();
}

static inline std::string toHexString(const size_t value)
{
	std::stringstream ss;
	ss << std::hex << value;
	return ss.str();
}

#pragma optimize("", off)
static void modifyTargetFilePath(std::string& str)
{
	str.pop_back();
	str += "_MY_DUP";
	str.push_back('\0');
	return;
}
//#pragma optimize("", on)


ErrCode ArcParser::run(const std::string& inputFilePath,
	const std::string& outputDirPath,
	const std::string& reportFilePath)
{
	// Open the files first
	std::ifstream input(inputFilePath, std::ios_base::binary);
	if (!input)
	{
		std::cout << "Error: Unable to open the input file: \"" << inputFilePath.c_str() << "\"!" << std::endl;
		return ErrCode::ERRCODE_ARC_INVALID_INPUT_FILE;
	}
	else
	{
		std::cout << "Input file \"" << inputFilePath.c_str() << "\" successfully openned!" << std::endl;
	}
	std::ofstream rep(reportFilePath, std::ios_base::app);
	if (!rep)
	{
		std::cout << "Error: Unable to open the report file: \"" << reportFilePath.c_str() << "\"!" << std::endl;
		return ErrCode::ERRCODE_ARC_INVALID_REPORT_FILE;
	}
	else
	{
		std::cout << "Report file \"" << reportFilePath.c_str() << "\" successfully openned!" << std::endl;
	}

	// Write the start message
	std::cout << "Arc parser started!" << std::endl;
	rep       << "Arc parser started!" << std::endl;

	// Create a directory for the current .arc-file
	int resMkDir = _mkdir(outputDirPath.c_str());
	if (resMkDir != 0 && errno != EEXIST)
	{
		std::cout << "Error: Unable to create the output directory: \"" << outputDirPath.c_str() << "\"!" << std::endl;
		rep       << "Error: Unable to create the output directory: \"" << outputDirPath.c_str() << "\"!" << std::endl;
		return ErrCode::ERRCODE_ARC_INVALID_OUTPUT_DIR;
	}

	// Find the SARC records
	// ### It is supposed that it is GUARANTEED that there are no fake SARC records (i.e.,
	// ###  no actual file has a sequense of "SARC" among it's contents, and there are no
	// ###  damaged SARC records, like those in the very end with no contents, etc)
	// ### Alternatively, you could check the DWORD previous to the SARC signature, or check
	// ###  offsets somehow...
	std::vector<size_t> sarcOffsets;
	assert(sizeof(Dword) == 4); // ### TEMP DEBUG
	int match = 0;
	while (!input.eof())
	{
		char c = static_cast<char>(input.get());
		if (c == defaultArcSarcHeaderSignatureStr[0])
			match = 1;
		else if ((match == 1 && c == defaultArcSarcHeaderSignatureStr[1]) ||
			(match == 2 && c == defaultArcSarcHeaderSignatureStr[2]))
			++match;
		else if (match == 3 && c == defaultArcSarcHeaderSignatureStr[3])
		{
			// Complete match
			size_t sarcOffset = static_cast<size_t>(input.tellg()) - sizeof(Dword) - sizeof(Dword);
			sarcOffsets.push_back(sarcOffset);
			match = 0;

//			break; // ### TEMP DEBUG
		}
		else
			match = 0;
	}

	input.clear(); // Drop EOF flag, so that we could keep going on

	// Determine the file size and push the file end offset to the list (for comfortability)
	input.seekg(0, std::ios_base::end);
	size_t finalOffset = input.tellg();
	sarcOffsets.push_back(finalOffset);
	input.seekg(0, std::ios_base::beg);

	unsigned renamed = 0;
	unsigned duplicated = 0;

	// Read the SARC records
	std::cout << "Found " << std::dec << sarcOffsets.size() - 1 << " SARC records!" << std::endl;
	rep       << "Found " << std::dec << sarcOffsets.size() - 1 << " SARC records!" << std::endl;
	for (unsigned recordNumber = 0; recordNumber < sarcOffsets.size() - 1; ++recordNumber)
	{
		size_t sarcOffset = sarcOffsets[recordNumber];
		std::cout << "Aggregating SARC record number " << std::dec << recordNumber + 1 << "..." << std::endl;
		rep       << "Found SARC record number " << std::dec << recordNumber + 1 << " at offset 0x" << std::hex << std::setfill('0') << std::setw(8) << sarcOffset << "!" << std::endl;
		
		// Determine the record size (for comfortability)
		size_t sarcSize = sarcOffsets[recordNumber + 1] - sarcOffsets[recordNumber];

		input.seekg(sarcOffset, std::ios_base::beg);

		// Allocate memory for the whole SARC record
		std::vector<char> sarcData(sarcSize);
		if (sarcSize != sarcData.size())
		{
			std::cout << "SARC record memory allocation failure!" << std::endl;
			rep       << "SARC record memory allocation failure!" << std::endl;
			return ErrCode::ERRCODE_SARC_MEMORY_ALLOCATION_FAILURE;
		}

		// Copy the whole file contents to the memory allocated
		input.read(sarcData.data(), sarcData.size()); // ### Nothing really happens here, BUT WHY IN THE FUCKING FUCK?!
		const char* sarcBeg = sarcData.data();
		const char* sarcEnd = sarcBeg + sarcSize;
		const char* sarcPos = sarcBeg; // Current position to go

		size_t finalRecordOffset = 0;

		// Read the header
		{
			ArcSarcHeader sarcHeader;
			sarcHeader.m_unk1 = readDword(sarcPos);
			sarcHeader.m_signature = readDword(sarcPos);
			sarcHeader.m_unk2 = readDword(sarcPos);
			sarcHeader.m_unk3 = readDword(sarcPos);

			// Check fields
			if (sarcHeader.m_unk1 != defaultArcSarcHeaderUnk1)
			{
				std::cout << "SARC record header field m_unk1 is invalid!" << std::endl;
				rep       << "SARC record header field m_unk1 is invalid!" << std::endl;
				return ErrCode::ERRCODE_SARC_MALFORMED_HEADER;
			}
			if (sarcHeader.m_signature != defaultArcSarcHeaderSignature)
			{
				std::cout << "SARC record header signature is invalid!" << std::endl;
				rep       << "SARC record header signature is invalid!" << std::endl;
				return ErrCode::ERRCODE_SARC_MALFORMED_HEADER;
			}
//			if (sarcHeader.m_unk2 != defaultArcSarcHeaderUnk2)
//			{
//				std::cout << "SARC record header field m_unk2 is invalid!" << std::endl;
//				rep       << "SARC record header field m_unk2 is invalid!" << std::endl;
//				return ErrCode::ERRCODE_SARC_MALFORMED_HEADER;
//			}

			finalRecordOffset = sarcHeader.m_unk3;
		}

		// Create a directory for the current SARC record
		std::istringstream ss;
		std::string sarcDirPath = outputDirPath + getDelimiter(outputDirPath) + "SARC-0x" + toHexString(sarcOffset);
		int resMkDir = _mkdir(sarcDirPath.c_str());
		if (resMkDir != 0 && errno != EEXIST)
		{
			std::cout << "Error: Unable to create the SARC record output directory: \"" << sarcDirPath.c_str() << "\"!" << std::endl;
			rep       << "Error: Unable to create the SARC record output directory: \"" << sarcDirPath.c_str() << "\"!" << std::endl;
			return ErrCode::ERRCODE_ARC_INVALID_OUTPUT_DIR;
		}

		// Read the file records
		unsigned subrecordNumber = 0;
		while (sarcPos < sarcBeg + finalRecordOffset) // ### This seems not so good, but practically it appears to be not so bad...
		{
			++subrecordNumber;

			ArcSarcFileRecord record;
			record.m_nameSize = readDword(sarcPos);

			if (record.m_nameSize < minArcSarcFileRecordNameSize)
			{
				std::cout << "SARC file record number " << std::dec << subrecordNumber << " has too small name size = " << std::dec << record.m_nameSize << "!" << std::endl;
				rep       << "SARC file record number " << std::dec << subrecordNumber << " has too small name size = " << std::dec << record.m_nameSize << "!" << std::endl;
				return ErrCode::ERRCODE_SARC_FILE_RECORD_NAME_SIZE_LESS_THAN_MIN;
			}
			if (record.m_nameSize > maxArcSarcFileRecordNameSize)
			{
				std::cout << "SARC file record number " << std::dec << subrecordNumber << " has too big name size = " << std::dec << record.m_nameSize << "!" << std::endl;
				rep       << "SARC file record number " << std::dec << subrecordNumber << " has too big name size = " << std::dec << record.m_nameSize << "!" << std::endl;
				return ErrCode::ERRCODE_SARC_FILE_RECORD_NAME_SIZE_MORE_THAN_MAX;
			}

			record.m_nameStr.resize(record.m_nameSize + 1); // ### No resizing checks
			char* nameStrData = const_cast<char*>(record.m_nameStr.data());
			for (size_t i = 0; i < record.m_nameSize; ++i)
				*nameStrData++ = *sarcPos++;

			size_t foundDelim = record.m_nameStr.find_last_of("\\/");
			if (foundDelim != std::string::npos)
			{
				++renamed;
				std::cout << "File name misuse: \"" << record.m_nameStr.c_str() << "\"!" << std::endl;
				rep       << "File name misuse: \"" << record.m_nameStr.c_str() << "\"!" << std::endl;
				record.m_nameStr = record.m_nameStr.substr(foundDelim + 1);
				std::cout << "Will be renamed to: \"" << record.m_nameStr.c_str() << "\"!" << std::endl;
				rep       << "Will be renamed to: \"" << record.m_nameStr.c_str() << "\"!" << std::endl;
			}

			record.m_offset = readDword(sarcPos);
			record.m_size = readDword(sarcPos);

			// Dump file info
#ifdef VERBOSE_REPORT
			std::cout << "Found archived file \"" << record.m_nameStr.c_str() << "\"" <<
				" at offset 0x" << std::hex << std::setfill('0') << std::setw(8) << record.m_offset <<
				" of size = " << std::dec << record.m_size <<
				" (0x" << std::hex << std::setfill('0') << std::setw(8) << record.m_size <<
				") [0x" << std::hex << std::setfill('0') << std::setw(8) << (record.m_offset + record.m_size) << "]!" << std::endl;
			rep       << "Found archived file \"" << record.m_nameStr.c_str() << "\"" <<
				" at offset 0x" << std::hex << std::setfill('0') << std::setw(8) << record.m_offset <<
				" of size = " << std::dec << record.m_size <<
				" (0x" << std::hex << std::setfill('0') << std::setw(8) << record.m_size <<
				") [0x" << std::hex << std::setfill('0') << std::setw(8) << (record.m_offset + record.m_size) << "]!" << std::endl;
#endif

			;

			// Save the file itself
			// ### There might already been one... or even already existing duplicate...
			std::string targetFilePath = sarcDirPath + getDelimiter(outputDirPath) + record.m_nameStr;

//			std::ifstream ifs(targetFilePath);
//			bool isGood = ifs.good();

			while (doesExist(targetFilePath))
//			while (!isGood)
			{
				++duplicated; // ### It should not work like that really, but I guess it would be alright
					// ### (SUPPOSE each file name cannot be repeated more than once per a SARC record...)

				std::cout << "File duplicate found: \"" << targetFilePath.c_str() << "\"!" << std::endl;
				rep       << "File duplicate found: \"" << targetFilePath.c_str() << "\"!" << std::endl;
//				targetFilePath += "_=_MY_DUP";
//				targetFilePath = targetFilePath + "_=_MY_DUP";
//				targetFilePath = targetFilePath.append("__MY_DUP");
//				targetFilePath.push_back('\0');
//				targetFilePath += std::string("__MY_DUP");
				modifyTargetFilePath(targetFilePath);

				std::cout << "The new file name will be: \"" << targetFilePath.c_str() << "\"!" << std::endl;
				rep       << "The new file name will be: \"" << targetFilePath.c_str() << "\"!" << std::endl;

//				std::ifstream ifs(targetFilePath);
//				bool isGood = ifs.good();
			}
//			targetFilePath += "";

			std::ofstream target(targetFilePath);
			if (!target)
			{
				std::cout << "Error: Unable to create the target file: \"" << targetFilePath.c_str() << "\"!" << std::endl;
				rep       << "Error: Unable to create the target file: \"" << targetFilePath.c_str() << "\"!" << std::endl;
				return ErrCode::ERRCODE_SARC_INVALID_TARGET_FILE;
			}
			target.write(sarcBeg + record.m_offset, record.m_size);

#ifdef VERBOSE_REPORT
			std::cout << "File number " << std::dec << subrecordNumber << " - \"" << targetFilePath.c_str() << "\" successfully written!" << std::endl;
			rep       << "File number " << std::dec << subrecordNumber << " - \"" << targetFilePath.c_str() << "\" successfully written!" << std::endl;
#endif

//			break; // ### TEMP DEBUG
		}

		std::cout << "SARC record number " << std::dec << recordNumber + 1 << ": successfully written " << std::dec << subrecordNumber << " files!" << std::endl;
		rep       << "SARC record number " << std::dec << recordNumber + 1 << ": successfully written " << std::dec << subrecordNumber << " files!" << std::endl;

//		break; // ### TEMP DEBUG
	}

	std::cout << "Renamed files: " << std::dec << renamed << std::endl;
	rep       << "Renamed files: " << std::dec << renamed << std::endl;
	std::cout << "Duplicate files: " << std::dec << duplicated << std::endl;
	rep       << "Duplicate files: " << std::dec << duplicated << std::endl;

	return ErrCode::ERRCODE_NONE;
}

#if 0

...
{
	...

	std::vector<size_t> headers;
	// First find all SARC archives and save thier offsets (in order to consume less memory)
	if (m_reportConsole)
		m_reportConsole << "Searching for SARC records..." << std::endl;
	if (m_reportFile)
		m_reportFile << "Searching for SARC records..." << std::endl;
	ErrCode result = findSarcHeaders(headers, fileSize);
	if (result != ErrCode::ERRCODE_NONE)
		return result;
	if (m_reportConsole)
		m_reportConsole << "Found " << std::dec << headers.size() << " SARC records!" << std::endl;
	if (m_reportFile)
		m_reportFile << "Found " << std::dec << headers.size() << " SARC records!" << std::endl;

	// Aggregate each header
	unsigned recordNumber = 0;
	for (const size_t offset : headers)
	{
		++recordNumber; // The 'normal' ('non-C-programmer') way

		if (m_reportConsole)
			m_reportConsole << "Aggregating the SARC record number " << std::dec << recordNumber << "..." << std::endl;
		if (m_reportFile)
			m_reportFile << "Aggregating the SARC record number " << std::dec << recordNumber << "..." << std::endl;
		ErrCode result = aggregateSarcRecord(offset, outDirPath);
		if (result != ErrCode::ERRCODE_NONE)
			return result;
		if (m_reportConsole)
			m_reportConsole << "Aggregated the SARC record number " << std::dec << recordNumber << "!" << std::endl;
		if (m_reportFile)
			m_reportFile << "Aggregated the SARC record number " << std::dec << recordNumber << "!" << std::endl;
	}

	...
}

ErrCode ArcParser::aggregateSarcRecord(const size_t offset, const std::string& curArcDirPath)
{
	m_input.seekg(offset, std::ios_base::beg);

	ArcSarcHeader header;
	m_input.read(reinterpret_cast<char*>(&header), sizeof(ArcSarcHeader));

	assert(sizeof(ArcSarcHeader) == 16); // ### TEMP DEBUG

	if (m_reportConsole)
		m_reportConsole << "SARC at offset = 0x" << std::hex << std::setfill('0') << std::setw(8) << offset << std::endl;
	if (m_reportFile)
	{
		m_reportFile << "SARC at offset = 0x" << std::hex << std::setfill('0') << std::setw(8) << offset << std::endl;
		m_reportFile << "Header: ";
		m_reportFile << "unk1 = 0x" << std::hex << std::setfill('0') << std::setw(8) << header.m_unk1;
		m_reportFile << ", ";
		m_reportFile << "signature = 0x" << std::hex << std::setfill('0') << std::setw(8) << header.m_signature;
		m_reportFile << ", ";
		m_reportFile << "unk2 = 0x" << std::hex << std::setfill('0') << std::setw(8) << header.m_unk2;
		m_reportFile << ", ";
		m_reportFile << "unk3 = 0x" << std::hex << std::setfill('0') << std::setw(8) << header.m_unk3;
		m_reportFile << std::endl;
	}

	// Create the folder for the current SARC

	if (m_reportConsole)
		m_reportConsole << "Sarc record folder creation..." << std::endl;
	if (m_reportFile)
		m_reportFile << "Sarc record folder creation..." << std::endl;

	std::string outDirPath = curArcDirPath;
	size_t foundDelim = outDirPath.find_last_of("\\/");
	if (foundDelim == std::string::npos)
	{
		if (m_reportConsole)
			m_reportConsole << "Error: Unable to create a new directory for the SARC record [delimiter]!" << std::endl;
		if (m_reportFile)
			m_reportFile << "Error: Unable to create a new directory for the SARC record [delimiter]!" << std::endl;
		return ErrCode::ERRCODE_GEN_DELIMITER_RETRIEVAL_ERROR;
	}
	char delimiter = outDirPath[foundDelim];
	outDirPath += delimiter + "SARC" + std::to_string(offset);

	int dirCreationResult = _mkdir(outDirPath.c_str());
	if (dirCreationResult != 0)
	{
		if (m_reportConsole)
			m_reportConsole << "Error: Unable to create directory \"" << outDirPath.c_str() << "\"!" << std::endl;
		if (m_reportFile)
			m_reportFile << "Error: Unable to create directory \"" << outDirPath.c_str() << "\"!" << std::endl;

		if (errno == EEXIST)
		{
			if (m_reportConsole)
				m_reportConsole << "Error: The directory already exists!" << std::endl;
			if (m_reportFile)
				m_reportFile << "Error: The directory already exists!" << std::endl;
		}
		else if (errno == ENOENT)
		{
			if (m_reportConsole)
				m_reportConsole << "Error: The directory path not found!" << std::endl;
			if (m_reportFile)
				m_reportFile << "Error: The directory path not found!" << std::endl;
		}
		else
		{
			if (m_reportConsole)
				m_reportConsole << "Error: Unknown error!" << std::endl;
			if (m_reportFile)
				m_reportFile << "Error: Unknown error!" << std::endl;
		}

		return ErrCode::ERRCODE_GEN_UNABLE_TO_CREATE_DIRECTORY;
	}

	if (m_reportConsole)
		m_reportConsole << "Sarc record folder created successfully!" << std::endl;
	if (m_reportFile)
		m_reportFile << "Sarc record folder created successfully!" << std::endl;

	// Prepare the records processing environment

	size_t recordsFieldSize = header.m_unk3 - sizeof(ArcSarcHeader);
	std::vector<char> recordsFieldArr(recordsFieldSize);
	if (recordsFieldArr.size() != recordsFieldSize)
	{
		if (m_reportConsole)
			m_reportConsole << "Sarc record demands too much memory!" << std::endl;
		if (m_reportFile)
			m_reportFile << "Sarc record demands too much memory!" << std::endl;
		return ErrCode::ERRCODE_SARC_MEMORY_ALLOCATION_FAILURE;
	}
	m_input.read(recordsFieldArr.data(), recordsFieldArr.size());

	const char* fileBeg = recordsFieldArr.data();
	const char* fileEnd = fileBeg + recordsFieldSize;
	const char* filePos = fileBeg; // Current position to go

	unsigned recordNumber = 0;
	// One-by-one record
	while (filePos < fileEnd)
	{
		++recordNumber; // The 'normal' ('non-C-programmer') way

						// Process the record itself

		ArcSarcFileRecord record;

		record.m_nameSize = readDword(filePos);
		if (record.m_nameSize < minArcSarcFileRecordNameSize)
		{
			if (m_reportConsole)
				m_reportConsole << "Sarc file record number " << std::dec << recordNumber << " has too small name size = " << std::dec << record.m_nameSize << "!" << std::endl;
			if (m_reportFile)
				m_reportFile << "Sarc file record number " << std::dec << recordNumber << " has too small name size = " << std::dec << record.m_nameSize << "!" << std::endl;
			return ErrCode::ERRCODE_SARC_FILE_RECORD_NAME_SIZE_LESS_THAN_MIN;
		}
		if (record.m_nameSize > maxArcSarcFileRecordNameSize)
		{
			if (m_reportConsole)
				m_reportConsole << "Sarc file record number " << std::dec << recordNumber << " has too big name size = " << std::dec << record.m_nameSize << "!" << std::endl;
			if (m_reportFile)
				m_reportFile << "Sarc file record number " << std::dec << recordNumber << " has too big name size = " << std::dec << record.m_nameSize << "!" << std::endl;
			return ErrCode::ERRCODE_SARC_FILE_RECORD_NAME_SIZE_MORE_THAN_MAX;
		}

		record.m_nameStr.resize(record.m_nameSize + 1); // ### No resizing checks
		char* nameStrData = const_cast<char*>(record.m_nameStr.data());
		for (size_t i = 0; i < record.m_nameSize; ++i)
			*nameStrData++ = *filePos++;

		record.m_offset = readDword(filePos);
		record.m_size = readDword(filePos);

		if (m_reportConsole)
			m_reportConsole << "Sarc file record number " << std::dec << recordNumber << " read!" << std::endl;
		if (m_reportFile)
		{
			m_reportFile << "Sarc file record number " << std::dec << recordNumber << " read!" << std::endl;
			m_reportFile << "File record: name = " << record.m_nameStr.c_str();
			m_reportFile << ", ";
			m_reportFile << "offset from SARC beginning = 0x" << std::hex << std::setfill('0') << std::setw(8) << record.m_offset;
			m_reportFile << ", ";
			m_reportFile << "offset from file beginning = 0x" << std::hex << std::setfill('0') << std::setw(8) << (offset + record.m_offset);
			m_reportFile << ", ";
			m_reportFile << "size = 0x" << std::hex << std::setfill('0') << std::setw(8) << record.m_size << " / ";
			m_reportFile << std::dec << record.m_size;
		}

		// Save the corresponding file data as a separate file

		std::string outFilePath = outDirPath + delimiter + record.m_nameStr;

		if (m_reportConsole)
			m_reportConsole << "Preparing to save the file as \"" << outFilePath.c_str() << "\"..." << std::endl;
		if (m_reportFile)
			m_reportFile << "Preparing to save the file as \"" << outFilePath.c_str() << "\"..." << std::endl;
		std::ofstream outFileStream(outFilePath);
		if (!outFileStream)
		{
			if (m_reportConsole)
				m_reportConsole << "Error: Unable to save the file as \"" << outFilePath.c_str() << "\"!" << std::endl;
			if (m_reportFile)
				m_reportFile << "Error: Unable to save the file as \"" << outFilePath.c_str() << "\"!" << std::endl;
			return ErrCode::ERRCODE_SARC_FILE_INVALID_OUTPUT_FILE;
		}
		ErrCode result = saveFile(offset + record.m_offset, record.m_size, outFileStream);
		if (result != ErrCode::ERRCODE_NONE)
		{
			if (m_reportConsole)
				m_reportConsole << "Error: Failure while saving the file!" << std::endl;
			if (m_reportFile)
				m_reportFile << "Error: Failure while saving the file!" << std::endl;
			return result;
		}
		if (m_reportConsole)
			m_reportConsole << "File \"" << outFilePath.c_str() << "\" successfully saved!" << std::endl;
		if (m_reportFile)
			m_reportFile << "File \"" << outFilePath.c_str() << "\" successfully saved!" << std::endl;
	}

	if (m_reportConsole)
		m_reportConsole << "SARC records (" << std::dec << recordNumber << ") successfully aggregated!" << std::endl;
	if (m_reportFile)
		m_reportFile << "SARC records (" << std::dec << recordNumber << ") successfully aggregated!" << std::endl;

	return ErrCode::ERRCODE_NONE;
}

ErrCode ArcParser::saveFile(const size_t offsetAbsolute, const size_t size, std::ofstream& outFileStream)
{
	if (!outFileStream)
		return ErrCode::ERRCODE_SARC_FILE_INVALID_OUTPUT_FILE;

	m_input.seekg(offsetAbsolute, std::ios_base::beg);

	for (size_t i = 0; i < size; ++i)
		outFileStream.put(m_input.get());

	return ErrCode::ERRCODE_NONE;
}

ErrCode ArcParser::findSarcHeaders(std::vector<size_t>& headers, const size_t fileSize)
{
	// Find the SARC signature (primitive search)
	// WARNING: suppose there are no empty SARC-records; hence, no SARC-records checks performed here
	//  This is a really bad algorithm anyway
	size_t match = 0; // Amount of SARC signature symbols checked so far
	size_t fileBeg = m_input.tellg();

	while (!m_input.eof())
	{
		char c;
		m_input >> c;
		if (match == 0)
		{
			if (c == defaultArcSarcHeaderSignatureStr[0])
				++match;
			else
				match = 0;
		}
		else if (match == 1)
		{
			if (c == defaultArcSarcHeaderSignatureStr[1])
				++match;
			else
				match = 0;
		}
		else if (match == 2)
		{
			if (c == defaultArcSarcHeaderSignatureStr[2])
				++match;
			else
				match = 0;
		}
		else if (match == 3)
		{
			if (c == defaultArcSarcHeaderSignatureStr[3])
			{
				// All 4 symbols coincide, so that we'd better check the previous version field (just in case)
				// Now unwind 8 symbols back
				m_input.unget();
				m_input.unget();
				m_input.unget();
				m_input.unget();

				size_t filePos = m_input.tellg();
				if (m_reportFile)
					m_reportFile << "SARC candidate found at offset = 0x" << std::hex << std::setfill('0') << std::setw(8) << static_cast<size_t>(filePos - fileBeg) << "!" << std::endl;

				m_input.unget();
				m_input.unget();
				m_input.unget();
				m_input.unget();

				int version;
				version = m_input.get();
				if (version != defaultArcSarcHeaderUnk1Str[0])
				{
					m_input.get();
					m_input.get();
					m_input.get();
					m_input.get();
					m_input.get();
					m_input.get();
					m_input.get();
					goto LABEL_DROP;
				}
				version = m_input.get();
				if (version != defaultArcSarcHeaderUnk1Str[1])
				{
					m_input.get();
					m_input.get();
					m_input.get();
					m_input.get();
					m_input.get();
					m_input.get();
					goto LABEL_DROP;
				}
				version = m_input.get();
				if (version != defaultArcSarcHeaderUnk1Str[2])
				{
					m_input.get();
					m_input.get();
					m_input.get();
					m_input.get();
					m_input.get();
					goto LABEL_DROP;
				}
				version = m_input.get();
				if (version != defaultArcSarcHeaderUnk1Str[3])
				{
					m_input.get();
					m_input.get();
					m_input.get();
					m_input.get();
					goto LABEL_DROP;
				}

				if (m_reportFile)
					m_reportFile << "SARC candidate succeeded!" << std::endl;

				// Now SARC record found!
				headers.push_back(filePos);

				// Do not forget to re-get the found signature
				m_input.get();
				m_input.get();
				m_input.get();
				m_input.get();

				match = 0;
				continue;

			LABEL_DROP:
				if (m_reportFile)
					m_reportFile << "SARC candidate failed in version!" << std::endl;
			}
			match = 0;
		}
	}

	return ErrCode::ERRCODE_NONE;
}

#endif
