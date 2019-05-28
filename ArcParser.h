#pragma once

#include "Error.h"
#include <iostream>
#include <vector>
#include <string>

// .arc-file parser
// ### Temporarily cannot aggregate the "pc0.arc" file

class ArcParser
{
public:

	ArcParser() = default;
	~ArcParser() = default;

	ErrCode run(const std::string& inputFilePath,
		const std::string& outputDirPath,
		const std::string& reportFilePath);

private:

	static const Dword defaultArcSarcHeaderUnk1 = 0x00'00'00'04;
	static const int defaultArcSarcHeaderUnk1Str[4];
	static const Dword defaultArcSarcHeaderSignature = 0x43'52'41'53; // In ASCII: "SARC"
	static const char defaultArcSarcHeaderSignatureStr[5];
	static const Dword defaultArcSarcHeaderUnk2 = 0x00'00'00'02;
	static const size_t minArcSarcFileRecordNameSize = 5; // Restricted by the ArcSarcHeader.m_unk3 field (see aggregateSarcRecord(...))
	static const size_t maxArcSarcFileRecordNameSize = 128; // Not so reasonable check, but anyway... Rather artifitial restriction, anyway

	struct ArcSarcHeader
	{
		Dword m_unk1; // Possibly the version
		Dword m_signature;
		Dword m_unk2; // Possibly the version
		Dword m_unk3; // Possibly the final record ending offset (relatively from the SARC record beginning), aligned by 16-bytes border (to the lower bound)
		// ### TODO: UNK3 to a more reasonable description
	};

	struct ArcSarcFileRecord
	{
		Dword m_nameSize;
		std::string m_nameStr;
		Dword m_offset; // Relative from ... // ### ?
		Dword m_size;
	};

//	struct ArcSarcRecord
//	{
//		ArcSarcHeader m_header;
//		std::vector<ArcSarcFileRecord> m_recordsList;
//		std::vector<char> m_mergedData;
//	};

	ErrCode findSarcHeaders(std::vector<size_t>& headers, const size_t fileSize);
	ErrCode aggregateSarcRecord(const size_t offset, const std::string& curArcDirPath);
	ErrCode saveFile(const size_t offsetAbsolute, const size_t size, std::ofstream& outFileStream);
};
