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

/*产生式*/
typedef struct Node
{
	string left;//产生式左部
	vector<string>right;//产生式右部
	int index = 0;//语法分析中的点
}node;

/*项目集*/
struct I 
{
	int id;//项目集编号
	vector<node> vec;//项目集
	map<string, int> m; //转化关系
};

/*词法单元*/
struct property
{
	string type;//单词属性类别
	int line;//所在行数
	property(string _type, int _line)
	{
		type = _type;
		line = _line;
	}
	property(){}
};

extern ifstream in;//输入文件
extern ofstream out;//输出文件

extern vector<string> ana_s;//待分析的字符串

extern map<string, struct property> Tokens;//符号表，由符号查找到其属性

extern int grammer_num;//文法数量
extern node grammar[400]; //文法

extern set<string>term;//终结符号
extern set<string>non_term;//非终结符号

extern map<string, set<string>>first_set;//first集
extern map<string, set<string>>follow_set;//follow集

extern map<pair<string, string>, int>analysisTable_1;//自上而下分析表 (<文法左部, 终结符号+&>, 产生式下标)

extern vector<I> projectSet; //项目集

extern map<pair<int, string>, pair<string, int>> ACTION; //ACTION函数
extern map<pair<int, string>, int> GOTO;				  //GOTO函数

extern void wordAna();//词法分析程序
