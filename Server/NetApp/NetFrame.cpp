#include "stdafx.h"
#include "NetFrame.h"
#include <boost/array.hpp>

namespace NETAPP{
	void Connection::Start()
	{
		socket_->async_receive(boost::asio::buffer(msgbuf_, sizeof(msgbuf_)), boost::bind(&Connection::ReadHandler, shared_from_this(), _1, _2));
	}

	void Connection::Close(bool send /* = true */)
	{
		if (status_.exchange(kClosed) != kClosed)
		{
			boost::system::error_code errcode;
			if (socket_->close(errcode))
			{
				std::cerr << "" << std::endl;
			}
			else
			{
				std::cerr << "" << std::endl;
			}
			
			if (send)
			{
				//发空包处理连接断开
				ServerPtr_->AddRecvMssage(std::shared_ptr<ClientMessage>(new ClientMessage(ClientID, nullptr)));
			}
		}
	}

	void Connection::ReadHandler(const boost::system::error_code & error, std::size_t bytes_transferred)
	{
		if (!error)
		{
			msgbuf_[bytes_transferred] = '\0';
			std::shared_ptr<std::string> sendMessage(new std::string(msgbuf_));
			if (sendMessage != nullptr)
			{
				ServerPtr_->AddRecvMssage(std::shared_ptr<ClientMessage>(new ClientMessage(ClientID, sendMessage)));
			}

			socket_->async_receive(boost::asio::buffer(msgbuf_,sizeof(msgbuf_)), boost::bind(&Connection::ReadHandler, shared_from_this(), _1, _2));
		}
		else if (error == boost::asio::error::operation_aborted)
		{
			std::cout << "" << std::endl;
		}
		else
		{
			ConnStatus expected = kConnected;
			if (status_.compare_exchange_strong(expected, kError))
			{
				std::cout << "" << std::endl;
			}
		}
	}

	void Connection::WriteHandler(const boost::system::error_code& error, std::size_t bytes_transferred)
	{
		if (!error)
		{
			std::cout << "send Message sucees" << std::endl;
		}
		else if (error == boost::asio::error::operation_aborted)
		{
			std::cout << "" << std::endl;
		}
		else
		{
			ConnStatus expected = kConnected;
			if (status_.compare_exchange_strong(expected, kError))
			{
				std::cout << "" << std::endl;
			}
		}
	}

	void Connection::WriteMessage(std::shared_ptr<ClientMessage> msg)
	{
		if (msg->buffer == nullptr)
		{
			//服务器主动断开连接
			Close(false);
		}
		else
		{
			async_write(*socket_, boost::asio::buffer(*msg->buffer), boost::bind(&Connection::WriteHandler, shared_from_this(), _1, _2));
		}
	}

	void Connection::setStatus(ConnStatus value)
	{
		status_.exchange(value);
	}

	EchoServer::EchoServer(IOServeicePtr io_service) : stopped_(false), io_service_(io_service), acceptor_(*io_service), maxID(0)
	{
	}

	int EchoServer::Start(const std::string &host, unsigned short port)
	{
		boost::system::error_code errcode;
		boost::asio::ip::address address = boost::asio::ip::address::from_string(host, errcode);
		if (errcode)
		{
			return false;
		}

		if (acceptor_.open(boost::asio::ip::tcp::v4(), errcode))
		{
			return false;
		}

		acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
		boost::asio::ip::tcp::endpoint endpoint(address, port);
		if (acceptor_.bind(endpoint, errcode) || acceptor_.listen(1024, errcode))
		{
			return false;
		}

		SocketPtr socket(new boost::asio::ip::tcp::socket(*io_service_));
		acceptor_.async_accept(*socket, boost::bind(&EchoServer::AcceptHandler, shared_from_this(), socket, _1));
		return true;
	}

	uint32_t EchoServer::GetClientID()
	{
		uint32_t clientID = 0;
		if (userIDList.empty())
		{
			maxID++;
			clientID = maxID;
		}
		else
		{
			clientID = userIDList.front();
			userIDList.pop_front();
		}

		return clientID;
	}

	void EchoServer::AcceptHandler(SocketPtr socket, const boost::system::error_code& error)
	{
		if (error == boost::asio::error::operation_aborted)
		{
			std::cout << "" << std::endl;
			return;
		}
		else if (!error)
		{
			std::cout << "" << std::endl;
			auto clientID = GetClientID();
			ConnPtr new_conn(new Connection(socket, clientID));
			new_conn->SetServerPtr(this);
			new_conn->Start();
			
			conn_set_.insert(std::make_pair(clientID, new_conn));

			TimerPtr socket_time(new boost::asio::deadline_timer(*io_service_));
			socket_time->expires_from_now(boost::posix_time::millisec(1));
			socket_time->async_wait(boost::bind(&EchoServer::CheckSocketStatus, shared_from_this(), clientID, socket_time, _1));
		}
		else
		{
			std::cout << "" << std::endl;
		}

		SocketPtr new_socket(new boost::asio::ip::tcp::socket(*io_service_));
		acceptor_.async_accept(*new_socket, boost::bind(&EchoServer::AcceptHandler, shared_from_this(), new_socket, _1));
	}

	void EchoServer::CheckSocketStatus(uint32_t clientID, TimerPtr socket_timer, const boost::system::error_code& error)
	{
		//遍历要发送的数据
		/*{ 
			LOCK_GUARD(mutex_);
			for (auto ptr = sendMessageList.begin(); ptr != sendMessageList.end(); ptr++)
			{
				auto msg = ptr->get();
				auto iter = conn_set_.find(msg->clientID);
				if (iter == conn_set_.end())
				{
					continue;
				}
				auto connection = iter->second.get();
				if (connection == nullptr)
				{
					continue;
				}
				connection->WriteMessage(*ptr);
			}

			sendMessageList.clear();
		}

		auto iter = conn_set_.find(clientID);
		assert(iter != conn_set_.end());
		if (stopped_.load())
		{
		}
		else if (iter->second->status() == Connection::kError)
		{
			iter->second->Close();
			conn_set_.erase(clientID);
		}
		else if (iter->second->status() == Connection::kClosed)
		{
			conn_set_.erase(clientID);
		}
		else
		{
			socket_timer->expires_from_now(boost::posix_time::seconds(1));
			socket_timer->async_wait(boost::bind(&EchoServer::CheckSocketStatus, shared_from_this(), clientID, socket_timer, _1));
		}*/
	}

	void EchoServer::AddRecvMssage(std::shared_ptr<ClientMessage> message)
	{
		LOCK_GUARD(mutex_);
		recvMessageList.push_back(message);
	}

	bool EchoServer::PopRecvMessage(std::shared_ptr<ClientMessage>& message)
	{
		LOCK_GUARD(mutex_);
		if (false == recvMessageList.empty())
		{
			message = recvMessageList.back();
			recvMessageList.pop_back();
			return true;
		}

		return false;
	}

	void EchoServer::AddSendMssage(std::shared_ptr<ClientMessage> message)
	{
		LOCK_GUARD(mutex_);
		sendMessageList.push_back(message);
	}
}