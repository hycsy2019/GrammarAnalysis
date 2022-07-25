#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <vector>
#define ARRAY_SIZE(a)(sizeof(a)/sizeof(a[0])) //数组元素个数

using namespace std;

enum errorType{DIGIT,COMMENT,STRING,CHAR};//拼写错误类型：数字、注释、字符串、特殊字符

typedef struct error
{
	errorType type;//错误类型
	int line;//行号
	string s;//错误单词
}Error;

ifstream in("input.txt");//输入文件
ofstream out("output.txt");//输出文件
int Line=1;//语句行数
int Sum;//字符总数
int Opt;//运算符总数
int Id;//标识符总数
int Con;//常数总数
int Split;//分界符总数
int Key;//关键字总数
vector<Error> Errors;//存储错误位置

/*查找s是否在保留字表keyword中*/
bool lookupKeyword(string s)
{
	/*保留字表*/
	string keyword[] = { "void","main","short","long","int","double","float","while","if","else","for","break",
		"return","char","auto","signed","unsigned","struct","union","enum","static","switch","case","default",
	"continue","register","const","volatile","typedef","extern","do","goto","sizeof"};

	/*匹配s，是保留字返回1，否则返回0*/
	for (int i = 0; i < ARRAY_SIZE(keyword); i++) 
	{
		if (s.compare(keyword[i]) == 0)
			return 1;
	}
	return 0;
}

/*查找c是否在运算符表optword中*/
bool lookupOpt(char c)
{
	/*运算符表*/
	char optword[] = {'=','!','>','<','+','-','*','%','&','|','^'};

	/*匹配c，是运算符返回1，否则返回0*/
	for (int i = 0; i < ARRAY_SIZE(optword); i++)
	{
		if (c==optword[i])
			return 1;
	}
	return 0;
}

/*查找c是否在分界符表splitword中*/
bool lookupSplit(char c)
{
	/*分界符表*/
	char splitword[] = {',',';','(',')','[',']','{','}' ,'\'','#',':'};

	/*匹配c，是分隔符返回1，否则返回0*/
	for (int i = 0; i < ARRAY_SIZE(splitword); i++)
	{
		if (c == splitword[i])
			return 1;
	}
	return 0;
}

/*查找c是否在缩进符表spaceword中*/
bool lookupSpace(char c)
{
	/*缩进符表*/
	char spaceword[] = { ' ','\t','\n' };

	/*匹配c，是分隔符返回1，否则返回0*/
	for (int i = 0; i < ARRAY_SIZE(spaceword); i++)
	{
		if (c == spaceword[i])
			return 1;
	}
	return 0;
}

/*读取一个字符，字符总数加一*/
char getNextChar()
{
	Sum++;
	return in.get();
}

/*拼写错误处理*/
void spellError(char ch,string errorWord,errorType type)
{
		/*存储错误信息*/
		Error *e=new Error;
		e->line = Line;
		e->type = type;

		/*跳过错误*/
		while ((!lookupSpace(ch))&&(!lookupSplit(ch)))
		{
			errorWord += ch;
			ch = getNextChar();
		}
		
		e->s = errorWord;
		Errors.push_back(*e);

		/*如果为非法字符则不输出，否则会乱码*/
		if (type == CHAR)
			out << "Illegal Character!\n";
		else
			out << errorWord + " is error!\n";
}

/*识别数字*/
char recDigit(char ch)
{
	string s = "";
	while (isdigit(ch))
	{
		s += ch;
		ch = getNextChar();
		if (ch == '.'||ch=='e'||ch=='x') 
		{
			/*判断常数拼写是否正确*/
			s += ch;
			ch = getNextChar();
			if (isdigit(ch)) 
			{
				while (isdigit(ch)) 
				{
					s += ch;
					ch = getNextChar();
				}

				if ((!lookupSpace(ch))&&(!lookupSplit(ch)))
				{
					spellError(ch, s, DIGIT);
					Con--;
					return ch;
				}

				out << "< " << s << " , "<<Line << " , 常数 >\n";
			}
			/*如果后面没有数字，则报错*/
			else
			{
				spellError(ch, s, DIGIT);
				Con--;
				return ch;
			}
		}

		if (!isdigit(ch)) 
		{
			/*常整数*/
			out << "< " << s << " , "<<Line << " , 常数 >\n";
			break;
		}
	}
	return ch;
}

/*识别变量名或保留字*/
char recAlpha(char ch)
{
	string s = "";

	/*扫描获取整个词组*/
	while (isalpha(ch) || isdigit(ch) || ch == '_') 
	{
		s += ch;
		ch = getNextChar();
	}

	/*在keyword表中查找s是否为保留字*/
	if (lookupKeyword(s)) 
	{
		Key++;
		out << "< " << s << " , "<<Line << " , 保留字 >\n";
	}
	else
	{
		Id++;
		out << "< " << s << " , "<<Line << " , 标识符 >\n";
	}

	return ch;
}

/*识别运算符*/
char recOpt(char ch)
{
	string s;
	s = "";
	s += ch;

	/*识别非赋值运算运算符*/
	switch(ch)
	{
		case'>':
			ch = getNextChar();
			if (ch=='>') //>>
			{
				s += ch;
				ch = getNextChar();
			}
			break;

		case'<':
			ch = getNextChar();
			if (ch == '<') //<<
			{
				s += ch;
				ch = getNextChar();
			}
			break;

		case'+':
			ch = getNextChar();
			if (ch == '+' )//++
			{
				s += ch;
				ch = getNextChar();
			}
			out << "< " << s << " , "<<Line << " , 运算符 >\n";
			return ch;

		case'-':
			ch = getNextChar();
			if (ch == '-'||ch=='>' )//-- ->
			{
				s += ch;
				ch = getNextChar();
			}
			out << "< " << s << " , "<<Line << " , 运算符 >\n";
			return ch;

		case'&':
			ch = getNextChar();
			if (ch == '&')//&&
			{
				s += ch;
				ch = getNextChar();
			}
			out << "< " << s << " , "<<Line << " , 运算符 >\n";
			return ch;

		case'|':
			ch = getNextChar();
			if (ch == '|')//||
			{
				s += ch;
				ch = getNextChar();
			}
			out << "< " << s << " , "<<Line << " , 运算符 >\n";
			return ch;

		case'.':
			ch = getNextChar();
			out << "< " << s << " , "<<Line << " , 运算符 >\n";
			return ch;

		default:
			ch = getNextChar();
	}
	
	/*识别赋值运算运算符*/
	if (ch == '=')
	{
		s += ch;
		ch = getNextChar();
	}

	out << "< " << s << " , "<<Line << " , 运算符 >\n";
	return ch;
}

/*识别注释并跳过*/
char recCom(char ch)
{
	int state = 1;
	string s;
	s = "";
	bool flag = false;

	while (true)
	{
		if(ch!='\n')
		s += ch;

		if (state == 1)
		{
			if (ch == '/')
				state = 2;
		}
		else if (state == 2)
		{
			if (ch == '*')
			{
				state = 3;
				flag = true;
			}
			else if(ch=='/')
			{
				state = 6;
			}
			else
			{
				/*非注释*/
				if (ch == '=')
				{
					out << "< /=" << ch << ",运算符 >\n";
					ch = getNextChar();
				}
				else
				{
					out << "< /" << ",运算符 >\n";
				}
				Opt++;
				return ch;
			}
		}
		else if (state == 3)
		{
			if (ch == '*')
				state = 4;
			/*无法找到注释结尾则报错*/
			else if (ch == '\n')
			{
				ch = getNextChar();
				spellError(ch, s, COMMENT);
				Line++;
				return ch;
			}
		}
		else if (state == 4)
		{
			if (ch == '/')
				state = 5;
			else
			{
				state = 3;
				continue;
			}
		}
		else if (state == 6)
		{
			if (ch == '\n')
			{
				state = 5;
				Line++;
			}
			if (in.eof())
				return ch;
		}

		if (state == 5)
		{
			ch = getNextChar();
			out << "< " << s << " , "<<Line << " , 注释 >\n";
			return ch;
		}

		ch = getNextChar();
	}
}

/*识别字符串*/
char recString(char ch)
{
	string s = "\"";
	
	ch = getNextChar();
	while (ch != '"'&&ch!='\n')
	{
		s += ch;
		ch = getNextChar();
	}

	if (ch == '"')
	{
		s += ch;
		out << "< " << s << " , "<<Line << " , 字符串 >\n";
	}
	else
	{
		spellError(ch, s, STRING);
		Line++;
	}
	ch = getNextChar();
	return ch;
}

/*打印错误*/
void printError()
{
	int i = 1;
	for (vector<Error>::iterator it = Errors.begin(); it != Errors.end(); ++it)
	{
		if (i == 1)
			out << "-------------------------------Error-------------------------------" << endl;

		out << "Line " << it->line << "：";
		switch (it->type)
		{
			case DIGIT:
				out << "常数" << "“" << it->s << "”" << "格式错误" << endl;
				break;
			case COMMENT:
				out << "未找到注释" << "“" << it->s << "”" << "的结束符" << endl;
				break;
			case CHAR:
				out << "存在非法字符" << endl;
				break;
			case STRING:
				out << "字符串" << it->s << "缺少右引号" << endl;
				break;
			default:
				break;
		}
		i++;
	}
}

int main()
{
	char ch;

	cout << "正在进行词法分析..." << endl;
	
	ch = getNextChar();
	while (!in.eof()) 
	{
		/*识别界符*/
		if (lookupSplit(ch)) 
		{
			out << "< " << ch << " , "<<Line << " , 界符 >\n";
			Split++;
			ch = getNextChar();
		}
		/*识别实数*/
		else if (isdigit(ch)) 
		{
			ch=recDigit(ch);
			Con++;
		}
		/*识别标识符/保留字*/
		else if (isalpha(ch) || ch == '_') 
		{
			ch=recAlpha(ch);
		}
		/*识别运算符*/
		else if (lookupOpt(ch)== 1)
		{
			ch = recOpt(ch);
			Opt++;
		}
		/*识别注释*/
		else if (ch=='/')
		{
			ch = recCom(ch);
		}
		/*识别换行符，行数+1*/
		else if (ch == '\n')
		{
			Line++;
			ch = getNextChar();
		}
		/*识别转义符*/
		else if (ch == '\\')
		{
			out << "< " << ch << " , "<<Line << " , 转义符 >\n";
			ch = getNextChar();
		}
		/*识别字符串*/
		else if (ch == '"')
		{
			ch = recString(ch);
		}
		/*跳过当前单词*/
		else 
		{
			/*检测到非法字符则报错*/
			if (!lookupSpace(ch))
			{
				spellError(ch, &ch, CHAR);
			}
			ch = getNextChar();
		}
	}

	out << "总计" << Line << "行语句，"<<Sum<<"个字符。"<<endl;
	out << "其中包含" << Opt << "个运算符，" << Id << "个标识符，"
		<< Con << "个常数，" << Split << "个分界符，" << Key << "个关键字。" << endl;

	printError();

	cout << "分析完成，结果已输出到\"output.txt\"中" << endl;
	system("pause");
	return 0;
}