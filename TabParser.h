#pragma once

#include "Error.h"
#include <iostream>
//#include <vector>

// .tab-file parser
// No actual .tab-file possible malformation checks performed

class TabParser
{
public:

	TabParser() = default;
	~TabParser() = default;

	ErrCode run(const std::string& inputFilePath,
		const std::string& reportFilePath);

private:

	static const Dword defaultTabHeaderUnk1 = 0x00'00'00'03;
	static const Dword defaultTabHeaderUnk2 = 0x00'00'08'00;
	static const Dword defaultTabHeaderUnk3 = 0x00'00'00'05;

	struct TabHeader
	{
		Dword m_unk1; // Possibly the version
		Dword m_unk2; // Possibly the block size
		Dword m_unk3; // Possibly yet another version info, or amount of .arc-files, this .tab-file refers to
	};

	struct TabFileRecord
	{
		Dword m_unk1; // Possibly the file offset or mapping info
		Dword m_unk2; // Possibly the file offset or mapping info
		Dword m_unk3; // Possibly the file size
	};

//	struct TabRecord
//	{
//		TabHeader m_header;
//		std::vector<TabFileRecord> m_recordsList;
//	};
};
