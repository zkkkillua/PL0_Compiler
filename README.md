# PL0_Compiler
编译原理与技术的课程实验，实现一个对PL/0语言的编译器。
  
本实验基于“编译原理实习题.pdf”的要求实现，并额外添加了语法树的相关实现。
以下内容几乎直接copy实验报告，较为啰嗦，请见谅。
_____________________________
  
## 一．代码总述
1.	头文件utilities.h
其中定义了编译器需要使用的所有函数，并对需要在不同文件中共享的变量进行了extern声明。
2.	词法分析实现文件lexer.cpp
其中对utilities.h中定义的用于词法分析的函数进行了实现，并且定义了在词法分析过程中需要使用到的诸多辅助变量。
3.	语法分析、语法树产生、目标代码产生实现文件parser.cpp
其中对utilities.h中定义的用于语法分析、语法树产生、目标代码产生的函数进行了实现，并且定义了在上述过程中需要使用到的诸多辅助变量。
4.	解释执行实现文件interpreter.cpp
其中对utilities.h中定义的用于解释执行的函数进行了实现，并且定义了在解释执行过程中需要使用到的诸多辅助变量。
5.	程序总执行文件PL0_Compiler.cpp
该文件中包含main函数，是整个PL0编译器的控制部分。
  
## 二．词法分析
1.	要求：实现`GETSYM()`函数，将token读入到以下三个变量中。
(1)	`SYM`：存放每个单词的类别，为内部编码的表示形式。
(2)	`ID`：存放用户所定义的标识符的值，即标识符字符串的机内表示。
(3)	`NUM`：存放用户定义的数。
```cpp
symType SYM;        // 类型  
string ID;          // 标识符名称  
int NUM;            // 用户定义的数  
```
其中的`symType`是枚举的编码，见如下符号编码。

2.	符号编码
为实现对读入的token的表示和存储，需要将token映射到不同的编码上。
(1)	特殊编码
类型	编码	编码值
错误标识符	ERROR	0
数字标识符	NUMBER	5
(2)	关键字
符号	编码	编码值
var	VARSYM	10
const	CONSTSYM	11
call	CALLSYM	12
procedure	PROCEDURESYM	13
begin	BEGINSYM	14
end	ENDSYM	15
if	IFSYM	16
then	THENSYM	17
while	WHILESYM	18
do	DOSYM	19
read	READSYM	20
write	WRITESYM	21
(3)	运算符
符号	编码	编码值
+	PLUSSYM	30
-	MINUSSYM	31
*	MULTIPLYSYM	32
/	DIVIDESYM	33
=	EQUALSYM	34
<>	NOTEQUALSYM	35
<	LESSSYM	36
<=	LESSEQUALSYM	37
>	GREATER	38
>=	GREATEREQUALSYM	39
:=	ASSIGNSYM	40
(4)	界符
符号	编码	编码值
,	COMMASYM	50
;	SEMICOLONSYM	51
.	PERIODSYM	52
(	LEFTPARENTHESIS	53
)	RIGHTPARENTHESIS	54
(5)	标识符
符号	编码	编码值
用户自定义标识符	IDENT	1

这些编码通过枚举类型进行定义，实现编码与编码值之间的关联。
1.	enum symType {  
2.	    ERROR = 0,      // 错误类型  
3.	    IDENT = 1,      // 标识符  
4.	    NUMBER = 5,     // 数字  
5.	    VARSYM = 10, CONSTSYM, CALLSYM, PROCEDURESYM, BEGINSYM, ENDSYM, IFSYM, THENSYM, WHILESYM, DOSYM, READSYM, WRITESYM, // 关键字  
6.	    PLUSSYM = 30, MINUSSYM, MULTIPLYSM, DIVIDESYM, EQUALSYM, NOTEQUALSYM, LESSSYM, LESSEQUALSYM, GREATER, GREATEREQUALSYM, ASSIGNSYM,   // 算符  
7.	    COMMASYM = 50, SEMICOLONSYM, PERIODSYM, LEFTPARENTHESIS, RIGHTPARENTHESIS       // 界符  
8.	};  
对于符号与编码和编码值之间的关联，借助map实现string到symType的映射。
1.	// 通过map将类型直接映射到编码  
2.	map<string, symType> keyType = { {"var", symType::VARSYM}, {"const", symType::CONSTSYM}, {"call", symType::CALLSYM},  
3.	    {"procedure", symType::PROCEDURESYM}, {"begin", symType::BEGINSYM}, {"end", symType::ENDSYM}, {"if", symType::IFSYM},  
4.	    {"then", symType::THENSYM}, {"while", symType::WHILESYM}, {"do", symType::DOSYM}, {"read", symType::READSYM}, {"write", symType::WRITESYM} };  
5.	map<string, symType> opeType = { {"+", symType::PLUSSYM}, {"-", symType::MINUSSYM}, {"*", symType::MULTIPLYSM}, {"/", symType::DIVIDESYM},  
6.	    {"=", symType::EQUALSYM}, {"<>", symType::NOTEQUALSYM}, {"<", symType::LESSSYM}, {"<=", symType::LESSEQUALSYM},  
7.	    {">", symType::GREATER}, {">=", symType::GREATEREQUALSYM}, {":=", symType::ASSIGNSYM} };  
8.	map<char, symType> boundType = { {',', symType::COMMASYM}, {';', symType::SEMICOLONSYM}, {'.', symType::PERIODSYM},  
9.	    {'(', symType::LEFTPARENTHESIS}, {')', symType::RIGHTPARENTHESIS} };  

3.	GETSYM()实现
通过字符串的处理，将每个读入的token转为以上编码储存和使用，便可以实现GETSYM()函数。
代码的读入是通过getline()函数实现的，每次读入一行代码，之后通过字节流对代码进行分割，然后在GETSYM()函数中对分割后的单词进行判断。
1.	void GETSYM()  
2.	{  
3.	    if (words.empty()) {  
4.	        if (!(strCin >> words)) {  
5.	            strCin.clear();  
6.	            while (getline(inFile, str)) {  
7.	                line++;  
8.	                if (!str.empty())  
9.	                    break;  
10.	            }  
11.	            strCin.str(str);  
12.	            strCin >> words;  
13.	        }  
14.	    }  
15.	  
16.	    //关键字肯定是单独作为一个单词的  
17.	    if (isKeywords(words))  
18.	    {  
19.	        SYM = keyType[words];  
20.	        cout << "Line " << line << ": (" << SYM << ", " << words << ")" << endl;  
21.	        words = "";  
22.	        return;  
23.	    }  
24.	  
25.	    //非关键字：关键字，标识符，运算符，界符等混合在一起的单词  
26.	    //修改为每次只处理一个token  
27.	    int i = 0;  
28.	    //判断是否是关键字+界符的形式，如"write("  
29.	    if (words[i] >= 'a' && words[i] <= 'z') {  
30.	        int len = 1;  
31.	        while (words[len] >= 'a' && words[len] <= 'z')  
32.	            len++;  
33.	        if (isKeywords(words.substr(0, len))) {  
34.	            if (isBound(words[len])) {  
35.	                SYM = keyType[words.substr(0, len)];  
36.	                cout << "Line " << line << ": (" << SYM << ", " << words.substr(0, len) << ")" << endl;  
37.	                words = words.substr(len);  
38.	                return;  
39.	            }  
40.	            else {  
41.	                lexicalError();  
42.	                return;  
43.	            }  
44.	        }  
45.	    }  
46.	    //判断是否是两位的运算符，比如"<=", ">=", "<>", ":="  
47.	    if ((i < words.length() - 1) && isOperator(words.substr(i, 2))) {  
48.	        SYM = opeType[words.substr(i, 2)];  
49.	        cout << "Line " << line << ": (" << SYM << ", " << words.substr(i, 2) << ")" << endl;  
50.	        words = words.substr(2);  
51.	    }  
52.	    //判断是否是一位的运算符，比如+, -等  
53.	    else if (isOperator(words.substr(i, 1))) {  
54.	        SYM = opeType[words.substr(i, 1)];  
55.	        cout << "Line " << line << ": (" << SYM << ", " << words[i] << ")" << endl;  
56.	        words = words.substr(1);  
57.	    }  
58.	    //判断是否是界符  
59.	    else if (isBound(words[i])) {  
60.	        SYM = boundType[words[i]];  
61.	        cout << "Line " << line << ": (" << SYM << ", " << words[i] << ")" << endl;  
62.	        words = words.substr(1);  
63.	    }  
64.	    //判断是否是数字  
65.	    else if (words[i] >= '0' && words[i] <= '9') {  
66.	        int len = 1;  
67.	        while (i + 1 < words.length() && words[i + 1] >= '0' && words[i + 1] <= '9') {  
68.	            len++;  
69.	            i++;  
70.	        }  
71.	        if (i + 1 < words.length()) {  
72.	            char next = words[i + 1];  
73.	            if ((next >= 'a' && next <= 'z') || next == ':' || next == '(') { //数字后面不可能跟字母、赋值号:=、左括号(  
74.	                lexicalError();  
75.	                return;  
76.	            }  
77.	        }  
78.	        SYM = NUMBER;  
79.	        NUM = stoi(words.substr(i - len + 1, len));  
80.	        cout << "Line " << line << ": (" << SYM << ", " << words.substr(i - len + 1, len) << ")" << endl;  
81.	        words = words.substr(i + 1);  
82.	    }  
83.	    //判断是否是标识符  
84.	    else if (words[i] >= 'a' && words[i] <= 'z') {  
85.	        int len = 1;  
86.	        while (i + 1 < words.length() && ((words[i + 1] >= '0' && words[i + 1] <= '9') || (words[i + 1] >= 'a' && words[i + 1] <= 'z'))) {  
87.	            len++;  
88.	            i++;  
89.	        }  
90.	  
91.	        ID = words.substr(i - len + 1, len);  
92.	        SYM = IDENT;  
93.	        cout << "Line " << line << ": (" << SYM << ", " << ID << ")" << endl;  
94.	        words = words.substr(i + 1);  
95.	    }  
96.	}  
4.	词法分析示例
以“PL0_code2.in”为例，GETSYM()过程获得的代码如下所示。
最左侧标注该token在源文件中所在的行数，括号中左侧为该token的编码，右侧为token名称。
 

三．	语法分析
1.	语法分析使用自上而下的递归实现，根据PL/0代码的文法执行相应的函数，因此首先需要将文法转为对应的函数。
(1)	每项的代码表示
term	code
<程序>	P
<分程序>	SP
<常量说明部分>	CSTAT
<常量定义>	CDEF
<无符号整数>	UN
<变量说明部分>	V
<标识符>	IDF
<过程说明部分>	PROSTAT
<过程首部>	PROHEAD
<语句>	ST
<赋值语句>	AS
<复合语句>	COM
<条件>	COND
<表达式>	EXP
<项>	T
<因子>	F
<加减运算符>	PM
<乘除运算符>	MD
<关系运算符>	REL
<条件语句>	CONDSTAT
<过程调用语句>	CAL
<当型循环语句>	LOOP
<读语句>	R
<写语句>	W
<字母>	AZ
<数字>	N
<空>	NU
(2)	整体文法的代码表示
BNF	代码
<程序> → <分程序>.	P → SP.
<分程序> → [<常量说明部分>][<变量说明部分>][<过程说明部分>]<语句>	SP → [CSTAT][V][PROSTAT]ST
<常量说明部分> → CONST<常量定义>{ ,<常量定义>};	CSTAT → CONST CDEF{,CDEF};
<常量定义> → <标识符>=<无符号整数>	CDEF → IDF=UN
<无符号整数> → <数字>{<数字>}	UN → N{N}
<变量说明部分> → VAR<标识符>{ ,<标识符>};	V → VAR IDF{,IDF};
<标识符> → <字母>{<字母>|<数字>}	IDF → AZ{AZ|N}
<过程说明部分> → <过程首部><分程序>;{<过程说明部分>}	PROSTAT → PROHEAD SP;{PROSTAT}
<过程首部> → procedure<标识符>;	PROHEAD → procedure IDF;
<语句> → <赋值语句>|<条件语句>|<当型循环语句>|<过程调用语句>|<读语句>|<写语句>|<复合语句>|<空>	ST → AS|CONDSTAT|LOOP|CAL|R|W|COM|NU
<赋值语句> → <标识符>:=<表达式>	AS → IDF:=EXP
<复合语句> → begin<语句>{ ;<语句>}end	COM → begin ST{;ST}end
<条件> → <表达式><关系运算符><表达式>|odd<表达式>	COND → EXP REL EXP|odd EXP
<表达式> → [+|-]<项>{<加减运算符><项>}	EXP → [+|-]T{PM T}
<项> → <因子>{<乘除运算符><因子>}	T → F{MD F}
<因子> → <标识符>|<无符号整数>|(<表达式>)	F → IDF|UN|(EXP)
<加减运算符> → +|-	PM → +|-
<乘除运算符> → *|/	MD → *|/
<关系运算符> → =|<>|<|<=|>|>=	REL → =|<>|<|<=|>|>=
<条件语句> → if<条件>then<语句>	CONDSTAT → if COND then ST
<过程调用语句> → call<标识符>	CAL → call IDF
<当型循环语句> → while<条件>do<语句>	LOOP → while COND do ST
<读语句> → read(<标识符>{ ，<标识符>})	R → read(IDF{,IDF})
<写语句> → write(<表达式>{，<表达式>})	W → write(EXP{,EXP})
<字母> → a|b|c...y|z	AZ → a|b|c...y|z
<数字> → 0|1|2...8|9	N → 0|1|2...8|9

2.	递归下降分析程序实现
根据上述文法转换成的函数调用关系实现各函数即可，由于函数过多，此处不依次展示，请参照parser.cpp中的代码注释。
此外，由于在此过程中还需要实现语法树的产生和目标代码生成，因此在递归下降实现的各部分函数中还需要附加额外的处理代码。
1.	void P();           // P -> SP.  
2.	void SP();          // SP -> [CSTAT][V][PROSTAT]ST  
3.	void CSTAT();       // CSTAT -> const CDEF{,CDEF};  
4.	void CDEF();        // CDEF -> IDF=UN  
5.	void V();           // var IDF{,IDF};  
6.	void PROSTAT();     // PROSTAT -> PROHEAD SP;{PROSTAT}  
7.	void PROHEAD();     // procedure IDF;  
8.	void ST();          // ST -> AS|CONDSTAT|LOOP|CALL|R|W|COM|NU  
9.	void AS();          // AS -> IDF:=EXP  
10.	void COM();         // COM -> begin ST{;ST}end  
11.	void COND();        // COND -> EXP REL EXP  
12.	void EXP();         // EXP -> [+|-]T{PM T}  
13.	void T();           // T -> F{MD F}  
14.	void F();           // F -> IDF|UN|(EXP)  
15.	symType REL();      // REL -> =|<>|<|<=|>|>=  
16.	void CONDSTAT();    // CONDSTAT -> if COND then ST  
17.	void CALL();        // CALL -> call IDF  
18.	void LOOP();        // LOOP -> while COND do ST  
19.	void R();           // R -> read(IDF{,IDF})  
20.	void W();           // W -> write(EXP{,EXP})  
3.	设计符号表
在语法分析过程中需要使用符号表，记录常量、变量和过程的相关信息。符号表的属性类型定义如下。
1.	struct sym {  
2.	    string name;    // 名字  
3.	    int type;       // 类型  
4.	    int val;        // 标识符的值或过程的入口地址  
5.	    int level;      // 层次  
6.	    int address;    // 地址  
7.	    int size;       // 当标识符是过程名时，记录形参个数  
8.	};  
4.	符号表的填写。
在获得变量、常量和过程时，需要将相应的信息填入符号表中。
由于这些符号可能已经存在，因此需要定义多个辅助函数查找、判断符号的相应信息。
在填写符号表时，需要记录层次、偏移量等信息。层次可以定义全局变量，在声明函数时累加，离开函数时恢复，偏移量需要在定义变量等信息时累加。具体可见parser.cpp的代码注释。
1.	// 符号表中的类型  
2.	const int CONSTANT = 0;     // 常量  
3.	const int VARIABLE = 1;     // 变量  
4.	const int PROCEDURE = 2;    // 过程  
5.	  
6.	sym symTable[1000]; // 建立一个一维数组记录syms  
7.	int tx = 0;         // symTable的指针  
8.	int dx;             // 每层的局部变量的相对地址  
9.	int lev;  

四．	语法树产生
1.	语法树的产生
语法分析通过递归下降分析实现，在此过程中会隐含地产生语法树。
2.	记录语法树
只需要在语法分析过程中记录每层获取的符号和调用的函数，便可以模拟记录调用过程产生的语法树。
由于语法树的构建过程与语法分析是同步的，因此语法树的产生过程代码在语法分析的递归下降过程代码中。
3.	语法树的存储类型
语法树的信息被存在自定义的treeNode类型中，该类型中记录了调用节点的名称name和其调用的子节点，并通过vector储存。
1.	struct treeNode {       // 语法树节点  
2.	    treeNode(string nam = "") {  
3.	        name = nam;  
4.	        children.clear();  
5.	    }  
6.	    string name;  
7.	    vector<treeNode*> children;  
8.	};  
4.	语法树的输出
使用层次遍历，输出语法树，即可得到一棵从上到下递归产生的语法树。
1.	void displayTree()  
2.	{  
3.	    int layer = 0;  
4.	    queue<treeNode*> q;  
5.	    q.push(root);  
6.	    while (!q.empty()) {  
7.	        int sz = q.size();  
8.	        cout << "Layer " << layer << ": ";  
9.	        while (sz--) {  
10.	            treeNode* temp = q.front();  
11.	            q.pop();  
12.	            cout << temp->name << ' ';  
13.	            for (int i = 0; i < temp->children.size(); i++)  
14.	                q.push(temp->children[i]);  
15.	        }  
16.	        cout << endl;  
17.	        layer++;  
18.	    }  
19.	}  
以“PL0_code2.in”为例，产生的语法树如下所示：
 

五．	目标代码产生
1.	目标代码的格式
PL/0语言的目标指令格式为f l a，f代表功能码，l代表层次差，a代表位移量。
功能码共有8种，代表不同的功能。
2.	目标代码的功能与编码方式
结合功能码f，定义对应的功能与编码方式如下：
1.	const int LIT = 0;      // 将常数放到运栈顶，a域为常数。  
2.	const int LOD = 1;      // 将变量放到栈顶。a域为变量所在说明层中的相对位置，l为调用层与说明层的层差值。  
3.	const int STO = 2;      // 将栈顶的内容送到某变量单元中。a,l域的含义与LOD的相同。  
4.	const int CAL = 3;      // 调用过程的指令。a为被调用过程的目标程序的入口地址，l为层差。  
5.	const int INT = 4;      // 为被调用的过程或主程序在运行栈中开辟数据区。a域为开辟的个数。  
6.	const int JMP = 5;      // 无条件转移指令，a为转向地址。  
7.	const int JPC = 6;      // 条件转移指令，当栈顶的布尔值为非真时，转向a域的地址，否则顺序执行。  
8.	const int OPR = 7;      // 关系和算术运算。具体操作由a域给出，运算对象为栈顶和次栈顶的内容，结果放在次栈顶。a域为0时退出数据区。  
3.	OPR指令中a域的功能定义
OPR指令中，具体的操作由a域的值指出，定义a域的值和对应操作如下。
1.	// a域的取值及含义  
2.	// 0：过程返回或程序结束  
3.	// 1：取负数  
4.	// 2：加法  
5.	// 3：减法  
6.	// 4：乘法  
7.	// 5：除法  
8.	// 6：判断相等  
9.	// 7：判断不等  
10.	// 8：判断小于  
11.	// 9：判断不小于  
12.	// 10：判断大于  
13.	// 11：判断不大于  
14.	// 12：输出栈顶内容  
15.	// 13：输出换行  
16.	// 14：读入  
4.	目标代码的产生函数gen()
gen()产生的目标代码由参数决定，参数在语法分析过程中由递归下降的过程函数传入。
1.	void gen(int f, int l, int a)   // 中间代码生成函数  
2.	{  
3.	    CODE[cx].f = f;  
4.	    CODE[cx].l = l;  
5.	    CODE[cx].a = a;  
6.	    cx++;  
7.	} 
5.	目标代码示例
以“PL0_code2.in”为例，产生的目标代码如下：
 

六．	解释执行
1.	解释执行的基础
解释执行需要设置寄存器和数据栈，用于存储解释执行过程中需要的指令和数据信息。
1.	codeType REG_I;     //指令寄存器REG_I，存放当前要执行的代码  
2.	int REG_P = 0;      //程序地址寄存器REG_P，存放下一条要执行的指令的地址  
3.	int REG_T = 0;      //栈顶指示器REG_T  
4.	int REG_B = 0;      //基址寄存器REG_B，存放当前运行过程的数据区在STACK中的起始地址  
5.	int dataStack[1000];  
2.	解释执行需要的编码设置
解释执行过程中，使用switch-case语句根据不同的指令进行跳转，指令的编码方式与“目标代码产生”中相同。
此外，由于OPR指令需要根据a域的值判断具体的操作，因此内部也需要使用switch-case语句跳转，a域的编码也与“目标代码产生”中相同。
3.	解释执行的实现
(1)	LIT l a. 将常数放到运栈顶，a域为常数。
将a放到栈顶即可。
1.	case 0:  
2.	    dataStack[REG_T++] = REG_I.a;  
3.	    break;  
(2)	LOD l a. 将变量放到栈顶。a域为变量所在说明层中的相对位置，l为调用层与说明层的层差值。
首先利用getBase()函数获得指定层差的基地址，然后与a域表示的偏移量相加获得变量的位置，将其放到栈顶。
1.	case 1:  
2.	    dataStack[REG_T++] = dataStack[REG_I.a + getBase(REG_I.l)];  
3.	    break;  
(3)	STO l a. 将栈顶的内容送到某变量单元中。a, l域的含义与LOD的相同。
与LOD过程相反。
1.	case 2:  
2.	    REG_T--;  
3.	    dataStack[REG_I.a + getBase(REG_I.l)] = dataStack[REG_T];  
4.	    break;  
(4)	CAL l a. 调用过程的指令。a为被调用过程的目标程序的入口地址，l为层差。
每个过程被调用时，在栈顶分配三个联系单元。这三个单元的内容分别是：
SL：静态链，它是指向定义该过程的直接外过程运行时数据段的基地址。
DL：动态链，它是指向调用该过程前正在运行过程的数据段的基地址。
RA：返回地址，记录调用该过程时目标程序的断点，即当时的程序地址寄存器P的值。
1.	case 3:  
2.	    dataStack[REG_T] = getBase(REG_I.l);    //栈顶压入静态链SL  
3.	    dataStack[REG_T + 1] = REG_B;           //栈顶压入当前数据区基址，作为动态链DL  
4.	    dataStack[REG_T + 2] = REG_P;           //栈顶压入当前PC，作为返回地址RA  
5.	    REG_B = REG_T;                          //当前数据区基址为当前的栈顶  
6.	    REG_P = REG_I.a;                        //程序跳转到a处继续执行  
7.	    break;  
(5)	INT l a. 为被调用的过程或主程序在运行栈中开辟数据区。a域为开辟的个数。
增加栈顶指示寄存器REG_T的值代表开辟数据空间。
1.	case 4:  
2.	    REG_T += REG_I.a;  
3.	    break;  
(6)	JMP l a. 无条件转移指令，a为转向地址。
将要转向的地址a赋值给程序地址寄存器REG_P。
1.	case 5:  
2.	    REG_P = REG_I.a;  
3.	    break;  

(7)	JPC l a. 条件转移指令，当栈顶的布尔值为非真时，转向a域的地址，否则顺序执行。
当栈顶非真时，才进行跳转。
1.	case 6:  
2.	    if (dataStack[REG_T - 1] == 0)  
3.	        REG_P = REG_I.a;  
4.	    break;  
(8)	OPR l a. 关系和算术运算。具体操作由a域给出，运算对象为栈顶和次栈顶的内容，结果放在次栈顶。a域为0时退出数据区。
由于OPR指令的举止执行操作由a域给出，因此借助switch-case语句根据a域的不同取值转向不同的操作功能。
1.	case 7:  
2.	    switch (REG_I.a)  
3.	    {  
4.	    case 0:     // 返回  
5.	        REG_T = REG_B;                  //释放这段子过程占用的数据栈空间  
6.	        REG_B = dataStack[REG_T + 1];   //数据段基址为DL  
7.	        REG_P = dataStack[REG_T + 2];   //返回地址为RA  
8.	        break;  
9.	    case 1:     // 取负数  
10.	        dataStack[REG_T - 1] = -dataStack[REG_T - 1];  
11.	        break;  
12.	    case 2:     // 加法  
13.	        REG_T--;  
14.	        dataStack[REG_T - 1] = dataStack[REG_T - 1] + dataStack[REG_T];  
15.	        break;  
16.	    case 3:     // 减法  
17.	        REG_T--;  
18.	        dataStack[REG_T - 1] = dataStack[REG_T - 1] - dataStack[REG_T];  
19.	        break;  
20.	    case 4:     // 乘法  
21.	        REG_T--;  
22.	        dataStack[REG_T - 1] = dataStack[REG_T - 1] * dataStack[REG_T];  
23.	        break;  
24.	    case 5:     // 除法  
25.	        REG_T--;  
26.	        dataStack[REG_T - 1] = dataStack[REG_T - 1] / dataStack[REG_T];  
27.	        break;  
28.	    case 6:     // 判断相等  
29.	        REG_T--;  
30.	        if (dataStack[REG_T - 1] == dataStack[REG_T])  
31.	            dataStack[REG_T - 1] = 1;  
32.	        else  
33.	            dataStack[REG_T - 1] = 0;  
34.	        break;  
35.	    case 7:     // 判断不等  
36.	        REG_T--;  
37.	        if (dataStack[REG_T - 1] != dataStack[REG_T])  
38.	            dataStack[REG_T - 1] = 1;  
39.	        else  
40.	            dataStack[REG_T - 1] = 0;  
41.	        break;  
42.	    case 8:     // 判断小于  
43.	        REG_T--;  
44.	        if (dataStack[REG_T - 1] < dataStack[REG_T])  
45.	            dataStack[REG_T - 1] = 1;  
46.	        else  
47.	            dataStack[REG_T - 1] = 0;  
48.	        break;  
49.	    case 9:     // 判断不小于  
50.	        REG_T--;  
51.	        if (dataStack[REG_T - 1] >= dataStack[REG_T])  
52.	            dataStack[REG_T - 1] = 1;  
53.	        else  
54.	            dataStack[REG_T - 1] = 0;  
55.	        break;  
56.	    case 10:    // 判断大于  
57.	        REG_T--;  
58.	        if (dataStack[REG_T - 1] > dataStack[REG_T])  
59.	            dataStack[REG_T - 1] = 1;  
60.	        else  
61.	            dataStack[REG_T - 1] = 0;  
62.	        break;  
63.	    case 11:    // 判断不大于  
64.	        REG_T--;  
65.	        if (dataStack[REG_T - 1] <= dataStack[REG_T])  
66.	            dataStack[REG_T - 1] = 1;  
67.	        else  
68.	            dataStack[REG_T - 1] = 0;  
69.	        break;  
70.	    case 12:    // 输出栈顶内容  
71.	        cout << dataStack[REG_T - 1];  
72.	        cout << " ";  
73.	        break;  
74.	    case 13:    // 输出换行  
75.	        cout << endl;  
76.	        break;  
77.	    case 14:    // 读入  
78.	        int num;  
79.	        cin >> num;  
80.	        dataStack[REG_T++] = num;  
81.	        break;  
82.	    default:  
83.	        cout << "Error in Interpreter: wrong OPR code." << endl;  
84.	    }  
4.	解释执行示例
以“PL0_code2.in”为例，解释执行的结果如下图所示。
可以看到该程序实现了输出从1到10的每个数的平方的功能。
 


