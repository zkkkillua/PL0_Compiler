#include "utilities.h"
#include <queue>

codeType* CODE = new codeType[1000];

// ���ű��е�����
const int CONSTANT = 0;		// ����
const int VARIABLE = 1;		// ����
const int PROCEDURE = 2;	// ����

sym symTable[1000];	// ����һ��һά�����¼syms
int tx = 0;			// symTable��ָ��
int dx;				// ÿ��ľֲ���������Ե�ַ
int lev;

treeNode* root;
treeNode* cur;

const int LIT = 0;		// �������ŵ���ջ����a��Ϊ������
const int LOD = 1;		// �������ŵ�ջ����a��Ϊ��������˵�����е����λ�ã�lΪ���ò���˵����Ĳ��ֵ��
const int STO = 2;		// ��ջ���������͵�ĳ������Ԫ�С�a,l��ĺ�����LOD����ͬ��
const int CAL = 3;		// ���ù��̵�ָ�aΪ�����ù��̵�Ŀ��������ڵ�ַ��lΪ��
const int INT = 4;		// Ϊ�����õĹ��̻�������������ջ�п�����������a��Ϊ���ٵĸ�����
const int JMP = 5;		// ������ת��ָ�aΪת���ַ��
const int JPC = 6;		// ����ת��ָ���ջ���Ĳ���ֵΪ����ʱ��ת��a��ĵ�ַ������˳��ִ�С�
const int OPR = 7;		// ��ϵ���������㡣���������a��������������Ϊջ���ʹ�ջ�������ݣ�������ڴ�ջ����a��Ϊ0ʱ�˳���������

int cx;			// CODE�����еĵ�ǰ�±�

void enterTable(string name, int type, int val, int level, int dx)
{
	tx++;
	symTable[tx].name = name;
	symTable[tx].type = type;
	symTable[tx].val = val;
	symTable[tx].level = level;
	symTable[tx].address = dx;
}

int position(string name)
{
	int i;
	symTable[0].name = name;
	i = tx;
	while (symTable[i].name != name)
		i--;
	return i;
}

int searchproc()
{
	for (int i = tx; i >= 1; i--)
		if (symTable[i].type == PROCEDURE)
			return i;
	return -1;
}

bool existInCurLevel(string name, int level)
{
	for (int i = 1; i <= tx; i++)
		if (symTable[i].name == name && symTable[i].level == level)
			return true;
	return false;
}

bool existInOuterLevel(string name, int level)
{
	for (int i = 1; i <= tx; i++) {
		if (symTable[i].name == name) {
			if (symTable[i].level == level)		//ͬһ��
				return true;
			else {
				do {
					level = level - 1;               //��������
					if (symTable[i].level == level)
						return true;
				} while (level >= 0);
			}
		}
	}
	return false;
}

void displayTree()
{
	int layer = 0;
	queue<treeNode*> q;
	q.push(root);
	while (!q.empty()) {
		int sz = q.size();
		cout << "Layer " << layer << ": ";
		while (sz--) {
			treeNode* temp = q.front();
			q.pop();
			cout << temp->name << ' ';
			for (int i = 0; i < temp->children.size(); i++)
				q.push(temp->children[i]);
		}
		cout << endl;
		layer++;
	}
}

void gen(int f, int l, int a)	// �м�������ɺ���
{
	CODE[cx].f = f;
	CODE[cx].l = l;
	CODE[cx].a = a;
	cx++;
}

const string midOpes[8] = { "LIT","LOD","STO","CAL","INT","JMP","JPC","OPR" };
void displayCode()				// ���CODE�����е��м����
{
	for (int i = 0; i < cx; i++)
		cout << i << " " << midOpes[CODE[i].f] << " " << CODE[i].l << " " << CODE[i].a << endl;
}

// P -> SP.
void P()
{
	root = new treeNode("P");
	cur = root;
	GETSYM();
	cur->children.push_back(new treeNode("SP"));
	SP();
	cur = root;
	if (SYM == PERIODSYM) {
		cur->children.push_back(new treeNode("."));
		cout << "�ʷ���������." << endl;
		cout << "�﷨��������." << endl;
	}
	else
		cout << "Error in P." << endl;
}

// SP -> [CSTAT][V][PROSTAT]ST
void SP()
{
	treeNode* curNode = cur->children.back();
	cur = curNode;
	int dx0 = dx;			//��¼��ǰ�Ĳ�Σ��Ա�ָ�ʱʹ��
	int tx0 = tx + 1;		//��¼���ű�ǰ�ȴ������λ��
	int n = 0;
	if (tx0 > 1) {
		n = searchproc();				//Ѱ�ҹ��̱�ʶ��
		if (n != -1)
			tx0 = tx0 - symTable[n].size;
	}
	if (tx0 == 1) {
		dx = 3;							//��̬������̬�������ص�ַ
	}
	else {
		dx = 3 + symTable[n].size;		//3+�βεĸ������β����ھ�̬������̬�������ص�ַ�ͱ���������֮ǰ
	}
	int cx0 = cx;						//��¼��תָ���λ�ã��ȴ��´λ���
	gen(JMP, 0, 0);

	if (SYM != CONSTSYM && SYM != VARSYM && SYM != PROCEDURESYM && SYM != BEGINSYM) {
		cout << "Error in SP." << endl;
		return;
	}
	if (SYM == CONSTSYM) {
		curNode->children.push_back(new treeNode("CSTAT"));
		CSTAT();
		cur = curNode;
	}
	if (SYM == VARSYM) {
		curNode->children.push_back(new treeNode("V"));
		V();
	}
	if (SYM == PROCEDURESYM) {
		curNode->children.push_back(new treeNode("PROSTAT"));
		PROSTAT();
		cur = curNode;
		lev--;          //��Ƕ�ף����-1
	}
	if (tx0 > 1) {
		int i;
		n = searchproc();
		for (i = 0; i < symTable[n].size; i++) {
			gen(STO, 0, symTable[n].size + 3 - 1 - i);   //��ʵ�ε�ֵ�����β�
		}
	}

	CODE[cx0].a = cx;		//����JMPָ��
	gen(INT, 0, dx);		//����dx���ռ�
	if (tx != 1)
		symTable[n].val = cx - 1 - symTable[n].size;    //��������ڵ�ַ������̶����value���Ա�callָ��ʹ��
	curNode->children.push_back(new treeNode("ST"));
	ST();
	cur = curNode;
	gen(OPR, 0, 0);
	tx = tx0;				//����ű������ű�ָ����ǰ����tx0��
	dx = dx0;
}

// CSTAT -> const CDEF{,CDEF};
void CSTAT()
{
	treeNode* curNode = cur->children.back();
	cur = curNode;
	if (SYM == CONSTSYM) {
		curNode->children.push_back(new treeNode("const"));
		GETSYM();
		curNode->children.push_back(new treeNode("CDEF"));
		CDEF();
		while (SYM == COMMASYM) {
			curNode->children.push_back(new treeNode(","));
			GETSYM();
			curNode->children.push_back(new treeNode("CDEF"));
			CDEF();
		}
		if (SYM == SEMICOLONSYM) {
			curNode->children.push_back(new treeNode(";"));
			GETSYM();
		}
		else
			cout << "Error in CSTAT." << endl;
	}
	else
		cout << "Error in CSTAT." << endl;
}

// CDEF -> IDF=UN
void CDEF()
{
	treeNode* curNode = cur->children.back();
	string name;
	int value;
	if (SYM == IDENT) {
		name = ID;
		curNode->children.push_back(new treeNode(name));
		GETSYM();
		if (SYM == EQUALSYM) {
			curNode->children.push_back(new treeNode("="));
			GETSYM();
			if (SYM == NUMBER) {
				value = NUM;
				curNode->children.push_back(new treeNode(to_string(value)));
				if (existInCurLevel(name, lev))
					cout << "Error in CDEF: duplicate definition." << endl;
				enterTable(name, CONSTANT, value, lev, dx);
				GETSYM();
			}
		}
		else
			cout << "Error in CDEF." << endl;
	}
	else
		cout << "Error in CDEF." << endl;
}

// var IDF{,IDF};
void V()
{
	treeNode* curNode = cur->children.back();
	string name;
	if (SYM == VARSYM) {
		curNode->children.push_back(new treeNode("var"));
		GETSYM();
		if (SYM == IDENT) {
			name = ID;
			curNode->children.push_back(new treeNode(name));
			if (existInCurLevel(name, lev))
				cout << "Error in V: duplicate definition." << endl;
			enterTable(name, VARIABLE, 0, lev, dx);
			dx++;
			GETSYM();
			while (SYM == COMMASYM) {
				curNode->children.push_back(new treeNode(","));
				GETSYM();
				if (SYM == IDENT) {
					name = ID;
					curNode->children.push_back(new treeNode(name));
					if (existInCurLevel(name, lev))
						cout << "Error in V: duplicate definition." << endl;
					enterTable(name, VARIABLE, 0, lev, dx);
					dx++;
					GETSYM();
				}
				else
					cout << "Error in V." << endl;
			}
			if (SYM != SEMICOLONSYM)
				cout << "Error in V." << endl;
			else {
				curNode->children.push_back(new treeNode(";"));
				GETSYM();
			}
		}
		else
			cout << "Error in V." << endl;
	}
	else
		cout << "Error in V." << endl;
}

// PROSTAT -> PROHEAD SP;{PROSTAT}
void PROSTAT()
{
	treeNode* curNode = cur->children.back();
	cur = curNode;
	curNode->children.push_back(new treeNode("PROHEAD"));
	PROHEAD();
	curNode->children.push_back(new treeNode("SP"));
	SP();
	cur = curNode;
	if (SYM == SEMICOLONSYM) {
		curNode->children.push_back(new treeNode(";"));
		GETSYM();
		if (SYM == PROCEDURESYM) {
			lev--;
			curNode->children.push_back(new treeNode("PROSTAT"));
			PROSTAT();
			cur = curNode;
		}
		else
			return;
	}
	else
		cout << "Error in PROSTAT." << endl;
}

// procedure IDF;
void PROHEAD()
{
	treeNode* curNode = cur->children.back();
	if (SYM == PROCEDURESYM) {
		curNode->children.push_back(new treeNode("procedure"));
		GETSYM();
		if (SYM == IDENT) {
			string name = ID;
			curNode->children.push_back(new treeNode(name));
			if (existInCurLevel(name, lev))
				cout << "Error in PROHEAD: duplicate definition." << endl;
			enterTable(name, PROCEDURE, 0, lev, dx);
			lev++;
			GETSYM();
			if (SYM == SEMICOLONSYM) {
				curNode->children.push_back(new treeNode(";"));
				GETSYM();
			}
			else
				cout << "Error in PROHEAD." << endl;
		}
		else
			cout << "Error in PROHEAD." << endl;
	}
	else
		cout << "Error in PROHEAD." << endl;
}

// ST -> AS|CONDSTAT|LOOP|CALL|R|W|COM|NU
void ST()
{
	treeNode* curNode = cur->children.back();
	cur = curNode;
	if (SYM == IDENT) {
		curNode->children.push_back(new treeNode("AS"));
		AS();
		cur = curNode;
	}
	else if (SYM == IFSYM) {
		curNode->children.push_back(new treeNode("CONDSTAT"));
		CONDSTAT();
		cur = curNode;
	}
	else if (SYM == WHILESYM) {
		curNode->children.push_back(new treeNode("LOOP"));
		LOOP();
		cur = curNode;
	}
	else if (SYM == CALLSYM) {
		curNode->children.push_back(new treeNode("CALL"));
		CALL();
	}
	else if (SYM == READSYM) {
		curNode->children.push_back(new treeNode("R"));
		R();
	}
	else if (SYM == WRITESYM) {
		curNode->children.push_back(new treeNode("W"));
		W();
		cur = curNode;
	}
	else if (SYM == BEGINSYM) {
		curNode->children.push_back(new treeNode("COM"));
		COM();
		cur = curNode;
	}
	else		// NU �����
		return;
}

// AS -> IDF:=EXP
void AS()
{
	treeNode* curNode = cur->children.back();
	cur = curNode;
	if (SYM == IDENT) {
		string name = ID;
		curNode->children.push_back(new treeNode(name));
		GETSYM();
		if (SYM == ASSIGNSYM) {
			curNode->children.push_back(new treeNode(":="));
			GETSYM();
			curNode->children.push_back(new treeNode("EXP"));
			EXP();
			cur = curNode;
			if (!existInOuterLevel(name, lev))
				cout << "Error in AS: variable not defined." << endl;
			else {
				int i = position(name);
				if (i > 0 && symTable[i].type == VARIABLE)
					gen(STO, lev - symTable[i].level, symTable[i].address);
				else
					cout << "Error in AS: variable not found." << endl;
			}
		}
		else
			cout << "Error in AS." << endl;
	}
	else
		cout << "Error in AS." << endl;
}

// COM -> begin ST{;ST}end
void COM()
{
	treeNode* curNode = cur->children.back();
	cur = curNode;
	if (SYM == BEGINSYM) {
		curNode->children.push_back(new treeNode("begin"));
		GETSYM();
		curNode->children.push_back(new treeNode("ST"));
		ST();
		cur = curNode;
		while (SYM == SEMICOLONSYM) {
			curNode->children.push_back(new treeNode(";"));
			GETSYM();
			curNode->children.push_back(new treeNode("ST"));
			ST();
			cur = curNode;
		}
		if (SYM == ENDSYM) {
			curNode->children.push_back(new treeNode("end"));
			GETSYM();
		}
		else
			cout << "Error in COM." << endl;
	}
	else
		cout << "Error in COM." << endl;
}

// COND -> EXP REL EXP
void COND()
{
	treeNode* curNode = cur->children.back();
	cur = curNode;
	curNode->children.push_back(new treeNode("EXP"));
	EXP();
	cur = curNode;
	curNode->children.push_back(new treeNode("REL"));
	symType i = REL();
	curNode->children.push_back(new treeNode("EXP"));
	EXP();
	cur = curNode;
	if (i == EQUALSYM)
		gen(OPR, 0, 6);
	else if (i == NOTEQUALSYM)
		gen(OPR, 0, 7);
	else if (i == LESSSYM)
		gen(OPR, 0, 8);
	else if (i == GREATEREQUALSYM)
		gen(OPR, 0, 9);
	else if (i == GREATER)
		gen(OPR, 0, 10);
	else if (i == LESSEQUALSYM)
		gen(OPR, 0, 11);
}

// EXP -> [+|-]T{PM T}
void EXP()
{
	treeNode* curNode = cur->children.back();
	cur = curNode;
	symType temp = SYM;
	if (SYM == PLUSSYM || SYM == MINUSSYM) {
		if (SYM == PLUSSYM)
			curNode->children.push_back(new treeNode("+"));
		else
			curNode->children.push_back(new treeNode("-"));
		GETSYM();
	}
	curNode->children.push_back(new treeNode("T"));
	T();
	cur = curNode;
	if (temp == MINUSSYM)
		gen(OPR, 0, 1);
	while (SYM == PLUSSYM || SYM == MINUSSYM) {
		if (SYM == PLUSSYM)
			curNode->children.push_back(new treeNode("+"));
		else
			curNode->children.push_back(new treeNode("-"));
		temp = SYM;
		GETSYM();
		curNode->children.push_back(new treeNode("T"));
		T();
		cur = curNode;
		if (temp == PLUSSYM)
			gen(OPR, 0, 2);
		else if (temp == MINUSSYM)
			gen(OPR, 0, 3);
	}
}

// T -> F{MD F}
void T()
{
	treeNode* curNode = cur->children.back();
	cur = curNode;
	curNode->children.push_back(new treeNode("F"));
	F();
	cur = curNode;
	while (SYM == MULTIPLYSM || SYM == DIVIDESYM) {
		if (SYM == MULTIPLYSM)
			curNode->children.push_back(new treeNode("*"));
		else
			curNode->children.push_back(new treeNode("/"));
		symType  temp = SYM;
		GETSYM();
		curNode->children.push_back(new treeNode("F"));
		F();
		cur = curNode;
		if (temp == MULTIPLYSM)
			gen(OPR, 0, 4);
		else if (temp == DIVIDESYM)
			gen(OPR, 0, 5);
	}
}

// F -> IDF|UN|(EXP)
void F()
{
	treeNode* curNode = cur->children.back();
	cur = curNode;
	if (SYM == IDENT) {
		string name = ID;
		curNode->children.push_back(new treeNode(name));
		if (!existInOuterLevel(name, lev))
			cout << "Error in F: variable not defined." << endl;
		else {
			int i = position(name);
			if (symTable[i].type == VARIABLE)
				gen(LOD, lev - symTable[i].level, symTable[i].address);
			else if (symTable[i].type == CONSTANT)
				gen(LIT, 0, symTable[i].val);
			else
				cout << "Error in F." << endl;
		}
		GETSYM();
	}
	else if (SYM == NUMBER) {
		curNode->children.push_back(new treeNode(to_string(NUM)));
		gen(LIT, 0, NUM);
		GETSYM();
	}
	else if (SYM == LEFTPARENTHESIS) {
		curNode->children.push_back(new treeNode("("));
		GETSYM();
		curNode->children.push_back(new treeNode("EXP"));
		EXP();
		cur = curNode;
		if (SYM == RIGHTPARENTHESIS) {
			curNode->children.push_back(new treeNode(")"));
			GETSYM();
		}
		else
			cout << "Error in F." << endl;
	}
	else
		cout << "Error in F." << endl;
}

// REL -> =|<>|<|<=|>|>=
symType REL()
{
	treeNode* curNode = cur->children.back();
	if (SYM == EQUALSYM) {
		curNode->children.push_back(new treeNode("="));
		GETSYM();
		return EQUALSYM;
	}
	else if (SYM == NOTEQUALSYM) {
		curNode->children.push_back(new treeNode("<>"));
		GETSYM();
		return NOTEQUALSYM;
	}
	else if (SYM == LESSSYM) {
		curNode->children.push_back(new treeNode("<"));
		GETSYM();
		return LESSSYM;
	}
	else if (SYM == LESSEQUALSYM) {
		curNode->children.push_back(new treeNode("<="));
		GETSYM();
		return LESSEQUALSYM;
	}
	else if (SYM == GREATER) {
		curNode->children.push_back(new treeNode(">"));
		GETSYM();
		return GREATER;
	}
	else if (SYM == GREATEREQUALSYM) {
		curNode->children.push_back(new treeNode(">="));
		GETSYM();
		return GREATEREQUALSYM;
	}
	return ERROR;
}

// CONDSTAT -> if COND then ST
void CONDSTAT()
{
	treeNode* curNode = cur->children.back();
	cur = curNode;
	if (SYM == IFSYM) {
		curNode->children.push_back(new treeNode("if"));
		GETSYM();
		curNode->children.push_back(new treeNode("COND"));
		COND();
		cur = curNode;
		if (SYM == THENSYM) {
			curNode->children.push_back(new treeNode("then"));
			int cx1 = cx;		// ��¼�����ַ
			gen(JPC, 0, 0);
			GETSYM();
			curNode->children.push_back(new treeNode("ST"));
			ST();
			cur = curNode;
			CODE[cx1].a = cx;
		}
		else {
			cout << "Error in CONDSTAT." << endl;
		}
	}
	else
		cout << "Error in CONDSTAT." << endl;
}

// CALL -> call IDF
void CALL()
{
	treeNode* curNode = cur->children.back();
	if (SYM == CALLSYM) {
		curNode->children.push_back(new treeNode("call"));
		GETSYM();
		int count = 0, i;
		if (SYM == IDENT) {
			curNode->children.push_back(new treeNode(ID));
			if (!existInOuterLevel(ID, lev))
				cout << "Error in CALL: procedure not defined." << endl;
			else {
				i = position(ID);
				if (symTable[i].type == PROCEDURE) {
					gen(CAL, lev - symTable[i].level, symTable[i].val);
				}
				else
					cout << "Error in CALL." << endl;
			}
			GETSYM();
		}
		else
			cout << "Error in CALL." << endl;
	}
	else
		cout << "Error in CALL." << endl;
}

// LOOP -> while COND do ST
void LOOP()
{
	treeNode* curNode = cur->children.back();
	cur = curNode;
	if (SYM == WHILESYM) {
		curNode->children.push_back(new treeNode("while"));
		int cx1 = cx;
		GETSYM();
		curNode->children.push_back(new treeNode("COND"));
		COND();
		cur = curNode;
		if (SYM == DOSYM) {
			curNode->children.push_back(new treeNode("do"));
			int cx2 = cx;
			gen(JPC, 0, 0);
			GETSYM();
			curNode->children.push_back(new treeNode("ST"));
			ST();
			cur = curNode;
			gen(JMP, 0, cx1);		// ÿ��ѭ����������ת�Ƶ���ʼλ��
			CODE[cx2].a = cx;		// ����ʱ��ת���ý���λ��
		}
		else
			cout << "Error in LOOP." << endl;
	}
	else
		cout << "Error in LOOP." << endl;
}

// R -> read(IDF{,IDF})
void R()
{
	treeNode* curNode = cur->children.back();
	if (SYM == READSYM) {
		curNode->children.push_back(new treeNode("read"));
		GETSYM();
		if (SYM == LEFTPARENTHESIS) {
			curNode->children.push_back(new treeNode("("));
			GETSYM();
			if (SYM == IDENT) {
				curNode->children.push_back(new treeNode(ID));
				if (!existInOuterLevel(ID, lev))
					cout << "Error in R: variable not defined." << endl;
				else {
					int i = position(ID);
					if (symTable[i].type == VARIABLE) {
						gen(OPR, 0, 14);	// ��������
						gen(STO, lev - symTable[i].level, symTable[i].address);		// ���ݴ������
					}
					else
						cout << "Error in R." << endl;
				}
				GETSYM();
				while (SYM == COMMASYM) {
					curNode->children.push_back(new treeNode(","));
					GETSYM();
					if (SYM == IDENT) {
						curNode->children.push_back(new treeNode(ID));
						if (!existInOuterLevel(ID, lev))
							cout << "Error in R: variable not defined." << endl;
						else {
							int i = position(ID);
							if (symTable[i].type == VARIABLE) {
								gen(OPR, 0, 14);	// ��������
								gen(STO, lev - symTable[i].level, symTable[i].address);		// ���ݴ������
							}
							else
								cout << "Error in R." << endl;
						}
						GETSYM();
					}
					else
						cout << "Error in R." << endl;
				}
				if (SYM == RIGHTPARENTHESIS) {
					curNode->children.push_back(new treeNode(")"));
					GETSYM();
				}
				else
					cout << "Error in R." << endl;
			}
			else
				cout << "Error in R." << endl;
		}
		else
			cout << "Error in R." << endl;
	}
	else
		cout << "Error in R." << endl;
}

// W -> write(EXP{,EXP})
void W()
{
	treeNode* curNode = cur->children.back();
	cur = curNode;
	if (SYM == WRITESYM) {
		curNode->children.push_back(new treeNode("write"));
		GETSYM();
		if (SYM == LEFTPARENTHESIS) {
			curNode->children.push_back(new treeNode("("));
			GETSYM();
			curNode->children.push_back(new treeNode("EXP"));
			EXP();
			cur = curNode;
			gen(OPR, 0, 12);
			while (SYM == COMMASYM) {
				curNode->children.push_back(new treeNode(","));
				GETSYM();
				curNode->children.push_back(new treeNode("EXP"));
				EXP();
				cur = curNode;
				gen(OPR, 0, 12);
			}
			gen(OPR, 0, 13);
			if (SYM == RIGHTPARENTHESIS) {
				curNode->children.push_back(new treeNode(")"));
				GETSYM();
			}
			else
				cout << "Error in W." << endl;
		}
		else
			cout << "Error in W." << endl;
	}
	else
		cout << "Error in W." << endl;
}

