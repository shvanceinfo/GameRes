// DBApp.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "iostream"
#include "mysql.h"

using namespace std;

#define INSERT_QUERY ""
#define STRING_SIZE 50
#define SELECT_SAMPLE "select col1,col2,col3,col4 from test_table"

MYSQL_STMT *stmt;
MYSQL_BIND _bind[4];
MYSQL_RES *prepare_meta_result;
MYSQL_TIME ts;
unsigned long length[4];
int param_count, column_count, row_count;
short small_data;
int int_data;
char str_data[STRING_SIZE];
my_bool is_null[4];
my_bool error[4];

#include "DatabaseThreadPool.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

int _tmain(int argc, _TCHAR* argv[])
{
	{
		DatabaseThreadPool<std::string> pool;
		pool.CreateThreadPool(10, "127.0.0.1", 3306, "root", "root", "testdatabase");

		std::string temp = "select * from student";
		for (;;)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			pool.post(temp);
		}

		pool.wait();
		getchar();
		return 0;
	}
	MYSQL mydata;

	if (0 != mysql_library_init(0,NULL,NULL))
	{
		return 0;
	}

	if (NULL == mysql_init(&mydata))
	{
		return 0;
	}
	

 	if (0 != mysql_options(&mydata,MYSQL_SET_CHARSET_NAME,"UTF8"))
 	{
 		return 0;
 	}

	if (NULL == mysql_real_connect(&mydata,"127.0.0.1", "root", "root", "testdatabase", 3306, NULL,0))
	{
		return 0;
	}

	if (0 != mysql_set_character_set(&mydata, "utf8"))
	{
		return 0;
	}

	string strsql;
	MYSQL_RES *result = NULL;

	strsql = "insert into student(id, state, nickname) values(6,1,'aaaa')";
	int ret = mysql_query(&mydata, strsql.c_str());
	if (0 != ret)
	{
		cout << mysql_error(&mydata)<<endl;
	}
	

	strsql = "select * from student";
	if (0 == mysql_query(&mydata, strsql.c_str()))
	{
		result = mysql_store_result(&mydata);
	}
	else
	{
		return 0;
	}


	unsigned long long rowcount = mysql_num_rows(result);
	//cout << rowcount << endl;
	int feildcount = mysql_num_fields(result);

	MYSQL_FIELD *feild = NULL;
	for (int i = 0; i < feildcount; i++)
	{
		feild = mysql_fetch_field_direct(result, i);
		cout << feild->name << "	";
	}

	cout << endl;

	MYSQL_ROW row = NULL;
	while (NULL != (row = mysql_fetch_row(result)))
	{
		for (int i = 0; i < feildcount; i++)
		{
			cout << row[i] << "		";
		}

		cout << endl;
	}

	mysql_free_result(result);
	mysql_close(&mydata);

	getchar();
	return 0;
}

