#pragma once
#include <boost/asio.hpp>
#include <string>
#include "NetLink.h"
#include "TCPSerializer.cpp"



class Master_TCP 
{

private:
	
	/**
		Remote IP Address of the connected system
	*/
	std::string _remote_ip;

	/**
		PortNumber of the CONNECTED remote system if the generated TCP Network is being used for Tag System
	*/
	unsigned short _portNum_TagSystem;

	/**
		PortNumber of the CONNECTED remote system if the generated TCP Network is being used for MNO/Object System
	*/
	unsigned short _portNum_ObjSystem;
	
	/**
		Number Of Masters Connected to the System
	*/
	int _masters_count;

	/**
		The only Active IO Service
	*/
	boost::asio::io_service _io_service;
	
	/**
		A generic Network Link existing in both Master and front End , Uses AN OVERRIDEN MasterLink in case of Master PC
	*/
	NetLink::Link _link;

	/**
		Used to send the message containing Objects Accross the Network
	*/
	bool Send(TCPMessage::TCPMessageLIST messageList);

	/**
		Used to start the Tag System - > Is used to open an asynchronous CONNECT function using boost::asio
		The System waits until all masters are connected
	*/
	bool StartTagSystem();

	/**
		Used  to start the Object/MNO System
	*/
	bool StartObjectSystem();

public:
	
	/**
		Used to set Number of Masters Connected to the System
		Takes in 1 parameter : - 
			- @param - num - int Number of Master s Connected to the System
	*/
	void SetMastersCount(int num);

	/**
		Used to Set Ip Address of the Front End SYSTEM which needs to be connected to master 
		Takes in 1 Parameter : - 
			- @param - std::String ip_address - IP Address of Remote System
	*/
	void SetFrontEnd(std::string ip_address);

	/**
		Used to Start the TCP System, 
		Takes in 1 Parameter :-
			- @param - isTagSystem - bool - -should be true if the User wants to start a Tag System DEFAULT = false
	*/
	bool Start(bool isTagSystem = false);

	/**
		Used to Send A Tag Accross to the Front End
		Takes in 4 parameters : -
			- @param commandName - std::string - Command Name of the TAG
			- @param message - std::string - Message to be delivered with the Tag
			- @param timeOutTime - int - Time Out Time in Milli Seconds - Default = 300
			- @param isBlocking - bool - Should be true if a Message is A BLOCKING message - Default = false
	*/
	bool SendTag(std::string commandName, std::string message, int timeOutTime = 300, bool isBlocking = false);

	/**
		Used to check if there are any Tag Received By Master
		
		## This function only returns if the Master has received at least 1 Tag from the Master
	*/
	TagSystem::TagList GetReceivedTags();

	/**
		Used to Send an Object to FrontEnd
		It takes in 1 Parameter :-
			- @param TemplateObject obj - Any object can be passed into this parameter given that it has
			  a serialize function implemented in its functional members
	*/
	template<typename TemplateObject> bool Send(TemplateObject obj)
	{
		TCPMessage::TCPMessageLIST messageVector;
		TCPSerializer::Serialize(obj, &messageVector);
		bool flag = Send(messageVector);
		return flag;
	};
	
	/**
		Used to Send a vector of Objects to FrontEnd
		It takes in 1 Parameter :-
			- @param std::vector<TemplateObject> obj - Vector of any Object can be passed into this parameter given that object has
			  a serialize function implemented in its functional members
	*/
	template<typename TemplateObject> bool Send(std::vector<TemplateObject> objVector)
	{
		TCPMessage::TCPMessageLIST messageVector;
		TCPSerializer::Serialize(objVector, &messageVector);
		bool flag = Send(messageVector);
		return flag;
	};
	
	Master_TCP();
	~Master_TCP();
};

