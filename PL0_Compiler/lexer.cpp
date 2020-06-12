#include "utilities.h"

symType SYM;		// 类型
string ID;			// 标识符名称
int NUM;			// 用户定义的数

string words = "";		// 从文件中读入的一个单词
string str;				// 从文件输入流中获取一行
int line = 0;			// 记录当前代码行数

vector<string> keyVec = { "var", "const", "call", "procedure", "begin", "end", "if", "then", "while", "do", "read", "write" };
vector<string> opeVec = { "+", "-", "*", "/", "=", "<>", "<", "<=", ">", ">=", ":=" };
vector<char> boundVec = { ',', ';', '.', '(', ')' };

// 通过map将类型直接映射到编码
map<string, symType> keyType = { {"var", symType::VARSYM}, {"const", symType::CONSTSYM}, {"call", symType::CALLSYM},
	{"procedure", symType::PROCEDURESYM}, {"begin", symType::BEGINSYM}, {"end", symType::ENDSYM}, {"if", symType::IFSYM},
	{"then", symType::THENSYM}, {"while", symType::WHILESYM}, {"do", symType::DOSYM}, {"read", symType::READSYM}, {"write", symType::WRITESYM} };
map<string, symType> opeType = { {"+", symType::PLUSSYM}, {"-", symType::MINUSSYM}, {"*", symType::MULTIPLYSM}, {"/", symType::DIVIDESYM},
	{"=", symType::EQUALSYM}, {"<>", symType::NOTEQUALSYM}, {"<", symType::LESSSYM}, {"<=", symType::LESSEQUALSYM},
	{">", symType::GREATER}, {">=", symType::GREATEREQUALSYM}, {":=", symType::ASSIGNSYM} };
map<char, symType> boundType = { {',', symType::COMMASYM}, {';', symType::SEMICOLONSYM}, {'.', symType::PERIODSYM},
	{'(', symType::LEFTPARENTHESIS}, {')', symType::RIGHTPARENTHESIS} };

void lexicalError()
{
	cout << "Lexical Error in line " << line << " with word \"" << words << "\"." << endl;
}

bool isKeywords(const string& s)		//关键字
{
	vector<string>::iterator result = find(keyVec.begin(), keyVec.end(), s);
	if (result != keyVec.end())
		return true;
	else
		return false;
}

bool isOperator(const string& s)		//运算符
{
	vector<string>::iterator result = find(opeVec.begin(), opeVec.end(), s);
	if (result != opeVec.end())
		return true;
	else
		return false;
}

bool isBound(const char& s)				//界符
{
	vector<char>::iterator result = find(boundVec.begin(), boundVec.end(), s);
	if (result != boundVec.end())
		return true;
	else
		return false;
}

void GETSYM()
{
	if (words.empty()) {
		if (!(strCin >> words)) {
			strCin.clear();
			while (getline(inFile, str)) {
				line++;
				if (!str.empty())
					break;
			}
			strCin.str(str);
			strCin >> words;
		}
	}

	//关键字肯定是单独作为一个单词的
	if (isKeywords(words))
	{
		SYM = keyType[words];
		cout << "Line " << line << ": (" << SYM << ", " << words << ")" << endl;
		words = "";
		return;
	}

	//非关键字：关键字，标识符，运算符，界符等混合在一起的单词
	//修改为每次只处理一个token
	int i = 0;
	//判断是否是关键字+界符的形式，如"write("
	if (words[i] >= 'a' && words[i] <= 'z') {
		int len = 1;
		while (words[len] >= 'a' && words[len] <= 'z')
			len++;
		if (isKeywords(words.substr(0, len))) {
			if (isBound(words[len])) {
				SYM = keyType[words.substr(0, len)];
				cout << "Line " << line << ": (" << SYM << ", " << words.substr(0, len) << ")" << endl;
				words = words.substr(len);
				return;
			}
			else {
				lexicalError();
				return;
			}
		}
	}
	//判断是否是两位的运算符，比如"<=", ">=", "<>", ":="
	if ((i < words.length() - 1) && isOperator(words.substr(i, 2))) {
		SYM = opeType[words.substr(i, 2)];
		cout << "Line " << line << ": (" << SYM << ", " << words.substr(i, 2) << ")" << endl;
		words = words.substr(2);
	}
	//判断是否是一位的运算符，比如+, -等
	else if (isOperator(words.substr(i, 1))) {
		SYM = opeType[words.substr(i, 1)];
		cout << "Line " << line << ": (" << SYM << ", " << words[i] << ")" << endl;
		words = words.substr(1);
	}
	//判断是否是界符
	else if (isBound(words[i])) {
		SYM = boundType[words[i]];
		cout << "Line " << line << ": (" << SYM << ", " << words[i] << ")" << endl;
		words = words.substr(1);
	}
	//判断是否是数字
	else if (words[i] >= '0' && words[i] <= '9') {
		int len = 1;
		while (i + 1 < words.length() && words[i + 1] >= '0' && words[i + 1] <= '9') {
			len++;
			i++;
		}
		if (i + 1 < words.length()) {
			char next = words[i + 1];
			if ((next >= 'a' && next <= 'z') || next == ':' || next == '(') {	//数字后面不可能跟字母、赋值号:=、左括号(
				lexicalError();
				return;
			}
		}
		SYM = NUMBER;
		NUM = stoi(words.substr(i - len + 1, len));
		cout << "Line " << line << ": (" << SYM << ", " << words.substr(i - len + 1, len) << ")" << endl;
		words = words.substr(i + 1);
	}
	//判断是否是标识符
	else if (words[i] >= 'a' && words[i] <= 'z') {
		int len = 1;
		while (i + 1 < words.length() && ((words[i + 1] >= '0' && words[i + 1] <= '9') || (words[i + 1] >= 'a' && words[i + 1] <= 'z'))) {
			len++;
			i++;
		}

		ID = words.substr(i - len + 1, len);
		SYM = IDENT;
		cout << "Line " << line << ": (" << SYM << ", " << ID << ")" << endl;
		words = words.substr(i + 1);
	}
}

