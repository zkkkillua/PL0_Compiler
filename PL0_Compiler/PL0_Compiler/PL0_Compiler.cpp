#include "utilities.h"

ifstream inFile;			// 文件输入流
istringstream strCin;	// 输入流

int main()
{
	cout << "请输入要执行的程序名称（无odd功能）：" << endl;
	string file;
	cin >> file;
	inFile.open(file);
	cout << endl;
	cout << "程序源代码：" << endl;
	string oriStr;
	while (getline(inFile, oriStr)) //读取文件每一次读取一行,遇到EOF结束
		cout << oriStr << endl;
	cout << endl;
	inFile.close();
	inFile.open(file);

	cout << "词法分析：" << endl;
	P();
	cout << endl;
	cout << "语法树：" << endl;
	displayTree();
	cout << endl;
	cout << "中间代码：" << endl;
	displayCode();
	cout << endl;
	cout << "是否解释执行（y/n）？" << endl;
	char ch;
	cin >> ch;
	if (ch == 'y' || ch == 'Y') {
		Interpreter();
		cout << "解释执行完毕。" << endl;
	}

	inFile.close();

	return 0;
}

