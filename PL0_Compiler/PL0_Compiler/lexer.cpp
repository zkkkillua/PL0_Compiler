#include "utilities.h"

symType SYM;		// ����
string ID;			// ��ʶ������
int NUM;			// �û��������

string words = "";		// ���ļ��ж����һ������
string str;				// ���ļ��������л�ȡһ��
int line = 0;			// ��¼��ǰ��������

vector<string> keyVec = { "var", "const", "call", "procedure", "begin", "end", "if", "then", "while", "do", "read", "write" };
vector<string> opeVec = { "+", "-", "*", "/", "=", "<>", "<", "<=", ">", ">=", ":=" };
vector<char> boundVec = { ',', ';', '.', '(', ')' };

// ͨ��map������ֱ��ӳ�䵽����
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

bool isKeywords(const string& s)		//�ؼ���
{
	vector<string>::iterator result = find(keyVec.begin(), keyVec.end(), s);
	if (result != keyVec.end())
		return true;
	else
		return false;
}

bool isOperator(const string& s)		//�����
{
	vector<string>::iterator result = find(opeVec.begin(), opeVec.end(), s);
	if (result != opeVec.end())
		return true;
	else
		return false;
}

bool isBound(const char& s)				//���
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

	//�ؼ��ֿ϶��ǵ�����Ϊһ�����ʵ�
	if (isKeywords(words))
	{
		SYM = keyType[words];
		cout << "Line " << line << ": (" << SYM << ", " << words << ")" << endl;
		words = "";
		return;
	}

	//�ǹؼ��֣��ؼ��֣���ʶ���������������Ȼ����һ��ĵ���
	//�޸�Ϊÿ��ֻ����һ��token
	int i = 0;
	//�ж��Ƿ��ǹؼ���+�������ʽ����"write("
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
	//�ж��Ƿ�����λ�������������"<=", ">=", "<>", ":="
	if ((i < words.length() - 1) && isOperator(words.substr(i, 2))) {
		SYM = opeType[words.substr(i, 2)];
		cout << "Line " << line << ": (" << SYM << ", " << words.substr(i, 2) << ")" << endl;
		words = words.substr(2);
	}
	//�ж��Ƿ���һλ�������������+, -��
	else if (isOperator(words.substr(i, 1))) {
		SYM = opeType[words.substr(i, 1)];
		cout << "Line " << line << ": (" << SYM << ", " << words[i] << ")" << endl;
		words = words.substr(1);
	}
	//�ж��Ƿ��ǽ��
	else if (isBound(words[i])) {
		SYM = boundType[words[i]];
		cout << "Line " << line << ": (" << SYM << ", " << words[i] << ")" << endl;
		words = words.substr(1);
	}
	//�ж��Ƿ�������
	else if (words[i] >= '0' && words[i] <= '9') {
		int len = 1;
		while (i + 1 < words.length() && words[i + 1] >= '0' && words[i + 1] <= '9') {
			len++;
			i++;
		}
		if (i + 1 < words.length()) {
			char next = words[i + 1];
			if ((next >= 'a' && next <= 'z') || next == ':' || next == '(') {	//���ֺ��治���ܸ���ĸ����ֵ��:=��������(
				lexicalError();
				return;
			}
		}
		SYM = NUMBER;
		NUM = stoi(words.substr(i - len + 1, len));
		cout << "Line " << line << ": (" << SYM << ", " << words.substr(i - len + 1, len) << ")" << endl;
		words = words.substr(i + 1);
	}
	//�ж��Ƿ��Ǳ�ʶ��
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

