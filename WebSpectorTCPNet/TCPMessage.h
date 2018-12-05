#pragma once
#include <boost/asio/streambuf.hpp>
#include <boost/shared_ptr.hpp>
#include <string>

/**
	This Message is used as the Generic Message Holder which is sent Accross on the network 
	TCP Message contains {
		1. Message Type
		2. Message Size
		3. Actual Message Stream
	}
	The message is sent over the network in above given Sequence

*/
class TCPMessage
{
private :

	/**
		Size of the Object To Be Sent Accross the Network
	*/
	std::string _MSG_Size;

	/**
		Type of the Object Sent Accross the Network
		Eg. "MNO"
	*/
	std::string _MSG_Type;
	
	/**
		Sets up the message Size According to the Message Buffer and returns the same in string format
	*/
	static std::string Get_MSG_Size_20Bytes(size_t write_len);

	/**
		Converts a string to buffer which can be sent accross the network
	*/
	static boost::asio::const_buffers_1 Str_To_Buffer(std::string);

public :
	/**
		Contains a List of TCP Message to be sent accross the network
	*/
	typedef std::vector<TCPMessage*> TCPMessageLIST;

	/**
		Contains Actual Serialized Message
	*/
	boost::asio::streambuf _MSG_Stream;

	/**
	Sets up the message type As set by the user
	*/
	void SetMSG_Type(std::string MSG_Type);

	/**
		Gets type of The MESSAGE 
		For Eg - MNO
	*/
	std::string GetMSG_Type();

	/**
		Gets Size of the MSG in std::string
	*/
	std::string GetMSG_Size();

	/**
		Returns Number Of Total Bytes Expected by the Message
	*/
	size_t Get_Expected_MSG_Size();
	
	void SetUp_MSG_Stream(boost::asio::streambuf::const_buffers_type MSG_Buffer, size_t buf_size, std::string MSG_Type);

	/**
		Sets the TCP Message According to any Stream Buffer Containing boost::archive::binary_oarchive which needs to be sent
	*/
	TCPMessage(boost::asio::streambuf::const_buffers_type MSG_Buffer, size_t buf_size, std::string MSG_Type);
	
	TCPMessage();
	~TCPMessage();
};

