#ifndef ____UTILITIES_H____
#define ____UTILITIES_H____

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

/*		—— 词法分析 ——		*/
extern ifstream inFile;			// 文件输入流
extern istringstream strCin;	// 输入流
void lexicalError();			// 输出错误信息

bool isKeywords(const string& s);		//关键字
bool isOperator(const string& s);		//运算符
bool isBound(const char& s);			//界符
void GETSYM();

enum symType {
	ERROR = 0,		// 错误类型
	IDENT = 1,		// 标识符
	NUMBER = 5,		// 数字
	VARSYM = 10, CONSTSYM, CALLSYM, PROCEDURESYM, BEGINSYM, ENDSYM, IFSYM, THENSYM, WHILESYM, DOSYM, READSYM, WRITESYM,	// 关键字
	PLUSSYM = 30, MINUSSYM, MULTIPLYSM, DIVIDESYM, EQUALSYM, NOTEQUALSYM, LESSSYM, LESSEQUALSYM, GREATER, GREATEREQUALSYM, ASSIGNSYM,	// 算符
	COMMASYM = 50, SEMICOLONSYM, PERIODSYM, LEFTPARENTHESIS, RIGHTPARENTHESIS		// 界符
};


/*		—— 语法分析 ——		*/
extern symType SYM;		// 类型
extern string ID;		// 标识符名称
extern int NUM;			// 用户定义的数

// 符号表的类型
struct sym {
	string name;	// 名字
	int type;		// 类型
	int val;		// 标识符的值或过程的入口地址
	int level;		// 层次
	int address;	// 地址
	int size;		// 当标识符是过程名时，记录形参个数
};

void enterTable(string name, int type, int val, int level, int dx);		// 填符号表
int position(string name);		// 在符号表symTable中查找标识符的位置
int searchproc();		// 查找符号表中定义为procedure的标识符
bool existInCurLevel(string name, int level);		// 在同一层中查找标识符是否定义
bool existInOuterLevel(string name, int level);		// 在同层和外层查找查找标识符是否定义

void P();			// P -> SP.
void SP();			// SP -> [CSTAT][V][PROSTAT]ST
void CSTAT();		// CSTAT -> const CDEF{,CDEF};
void CDEF();		// CDEF -> IDF=UN
void V();			// var IDF{,IDF};
void PROSTAT();		// PROSTAT -> PROHEAD SP;{PROSTAT}
void PROHEAD();		// procedure IDF;
void ST();			// ST -> AS|CONDSTAT|LOOP|CALL|R|W|COM|NU
void AS();			// AS -> IDF:=EXP
void COM();			// COM -> begin ST{;ST}end
void COND();		// COND -> EXP REL EXP
void EXP();			// EXP -> [+|-]T{PM T}
void T();			// T -> F{MD F}
void F();			// F -> IDF|UN|(EXP)
symType REL();		// REL -> =|<>|<|<=|>|>=
void CONDSTAT();	// CONDSTAT -> if COND then ST
void CALL();		// CALL -> call IDF
void LOOP();		// LOOP -> while COND do ST
void R();			// R -> read(IDF{,IDF})
void W();			// W -> write(EXP{,EXP})

struct treeNode {		// 语法树节点
	treeNode(string nam = "") {
		name = nam;
		children.clear();
	}
	string name;
	vector<treeNode*> children;
};
void displayTree();


/*		—— 目标代码产生 ——		*/
// a域的取值及含义
// 0：过程返回或程序结束
// 1：取负数
// 2：加法
// 3：减法
// 4：乘法
// 5：除法
// 6：判断相等
// 7：判断不等
// 8：判断小于
// 9：判断不小于
// 10：判断大于
// 11：判断不大于
// 12：输出栈顶内容
// 13：输出换行
// 14：读入

struct codeType {	// 生成的目标代码类型
	int f;			// 功能码
	int l;			// 层次差
	int a;			// 位移量
};
//extern codeType CODE[1000];
extern codeType* CODE;
void gen(int f, int l, int a);	// 中间代码生成函数
void displayCode();		// 输出CODE数组中的中间代码


/*		—— 解释执行 ——		*/
void Interpreter();

#endif