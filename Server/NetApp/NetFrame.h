#ifndef __NET_FRAME_H__
#define __NET_FRAME_H__

#ifndef _WIN32
#include "signal.h"
#include "unistd.h"
#endif
#include <memory>
#include <iostream>
#include <unordered_map>
#include "uv.h"
#include <atomic>
#include <thread>
#include "tbb44_20160413oss/include/tbb/concurrent_queue.h"

namespace uv
{
	enum STATUS
	{
		NONE = 0,	//初始化
		RUNING,		//运行中
		CLOSE,		//已关闭
	};

	struct MessageInfo
	{
		uint32_t conn = 0;
		uint32_t len = 0;
		char* buf = nullptr;

		MessageInfo(){}

		MessageInfo(uint32_t conn_, char* buf_, uint32_t len_) : conn(conn_), len(len_)
		{
			buf = buf_;
		}

		MessageInfo(MessageInfo && info)
		{
			buf = info.buf;
			info.buf = nullptr;
			conn = info.conn;
			len = info.len;
		}
	};

	//服务端
	class TCPServer
	{
	public:
		TCPServer(const char* ip_, int port_);
		~TCPServer();
		
		void Run();
		void Close();
		std::shared_ptr<MessageInfo> GetRecvMessage();
		void PushSendMessage(uint32_t conn, char* buf, uint32_t len);
		
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
		int Start();
		void SetStatus(STATUS status_);
		STATUS GetStatus(){ return status.load(); }
		uint32_t GetConnectionID();
		void ReleaseConnection(uv_stream_t* handle);
		void AddConnection(uint32_t conn, uv_stream_t* handle);
		void PushRecvMessage(uv_stream_t* handle, char* buf, uint32_t len);
		std::shared_ptr<MessageInfo> GetSendMessage();
	private:
		std::unordered_map<uv_stream_t*, uint32_t> m_OnlineStream;	//在线连接
		std::unordered_map<uint32_t, uv_stream_t*> m_OnlineID;		//在线连接

		std::list<uint32_t>					m_ConnectionID;	//连接ID
		tbb::concurrent_queue<MessageInfo>	m_SendMessage;	//发送数据
		tbb::concurrent_queue<MessageInfo>	m_RecvMessage;	//接收数据
		
		uint32_t		m_MaxConnectionID;		//当前最大ID
		uv_loop_t*		loop = nullptr;
		uv_tcp_t		tcpServer;
		uv_handle_t*	server = nullptr;
		uv_timer_t		timer;

		char*			ip = nullptr;
		int				port = 0;
		
		std::atomic<STATUS>				status	= NONE;		//服务器状态
		std::shared_ptr<std::thread>	thread_	= nullptr;	//网络线程
	};

	//客户端
	class TCPClient
	{
	public:
		TCPClient(uint32_t unicode_, const char* ip_, int port_);
		void Run();
		void Close();
		std::shared_ptr<MessageInfo> GetRecvMessage();
		void PushSendMssage(MessageInfo&& info);
	private:
		int Start();
		void SetStatus(STATUS status_){ status.store(status_); }
		void PushRecvMessage(MessageInfo &&info);
		std::shared_ptr<MessageInfo> GetSendMessage();
	protected:
		static void alloc_cb(uv_handle_t* handle, size_t size, uv_buf_t* buf);
		static void close_cb(uv_handle_t* handle);
		static void shutdown_cb(uv_shutdown_t* req, int status);
		static void read_cb(uv_stream_t* tcp, ssize_t nread, const uv_buf_t* buf);
		static void timer_cb(uv_timer_t* handle);
		static void write_cb(uv_write_t* req, int status);
		static void connect_cb(uv_connect_t* req, int status);
	private:
		uv_loop_t*	loop = nullptr;
		uv_tcp_t	client;
		uv_timer_t	timer;
		uv_write_t	write_req;
		uv_shutdown_t	shutdown_req;
		uv_connect_t	connect_req;

		uint32_t	unicode = 0;
		char*		ip = nullptr;
		int			port = 0;
		struct		sockaddr_in addr;

		std::atomic<STATUS>					status	= NONE;		//客户端状态
		std::shared_ptr<std::thread>		thread_ = nullptr;	//网络线程

		tbb::concurrent_queue<MessageInfo>	m_SendMessage;
		tbb::concurrent_queue<MessageInfo>	m_RecvMessage;
	};
}
#endif