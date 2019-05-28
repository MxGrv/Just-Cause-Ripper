#include "stdafx.h"
#include "JustCauseRipper.h"
#include "TabParser.h"
#include "ArcParser.h"
#include <fstream>

#include <direct.h> // MSVS C++ directories (folders) operations

JustCauseRipper::JustCauseRipper(const std::list<std::string>& targetFilesPathsList,
	const std::string& outputDirectoryPath) :
	m_targetsList(targetFilesPathsList),
	m_outDirPath(outputDirectoryPath)
{
}

static const std::string extTab = "tab";
static const std::string extArc = "arc";

static const std::string reportFileName = "report.txt";

// Lazy: no letter case checks
static inline bool isExt(const std::string& target, const std::string& ext)
{
	size_t foundDelim = target.find_last_of("\\/");
	std::string targetLastSection = target;
	if (foundDelim != std::string::npos)
		targetLastSection = target.substr(foundDelim + 1);
	else
		targetLastSection = target;

	size_t foundDot = targetLastSection.find_last_of(".");
	if (foundDot == std::string::npos)
		return false;
	targetLastSection = targetLastSection.substr(foundDot + 1);
	return targetLastSection == ext;
}

static inline std::string removeExt(const std::string& target)
{
	size_t foundDelim = target.find_last_of("\\/");
	std::string targetLastSection = target;
	if (foundDelim != std::string::npos)
		targetLastSection = target.substr(foundDelim + 1);
	else
		targetLastSection = target;

	size_t foundDot = targetLastSection.find_last_of(".");
	if (foundDot == std::string::npos)
		return target;
	return targetLastSection.substr(0, foundDot);
}

ErrCode JustCauseRipper::run(const bool doReport)
{
	ErrCode result = ErrCode::ERRCODE_NONE;

//	const OstreamPtr report = std::make_shared<std::ostream>();
//	if (doReport)
//	{
		size_t foundDelim = m_outDirPath.find_last_of("\\/");
		if (foundDelim == std::string::npos)
		{
			std::cout << "Error: Unable to create the report file [delimiter]!" << std::endl;
			return ErrCode::ERRCODE_GEN_DELIMITER_RETRIEVAL_ERROR;
		}
		char delimiter = m_outDirPath[foundDelim];
		std::string reportFilePath = m_outDirPath + delimiter + reportFileName;
//
//		report->open(reportFilePath);
//		if (!report->operator bool())
//		{
//			std::cout << "Error: Unable to open the report file: \"" << reportFilePath.c_str() << "\"!" << std::endl;
//			return ErrCode::ERRCODE_RIPPER_INVALID_REPORT_FILE;
//		}
//		else
//		{
//			std::cout << "Report file \"" << reportFilePath.c_str() << "\" successfully openned!" << std::endl;
//		}
//	}
//	else
//	{
//		std::cout << "No report file will be written! Are you sure you want to proceed?" << std::endl;
//		std::cout << "[Y/N]" << std::endl;
//		char c;
//		std::cin >> c;
//		if (c == 'Y' || c == 'y')
//		{
//			std::cout << "Going on!" << std::endl;
//		}
//		else
//		{
//			std::cout << "Cancelled by the user! Aborting!" << std::endl;
//			return ErrCode::ERRCODE_USER_CANCEL;
//		}
//	}

	bool isTabFileFound = false;
	for (const std::string& target : m_targetsList)
	{
		if (isExt(target, extTab))
		{
			isTabFileFound = true;

//			std::ifstream ifs(target);
//			if (!ifs)
//			{
//				std::cout  << "Error: Unable to open the input file \"" << target.c_str() << "\"!" << std::endl;
//				if (report->operator bool())
//					report << "Error: Unable to open the input file \"" << target.c_str() << "\"!" << std::endl;
//				return ErrCode::ERRCODE_RIPPER_INAVLID_INPUT_FILE;
//			}

			std::cout <<  "Starting the tab parser for \"" << target.c_str() << "\"..." << std::endl;
//			if (report)
//				report << "Starting the tab parser for \"" << target.c_str() << "\"..." << std::endl;

			ErrCode result = TabParser().run(target, reportFilePath);
			if (result != ErrCode::ERRCODE_NONE)
			{
				std::cout  << "The tab parser finished with an error!" << std::endl;
//				if (report)
//					report << "The tab parser finished with an error!" << std::endl;
				return result;
			}

			std::cout <<  "The tab parser finished successfully!" << std::endl;
//			if (report)
//				report << "The tab parser finished successfully!" << std::endl;
		}
		else if (isExt(target, extArc))
		{
			std::ifstream ifs(target);
			if (!ifs)
			{
				std::cout  << "Error: Unable to open the input file \"" << target.c_str() << "\"!" << std::endl;
//				if (report)
//					report << "Error: Unable to open the input file \"" << target.c_str() << "\"!" << std::endl;
				return ErrCode::ERRCODE_RIPPER_INAVLID_INPUT_FILE;
			}

			std::cout  << "Starting the arc parser for \"" << target.c_str() << "\"..." << std::endl;
//			if (report)
//				report << "Starting the arc parser for \"" << target.c_str() << "\"..." << std::endl;

			// Make a dir for the .arc-file
			// ### TODO: ALL

			size_t foundDelim = m_outDirPath.find_last_of("\\/");
			if (foundDelim == std::string::npos)
			{
				std::cout << "Error: Unable to create the report file [delimiter]!" << std::endl;
				return ErrCode::ERRCODE_GEN_DELIMITER_RETRIEVAL_ERROR;
			}
			char delimiter = m_outDirPath[foundDelim];
			std::string reportFilePath = m_outDirPath + delimiter + reportFileName;

			std::string outDirPath = m_outDirPath + delimiter + removeExt(target);
//			int dirCreationResult = _mkdir(outDirPath.c_str());
//			if (dirCreationResult != 0)
//			{
//				std::cout  << "Error: Unable to create directory \"" << outDirPath.c_str() << "\"!" << std::endl;
////				if (report)
////					report << "Error: Unable to create directory \"" << outDirPath.c_str() << "\"!" << std::endl;
//
//				if (errno == EEXIST)
//				{
//					std::cout  << "Error: The directory already exists!" << std::endl;
////					if (report)
////						report << "Error: The directory already exists!" << std::endl;
//				}
//				else if (errno == ENOENT)
//				{
//					std::cout  << "Error: The directory path not found!" << std::endl;
////					if (report)
////						report << "Error: The directory path not found!" << std::endl;
//				}
//				else
//				{
//					std::cout  << "Error: Unknown error!" << std::endl;
////					if (report)
////						report << "Error: Unknown error!" << std::endl;
//				}
//
//				return ErrCode::ERRCODE_GEN_UNABLE_TO_CREATE_DIRECTORY;
//			}

			ErrCode result = ArcParser().run(target, outDirPath, reportFilePath);
			if (result != ErrCode::ERRCODE_NONE)
			{
				std::cout  << "The arc parser finished with an error!" << std::endl;
//				if (report)
//					report << "The arc parser finished with an error!" << std::endl;
				return result;
			}

			std::cout  << "The arc parser finished successfully!" << std::endl;
//			if (report)
//				report << "The arc parser finished successfully!" << std::endl;
		}
		else
		{
			std::cout  << "Found an unexpected file \"" << target.c_str() << "\"!" << std::endl;
//			if (report)
//				report << "Found an unexpected file \"" << target.c_str() << "\"!" << std::endl;
			return ErrCode::ERRCODE_RIPPER_INAVLID_UNKNOWN_FILE;
		}
	}

	std::cout  << "Successfully ripped!" << std::endl;
//	if (report)
//		report << "Successfully ripped!" << std::endl;

	return result;
}
