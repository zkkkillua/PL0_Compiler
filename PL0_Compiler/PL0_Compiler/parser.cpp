#include "utilities.h"
#include <queue>

codeType* CODE = new codeType[1000];

// 符号表中的类型
const int CONSTANT = 0;		// 常量
const int VARIABLE = 1;		// 变量
const int PROCEDURE = 2;	// 过程

sym symTable[1000];	// 建立一个一维数组记录syms
int tx = 0;			// symTable的指针
int dx;				// 每层的局部变量的相对地址
int lev;

treeNode* root;
treeNode* cur;

const int LIT = 0;		// 将常数放到运栈顶，a域为常数。
const int LOD = 1;		// 将变量放到栈顶。a域为变量所在说明层中的相对位置，l为调用层与说明层的层差值。
const int STO = 2;		// 将栈顶的内容送到某变量单元中。a,l域的含义与LOD的相同。
const int CAL = 3;		// 调用过程的指令。a为被调用过程的目标程序的入口地址，l为层差。
const int INT = 4;		// 为被调用的过程或主程序在运行栈中开辟数据区。a域为开辟的个数。
const int JMP = 5;		// 无条件转移指令，a为转向地址。
const int JPC = 6;		// 条件转移指令，当栈顶的布尔值为非真时，转向a域的地址，否则顺序执行。
const int OPR = 7;		// 关系和算术运算。具体操作由a域给出，运算对象为栈顶和次栈顶的内容，结果放在次栈顶。a域为0时退出数据区。

int cx;			// CODE数组中的当前下标

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
			if (symTable[i].level == level)		//同一层
				return true;
			else {
				do {
					level = level - 1;               //查找外层的
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

void gen(int f, int l, int a)	// 中间代码生成函数
{
	CODE[cx].f = f;
	CODE[cx].l = l;
	CODE[cx].a = a;
	cx++;
}

const string midOpes[8] = { "LIT","LOD","STO","CAL","INT","JMP","JPC","OPR" };
void displayCode()				// 输出CODE数组中的中间代码
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
		cout << "词法分析结束." << endl;
		cout << "语法分析结束." << endl;
	}
	else
		cout << "Error in P." << endl;
}

// SP -> [CSTAT][V][PROSTAT]ST
void SP()
{
	treeNode* curNode = cur->children.back();
	cur = curNode;
	int dx0 = dx;			//记录当前的层次，以便恢复时使用
	int tx0 = tx + 1;		//记录符号表当前等待填入的位置
	int n = 0;
	if (tx0 > 1) {
		n = searchproc();				//寻找过程标识符
		if (n != -1)
			tx0 = tx0 - symTable[n].size;
	}
	if (tx0 == 1) {
		dx = 3;							//静态链、动态链、返回地址
	}
	else {
		dx = 3 + symTable[n].size;		//3+形参的个数，形参置于静态链、动态链、返回地址和变量、常量之前
	}
	int cx0 = cx;						//记录跳转指令的位置，等待下次回填
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
		lev--;          //出嵌套，层次-1
	}
	if (tx0 > 1) {
		int i;
		n = searchproc();
		for (i = 0; i < symTable[n].size; i++) {
			gen(STO, 0, symTable[n].size + 3 - 1 - i);   //将实参的值传给形参
		}
	}

	CODE[cx0].a = cx;		//回填JMP指令
	gen(INT, 0, dx);		//开辟dx个空间
	if (tx != 1)
		symTable[n].val = cx - 1 - symTable[n].size;    //将过程入口地址填入过程定义的value，以便call指令使用
	curNode->children.push_back(new treeNode("ST"));
	ST();
	cur = curNode;
	gen(OPR, 0, 0);
	tx = tx0;				//清符号表，将符号表指针往前移至tx0处
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
	else		// NU 空语句
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
			int cx1 = cx;		// 记录回填地址
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
			gen(JMP, 0, cx1);		// 每次循环后无条件转移到开始位置
			CODE[cx2].a = cx;		// 非真时跳转到该结束位置
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
						gen(OPR, 0, 14);	// 读入数据
						gen(STO, lev - symTable[i].level, symTable[i].address);		// 数据存入变量
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
								gen(OPR, 0, 14);	// 读入数据
								gen(STO, lev - symTable[i].level, symTable[i].address);		// 数据存入变量
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

