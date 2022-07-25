#include"parser.h"
using namespace std;

int grammer_num;//文法数量
node grammar[400]; //文法

set<string>term;//终结符号
set<string>non_term;//非终结符号

map<string, set<string>>first_set;//first集
map<string, set<string>>follow_set;//follow集

map<pair<string, string>, int>analysisTable_1;//自上而下分析表 (<文法左部, 终结符号+&>, 产生式下标)

vector<I> projectSet; //项目集

map<pair<int, string>, pair<string, int>> ACTION; //ACTION函数
map<pair<int, string>, int> GOTO;				  //GOTO函数

/*判断为终结符还是非终结符*/
bool is_term(string s)
{
	if (s[0] == '<' && s[s.length() - 1] == '>')
		return false;
	return true;
}

/*读入文法*/
void scan_grammer(string filePath)
{
	ifstream t(filePath);
	string line = "";
	int index = 0;//grammer[100]下标
	while (getline(t, line)) //逐行读取文法
	{
		line += " ";
		bool isleft = true;//是否是文法左部
		for (int i = 0, j = 0; i < line.length();) {//通过空格分割文法字符串

			// ->
			if (line[i] == '-' && line[i + 1] == '>') {
				i += 3;
				j += 3;
				isleft = false;
			}

			// |
			else if (line[i] == '|') {
				index++;
				grammar[index].left = grammar[index - 1].left;
				i += 2;
				j += 2;
			}

			//文法左部
			else if (line[i] == ' ' && isleft) {
				string temp = line.substr(j, i - j);
				grammar[index].left = temp;
				j = i + 1;
				i++;
			}

			//文法右部
			else if (line[i] == ' ' && !isleft) {
				string temp = line.substr(j, i - j);
				if(temp!="")
					grammar[index].right.push_back(temp);
				j = i + 1;
				i++;
			}

			else i++;

		}
		index++;
	}

	grammer_num = index;

	//初始化终结符号和非终结符号表
	for (int i = 0; i < index; i++) {
		non_term.insert(grammar[i].left);
		for (vector<string>::iterator it = grammar[i].right.begin(); it != grammar[i].right.end(); it++) {
			if (is_term(*it))
				term.insert(*it);//插入终结符号
			else
				non_term.insert(*it);//插入非终结符号
		}
	}

	//输出文法
	cout << "从"<<filePath<<"读入的文法如下：" << endl;
	for (int i = 0; i < index; i++) {
		cout << grammar[i].left << "->";
		for (vector<string>::iterator it = grammar[i].right.begin(); it != grammar[i].right.end(); it++) {
			cout << *it;
		}
		cout << endl;
	}
	cout << "+-------------------------------------------------------------+\n";
}

/*生成first集*/
void set_first(string target) 
{
	//匹配文法左部
	for (int i = 0; i < grammer_num; i++) {
		if (grammar[i].left == target) {
			int num = grammar[i].right.size();
			for (vector<string>::iterator it = grammar[i].right.begin(); it != grammar[i].right.end(); it++) {

				//终结符号
				if (is_term(*it)) {
					first_set[target].insert(*it);
					break;
				}

				//空串
				else if (*it == "@") {
					if ((it + 1) == grammar[i].right.end())//最后一个字符串
						first_set[target].insert("@");
				}

				//非终结符号
				else {
					if ((*it) != grammar[i].left)//避免左递归
					{
						set_first(*it);//递归求first集
						if (first_set[*it].count("@") == 0)//如果这个非终结符号的first集没有@
						{
							set_union(first_set[target].begin(), first_set[target].end(), first_set[*it].begin(), first_set[*it].end(), inserter(first_set[target], first_set[target].begin())); // 求并集
							break;
						}
						else {
							set_union(first_set[target].begin(), first_set[target].end(), first_set[*it].begin(), first_set[*it].end(), inserter(first_set[target], first_set[target].begin())); // 求并集
							first_set[target].erase("@");
						}
					}
					else
						break;
				}
			}
		}
	}
}

/*生成follow集*/
void set_follow(string target,string start_term) 
{
	for (int i = 0; i < grammer_num; i++) {
		bool flag = false;
		vector<string>::iterator it;
		for (it = grammar[i].right.begin(); it != grammar[i].right.end(); it++)
		{
			if (*it == target) {
				flag = true;
				break;
			}
		}
		if (flag) {
			//右边为空串
			if ((it + 1) == grammar[i].right.end()) {
				if (*it != grammar[i].left) {
					set_follow(grammar[i].left,start_term);
					set_union(follow_set[*it].begin(), follow_set[*it].end(), follow_set[grammar[i].left].begin(), follow_set[grammar[i].left].end(), inserter(follow_set[*it], follow_set[*it].begin())); // 求并集
				}
			}

			//右边不为空
			else {
				vector<string>::iterator it2;
				for (it2 = it + 1; it2 != grammar[i].right.end(); it2++) {
					if (is_term(*it2)) {
						follow_set[*it].insert(*it2);
						break;
					}
					else if (first_set[*it2].count("@") == 0)
					{
						set_union(follow_set[*it].begin(), follow_set[*it].end(), first_set[*it2].begin(), first_set[*it2].end(), inserter(follow_set[*it], follow_set[*it].begin())); // 求并集
						break;
					}
					else {
						set_union(follow_set[*it].begin(), follow_set[*it].end(), first_set[*it2].begin(), first_set[*it2].end(), inserter(follow_set[*it], follow_set[*it].begin())); // 求并集
						set_union(follow_set[*it].begin(), follow_set[*it].end(), follow_set[grammar[i].left].begin(), follow_set[grammar[i].left].end(), inserter(follow_set[*it], follow_set[*it].begin())); // 求并集follow_set[*it].erase("@");
						follow_set[*it].erase("@");
					}
				}
			}

		}
		if (grammar[i].left == start_term)//文法左部为起始符时在其follow集中加入$
			follow_set[start_term].insert("$");
	}
}

/*产生first集和follow集，并输出到文件中*/
void generate_FirstAndFollow(string start_term)
{
	for (set<string>::iterator it = non_term.begin(); it != non_term.end(); it++) {
		set_first(*it);
	}
	for (set<string>::iterator it = non_term.begin(); it != non_term.end(); it++) {
		set_follow(*it,start_term);
	}
	for (set<string>::iterator it = non_term.begin(); it != non_term.end(); it++) {
		set_follow(*it,start_term);
	}
	cout << "first集" << endl;
	for (map<string, set<string>>::iterator it = first_set.begin(); it != first_set.end(); it++) {
		cout << std::left << setw(20) << it->first << ":";
		for (set<string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++) {
			cout << *it2 << " ";
		}
		cout << endl;
	}
	cout << "+-------------------------------------------------------------+\n";
	cout << "follow集" << endl;
	for (map<string, set<string>>::iterator it = follow_set.begin(); it != follow_set.end(); it++) {
		cout << std::left << setw(20) << it->first << ":";
		for (set<string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++) {
			cout << *it2 << " ";
		}
		cout << endl;
	}
	cout << "+-------------------------------------------------------------+\n"; 
}

/*生成由上至下的分析表*/
map<pair<string, string>, int> getTopToBottomTable()
{
	for (int i = 0; i < grammer_num; i++)
	{
		for (vector<string>::iterator it0 = grammar[i].right.begin(); it0 != grammar[i].right.end(); it0++) {
			string s = *it0;
			if (is_term(s) && s != "@") {	//终结符号
				pair<string, string> position;
				position.first = grammar[i].left;
				position.second = s;
				analysisTable_1[position] = i;
				break;
			}
			else if (s == "@" && (it0 + 1) == grammar[i].right.end()) {
				for (set<string>::iterator it2 = follow_set[grammar[i].left].begin(); it2 != follow_set[grammar[i].left].end(); it2++) {
					pair<string, string> position;
					position.first = grammar[i].left;
					position.second = *it2;
					analysisTable_1[position] = -1;
				}
				analysisTable_1[make_pair(grammar[i].left, "$")] = -1;
				break;
			}
			else {
				set<string>::iterator it;
				bool flag = false;
				for (it = first_set[s].begin(); it != first_set[s].end(); it++) {
					if (*it != "@") {
						pair<string, string> position;
						position.first = grammar[i].left;
						position.second = *it;
						analysisTable_1[position] = i;
					}
					else {
						flag = true;
						continue;
					}
				}
				if (!flag)
					break;
			}

		}
	}
	return analysisTable_1;
}

/*打印预测分析表*/
void display_Table()
{
	set<string>::iterator it1, it2;

	term.insert("$");
	term.erase("@");

	cout << "预测分析表" << endl;
	cout << std::left << setw(15) << "";
	for (it1 = term.begin(); it1 != term.end(); it1++) {
		cout << std::left << setw(15) << *it1;
	}
	cout << endl;

	for (it2 = non_term.begin(); it2 != non_term.end(); it2++) {
		cout << std::left << setw(15) << *it2;
		for (it1 = term.begin(); it1 != term.end(); it1++)
		{
			pair<string, string> temp;
			temp.first = *it2;
			temp.second = *it1;
			if (analysisTable_1.count(temp) == 0) {
				cout << std::left << std::left << setw(15) << "";
			}
			else {
				int i = analysisTable_1[temp];
				if (i == -1) {
					cout << std::left << setw(15) << "@";
				}
				else {	
					for (vector<string>::iterator it = grammar[i].right.begin(); it != grammar[i].right.end(); it++) {
						if(it != grammar[i].right.begin()) cout << setw(0);
						cout << *it;
					}
					cout << "       ";
				}
			}
		}
		cout << endl;
	}
	term.erase("$");
	term.insert("@");
	cout << "+-------------------------------------------------------------+\n";
}

/*打印分析LL分析句子时的栈、输入、输出*/
void printAnaStepLL(string S[], int ip,string input,int gra_index)
{
	/*输出栈*/
	string str="";
	for(int i=0;i<=ip;i++)
	{
		str += S[i];
	}
	cout << std::left << setw(30) << str;


	/*输出缓冲区*/
	cout << std::left<< setw(20) << input;

	/*输出使用的文法规则*/
	if (gra_index != -2)
	{
		if(gra_index==-1)	cout<< "@";
		else
		{
			cout << grammar[gra_index].left << "->";
			for (vector<string>::iterator it = grammar[gra_index].right.begin(); it != grammar[gra_index].right.end(); it++)
				cout << *it;
		}
	}
	cout << endl;
}

/*将单词转换为文法中的非终结符*/
string tokenToTerm(string token)
{
	string type = Tokens[token].type;

	if(type == "num")	return "num";
	return token;
}

/*LL自顶向下分析句子*/
void analyzeLL()
{
	string Stack[100];//分析栈
	int ip = 1;//栈顶元素下标
	vector<string> ana_s_LL = ana_s;//复制待分析串

	/*初始化栈及指针*/
	Stack[0] = "$";
	Stack[1] = "<E>";
	int gra_index=-2;//使用到的文法规则下标
	ana_s_LL.push_back("$");
	vector<string>::iterator it = ana_s_LL.begin();//遍历待分析句子各个成分的迭代器

	cout << setw(30) << "栈" << setw(20) << "输入" << setw(20) << "输出" << endl;

	while (Stack[ip] != "$")
	{
		vector<string>::iterator it_copy{ it };
		string buf = "";
		while (it_copy != ana_s_LL.end())
		{
			buf+= *it_copy;
			it_copy++;
		}
		printAnaStepLL(Stack, ip,buf, gra_index);
		gra_index = -2;
		string token_switch = tokenToTerm(*it);//单词转换为非终结符

		/*栈顶为终结符号*/
		if (term.count(Stack[ip])!= 0)
		{
			if (Stack[ip] == token_switch)
			{
				ip--;
				it++;
			}
			else
			{
				cout << "语法错误" << endl;
				it++;
				continue;
			}
		}
		/*栈顶为非终结符号*/
		else
		{
			pair<string, string> temp;
			temp.first = Stack[ip];
			temp.second = token_switch;
			if (analysisTable_1.count(temp) != 0)
			{
				ip--;
				gra_index = analysisTable_1[temp];
				if(gra_index !=-1)
				{
					/*产生式反向入栈*/
					for (vector<string>::reverse_iterator it = grammar[gra_index].right.rbegin(); it != grammar[gra_index].right.rend(); it++)
					{
						if (*it == "")	continue;
						Stack[++ip] = *it;
					}
				}
			}
			else
			{
				cout << "语法错误" << endl;
				it++;
				continue;
			}
		}
	}
	printAnaStepLL(Stack, ip, "$", gra_index);
	cout << "+-------------------------------------------------------------+\n";
}

/*判断右部是否相同*/
bool judgeIs_SameVecOfString(vector<string> tmps, vector<string> vs) {
	if (tmps.size() == vs.size()) {
		for (int z = 0; z < tmps.size(); z++) {
			if (tmps[z] != vs[z]) {
				return false;
			}
		}
		return true;
	}
	return false;
}

/*遍历判断是否为重复项目集*/
bool judgeIs_SameVecOfNode(vector<node> tmp, vector<node> v) {
	if (tmp.size() != v.size()) return false;
	for (int j = 0; j < v.size(); j++) {//遍历一遍v，看看tmp里有没有v[j]
		bool has_vj = false;
		for (int k = 0; k < tmp.size(); k++) {
			if (v[j].index == tmp[k].index && v[j].left == tmp[k].left) {
				vector<string> tmps = tmp[k].right;
				vector<string> vs = v[j].right;
				if (judgeIs_SameVecOfString(tmps, vs)) {
					has_vj = true;
					break;
				}
			}
		}
		//如果没找到，就不一样
		if (has_vj == false) return false;
	}
	return true;
}

/*打印项目集*/
void printI(I pi) {
	cout << "I" << pi.id << endl;
	vector<node> v = pi.vec;
	for (int i = 0; i < v.size(); i++) {
		cout << v[i].left << "->";
		for (int j = 0; j < v[i].right.size(); j++) {
			if (j == v[i].index) cout << ".";
			cout << v[i].right[j] << " ";
		}
		if (v[i].index == v[i].right.size()) cout << ".";
		cout << endl;
	}
	for (map<string, int>::iterator it = pi.m.begin(); it != pi.m.end(); it++) {
		cout << it->first << " " << it->second << endl;
	}
	cout << endl;
}

/*构造项目集*/
void generateProjectSet() {
	//当前项目集
	I tmp;
	int id = 0;
	tmp.id = id;
	id++;
	tmp.vec.push_back(grammar[0]);
	queue<I> que;//等待完成的项目集队列
	que.push(tmp);

	vector<vector<node>> vecv;
	vecv.push_back(tmp.vec);
	map<int, vector<node> > mapvi;
	mapvi[tmp.id] = tmp.vec;

	cout << "构造LR(0)项目集如下：（每个项目集后跟转换关系）" << endl;
	while (!que.empty()) {
		tmp = que.front();
		que.pop();

		int size = tmp.vec.size();
		bool first = true;
		set<string> already;
		while (first || size != tmp.vec.size()) {
			first = false;
			size = tmp.vec.size();
			set<string> left;
			for (int i = 0; i < size; i++) {
				node t = tmp.vec[i];//当前文法；
				if (t.index < t.right.size()) {//如果点后有文法符号
					string s = t.right[t.index];
					if (!is_term(s) && already.count(s) == 0) {//如果是非终结符号且没被添加
						left.insert(t.right[t.index]);//把点后的非终结文法符号加到left里
					}
				}
			}
			for (set<string>::iterator it = left.begin(); it != left.end(); it++) {
				for (int j = 0; j < grammer_num; j++) {
					if (grammar[j].left == *it) {
						tmp.vec.push_back(grammar[j]);
					}
				}
				already.insert(*it);
			}
		}
		//以上构建完了项目集的文法集，下面构建转化关系

		set<string> stringSet;
		//stringSet.insert("@");
		for (int i = 0; i < tmp.vec.size(); i++) {
			node t = tmp.vec[i];//当前文法；
			if (t.index < t.right.size()) {//如果点后有文法符号
				string s = t.right[t.index]; //文法符号
				if (stringSet.count(s) == 0) {
					stringSet.insert(s);
				}
				else {
					continue;
				}
				vector<node> mainGrammerOfProjectSet; //项目集的主要文法 ，其他文法可由主文法推出
				for (int j = 0; j < tmp.vec.size(); j++) { //遍历tmp项目集文法，寻找点后为文法符号s的文法
					node newT = tmp.vec[j];
					if (newT.index < newT.right.size() && newT.right[newT.index] == s) {
						newT.index++;
						mainGrammerOfProjectSet.push_back(newT);
					}
				}
				
				//判断是否已生成该项目集，否则生成新项目集加到projectSet里面
				bool exist = false;
				for (int j = 0; j < vecv.size(); j++) {
					if (judgeIs_SameVecOfNode(vecv[j], mainGrammerOfProjectSet)) {
						exist = true;
						break;
					}
				}
				if (exist) {
					int id = -1;
					for (map<int, vector<node> >::iterator it = mapvi.begin(); it != mapvi.end(); it++) {
						if (judgeIs_SameVecOfNode(it->second, mainGrammerOfProjectSet)) {
							id = it->first;
							break;
						}
					}
					if (id == -1) cout << "mapvi错误" << endl;
					tmp.m[s] = id;
				}
				else {
					I newI;
					newI.id = id;
					tmp.m[s] = newI.id;
					id++;
					newI.vec = mainGrammerOfProjectSet;
					que.push(newI);
					vecv.push_back(mainGrammerOfProjectSet);
					mapvi[newI.id] = mainGrammerOfProjectSet;
				}
			}
		}
		printI(tmp);
		projectSet.push_back(tmp);
	}
	cout << "+-------------------------------------------------------------+\n";
}

/*生成LR0分析表*/
void generateLR0Table() {
	for (int i = 0; i < projectSet.size(); i++) {
		if (i == 2)
			i = 2;
		I currentI = projectSet[i];
		for (int j = 0; j < currentI.vec.size(); j++) {
			node currentNode = currentI.vec[j];
			if (currentNode.index == 1 && currentNode.left == grammar[0].left &&
				currentNode.right.size() == 1 && currentNode.right[0] == grammar[0].right[0]) {
				ACTION[make_pair(currentI.id, "$")] = make_pair("acc", -1);
			}
			if (currentNode.index < currentNode.right.size()) {
				string a = currentNode.right[currentNode.index];
				if (is_term(a) && currentI.m.count(a) != 0) {
					ACTION[make_pair(currentI.id, a)] = make_pair("s", currentI.m[a]);
				}
			}
			else if (currentNode.index == currentNode.right.size()) {
				//set_follow(currentNode.left);
				set<string> s = follow_set[currentNode.left];
				for (set<string>::iterator it = s.begin(); it != s.end(); it++) {
					int id;
					//寻找规约产生式的id；
					for (id = 0; id < grammer_num; id++) {
						if (currentNode.left == grammar[id].left &&
							judgeIs_SameVecOfString(currentNode.right, grammar[id].right)) break;
					}
					if (id == 0) continue;//按第一个产生式归约即为acc,不用管
					if (id == grammer_num) cout << "没有找到归约产生式" << endl;
					ACTION[make_pair(currentI.id, *it)] = make_pair("r", id);
				}
			}
		}
		for (map<string, int>::iterator it = currentI.m.begin(); it != currentI.m.end(); it++) {
			if (!is_term(it->first)) {
				GOTO[make_pair(currentI.id, it->first)] = it->second;
			}
		}
	}
}

/*打印LR0分析表*/
void printLR0Table() {
	vector<vector<string>> SL0Table;
	int id = 0;
	vector<string> symbol;
	//symbol.push_back("状态");
	map<string, int> symbolToId;
	for (set<string>::iterator it = term.begin(); it != term.end(); it++) {
		symbol.push_back(*it);
		symbolToId[*it] = id;
		id += 1;
	}
	symbol.push_back("$");
	symbolToId["$"] = id;
	id += 1;
	for (set<string>::iterator it = non_term.begin(); it != non_term.end(); it++) {
		if (*it == "<E>")	continue;
		symbol.push_back(*it);
		symbolToId[*it] = id;
		id += 1;
	}
	//SL0Table.push_back(symbol);
	for (int i = 0; i < projectSet.size(); i++) {
		vector<string> vec;
		for (int j = 0; j < term.size() + 1; j++) {
			if (ACTION.count(make_pair(i, symbol[j])) != 0) {
				pair<string, int> p = ACTION[make_pair(i, symbol[j])];
				if (p.first == "acc") vec.push_back(p.first);
				else vec.push_back(p.first + to_string(p.second));
			}
			else vec.push_back(" ");
		}

		for (set<string>::iterator it = non_term.begin(); it != non_term.end(); it++) {
			if (*it == "<E>")	continue;
			if (GOTO.count(make_pair(i, *it)) != 0) {
				vec.push_back(to_string(GOTO[make_pair(i, *it)]));
			}
			else vec.push_back(" ");
		}
		SL0Table.push_back(vec);
	}

	cout << "LR(0)分析表" << endl;
	cout << setw(5) << setfill(' ') << "state";
	for (int j = 0; j < symbol.size(); j++) {
		cout << setw(5) << setfill(' ') << symbol[j];
	}
	cout << endl;
	for (int i = 0; i < SL0Table.size(); i++) {
		cout << setw(5) << setfill(' ') << i;
		for (int j = 0; j < SL0Table[j].size(); j++) {
			cout << setw(5) << setfill(' ') << SL0Table[i][j];
		}
		cout << endl;
	}
	cout << "+-------------------------------------------------------------+\n";
}

/*清空文法信息*/
void grammar_clear()
{
	for (int i = 0; i < grammer_num; i++)
	{
		grammar[i].left = "";
		grammar[i].right.clear();
	}
	term.clear();
	non_term.clear();
}

/*打印分析LR分析句子时的栈、输入、输出*/
void printAnaStepLR(pair<int, string> S[], int ip, string input, int gra_index, string action,int next_state)
{
	/*输出栈状态*/
	string str = "";
	for (int i = 0; i <= ip; i++)
		str += std::to_string(S[i].first)+"  ";
	cout << std::left << setw(40) << str;

	/*输出缓冲区*/
	cout << std::left << setw(20) << input;

	/*输出分析动作*/
	if (action == "s")
		cout << "shift" << "  "<<next_state<<" ";
	else if (action == "r")
	{
		cout << "reduce by" << " ";
		if (gra_index != -2)
		{
			cout << grammar[gra_index].left << "->";
			for (vector<string>::iterator it = grammar[gra_index].right.begin(); it != grammar[gra_index].right.end(); it++)
				cout << *it;

			if (GOTO.count(make_pair(S[ip - 1].first, grammar[gra_index].left)) != 0) {
				cout << "  " << "goto[" << S[ip - 1].first << "," << grammar[gra_index].left << "]=" << GOTO[make_pair(S[ip - 1].first, grammar[gra_index].left)];
			}
		}
	}
	else
	{
		cout << "acc"<<endl<<"  "<<S[1].second;
		return;
	}

	/*输出栈符号*/
	if (ip > 0)
	{
		cout << endl<<" ";
		for (int i = 0; i <= ip; i++)
		{
			cout << std::left <<" "<< S[i].second;
		}
	}
	cout <<endl;
}

/*LR自底向上分析句子*/
void analyzeLR()
{
	pair<int,string> Stack[100];//分析栈，包含符号栈和状态栈
	int ip = 0;//栈顶元素下标
	vector<string> ana_s_LR = ana_s;//复制待分析的句子

	/*初始化栈及指针*/
	Stack[0].first = 0;
	int gra_index = -2;//使用到的文法规则下标，为-2表示空
	ana_s_LR.push_back("$");
	vector<string>::iterator it = ana_s_LR.begin();//遍历待分析句子成分的迭代器
	string token_switch = tokenToTerm(*it);//将句子成分转化为文法中的非终结符
	string action = ACTION[make_pair(Stack[ip].first, token_switch)].first;//当前采取的分析动作
	int next_state = ACTION[make_pair(Stack[ip].first, token_switch)].second;//移进的项目集ID或规约使用产生式的下标

	cout << setw(40) << "栈" << setw(20) << "输入" << setw(20) << "分析动作" << endl;

	while (true)
	{
		/*pair<int, string> temp_S[100];
		for (int i = 0; i <= ip; i++)
			temp_S[i] = Stack[i];*/
		vector<string>::iterator it_copy{ it };
		string buf = "";
		while (it_copy != ana_s_LR.end())
		{
			buf += *it_copy;
			it_copy++;
		}
		
		printAnaStepLR(Stack, ip, buf, gra_index,action,next_state);
		///*拷贝未修改的栈*/
		//for (int i = 0; i <= ip; i++)
		//	temp_S[i] = Stack[i];

		//gra_index = -2;
		string token_switch = tokenToTerm(*it);//单词转换为非终结符
		
		if (action == "s")
		{
			ip++;
			Stack[ip].first = next_state;
			Stack[ip].second = *it;
			it++;
		}
		else if (action == "r")
		{
			/*栈顶弹出符号*/
			vector<string> gra = grammar[next_state].right;
			ip -= gra.size();
			ip++;
			Stack[ip].second = grammar[next_state].left;
			Stack[ip].first=GOTO[make_pair(Stack[ip-1].first,Stack[ip].second)];
		}
		else if (action == "acc")
		{
			cout << endl<<"+-------------------------------------------------------------+\n";
			return;
		}
		else
		{
			cout << "语法错误" << endl;
			it++;
			continue;
		}

		token_switch = tokenToTerm(*it);
		action = ACTION[make_pair(Stack[ip].first, token_switch)].first;
		next_state = ACTION[make_pair(Stack[ip].first, token_switch)].second;
		gra_index = next_state;
	}
}

int main()
{
	/*读入文件、进行词法分析*/
	wordAna();

	/*自顶向下分析*/
	scan_grammer("LL.txt");
	generate_FirstAndFollow(grammar[0].left);
	analysisTable_1 = getTopToBottomTable();
	display_Table();
	analyzeLL();

	/*自底向上分析*/
	grammar_clear();
	scan_grammer("LR.txt");
	generate_FirstAndFollow(grammar[0].left);
	generateProjectSet();
	generateLR0Table();
	printLR0Table();
	analyzeLR();

	system("pause");
	return 0;
}