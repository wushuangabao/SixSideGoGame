#include "mysql.h"
#include <string>
#pragma comment(lib,"libmySQL.lib")

using namespace std;

class DB
{
public:
	char user[20];
	char passwd[20];
	char ip[20];
	char dbname[20];
	MYSQL * con;

	DB();
	~DB();
	//void query();  //查询所有表中的数据
	//void query(string tablename);  //查询某一张表中的数据
	string getTime();  //获取系统时间
	bool sql(string sql);  //调用sql语句(只能1句)
	string passwdOf(string name); //查询某个账号的密码
	bool hasPeople(string name);  //查询name账号是否已经存在
	bool CreAcc(char* name, char* pswd); //创建people表记录（创建账户）,preparestatement
	bool CreAcc(string name, string pswd); //创建people表记录（创建账户）
	void AddRecord(int i, int j);  //添加棋谱记录
	void AddChat(string s);  //添加聊天记录
	void AddResult(string winner, int num);  //添加对局输赢结果
	void CreateNewGame(string BName, string WName, int side);  //创建对局记录

private:
	unsigned int record_id;  // 记录上一条棋谱记录的id
	unsigned int chat_id;  // 记录上条聊天记录的id
	unsigned int game_id;  // 记录上一次对局的id

	MYSQL* connect();  //建立连接
	void openAutoCommit();  //开启自动提交事务
};
