#pragma once
#include <boost/shared_ptr.hpp>
#include <boost/asio/io_service.hpp>
#include "TCPMessage.h"
#include "TCPConnection.h"
#include "Error.h"
#include <string>
#include <map>
#include "TagSystem.h"

/**
	NetLink Class is a class used to create links from one PC to another.
	It creates a Shared Reference to this link which can then be moved 
	along to connect, Send Tags, Send MNOs etc.

	Is used for both Master And Front End

	Links made from Master PCs are called MasterLink ---- Master To FrontEnd
	Links made from FrontEnd PCs are called FrontEndLink ---- FrontEnd To Master
*/
class NetLink
{	
public :
	/**
		A shared reference of this class
	*/
	typedef boost::shared_ptr<NetLink> Link;

	/**
		Is true if the connection has been made succesfully - Is used before the System Starts because Receiver waits till 
		this boolean is turned to true before starting to Listen on incoming Port
	*/
	bool _connectionMade;

	/**
		Saves Multiple Links, Usually this type of Link is used by FrontEnd to mantain multiple links to 
		Multiple Masters
	*/
	typedef std::map<int, Link> MultipleLinks;

	/**
		Saves number of Connections inside a single NetLink
	*/
	int _NumberOfConnections;

	/**
		Saves IP Address of the remote system which is connected via the socket of this class
	*/
	std::string _Remote_IP;

	/**
		This function creates a MultipleLinks to every Master from FrontEnd.

		The connection made inside Multiple Links are TCPConnection and it is actually a Map of references to MasterLink

		This function takes in 2 parameters :-
		- @param - IO_Service - boost::asio::io_service
		- @param - number_of_masters - int (Default = 1)
	*/
	static MultipleLinks CreateMultipleLinks_ToMASTERS(boost::asio::io_service &fe_io_service,unsigned short local_port,int numOfMasters = 1);
	
	/**
		Creates Master Links - which means Links For Masters...

		Call this function in a Master PC to connect it to FrontEnd

		This function takes in 2 parameters :-
			- @param - IO_Service - boost::asio::io_service
			- @param - number_of_masters - int (Default = 1)
	*/
	static Link CreateMasterLink(boost::asio::io_service &MasterPC_io_service, int number_of_masters);
	
	/**
		Creates Front End Links for the FrontEnd 

		Call this function inside a FrontEnd PC to make links to the Master PC.

		This function takes in 2 parameters :-
			- @param - IO_Service - boost::asio::io_service
			- @param - local_port_number - unsigned short
	*/
	static Link CreateFrontEndLink(boost::asio::io_service &FE_PC_io_service, unsigned short local_port);
	
	/**
		Used to send a TCPMessage accross the Network

		Takes in 2 parameters :-
			-@param TCPMessage * - A reference to a TCP Message
			-@param Error::TCPError - Records the routines Logs for things which did not go so well
	*/
	virtual void Send(TCPMessage *message, Error::TCPError e);

	/**
		Used to send a List of TCPMessages accross the Network 

		A list of TCPMessages can also be represented by TCPMessage::TCPMessageLIST

		It takes in 2 parameters : -
			-@param - messageVector -- TCPMessage::TCPMessageLIST
			-@param - Error::TCPError e -- Records the routines Logs for things which did not go so well
	*/
	virtual bool Send(TCPMessage::TCPMessageLIST messageVector, Error::TCPError e);

	/**
		Used by MasterLinks to connect the Master PC to FrontEnd PC

		It takes in 2 parameters : -
			- @param - IPAddress_Remote - The IP Address of remote Front End PC
			- @param - portNum - Port number on which Front End is expected to be Accepting Connections
	*/
	virtual void ConnectToFrontEnd(std::string IPAddress_Remote, unsigned short portNum);
	
	/**
		This function is used to make the System to wait for any incoming Data
		on the socket where it wants to receive Tags

		It returns a Vector of Tags -> std::vector<Tag>
	*/
	virtual void StartReceivingTags();

	
	/**
		Used to receive TCP Message List from the Socket 

		TCPMessage List is actually a vector of TCPMessages.

		It takes in a reference of TCPMessageLIST which is -> std::vector<TCPMessage *> 
			- @param - TCPMessage::TCPMessageLIST* 

	*/
	virtual void StartReceivingObjects();


	
	/**
		Used to Send Tags Accross the Network
		It takes in 4 parameters : -
			- @param - std::string command - command name for the tag
			- @param - std::string message - message to be attached with the command
			- @param - bool isBlocking - Set to true if the Tag is a Blocking tag - Default = false
			- @param - timeOutTime - Used to set the TimeOut Time for a Blocking Tag in milli-seconds - Default = 20 ms
	*/
	virtual bool SendTag(std::string command, std::string message, bool isBlocking = false, int timeOutTime = 20);

	/**
		Gets All Received TAGS by the Tag System

		## This function only returns if there is atleast One Tag Received by the Systm
	*/
	TagSystem::TagList GetReceivedTags();

	/**
		A function used to wait for All the Connections to be made before starting to Listen on the Socket
	*/
	void WaitForConnections();

	/**
		A default constructor for the NetLink object
	*/
	NetLink();
	~NetLink();
	
protected:
	
	/** 
		Is true if the NetLink is being used for a Master
	*/
	bool _isMaster; 

	/**
		Used to handle the Tag System for the NetLink Object
		Takes care of the Blocking Tags and sending Acknowledgements if it has received
		some tags
	*/
	TagSystem _Tag_System;

	/**
		A shared reference to the Connection made by NetLink to any other PC
		TCP::connection = boost::shared_ptr<TCPConnection>
	*/
	TCPConnection::Connection _Connection;
	
	/**
		Used to send a Generic TCP Message accross the network

		Is an internal method of NetLink
	*/
	virtual size_t TCPSend( TCPMessage *message, Error::TCPError e);
	
	/**
		A threaded Receive Function
	*/
	virtual void ReceiveTags();

	/**
		A generic Function which parses through each TCPMessage inside
		Vector TCPMessage::TCPMessageLIST and sends them accross the network
	*/
	virtual int TCPSendVector(TCPMessage::TCPMessageLIST messageVector, Error::TCPError e);
	
	/**
		Used to connect to the Master PC s from Front End - Connection made in this 
		function is a TCPConnection and FrontEnd starts to accept the Connections on local_port 
		passed when creating the MultipleLink To MASTER, and the open ports for accepting 
		connections are from Local_Port to Local_Port + numOfMasters
	*/
	virtual void ConnectToMaster();

	/**
		Is used as a Trigger to the Receiver Of Object System -- Is always active in a Thread Spawned by this Class
	*/
	virtual void StartReceiver();
};

