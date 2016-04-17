// #include "stdafx.h"
// #include "NetSocket.h"
// 
// namespace LIBUV
// {
// 	TCPServer::TCPServer(uv_loop_t* loop /* = uv_default_loop() */)
// 		: newconcb(nullptr), isinit(false)
// 	{
// 		loop = loop;
// 	}
// 
// 	TCPServer::~TCPServer()
// 	{
// 		close();
// 	}
// 
// 	bool TCPServer::init()
// 	{
// 		if (isinit)
// 		{
// 			return true;
// 		}
// 
// 		if (!loop)
// 		{
// 			return false;
// 		}
// 
// 		int iret = uv_tcp_init(loop, &server);
// 		if (iret)
// 		{
// 			return false;
// 		}
// 
// 		isinit = true;
// 
// 		server.data = this;
// 		return true;
// 	}
// 
// 	void TCPServer::close()
// 	{
// 		for (auto it = clients_list.begin(); it != clients_list.end(); it++)
// 		{
// 			auto ptr = it->second;
// 			uv_close((uv_handle_t*)ptr->client_handle, AfterClientClose);
// 		}
// 
// 		clients_list.clear();
// 
// 		if (isinit)
// 		{
// 			uv_close((uv_handle_t*)&server, AfterServerClose);
// 		}
// 
// 		isinit = false;
// 	}
// 
// 	bool TCPServer::run(int status)
// 	{
// 		int iret = uv_run(loop, (uv_run_mode)status);
// 		if (iret)
// 		{
// 			return false;
// 		}
// 		return true;
// 	}
// 
// 	bool TCPServer::setNoDelay(bool enable)
// 	{
// 		int iret = uv_tcp_nodelay(&server, enable ? 1 : 0);
// 		if (iret)
// 		{
// 			return false;
// 		}
// 		return true;
// 	}
// 
// 	bool TCPServer::setKeepAlive(int enable, uint32_t delay)
// 	{
// 		int iret = uv_tcp_keepalive(&server, enable, delay);
// 		if (iret)
// 		{
// 			return false;
// 		}
// 		return true;
// 	}
// 
// 	bool TCPServer::bind(const char*ip, int port)
// 	{
// 		struct sockaddr_in bind_addr;
// 		int iret = uv_ip4_addr(ip, port, &bind_addr);
// 		if (iret)
// 		{
// 			return false;
// 		}
// 
// 		iret = uv_tcp_bind(&server, (const struct sockaddr*)&bind_addr, 0);
// 		if (iret)
// 		{
// 			return false;
// 		}
// 
// 		return true;
// 	}
// 
// 	bool TCPServer::listen(int backlog)
// 	{
// 		int iret = uv_listen((uv_stream_t*)&server, backlog, acceptConnection);
// 		if (iret)
// 		{
// 			return false;
// 		}
// 
// 		return true;
// 	}
// 
// 	bool TCPServer::Start(const char* ip, int port)
// 	{
// 		close();
// 
// 		if (!init())
// 		{
// 			return false;
// 		}
// 
// 		if (!bind(ip,port))
// 		{
// 			return false;
// 		}
// 
// 		if (!listen(SOMAXCONN))
// 		{
// 			return false;
// 		}
// 
// 		if (!run())
// 		{
// 			return false;
// 		}
// 
// 		return true;
// 	}
// 
// 	int TCPServer::send(int clientid, const char* data, std::size_t len)
// 	{
// 		auto itfind = clients_list.find(clientid);
// 		if (itfind == clients_list.end())
// 		{
// 			return -1;
// 		}
// 
// 		if (itfind->second->writebuffer.len < len)
// 		{
// 			itfind->second->writebuffer.base = (char*)realloc(itfind->second->writebuffer.base, len);
// 			itfind->second->writebuffer.len = len;
// 		}
// 
// 		memcpy(itfind->second->writebuffer.base, data, len);
// 		uv_buf_t buf = uv_buf_init((char*)itfind->second->writebuffer.base, len);
// 		int iret = uv_write(&itfind->second->write_req, (uv_stream_t*)itfind->second->client_handle, &buf, 1, AfterSend);
// 		if (iret)
// 		{
// 			return false;
// 		}
// 
// 		return true;
// 	}
// 
// 	void TCPServer::acceptConnection(uv_handle_t* server, int status)
// 	{
// 		if (!server->data)
// 		{
// 			return;
// 		}
// 
// 		TCPServer* tcpsock = (TCPServer*)server->data;
// 		int clientid = tcpsock->GetAvailaClientID();
// 		ClientData* cdata = new ClientData(clientid);
// 		cdata->tcp_server = tcpsock;
// 		int iret = uv_tcp_init(tcpsock->loop, cdata->client_handle);
// 		if (iret)
// 		{
// 			delete cdata;
// 			return;
// 		}
// 
// 		iret = uv_accept((uv_stream_t*)&tcpsock->server, (uv_stream_t*)cdata->client_handle);
// 		if (iret)
// 		{
// 			uv_close((uv_handle_t*)cdata->client_handle, NULL);
// 			delete cdata;
// 			return;
// 		}
// 
// 		tcpsock->clients_list.insert(std::make_pair(clientid, cdata));
// 		if (tcpsock->newconcb)
// 		{
// 			tcpsock->newconcb(clientid);
// 		}
// 
// 		iret = uv_read_start((uv_stream_t*)cdata->client_handle, onAllocBuffer, AfterServerRecv);
// 	}
// 
// 	void TCPServer::setrecvcb(uint32_t client_id, server_recvcb cb)
// 	{
// 		auto itfind = clients_list.find(client_id);
// 		if (itfind != clients_list.end())
// 		{
// 			itfind->second->recvdb = cb;
// 		}
// 	}
// 
// 	void TCPServer::setnewconnectcb(newconnect cb)
// 	{
// 		newconcb = cb;
// 	}
// 
// 	void TCPServer::onAllocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
// 	{
// 		if (!handle->data)
// 		{
// 			return;
// 		}
// 
// 		ClientData * client = (ClientData*)handle->data;
// 		*buf = client->readbuffer;
// 	}
// 
// 	void TCPServer::AfterServerRecv(uv_stream_t* client, ssize_t nred, const uv_buf_t* buf)
// 	{
// 		if
// 	}
// }

///////////////////////////////////
#include "stdafx.h"
#include "NetSocket.h"
//#include "log4z.h"

std::string GetUVError(int retcode)
{
	std::string err;
	err = uv_err_name(retcode);
	err += ":";
	err += uv_strerror(retcode);
	return std::move(err);
}

namespace uv
{
	/*****************************************TCP Server*************************************************************/
	TCPServer::TCPServer(uv_loop_t* loop)
		:newconcb_(nullptr), isinit_(false)
	{
		loop_ = loop;
	}


	TCPServer::~TCPServer()
	{
		close();
		//LOGI("tcp server exit.");
	}

	//��ʼ����ر�--��������ͻ���һ��
	bool TCPServer::init()
	{
		if (isinit_) {
			return true;
		}

		if (!loop_) {
			errmsg_ = "loop is null on tcp init.";
			//LOGE(errmsg_);
			return false;
		}
		int iret = uv_mutex_init(&mutex_handle_);
		if (iret) {
			errmsg_ = GetUVError(iret);
			//LOGE(errmsg_);
			return false;
		}
		iret = uv_tcp_init(loop_, &server_);
		if (iret) {
			errmsg_ = GetUVError(iret);
			//LOGE(errmsg_);
			return false;
		}
		isinit_ = true;
		server_.data = this;
		//iret = uv_tcp_keepalive(&server_, 1, 60);//���ô˺����������������ó���
		//if (iret) {
		//	errmsg_ = GetUVError(iret);
		//	return false;
		//}
		return true;
	}

	void TCPServer::close()
	{
		for (auto it = clients_list_.begin(); it != clients_list_.end(); ++it) {
			auto data = it->second;
			uv_close((uv_handle_t*)data->client_handle, AfterClientClose);
		}
		clients_list_.clear();

		//LOGI("close server");
		if (isinit_) {
			uv_close((uv_handle_t*)&server_, AfterServerClose);
			//LOGI("close server");
		}
		isinit_ = false;
		uv_mutex_destroy(&mutex_handle_);
	}

	bool TCPServer::run(int status)
	{
		//LOGI("server runing.");
		int iret = uv_run(loop_, (uv_run_mode)status);
		if (iret) {
			errmsg_ = GetUVError(iret);
			//LOGE(errmsg_);
			return false;
		}
		return true;
	}
	//��������--��������ͻ���һ��
	bool TCPServer::setNoDelay(bool enable)
	{
		int iret = uv_tcp_nodelay(&server_, enable ? 1 : 0);
		if (iret) {
			errmsg_ = GetUVError(iret);
			//LOGE(errmsg_);
			return false;
		}
		return true;
	}

	bool TCPServer::setKeepAlive(int enable, unsigned int delay)
	{
		int iret = uv_tcp_keepalive(&server_, enable, delay);
		if (iret) {
			errmsg_ = GetUVError(iret);
			//LOGE(errmsg_);
			return false;
		}
		return true;
	}

	//��Ϊserverʱ�ĺ���
	bool TCPServer::bind(const char* ip, int port)
	{
		struct sockaddr_in bind_addr;
		int iret = uv_ip4_addr(ip, port, &bind_addr);
		if (iret) {
			errmsg_ = GetUVError(iret);
			//LOGE(errmsg_);
			return false;
		}
		iret = uv_tcp_bind(&server_, (const struct sockaddr*)&bind_addr, 0);
		if (iret) {
			errmsg_ = GetUVError(iret);
			//LOGE(errmsg_);
			return false;
		}
		//LOGI("server bind ip=" << ip << ", port=" << port);
		return true;
	}

	bool TCPServer::bind6(const char* ip, int port)
	{
		struct sockaddr_in6 bind_addr;
		int iret = uv_ip6_addr(ip, port, &bind_addr);
		if (iret) {
			errmsg_ = GetUVError(iret);
			//LOGE(errmsg_);
			return false;
		}
		iret = uv_tcp_bind(&server_, (const struct sockaddr*)&bind_addr, 0);
		if (iret) {
			errmsg_ = GetUVError(iret);
			//LOGE(errmsg_);
			return false;
		}
		//LOGI("server bind ip=" << ip << ", port=" << port);
		return true;
	}

	bool TCPServer::listen(int backlog)
	{
		int iret = uv_listen((uv_stream_t*)&server_, backlog, acceptConnection);
		if (iret) {
			errmsg_ = GetUVError(iret);
			//LOGE(errmsg_);
			return false;
		}
		//LOGI("server listen");
		return true;
	}

	bool TCPServer::Start(const char *ip, int port)
	{
		close();
		if (!init()) {
			return false;
		}
		if (!bind(ip, port)) {
			return false;
		}
		if (!listen(SOMAXCONN)) {
			return false;
		}
		if (!run()) {
			return false;
		}
		//LOGI("start listen " << ip << ": " << port);
		return true;
	}

	bool TCPServer::Start6(const char *ip, int port)
	{
		close();
		if (!init()) {
			return false;
		}
		if (!bind6(ip, port)) {
			return false;
		}
		if (!listen(SOMAXCONN)) {
			return false;
		}
		if (!run()) {
			return false;
		}
		return true;
	}

	//���������ͺ���
	int TCPServer::send(int clientid, const char* data, std::size_t len)
	{
		auto itfind = clients_list_.find(clientid);
		if (itfind == clients_list_.end()) {
			errmsg_ = "can't find cliendid ";
			errmsg_ += std::to_string((long long)clientid);
			//LOGE(errmsg_);
			return -1;
		}
		//�Լ�����data����������ֱ��write����
		if (itfind->second->writebuffer.len < len) {
			itfind->second->writebuffer.base = (char*)realloc(itfind->second->writebuffer.base, len);
			itfind->second->writebuffer.len = len;
		}
		memcpy(itfind->second->writebuffer.base, data, len);
		uv_buf_t buf = uv_buf_init((char*)itfind->second->writebuffer.base, len);
		int iret = uv_write(&itfind->second->write_req, (uv_stream_t*)itfind->second->client_handle, &buf, 1, AfterSend);
		if (iret) {
			errmsg_ = GetUVError(iret);
			//LOGE(errmsg_);
			return false;
		}
		return true;
	}

	//������-�¿ͻ��˺���
	void TCPServer::acceptConnection(uv_stream_t *server, int status)
	{
		if (!server->data) {
			return;
		}
		TCPServer *tcpsock = (TCPServer *)server->data;
		int clientid = tcpsock->GetAvailaClientID();
		clientdata* cdata = new clientdata(clientid);//uv_close�ص��������ͷ�
		cdata->tcp_server = tcpsock;//�������������Ϣ
		int iret = uv_tcp_init(tcpsock->loop_, cdata->client_handle);//���������ͷ�
		if (iret) {
			delete cdata;
			tcpsock->errmsg_ = GetUVError(iret);
			//LOGE(tcpsock->errmsg_);
			return;
		}
		iret = uv_accept((uv_stream_t*)&tcpsock->server_, (uv_stream_t*)cdata->client_handle);
		if (iret) {
			tcpsock->errmsg_ = GetUVError(iret);
			uv_close((uv_handle_t*)cdata->client_handle, NULL);
			delete cdata;
			//LOGE(tcpsock->errmsg_);
			return;
		}
		tcpsock->clients_list_.insert(std::make_pair(clientid, cdata));//���뵽���Ӷ���
		if (tcpsock->newconcb_) {
			tcpsock->newconcb_(clientid);
		}
		//LOGI("new client(" << cdata->client_handle << ") id=" << clientid);
		iret = uv_read_start((uv_stream_t*)cdata->client_handle, onAllocBuffer, AfterServerRecv);//��������ʼ���տͻ��˵�����
		return;
	}

	//������-�������ݻص�����
	void TCPServer::setrecvcb(int clientid, server_recvcb cb)
	{
		auto itfind = clients_list_.find(clientid);
		if (itfind != clients_list_.end()) {
			itfind->second->recvcb_ = cb;
		}
	}

	//������-�����ӻص�����
	void TCPServer::setnewconnectcb(newconnect cb)
	{
		newconcb_ = cb;
	}

	//�����������ռ亯��
	void TCPServer::onAllocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
	{
		if (!handle->data) {
			return;
		}
		clientdata *client = (clientdata*)handle->data;
		*buf = client->readbuffer;
	}

	void TCPServer::AfterServerRecv(uv_stream_t *handle, ssize_t nread, const uv_buf_t* buf)
	{
		if (!handle->data) {
			return;
		}
		clientdata *client = (clientdata*)handle->data;//��������recv������clientdata
		if (nread < 0) {/* Error or EOF */
			TCPServer *server = (TCPServer *)client->tcp_server;
			if (nread == UV_EOF) {
				fprintf(stdout, "�ͻ���(%d)���ӶϿ����رմ˿ͻ���\n", client->client_id);
				//LOGW("�ͻ���(" << client->client_id << ")�����Ͽ�");
			}
			else if (nread == UV_ECONNRESET) {
				fprintf(stdout, "�ͻ���(%d)�쳣�Ͽ�\n", client->client_id);
				//LOGW("�ͻ���(" << client->client_id << ")�쳣�Ͽ�");
			}
			else {
				fprintf(stdout, "%s\n", GetUVError(nread));
				//LOGW("�ͻ���(" << client->client_id << ")�쳣�Ͽ���" << GetUVError(nread));
			}
			server->DeleteClient(client->client_id);//���ӶϿ����رտͻ���
			return;
		}
		else if (0 == nread)  {/* Everything OK, but nothing read. */

		}
		else if (client->recvcb_) {
			client->recvcb_(client->client_id, buf->base, nread);
		}
	}

	//��������ͻ���һ��
	void TCPServer::AfterSend(uv_write_t *req, int status)
	{
		if (status < 0) {
			//LOGE("������������:" << GetUVError(status));
			fprintf(stderr, "Write error %s\n", GetUVError(status));
		}
	}

	void TCPServer::AfterServerClose(uv_handle_t *handle)
	{
		//������,����Ҫ��ʲô
	}

	void TCPServer::AfterClientClose(uv_handle_t *handle)
	{
		clientdata *cdata = (clientdata*)handle->data;
		//LOGI("client " << cdata->client_id << " had closed.");
		delete cdata;
	}

	int TCPServer::GetAvailaClientID() const
	{
		static int s_id = 0;
		return ++s_id;
	}

	bool TCPServer::DeleteClient(int clientid)
	{
		uv_mutex_lock(&mutex_handle_);
		auto itfind = clients_list_.find(clientid);
		if (itfind == clients_list_.end()) {
			errmsg_ = "can't find client ";
			errmsg_ += std::to_string((long long)clientid);
			//LOGE(errmsg_);
			uv_mutex_unlock(&mutex_handle_);
			return false;
		}
		if (uv_is_active((uv_handle_t*)itfind->second->client_handle)) {
			uv_read_stop((uv_stream_t*)itfind->second->client_handle);
		}
		uv_close((uv_handle_t*)itfind->second->client_handle, AfterClientClose);

		clients_list_.erase(itfind);
		//LOGI("ɾ���ͻ���" << clientid);
		uv_mutex_unlock(&mutex_handle_);
		return true;
	}


	void TCPServer::StartLog(const char* logpath /*= nullptr*/)
	{
		// 		zsummer::log4z::ILog4zManager::GetInstance()->SetLoggerMonthdir(LOG4Z_MAIN_LOGGER_ID, true);
		// 		zsummer::log4z::ILog4zManager::GetInstance()->SetLoggerDisplay(LOG4Z_MAIN_LOGGER_ID, false);
		// 		zsummer::log4z::ILog4zManager::GetInstance()->SetLoggerLevel(LOG4Z_MAIN_LOGGER_ID, LOG_LEVEL_DEBUG);
		// 		zsummer::log4z::ILog4zManager::GetInstance()->SetLoggerLimitSize(LOG4Z_MAIN_LOGGER_ID, 100);
		// 		if (logpath) {
		// 			zsummer::log4z::ILog4zManager::GetInstance()->SetLoggerPath(LOG4Z_MAIN_LOGGER_ID, logpath);
		// 		}
		// 		zsummer::log4z::ILog4zManager::GetInstance()->Start();
	}


	/*****************************************TCP Client*************************************************************/
	TCPClient::TCPClient(uv_loop_t* loop)
		:recvcb_(nullptr), userdata_(nullptr)
		, connectstatus_(CONNECT_DIS)
		, isinit_(false)
	{
		readbuffer_ = uv_buf_init((char*)malloc(BUFFERSIZE), BUFFERSIZE);
		writebuffer_ = uv_buf_init((char*)malloc(BUFFERSIZE), BUFFERSIZE);
		loop_ = loop;
		connect_req_.data = this;
		write_req_.data = this;
	}


	TCPClient::~TCPClient()
	{
		free(readbuffer_.base);
		readbuffer_.base = nullptr;
		readbuffer_.len = 0;
		free(writebuffer_.base);
		writebuffer_.base = nullptr;
		writebuffer_.len = 0;
		close();
		//LOGI("�ͻ���(" << this << ")�˳�");
	}
	//��ʼ����ر�--��������ͻ���һ��
	bool TCPClient::init()
	{
		if (isinit_) {
			return true;
		}

		if (!loop_) {
			errmsg_ = "loop is null on tcp init.";
			//LOGE(errmsg_);
			return false;
		}
		int iret = uv_tcp_init(loop_, &client_);
		if (iret) {
			errmsg_ = GetUVError(iret);
			//LOGE(errmsg_);
			return false;
		}
		iret = uv_mutex_init(&write_mutex_handle_);
		if (iret) {
			errmsg_ = GetUVError(iret);
			//LOGE(errmsg_);
			return false;
		}
		isinit_ = true;
		fprintf(stdout, "�ͻ���(%p) init type = %d\n", &client_, client_.type);
		client_.data = this;
		//iret = uv_tcp_keepalive(&client_, 1, 60);//
		//if (iret) {
		//    errmsg_ = GetUVError(iret);
		//    return false;
		//}
		//LOGI("�ͻ���(" << this << ")Init");
		return true;
	}

	void TCPClient::close()
	{
		if (!isinit_) {
			return;
		}
		uv_mutex_destroy(&write_mutex_handle_);
		uv_close((uv_handle_t*)&client_, AfterClose);
		//LOGI("�ͻ���(" << this << ")close");
		isinit_ = false;
	}

	bool TCPClient::run(int status)
	{
		//LOGI("�ͻ���(" << this << ")run");
		int iret = uv_run(loop_, (uv_run_mode)status);
		if (iret) {
			errmsg_ = GetUVError(iret);
			//LOGE(errmsg_);
			return false;
		}
		return true;
	}

	//��������--��������ͻ���һ��
	bool TCPClient::setNoDelay(bool enable)
	{
		//http://blog.csdn.net/u011133100/article/details/21485983
		int iret = uv_tcp_nodelay(&client_, enable ? 1 : 0);
		if (iret) {
			errmsg_ = GetUVError(iret);
			//LOGE(errmsg_);
			return false;
		}
		return true;
	}

	bool TCPClient::setKeepAlive(int enable, unsigned int delay)
	{
		int iret = uv_tcp_keepalive(&client_, enable, delay);
		if (iret) {
			errmsg_ = GetUVError(iret);
			//LOGE(errmsg_);
			return false;
		}
		return true;
	}

	//��Ϊclient��connect����
	bool TCPClient::connect(const char* ip, int port)
	{
		close();
		init();
		connectip_ = ip;
		connectport_ = port;
		//LOGI("�ͻ���(" << this << ")start connect to server(" << ip << ":" << port << ")");
		int iret = uv_thread_create(&connect_threadhanlde_, ConnectThread, this);//����AfterConnect�����������ӳɹ����������߳�
		if (iret) {
			errmsg_ = GetUVError(iret);
			//LOGE(errmsg_);
			return false;
		}
		while (connectstatus_ == CONNECT_DIS) {
#if defined (WIN32) || defined(_WIN32)
			Sleep(100);
#else
			usleep((100) * 1000)
#endif
		}
		return connectstatus_ == CONNECT_FINISH;
	}

	bool TCPClient::connect6(const char* ip, int port)
	{
		close();
		init();
		connectip_ = ip;
		connectport_ = port;
		//LOGI("�ͻ���(" << this << ")start connect to server(" << ip << ":" << port << ")");
		int iret = uv_thread_create(&connect_threadhanlde_, ConnectThread6, this);//����AfterConnect�����������ӳɹ����������߳�
		if (iret) {
			errmsg_ = GetUVError(iret);
			//LOGE(errmsg_);
			return false;
		}
		while (connectstatus_ == CONNECT_DIS) {
			//fprintf(stdout,"client(%p) wait, connect status %d\n",this,connectstatus_);
#if defined (WIN32) || defined(_WIN32)
			Sleep(100);
#else
			usleep((100) * 1000)
#endif
		}
		return connectstatus_ == CONNECT_FINISH;
	}

	void TCPClient::ConnectThread(void* arg)
	{
		TCPClient *pclient = (TCPClient*)arg;
		fprintf(stdout, "client(%p) ConnectThread start\n", pclient);
		struct sockaddr_in bind_addr;
		int iret = uv_ip4_addr(pclient->connectip_.c_str(), pclient->connectport_, &bind_addr);
		if (iret) {
			pclient->errmsg_ = GetUVError(iret);
			//LOGE(pclient->errmsg_);
			return;
		}
		iret = uv_tcp_connect(&pclient->connect_req_, &pclient->client_, (const sockaddr*)&bind_addr, AfterConnect);
		if (iret) {
			pclient->errmsg_ = GetUVError(iret);
			//LOGE(pclient->errmsg_);
			return;
		}
		fprintf(stdout, "client(%p) ConnectThread end, connect status %d\n", pclient, pclient->connectstatus_);
		pclient->run();
	}


	void TCPClient::ConnectThread6(void* arg)
	{
		TCPClient *pclient = (TCPClient*)arg;
		//LOGI("�ͻ���(" << pclient << ")Enter Connect Thread.");
		fprintf(stdout, "client(%p) ConnectThread start\n", pclient);
		struct sockaddr_in6 bind_addr;
		int iret = uv_ip6_addr(pclient->connectip_.c_str(), pclient->connectport_, &bind_addr);
		if (iret) {
			pclient->errmsg_ = GetUVError(iret);
			//LOGE(pclient->errmsg_);
			return;
		}
		iret = uv_tcp_connect(&pclient->connect_req_, &pclient->client_, (const sockaddr*)&bind_addr, AfterConnect);
		if (iret) {
			pclient->errmsg_ = GetUVError(iret);
			//LOGE(pclient->errmsg_);
			return;
		}
		fprintf(stdout, "client(%p) ConnectThread end, connect status %d\n", pclient, pclient->connectstatus_);
		//LOGI("�ͻ���(" << pclient << ")Leave Connect Thread. connect status " << pclient->connectstatus_);
		pclient->run();
	}

	void TCPClient::AfterConnect(uv_connect_t* handle, int status)
	{
		fprintf(stdout, "start after connect\n");
		TCPClient *pclient = (TCPClient*)handle->handle->data;
		if (status) {
			pclient->connectstatus_ = CONNECT_ERROR;
			fprintf(stdout, "connect error:%s\n", GetUVError(status));
			return;
		}

		int iret = uv_read_start(handle->handle, onAllocBuffer, AfterClientRecv);//�ͻ��˿�ʼ���շ�����������
		if (iret) {
			fprintf(stdout, "uv_read_start error:%s\n", GetUVError(iret));
			pclient->connectstatus_ = CONNECT_ERROR;
		}
		else {
			pclient->connectstatus_ = CONNECT_FINISH;
		}
		//LOGI("�ͻ���(" << pclient << ")run");
		fprintf(stdout, "end after connect\n");
	}

	//�ͻ��˵ķ��ͺ���
	int TCPClient::send(const char* data, std::size_t len)
	{
		//�Լ�����data����������ֱ��write����
		if (!data || len <= 0) {
			errmsg_ = "send data is null or len less than zero.";
			return 0;
		}

		uv_mutex_lock(&write_mutex_handle_);
		if (writebuffer_.len < len) {
			writebuffer_.base = (char*)realloc(writebuffer_.base, len);
			writebuffer_.len = len;
		}
		memcpy(writebuffer_.base, data, len);
		uv_buf_t buf = uv_buf_init((char*)writebuffer_.base, len);
		int iret = uv_write(&write_req_, (uv_stream_t*)&client_, &buf, 1, AfterSend);
		if (iret) {
			uv_mutex_unlock(&write_mutex_handle_);
			errmsg_ = GetUVError(iret);
			//LOGE(errmsg_);
			return false;
		}
		return true;
	}

	//�ͻ���-�������ݻص�����
	void TCPClient::setrecvcb(client_recvcb cb, void* userdata)
	{
		recvcb_ = cb;
		userdata_ = userdata;
	}

	//�ͻ��˷����ռ亯��
	void TCPClient::onAllocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
	{
		if (!handle->data) {
			return;
		}
		TCPClient *client = (TCPClient*)handle->data;
		*buf = client->readbuffer_;
	}


	void TCPClient::AfterClientRecv(uv_stream_t *handle, ssize_t nread, const uv_buf_t* buf)
	{
		if (!handle->data) {
			return;
		}
		TCPClient *client = (TCPClient*)handle->data;//��������recv������TCPClient
		if (nread < 0) {
			if (nread == UV_EOF) {
				fprintf(stdout, "������(%p)�����Ͽ�\n", handle);
				//LOGW("�����������Ͽ�");
			}
			else if (nread == UV_ECONNRESET) {
				fprintf(stdout, "������(%p)�쳣�Ͽ�\n", handle);
				//LOGW("�������쳣�Ͽ�");
			}
			else {
				fprintf(stdout, "������(%p)�쳣�Ͽ�:%s\n", handle, GetUVError(nread));
				//LOGW("�������쳣�Ͽ�" << GetUVError(nread));
			}
			uv_close((uv_handle_t*)handle, AfterClose);
			return;
		}
		if (nread > 0 && client->recvcb_) {
			client->recvcb_(buf->base, nread, client->userdata_);
		}
	}

	//��������ͻ���һ��
	void TCPClient::AfterSend(uv_write_t *req, int status)
	{
		TCPClient *client = (TCPClient *)req->handle->data;
		uv_mutex_unlock(&client->write_mutex_handle_);
		if (status < 0) {
			//LOGE("������������:" << GetUVError(status));
			fprintf(stderr, "Write error %s\n", GetUVError(status));
		}
	}
	//��������ͻ���һ��
	void TCPClient::AfterClose(uv_handle_t *handle)
	{
		fprintf(stdout, "�ͻ���(%p)��close\n", handle);
		//LOGI("�ͻ���(" << handle << ")��close");
	}

	void TCPClient::StartLog(const char* logpath /*= nullptr*/)
	{
		// 		zsummer::log4z::ILog4zManager::GetInstance()->SetLoggerMonthdir(LOG4Z_MAIN_LOGGER_ID, true);
		// 		zsummer::log4z::ILog4zManager::GetInstance()->SetLoggerDisplay(LOG4Z_MAIN_LOGGER_ID, false);
		// 		zsummer::log4z::ILog4zManager::GetInstance()->SetLoggerLevel(LOG4Z_MAIN_LOGGER_ID, LOG_LEVEL_DEBUG);
		// 		zsummer::log4z::ILog4zManager::GetInstance()->SetLoggerLimitSize(LOG4Z_MAIN_LOGGER_ID, 100);
		// 		if (logpath) {
		// 			zsummer::log4z::ILog4zManager::GetInstance()->SetLoggerPath(LOG4Z_MAIN_LOGGER_ID, logpath);
		// 		}
		// 		zsummer::log4z::ILog4zManager::GetInstance()->Start();
	}

}


// #include "stdio.h"
// #include "stdlib.h"
// #include "uv.h"
// 
// const int backlog = 128;
// const int buffer_size = 1024;
// 
// uv_fs_t opern_req;
// uv_fs_t read_req;
// uv_tcp_t *client;
// 
// void on_new_connection(uv_stream_s *server, int status);
// uv_buf_t alloc_buffer(uv_handle_t *handle, size_t suggested_size);
// void on_client_read(uv_stream_t *client, ssize_t nread, uv_buf_t buf);
// void on_client_write(uv_write_t *req, int status);
// void on_file_open(uv_fs_t *req);
// void on_file_read(uv_fs_t *req);
// 
// void on_new_connection(uv_stream_s *server, int status)
// {
// 	if (status !=0)
// 	{
// 		uv_close((uv_handle_t*)client, NULL);
// 		return;
// 	}
// 
// 	client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
// 	uv_tcp_init(uv_default_loop(), client);
// 
// 	int result = uv_accept(server, (uv_stream_t*)client);
// 	if (result ==0)
// 	{
// 		uv_read_start((uv_stream_t*)client, alloc_buffer, on_client_read);
// 	}
// 	else
// 	{
// 		uv_close((uv_handle_t*)client, NULL);
// 	}
// }
// 
// uv_buf_t alloc_buffer(uv_handle_t *handle, size_t suggested_size)
// {
// 	return uv_buf_init((char*)malloc(suggested_size), suggested_size);
// }
// 
// void on_client_read(uv_stream_t *client, ssize_t nread, uv_buf_t buf)
// {
// 	if (nread < 0)
// 	{
// 		uv_close((uv_handle_t*)client, NULL);
// 	}
// 
// 	char *filename = buf.base;
// 	int mode = 0;
// 
// 	uv_fs_open(uv_default_loop(), &opern_req, filename, O_RDONLY, mode, on_file_open);
// }
// 
// void on_client_write(uv_write_t *req, int status)
// {
// 	if (status == -1)
// 	{
// 		uv_close((uv_handle_t*)client, NULL);
// 		return;
// 	}
// 
// 	free(req);
// 	char *buffer = (char*)req->data;
// 	free(buffer);
// 
// 	uv_close((uv_handle_t*)client, NULL);
// }