#include "Tag.h"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <thread>
#include <string>
#include <LogWriter.h>

Tag::Tag(std::string tagString) 
{
	_size_of_ackID = 2;
	BreakTagString(tagString);
	_timeOutTime = 20;
	_blocking = false;
}

Tag::Tag(std::string commandName,std::string message,int ackID,bool isMaster) 
{
	_size_of_ackID = 2;
	if (isMaster) {
		_msg_delimiter = ";";
		_ack_delimiter = ":";
	}
	else {
		_msg_delimiter = ":";
		_ack_delimiter = ";";
	}
	SetAckID(ackID);
	_command_name = commandName;
	_message = message;
	_blocking = false;
	_timeOutTime = 20;
	_timed_OUT = false;
}

Tag::~Tag()
{
}

void Tag::SetMessageDelimiter(std::string delimiter)
{
	if (delimiter == ":") {
		_msg_delimiter = delimiter;
		_ack_delimiter = ";";
	}
	else if (delimiter == ";") {
		_msg_delimiter = delimiter;
		_ack_delimiter = ":";
	}
	else {
		throw "Wrong Delimiter Used";
	}
}

int Tag::Identity()
{
	return _ack_ID;
}

void Tag::BreakTagString(std::string tagString)
{
	int start_pos = 0;
	int first_colon_pos = tagString.find(":", start_pos);
	SetMessageDelimiter(":");
	if (first_colon_pos == std::string::npos) {
		SetMessageDelimiter(";");
		first_colon_pos = tagString.find(";", start_pos);
	}
	int first_exclamation_pos = tagString.find("!", start_pos);
	int size = first_colon_pos - start_pos;
	std::string cmd = tagString.substr(start_pos, size);
	size = (first_exclamation_pos - (first_colon_pos + 1));
	std::string msg = tagString.substr((first_colon_pos + 1), size);
	size = _size_of_ackID;
	std::string ack = tagString.substr(first_exclamation_pos + 1,size);
	_ack_ID = std::stoi(ack);
	_command_name = cmd;
	_message = msg;
	_acknowledgementString = ack;
}

void Tag::SetTimeOut(int timeout)
{
	_timeOutTime = timeout;
}

void Tag::SetIsBlocking(bool enable)
{
	_blocking = enable;
}

void Tag::SetAckID(int ackid)
{
	ackid = ackid % 100;
	_ack_ID = ackid;
	if (ackid / 10 == 0) {
		_acknowledgementString = "0" + std::to_string(ackid);
	}
	else {
		_acknowledgementString = std::to_string(ackid);
	}
}

void Tag::SetString(std::string tagString)
{
	BreakTagString(tagString);
}

void Tag::SetMessage(std::string message)
{
	_message = message;
}

void Tag::SetCommandName(std::string cmdName)
{
	_command_name = cmdName;
}

void Tag::StartTimer() {
	_ack_rcvd = false;
	_timed_OUT = false;
	_start_time = boost::asio::deadline_timer::traits_type::now();
	auto tagStartTime = _start_time;
	_expiry_time = tagStartTime + boost::posix_time::milliseconds(_timeOutTime);
	std::string msg = "StartTimer(...) -> {'" + ToString() + "' - Time Out Timer Triggered (Expiry Time = " + boost::posix_time::to_simple_string(_expiry_time)
		+ ", Start Time = " + boost::posix_time::to_simple_string(_start_time) + ")}";
	LogWriter::Write(msg);
}

int Tag::GetTimeOutTime()
{
	return _timeOutTime;
}

bool Tag::IsBlocking()
{
	return _blocking;
}

std::string Tag::AcknowledgeString()
{
	std::string ackString = "AK" + _ack_delimiter+"!" +_acknowledgementString;
	return ackString;
}

std::string Tag::Message()
{
	return _message;
}

std::string Tag::CommandName()
{
	return _command_name;
}

void Tag::SetAck_RCVD(bool success)
{
	_ack_rcvd = success;
	std::string msg = "TAG :: { '" + ToString() + "' } - Ack - SUCCESS";
	if (success == true && !_timed_OUT) {
		LogWriter::Write(msg);
	}
}

bool Tag::Ack_Received()
{
	return _ack_rcvd;
}

bool Tag::IsTimedOut()
{
	return _timed_OUT;
}

void Tag::TimeOutHandler()
{
	if (!_ack_rcvd && !_timed_OUT)
	{
		std::string error_msg = "TAG :: { '" + ToString() + "' } - #### TIMED OUT ####";
		if (_blocking) {
			throw error_msg;
		}
		else if (!_ack_rcvd) {
			LogWriter::Write(error_msg);
		}
		_timed_OUT = true;
	}
}

std::string Tag::ToString()
{
	std::string tagString = _command_name + _msg_delimiter + _message + "!" + _acknowledgementString;
	return tagString;
}

boost::posix_time::ptime Tag::GetStartTime()
{
	return _start_time;
}

boost::posix_time::ptime Tag::GetExpiryTime()
{
	return _expiry_time;
}

Tag::Tag()
{
	_size_of_ackID = 2;
	_command_name = "XX";
	SetMessageDelimiter(":");
	SetAckID(0);
	_message = "XXX";
	_timeOutTime = 20;
	_blocking = false;
	_timed_OUT = false;
}
