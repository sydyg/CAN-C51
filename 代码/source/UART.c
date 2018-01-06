#define _SPI_C
#include"./include/UART.h"
#include"./include/SPI.h"
#include"./include/config.h"

static unsigned char UART_Recv_Data;//MCU���յ�������PC����һ���ֽ�
static unsigned char UART_Recv_Buff[10]={'\0'};//MCU���յ�������PC�����ݵĻ�����
static unsigned char cmd_finish=0;//����PC��������ɱ��
/*
*1. ��������Uart_Init
*2. ������ʵ��MCU���ڵĳ�ʼ��
*3. ���룺��
*4. �������
*5. ����ֵ����
*6. ˵����1.���ô���ģʽ(SCON) 2.����λ��ʱ(TMOD)����ʱ��1��λ��ʱ��
     3.ʹ�ܴ����ж�ES 4.ʹ�����ж�
*    ÿ��λʱ�����32�Σ�������ɣ���T1���Ϳ��Ժ�PC������ͨ��
*/
void Uart_Init(void)
{
   	  SCON = 0x50;//ģʽ1��10λ���ݣ�����żУ��
	  TMOD = 0x20; //���ö�ʱ��Ϊ8λ��װģʽ
	  TH1 = TL1 = 0xfd;//���ò����㣬ÿ(256-0xfd)�������������һ�Σ�T1����ͶԴ����߲���
      TR1 = 1;//������ʱ��1,���ھͿ����շ�������
	  ES = 1;
	  EA = 1; 
}

/*
*1.��������delaynms
*2.��������ʱnMs
*3.���룺n 
*4.�������
*5.˵����
*/
void delaynms(unsigned int i)
{
    unsigned int j=100;
    while(i--)
	{
	    while(j--)//�ڲ���ʱ1ms
		    _nop_();
	}
}

/*
*1. ��������Uart_Send_Byte
*2. ������MCU��PC������һ�ֽ�����
*3. ���룺unsigned char data
*4. �������
*5. ����ֵ����
*6. ˵������MCU����ģ��ķ��ͻ�������д��һ�ֽ�����
*/
void Uart_Send_Byte(unsigned char Data)
{
	  SBUF = Data;
	  delaynms(1); //��ʱ2ms������д��PC����������������
}

/*
*1. ��������UART_Send_Data
*2. ������MCU�Ѵ�MCP2515_2���յ������ݷ��͵�PC��
*3. ���룺unsigned char Data[],unsigned char len
*4. �����ʵ�ʷ��͵��ֽ���
*5. ˵��������Uart_Send_Byte()ʵ�ֶ��ֽڷ���
*/
unsigned char UART_Send_Data(unsigned char Data[],unsigned char len)
{
      int i=0;
	  if(len>0)
	  {
	   while(Data[i]!='\0')
	       Uart_Send_Byte(Data[i++]);
	   return i;
	  }
	  return 0;
}
/*
*1.��������UART_Send_Promt
*2.��������PC���Ĵ��ڹ��ߴ�ӡ��ʾ�������������н���
*3.���룺unsigned char* ������ʾ��
*4.�������
*5.˵����MCU��PC����ӡ��ʾ����Ȼ�������ѭ�������յ�PC��������������˳���ѭ��
   �˳�ʱ��PC�������������Ѿ�������UART��������
*/
void UART_Send_Promt(unsigned char* prompt)
{
     while(*prompt!='\0')
	 {
	     SBUF = *prompt;
		 delaynms(1); //��ʱ2ms������д��PC����������������
		 ++prompt;
	 }
	 /*�ȴ������յ�PC��������������˳���ѭ��*/
	 while(1)
	 {
	      if(cmd_finish == 1)
		  {
		    cmd_finish = 0;
		    break;
		  }
	 }
}

/*
*1. ��������UART_Recv_CMD
*2. ��������UART����������������ȥ
*3. ���룺unsigned char data[]
*4. ������������ֽ���,0��ʾ��������
*5. ˵�����涨PC����������'#'�������Ӵ������뻺����ѭ���������ݣ������'#'
    ����ѭ����
*/
unsigned char UART_Recv_CMD(unsigned char Data[])
{
    int i=0;
	/*�������ǿղſ���*/
	if(UART_Recv_Buff[0]!='\0')
	{
		while(UART_Recv_Buff[i]!='\0')
		{
		    Data[i] = UART_Recv_Buff[i];
			++i;
		}
		Data[i] = '\0';
		UART_Recv_Buff[0] = '\0';//������ɣ���ջ�����
    	return i;
	}
	return 0;	
}
/*
*1.��������UART_Interrupt
*2.������UART�����жϺ���
*3.���룺��
*4.�������
*5.˵�������յ�PC��������һ���ֽں��жϣ��ѽ��յ�������������ջ�����,'#'��ʾ����
*/
void UART_Interrupt() interrupt 4
{
    static int i=0;
    if(RI)
	{
		RI = 0;
		UART_Recv_Data = SBUF;	
		if(UART_Recv_Data != '#')
		{
			UART_Recv_Buff[i++] = UART_Recv_Data;			
		}
		else
		{
		  cmd_finish = 1;
		  UART_Recv_Buff[i] = '\0';//����ַ���������ǣ��Ա����
		  i = 0;
		}
	}
	else if(TI)
	{
	    TI = 0;
	}
}

