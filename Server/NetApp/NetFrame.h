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
#endif