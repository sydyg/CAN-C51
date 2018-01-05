#define _SPI_C
#define _UART_C
#include"./include/MCP2515.h"
#include"./include/SPI.h"
#include"./include/config.h"
#include"./include/UART.h"

static void MCP2515_Write_Byte(unsigned char Addr,unsigned char Data,unsigned char chipnum);
static unsigned char MCP2515_Read_Byte(unsigned char Addr,unsigned char chipnum);
/*
1.��������MCP2515_Reset
2.˵������λMCP2515
3.���룺оƬ��
4.�������
5.����������SPI_Send_Byte(),��MCP2515������λ����
*/
static void  MCP2515_Reset(unsigned char chipnum)
{
      if(chipnum==1)
	  {
	    CS1 = 0; 
	    SPI_Send_Byte(CAN_RESET,chipnum);
		CS1 = 1;
	  }
	  else if(chipnum==2)
	  {
	    CS2 = 0; 
	    SPI_Send_Byte(CAN_RESET,chipnum);
		CS2 = 1;
	  }
}
/*
*1.��������MCP2515_Init
*2. ������ʵ��MCP2515�ĳ�ʼ��
*3. ���룺unsigned char chipnum��Ҫ��ʼ����оƬ��
*4. �������
*5. ����ֵ����
*6. ˵������
*/
void MCP2515_Init(unsigned char chipnum)
{
    unsigned char canstate;
	/*��ʼ������*/
	/*1.��λ*/
	MCP2515_Reset(chipnum);
	delaynms(1);
	/*2.���ò����ʺ���ͬ�����
	* ����������=500KBps,Fosc=8MHz,ÿλ20us=20TQ��ÿ��TQ=1us��
	* TQ=2*BRP/Fosc,��BRP=4��ȡͬ����=1TQ��������=3TQ��PS1=8TQ��PS2=8TQ
	* SJW=2TQ	   
	*/
	MCP2515_Write_Byte(CNF1,BRP|SJW_2TQ,chipnum);
	/*3.����λʱ��*/
	MCP2515_Write_Byte(CNF2,BTLMODE|SAM_1|PHSEG1_8TQ|PRSEG_3TQ,chipnum);
	MCP2515_Write_Byte(CNF3,SOF|WAKFIL|PHSEG2_8TQ,chipnum);
	/*5.���÷��ͻ�����0*/
	MCP2515_Write_Byte(TXB0CTRL,TXP,chipnum);
	MCP2515_Write_Byte(TXB0SIDH,SIDH,chipnum);
	MCP2515_Write_Byte(TXB0SIDL,SIDL,chipnum);
	/*6.��ս��ջ�����0�ı�׼֡��ʶ��*/
	
	MCP2515_Write_Byte(RXB0SIDH,0x00,chipnum);
	MCP2515_Write_Byte(RXB0SIDL,0x00,chipnum);
	/*7.���ý��ջ�����0���ƼĴ���*/
	MCP2515_Write_Byte(RXB0CTRL,RXM|FILHIT,chipnum);
	/*8.���ý��ջ�����0�������˲���,11λ��ʶ����Ϊ11111111000B*/
	MCP2515_Write_Byte(RXF0SIDH,SIDH,chipnum);
	MCP2515_Write_Byte(RXF0SIDL,SIDL,chipnum);
	/*9.���ý��ջ�����0�����μĴ���,����λ������֤�������Խ�������֡*/
	MCP2515_Write_Byte(RXM0SIDH,0xFF,chipnum);
	MCP2515_Write_Byte(RXM0SIDL,0xFF,chipnum);
	/*10.�����ж�ʹ�ܼĴ���*/
	MCP2515_Write_Byte(CANINTE,RX0IE,chipnum);
	/*11.�����жϱ�־�Ĵ���*/
	MCP2515_Write_Byte(CANINTF,0x00,chipnum);
	/*12.��������ģʽ*/
	MCP2515_Write_Byte(CANCTRL,REQOP_NORM|OSM|CLKEN_D,chipnum);
	/*13.ȷ�Ͻ�������ģʽ*/
	canstate = MCP2515_Read_Byte(CANSTAT,chipnum);
	/*δ��������ģʽ�������ٽ�һ��*/
	if((canstate&(0x07<<5))) //
	{
	   	MCP2515_Write_Byte(CANCTRL,REQOP_NORM|OSM|CLKEN_D,chipnum);
	}
}
/*
1.��������MCP2515_Sender
2.������MCU��MCP2515��������
3.���룺unsigned char Data[]:Ҫ���͵�����,��'\0'����,unsigned char chipnum:оƬ��
4.�����unsigned char len:���͵��ֽ���
5.˵����MCU��MCP2515��������
*/
unsigned char MCP2515_Sender(unsigned char Data[],unsigned char chipnum)
{
	 unsigned char i=0;
	 /*�ǿղŷ�*/
	 if(Data[0]!='\0')
	 {
		 while(Data[i]!='\0')
		 {
		      MCP2515_Write_Byte(TXB0D0+i,Data[i],chipnum);
			  ++i;
		 }
		 MCP2515_Write_Byte(TXB0DLC,i,chipnum);//��MCP2515��֪Ҫ���͵��ֽ���
		 MCP2515_Write_Byte(TXB0CTRL,TXREQ|TXP,1);//�����ͣ��Ҳ��ƻ����ȼ�
		 return i;
	 }
	 return 0;
}

/*
1.��������MCP2515_Recver
2.������MCU����MCP2515��������
3.���룺unsigned char Data[]:���ݻ�����,unsigned char chipnum:оƬ��
4.�����unsigned char len
5.˵����MCU����MCP2515��������
*/
unsigned char MCP2515_Recver(unsigned char Data[],unsigned char chipnum)
{
	 unsigned char len,i=0;
	 unsigned char canintf;	 
	 canintf = MCP2515_Read_Byte(CANINTF,chipnum);//��ѯ��оƬ���޽����ж�
	 if(canintf&0x01) //�н����ж�����Ŷ�ȡ����
	 {
	     len = 	MCP2515_Read_Byte(RXB0DLC,chipnum);
	     len &= 0x0f;//�������ݳ���λ
		 while(i<len)
		 {
		     Data[i] = MCP2515_Read_Byte(RXB0D0+i,chipnum);
			 ++i;
		 }
		MCP2515_Write_Byte(CANINTF,canintf&0xFE,chipnum);//���жϱ�־
		return len;
	 }
	 return 0;
}

/*
*1.��������MCP2515_Write_Byte
*2. ��������MCP2515дһ�ֽ�����
*3. ���룺unsigned char Addr��Ҫд�ĵ�ַ,unsigned char Data��Ҫд������,
          unsigned char chipnum��ѡ�е�оƬ
*4. �������
*5. ����ֵ����
*6. ˵����ѡ��ĳ��MCP2515оƬ��Ȼ������д������
*/
static void MCP2515_Write_Byte(unsigned char Addr,unsigned char Data,unsigned char chipnum)
{
	  if(chipnum==1)
	  {
		  CS1 = 0;
		  SPI_Send_Byte(CAN_WRITE,chipnum);//�ȷ���д����
		  SPI_Send_Byte(Addr,chipnum);//�ٷ���д��ַ
		  SPI_Send_Byte(Data,chipnum);//�����Ҫд������
		  CS1 = 1;
	  }
	  else if(chipnum==2)
	  {
		  CS2 = 0;
		  SPI_Send_Byte(CAN_WRITE,chipnum);//�ȷ���д����
		  SPI_Send_Byte(Addr,chipnum);//�ٷ���д��ַ
		  SPI_Send_Byte(Data,chipnum);//�����Ҫд������
		  CS2 = 1;
	  }
}
/*
*1.��������MCP2515_Read_Byte
*2. ��������MCP2515��һ�ֽ�����
*3. ���룺unsigned char Addr��Ҫ���ĵ�ַ,unsigned char chipnum��ѡ�е�оƬ
*4. �����Data���������
*5. ����ֵ����
*6. ˵����ѡ��ĳ��MCP2515оƬ��Ȼ������д������
*/
static unsigned char MCP2515_Read_Byte(unsigned char Addr,unsigned char chipnum)
{
      unsigned char Data;
      if(chipnum==1)
	  {
		  CS1 = 0;
		  SPI_Send_Byte(CAN_READ,chipnum);//�ȷ���������
		  SPI_Send_Byte(Addr,chipnum);//�ٷ�������ַ
		  Data = SPI_Recv_Byte(chipnum);//������
		  CS1 = 1;
	  }
	  else if(chipnum==2)
	  {
		  CS2 = 0;
		  SPI_Send_Byte(CAN_READ,chipnum);//�ȷ���������
		  SPI_Send_Byte(Addr,chipnum);//�ٷ�������ַ
		  Data = SPI_Recv_Byte(chipnum);//������
		  CS2 = 1;
	  }
	  return Data;
}





