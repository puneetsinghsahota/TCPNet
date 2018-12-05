#include "MasterLink.h"
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <boost/thread.hpp>
#include <LogWriter.h>

void MasterLink::Handle_IfConnected(TCPConnection::Connection new_connection, const boost::system::error_code & error)
{
	LogWriter::Write("Connection Handler Called");
	if (!error)
	{
		_connectionMade = true;
		_Connection = new_connection;
		_Remote_IP = new_connection->GetSocket().remote_endpoint().address().to_string();
		_Tag_System.SetSocket(&new_connection->GetSocket(),_isMaster);
		_NumberOfConnections++;
		std::string msg = " Connected To FrontEnd (" + _Remote_IP + "::" + std::to_string(_port_num) + ") - Connect SUCCESFULL";
		LogWriter::Write(msg);
	}
	if (_NumberOfConnections < 1)
	{
		if (_port_num == _max_port_number)
		{
			_port_num = _first_port_number;
		}
		else
		{
			_port_num++;
		}
		StartLookingForFrontEndConnection();
	}
}

void MasterLink::StartLookingForFrontEndConnection()
{
	LogWriter::Write("Searching for a Front END {...}");
	_connectionMade = false;
	TCPConnection::Connection tcp_connection = TCPConnection::CreateConnection(_resolver.get_io_service(), false);
	boost::asio::ip::tcp::endpoint contact_point(boost::asio::ip::address::from_string(_Remote_IP), _port_num);
	boost::asio::ip::tcp::resolver::iterator endpoint_iterator = _resolver.resolve(contact_point);
	boost::asio::async_connect(tcp_connection->GetSocket(),endpoint_iterator,boost::bind(&MasterLink::Handle_IfConnected,this, tcp_connection,boost::asio::placeholders::error));
}

bool MasterLink::CheckIPValidity(std::string RemoteIP)
{
	return true;
}

void MasterLink::ConnectToFrontEnd(std::string IPAddress_Remote,unsigned short portNum)
{
	_max_port_number += portNum;
	_first_port_number = portNum;
	_Remote_IP = IPAddress_Remote;
	_port_num = portNum;
	StartLookingForFrontEndConnection();
}

MasterLink::MasterLink(boost::asio::io_service &MasterPC_io_service,int numOfMasters):_resolver(MasterPC_io_service)
{
	LogWriter::Write("Creating a Master Link from Master To Front End ...");
	_max_port_number = numOfMasters;
	_isMaster = true;
	_NumberOfConnections = 0;
}

MasterLink::~MasterLink()
{
}
