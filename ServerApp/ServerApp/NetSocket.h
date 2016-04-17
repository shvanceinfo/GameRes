// #ifndef __NET_SOCKET_H__
// #define __NET_SOCKET_H__
// 
// #include "uv.h"
// #include "iostream"
// #include "list"
// #include "map"
// 
// #define BUFFERSIZE (1024 * 1024)
// 
// using std::string;
// namespace LIBUV
// {
// 	typedef void(*newconnect)(uint32_t clientID);
// 	typedef void(*server_recvcb)(uint32_t clientID, const char* buf, int bufsize);
// 	typedef void(*client_recvcb)(const char* buf, int bufsize, void* userData);
// 
// 	class TCPServer;
// 	class ClientData
// 	{
// 	public:
// 		ClientData(uint32_t client_id) : clientID(client_id), recvdb(nullptr)
// 		{
// 			client_handle = (uv_tcp_t *)malloc(sizeof(*client_handle));
// 			client_handle->data = this;
// 			readbuffer = uv_buf_init((char*)malloc(BUFFERSIZE), BUFFERSIZE);
// 			writebuffer = uv_buf_init((char*)malloc(BUFFERSIZE), BUFFERSIZE);
// 		}
// 
// 		~ClientData()
// 		{
// 			free(readbuffer.base);
// 			readbuffer.base = nullptr;
// 			readbuffer.len = 0;
// 
// 			free(writebuffer.base);
// 			writebuffer.base = nullptr;
// 			writebuffer.len = 0;
// 
// 			free(client_handle);
// 			client_handle = nullptr;
// 		}
// 
// 		uint32_t clientID;
// 		uv_tcp_t *client_handle;
// 		TCPServer *tcp_server;
// 		uv_buf_t readbuffer;
// 		uv_buf_t writebuffer;
// 		uv_write_t write_req;
// 		server_recvcb recvdb;
// 	};
// 
// 	class TCPServer
// 	{
// 	public:
// 		TCPServer(uv_loop_t* loop = uv_default_loop());
// 		~TCPServer();
// 		
// 	public:
// 		bool Start(const char* ip, int port);
// 		void close();
// 
// 		bool setNoDelay(bool enable);
// 		bool setKeepAlive(int enable, uint32_t delay);
// 		
// 
// 		int send(int client_id, const char* data, std::size_t len);
// 		void setnewconnectcb(newconnect cb);
// 		void setrecvcb(uint32_t client_id, server_recvcb cb);
// 
// 	protected:
// 		int GetAvailaClientID() const;
// 		bool DeleteClient(int client_id);
// 
// 		static void AfterServerRecv(uv_stream_t* client, ssize_t nred, const uv_buf_t* buf);
// 		static void AfterSend(uv_write_t* req, int status);
// 		static void onAllocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t* buf);
// 		static void AfterServerClose(uv_handle_t* handle);
// 		static void AfterClientClose(uv_handle_t* handle);
// 		static void acceptConnection(uv_handle_t* server, int status);
// 
// 	private:
// 		bool init();
// 		bool run(int status = UV_RUN_DEFAULT);
// 		bool bind(const char* ip, int port);
// 		bool listen(int backlog = 1024);
// 
// 		uv_tcp_t server;
// 		std::map<int, ClientData*> clients_list;
// 		uv_loop_t *loop;
// 		newconnect newconcb;
// 		bool isinit;
// 	};
// 
// 	class TCPClient
// 	{
// 	private:
// 		enum {
// 			CONNECT_TIMEOUT,
// 			CONNECT_FINISH,
// 			CONNECT_ERROR,
// 			CONNECT_DIS,
// 		};
// 
// 		uv_tcp_t client;
// 		uv_loop_t *loop;
// 		uv_write_t write_req;
// 		uv_connect_t connect_req;
// 		uv_thread_t connect_threadhandle;
// 		uv_buf_t readbuffer;
// 		uv_buf_t writebuffer;
// 		uv_mutex_t write_mutex_handle;
// 
// 		int connectstatus;
// 		client_recvcb recvcb;
// 		void *userdata;
// 		std::string connectip;
// 		int connectport;
// 		bool isinit;
// 	};
// }
// 
// 
// #endif



/////////////////////////
/***************************************
* @file     tcpsocket.h
* @brief    ����libuv��װ��tcp��������ͻ���,ʹ��log4z����־����
* @details
* @author   phata, wqvbjhc@gmail.com
* @date     2014-5-13
* @mod      2014-5-13  phata  ������������ͻ��˵Ĵ���.�ַ�����֧�ֶ�ͻ�������
�޸Ŀͻ��˲��Դ��룬֧�ֲ�����ͻ��˲���
****************************************/
#ifndef TCPSocket_H
#define TCPSocket_H
#include "uv.h"
#include <string>
#include <list>
#include <map>
#define BUFFERSIZE (1024*1024)

namespace uv
{
	typedef void(*newconnect)(int clientid);
	typedef void(*server_recvcb)(int cliendid, const char* buf, int bufsize);
	typedef void(*client_recvcb)(const char* buf, int bufsize, void* userdata);

	class TCPServer;
	class clientdata
	{
	public:
		clientdata(int clientid) :client_id(clientid), recvcb_(nullptr) {
			client_handle = (uv_tcp_t*)malloc(sizeof(*client_handle));
			client_handle->data = this;
			readbuffer = uv_buf_init((char*)malloc(BUFFERSIZE), BUFFERSIZE);
			writebuffer = uv_buf_init((char*)malloc(BUFFERSIZE), BUFFERSIZE);
		}
		virtual ~clientdata() {
			free(readbuffer.base);
			readbuffer.base = nullptr;
			readbuffer.len = 0;

			free(writebuffer.base);
			writebuffer.base = nullptr;
			writebuffer.len = 0;

			free(client_handle);
			client_handle = nullptr;
		}
		int client_id;//�ͻ���id,Ωһ
		uv_tcp_t* client_handle;//�ͻ��˾��
		TCPServer* tcp_server;//���������(��������ΪĳЩ�ص�������Ҫ��)
		uv_buf_t readbuffer;//�������ݵ�buf
		uv_buf_t writebuffer;//д���ݵ�buf
		uv_write_t write_req;
		server_recvcb recvcb_;//�������ݻص����û��ĺ���
	};


	class TCPServer
	{
	public:
		TCPServer(uv_loop_t* loop = uv_default_loop());
		virtual ~TCPServer();
		static void StartLog(const char* logpath = nullptr);//������־�����������Ż�������־
	public:
		//��������
		bool Start(const char *ip, int port);//����������,��ַΪIP4
		bool Start6(const char *ip, int port);//��������������ַΪIP6
		void close();

		bool setNoDelay(bool enable);
		bool setKeepAlive(int enable, unsigned int delay);

		const char* GetLastErrMsg() const {
			return errmsg_.c_str();
		};

		virtual int  send(int clientid, const char* data, std::size_t len);
		virtual void setnewconnectcb(newconnect cb);
		virtual void setrecvcb(int clientid, server_recvcb cb);//���ý��ջص�������ÿ���ͻ��˸���һ��
	protected:
		int GetAvailaClientID()const;//��ȡ���õ�client id
		bool DeleteClient(int clientid);//ɾ�������еĿͻ���
		//��̬�ص�����
		static void AfterServerRecv(uv_stream_t *client, ssize_t nread, const uv_buf_t* buf);
		static void AfterSend(uv_write_t *req, int status);
		static void onAllocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
		static void AfterServerClose(uv_handle_t *handle);
		static void AfterClientClose(uv_handle_t *handle);
		static void acceptConnection(uv_stream_t *server, int status);

	private:
		bool init();
		bool run(int status = UV_RUN_DEFAULT);
		bool bind(const char* ip, int port);
		bool bind6(const char* ip, int port);
		bool listen(int backlog = 1024);


		uv_tcp_t server_;//����������
		std::map<int, clientdata*> clients_list_;//�ӿͻ�������
		uv_mutex_t mutex_handle_;//����clients_list_
		uv_loop_t *loop_;
		std::string errmsg_;
		newconnect newconcb_;
		bool isinit_;//�Ƿ��ѳ�ʼ��������close�������ж�
	};



	class TCPClient
	{
		//ֱ�ӵ���connect/connect6���������
	public:
		TCPClient(uv_loop_t* loop = uv_default_loop());
		virtual ~TCPClient();
		static void StartLog(const char* logpath = nullptr);//������־�����������Ż�������־
	public:
		//��������
		virtual bool connect(const char* ip, int port);//����connect�̣߳�ѭ���ȴ�ֱ��connect���
		virtual bool connect6(const char* ip, int port);//����connect�̣߳�ѭ���ȴ�ֱ��connect���
		virtual int  send(const char* data, std::size_t len);
		virtual void setrecvcb(client_recvcb cb, void* userdata);////���ý��ջص�������ֻ��һ��
		void close();

		//�Ƿ�����Nagle�㷨
		bool setNoDelay(bool enable);
		bool setKeepAlive(int enable, unsigned int delay);

		const char* GetLastErrMsg() const {
			return errmsg_.c_str();
		};
	protected:
		//��̬�ص�����
		static void AfterConnect(uv_connect_t* handle, int status);
		static void AfterClientRecv(uv_stream_t *client, ssize_t nread, const uv_buf_t* buf);
		static void AfterSend(uv_write_t *req, int status);
		static void onAllocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
		static void AfterClose(uv_handle_t *handle);

		static void ConnectThread(void* arg);//������connect�߳�
		static void ConnectThread6(void* arg);//������connect�߳�

		bool init();
		bool run(int status = UV_RUN_DEFAULT);
	private:
		enum {
			CONNECT_TIMEOUT,
			CONNECT_FINISH,
			CONNECT_ERROR,
			CONNECT_DIS,
		};
		uv_tcp_t client_;//�ͻ�������
		uv_loop_t *loop_;
		uv_write_t write_req_;//дʱ����
		uv_connect_t connect_req_;//����ʱ����
		uv_thread_t connect_threadhanlde_;//�߳̾��
		std::string errmsg_;//������Ϣ
		uv_buf_t readbuffer_;//�������ݵ�buf
		uv_buf_t writebuffer_;//д���ݵ�buf
		uv_mutex_t write_mutex_handle_;//����write,����ǰһwrite��ɲŽ�����һwrite

		int connectstatus_;//����״̬
		client_recvcb recvcb_;//�ص�����
		void* userdata_;//�ص��������û�����
		std::string connectip_;//���ӵķ�����IP
		int connectport_;//���ӵķ������˿ں�
		bool isinit_;//�Ƿ��ѳ�ʼ��������close�������ж�
	};

}


#endif // TCPSocket_H