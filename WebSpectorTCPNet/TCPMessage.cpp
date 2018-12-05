#include "TCPMessage.h"
#include <sstream>
#include <iostream>
#include <exception>
#include <boost/make_shared.hpp>

std::string TCPMessage::Get_MSG_Size_20Bytes(size_t write_len)
{
	std::string messageSize = std::to_string(write_len);
	int numOfDigits = messageSize.length();
	for (int i = 0; i < (20 - numOfDigits); i++)
	{
		messageSize = "0" + messageSize;
	}
	return messageSize;
}

boost::asio::const_buffers_1 TCPMessage::Str_To_Buffer(std::string MSG)
{
	std::ostringstream archive(MSG);
	return boost::asio::buffer(archive.str(), archive.str().size());
}

void TCPMessage::SetMSG_Type(std::string MSG_Type)
{
	_MSG_Type = MSG_Type;
}

std::string TCPMessage::GetMSG_Type()
{
	return _MSG_Type;
}

std::string TCPMessage::GetMSG_Size()
{
	return _MSG_Size;
}

size_t TCPMessage::Get_Expected_MSG_Size()
{
	std::string sizeStringAccurate = _MSG_Size.substr(0, 20);
	int expectedNumberOfBytes = std::stoi(sizeStringAccurate);
	return expectedNumberOfBytes;
}

void TCPMessage::SetUp_MSG_Stream(boost::asio::streambuf::const_buffers_type MSG_Buffer, size_t buf_size, std::string MSG_Type)
{
	size_t copiedBytes = boost::asio::buffer_copy(_MSG_Stream.prepare(buf_size), MSG_Buffer);
	_MSG_Stream.commit(copiedBytes);
	SetMSG_Type(MSG_Type);
	_MSG_Size = Get_MSG_Size_20Bytes(_MSG_Stream.size());
}

TCPMessage::TCPMessage(boost::asio::streambuf::const_buffers_type MSG_Buffer,size_t buf_size, std::string MSG_Type)
{
	size_t copiedBytes = boost::asio::buffer_copy(_MSG_Stream.prepare(buf_size),MSG_Buffer);
	_MSG_Stream.commit(copiedBytes);
	SetMSG_Type(MSG_Type);
	_MSG_Size = Get_MSG_Size_20Bytes(_MSG_Stream.size());
}

TCPMessage::TCPMessage()
{
}

TCPMessage::~TCPMessage()
{
}
