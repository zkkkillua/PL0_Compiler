#include "utilities.h"

codeType REG_I;		//ָ��Ĵ���REG_I����ŵ�ǰҪִ�еĴ���
int REG_P = 0;		//�����ַ�Ĵ���REG_P�������һ��Ҫִ�е�ָ��ĵ�ַ
int REG_T = 0;		//ջ��ָʾ�Ĵ���REG_T
int REG_B = 0;		//��ַ�Ĵ���REG_B����ŵ�ǰ���й��̵���������STACK�е���ʼ��ַ
int dataStack[1000];

int getBase(int lev)	// ���ݲ�Ѱ����ʼ��ַ
{
	int b = REG_B;		//�ӵ�ǰ�㿪ʼ
	while (lev > 0) {
		b = dataStack[b];	//���ݻ�ַ���ݣ�SL���ҵ���һ��Ļ�ַ
		lev--;
	}
	return b;
}

void Interpreter()
{
	REG_I = CODE[REG_P];
	REG_P = 0;
	REG_T = 0;
	REG_B = 0;
	fill(dataStack, dataStack + 1000, 0);//��ʼ��������
	dataStack[0] = 0;	//������SLΪ0
	dataStack[1] = 0;	//������DLΪ0
	dataStack[2] = 0;	//������RAΪ0
	do {
		REG_I = CODE[REG_P++];
		switch (REG_I.f)
		{
		case 0:
			dataStack[REG_T++] = REG_I.a;
			break;
		case 1:
			dataStack[REG_T++] = dataStack[REG_I.a + getBase(REG_I.l)];
			break;
		case 2:
			REG_T--;
			dataStack[REG_I.a + getBase(REG_I.l)] = dataStack[REG_T];
			break;
		case 3:
			dataStack[REG_T] = getBase(REG_I.l);	//ջ��ѹ�뾲̬��SL
			dataStack[REG_T + 1] = REG_B;			//ջ��ѹ�뵱ǰ��������ַ����Ϊ��̬��DL
			dataStack[REG_T + 2] = REG_P;			//ջ��ѹ�뵱ǰPC����Ϊ���ص�ַRA
			REG_B = REG_T;							//��ǰ��������ַΪ��ǰ��ջ��
			REG_P = REG_I.a;						//������ת��a������ִ��
			break;
		case 4:
			REG_T += REG_I.a;
			break;
		case 5:
			REG_P = REG_I.a;
			break;
		case 6:
			if (dataStack[REG_T - 1] == 0)
				REG_P = REG_I.a;
			break;
		case 7:
			switch (REG_I.a)
			{
			case 0:		// ����
				REG_T = REG_B;					//�ͷ�����ӹ���ռ�õ�����ջ�ռ�
				REG_B = dataStack[REG_T + 1];	//���ݶλ�ַΪDL
				REG_P = dataStack[REG_T + 2];	//���ص�ַΪRA
				break;
			case 1:		// ȡ����
				dataStack[REG_T - 1] = -dataStack[REG_T - 1];
				break;
			case 2:		// �ӷ�
				REG_T--;
				dataStack[REG_T - 1] = dataStack[REG_T - 1] + dataStack[REG_T];
				break;
			case 3:		// ����
				REG_T--;
				dataStack[REG_T - 1] = dataStack[REG_T - 1] - dataStack[REG_T];
				break;
			case 4:		// �˷�
				REG_T--;
				dataStack[REG_T - 1] = dataStack[REG_T - 1] * dataStack[REG_T];
				break;
			case 5:		// ����
				REG_T--;
				dataStack[REG_T - 1] = dataStack[REG_T - 1] / dataStack[REG_T];
				break;
			case 6:		// �ж����
				REG_T--;
				if (dataStack[REG_T - 1] == dataStack[REG_T])
					dataStack[REG_T - 1] = 1;
				else
					dataStack[REG_T - 1] = 0;
				break;
			case 7:		// �жϲ���
				REG_T--;
				if (dataStack[REG_T - 1] != dataStack[REG_T])
					dataStack[REG_T - 1] = 1;
				else
					dataStack[REG_T - 1] = 0;
				break;
			case 8:		// �ж�С��
				REG_T--;
				if (dataStack[REG_T - 1] < dataStack[REG_T])
					dataStack[REG_T - 1] = 1;
				else
					dataStack[REG_T - 1] = 0;
				break;
			case 9:		// �жϲ�С��
				REG_T--;
				if (dataStack[REG_T - 1] >= dataStack[REG_T])
					dataStack[REG_T - 1] = 1;
				else
					dataStack[REG_T - 1] = 0;
				break;
			case 10:	// �жϴ���
				REG_T--;
				if (dataStack[REG_T - 1] > dataStack[REG_T])
					dataStack[REG_T - 1] = 1;
				else
					dataStack[REG_T - 1] = 0;
				break;
			case 11:	// �жϲ�����
				REG_T--;
				if (dataStack[REG_T - 1] <= dataStack[REG_T])
					dataStack[REG_T - 1] = 1;
				else
					dataStack[REG_T - 1] = 0;
				break;
			case 12:	// ���ջ������
				cout << dataStack[REG_T - 1];
				cout << " ";
				break;
			case 13:	// �������
				cout << endl;
				break;
			case 14:	// ����
				int num;
				cin >> num;
				dataStack[REG_T++] = num;
				break;
			default:
				cout << "Error in Interpreter: wrong OPR code." << endl;
			}
			break;
		default:
			cout << "Error in Interpreter: wrong f code." << endl;
			if (REG_T > 1000)
				cout << "Error in Interpreter: stack overflow." << endl;
		}
	} while (REG_P != 0);

	delete[]CODE;	//ʹ�����֮��ɾ��
}

