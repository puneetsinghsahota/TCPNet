#pragma once
#include "NetLink.h"
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_service.hpp>
#include <map>


/**
	FrontEndLink class is an implementation of NetLink Class if the class
	is responsible for forming FrontEnd Link

	FrontEnd Link = Link made from FrontEnd PC to Master PC
*/
class FrontEndLink: public NetLink
{
private :

	/**
		A TCP Acceptor to Asynchronously accept connections
	*/
	boost::asio::ip::tcp::acceptor _TCP_Acceptor;

	/**
		Stores the local port number of the Machine on which it exists - Has to be the FrontEnd
	*/
	unsigned short int _local_PortNumber;

	/**
		Handles Connection if there is an incoming Connection
	*/
	void Handle_IfConnected(TCPConnection::Connection new_connection, const boost::system::error_code& error);

public:

	/**
		Used to connect to the Master PC s from Front End - Connection made in this
		function is a TCPConnection and FrontEnd starts to accept the Connections on local_port
		passed when creating the MultipleLink To MASTER, and the open ports for accepting
		connections are from Local_Port to Local_Port + numOfMasters
	*/
	void ConnectToMaster();

	FrontEndLink(boost::asio::io_service &io_service, unsigned short portNum);
	~FrontEndLink();
};

