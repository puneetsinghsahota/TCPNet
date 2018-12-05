#pragma once
#include <boost/asio/io_service.hpp>
#include <boost/container/vector.hpp>
#include "Tag.h"

/**
	Heart beat timer - A timer which triggers its Check Tags function on every Heart Beat

	----- The heart beat function is responsible to filter out the tags which have been timed out

*/
class HeartBeatTimer 
{
private:
	
	/**
		A boost vector to keep a reference of The Tags Received
	*/
	boost::container::vector<Tag>* _sentTags;

	/**
		Heart Beat Time Interval
	*/
	const int _time_interval;

	/**
		A predicate function to check if a Tag has been timed Out
	*/
	bool Tag_TimeOutCheck(Tag &tag);

	/**
		The MAIN HeartBeat Function which checks if a tag has been timedOut without reception of Acknowledgement
	*/
	void CheckTagList();
	
public:

	/**
		The constructor takes in the IO Service being used by the Sockets 
		It takes in 3 parameters :-
			- @param - boost::asio::io_Service& - the current IO Service of Tag System
			- @param - int - Time in Milli Seconds to perform Time Out CHECK at every heart beat
			- @param boost::container::vector<Tag>* - A reference to the _Sent Tags Queue in Tag System
	*/
	HeartBeatTimer(int timeinMilliSeconds, boost::container::vector<Tag>* sentTagsReferenced);
	~HeartBeatTimer();
};

