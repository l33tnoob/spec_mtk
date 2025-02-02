
/**************LPC1768 使用12MHZ晶振    加密芯片使用3.6864MHZ的晶振*******************/
#include <LPC17xx.h>

/*1us延时函数*/
void Delay_1us(char Sec)
{
	char i,k;
	for(k=0;k<Sec;k++)
	{
		for(i=0;i<18;i++);
	}
}
unsigned char buf[30];			 //数据存储区

/*发送命令函数：参数H = 1 为高速，H = 0 为低速 ; m 为接收字节的个数*/
void ReceArt(char H,char m)	   		   
{
	unsigned char i,n;
	unsigned char cIO;
	LPC_GPIO2->FIODIR = 0x0000007f;// io口为输入
	for(n=0;n<m;n++)
	{	
		while(LPC_GPIO2->FIOPIN & (1<<7));
		cIO = 0x00;
		if(H == 1)				    //H = 1 时为高速通讯
		{
			Delay_1us(3);	
		}
		else if(H == 0)				 //H = 0 时为低速通讯
		{
			Delay_1us(30);
		}
		for(i=0;i<8;i++)   		   //读一个字节函数 
		{
			if(H == 1)				 //H = 1 时为高速通讯每位时间
			{
				Delay_1us(8);
			}
			else if(H == 0)			  //H = 0 时为高速通讯每位时间
			{
				Delay_1us(100);
			} 
			cIO >>= 1;
			if(LPC_GPIO2->FIOPIN & (1<<7))
			{
				cIO |= 0x80;
			}
		}
		buf[n] =cIO;
		if(H == 1)
		{
			Delay_1us(24);	
		}
		else if(H == 0)
		{
			Delay_1us(100);
			Delay_1us(200);
		}	
	}
}

char	Send_1[4]={0xFF,0x10,0x95,0x7A} ;  //发送提速命令
char	Send_2[22]={0x80,0x08,0x00,0x00,0x11,0x02,0xAA,
						0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,
						0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff	};

char	Send_3[5]={0x00,0xc0,0x00,0x00,0x08};

/*发送命令函数：参数H = 1 为高速，H = 0 为低速 ; m 为发送字节的个数 ; SendBuf[]发送命令数组*/
void SendChar(char H,char m,char SendBuf[])
{
    char i,n,flag,uChar;

	 LPC_GPIO2->FIODIR = 0x000000ff;	  //设置成输出
	 
	for(n = 0 ;n<m ; n++)
	{	
		uChar  = SendBuf[n] ;
		flag = 0; 
		LPC_GPIO2->FIOPIN &= ~(1<<7);
		for(i=0;i<8;i++)			   //发送一个字节
		{
			if(H == 1)
			{
				Delay_1us(8);	
			}
			else if(H == 0)
			{
				Delay_1us(103);
			}	
			
			if(uChar & 0x01)
			{
				LPC_GPIO2->FIOPIN |= (1<<7);	//用于判断偶校验位
			    flag++;
			}
			else
			{
				LPC_GPIO2->FIOPIN &= ~(1<<7);
			}
			uChar >>=1;
		}

		if(H == 1)
		{
			Delay_1us(8);		
		}
		else if(H == 0)
		{
				Delay_1us(100);
		}

		if(flag%2 == 1)							 	//判断校验位
		{
			LPC_GPIO2->FIOPIN |= (1<<7);
		}
		else
		{
			LPC_GPIO2->FIOPIN &= ~(1<<7);
		}
		if(H == 1)								  //延时一个校验位
		{
			Delay_1us(8);	
		}
		else if(H == 0)
		{
			Delay_1us(100);
		}	
		LPC_GPIO2->FIOPIN |= (1<<7);
		if(H == 1)
		{											//延时两个停止位
			Delay_1us(16);	
		}
		else if(H == 0)
		{
			Delay_1us(200);
		}		   		
	}
}

/*引脚初始化*/
void Rest(void)
{
	LPC_GPIO2->FIODIR |= (1<<0);			//复位脚设置成输出
	LPC_GPIO2->FIODIR |= (1<<7);  			//数据脚设置成输出
	LPC_GPIO2->FIOPIN &= ~(1<<0); 			//复位脚拉低
 
	Delay_1us(100); 
	LPC_GPIO2->FIOPIN |= (1<<7);			 //数据较保持高电平 
	LPC_GPIO2->FIOPIN |= (1<<0);			 //复位脚拉高
}

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void)
{  
	char  temp;
	temp = Send_2[1];					  //保存第二个字节
	Rest();								   //复位
	ReceArt(0,17);						   //接收复位信息
	SendChar(0,4,Send_1);				   //发送提速命令
	ReceArt(0,4);

	SendChar(1,5,Send_2);				  //根据T=0协议先发送前五个字节
	ReceArt(1,1);

	if(buf[0] == temp)					 //判断接收的是不是第二个字节
	{	
		
		SendChar(1,17,&Send_2[5]);		   //发送剩下的字节
		ReceArt(1,2);	
		  									
	}	
			  
	if(buf[0]==0x61 && buf[1] ==0x08 )	 	 //判断返回信息
	{
		SendChar(1,5,Send_3) ;				//发送获取数据命令 
		ReceArt(1,11);
	}  	
	while(1)  ;
}

