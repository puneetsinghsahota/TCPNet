#pragma once
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>


/**
	Used to store the errors encountered while the System goes through
	all kinds of TCP Communication and parsing through the received buffer
*/
class Error
{
private:

	/**
		is true if the Communication is a Success
	*/
	bool _isSuccess;

	/**
		An independent process Name for the Error List
	*/
	std::string _processName;

	/** 
		A list of errors or warnings in Complete Execution OF TCP Communication
	*/
	std::vector<std::string> _ErrorList; 
	
	/** 
		Total Number of Errors
	*/
	int _ErrorCount; 

public:

	/**
		A shared Reference Type of the Error class object
	*/
	typedef boost::shared_ptr<Error> TCPError;

	/**
		Used to create a reference type TCPError -> boost::shared_ptr<Error> with an independent Process Name
	*/
	static Error::TCPError create(std::string ProcessName);

	/**
		To Check if the process was Success or not
		Returns true if no Errors
	*/
	bool isSuccess();

	/**
		Used to push Error inside Error List
	*/
	void PushError(std::string error_str);

	/**
		A constructor which takes in the Independent Process Name for the Error List
	*/
	Error(std::string ProcessName);

	~Error();
};

