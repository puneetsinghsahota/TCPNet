#include "HeartBeatTimer.h"
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <LogWriter.h>
#include <thread>

bool HeartBeatTimer::Tag_TimeOutCheck(Tag & tag)
{
	bool found = false;
	if (!tag.IsTimedOut() && !tag.Ack_Received())
	{
		auto startTime = tag.GetStartTime();
		auto expiryTime = tag.GetExpiryTime();
		auto currentTime = boost::asio::deadline_timer::traits_type::now();


		std::string tagString = tag.ToString();

		std::string msg = "Checking {'" + tagString + "', Expiry Time = " + boost::posix_time::to_simple_string(expiryTime) + ", Timer Start Time = " + boost::posix_time::to_simple_string(startTime) + " }, Time Now = " + boost::posix_time::to_simple_string(currentTime);
		LogWriter::Write(msg);
		if (expiryTime < currentTime)
		{
			tag.TimeOutHandler();
			found = true;
		}
	}
	return found;
}

void HeartBeatTimer::CheckTagList()
{	
	if (_sentTags->size() > 0)
	{
		std::find_if(_sentTags->begin(), _sentTags->end(), boost::bind(&HeartBeatTimer::Tag_TimeOutCheck, this, _1));
	}
	boost::this_thread::sleep(boost::posix_time::milliseconds(_time_interval));
	CheckTagList();
}

HeartBeatTimer::HeartBeatTimer(int timeInMilliSeconds, boost::container::vector<Tag>* sentTagsReferenced)
					: _time_interval(timeInMilliSeconds)
{

	_sentTags = sentTagsReferenced;
	std::thread HeartBeatThread(boost::bind(&HeartBeatTimer::CheckTagList,this));
	LogWriter::Write("HeartBeat Timer Triggered {...}");
	HeartBeatThread.detach();
}

HeartBeatTimer::~HeartBeatTimer()
{
}
