#include "utilities.h"

codeType REG_I;		//指令寄存器REG_I，存放当前要执行的代码
int REG_P = 0;		//程序地址寄存器REG_P，存放下一条要执行的指令的地址
int REG_T = 0;		//栈顶指示寄存器REG_T
int REG_B = 0;		//基址寄存器REG_B，存放当前运行过程的数据区在STACK中的起始地址
int dataStack[1000];

int getBase(int lev)	// 根据层差，寻找起始地址
{
	int b = REG_B;		//从当前层开始
	while (lev > 0) {
		b = dataStack[b];	//根据基址内容（SL）找到上一层的基址
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
	fill(dataStack, dataStack + 1000, 0);//初始化数据区
	dataStack[0] = 0;	//主程序SL为0
	dataStack[1] = 0;	//主程序DL为0
	dataStack[2] = 0;	//主程序RA为0
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
			dataStack[REG_T] = getBase(REG_I.l);	//栈顶压入静态链SL
			dataStack[REG_T + 1] = REG_B;			//栈顶压入当前数据区基址，作为动态链DL
			dataStack[REG_T + 2] = REG_P;			//栈顶压入当前PC，作为返回地址RA
			REG_B = REG_T;							//当前数据区基址为当前的栈顶
			REG_P = REG_I.a;						//程序跳转到a处继续执行
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
			case 0:		// 返回
				REG_T = REG_B;					//释放这段子过程占用的数据栈空间
				REG_B = dataStack[REG_T + 1];	//数据段基址为DL
				REG_P = dataStack[REG_T + 2];	//返回地址为RA
				break;
			case 1:		// 取负数
				dataStack[REG_T - 1] = -dataStack[REG_T - 1];
				break;
			case 2:		// 加法
				REG_T--;
				dataStack[REG_T - 1] = dataStack[REG_T - 1] + dataStack[REG_T];
				break;
			case 3:		// 减法
				REG_T--;
				dataStack[REG_T - 1] = dataStack[REG_T - 1] - dataStack[REG_T];
				break;
			case 4:		// 乘法
				REG_T--;
				dataStack[REG_T - 1] = dataStack[REG_T - 1] * dataStack[REG_T];
				break;
			case 5:		// 除法
				REG_T--;
				dataStack[REG_T - 1] = dataStack[REG_T - 1] / dataStack[REG_T];
				break;
			case 6:		// 判断相等
				REG_T--;
				if (dataStack[REG_T - 1] == dataStack[REG_T])
					dataStack[REG_T - 1] = 1;
				else
					dataStack[REG_T - 1] = 0;
				break;
			case 7:		// 判断不等
				REG_T--;
				if (dataStack[REG_T - 1] != dataStack[REG_T])
					dataStack[REG_T - 1] = 1;
				else
					dataStack[REG_T - 1] = 0;
				break;
			case 8:		// 判断小于
				REG_T--;
				if (dataStack[REG_T - 1] < dataStack[REG_T])
					dataStack[REG_T - 1] = 1;
				else
					dataStack[REG_T - 1] = 0;
				break;
			case 9:		// 判断不小于
				REG_T--;
				if (dataStack[REG_T - 1] >= dataStack[REG_T])
					dataStack[REG_T - 1] = 1;
				else
					dataStack[REG_T - 1] = 0;
				break;
			case 10:	// 判断大于
				REG_T--;
				if (dataStack[REG_T - 1] > dataStack[REG_T])
					dataStack[REG_T - 1] = 1;
				else
					dataStack[REG_T - 1] = 0;
				break;
			case 11:	// 判断不大于
				REG_T--;
				if (dataStack[REG_T - 1] <= dataStack[REG_T])
					dataStack[REG_T - 1] = 1;
				else
					dataStack[REG_T - 1] = 0;
				break;
			case 12:	// 输出栈顶内容
				cout << dataStack[REG_T - 1];
				cout << " ";
				break;
			case 13:	// 输出换行
				cout << endl;
				break;
			case 14:	// 读入
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

	delete[]CODE;	//使用完毕之后删除
}

