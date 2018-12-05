#pragma once

#include <opencv2/opencv.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <iostream>
#include <sstream>
#include <vector>
#include <LogWriter.h>
#include "TCPMessage.h"

/**
	USED TO SERIALIZE THE MAT OBJECT
	--------------------------------
	Doesn't use Multiple Iterations and hence is FAST
*/

BOOST_SERIALIZATION_SPLIT_FREE(::cv::Mat)

namespace boost {
	namespace serialization {
		template<class Archive>
		inline void save(Archive & ar, const ::cv::Mat& m, const unsigned int version)
		{
			size_t elem_size = m.elemSize();
			size_t elem_type = m.type();
			ar & m.cols;
			ar & m.rows;
			ar & elem_size;
			ar & elem_type;
			const size_t data_size = m.cols * m.rows * elem_size;
			ar & boost::serialization::make_array(m.ptr(), data_size);
		}
		template<class Archive>
		inline void load(Archive & ar, ::cv::Mat& m, const unsigned int version)
		{
			int cols, rows;
			size_t elem_size, elem_type;
			ar & cols;
			ar & rows;
			ar & elem_size;
			ar & elem_type;
			m.create(rows, cols, elem_type);
			size_t data_size = m.cols * m.rows * elem_size;
			ar & boost::serialization::make_array(m.ptr(), data_size);
		}
	}
}

/**
	TCPSerializer
	-------------

	Used to Serialize any Object into A TCP Message which can be easily sent Accross the 
	Network by WebSpectorTCPNet.lib
*/
class TCPSerializer
{
public:
	/**
		Used to Serialize an object of ANY class T
		It takes in 2 Parameters:-
			-@param1 - An object of Template Class T (can be any class ---- Containing a Serialize Function in its implementation )
			- @param2 - A reference to TCPMessage::TCPMessageLIST -> std::vector<TCPMessage *>
		This function fills up the message passed as the second argument to this Template Function which can then be sent through a TCP Network
	*/
	template<typename TemplateObject> static void Serialize(TemplateObject obj, TCPMessage::TCPMessageLIST* MSG_LIST)
	{
		TCPMessage *message;
		boost::asio::streambuf stream_buffer;
		std::ostream outputStream(&stream_buffer);
		boost::archive::binary_oarchive archive(outputStream);
		archive << obj;
		message->SetUp_MSG_Stream(stream_buffer.data(), stream_buffer.size(), "MNO");
		MSG_LIST->push_back(message);
	};

	/**
		Used to Serialize a Vector of ANY class T
		It takes in 2 Parameters:-
			- @param1 - A VECTOR of any class (can be any class ---- Containing a Serialize Function in its implementation )
			- @param2 - A reference to TCPMessage::TCPMessageLIST -> std::vector<TCPMessage *>
	*/
	template<typename TemplateObject> static void Serialize(std::vector<TemplateObject> objVector, TCPMessage::TCPMessageLIST* MSG_LIST)
	{
		auto start = boost::posix_time::second_clock::local_time();
		for (auto obj : objVector)
		{
			TCPMessage* message = new TCPMessage();
			boost::asio::streambuf stream_buffer;
			std::ostream outputStream(&stream_buffer);
			boost::archive::binary_oarchive archive(outputStream);
			archive << obj; // Serialization of A Template Class
			message->SetUp_MSG_Stream(stream_buffer.data(), stream_buffer.size(), "MNO");
			MSG_LIST->push_back(message);
		}
		auto end = boost::posix_time::second_clock::local_time();
		boost::posix_time::time_duration serialization_time = end - start;
		std::string msg = "Serialization Time = '" + std::to_string(serialization_time.total_milliseconds()) + " ms', Size = " + std::to_string(MSG_LIST->size());
		LogWriter::Write(msg);
	};

	/**
		Used to De-Serialize a Stream Of Buffer into an Object as Passed by the Network
			- And Deallocates the memory allocated to the Stream Buffer
	*/
	template<typename TemplateObject> static void DeSerialize(std::vector<TemplateObject> *OBJ_VECTOR, TCPMessage::TCPMessageLIST list)
	{
		auto start = boost::posix_time::second_clock::local_time();
		for (auto &message : list)
		{
			boost::archive::binary_iarchive archive(message->_MSG_Stream);
			TemplateObject obj;
			archive >> BOOST_SERIALIZATION_NVP(obj);
			message->_MSG_Stream.consume(message->_MSG_Stream.size());
			OBJ_VECTOR->push_back(obj);
		}
		auto end = boost::posix_time::second_clock::local_time();
		boost::posix_time::time_duration deSerialization_time = end - start;
		std::string msg = "De - Serialization Time = '" + std::to_string(deSerialization_time.total_milliseconds()) + " ms', Size = " + std::to_string(list.size());
		LogWriter::Write(msg);
	};
};