#ifndef __NET_FRAME_H__
#define __NET_FRAME_H__

#ifndef _WIN32
#include "signal.h"
#include "unistd.h"
#endif

#include <assert.h>
#include <iostream>
#include <string>
#include <deque>
#include <set>
#include <atomic>
#include <thread>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/atomic.hpp>
#include <unordered_map>


#define LOCK_GUARD(x) boost::lock_guard<boost::mutex> guard(x);

namespace NETAPP
{
	class EchoServer;
	typedef boost::shared_ptr<EchoServer> EchoServerPtr;
	typedef boost::shared_ptr<boost::asio::io_service> IOServeicePtr;
	typedef boost::shared_ptr<boost::asio::ip::tcp::socket> SocketPtr;

	class Connection;
	typedef boost::shared_ptr<Connection> ConnPtr;
	typedef boost::shared_ptr<std::string> StringPtr;
	typedef boost::shared_ptr<boost::asio::deadline_timer> TimerPtr;

	struct ClientMessage
	{
		uint32_t clientID = 0;
		std::shared_ptr<std::string> buffer = nullptr;
		ClientMessage(){}
		ClientMessage(uint32_t clientID_, std::shared_ptr<std::string> buffer_) : clientID(clientID_), buffer(buffer_){}
	};
	typedef std::list<std::shared_ptr<ClientMessage>> MessageList;

	class Connection : public boost::enable_shared_from_this<Connection>
	{
	public:
		enum ConnStatus
		{
			kConnected = 0,
			kError = 1,
			kClosed = 2,
		};

		Connection(SocketPtr socket, uint64_t id) : status_(kConnected), socket_(socket), ClientID(id){ memset(msgbuf_, 0, sizeof(msgbuf_)); }
		~Connection()
		{
			std::cout << __FUNCTION__ << std::endl;
		}

		void Start();

		void Close(bool send = true);

		ConnStatus status() { return status_.load(); }
		
		void setStatus(ConnStatus value);
		void SetServerPtr(EchoServer* Serverptr)
		{ 
			ServerPtr_ = Serverptr; 
		}

		void WriteMessage(std::shared_ptr<ClientMessage> msg);
	private:
		void ReadHandler(const boost::system::error_code & error, std::size_t bytes_transferred);

		void WriteHandler(const boost::system::error_code& error, std::size_t bytes_transferred);

		boost::atomic<ConnStatus> status_;
		char msgbuf_[1024 * 16];
		SocketPtr socket_;
		uint32_t ClientID;
		EchoServer* ServerPtr_;
	};

	class EchoServer : public boost::enable_shared_from_this<EchoServer>
	{
	public:
		EchoServer(IOServeicePtr io_service);
		~EchoServer()
		{
			std::cout << __FUNCTION__ << std::endl;
			for (auto iter = conn_set_.begin(); iter != conn_set_.end(); iter++)
			{
				iter->second->Close();
			}
		}

		int Start(const std::string &host, unsigned short port);

		void Stop()
		{
			boost::system::error_code errcode;
			if (acceptor_.close(errcode))
			{
				std::cerr << "" << std::endl;
			}
			stopped_.store(true);
		}

		uint32_t GetClientID();
		void AddRecvMssage(std::shared_ptr<ClientMessage> message);
		bool PopRecvMessage(std::shared_ptr<ClientMessage>& message);
		void AddSendMssage(std::shared_ptr<ClientMessage> message);
	private:
		void AcceptHandler(SocketPtr socket, const boost::system::error_code& error);

		void CheckSocketStatus(uint32_t clientID, TimerPtr socket_timer, const boost::system::error_code& error);

		typedef std::unordered_map<uint32_t,ConnPtr> ConnMap;
		
		boost::atomic<int> stopped_;
		ConnMap conn_set_;
		IOServeicePtr io_service_;
		boost::asio::ip::tcp::acceptor acceptor_;
		std::list<uint32_t> userIDList;
		uint32_t maxID;
		//////////////////////////////////////////////////////////////////////////
		//TODO,改为无锁队列
		MessageList recvMessageList;
		MessageList sendMessageList;
		//////////////////////////////////////////////////////////////////////////
		boost::mutex mutex_;
	};

	static volatile sig_atomic_t g_shutdown_server = 0;
	static void ShutdownServerHandler(int signo)
	{
		g_shutdown_server = 1;
	}

	static void SetupSignalHandler()
	{
#ifndef _WIN32
		sigset_t sigset;
		sigfillset(&sigset);
		sigdelset(&sigset, SIGTERM);
		sigdelset(&sigset, SIGINT);
		sigprocmast(SIG_SETMASK, &sigset, NULL);

		struct sigaction act;
		memset(&act, 0, sizeof(act));
		act.sa_handler = ShutdownServerHandler;
		sigaction(SIGINT, &act, NULL);
		sigaction(SIGTERM, &act, NULL);
#endif
	}

	static void AsioThreadMain(IOServeicePtr io_service)
	{
		io_service->run();
	}
}
#endif