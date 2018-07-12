#include <winsock.h>
#include "stdafx.h"
#include <time.h>
#include "db.h"
#include "mysql.h"

#pragma comment(lib,"libmySQL.lib")

using namespace std;

//////////////////////////////////
//全局常量/变量
//////////////////////////////////
const string user_s = "root";
const string pswd_s = "";
const string ip_s = "localhost";
const string dbname_s = "ssgo";

/////////////////////////////////////////////
//执行任意SQL语句
/////////////////////////////////////////////
bool DB::sql(string sql_s)
{
	char sql[400];
	strcpy(sql, sql_s.c_str());
	int rt = mysql_real_query(con, sql, strlen(sql));
	if (rt)
		return false;
	else
		return true;
}

/////////////////////////////////////////////
//查询数据
/////////////////////////////////////////////
//void DB::query() {
//	cout << "Table 1 (people) --------------------------" << endl;
//	this->query("people");
//	cout << "Table 2 (game) ----------------------------" << endl;
//	this->query("actor");
//	cout << "Table 3 (chat) ----------------------------" << endl;
//	this->query("msg");
//}
//
//void DB::query(string tablename) {
//	int rt, count=0;
//	unsigned int t;
//	MYSQL_RES *res;
//	char tmp[400];
//
//	sprintf_s(tmp, "select * from %s", tablename);//此句无法给tmp赋值。只能用下面三行代替，且要关闭sdl检查。
//	/*string tmp_s = "select * from " + tablename;
//	unsigned int length = tmp_s.copy(tmp, 399);
//	tmp[length] = '\0';*/
//
//	rt = mysql_real_query(con, tmp, strlen(tmp));
//	if (rt)
//	{
//		cout << ": mysql_real_query failed! " << mysql_error(con) << endl;
//		exit(0);
//	}
//	else
//	{
//		cout << tmp << ": success." << endl;
//	}
//	res = mysql_store_result(con); //将结果保存在res结构体中
//
//	MYSQL_FIELD *fd;  //字段列数组
//	char field[32][32];  //存字段名二维数组
//	for (int i = 0;fd = mysql_fetch_field(res);i++)  //获取字段名  
//		strcpy(field[i], fd->name);
//	int j = mysql_num_fields(res);  // 获取列数  
//	for (int i = 0;i<j;i++)  //打印字段  
//		cout<< field[i] << "\t";
//	cout << endl;
//	MYSQL_ROW row;
//	while (row = mysql_fetch_row(res))
//	{
//		for (t = 0;t<j;t++)
//			if (row[t] == NULL)
//				cout << "NULL\t";
//			else
//				cout << row[t] << "\t";
//		count++;
//		cout << endl;
//	}
//	cout << "The datas' number is " << count << endl;
//	if (tablename == "msg")
//		msg_id = count + 1;
//	mysql_free_result(res);  //释放结果集
//}

/////////////////////////////////////////////
//连接数据库
/////////////////////////////////////////////
MYSQL * DB::connect()
{
	int rt;
	con = mysql_init((MYSQL*)0);
	if (con != NULL && mysql_real_connect(con, ip, user, passwd, dbname, 3306, NULL, 0))
	{
		if (!mysql_select_db(con, dbname))
		{
			cout << "mysql_select_db success." << endl;
			con->reconnect = 1;
			//rt = mysql_real_query(con, "SET NAMES GBK;", (unsigned int)strlen("SET NAME GBK;"));//否则cmd下中文乱码
			//if (rt)
			//{
			//	cout << "mysql_real_query ZiFuJi can't be set: " << mysql_error(con) << endl;
			//	system("pause");
			//	exit(0);
			//}
			//else
			//{
			//	cout << "mysql_real_query ZiFuJi is set!" << endl;
			//}
		}
	}
	else
	{
		cout << "mysql_select_db failed!" << endl;
		exit(0);
	}
	return con;
}

/////////////////////////////////////////////
//构造/析构
/////////////////////////////////////////////
DB::DB()
{
	strcpy(user, user_s.c_str());
	strcpy(passwd, pswd_s.c_str());
	strcpy(ip, ip_s.c_str());
	strcpy(dbname, dbname_s.c_str());
	record_id = 0;
	game_id = 0;
	connect();
}

DB::~DB()
{
	mysql_close(con); //释放连接
	memset(user, 0, sizeof(user));
	memset(passwd, 0, sizeof(passwd));
	memset(ip, 0, sizeof(ip));
	memset(dbname, 0, sizeof(dbname));
	record_id = 0;
}

////////////////////////////////////////////
//获取密码
////////////////////////////////////////////
string DB::passwdOf(string actor)
{
	MYSQL_RES *res;
	char tmp[400];
	//sprintf_s(tmp, "SELECT `passwd` from people WHERE `name`=\"%s\"",actor);  //报错：字符串中的字符无效。采用下面3行代替
	string tmp_s = "SELECT `passwd` from people WHERE `name`=\"" + actor + "\"";
	unsigned int length = tmp_s.copy(tmp, 399);
	tmp[length] = '\0';

	int rt = mysql_real_query(con, tmp, (unsigned int)strlen(tmp));
	if (rt)
	{
		cout << "Cannot get the passwd of " << actor << "! error: " << mysql_error(con) << endl;
		return "???";
	}
	res = mysql_store_result(con); //将结果保存在res结构体中
	string user;
	MYSQL_ROW row = mysql_fetch_row(res);
	if (row == NULL)
		user = "???";
	else
		user = row[0];
	mysql_free_result(res);  //释放结果集
	return user;
}

/////////////////////////////////////////////
//查询name账号是否已存在
/////////////////////////////////////////////
bool DB::hasPeople(string name) 
{
	bool b;
	string sql = "SELECT * FROM people WHERE name = \"" + name + "\"";
	this->sql(sql);
	MYSQL_RES *res = mysql_store_result(con);
	MYSQL_ROW row = mysql_fetch_row(res);
	if (row == NULL)
		b = false;
	else
		b = true;
	mysql_free_result(res);
	return b;
}

////////////////////////////////////////////
//获取时间
////////////////////////////////////////////
string DB::getTime()
{
	time_t t = time(0);
	char tmp[64];
	strftime(tmp, sizeof(tmp), "%Y-%m-%d %X", localtime(&t));
	return string(tmp);
}

void DB::openAutoCommit()
{
	if (!this->sql("set autocommit=1;"))
		cout << "Open autocommit failed: " << mysql_error(con) << endl;
}

//////////////////////////////////////
//创建账户
//////////////////////////////////////
bool DB::CreAcc(char* name, char* pswd)
{
	MYSQL_STMT *stmt = mysql_stmt_init(con); //创建MYSQL_STMT句柄
	char* sql = "insert into people values(?,?,0);";
	if (mysql_stmt_prepare(stmt, sql, strlen(sql)))
	{
		cout << "mysql_stmt_prepare failed: " << mysql_error(con);
		return false;
	}
	MYSQL_BIND params[2];
	memset(params, 0, sizeof(params));
	params[0].buffer_type = MYSQL_TYPE_STRING;
	params[0].buffer = name;
	params[0].buffer_length = strlen(name);
	params[1].buffer_type = MYSQL_TYPE_STRING;
	params[1].buffer = pswd;
	params[1].buffer_length = strlen(pswd);

	mysql_stmt_bind_param(stmt, params);
	mysql_stmt_bind_result(stmt, params); //用于将结果集中的列与数据缓冲和长度缓冲关联（绑定）起来
	mysql_stmt_execute(stmt);  //执行与语句句柄相关的预处理
	mysql_stmt_store_result(stmt);  //以便后续的mysql_stmt_fetch()调用能返回缓冲数据
	string err = mysql_stmt_error(stmt);
	mysql_stmt_close(stmt);
	if (err == "")  //创建账户成功
		return true;
	else  //创建账户失败
	{
		cout << "Error when create accout: " << err << endl;
		return false;
	}
}

bool DB::CreAcc(string name, string pswd)
{
	string str = "insert into people values(\"" + name + "\",\"" + pswd + "\",0);";
	if (sql(str))
	{
		cout << "Create accout success!" << endl;
		return true;
	}
	else
		return false;
}

//////////////////////////////////////
//创建棋谱记录
//////////////////////////////////////
void DB::AddRecord(int i, int j)
{
	record_id++;
	string sql = "insert into record values(" + to_string(record_id) + "," + to_string(i) + "," + to_string(j) + ",'" + getTime() + "'," + to_string(game_id) +");";
	if (this->sql(sql))
		cout << "AddRecord success!" << endl;
}

//////////////////////////////////////
//创建对局
//////////////////////////////////////
void DB::CreateNewGame(string BName, string WName, int side)
{
	MYSQL_RES* res;
	MYSQL_ROW row;
	int r;

	this->sql("select count(id) from game");
	res = mysql_use_result(con);
	for (r = 0;r < mysql_field_count(con);r++)
	{
		row = mysql_fetch_row(res);
		if (row < 0)
			break;
		game_id = atoi(row[0]);
	}
	mysql_free_result(res);

	this->sql("select count(id) from record");
	res = mysql_use_result(con);
	for (r = 0;r < mysql_field_count(con);r++)
	{
		row = mysql_fetch_row(res);
		if (row < 0)
			break;
		record_id = atoi(row[0]);
	}
	mysql_free_result(res);

	this->sql("select count(id) from chat");
	res = mysql_use_result(con);
	for (r = 0;r < mysql_field_count(con);r++)
	{
		row = mysql_fetch_row(res);
		if (row < 0)
			break;
		chat_id = atoi(row[0]);
	}
	mysql_free_result(res);
	
	game_id++;
	string sql = "insert into game values(" + to_string(game_id) + ",\"" + BName + "\",\"" + WName + "\"," + to_string(side) + ",'" + getTime() + "',NULL,NULL);";
	if (this->sql(sql))
		cout << "CreateNewGame success!" << endl;
}

//////////////////////////////////////
//记录对局结果，增加胜者经验值
//////////////////////////////////////
void DB::AddResult(string winner, int num)
{
	string sql;
	if(winner != "=" && num == 0)  // 说明中盘胜（有人认输）
		sql = "update game set winner=\"" + winner + "\" where id=" + to_string(game_id) + ";";
	else
		sql = "update game set winner=\"" + winner + "\",num=" + to_string(num) + " where id=" + to_string(game_id) + ";";
	this->sql(sql);
	//todo:查询winner经验，增加一定数值。可以再添加等级机制
}

//////////////////////////////////////
//记录聊天消息
//////////////////////////////////////
void DB::AddChat(string s)
{
	chat_id++;
	string str = "insert into chat values(" + to_string(chat_id) + "," + to_string(game_id) + ",\"" + s + "\");";
	if (sql(str))
	{
		cout << "AddChat success!" << endl;
	}
}