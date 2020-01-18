
#include <string.h>
#include <stdio.h>
#include <STC12C5A60S2.H>

#define      S2RI             0x01    //S2CON.1   
#define      S2TI             0x02    //S2CON.2   
#define      S2RB8            0x04    //S2CON.3   
#define      S2TB8            0x08    //S2CON.4  

void wait_response(unsigned int);
unsigned int string_compare, verified=0;
unsigned char buff[70];
void GSM_Response();
void do_something(void);
unsigned char start_gsm = 0;				
volatile int buffer_pointer=0;

/*DELVIN PART*/
void actuator_msdelay(int)
void main(void){

sbit steppin=P2^6;
sbit direction=P2^7;


/*xizhe part*/


void Init_Uart2(void)
{	
	//AUXR &= 0xF7;		//Baudrate no doubled
	S2CON = 0x50;		//8bit and variable baudrate
	//AUXR &= 0xFB;		//BRT's clock is Fosc/12 (12T)
	BRT = 0xFD;		//Set BRT's reload value
	AUXR = 0x11;		//BRT running
	AUXR1=0x00;
	IE2=0x01;
	EA=1;
}

void Init_Uart1(void)
{
	PCON &= 0x7F;		
	SCON = 0x50;		//0101 0000, 1 startbit,1 stopbit
	TMOD &= 0x0F;		
	TMOD = 0x21;		//0010 1111, mode2-autoreload, 
	TL1 = 0xFD;		
	TH1 = 0xFD;	
	TR1 = 1;		
	ES=1;
	EA=1;
}



void Delay_Xms(unsigned int x)
{	
	unsigned char i, j;

	while(x--)
	{
		i = 2;
		j = 199;
		do
		{
			while (--j);
		} while (--i);
	}
}

void SendByte_UART2(unsigned char dat)
{
	unsigned char temp=0;
	IE2=0x00;
	S2CON=S2CON&0xFD;
	S2BUF=dat;
	do{
		temp=S2CON;
		temp=temp&0x02;
	}while(temp==0);
	S2CON=S2CON&0xFD;
	IE2=0x01;
	
}

void SendString_UART2(unsigned char *p)
{
	while(*p)
	{
		SendByte(*p++);
	}
	p = NULL;
}

void SendByte_UART1(unsigned char dat)
{
	ES=0;
	TI=0;
	SBUF=dat;
	while(!TI);
	TI=0;
	ES=1;
}

void SendString_UART1(unsigned char *p)
{
	while(*p)
	{
		SendByte(*p++);
	}
	p = NULL;
}


void Online(void)
{
	//start:
	start_gsm=1;
	Delay_Xms(1500);
	SendString_UART1("AT+CIPSHUT\r\n");
	//do_something();
	wait_response(1);
	Delay_Xms(100);
	SendString("AT+CIPMUX=0\r\n");
	wait_response(2);
	Delay_Xms(100);
	SendString("AT+CGATT=1\r\n");
	wait_response(2);
	Delay_Xms(100);
	SendString("AT+CSTT=\"diginet\",\"\",\"\"\r\n");
	wait_response(2);
	Delay_Xms(100);
	SendString("AT+CIICR\r\n");
	wait_response(2);
	Delay_Xms(100);
	SendString("AT+CIFSR\r\n");
	wait_response(0);
	Delay_Xms(100);
	SendString("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80 \r\n");
	Delay_Xms(300);
	wait_response(3);
	Delay_Xms(100);
	SendString("AT+CIPSEND  \r\n");
	wait_response(4);
	Delay_Xms(100);
	SendString("GET https://api.thingspeak.com/update?api_key=2A3T6MDHMI7AYUTN&field1=100 \r\n");
	SendByte(0x1A);
	wait_response(5);
	start_gsm=0;
}

void Uart_isr (void) interrupt 8 using 1
{
	unsigned char k=0;
	k=S2CON;
	k=k&0x01;
	if (k==1){
		if(start_gsm==1){
			S2CON = S2CON&0xFE;
			buff[buffer_pointer] = S2BUF;
			buffer_pointer++;
		}else{
			//read_status = SBUF;
			S2CON=S2CON & 0xFE;
		}
	}else{
		S2CON=S2CON & 0xFD;
	}
}

void main(void){
	int x;
	Init_Uart();
	
	Online();
	
	while(1){
		direction=1;
		for(x = 0; x < 3800; x++){ 
			steppin=1;
			actuator_msdelay(500);
			steppin=0;
			actuator_msdelay(500);
		}
		
		direction=0;//Forward
		for(x = 0; x < 3800; x++){ 
			steppin=1;
			actuator_msdelay(500);
			steppin=0;
			actuator_msdelay(500);
		}
	}
	
}

void wait_response(unsigned alp){
	while(1){
		if (alp == 0){
			Delay_Xms(1000);
			memset(buff, 0, 70*sizeof(buff[0]));
			buffer_pointer=0;
			break;
		}else if (alp == 1){
			if(strstr(buff,"SHUT OK")){
				//GSM_Response();			/* get Response */
				memset(buff, 0, 70*sizeof(buff[0]));
				buffer_pointer=0;
				break;
			}
		}else if (alp == 2){
			if(strstr(buff,"OK")){
				//GSM_Response();			/* get Response */
				memset(buff, 0, 70*sizeof(buff[0]));
				buffer_pointer=0;
				break;
			}
		}else if (alp == 3){
			if(strstr(buff,"CONNECT OK")){
				memset(buff, 0, 70*sizeof(buff[0]));
				buffer_pointer=0;
				break;
			}
		}else if (alp == 4){
			if(strstr(buff,">")){
				Delay_Xms(200);
				memset(buff, 0, 70*sizeof(buff[0]));
				buffer_pointer=0;
				break;
			}
		}else if (alp == 5){
			if(strstr(buff,"CLOSED")){
				memset(buff, 0, 70*sizeof(buff[0]));
				buffer_pointer=0;
				break;
			}
		}
	}
}

void actuator_msdelay(int x){
int i;

for(i=0;i<x;i++);
}
