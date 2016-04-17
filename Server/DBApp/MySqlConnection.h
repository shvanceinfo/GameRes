#ifndef __MY_SQL_CONNECTION_H__
#define __MY_SQL_CONNECTION_H__

#include <iostream>
#include <mysql.h>

template<typename T>
class MySqlConnection
{
public:
	MySqlConnection(std::string ip, uint16_t port, std::string account, std::string pass, std::string name) : 
		ip_(ip), 
		port_(port), 
		account_(account), 
		pass_(pass),
		name_(name), 
		isConnect_(false)
	{
	}

	void init()
	{
		MYSQL* mysqlInit;

		if (NULL == (mysqlInit = mysql_init(NULL)))
		{
			return ;
		}

		if (NULL != mysql_options(mysqlInit, MYSQL_SET_CHARSET_NAME, "UTF8"))
		{
			return ;
		}

		if (NULL == (mysql_ = mysql_real_connect(mysqlInit, ip_.c_str(), account_.c_str(), pass_.c_str(), name_.c_str(), port_, NULL, 0)))
		{
			return ;
		}

		isConnect_ = true;
	}

	void Query( T &value)
	{
		if ( mysql_ == NULL || isConnect_ == false )
		{
			return;
		}

		MYSQL_RES *result = NULL;

		if (0 == mysql_query(mysql_, value.c_str()))
		{
			result = mysql_store_result(mysql_);
		}
		else
		{
			return ;
		}

		unsigned long long rowcount = mysql_num_rows(result);
		cout << rowcount << endl;
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
	};
private:
	MYSQL* mysql_;
	std::string ip_;
	uint16_t port_;
	std::string account_;
	std::string pass_;
	std::string name_;
	int isConnect_;
};

#endif