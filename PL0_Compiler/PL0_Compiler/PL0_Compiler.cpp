#include "utilities.h"

ifstream inFile;			// �ļ�������
istringstream strCin;	// ������

int main()
{
	cout << "������Ҫִ�еĳ������ƣ���odd���ܣ���" << endl;
	string file;
	cin >> file;
	inFile.open(file);
	cout << endl;
	cout << "����Դ���룺" << endl;
	string oriStr;
	while (getline(inFile, oriStr)) //��ȡ�ļ�ÿһ�ζ�ȡһ��,����EOF����
		cout << oriStr << endl;
	cout << endl;
	inFile.close();
	inFile.open(file);

	cout << "�ʷ�������" << endl;
	P();
	cout << endl;
	cout << "�﷨����" << endl;
	displayTree();
	cout << endl;
	cout << "�м���룺" << endl;
	displayCode();
	cout << endl;
	cout << "�Ƿ����ִ�У�y/n����" << endl;
	char ch;
	cin >> ch;
	if (ch == 'y' || ch == 'Y') {
		Interpreter();
		cout << "����ִ����ϡ�" << endl;
	}

	inFile.close();

	return 0;
}

