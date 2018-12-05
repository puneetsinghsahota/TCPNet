#include "Master_TCP.h"
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <LogWriter.h>

bool Master_TCP::Send(TCPMessage::TCPMessageLIST messageList)
{
	Error::TCPError ERR;
	int size = messageList.size();
	std::string msg = "Attempting to Send Vector of Size -- " + std::to_string(size);
	LogWriter::Write(msg);
	auto start = boost::posix_time::second_clock::local_time();
	bool success = _link->Send(messageList, ERR);
	auto end = boost::posix_time::second_clock::local_time();
	boost::posix_time::time_duration sending_time = end - start;
	msg = "Vector SENT --- Size = " +std::to_string(size) + ", Sending Time = '" + std::to_string(sending_time.total_milliseconds()) + " ms'";
	LogWriter::Write(msg);
	return success;
}

void Master_TCP::SetMastersCount(int num)
{
	_masters_count = num;
}

void Master_TCP::SetFrontEnd(std::string ip_address)
{
	_remote_ip = ip_address;
}

bool Master_TCP::StartObjectSystem()
{
	try {
		LogWriter::Write("Starting Object System {...}");
		_link->ConnectToFrontEnd(_remote_ip, _portNum_ObjSystem);
		boost::thread runMasterThread(boost::bind(&boost::asio::io_service::run, &_io_service));
		_link->StartReceivingObjects();
	}
	catch (std::exception e)
	{
		throw e;
	}
	return true;
}

bool Master_TCP::StartTagSystem()
{
	try {
		LogWriter::Write("Starting Tag System {...}");
		_link->ConnectToFrontEnd(_remote_ip, _portNum_TagSystem);
		boost::thread runMasterThread(boost::bind(&boost::asio::io_service::run, &_io_service));
		_link->StartReceivingTags();
		
	}
	catch (std::exception e)
	{
		throw e;
	}
	return true;
}

bool Master_TCP::Start(bool isTagSystem)
{
	
	bool success = false;
	_link = NetLink::CreateMasterLink(_io_service, _masters_count);
	if (isTagSystem)
	{
		success = StartTagSystem();
	}
	else
	{ 
		success = StartObjectSystem();
	}
	return success;
}

bool Master_TCP::SendTag(std::string commandName, std::string message, int timeOutTime, bool isBlocking)
{
	if (timeOutTime < 0) {
		std::string exception_message = "Time Out Time not in range, Range -> {1,"+ std::to_string(static_cast<unsigned char>(236)) +" }"; ///ASCII code for 236 is Sign of Infinity
		throw exception_message;
	}
	bool success = _link->SendTag(commandName, message, isBlocking,timeOutTime);
	return success;
}

TagSystem::TagList Master_TCP::GetReceivedTags()
{
	TagSystem::TagList rcvd_tags = _link->GetReceivedTags();
	return rcvd_tags;
}

Master_TCP::Master_TCP()
{
	_remote_ip = "NULL";
	_portNum_TagSystem = 1240;
	_portNum_ObjSystem = 1060;
	_masters_count = 1;
}

Master_TCP::~Master_TCP()
{
}
