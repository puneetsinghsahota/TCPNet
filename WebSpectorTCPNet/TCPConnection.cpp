#include "TCPConnection.h"
#include <exception>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <LogWriter.h>

int TCPConnection::GetBytesToBeReceived_20Bytes()
{
	boost::array<char, 20> messageSizeBuffer{};
	int bytesRead = 0;
	while (bytesRead != 20)
	{
		bytesRead = _TCPSocket.read_some(boost::asio::buffer(messageSizeBuffer));
	}
	std::string messageSizeString(messageSizeBuffer.data());
	messageSizeBuffer.empty();
	std::string sizeStringAccurate = messageSizeString.substr(0, 20);
	int expectedNumberOfBytes = std::stoi(sizeStringAccurate);
	return expectedNumberOfBytes;
}

TCPConnection::Connection TCPConnection::CreateConnection(boost::asio::io_service& io_service, bool connType)
{
	return Connection(new TCPConnection(io_service));
}

boost::asio::ip::tcp::socket & TCPConnection::GetSocket()
{
	return _TCPSocket;
}

size_t TCPConnection::Send(TCPMessage *message, Error::TCPError e)
{
	size_t NumOfBytesSent = 0;
	try
	{
		std::ostringstream archive_stream(message->GetMSG_Size());
		//Write the message Size To The SOCKET first
		boost::asio::write(_TCPSocket, boost::asio::buffer(archive_stream.str(), archive_stream.str().size()));
		//Write the actual Message Buffer to the SOCKET
		NumOfBytesSent = boost::asio::write(_TCPSocket, message->_MSG_Stream);

	}
	catch (std::exception ex)
	{
		std::string ErrorMessage = ex.what();
		e->PushError(ErrorMessage);
		throw ex;
	}
	return NumOfBytesSent;
}

size_t TCPConnection::Send_10ByteMSGCount(int count, Error::TCPError e)
{
	std::string messageSize = std::to_string(count);
	int numOfDigits = messageSize.length();
	
	//Get A 10 Byte Message String 
	for (int i = 0; i < (10 - numOfDigits); i++)
	{
		messageSize = "0" + messageSize;
	}
	std::ostringstream archive_stream(messageSize);

	//Write the Object Count To The SOCKET first
	size_t result = boost::asio::write(_TCPSocket, boost::asio::buffer(archive_stream.str(), archive_stream.str().size()));
	return result;
}

int TCPConnection::Receive(int NumOfObjs, TCPMessage::TCPMessageLIST * list)
{
	int ObjCount = 0;
	for (ObjCount = 0; ObjCount < NumOfObjs; ObjCount++)
	{
		LogWriter::Write("Trying to Get Size Of Top TCPMessage in Socket Queue {...}");
		int expectedBytes = GetBytesToBeReceived_20Bytes();
		std::string msg = "Size Of top TCPMessage { '" + std::to_string(expectedBytes) + " Characters' }";
		LogWriter::Write(msg);
		boost::asio::streambuf stream_buffer;
		int receivedBytes = 0;
		int bytesToBeReceived = expectedBytes;
		while (receivedBytes != expectedBytes)
		{
			boost::asio::streambuf::mutable_buffers_type readBuffer = stream_buffer.prepare(bytesToBeReceived);
			int bytes = _TCPSocket.read_some(readBuffer);
			receivedBytes += bytes;
			stream_buffer.commit(bytes);
			bytesToBeReceived = expectedBytes - receivedBytes;
		}
		TCPMessage *message = new TCPMessage();
		message->SetUp_MSG_Stream(stream_buffer.data(), stream_buffer.size(), "MNO");
		list->push_back(message);
		LogWriter::Write("TCPMessage Received Succesfully");
	}
	return ObjCount;
}

int TCPConnection::Receive_10ByteMSGCount()
{
	int result = 0;
	while (result==0)
	{
		boost::array<char, 10> objCountBuffer{};
		int bytesRead = _TCPSocket.read_some(boost::asio::buffer(objCountBuffer));
		std::string objCountString(objCountBuffer.data());
		objCountBuffer.empty();
		if (bytesRead == 10)
		{
			result = std::stoi(objCountString);
			std::string msg = "Received 10 Charecters Long Message Count -- " + std::to_string(result);
			LogWriter::Write(msg);
		}
		else if (bytesRead > 0 && bytesRead < 10)
		{
			LogWriter::Write("SOCKET ERROR");
			result = (-1);
		}
	}
	return result;
}

TCPConnection::TCPConnection(boost::asio::io_service &io_service):_TCPSocket(io_service)
{
}

TCPConnection::~TCPConnection()
{
}
