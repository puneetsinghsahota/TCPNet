#include "TagSystem.h"
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <algorithm>
#include <future>
#include <chrono>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/container/deque.hpp>
#include <LogWriter.h>

bool TagSystem::TAG_Q_CheckTag(Tag &tag)
{
	bool flag = (tag.Ack_Received() || tag.IsTimedOut());
	return flag;
}

bool TagSystem::TAG_Q_CheckAck(Tag& tag,int ack_id)
{
	bool found = false;
	if (ack_id == tag.Identity()) 
	{
		found = true;
		tag.SetAck_RCVD(found);
		std::string msg = "Acknowledgement Matched -- Tag -> {'" + tag.ToString() + "'}";
		LogWriter::Write(msg);
	}
	return found;
}

void TagSystem::TRY_Clear_SentTags_Queue()
{
	if (_SentTags_Queue.size() == 100) {

		_SentTags_Queue.erase(std::remove_if(_SentTags_Queue.begin(), _SentTags_Queue.end(), boost::bind(&TagSystem::TAG_Q_CheckTag, this, _1)), _SentTags_Queue.end());
		std::string msg = "Sent Tag Queue Cleared --- Number of Tags Before = 100 , Now = "+std::to_string(_SentTags_Queue.size());
		LogWriter::Write(msg);
	}
}

void TagSystem::TCPSendAck(Tag& tag)
{
	
	std::string ack_ID = tag.AcknowledgeString();
	std::ostringstream tag_ack(ack_ID);
	std::string msg = "RECEIVED :: {'" + tag.ToString() + "'} -> Trying to Send Ack - {'" + ack_ID + "'} ...";
	LogWriter::Write(msg);
	
	boost::asio::async_write(*_socket
		, boost::asio::buffer(tag_ack.str(), tag_ack.str().size())
		, boost::bind(TagSystem::HandleAckWrite));
	msg = "Ack - {'" + ack_ID + "'} Sent Succesfully";
	LogWriter::Write(msg);
	
}

void TagSystem::HandleWrite()
{
	std::string msg = "Tag Sent Handler Called";
	LogWriter::Write(msg);
}

void TagSystem::HandleAckWrite() {
	std::string msg = "Sending Ack Handler Called";
	LogWriter::Write(msg);
}

void TagSystem::CheckAcknowledgement(std::string tagString) {
	Tag ack(tagString);
	int ack_id = ack.Identity();
	std::string msg = "Checking Received Acknowledgement , {'" + ack.ToString() + "'}";
	LogWriter::Write(msg);
	if (_SentTags_Queue.size() > 0) {
		std::find_if(_SentTags_Queue.begin(), _SentTags_Queue.end(),boost::bind(&TagSystem::TAG_Q_CheckAck,this,_1,ack_id));
	}
}

TagSystem::TagList TagSystem::ParseTags(std::string tagMessage) {
	LogWriter::Write("Trying To Parse Received Tag Buffer ... ");
	TagSystem::TagList tagVector;
	tagMessage = _unparsed_buf + tagMessage;
	_unparsed_buf = "";
	while (tagMessage.length() != 0) {
		Tag tag;
		ParseFirstTag(&tagMessage, tag);
		if (tag.CommandName() != "XX") {
			tagVector.push_back(tag);
		}
		TRY_Clear_SentTags_Queue();
	}
	return tagVector;
}

void TagSystem::WriteTagOnSocket(Tag& tag)
{
	std::string socket_msg = tag.ToString();
	std::ostringstream archive_stream(socket_msg);
	
	boost::asio::async_write(*_socket
		, boost::asio::buffer(archive_stream.str(), archive_stream.str().size())
		, boost::bind(TagSystem::HandleWrite));
}

void TagSystem::SendIfSocketOpen(Tag& tag)
{
	if (_socket->is_open()) 
	{
		WriteTagOnSocket(tag);
	}
}

TagSystem::TagList TagSystem::ReadTagsFromSocket()
{
	std::string read_until_delimiter = "!";
	boost::asio::streambuf buffer_stream;
	LogWriter::Write("Trying To Read Bytes From The Socket");
	int bytesRead = boost::asio::read_until(*_socket, buffer_stream, read_until_delimiter);
	LogWriter::Write("Read these bytes From Socket Until '!' ");
	TagSystem::TagList tagVector;
	if (bytesRead >= 0) {
		std::string tagMessage((std::istreambuf_iterator<char>(&buffer_stream)), std::istreambuf_iterator<char>());
		std::string msg = "Read Number OF Bytes  = " + std::to_string(tagMessage.size());
		LogWriter::Write(msg);
		tagVector = ParseTags(tagMessage);
	}
	else {
		throw "Socket Error : While Receiving Tags";
	}
	return tagVector;
}

void TagSystem::ParseFirstTag(std::string *tagMessage,Tag& parsed_Tag)
{
	LogWriter::Write("Trying To Parse First Tag in Tag Buffer ... ");
	std::string msg = "Tag Buffer = \"" + *tagMessage + "\"";
	LogWriter::Write(msg);
	int exclaimation_pos = tagMessage->find('!', 0);
	if (exclaimation_pos == std::string::npos) {
		_unparsed_buf = *tagMessage;
		*tagMessage = "";
	}
	else {
		int singleTagSize = exclaimation_pos + 2 + 1;
		if (singleTagSize <= tagMessage->length()) {
			std::string tagString = tagMessage->substr(0, singleTagSize);
			ClassifyAndGetTag(tagString,parsed_Tag);
			RemoveParsedCharactersFromBuffer(singleTagSize,tagMessage);
		}
		else {
			_unparsed_buf = *tagMessage;
			*tagMessage = "";
		}
	}
}

bool TagSystem::ClassifyAndGetTag(std::string tagString, Tag& parsed_Tag)
{
	std::string msg = "Parsed First Tag :: " + tagString + ", Trying to Classify Tag ... " ;
	LogWriter::Write(msg);
	bool isAck = false;
	if (tagString.find("AK") == 0) {
		isAck = true;
		LogWriter::Write("Tag Detected as ACKNOWLEDGEMENT");
		boost::thread checkAck(boost::bind(&TagSystem::CheckAcknowledgement, this, tagString));
	}
	else {
		LogWriter::Write("Tag Detected as a Normal TAG");
		parsed_Tag.SetString(tagString);
		boost::thread sendAck(boost::bind(&TagSystem::TCPSendAck,this,parsed_Tag));
	}
	return isAck;
}

void TagSystem::RemoveParsedCharactersFromBuffer(int tagSize, std::string * tagMessage)
{
	LogWriter::Write("Refreshing Buffer - Removing Parsed Characters");
	if (tagSize == tagMessage->length()) {
		*tagMessage = "";
	}
	else {
		*tagMessage = tagMessage->substr(tagSize);
	}
}

void TagSystem::SendTag(std::string cmd_name,std::string message,bool blocking,int timeOutTime)
{
	Tag tag(cmd_name, message, _SentTags_Queue.size(), _isMaster);
	tag.SetIsBlocking(blocking);
	tag.SetTimeOut(timeOutTime);

	_SentTags_Queue.push_back(tag);
	

	std::string tag_string = tag.ToString();
	std::string msg = "SENDING TAG : { '" + tag_string + "' }";
	LogWriter::Write(msg);
	_SentTags_Queue.back().StartTimer();
	SendIfSocketOpen(tag);
	
}

void TagSystem::ReceiveTags()
{
	while (true) {
		int readable_bytes = 0;
		readable_bytes = _socket->available();
		if (readable_bytes > 0) {
			LogWriter::Write("Some Bytes Available To Read !!!");
			TagSystem::TagList tagVector = ReadTagsFromSocket();
			for (auto &tag : tagVector)
			{
				std::string msg = "Received Number Of Tags = " + std::to_string(tagVector.size()) + ", Pushing these TAGS inside RCVD Tags Queue";
				LogWriter::Write(msg);
				_receivedTags_list.push_back(tag);
			}
			tagVector.clear();
		}
	}
}

void TagSystem::SetSocket(boost::asio::ip::tcp::socket* tag_socket, bool isMaster)
{
	LogWriter::Write("Setting Up the Socket for Tag System .. . Socket Set");
	_isMaster = isMaster;
	_socket = tag_socket;
}

TagSystem::TagSystem() : _TO_heartbeat(100,&_SentTags_Queue)
{
	_unparsed_buf = "";
	_max_queue_size = 100;
}

TagSystem::~TagSystem()
{
}
