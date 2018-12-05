#include "FrontEndLink.h"
#include <boost/bind.hpp>
#include <iostream>
#include <LogWriter.h>

void FrontEndLink::Handle_IfConnected(TCPConnection::Connection new_connection, const boost::system::error_code & error)
{
	LogWriter::Write("Searvhing For A MASTER ...");
	if (!error)
	{
		_Remote_IP = new_connection->GetSocket().remote_endpoint().address().to_string();
		unsigned short remote_port = new_connection->GetSocket().remote_endpoint().port();
		_connectionMade = true;
		_Tag_System.SetSocket(&new_connection->GetSocket(), _isMaster);
		std::string msg = "Connected To Master(" + _Remote_IP + "::" + std::to_string(remote_port) + ") on Local Port-" + std::to_string(_local_PortNumber) + "- CONNECTION SUCCESS";
		LogWriter::Write(msg);
		_Connection = new_connection;
		_NumberOfConnections++;
		_TCP_Acceptor.close();
	}
}

void FrontEndLink::ConnectToMaster()
{
	_connectionMade = false;
	TCPConnection::Connection tcp_connection = TCPConnection::CreateConnection(_TCP_Acceptor.get_io_service(), false);
	_TCP_Acceptor.async_accept(tcp_connection->GetSocket(), boost::bind(&FrontEndLink::Handle_IfConnected, this, tcp_connection, boost::asio::placeholders::error));
}

FrontEndLink::FrontEndLink(boost::asio::io_service& io_service, unsigned short portNum):_TCP_Acceptor(io_service,boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),portNum))
{
	_local_PortNumber = portNum;
	_NumberOfConnections = 0;
	_isMaster = false;
}

FrontEndLink::~FrontEndLink()
{
}
