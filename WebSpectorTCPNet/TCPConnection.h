#pragma once
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include "TCPMessage.h"
#include "Error.h"
/**
	TCP Connection -- Holds a connection with single socket as a means of either Reading Or Writing 
	Whether this Connection Reads or Writes is Decided by bool _ConnType 
*/
class TCPConnection
{
private:
	
	boost::asio::ip::tcp::socket _TCPSocket; /** Holds the SOCKET of each Connection*/

	/**
		Used to get the 20 Bytes Message Size of a Single Object 
	*/
	int GetBytesToBeReceived_20Bytes();
public:
	
	/**
		A shared resource allocation to Class TCPConnection
	*/
	typedef boost::shared_ptr<TCPConnection> Connection;
	
	/**
		Creates a Shared Resource for the class TCP Connection
			@param - Takes current IO Service being run as the parameter
			@param - Takes true if the Connection Acts as a Reciever and False if the Connection Has to act as a Transmitter
	*/
	static Connection CreateConnection(boost::asio::io_service& io_service,bool connType);

	/**
		Returns Socket being used in the Connection	
	*/
	boost::asio::ip::tcp::socket& GetSocket();
	
	/**
		Sends the TCP Message
			@param1 - TCPMessage
	*/
	size_t Send(TCPMessage *message,Error::TCPError e);
	
	/**
		Sends the total number of MNOs being sent in one go
	*/
	size_t Send_10ByteMSGCount(int count, Error::TCPError e);

	/**
		Receives Number Of Objects and returns an Integer which shows number of Objects Received Succesfully

		Takes in 2 Parameters :
			- @param NumOfObjs - int - Number Of Objects which the System Accepts
			- @param list - TCP::TCPMessageLIST * - a reference to attach received Objects

	*/
	int Receive(int NumOfObjs,TCPMessage::TCPMessageLIST *list); //TO DO :: Implement this Function
	
	/**
		Receive 10 Bytes of Message Count Before The Actual Object Vector
	*/
	int Receive_10ByteMSGCount();

	/**
		Initializes TCP Connection 
		Takes one param:-
			@param1 - The current IO Service
	*/
	TCPConnection(boost::asio::io_service& io_service);

	~TCPConnection();
};

