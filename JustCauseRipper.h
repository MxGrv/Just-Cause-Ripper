#pragma once

#include "Error.h"
#include <string>
#include <list>

// Just Cause ripper
// Receives a list of files and calls the appropriate handlers
//  Target files are only a single .tab-file and one or several .arc-files
// Receives the target directory path
// Also manages reporting

static const std::list<std::string> typicalTargetFilesNamesList = 
{
	"pc.tab",
	"pc0.arc",
	"pc1.arc",
	"pc2.arc",
	"pc3.arc",
	"pc4.arc",
};

class JustCauseRipper
{
public:

	JustCauseRipper(const std::list<std::string>& targetFilesPathsList,
		const std::string& outputDirectoryPath);
	~JustCauseRipper() = default;

	ErrCode run(const bool doReport = true);

private:

	const std::list<std::string> m_targetsList;
	const std::string m_outDirPath;
};
