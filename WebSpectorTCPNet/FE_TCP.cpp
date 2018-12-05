#include "FE_TCP.h"
#include <boost/thread.hpp>
#include <LogWriter.h>

FE_TCP::FE_TCP()
{
	_masters_count = 1;
	_portNum_ObjSystem = 1060;
	_portNum_TagSystem = 1240;
}

FE_TCP::~FE_TCP()
{
}

bool FE_TCP::BroadcastObj(TCPMessage::TCPMessageLIST messageList)
{
	bool success = false;
	for (int masterID = 0; masterID < _masters_count; masterID++)
	{
		Error::TCPError ERR;
		success = _link_pool[masterID]->Send(messageList, ERR);
	}
	return success;
}

bool FE_TCP::Send(int masterID, TCPMessage::TCPMessageLIST messageList)
{
	Error::TCPError ERR;
	bool success = _link_pool[masterID]->Send(messageList, ERR);
	return success;
}

bool FE_TCP::StartTagSystem()
{
	try {
		LogWriter::Write("Starting Tag System {...}");
		_link_pool = NetLink::CreateMultipleLinks_ToMASTERS(_io_service, _portNum_TagSystem, _masters_count);
		boost::thread runFrontEnd(boost::bind(&boost::asio::io_service::run, &_io_service));
		for (int masterID = 0; masterID < _masters_count; masterID++)
		{
			_link_pool[masterID]->StartReceivingTags();
		}
	}
	catch (std::exception e)
	{
		throw e;
	}
	return true;
}

bool FE_TCP::StartObjectSystem()
{
	try {
		LogWriter::Write("Starting Object System {...}");
		_link_pool = NetLink::CreateMultipleLinks_ToMASTERS(_io_service, _portNum_ObjSystem, _masters_count);
		boost::thread runFrontEnd(boost::bind(&boost::asio::io_service::run, &_io_service));
		for (int masterID = 0; masterID < _masters_count; masterID++)
		{
			_link_pool[masterID]->StartReceivingObjects();
		}
	}
	catch (std::exception e)
	{
		throw e;
	}
	return true;
}

void FE_TCP::SetMastersCount(int numOfMasters)
{
	_masters_count = numOfMasters;
}

bool FE_TCP::Start(bool isTagSystem)
{
	bool success = true;
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

bool FE_TCP::BroadcastTag(std::string commandName, std::string message, int timeOutTime, bool isBlocking)
{
	bool success = false;
	if (timeOutTime < 0) {
		std::string exception_message = "Time Out Time not in range, Range -> {1," + std::to_string(static_cast<unsigned char>(236)) + " }"; ///ASCII code for 236 is Sign of Infinity
		throw exception_message;
	}
	for (int masterID = 0; masterID < _masters_count; masterID++)
	{
		success = _link_pool[masterID]->SendTag(commandName, message, isBlocking, timeOutTime);
	}
	return success;
}

bool FE_TCP::SendTag(int masterID, std::string commandName, std::string message, int timeOutTime, bool isBlocking)
{
	if (timeOutTime < 0) {
		std::string exception_message = "Time Out Time not in range, Range -> {1," + std::to_string(static_cast<unsigned char>(236)) + " }"; ///ASCII code for 236 is Sign of Infinity
		throw exception_message;
	}
	bool success = _link_pool[masterID]->SendTag(commandName, message, isBlocking, timeOutTime);
	return success;
}

TagSystem::TagList FE_TCP::GetAllReceivedTags()
{
	TagSystem::TagList rcvd_tags;
	for (int masterID = 0; masterID < _masters_count; masterID++)
	{
		TagSystem::TagList rcvdTags = _link_pool[masterID]->GetReceivedTags();
		for (auto &tag : rcvdTags)
		{
			rcvd_tags.push_back(tag);
		}
	}
	return rcvd_tags;
}

TagSystem::TagList FE_TCP::GetReceivedTagsFrom(int masterID)
{
	TagSystem::TagList rcvd_tags = _link_pool[masterID]->GetReceivedTags();
	return rcvd_tags;
}

