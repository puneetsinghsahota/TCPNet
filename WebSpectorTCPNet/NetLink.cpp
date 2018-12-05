#include "NetLink.h"
#include "MasterLink.h"
#include "FrontEndLink.h"
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <LogWriter.h>

NetLink::MultipleLinks NetLink::CreateMultipleLinks_ToMASTERS(boost::asio::io_service &fe_io_service,unsigned short local_port, int numOfMasters)
{
	LogWriter::Write("Trying To Connect To Multiple Masters");
	MultipleLinks masters;
	for (int master_num = 0; master_num < numOfMasters; master_num++,local_port++)
	{
		NetLink::Link FE_Link = NetLink::CreateFrontEndLink(fe_io_service, local_port);
		FE_Link->ConnectToMaster();
		masters[master_num] = FE_Link;
	}
	return masters;
}

TagSystem::TagList NetLink::GetReceivedTags()
{
	TagSystem::TagList received_tags;
	while (received_tags.size() <= 0)
	{
		if (_Tag_System._receivedTags_list.size() > 0)
		{
			for (auto &tag : _Tag_System._receivedTags_list)
			{
				received_tags.push_back(tag);
			}			
			_Tag_System._receivedTags_list.clear();
		}
	}
	
	return received_tags;
}

void NetLink::WaitForConnections()
{
	while (!_connectionMade)
	{
		LogWriter::Write("Waiting For Connection ... ");
		boost::this_thread::sleep(boost::posix_time::milliseconds(50));
	}
}

size_t NetLink::TCPSend(TCPMessage * message, Error::TCPError e)
{
	size_t NumOfBytesSent = _Connection->Send(message, e);
	return NumOfBytesSent;
}

NetLink::Link NetLink::CreateMasterLink(boost::asio::io_service & MasterPC_io_service,int num_of_masters)
{
	return Link(new MasterLink(MasterPC_io_service,num_of_masters));
}

NetLink::Link NetLink::CreateFrontEndLink(boost::asio::io_service & FE_PC_io_service, unsigned short local_port)
{
	LogWriter::Write("Creating a Link From FrontEnd To Master From FrontEnd System");
	return Link(new FrontEndLink(FE_PC_io_service,local_port));
}

void NetLink::Send(TCPMessage * message, Error::TCPError e)
{
	TCPMessage::TCPMessageLIST messageVector;
	messageVector.push_back(message);
	Send( messageVector, e);
}

bool NetLink::Send(TCPMessage::TCPMessageLIST messageVector, Error::TCPError e)
{
	bool success = false;
	int numOfBytesSent = TCPSendVector(messageVector, e);
	if (numOfBytesSent > 0)
	{
		success = true;
	}
	return success;
}

int NetLink::TCPSendVector(TCPMessage::TCPMessageLIST messageVector, Error::TCPError e)
{
	int count = messageVector.size();
	size_t sentBytes = 0;

	sentBytes = _Connection->Send_10ByteMSGCount(count, e);

	size_t NumOfBytesSent = 0;
	if (sentBytes != 0)
	{
		for (auto &message : messageVector)
		{
			size_t bytesSent = TCPSend(message, e);
			if (bytesSent == 0)
			{
				NumOfBytesSent = 0;
				return NumOfBytesSent;
			}
			else
			{
				NumOfBytesSent += bytesSent;
			}
		}
	}
	return NumOfBytesSent;
}

void NetLink::ConnectToFrontEnd(std::string IPAddress_Remote, unsigned short portNum)
{
	if (_isMaster == false)
	{
		throw "Trying to Connect FrontEnd To FrontEnd - EXCEPTION";
	}
}

void NetLink::StartReceivingTags()
{
	WaitForConnections();
	boost::thread receiveTagsThread(boost::bind(&NetLink::ReceiveTags, this));
}

void NetLink::ReceiveTags()
{
	LogWriter::Write("Starting To Receive Tags");
	_Tag_System.ReceiveTags();
}

void NetLink::StartReceivingObjects()
{
	WaitForConnections();
	boost::thread receiveObjectsThread(boost::bind(&NetLink::StartReceiver, this));
}

void NetLink::ConnectToMaster()
{
	if (_isMaster)
	{
		throw "Trying To Connect Master to Master - EXCEPTION";
	}
}

void NetLink::StartReceiver()
{
	LogWriter::Write("Receiver Started {...}");
	int ReceivedObjsCount = 0;
	while (ReceivedObjsCount >= 0)
	{
		ReceivedObjsCount = _Connection->Receive_10ByteMSGCount();
		if (ReceivedObjsCount > 0)
		{
			
			TCPMessage::TCPMessageLIST list;
			auto start = boost::posix_time::second_clock::local_time();
			int result = _Connection->Receive(ReceivedObjsCount, &list);
			auto end = boost::posix_time::second_clock::local_time();
			boost::posix_time::time_duration receivingTime = end - start;
			std::string msg = "Receiving Time  = '" + std::to_string(receivingTime.total_milliseconds()) + " ms', Size = " + std::to_string(list.size());
			LogWriter::Write(msg);
			if (result == (-1))
			{
				throw "Socket Error :: While Receiving Object - Socket closed Abruptly";
			}
		}
		else if (ReceivedObjsCount < 0)
		{
			throw "Socket Error :: While Receiving Object - Non-parsable Data On Socket";
		}
		std::string message = "Number Of Objects Received = " + std::to_string(ReceivedObjsCount);
		LogWriter::Write(message);
	}
}

bool NetLink::SendTag(std::string command, std::string message, bool isBlocking, int timeOutTime)
{
	try
	{
		_Tag_System.SendTag(command, message, isBlocking, timeOutTime);
		return true;
	}
	catch (std::exception e)
	{
		return false;
	}
	
}

NetLink::NetLink()
{
}

NetLink::~NetLink()
{
}
