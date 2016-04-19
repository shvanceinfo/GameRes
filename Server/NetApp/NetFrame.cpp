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
			tcp->PushRecvMessage(handle, nullptr);
			//销毁在线连接
			tcp->ReleaseConnection(handle);
			return;
		}else if (nread == 0)
		{
			free(buf->base);
			fprintf(stdout, "Everything OK, but nothing read.\n");
		}else 
		{
			TCPServer* tcp = (TCPServer*)handle->data;
			buf->base[nread] = '\0';
			tcp->PushRecvMessage(handle, buf->base);

			auto conn = tcp->m_OnlineStream.find(handle);
			tcp->PushSendMessage(conn->second, nullptr);
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
				else
				{
					//发送消息
					uv_write_t req;
					uv_buf_t buf = uv_buf_init(msg->buf, strlen(msg->buf));
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

	void TCPServer::PushRecvMessage(uv_stream_t* handle, char* buf)
	{
		auto conn = m_OnlineStream[handle];
		std::lock_guard<std::mutex> lck(mutex_recv);
		m_RecvMessage.push_back(MessageInfo(conn, buf));
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

	void TCPServer::PushSendMessage(uint32_t conn, char* buf)
	{
		MessageInfo msg(conn, buf);
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
}
// 
// namespace NETAPP{
// 	void Connection::Start()
// 	{
// 		socket_->async_receive(boost::asio::buffer(msgbuf_, sizeof(msgbuf_)), boost::bind(&Connection::ReadHandler, shared_from_this(), _1, _2));
// 	}
// 
// 	void Connection::Close(bool send /* = true */)
// 	{
// 		if (status_.exchange(kClosed) != kClosed)
// 		{
// 			boost::system::error_code errcode;
// 			if (socket_->close(errcode))
// 			{
// 				std::cerr << "" << std::endl;
// 			}
// 			else
// 			{
// 				std::cerr << "" << std::endl;
// 			}
// 			
// 			if (send)
// 			{
// 				//发空包处理连接断开
// 				ClientMessage msg(ClientID, "");
// 				ServerPtr_->AddRecvMssage(msg);
// 			}
// 		}
// 	}
// 
// 	void Connection::ReadHandler(const boost::system::error_code & error, std::size_t bytes_transferred)
// 	{
// 		if (!error)
// 		{
// 			msgbuf_[bytes_transferred] = '\0';
// 			std::string sendMessage(msgbuf_);
// 			ClientMessage msg(ClientID, sendMessage);
// 			ServerPtr_->AddRecvMssage(msg);
// 
// 			socket_->async_receive(boost::asio::buffer(msgbuf_,sizeof(msgbuf_)), boost::bind(&Connection::ReadHandler, shared_from_this(), _1, _2));
// 		}
// 		else if (error == boost::asio::error::operation_aborted)
// 		{
// 			std::cout << "" << std::endl;
// 		}
// 		else
// 		{
// 			ConnStatus expected = kConnected;
// 			if (status_.compare_exchange_strong(expected, kError))
// 			{
// 				std::cout << "" << std::endl;
// 			}
// 		}
// 	}
// 
// 	void Connection::WriteHandler(const boost::system::error_code& error, std::size_t bytes_transferred)
// 	{
// 		if (!error)
// 		{
// 			std::cout << "send Message sucees" << std::endl;
// 		}
// 		else if (error == boost::asio::error::operation_aborted)
// 		{
// 			std::cout << "" << std::endl;
// 		}
// 		else
// 		{
// 			ConnStatus expected = kConnected;
// 			if (status_.compare_exchange_strong(expected, kError))
// 			{
// 				std::cout << "" << std::endl;
// 			}
// 		}
// 	}
// 
// 	void Connection::WriteMessage(ClientMessage msg)
// 	{
// 		if (msg.buffer.empty())
// 		{
// 			//服务器主动断开连接
// 			Close(false);
// 		}
// 		else
// 		{
// 			std::cout << "=======" << msg.buffer <<"======"<< std::endl;
// 			async_write(*socket_, boost::asio::buffer(msg.buffer), boost::bind(&Connection::WriteHandler, shared_from_this(), _1, _2));
// 		}
// 	}
// 
// 	void Connection::setStatus(ConnStatus value)
// 	{
// 		status_.exchange(value);
// 	}
// 
// 	EchoServer::EchoServer(IOServeicePtr io_service) : stopped_(false), io_service_(io_service), acceptor_(*io_service), maxID(0)
// 	{
// 	}
// 
// 	int EchoServer::Start(const std::string &host, unsigned short port)
// 	{
// 		boost::system::error_code errcode;
// 		boost::asio::ip::address address = boost::asio::ip::address::from_string(host, errcode);
// 		if (errcode)
// 		{
// 			return false;
// 		}
// 
// 		if (acceptor_.open(boost::asio::ip::tcp::v4(), errcode))
// 		{
// 			return false;
// 		}
// 
// 		acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
// 		boost::asio::ip::tcp::endpoint endpoint(address, port);
// 		if (acceptor_.bind(endpoint, errcode) || acceptor_.listen(1024, errcode))
// 		{
// 			return false;
// 		}
// 
// 		SocketPtr socket(new boost::asio::ip::tcp::socket(*io_service_));
// 		acceptor_.async_accept(*socket, boost::bind(&EchoServer::AcceptHandler, shared_from_this(), socket, _1));
// 		return true;
// 	}
// 
// 	uint32_t EchoServer::GetClientID()
// 	{
// 		uint32_t clientID = 0;
// 		if (userIDList.empty())
// 		{
// 			maxID++;
// 			clientID = maxID;
// 		}
// 		else
// 		{
// 			clientID = userIDList.front();
// 			userIDList.pop_front();
// 		}
// 
// 		return clientID;
// 	}
// 
// 	void EchoServer::AcceptHandler(SocketPtr socket, const boost::system::error_code& error)
// 	{
// 		if (error == boost::asio::error::operation_aborted)
// 		{
// 			std::cout << "" << std::endl;
// 			return;
// 		}
// 		else if (!error)
// 		{
// 			std::cout << "" << std::endl;
// 			auto clientID = GetClientID();
// 			ConnPtr new_conn(new Connection(socket, clientID));
// 			new_conn->SetServerPtr(this);
// 			new_conn->Start();
// 			
// 			conn_set_.insert(std::make_pair(clientID, new_conn));
// 
// 			TimerPtr socket_time(new boost::asio::deadline_timer(*io_service_));
// 			socket_time->expires_from_now(boost::posix_time::millisec(1));
// 			socket_time->async_wait(boost::bind(&EchoServer::CheckSocketStatus, shared_from_this(), clientID, socket_time, _1));
// 		}
// 		else
// 		{
// 			std::cout << "" << std::endl;
// 		}
// 
// 		SocketPtr new_socket(new boost::asio::ip::tcp::socket(*io_service_));
// 		acceptor_.async_accept(*new_socket, boost::bind(&EchoServer::AcceptHandler, shared_from_this(), new_socket, _1));
// 	}
// 
// 	void EchoServer::CheckSocketStatus(uint32_t clientID, TimerPtr socket_timer, const boost::system::error_code& error)
// 	{
// 		//遍历要发送的数据
// 		{ 
// 			LOCK_GUARD(mutex_);
// 			for (auto ptr = sendMessageList.begin(); ptr != sendMessageList.end(); ptr++)
// 			{
// 				auto iter = conn_set_.find(ptr->clientID);
// 				if (iter == conn_set_.end())
// 				{
// 					continue;
// 				}
// 				auto connection = iter->second.get();
// 				if (connection == nullptr)
// 				{
// 					continue;
// 				}
// 				connection->WriteMessage(*ptr);
// 			}
// 
// 			sendMessageList.clear();
// 		}
// 
// 		auto iter = conn_set_.find(clientID);
// 		assert(iter != conn_set_.end());
// 		if (stopped_.load())
// 		{
// 		}
// 		else if (iter->second->status() == Connection::kError)
// 		{
// 			iter->second->Close();
// 			conn_set_.erase(clientID);
// 		}
// 		else if (iter->second->status() == Connection::kClosed)
// 		{
// 			conn_set_.erase(clientID);
// 		}
// 		else
// 		{
// 			socket_timer->expires_from_now(boost::posix_time::seconds(1));
// 			socket_timer->async_wait(boost::bind(&EchoServer::CheckSocketStatus, shared_from_this(), clientID, socket_timer, _1));
// 		}
// 	}
// 
// 	void EchoServer::AddRecvMssage(ClientMessage message)
// 	{
// 		LOCK_GUARD(mutex_);
// 		recvMessageList.push_back(message);
// 	}
// 
// 	bool EchoServer::PopRecvMessage(ClientMessage& message)
// 	{
// 		LOCK_GUARD(mutex_);
// 		if (false == recvMessageList.empty())
// 		{
// 			message = recvMessageList.back();
// 			recvMessageList.pop_back();
// 			return true;
// 		}
// 
// 		return false;
// 	}
// 
// 	void EchoServer::AddSendMssage(ClientMessage message)
// 	{
// 		LOCK_GUARD(mutex_);
// 		sendMessageList.push_back(message);
// 	}
// }