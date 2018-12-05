#include "Error.h"

Error::TCPError Error::create(std::string ProcessName)
{
	return TCPError(new Error(ProcessName));
}

bool Error::isSuccess()
{
	return _isSuccess;
}

Error::Error(std::string ProcessName)
{
	_isSuccess = true;
	_processName = ProcessName;
	_ErrorCount = 0;
}

void Error::PushError(std::string error_str)
{	
	_isSuccess = false;
	std::string errorMessage = "EXCEPTION :: " + error_str;
	_ErrorList.push_back(errorMessage);
	_ErrorCount++;
}

Error::~Error()
{
}
