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

/*		���� �ʷ����� ����		*/
extern ifstream inFile;			// �ļ�������
extern istringstream strCin;	// ������
void lexicalError();			// ���������Ϣ

bool isKeywords(const string& s);		//�ؼ���
bool isOperator(const string& s);		//�����
bool isBound(const char& s);			//���
void GETSYM();

enum symType {
	ERROR = 0,		// ��������
	IDENT = 1,		// ��ʶ��
	NUMBER = 5,		// ����
	VARSYM = 10, CONSTSYM, CALLSYM, PROCEDURESYM, BEGINSYM, ENDSYM, IFSYM, THENSYM, WHILESYM, DOSYM, READSYM, WRITESYM,	// �ؼ���
	PLUSSYM = 30, MINUSSYM, MULTIPLYSM, DIVIDESYM, EQUALSYM, NOTEQUALSYM, LESSSYM, LESSEQUALSYM, GREATER, GREATEREQUALSYM, ASSIGNSYM,	// ���
	COMMASYM = 50, SEMICOLONSYM, PERIODSYM, LEFTPARENTHESIS, RIGHTPARENTHESIS		// ���
};


/*		���� �﷨���� ����		*/
extern symType SYM;		// ����
extern string ID;		// ��ʶ������
extern int NUM;			// �û��������

// ���ű������
struct sym {
	string name;	// ����
	int type;		// ����
	int val;		// ��ʶ����ֵ����̵���ڵ�ַ
	int level;		// ���
	int address;	// ��ַ
	int size;		// ����ʶ���ǹ�����ʱ����¼�βθ���
};

void enterTable(string name, int type, int val, int level, int dx);		// ����ű�
int position(string name);		// �ڷ��ű�symTable�в��ұ�ʶ����λ��
int searchproc();		// ���ҷ��ű��ж���Ϊprocedure�ı�ʶ��
bool existInCurLevel(string name, int level);		// ��ͬһ���в��ұ�ʶ���Ƿ���
bool existInOuterLevel(string name, int level);		// ��ͬ��������Ҳ��ұ�ʶ���Ƿ���

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

struct treeNode {		// �﷨���ڵ�
	treeNode(string nam = "") {
		name = nam;
		children.clear();
	}
	string name;
	vector<treeNode*> children;
};
void displayTree();


/*		���� Ŀ�������� ����		*/
// a���ȡֵ������
// 0�����̷��ػ�������
// 1��ȡ����
// 2���ӷ�
// 3������
// 4���˷�
// 5������
// 6���ж����
// 7���жϲ���
// 8���ж�С��
// 9���жϲ�С��
// 10���жϴ���
// 11���жϲ�����
// 12�����ջ������
// 13���������
// 14������

struct codeType {	// ���ɵ�Ŀ���������
	int f;			// ������
	int l;			// ��β�
	int a;			// λ����
};
//extern codeType CODE[1000];
extern codeType* CODE;
void gen(int f, int l, int a);	// �м�������ɺ���
void displayCode();		// ���CODE�����е��м����


/*		���� ����ִ�� ����		*/
void Interpreter();

#endif