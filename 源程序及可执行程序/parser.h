#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#include<set>
#include<map>
#include<algorithm>
#include<iterator>
#include<iomanip>
#include<queue>
#include<stack>
#include <cstdio>
#include <cstdlib>

using namespace std;

/*����ʽ*/
typedef struct Node
{
	string left;//����ʽ��
	vector<string>right;//����ʽ�Ҳ�
	int index = 0;//�﷨�����еĵ�
}node;

/*��Ŀ��*/
struct I 
{
	int id;//��Ŀ�����
	vector<node> vec;//��Ŀ��
	map<string, int> m; //ת����ϵ
};

/*�ʷ���Ԫ*/
struct property
{
	string type;//�����������
	int line;//��������
	property(string _type, int _line)
	{
		type = _type;
		line = _line;
	}
	property(){}
};

extern ifstream in;//�����ļ�
extern ofstream out;//����ļ�

extern vector<string> ana_s;//���������ַ���

extern map<string, struct property> Tokens;//���ű��ɷ��Ų��ҵ�������

extern int grammer_num;//�ķ�����
extern node grammar[400]; //�ķ�

extern set<string>term;//�ս����
extern set<string>non_term;//���ս����

extern map<string, set<string>>first_set;//first��
extern map<string, set<string>>follow_set;//follow��

extern map<pair<string, string>, int>analysisTable_1;//���϶��·����� (<�ķ���, �ս����+&>, ����ʽ�±�)

extern vector<I> projectSet; //��Ŀ��

extern map<pair<int, string>, pair<string, int>> ACTION; //ACTION����
extern map<pair<int, string>, int> GOTO;				  //GOTO����

extern void wordAna();//�ʷ���������
