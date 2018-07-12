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
	//void query();  //��ѯ���б��е�����
	//void query(string tablename);  //��ѯĳһ�ű��е�����
	string getTime();  //��ȡϵͳʱ��
	bool sql(string sql);  //����sql���(ֻ��1��)
	string passwdOf(string name); //��ѯĳ���˺ŵ�����
	bool hasPeople(string name);  //��ѯname�˺��Ƿ��Ѿ�����
	bool CreAcc(char* name, char* pswd); //����people���¼�������˻���,preparestatement
	bool CreAcc(string name, string pswd); //����people���¼�������˻���
	void AddRecord(int i, int j);  //������׼�¼
	void AddChat(string s);  //��������¼
	void AddResult(string winner, int num);  //��ӶԾ���Ӯ���
	void CreateNewGame(string BName, string WName, int side);  //�����Ծּ�¼

private:
	unsigned int record_id;  // ��¼��һ�����׼�¼��id
	unsigned int chat_id;  // ��¼���������¼��id
	unsigned int game_id;  // ��¼��һ�ζԾֵ�id

	MYSQL* connect();  //��������
	void openAutoCommit();  //�����Զ��ύ����
};
