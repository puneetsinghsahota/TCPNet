#pragma once
#include <string>
#include <vector>
#include <chrono>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>

/**
	Tag Class is used to keep information related to the tag
	and take care of the TimeOut Time of a Particular Sent Tag
*/
class Tag
{
public: 

	/**
		Returns identity of a Tag 
		Identity of a tag is actually the 2 Character long Acknowledgement ID Attatched to it
	*/
	int Identity();

	/**
		Used to set the TimeOut Time in milli seconds
	*/
	void SetTimeOut(int timeout);

	/**
		Used to set whether this tag is a Blocking Tag or not
	*/
	void SetIsBlocking(bool enable);
	
	/**
		Used to set the Ack ID of the Tag

		'The 2 character long Ack ID'

		If the integer passed is more than 2 characters long than it will take 
		last two digits as Ack ID
	*/
	void SetAckID(int ackid);

	/**
		Sets A Tag According to String
			- @param tagString - String to be parsed into a tag
	*/
	void SetString(std::string tagString);

	/**
		Used to set the Message Attached with the Command Name
	*/
	void SetMessage(std::string message);

	/**
		Used to set the command name for the Tag
	*/
	void SetCommandName(std::string cmdName);

	/**
		Used to start Timer of Tag to check for timeOut Time

	*/
	void StartTimer();

	/**
		Used to get the timeOut Time in milliseconds
	*/
	int GetTimeOutTime();
	
	/**
		Used to check if the system is Blocking or not
	*/
	bool IsBlocking();

	/**
		Used to get the Acknowledgement String formed by Ack_ID 

		Joins 'AK:!' TO Ack_ID
	*/
	std::string AcknowledgeString();
	
	/**
		Used to check existing Message attached with the Command Name
	*/
	std::string Message();

	/**
		Returns Command Name
	*/
	std::string CommandName();

	/**
		Used to Set Received Acknowledgement to true if the System has
		received Acknowledgement for this tag
	*/
	void SetAck_RCVD(bool success = true);

	/**
		Used to check whether an Acknowledgement has been received for this tag or not
	*/
	bool Ack_Received();

	/**
		Used to check Whether a Tag has been timed out
	*/
	bool IsTimedOut();
	
	/**
		Triggered by HeartBeat Timer if a tag has timed out
	*/
	void TimeOutHandler();

	/**
		Return the tag in a format in which it sent Accross the network
	*/
	std::string ToString();

	/**
		Gets the Sending Time of the Tag
			- The tag must be sent and the StartTimer function must have been called at the 
			time of Sending to get the correct StartTime
	*/
	boost::posix_time::ptime GetStartTime();

	/**
		Gets the Expiry Time/TimeOut time relative to the StartTime of the tag
			- The tag must be sent and the StartTimer function must have been called at the 
			time of Sending to get the correct Expiry Time/Time Out Time for the tag
	*/
	boost::posix_time::ptime GetExpiryTime();


	Tag();

	Tag(std::string tagString);

	Tag(std::string commandName, std::string message, int ackID, bool isMaster = true);

	~Tag();

private:

	bool _ack_rcvd; /** Is true if an Acknowledgement has been received*/
	int _size_of_ackID; /** SIZE_OF_ACK_ID is always 2*/
	int _ack_ID; /** Saves the identity of the tag*/
	std::string _command_name; /** Saves the Command Name of this tag*/
	std::string _message; /**Used to store message of this particular Tag*/
	std::string _acknowledgementString; /** Stores the Acknowledgement ID in the Form of AK;! + 'Ack_ID' */
	std::string _ack_delimiter; /** Stores delimiter to be used while sending Acknowledgements*/
	std::string _msg_delimiter; /** Stores Delimiter used to break String when A Tag is Received*/
	
	boost::posix_time::ptime _start_time; /** Used to store START time of the Time Out Timer*/
	boost::posix_time::ptime _expiry_time; /** Used to store Expiry Time of the Time Out Timer*/

	int _timeOutTime; /** Stores TimeOut Time of the tag in Milli Seconds*/
	bool _blocking; /** Stores true if the message is Blocking*/
	bool _timed_OUT; /** Is true if a Tag is Already TimedOut*/
	
	/**
		The default wait time before Tag Checks whether an Acknowledgement has been received or not
	*/
	int _default_wait_time;


	/**
		Used to break a Tag Received in the Format expected on the Network 

		Breaks the tag into three segments - 
			- Command Name
			- Message
			- Acknowledgement ID
	*/
	void BreakTagString(std::string tagString);

	/**
		Used to set the Delimiter of a Given System 

		Delimiters depend on whether a System is A Master or a FrontEnd
	*/
	void SetMessageDelimiter(std::string delimiter);	

};

