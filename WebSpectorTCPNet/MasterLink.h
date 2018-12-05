#pragma once
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <map>
#include "TCPConnection.h"
#include "TCPMessage.h"
#include "Error.h"
#include "NetLink.h"


/**
	MasterLink class is an implementation of NetLink Class if the class
	is responsible for forming Master Links

	Master Links = Link made inside Master PC to FrontEnd PC
*/
class MasterLink:public NetLink
{
private:	

	unsigned short _port_num; /** it is the port_number of the Remote FrontEnd PC on which it is expected to accept Master PCs Connections*/
	boost::asio::ip::tcp::resolver _resolver; /** Used for resolving the query to a list of Endpoints */
	
	/** 
		It is the Maximum value of a Port Number 
		that A FrontEnd could be accepting a connection.

		MAX_PORT_NUMBER = FIRST_PORT_NUMBER +numOfMasters
	*/
	int _max_port_number; 

	/**
		It is the first port number on which Master tries to connect 
		with FrontEnd
	*/
	int _first_port_number;/** */
	
	/**
		Handles the flow if the Connection is made succesfully
	*/
	void Handle_IfConnected(TCPConnection::Connection new_connection, const boost::system::error_code& error);
	
	/**
		This function is Called in order to start looking for Connections 
		to FrontEnd
	*/
	void StartLookingForFrontEndConnection();

	/**
		Used to check whether IP Address is part of the System or not

		TODO::Still to be implemented
	*/
	bool CheckIPValidity(std::string RemoteIP);
	
public:
	
	/**
		Used to connect to the Front End FROM Master PC as explained in 
		NetLink.h
	*/
	void ConnectToFrontEnd(std::string IPAddress_Remote,unsigned short portNum);
	
	MasterLink(boost::asio::io_service &MasterPC_io_service,int numOfMasters);
	~MasterLink();
};

