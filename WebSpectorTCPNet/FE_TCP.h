#pragma once
#include "TCPMessage.h"
#include "TagSystem.h"
#include "NetLink.h"
#include "TCPSerializer.cpp"

/**
	FE_TCP
	------

	A single instance of FE_TCP Class can be used by FrontEnd, either to Send Objects accross 
	the network  or to send Accross Tags Over The Network WHICH means that single instance can 
	only be used in place of 1 of the Systems given below :-

		-> TAG SYSTEM -- Used to send tags accross the network
		-> OBJECT SYSTEM -- Used to send MNO s accross the network
	
	Port Numbers for both the systems are different and can be changed from the Defaut Constructor of this class
		Tag System in Existing Front End uses 1240
		MNO System in Existing Front End used 1060

	Uses a HeartBeat Timer to in case of Tag System to keep checking the Time Out Time -- Implemented with the help of boost
*/
class FE_TCP
{
private:

	/**
		PortNumber of the CONNECTED remote system if the generated TCP Network is being used for Tag System
	*/
	unsigned short _portNum_TagSystem;

	/**
		PortNumber of the CONNECTED remote system if the generated TCP Network is being used for MNO/Object System
	*/
	unsigned short _portNum_ObjSystem;

	/**
		Used to broad cast a TCPMessage LIST to all the Connected Masters
	*/
	bool BroadcastObj(TCPMessage::TCPMessageLIST messageList);

	/**
		Used to Send A TCPMessage List to only a single Master
	*/
	bool Send(int masterID, TCPMessage::TCPMessageLIST messageList);

	/**
		Used to start the Tag System , Uses _portTagSystem
	*/
	bool StartTagSystem();

	/**
		Used to start Object Sending System - Used for Sending MNO s accross the Network
	*/
	bool StartObjectSystem();

	/**
		Stores number of Master PCs connected to a System  DEFAULT = 1
	*/
	int _masters_count;

	/**
		A Collection Of NetLinks used to connect to each and ever Master in the Network, 
	*/
	NetLink::MultipleLinks _link_pool;

	/**
		The primary IO_Service which runs everything Asynchronously 

		## Another instance of IO Service inside this IO Service might 
		cause a Mutual Exclusion OR Mutex Exception
	*/
	boost::asio::io_service _io_service;

public:

	/**
		Used to set Nummber Of Master that FrontEnd wants to talk to
		It takes in 1 parameter :-
			-@param int NumOfMasters -- Number of masters expected to be connected with FrontEnd
	*/
	void SetMastersCount(int numOfMasters);

	/**
		Used to start the System
		It takes in 1 Parameter :-
			- @param bool - isTagSystem should be true if the user wants to start a Tag System
								DEFAULT = false;

	*/
	bool Start(bool isTagSystem = false);

	/**
		Used to Broad cast a particular Tag accross all Masters
		It takes in 4 parameters : -
			- @param commandName - std::string - Command Name of the TAG
			- @param message - std::string - Message to be delivered with the Tag
			- @param timeOutTime - int - Time Out Time in Milli Seconds - Default = 300
			- @param isBlocking - bool - Should be true if a Message is A BLOCKING message - Default = false
	*/
	bool BroadcastTag(std::string commandName, std::string message, int timeOutTime = 300, bool isBlocking = false);
	
	/**
		Used to send Tag to a Particular Master.
		Takes in 5 Parameters :-
			- @param masterID - Master ID as saved in the Mapping of NetLink::MultipleLinks _link_pool of this object
			- @param commandName - std::string - Command Name of the TAG
			- @param message - std::string - Message to be delivered with the Tag
			- @param timeOutTime - int - Time Out Time in Milli Seconds - Default = 300
			- @param isBlocking - bool - Should be true if a Message is A BLOCKING message - Default = false
	*/
	bool SendTag(int masterID, std::string commandName, std::string message, int timeOutTime = 300, bool isBlocking = false);
	
	/**
		Used to receive Tags from all the Connected Masters
		
		## This function only returns if FrontEnd has received at least 1 tag from Each Master
	*/
	TagSystem::TagList GetAllReceivedTags();

	/**
		Used to Receive Tags from a Particular Master.
		Takes in 1 Parameter :-
			- @param masterID - Master ID as saved in the Mapping of NetLink::MultipleLinks _link_pool of this object

		## This function only returns if FrontEnd has received atleast 1 Tag from Master
	*/
	TagSystem::TagList GetReceivedTagsFrom(int masterID);

	/**
		Used to Broad Cast an Object to all connected Masters
		It takes in 1 Parameter :- 
			- @param TemplateObject obj - Any object can be passed into this parameter given that it has 
			  a serialize function implemented in its functional members
	*/
	template<typename TemplateObject> bool Broadcast(TemplateObject obj)
	{
		TCPMessage::TCPMessageLIST messageVector;
		TCPSerializer::Serialize(obj, &messageVector);
		bool flag = BroadcastObj(messageVector);
		return flag;
	};

	/**
		Used to Broad Cast a vector of Objects to all connected Masters
		It takes in 1 Parameter :- 
			- @param std::vector<TemplateObject> obj - Vector of any Object can be passed into this parameter given that object has 
			  a serialize function implemented in its functional members
	*/
	template<typename TemplateObject> bool Broadcast(std::vector<TemplateObject> objVector)
	{
		TCPMessage::TCPMessageLIST messageVector;
		TCPSerializer::Serialize(objVector, &messageVector);
		bool flag = BroadcastObj(messageVector);
		return flag;
	};

	/**
		Used to Send an Object to a particular connected Masters
		It takes in 2 Parameters :- 
			- @param masterID - Master ID as saved in the Mapping of NetLink::MultipleLinks _link_pool of this object
			- @param TemplateObject obj - Any object can be passed into this parameter given that it has 
			  a serialize function implemented in its functional members
	*/
	template<typename TemplateObject> bool Send(int masterID,TemplateObject obj)
	{
		TCPMessage::TCPMessageLIST messageVector;
		TCPSerializer::Serialize(obj, &messageVector);
		bool flag = Send(masterID,messageVector);
		return flag;
	};
	
	/**
		Used to Send a Vector of any Object to a particular connected Masters
		It takes in 2 Parameters :-
			- @param masterID - Master ID as saved in the Mapping of NetLink::MultipleLinks _link_pool of this object
			- @param std::vector<TemplateObject> obj - Vector of any Object can be passed into this parameter given that object has 
			  a serialize function implemented in its functional members
	*/
	template<typename TemplateObject> bool Send(int masterID, std::vector<TemplateObject> objVector)
	{
		TCPMessage::TCPMessageLIST messageVector;
		TCPSerializer::Serialize(objVector, &messageVector);
		bool flag = Send(masterID,messageVector);
		return flag;
	};

	FE_TCP();
	~FE_TCP();
};

