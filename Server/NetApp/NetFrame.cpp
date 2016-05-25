#include "stdafx.h"
#include "NetFrame.h"

namespace uv
{
	TCPServer::TCPServer(const char* ip_, int port_) :
		m_MaxConnectionID(0)
	{
		ip = new char[10];
		memset(ip, 0, 10);
		port = port_;
		memcpy_s(ip, 10, ip_, strlen(ip_));
	}

	TCPServer::~TCPServer()
	{

	}

	void TCPServer::after_write(uv_write_t* req, int status)
	{
		//free(req);

		if (status != 0)
		{
			fprintf(stderr,
				"uv_write error: %s - %s\n",
				uv_err_name(status),
				uv_strerror(status));
		}
	}

	void TCPServer::after_shutdown(uv_shutdown_t* req, int status)
	{
		uv_close((uv_handle_t*)req->handle, on_close);
		free(req);
	}

	void TCPServer::after_read(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf)
	{
		if (nread < 0)
		{
			if (nread == UV_EOF)
			{
				fprintf(stdout, "客户端连接断开，关闭此客户端\n");
			}
			else if (nread == UV_ECONNRESET)
			{
				fprintf(stdout, "客户端异常断开\n");
			}
			else
			{
				fprintf(stdout, "%s:%s\n", uv_err_name(nread), uv_strerror(nread));
				free(buf->base);
				uv_shutdown_t* sreq = (uv_shutdown_t*)malloc(sizeof(uv_shutdown_t));
				int ret = uv_shutdown(sreq, handle, after_shutdown);
			}

			TCPServer* tcp = (TCPServer*)handle->data;
			//添加空消息给逻辑线程
			tcp->PushRecvMessage(handle, nullptr, 0);
			//销毁在线连接
			tcp->ReleaseConnection(handle);
			return;
		}
		else if (nread == 0)
		{
			free(buf->base);
			fprintf(stdout, "Everything OK, but nothing read.\n");
		}
		else
		{
			int len = strlen(buf->base);
			TCPServer* tcp = (TCPServer*)handle->data;
			tcp->PushRecvMessage(handle, buf->base, nread);
		}

		/*
		* Scan for the letter Q which signals that we should quit the server.
		* If we get QS it means close the stream.
		*/
		// 		if (!server_closed) {
		// 			for (i = 0; i < nread; i++) {
		// 				if (buf->base[i] == 'Q') {
		// 					if (i + 1 < nread && buf->base[i + 1] == 'S') {
		// 						free(buf->base);
		// 						uv_close((uv_handle_t*)handle, on_close);
		// 						return;
		// 					}
		// 					else {
		// 						uv_close(server, on_server_close);
		// 						server_closed = 1;
		// 					}
		// 				}
		// 			}
		// 		}


		// 		uv_buf_t sendbuf;
		// 		sendbuf = uv_buf_init(buf->base, nread);
		// 
		// 		uv_write_t* req = (uv_write_t*)malloc(sizeof(uv_write_t));
		// 		int ret = uv_write(req, handle, &sendbuf, 1, after_write);
		// 		if (0 != ret)
		// 		{
		// 			int x = 0;
		// 			x++;
		// 		}
		// 		free(buf->base);
	}

	void TCPServer::on_close(uv_handle_t* peer)
	{
		free(peer);
	}

	void TCPServer::echo_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
	{
		buf->base = (char*)malloc(suggested_size);
		buf->len = suggested_size;
	}

	void TCPServer::on_connection(uv_stream_t* handle, int status)
	{
		if (status != 0)
		{
			fprintf(stderr, "Connect error %s\n", uv_err_name(status));
			return;
		}

		uv_stream_t *stream = (uv_stream_t*)malloc(sizeof(uv_tcp_t));
		if (stream == NULL)
		{
			fprintf(stderr, "malloc connect error \n");
			return;
		}

		TCPServer* tcpsock = (TCPServer*)handle->data;

		int r = uv_tcp_init(tcpsock->loop, (uv_tcp_t*)stream);
		if (r != 0)
		{
			fprintf(stdout, "%s:%s\n", uv_err_name(r), uv_strerror(r));
			return;
		}

		stream->data = tcpsock;

		r = uv_accept(handle, stream);
		if (r != 0)
		{
			fprintf(stdout, "%s:%s\n", uv_err_name(r), uv_strerror(r));
			return;
		}

		r = uv_read_start(stream, echo_alloc, after_read);
		if (r != 0)
		{
			fprintf(stdout, "%s:%s\n", uv_err_name(r), uv_strerror(r));
			return;
		}

		uint32_t conn = tcpsock->GetConnectionID();
		tcpsock->AddConnection(conn, stream);
	}

	void TCPServer::on_server_close(uv_handle_t* handle)
	{
		// 		if (handle != server)
		// 		{
		// 			fprintf(stderr, "111111111111111 error \n");
		// 		}
	}

	void TCPServer::close_cb(uv_handle_t* handle)
	{
		// 		if (handle == (uv_handle_t*)&timer)
		// 		{
		// 			fprintf(stderr, "211111111111111 error \n");
		// 		}
	}

	void TCPServer::timer_cb(uv_timer_t* handle)
	{
		auto server = (TCPServer*)handle->data;
		auto msg = server->GetSendMessage();

		if (msg != nullptr)
		{
			auto stream = server->m_OnlineID.find(msg->conn);
			if (stream != server->m_OnlineID.end())
			{
				if (msg->buf == nullptr)
				{
					//如果收到的消息为空，则主动断开接连
					uv_shutdown_t* sreq = (uv_shutdown_t*)malloc(sizeof(uv_shutdown_t));
					int ret = uv_shutdown(sreq, stream->second, after_shutdown);
					if (ret != 0)
					{
						fprintf(stderr, "uv_shutdown1 error \n");
					}
				}
				else if (strcmp(msg->buf, "") == 0)
				{
					fprintf(stderr, "msg is empty");
				}
				else
				{
					//发送消息
					uv_write_t req;
					uv_buf_t buf = uv_buf_init(msg->buf, msg->len);
					int ret = uv_write(&req, stream->second, &buf, 1, after_write);

					if (buf.base != nullptr)
					{
						free(buf.base);
						buf.base = nullptr;
					}
				}
			}
		}

		uv_timer_start(handle, timer_cb, 10, 0);
	}

	int TCPServer::tcp4_echo_start()
	{
		loop = uv_default_loop();

		struct sockaddr_in addr;
		if (0 != uv_ip4_addr(ip, port, &addr))
		{
			fprintf(stderr, "311111111111111 error \n");
			return 1;
		}

		server = (uv_handle_t*)&tcpServer;

		int r = uv_tcp_init(loop, &tcpServer);
		if (r != 0)
		{
			fprintf(stderr, "Socket creation error\n");
			return 1;
		}

		r = uv_tcp_bind(&tcpServer, (const struct sockaddr*)&addr, 0);
		if (r != 0)
		{
			fprintf(stderr, "Socket uv_tcp_bind error\n");
			return 1;
		}

		tcpServer.data = this;

		r = uv_listen((uv_stream_t*)&tcpServer, SOMAXCONN, on_connection);
		if (r != 0)
		{
			fprintf(stderr, "Socket uv_listen error\n");
			return 1;
		}

		r = uv_timer_init(loop, &timer);
		if (r != 0) {
			fprintf(stderr, "uv_timer_init %s\n", uv_err_name(r));
			return 1;
		}

		timer.data = this;
		r = uv_timer_start(&timer, timer_cb, 10, 0);
		if (r != 0) {
			fprintf(stderr, "uv_timer_init %s\n", uv_err_name(r));
			return 1;
		}

		uv_run(loop, UV_RUN_DEFAULT);
		return 0;
	}

	uint32_t TCPServer::GetConnectionID()
	{
		if (m_ConnectionID.empty())
		{
			m_MaxConnectionID++;
			return m_MaxConnectionID;
		}

		uint32_t id = m_ConnectionID.front();
		m_ConnectionID.pop_front();

		return id;
	}

	void TCPServer::AddConnection(uint32_t conn, uv_stream_t* handle)
	{
		m_OnlineStream[handle] = conn;
		m_OnlineID[conn] = handle;
	}

	void TCPServer::ReleaseConnection(uv_stream_t* handle)
	{
		for (auto ptr = m_OnlineID.begin(); ptr != m_OnlineID.end(); ptr++)
		{
			if (ptr->second == handle)
			{
				m_ConnectionID.push_back(ptr->first);
				m_OnlineID.erase(ptr);
				break;
			}
		}

		for (auto ptr = m_OnlineStream.begin(); ptr != m_OnlineStream.end(); ptr++)
		{
			if (ptr->first == handle)
			{
				m_OnlineStream.erase(ptr);
				break;
			}
		}
	}

	void TCPServer::PushRecvMessage(uv_stream_t* handle, char* buf, uint32_t len)
	{
		auto conn = m_OnlineStream[handle];
		std::lock_guard<std::mutex> lck(mutex_recv);
		MessageInfo msg(conn, buf, len);
		m_RecvMessage.push_back(msg);
	}

	std::shared_ptr<MessageInfo> TCPServer::GetRecvMessage()
	{
		std::lock_guard<std::mutex> lck(mutex_recv);
		if (m_RecvMessage.empty() == false)
		{
			MessageInfo msg = m_RecvMessage.front();
			m_RecvMessage.pop_front();
			return std::make_shared<MessageInfo>(msg);
		}

		return nullptr;
	}

	void TCPServer::PushSendMessage(uint32_t conn, char* buf, uint32_t len)
	{
		MessageInfo msg(conn, buf, len);
		std::lock_guard<std::mutex> lck(mutex_send);
		m_SendMessage.push_back(msg);
	}

	std::shared_ptr<MessageInfo> TCPServer::GetSendMessage()
	{
		std::lock_guard<std::mutex> lck(mutex_send);
		if (m_SendMessage.empty() == false)
		{
			auto msg = std::make_shared<MessageInfo>(m_SendMessage.front());
			m_SendMessage.pop_front();
			return msg;
		}

		return nullptr;
	}

	void TCPClient::alloc_cb(uv_handle_t* handle, size_t size, uv_buf_t* buf)
	{
		buf->len = size;
		buf->base = (char*)malloc(size);
	}

	void TCPClient::close_cb(uv_handle_t* handle)
	{
		TCPClient* tcpClient = (TCPClient*)handle->data;
		tcpClient->status = NONE;
		uv_tcp_init(tcpClient->loop, &tcpClient->client);
	}

	void TCPClient::shutdown_cb(uv_shutdown_t* req, int status)
	{
	}

	void TCPClient::read_cb(uv_stream_t* tcp, ssize_t nread, const uv_buf_t* buf)
	{
		free(buf->base);
		if (nread == 0)
		{
			return;
		}
		else if (nread < 0)
		{
			uv_close((uv_handle_t*)tcp, close_cb);
			return;
		}
		TCPClient* tcpClient = (TCPClient*)tcp->data;
		MessageInfo msg(tcpClient->unicode, buf->base, nread);
	}

	void TCPClient::timer_cb(uv_timer_t* handle)
	{
		TCPClient* tcpClient = (TCPClient*)handle->data;

		if (tcpClient->status != CONNECTED)
		{
			//断线再次连接
			uv_tcp_connect(&tcpClient->connect_req, &tcpClient->client, (const struct sockaddr*)&tcpClient->addr, connect_cb);
			return;
		}

		if (uv_read_start((uv_stream_t*)&tcpClient->client, alloc_cb, read_cb))
		{
		}

		//TODO
		static const char MESSAGE[] = "Failure is for the weak. Everyone dies alone.";
		uv_buf_t buf;
		buf.base = (char*)&MESSAGE;
		buf.len = sizeof MESSAGE;

		if (uv_write(&tcpClient->write_req, (uv_stream_t*)tcpClient->connect_req.handle, &buf, 1, write_cb)) {
			std::cout << "uv_write failed" << std::endl;
		}

		int r = uv_timer_start(&tcpClient->timer, timer_cb, 10, 0);
		if (r)
		{
			return;
		}
	}

	void TCPClient::write_cb(uv_write_t* req, int status)
	{
	}

	void TCPClient::connect_cb(uv_connect_t* req, int status)
	{
		TCPClient* tcpClient = (TCPClient*)req->data;
		if (0 != status)
		{
			std::cout <<"connect faild:"<< status << std::endl;
		}
		else
		{
			tcpClient->status = CONNECTED;
		}

		int r = uv_timer_init(tcpClient->loop, &tcpClient->timer);
		if (r)
		{
			return;
		}
		tcpClient->timer.data = (void*)tcpClient;
		r = uv_timer_start(&tcpClient->timer, timer_cb, 10, 0);
		if (r)
		{
			return;
		}
	}

	TCPClient::TCPClient(uint32_t unicode_, const char* ip_, int port_)
	{
		ip = new char[10];
		memset(ip, 0, 10);
		port = port_;
		memcpy_s(ip, 10, ip_, strlen(ip_));
		unicode = unicode_;
	}

	int TCPClient::tcp4_echo_start()
	{
		loop = uv_default_loop();
		
		int r = uv_ip4_addr(ip, port, &addr);
		if (0 != r)
		{
			return r;
		}

		r = uv_tcp_init(loop, &client);

		client.data = this;
		connect_req.data = this;
		r = uv_tcp_connect(&connect_req, &client, (const struct sockaddr*)&addr, connect_cb);
		if (0 != r)
		{
			return r;
		}
		status = CONNECTING;

		uv_run(loop, UV_RUN_DEFAULT);

		uv_loop_delete(loop);

		return r;
	}

	void TCPClient::PushRecvMessage(MessageInfo &&info)
	{
		m_RecvMessage.push(info);
	}

	std::shared_ptr<MessageInfo> TCPClient::GetRecvMessage()
	{
		std::shared_ptr<MessageInfo> msg = nullptr;
		MessageInfo info;
		if (true == m_RecvMessage.try_pop(info))
		{
			msg = std::make_shared<MessageInfo>(info);
		}

		return msg;
	}

	void TCPClient::PushSendMssage(MessageInfo&& info)
	{
		m_SendMessage.push(info);
	}

	std::shared_ptr<MessageInfo> TCPClient::GetSendMessage()
	{
		std::shared_ptr<MessageInfo> msg = nullptr;
		MessageInfo info;
		if (true == m_SendMessage.try_pop(info))
		{
			msg = std::make_shared<MessageInfo>(info);
		}

		return msg;
	}
}