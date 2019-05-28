#include "stdafx.h"
#include "TabParser.h"
#include <iomanip>
#include <cassert>
#include <vector>
#include <fstream>

static Dword readDword(const char*& pos)
{
	Dword dword = 0;
	dword += static_cast<Dword>(static_cast<unsigned char>(*pos++));
	dword += static_cast<Dword>((static_cast<unsigned char>(*pos++)) << 8);
	dword += static_cast<Dword>((static_cast<unsigned char>(*pos++)) << 16);
	dword += static_cast<Dword>((static_cast<unsigned char>(*pos++)) << 24);
	return dword;
}

ErrCode TabParser::run(const std::string& inputFilePath,
	const std::string& reportFilePath)
{
	// Open the files first
	std::ifstream input(inputFilePath, std::ios_base::binary);
	if (!input)
	{
		std::cout << "Error: Unable to open the input file: \"" << inputFilePath.c_str() << "\"!" << std::endl;
		return ErrCode::ERRCODE_TAB_INVALID_INPUT_FILE;
	}
	else
	{
		std::cout << "Input file \"" << inputFilePath.c_str() << "\" successfully openned!" << std::endl;
	}
	std::ofstream rep(reportFilePath, std::ios_base::app);
	if (!rep)
	{
		std::cout << "Error: Unable to open the report file: \"" << reportFilePath.c_str() << "\"!" << std::endl;
		return ErrCode::ERRCODE_TAB_INVALID_REPORT_FILE;
	}
	else
	{
		std::cout << "Report file \"" << reportFilePath.c_str() << "\" successfully openned!" << std::endl;
	}

	// Write the start message
	std::cout << "Tab parser started!" << std::endl;
	rep       << "Tab parser started!" << std::endl;

	// Determine the file size
	input.seekg(0, std::ios_base::end);
	size_t fileSize = input.tellg();
	input.seekg(0, std::ios_base::beg);
	fileSize -= input.tellg();

	// Perform basic size check
	std::cout << "Tab file size is " << std::dec << fileSize << " bytes!" << std::endl;
	rep       << "Tab file size is " << std::dec << fileSize << " bytes!" << std::endl;
	assert(sizeof(Dword) == 4); // ### TEMP DEBUG
	if (fileSize % (3 * sizeof(Dword)) != 0)
	{
		std::cout << "Tab file size is invalid: must be multiple of 12" << std::endl;
        rep       << "Tab file size is invalid: must be multiple of 12" << std::endl;
		return ErrCode::ERRCODE_TAB_MALFORMED_INPUT_FILE;
	}

	// Allocate memory for the whole file
	std::vector<char> fileData(fileSize);
	if (fileSize != fileData.size())
	{
		std::cout << "Tab file memory allocation failure!" << std::endl;
		rep       << "Tab file memory allocation failure!" << std::endl;
		return ErrCode::ERRCODE_TAB_MEMORY_ALLOCATION_FAILURE;
	}

	// Copy the whole file contents to the memory allocated
	input.read(fileData.data(), fileData.size());
	const char* fileBeg = fileData.data();
	const char* fileEnd = fileBeg + fileSize;
	const char* filePos = fileBeg; // Current position to go

	// Read the header
	{
		TabHeader tabHeader;
		tabHeader.m_unk1 = readDword(filePos);
		tabHeader.m_unk2 = readDword(filePos);
		tabHeader.m_unk3 = readDword(filePos);

		// Check fields
		if (tabHeader.m_unk1 != defaultTabHeaderUnk1)
		{
			std::cout << "Tab header field m_unk1 is invalid!" << std::endl;
			rep       << "Tab header field m_unk1 is invalid!" << std::endl;
			return ErrCode::ERRCODE_TAB_MALFORMED_INPUT_FILE;
		}
		if (tabHeader.m_unk2 != defaultTabHeaderUnk2)
		{
			std::cout << "Tab header field m_unk2 is invalid!" << std::endl;
			rep       << "Tab header field m_unk2 is invalid!" << std::endl;
			return ErrCode::ERRCODE_TAB_MALFORMED_INPUT_FILE;
		}
		if (tabHeader.m_unk3 != defaultTabHeaderUnk3)
		{
			std::cout << "Tab header field m_unk3 is invalid!" << std::endl;
			rep       << "Tab header field m_unk3 is invalid!" << std::endl;
			return ErrCode::ERRCODE_TAB_MALFORMED_INPUT_FILE;
		}

		// Dump fields
		std::cout << "Tab header read!" << std::endl;
		rep       << "Tab header fields are: ";
		rep       << "0x" << std::hex << std::setfill('0') << std::setw(8) << tabHeader.m_unk1;
		rep       << ", ";
		rep       << "0x" << std::hex << std::setfill('0') << std::setw(8) << tabHeader.m_unk2;
		rep       << ", ";
		rep       << "0x" << std::hex << std::setfill('0') << std::setw(8) << tabHeader.m_unk3;
		rep       << std::endl;
	}

	// Cumulative sums might be helpfull...
//	Dword cumulSumUnk1 = 0;
//	Dword cumulSumUnk2 = 0;
//	Dword cumulSumUnk3 = 0;

	// Read the file records
	unsigned recordNumber = 0;
	while (filePos < fileEnd)
	{
		++recordNumber; // The 'normal' ('non-C-programmer') way

		TabFileRecord tabFileRecord;
		tabFileRecord.m_unk1 = readDword(filePos);
		tabFileRecord.m_unk2 = readDword(filePos);
		tabFileRecord.m_unk3 = readDword(filePos);

		// Dump fields
		std::cout << "Tab file record number " << std::dec << recordNumber << " read!" << std::endl;
		rep       << "Tab file record number " << std::dec << recordNumber << " fields are: ";
//		rep	      << "Tab file record fields are (with cumulative sums): ";
		rep	      << "0x" << std::hex << std::setfill('0') << std::setw(8) << tabFileRecord.m_unk1;
		rep	      << ", ";
		rep	      << "0x" << std::hex << std::setfill('0') << std::setw(8) << tabFileRecord.m_unk2;
		rep	      << ", ";
		rep	      << "0x" << std::hex << std::setfill('0') << std::setw(8) << tabFileRecord.m_unk3;
//		cumulSumUnk1 += tabFileRecord.m_unk1;
//		rep       << "0x" << std::hex << std::setfill('0') << std::setw(8) << cumulSumUnk1;
//		rep       << ", ";
//		cumulSumUnk2 += tabFileRecord.m_unk2;
//		rep       << "0x" << std::hex << std::setfill('0') << std::setw(8) << cumulSumUnk2;
//		rep       << ", ";
//		cumulSumUnk3 += tabFileRecord.m_unk3;
//		rep       << "0x" << std::hex << std::setfill('0') << std::setw(8) << cumulSumUnk3;
		rep       << std::endl;
	}

	// Write the finish message
	std::cout << "Tab parser finished successfully!" << std::endl;
	rep       << "Tab parser finished successfully!" << std::endl;

	return ErrCode::ERRCODE_NONE;
}
