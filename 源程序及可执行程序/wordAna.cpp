#include"parser.h"
#define ARRAY_SIZE(a)(sizeof(a)/sizeof(a[0])) //����Ԫ�ظ���

using namespace std;

enum errorType { DIGIT, COMMENT, STRING, CHAR };//ƴд�������ͣ����֡�ע�͡��ַ����������ַ�

typedef struct error
{
	errorType type;//��������
	int line;//�к�
	string s;//���󵥴�
}Error;

ifstream in("input.txt");//�����ļ�
int Line = 1;//�������
int Sum;//�ַ�����
int Opt;//���������
int Id;//��ʶ������
int Con;//��������
int Split;//�ֽ������
int Key;//�ؼ�������
vector<Error> Errors;//�洢����λ��
map<string, struct property> Tokens;//���ű��ɷ��Ų��ҵ�������
vector<string> ana_s ;//���������ַ���

/*����s�Ƿ��ڱ����ֱ�keyword��*/
bool lookupKeyword(string s)
{
	/*�����ֱ�*/
	string keyword[] = { "void","main","short","long","int","double","float","while","if","else","for","break",
		"return","char","auto","signed","unsigned","struct","union","enum","static","switch","case","default",
	"continue","register","const","volatile","typedef","extern","do","goto","sizeof" };

	/*ƥ��s���Ǳ����ַ���1�����򷵻�0*/
	for (int i = 0; i < ARRAY_SIZE(keyword); i++)
	{
		if (s.compare(keyword[i]) == 0)
			return 1;
	}
	return 0;
}

/*����c�Ƿ����������optword��*/
bool lookupOpt(char c)
{
	/*�������*/
	char optword[] = { '=','!','>','<','+','-','*','%','&','|','^' };

	/*ƥ��c�������������1�����򷵻�0*/
	for (int i = 0; i < ARRAY_SIZE(optword); i++)
	{
		if (c == optword[i])
			return 1;
	}
	return 0;
}

/*����c�Ƿ��ڷֽ����splitword��*/
bool lookupSplit(char c)
{
	/*�ֽ����*/
	char splitword[] = { ',',';','(',')','[',']','{','}' ,'\'','#',':' };

	/*ƥ��c���Ƿָ�������1�����򷵻�0*/
	for (int i = 0; i < ARRAY_SIZE(splitword); i++)
	{
		if (c == splitword[i])
			return 1;
	}
	return 0;
}

/*����c�Ƿ�����������spaceword��*/
bool lookupSpace(char c)
{
	/*��������*/
	char spaceword[] = { ' ','\t','\n' };

	/*ƥ��c���Ƿָ�������1�����򷵻�0*/
	for (int i = 0; i < ARRAY_SIZE(spaceword); i++)
	{
		if (c == spaceword[i])
			return 1;
	}
	return 0;
}

/*��ȡһ���ַ����ַ�������һ*/
char getNextChar()
{
	Sum++;
	char ch = in.get();
	return ch;
}

/*ƴд������*/
void spellError(char ch, string errorWord, errorType type)
{
	/*�洢������Ϣ*/
	Error *e = new Error;
	e->line = Line;
	e->type = type;

	/*��������*/
	while ((!lookupSpace(ch)) && (!lookupSplit(ch)))
	{
		errorWord += ch;
		ch = getNextChar();
	}

	e->s = errorWord;
	Errors.push_back(*e);

	/*���Ϊ�Ƿ��ַ�����������������*/
	if (type == CHAR)
		cout << "Illegal Character!\n";
	else
		cout << errorWord + " is error!\n";
}

/*ʶ������*/
char recDigit(char ch)
{
	string s = "";
	while (isdigit(ch))
	{
		s += ch;
		ch = getNextChar();
		if (ch == '.' || ch == 'e' || ch == 'x')
		{
			/*�жϳ���ƴд�Ƿ���ȷ*/
			s += ch;
			ch = getNextChar();
			if (isdigit(ch))
			{
				while (isdigit(ch))
				{
					s += ch;
					ch = getNextChar();
				}

				if ((!lookupSpace(ch)) && (!lookupSplit(ch)))
				{
					spellError(ch, s, DIGIT);
					Con--;
					return ch;
				}

				cout << "< " << s << " , " << Line << " , ���� >\n";
				Tokens[s] = property("num", Line);
				ana_s.push_back(s);
			}
			/*�������û�����֣��򱨴�*/
			else
			{
				spellError(ch, s, DIGIT);
				Con--;
				return ch;
			}
		}

		if (!isdigit(ch))
		{
			/*������*/
			cout << "< " << s << " , " << Line << " , ���� >\n";
			Tokens[s] = property("num", Line);
			ana_s.push_back(s);
			break;
		}
	}
	return ch;
}

/*ʶ�������������*/
char recAlpha(char ch)
{
	string s = "";

	/*ɨ���ȡ��������*/
	while (isalpha(ch) || isdigit(ch) || ch == '_')
	{
		s += ch;
		ch = getNextChar();
	}

	/*��keyword���в���s�Ƿ�Ϊ������*/
	if (lookupKeyword(s))
	{
		Key++;
		cout << "< " << s << " , " << Line << " , ������ >\n";
		Tokens[s] = property("keyword", Line);
		ana_s.push_back(s);
	}
	else
	{
		Id++;
		cout << "< " << s << " , " << Line << " , ��ʶ�� >\n";
		Tokens[s] = property("id", Line);
		ana_s.push_back(s);
	}

	return ch;
}

/*ʶ�������*/
char recOpt(char ch)
{
	string s;
	s = "";
	s += ch;

	/*ʶ��Ǹ�ֵ���������*/
	switch (ch)
	{
	case'>':
		ch = getNextChar();
		if (ch == '>') //>>
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
		if (ch == '+')//++
		{
			s += ch;
			ch = getNextChar();
		}
		cout << "< " << s << " , " << Line << " , ����� >\n";
		Tokens[s] = property("opt", Line);
		ana_s.push_back(s);
		return ch;

	case'-':
		ch = getNextChar();
		if (ch == '-' || ch == '>')//-- ->
		{
			s += ch;
			ch = getNextChar();
		}
		cout << "< " << s << " , " << Line << " , ����� >\n";
		Tokens[s] = property("opt", Line);
		ana_s.push_back(s);
		return ch;

	case'&':
		ch = getNextChar();
		if (ch == '&')//&&
		{
			s += ch;
			ch = getNextChar();
		}
		cout << "< " << s << " , " << Line << " , ����� >\n";
		Tokens[s] = property("opt", Line);
		ana_s.push_back(s);
		return ch;

	case'|':
		ch = getNextChar();
		if (ch == '|')//||
		{
			s += ch;
			ch = getNextChar();
		}
		cout << "< " << s << " , " << Line << " , ����� >\n";
		Tokens[s] = property("opt", Line);
		ana_s.push_back(s);
		return ch;

	case'.':
		ch = getNextChar();
		cout << "< " << s << " , " << Line << " , ����� >\n";
		Tokens[s] = property("opt", Line);
		ana_s.push_back(s);
		return ch;

	default:
		ch = getNextChar();
	}

	/*ʶ��ֵ���������*/
	if (ch == '=')
	{
		s += ch;
		ch = getNextChar();
	}

	cout << "< " << s << " , " << Line << " , ����� >\n";
	Tokens[s] = property("opt", Line);
	ana_s.push_back(s);
	return ch;
}

/*ʶ��ע�Ͳ�����*/
char recCom(char ch)
{
	int state = 1;
	string s;
	s = "";
	bool flag = false;

	while (true)
	{
		if (ch != '\n')
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
			else if (ch == '/')
			{
				state = 6;
			}
			else
			{
				/*��ע��*/
				if (ch == '=')
				{
					cout << "< /= , " <<Line<< " , ����� >\n";
					Tokens["/="] = property("opt", Line);
					ana_s.push_back(s);
					ch = getNextChar();
				}
				else
				{
					cout << "< / ," <<Line<< " , ����� >\n";
					Tokens["/"] = property("opt", Line);
					ana_s.push_back("/");
				}
				Opt++;
				return ch;
			}
		}
		else if (state == 3)
		{
			if (ch == '*')
				state = 4;
			/*�޷��ҵ�ע�ͽ�β�򱨴�*/
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
			Tokens[s] = property("com", Line);
			return ch;
		}

		ch = getNextChar();
	}
}

/*ʶ���ַ���*/
char recString(char ch)
{
	string s = "\"";

	ch = getNextChar();
	while (ch != '"'&&ch != '\n')
	{
		s += ch;
		ch = getNextChar();
	}

	if (ch == '"')
	{
		s += ch;
		cout << "< " << s << " , " << Line << " , �ַ��� >\n";
	}
	else
	{
		spellError(ch, s, STRING);
		Line++;
	}
	ch = getNextChar();
	return ch;
}

/*��ӡ����*/
void printError()
{
	int i = 1;
	for (vector<Error>::iterator it = Errors.begin(); it != Errors.end(); ++it)
	{
		if (i == 1)
			cout << "-------------------------------Error-------------------------------" << endl;

		cout << "Line " << it->line << "��";
		switch (it->type)
		{
		case DIGIT:
			cout << "����" << "��" << it->s << "��" << "��ʽ����" << endl;
			break;
		case COMMENT:
			cout << "δ�ҵ�ע��" << "��" << it->s << "��" << "�Ľ�����" << endl;
			break;
		case CHAR:
			cout << "���ڷǷ��ַ�" << endl;
			break;
		case STRING:
			cout << "�ַ���" << it->s << "ȱ��������" << endl;
			break;
		default:
			break;
		}
		i++;
	}
}

/*��ӡ���ű�*/
void printTokens()
{
	cout << "���ʷ��ű�" << endl;
	cout << std::left << setw(15) << "����" << setw(15) << "����"<<endl;
	for (map<string, struct property>::iterator iter = Tokens.begin(); iter != Tokens.end(); iter++)
	{
		cout <<setw(15)<< iter->first << setw(15)<< iter->second.type<<endl;
	}
	cout << "+-------------------------------------------------------------+\n";
}

/*�ʷ�����*/
void wordAna()
{
	char ch;
	string s="";

	cout << "�ʷ�����" << endl;

	ch = getNextChar();
	while (!in.eof())
	{
		/*ʶ����*/
		if (lookupSplit(ch))
		{
			cout << "< " << ch << " , " << Line << " , ��� >\n";
			Tokens[s+ch] = property("split", Line);
			ana_s.push_back(s+ch);
			Split++;
			ch = getNextChar();
		}
		/*ʶ��ʵ��*/
		else if (isdigit(ch))
		{
			ch = recDigit(ch);
			Con++;
		}
		/*ʶ���ʶ��/������*/
		else if (isalpha(ch) || ch == '_')
		{
			ch = recAlpha(ch);
		}
		/*ʶ�������*/
		else if (lookupOpt(ch) == 1)
		{
			ch = recOpt(ch);
			Opt++;
		}
		/*ʶ��ע��*/
		else if (ch == '/')
		{
			ch = recCom(ch);
		}
		/*ʶ���з�������+1*/
		else if (ch == '\n')
		{
			Line++;
			ch = getNextChar();
		}
		/*ʶ��ת���*/
		else if (ch == '\\')
		{
			cout << "< " << ch << " , " << Line << " , ת��� >\n";
			ch = getNextChar();
		}
		/*ʶ���ַ���*/
		else if (ch == '"')
		{
			ch = recString(ch);
		}
		/*������ǰ����*/
		else
		{
			/*��⵽�Ƿ��ַ��򱨴�*/
			if (!lookupSpace(ch))
			{
				spellError(ch, &ch, CHAR);
			}
			ch = getNextChar();
		}
	}
	cout << "+-------------------------------------------------------------+\n";
	printTokens();

	cout << "�ܼ�" << Line << "����䣬" << Sum << "���ַ���" << endl;
	cout << "���а���" << Opt << "���������" << Id << "����ʶ����"
		<< Con << "��������" << Split << "���ֽ����" << Key << "���ؼ��֡�" << endl;
	cout << "+-------------------------------------------------------------+\n";

	printError();
}