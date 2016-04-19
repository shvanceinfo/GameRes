#ifndef __NET_FRAME_H__
#define __NET_FRAME_H__

#ifndef _WIN32
#include "signal.h"
#include "unistd.h"
#endif
// #include <boost/asio.hpp>
// #include <boost/thread.hpp>
// #include <boost/bind.hpp>
// #include <boost/shared_ptr.hpp>
// #include <boost/enable_shared_from_this.hpp>
// #include <boost/thread/thread.hpp>
// #include <boost/date_time/posix_time/posix_time.hpp>
// #include <boost/atomic.hpp>
#include <memory>
#include <assert.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include "uv.h"
#include <vector>
#include <mutex>

namespace uv
{
	struct MessageInfo
	{
		uint32_t conn = 0;
		char* buf = nullptr;

		MessageInfo(uint32_t conn_, char* buf_) : conn(conn_)
		{
			buf = buf_;
			buf_ = nullptr;
		}

		MessageInfo(MessageInfo && info)
		{
			buf = info.buf;
			info.buf = nullptr;
			conn = info.conn;
		}
	};

	class TCPServer
	{
	public:
		TCPServer(const char* ip_, int port_);
		~TCPServer();
		int tcp4_echo_start();

		std::shared_ptr<MessageInfo> GetRecvMessage();
		void PushSendMessage(uint32_t conn, char* buf);

	protected:
		static void after_write(uv_write_t* req, int status);
		static void after_read(uv_stream_t*, ssize_t nread, const uv_buf_t* buf);
		static void on_close(uv_handle_t* peer);
		static void on_server_close(uv_handle_t* handle);
		static void on_connection(uv_stream_t*, int status);
		static void after_shutdown(uv_shutdown_t* req, int status);
		static void echo_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
		static void close_cb(uv_handle_t* handle);
		static void timer_cb(uv_timer_t* handle);

	private:
		uint32_t GetConnectionID();
		void ReleaseConnection(uv_stream_t* handle);
		void AddConnection(uint32_t conn, uv_stream_t* handle);
		void PushRecvMessage(uv_stream_t* handle, char* buf);
		std::shared_ptr<MessageInfo> GetSendMessage();
	private:
		std::unordered_map<uv_stream_t*, uint32_t> m_OnlineStream;	//在线连接
		std::unordered_map<uint32_t, uv_stream_t*> m_OnlineID;		//在线连接

		std::list<uint32_t>		m_ConnectionID;	//连接ID
		std::list<MessageInfo>	m_SendMessage;	//发送数据
		std::list<MessageInfo>	m_RecvMessage;	//接收数据

		std::mutex	mutex_recv, mutex_send;	//锁
		
		uint32_t	m_MaxConnectionID;		//当前最大ID
		uv_loop_t*	loop = nullptr;
		uv_tcp_t	tcpServer;
		uv_handle_t* server = nullptr;
		uv_timer_t	timer;
		char *ip;
		int port = 0;
	};
}
// 
// #define LOCK_GUARD(x) boost::lock_guard<boost::mutex> guard(x);
// 
// namespace NETAPP
// {
// 	class EchoServer;
// 	typedef boost::shared_ptr<EchoServer> EchoServerPtr;
// 	typedef boost::shared_ptr<boost::asio::io_service> IOServeicePtr;
// 	typedef boost::shared_ptr<boost::asio::ip::tcp::socket> SocketPtr;
// 
// 	class Connection;
// 	typedef boost::shared_ptr<Connection> ConnPtr;
// 	typedef boost::shared_ptr<std::string> StringPtr;
// 	typedef boost::shared_ptr<boost::asio::deadline_timer> TimerPtr;
// 
// 	struct ClientMessage
// 	{
// 		uint32_t clientID = 0;
// 		std::string buffer = "";
// 		ClientMessage(){}
// 		ClientMessage(uint32_t clientID_, std::string buffer_) : clientID(clientID_), buffer(buffer_){}
// 	};
// 	typedef std::list<ClientMessage> MessageList;
// 
// 	class Connection : public boost::enable_shared_from_this<Connection>
// 	{
// 	public:
// 		enum ConnStatus
// 		{
// 			kConnected = 0,
// 			kError = 1,
// 			kClosed = 2,
// 		};
// 
// 		Connection(SocketPtr socket, uint64_t id) : status_(kConnected), socket_(socket), ClientID(id){ memset(msgbuf_, 0, sizeof(msgbuf_)); }
// 		~Connection()
// 		{
// 			std::cout << __FUNCTION__ << std::endl;
// 		}
// 
// 		void Start();
// 
// 		void Close(bool send = true);
// 
// 		ConnStatus status() { return status_.load(); }
// 		
// 		void setStatus(ConnStatus value);
// 		void SetServerPtr(EchoServer* Serverptr)
// 		{ 
// 			ServerPtr_ = Serverptr; 
// 		}
// 
// 		void WriteMessage(ClientMessage msg);
// 	private:
// 		void ReadHandler(const boost::system::error_code & error, std::size_t bytes_transferred);
// 
// 		void WriteHandler(const boost::system::error_code& error, std::size_t bytes_transferred);
// 
// 		boost::atomic<ConnStatus> status_;
// 		char msgbuf_[1024 * 16];
// 		SocketPtr socket_;
// 		uint32_t ClientID;
// 		EchoServer* ServerPtr_;
// 	};
// 
// 	class EchoServer : public boost::enable_shared_from_this<EchoServer>
// 	{
// 	public:
// 		EchoServer(IOServeicePtr io_service);
// 		~EchoServer()
// 		{
// 			std::cout << __FUNCTION__ << std::endl;
// 			for (auto iter = conn_set_.begin(); iter != conn_set_.end(); iter++)
// 			{
// 				iter->second->Close();
// 			}
// 		}
// 
// 		int Start(const std::string &host, unsigned short port);
// 
// 		void Stop()
// 		{
// 			boost::system::error_code errcode;
// 			if (acceptor_.close(errcode))
// 			{
// 				std::cerr << "" << std::endl;
// 			}
// 			stopped_.store(true);
// 		}
// 
// 		uint32_t GetClientID();
// 		void AddRecvMssage(ClientMessage message);
// 		bool PopRecvMessage(ClientMessage& message);
// 		void AddSendMssage(ClientMessage message);
// 	private:
// 		void AcceptHandler(SocketPtr socket, const boost::system::error_code& error);
// 
// 		void CheckSocketStatus(uint32_t clientID, TimerPtr socket_timer, const boost::system::error_code& error);
// 
// 		typedef std::unordered_map<uint32_t,ConnPtr> ConnMap;
// 		
// 		boost::atomic<int> stopped_;
// 		ConnMap conn_set_;
// 		IOServeicePtr io_service_;
// 		boost::asio::ip::tcp::acceptor acceptor_;
// 		std::list<uint32_t> userIDList;
// 		uint32_t maxID;
// 		//////////////////////////////////////////////////////////////////////////
// 		//TODO,改为无锁队列
// 		MessageList recvMessageList;
// 		MessageList sendMessageList;
// 		//////////////////////////////////////////////////////////////////////////
// 		boost::mutex mutex_;
// 	};
// 
// 	static volatile sig_atomic_t g_shutdown_server = 0;
// 	static void ShutdownServerHandler(int signo)
// 	{
// 		g_shutdown_server = 1;
// 	}
// 
// 	static void SetupSignalHandler()
// 	{
// #ifndef _WIN32
// 		sigset_t sigset;
// 		sigfillset(&sigset);
// 		sigdelset(&sigset, SIGTERM);
// 		sigdelset(&sigset, SIGINT);
// 		sigprocmast(SIG_SETMASK, &sigset, NULL);
// 
// 		struct sigaction act;
// 		memset(&act, 0, sizeof(act));
// 		act.sa_handler = ShutdownServerHandler;
// 		sigaction(SIGINT, &act, NULL);
// 		sigaction(SIGTERM, &act, NULL);
// #endif
// 	}
// 
// 	static void AsioThreadMain(IOServeicePtr io_service)
// 	{
// 		io_service->run();
// 	}
// }
#endif