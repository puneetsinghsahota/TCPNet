#pragma once
#include"Tag.h"
#include <boost/asio/ip/tcp.hpp>
#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/container/vector.hpp>
#include "HeartBeatTimer.h"

/**
	TagSystem Class is used to handle all the operations regarding
	Sending and Receiving Of Tags in a Network
*/
class TagSystem {

public:
	/**
		A list type used by Tag System to keep the outgoing tags in a queue so as to
		align them for going through TCP SEND function
	*/
	typedef boost::container::vector<Tag> TagList;
private:
	
	/**
		A heartbeat timer used to check timeout for each and every tag after a
		certain period of time
	*/
	HeartBeatTimer _TO_heartbeat;

	/*
		Unary Predicate Function to check whether Tag has received
		Acknowledgement already or timed out already
	*/
	bool TAG_Q_CheckTag(Tag &tag);

	/*
		Unary Predicate function to compare Acknowledgement ID with each tag
		and set Ack Received = true if found same identity
	*/
	bool TAG_Q_CheckAck(Tag &tag,int ack_id);

	bool _isMaster; /*Is true if NetLink is being used for Master or else is False*/
	
	int _lastRCVD_Identity; /** Stores the last Received Identity/ Ack_ID on the Network*/
	
	int _max_queue_size; /** Stores maximum Size of Tag Queue*/

	/**
		Stores reference of the Socket being used by NetLink
	*/
	boost::asio::ip::tcp::socket* _socket; 
	
	/**
		Stores Unparsed Buffer -- A buffer which is received by the Tag System but 
		is not completely identified or lacks certain amount of information which still needs 
		to be received
	*/
	std::string _unparsed_buf;
	
	/**
		The tag queue used for aligning tags in form of a queue to be sent over the network 
		and is also responsible to give them identity according to their positions in the Queue
	*/
	TagList _SentTags_Queue;
	
	/**
		Used to delete a Tag at certain position inside the Tag Queue
	*/
	void TRY_Clear_SentTags_Queue();
	
	/**
		Used to send Acknowledgements for received Tag
	*/
	void TCPSendAck(Tag& tag);
	
	
	/**
		Just a Dummy Function to Implement Handler Event for every tag written to the Network

		TODO:: Still to be implemented Properly
	*/
	static void HandleWrite();// , const Tag& tag);

	/**
		Just a Dummy Function to Implement Handler Event for Every Tag Acknowledgement written to the NetWork

		TODO:: Still to be implemented Properly
	*/
	static void HandleAckWrite();
	
	/**
		Is implemented if the received Acknowledgement is an Acknowledgement String
		And checks whose Acknowledgement it is 

		If the acknowledgement received is of a tag which has already timedOut - Tag System 
		ignores the received Ack
	*/
	void CheckAcknowledgement(std::string tagString);
	 
	/**
		Identifies the Tag Type and Splits a received String Buffer to tags 
	 */
	TagList ParseTags(std::string tagMessage);

	/**
		Used to write a Tag to A Network Asynchronously
	*/
	void WriteTagOnSocket(Tag& tag);

	/**
		Used to Send Tag Accross the Network Asynchronously
	*/
	void SendIfSocketOpen(Tag& tag);

	/**
		Used to Receive Tags From Socket
	*/
	TagList ReadTagsFromSocket();

	/**
		Parses message received on the socket to find out tags inside the stream of characters
		Takes in 1 parameter :
			- @param tagmessage - std::string - A string of characters received on the socket
			- @param parsed_Tag - Tag * A reference to the object which needs to be initialized if the function finds a tag in tag Message
	*/
	void ParseFirstTag(std::string *tagMessage,Tag& parsed_Tag);

	/**
		This function is used to check whether a Tag String parsed from Tag Message Received on the socket
		is an Actual Tag or Just an Acknowledgement
		It takes 4 parameters :-
			- @param - std::string tagString - Tag String parsed by Tag Message Parser
			- @param - std::string tagMessage - Message Received on the socket
			- @param - Tag* parsed_Tag - Reference to the tag which needs to be initialized if parsed string
						is an actual Tag.
			- @param - int tagSize  - Size of the tag which needs to be read from the tag Message
	*/
	bool ClassifyAndGetTag(std::string tagString,Tag& parsed_Tag);

	/**
		This function is used to remove characters parsed by the Parser Already
		It takes in 2 parameters :-
			- @param - int tagSize - Size of the last parsed Tag
			- @param - std::string *tagMessage - A reference to the tag Message received on Socket 
	*/
	void RemoveParsedCharactersFromBuffer(int tagSize,std::string *tagMessage);

public:

	TagList _receivedTags_list;

	/**
		Used to send Tag Accross the Network 
		It takes in 4 Parameters : -
			- @param - std::string cmd_name - Command name of a particular tag
			- @param - std::string message - Message attached to the particular tag
			- @param - bool blocking - True if particular tag should be a blocking message - Default = false
			- @param - int timeOutTime - Sets the timeOutTime of a Particular Tag in milli-seconds - Default = 20 milli-seconds
	*/		
	void SendTag(std::string cmd_name, std::string message,bool blocking = false,int timeOutTime = 20);

	/**
		Used to receive tags on the network
		Doesn't return until a tag has been received 

		Tag could even be an acknowledgement of a Sent Tag
	*/
	void ReceiveTags();

	/**
		Used to set the Socket of a Paticular Tag System

		Takes in 2 parameters :-
			- @param boost::asio::ip::tcp::socket * - A reference to the Socket in Use by NetLink
			- @param bool - is true if NetLink is being used by Master or false if NetLink is used by FrontEnd
	*/
	void SetSocket(boost::asio::ip::tcp::socket* tag_socket, bool isMaster);
	
	TagSystem();
	~TagSystem();
};

