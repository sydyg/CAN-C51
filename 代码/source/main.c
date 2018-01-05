#define _UART_C
#define _MCP2515_C
#include"./include/UART.h"
#include"./include/MCP2515.h"
#include"./include/config.h"

static unsigned char PC_Cmd[10];//PC������MCU������
static unsigned char CAN_Cmd[10];//MCU��CAN���߶��ص�����
static code unsigned  char err1[] = "faile write to MCP2515_1\r\n";
static code unsigned  char err2[] = "faile read from MCP2515_2\r\n";
static code unsigned  char err3[] = "no recieve cmd\r\n";
static code unsigned  char msg1[] = "\r\nys# ";
bit CAN_Flag = 0;
/*
*1.��������EXTI1_init
*2.�����������ⲿ�ж�1
*3.���룺��
*4.�������
*5.˵����
*/
void EXTI1_init()
{
	 IT1 = 1;//������ʽΪ�½��ش���
	 ET0 = 1;
	 EA = 1;
}

/*
*1. ��������main
*2. ������ʵ�ֳ����������
*3. ���룺��
*4. �������
*5. ����ֵ����
*6. ˵�������ø�����ʼ����������ѭ����ʵ�ִ�PC����ȡ���ݣ�����MCP2515_1, 
*   �ٴ�MCP2515_2��ȡ���ݣ�����PC����PC������MCU��������'#'������������
*   ����ȣ�Ҳ����CAN�����ϴ��䡣
*/

void main()
{
    unsigned char CmdLen;
	Uart_Init(); //��ʼ���괮�ڣ��Ϳ��Ժ�PC��ͨ��
	EXTI1_init();
	MCP2515_Init(1);
	MCP2515_Init(2);
	while(1)
	{
	    UART_Send_Promt(msg1);//��ʾ�û��������������'#'����
		CmdLen = UART_Recv_CMD(PC_Cmd);//��UART����������ȡ����
		
		/*û�ӻ�������������,��PC���������*/
	   	if(CmdLen<1)
		{
		    UART_Send_Data(err3,sizeof(err3));	
			continue;
		}
		//UART_Send_Data(PC_CMD,sizeof(PC_CMD));
		
		if(MCP2515_Sender(PC_Cmd,1)<1)//��MCP2515_1д����
		{
		     UART_Send_Data(err1,sizeof(err1));	//��PC���������
			 continue;
		}
		/*�ȴ����յ�CAN���ߵ����ݲ��˳�*/
		while(1)
		{
		   if(CAN_Flag)
		   {
			    CAN_Flag = 0;
				if(MCP2515_Recver(CAN_Cmd,2)<1)//��ȡMCP2515_2������
				{
				    UART_Send_Data(err2,sizeof(err2));//��ȡʧ�ܣ����ʹ����PC
					break;
				}
				else
				{
				     UART_Send_Data(CAN_Cmd,sizeof(CAN_Cmd));//�Ѵ�CAN���߶�ȡ�������ͻ�PC
					 break;
				}

		   }
		}
		
	}
}

/*MCP2515_2���������ж�*/
void EXIT1_Inetrrupt()	interrupt 2
{
	  CAN_Flag = 1;
}






