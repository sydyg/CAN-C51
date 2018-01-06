#include"./include/SPI.h"
#include"./include/config.h"


/*
*1.��������SPI_Send_Byte1
*2.������ѡ��MCP2515_1����SPI���߷���һ�ֽ�����
*3.���룺unsigned char data
*4.�������
*5.˵��������SPIЭ��ʵ�ַ������ݣ���������һ���������ôӻ���������
*  ����ʱд��ֻ���MCP2515_1
*/
static void SPI_Send_Byte1(unsigned char Data)
{
	 int i=0;	 
	 for(i=0;i<8;++i)
	 {
		 SCK1 = 0;
		 if(Data & 0x80)
		   SI1 = 1;
		 else
		   SI1 = 0;		 
		 SCK1 = 1;
		 Data<<=1;
	 }
	 SCK1 = 0;
}

/*
*1.��������SPI_Send_Byte1
*2.��������SPI���߷���һ�ֽ�����
*3.���룺unsigned char data
*4.�������
*5.˵��������SPIЭ��ʵ�ַ������ݣ���������һ���������ôӻ���������
*  ����ʱд��ֻ���MCP2515_2
*/
static void SPI_Send_Byte2(unsigned char Data)
{
	 int i=0;	 
	 for(i=0;i<8;++i)
	 {
		 SCK2 = 0;
		 if(Data & 0x80)
		   SI2 = 1;
		 else
		   SI2 = 0;		 
		 SCK2 = 1;
		 Data<<=1;
	 }
	 SCK1 = 0;
}

/*
*1.��������SPI_Recv_Byte
*2.��������SPI���߽���һ�ֽ�����
*3.���룺��
*4.�����data
*5.˵��������SPIЭ��ʵ�����ݽ��գ���������һ�����������ôӻ��������ݡ�
*  ����ʱд��ֻ���MCP2515_1 
*/
static unsigned char SPI_Recv_Byte1()
{
    unsigned char Data=0;
	int i=0;
	SCK1 = 0;
	for(i=0;i<8;++i)
	{
	    SCK1 = 1;			
		Data<<=1;
		Data |= SO1;
		SCK1 = 0;		
	}
	return Data;
}

/*
*1.��������SPI_Recv_Byte
*2.��������SPI���߽���һ�ֽ�����
*3.���룺��
*4.�����data
*5.˵��������SPIЭ��ʵ�����ݽ��գ���������һ���½����ôӻ��������ݡ�
*  ����ʱд��ֻ���MCP2515_2 
*/
static unsigned char SPI_Recv_Byte2()
{
    unsigned char Data=0;
	int i=0;
	SCK2 = 0;
	for(i=0;i<8;++i)
	{
	    SCK2 = 1;
		Data<<=1;
		Data |= SO2;	
		SCK2 = 0;		
	}
	return Data;
}

/*
1.��������SPI_Send_Byte
2.������ѡ��ĳ��MCP2515оƬ����SPI���߷���һ�ֽ�����
3.���룺unsigned char data,unsigned char chipnum
4.�������
5.˵��������SPIЭ��ʵ�ַ������ݣ���������һ���������ôӻ���������
*/
void SPI_Send_Byte(unsigned char Data,unsigned char chipnum)
{
    if(chipnum==1)
	   SPI_Send_Byte1(Data);
    else if(chipnum==2)
	   SPI_Send_Byte2(Data);
}

/*
1.��������SPI_Recv_Byte
2.������ѡ��ĳ��MCP2515оƬ����SPI���߽���һ�ֽ�����
3.���룺ѡ�е�оƬ���num
4.�����data
5.˵��������SPIЭ��ʵ�����ݽ��գ���������һ���½����ôӻ��������ݡ�
*/
unsigned char SPI_Recv_Byte(unsigned char chipnum)
{
    unsigned char Data=0;
    if(chipnum==1)
	  Data = SPI_Recv_Byte1();
    else if(chipnum==2)
	  Data = SPI_Recv_Byte2();
	return Data;
}



