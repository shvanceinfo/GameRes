#ifndef __MY_SQL_CONNECTION_H__
#define __MY_SQL_CONNECTION_H__

#include <iostream>
#include <mysql.h>
using namespace std;

class MySqlConnection
{
public:
	bool init(std::string ip, uint16_t port, std::string account, std::string pass, std::string name)
	{
		MYSQL* mysqlInit;

		if (NULL == (mysqlInit = mysql_init(NULL)))
		{
			return false;
		}

		if (NULL != mysql_options(mysqlInit, MYSQL_SET_CHARSET_NAME, "UTF8"))
		{
			return false;
		}

		if (NULL == (mysql_ = mysql_real_connect(mysqlInit, ip.c_str(), account.c_str(), pass.c_str(), name.c_str(), port, NULL, 0)))
		{
			return false;
		}

		return true;
	}

	bool Query(std::string&& sql)
	{
		if (mysql_ == NULL)
		{
			return false;
		}

		MYSQL_RES *result = NULL;

		if (0 == mysql_query(mysql_, sql.c_str()))
		{
			result = mysql_store_result(mysql_);
		}
		else
		{
			return false;
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

		//T->Parse(feild);

		mysql_free_result(result);

		return true;
	};
private:
	MYSQL* mysql_ = nullptr;
};

#endif