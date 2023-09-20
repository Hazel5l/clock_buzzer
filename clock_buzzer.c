#include <reg51.h>
code unsigned char table[]=
{0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x40};
unsigned char num[8]={2,3,10,5,9,10,5,0};
unsigned char index=0,index1,index2,x,y,hour=23,min=59,sec=50;
unsigned char ttthour=23,tttmin=00,tttsec=00;
unsigned char adjust,condition,clockkkk,buzc=0;
unsigned char key=16,key_pressed=0,update=0;
code unsigned char mapping[4][4] = 
{ {12, 7, 4, 1} , {0, 8, 5, 2} , {14, 9, 6, 3} , {15, 13, 11, 10} };
sbit int0=P3^2; 
sbit buzzer=P3^4;	//設定蜂鳴器
int cnt=0;

	
void ex_int0() interrupt 0
{
  EX0=0; //開啟外部中斷源INT0
	TH1=0x3C; //設定timer1時間
	TL1=0x80;
	TR1=1; //restart Timer1
}

void timer0() interrupt 1
{
	TR0=0; // 關timer0
	TH0=0xFC; //設定timer0時間(大約50ms)
	TL0=0x80;
	TR0=1;  //restart timer0
	
	// 7-seg led
	index=(index+1)%8; //每次(index都會+1)且除以8，index=該算式的餘數
	P1=index; 
	P2=table[num[index]]; //P2是選擇亮什麼，並且去table查表，對應數字的程式碼後，顯示出來
	
	// for updating num[]
	cnt=(cnt+1)%1000; //設定大約1秒
	if(cnt==0)
	{
	  update=1;
	}
}

void timer1() interrupt 3 
{
	TR1=0; //restart timer1
	if (key==16) //如果有按到16顆其中一顆按鍵
	{
	key_pressed=1; 
	}
	else //若沒有則
	{
		EX0=1; //關閉外部中斷源INT0
		P0=0xF0; //把P0關掉
		key=16;
	}
}
void buzzer_delay(int time) //設定蜂鳴器的時間，基本單位是1ms
{
	unsigned int i,j;
	for(i=0;i<time;i++)
	for(j=0;j<1275;j++);
}
void scan4() //七段顯示器顯示''時鐘''的時間
{
   P0=0xF0;	//把P0關掉
	 num[0]=hour/10; //小時除以十得到的商
	 num[1]=hour%10; //小時除以十得到的餘數
   num[3]=min/10;  //分鐘除以十得到的商
	 num[4]=min%10;  //分鐘除以十得到的餘數
	 num[6]=sec/10;  //秒除以十得到的商
	 num[7]=sec%10;  //秒除以十得到的餘數
}
void scan5()  //七段顯示器顯示''鬧鐘''的時間
{
   P0=0xF0;	//把P0關掉
	 num[0]=ttthour/10;  //小時除以十得到的商
	 num[1]=ttthour%10;  //小時除以十得到的餘數
   num[3]=tttmin/10;   //分鐘除以十得到的商
	 num[4]=tttmin%10;   //分鐘除以十得到的餘數
	 num[6]=tttsec/10;   //秒除以十得到的商
	 num[7]=tttsec%10;   //秒除以十得到的餘數
}
void scan() //***按鍵相關副程式***
{
  unsigned char tmp;
	tmp=16;
	
	for (index1=0;index1<4;index1++)   //掃描按鍵
	{
		if (index1==0) P0 = 0xFE;
		if (index1==1) P0 = 0xFD;
		if (index1==2) P0 = 0xFB;
		if (index1==3) P0 = 0xF7;
		x=P0 & 0x0F; //取左半
		y=P0 & 0xF0; //取右半
		if (y!=0xF0)  // a key is pressed
		{
			if (y==0xE0) index2=0;
			if (y==0xD0) index2=1;
			if (y==0xB0) index2=2;
			if (y==0x70) index2=3;
			tmp = mapping[index1][index2]; 
		}
	}	
	if(tmp==7)//若按KEY7，adjust=1，暫停-調整''時鐘''時間
	{
		adjust=1; 
  }
	if(adjust==1) //按了key7之後
	{
		scan4(); //呼叫副程式scan4，七段顯示器上顯''時鐘''時間
		if(tmp==1)  //若按KEY1，condition=1
		condition=1;
		if(tmp==2)  //若按KEY2，condition=2
		condition=2; 
		if(tmp==3)  //若按KEY3，condition=3
		condition=3;

		if(condition==1) //若condition=1，調整''時鐘的小時''
		{
			if(tmp==4) //如果按key4 
			{
				if(hour==23) //讓小時的範圍在0~23
				{
				  hour=0;
				}
				else
					hour++; //按一次就+1
		  }
			if(tmp==5) //如果按key5 
			{
				if(hour==0) //讓小時的範圍在0~23
				{
				  hour=23;
				}
				else
				  hour--; //按一次就-1
		  }
			scan4(); //七段顯示器上顯示''時鐘''的時間
		}
		
		if(condition==2)//若condition=2，調整''時鐘的分鐘''
		{
			if(tmp==4)  //如果按key4 
			{
				if(min==59) //讓分鐘的範圍在0~59
				{
				  min=0;
				}
				else
			  	min++; //按一次就+1
		  }
			if(tmp==5)  //如果按key5 
			{
  			if(min==0) //讓分鐘的範圍在0~59
				{
				  min=59;
				}
				else
				min--; //按一次就-1
		  }
			scan4(); //七段顯示器上顯示''時鐘''的時間
		}
		
		if(condition==3)  //若condition=2，調整''時鐘的分鐘''
		{
			if(tmp==4) //如果按key4 
			{
				if(sec==59) //讓秒數的範圍在0~59
				{
				  sec=0;
				}
				else
				sec++;	//按一次就+1			 
		  }
			if(tmp==5)  //如果按key5 
			{
				if(sec==0) //讓秒數的範圍在0~59
				{
				  sec=59;
				}
				else
				sec--; //按一次就-1
		  }
			scan4();  //七段顯示器上顯示''時鐘''的時間
	  }
	}
	
	if(tmp==8)  //若按KEY8，adjust=2，暫停-調整''鬧鐘''時間
	{
		adjust=2; 
  }
	if(adjust==2) //按了key8之後 
	{
		scan5(); //呼叫副程式scan5，七段顯示器上顯''鬧鐘''時間
		if(tmp==1) //若按KEY1，clockkkk=1
		clockkkk=1;
		if(tmp==2) //若按KEY2，clockkkk=2
		clockkkk=2;
		if(tmp==3) //若按KEY3，clockkkk=3
		clockkkk=3;
		
		if(clockkkk==1) //若clockkkk=1，調整''鬧鐘的小時''
		{
			if(tmp==4)  //如果按key4 
			{
				if(ttthour==23) //讓小時的範圍在0~23
				{
				  ttthour=0;
				}
				else
					ttthour++; //按一次就+1
		  }
			if(tmp==5)  //如果按key5 
			{
				if(ttthour==0) //讓小時的範圍在0~23
				{
				  ttthour=23;
				}
				else
				  ttthour--;   /*按一次就-1*/
		  }
			scan5(); //七段顯示器上顯示''鬧鐘''的時間
		}
		
		if(clockkkk==2) //若clockkkk=2，調整''鬧鐘的分鐘''
		{
			if(tmp==4)  //如果按key4 
			{
				if(tttmin==59)  //讓分鐘的範圍在0~59
				{
				  tttmin=0;
				}
				else
			  	tttmin++; //按一次就+1
		  }
			if(tmp==5)  //如果按key5 
			{
  			if(tttmin==0) //讓分鐘的範圍在0~59
				{
				  tttmin=59;
				}
				else
				tttmin--; /*按一次就-1*/
		  }
			scan5(); //七段顯示器上顯示''鬧鐘''的時間
		}
		
		if(clockkkk==3)//sec
		{
			if(tmp==4)  //如果按key4 
			{
				if(tttsec==59) //讓秒數的範圍在0~59
				{
				  tttsec=0;
				}
				else
				tttsec++; //按一次就+1
				
		  }
			if(tmp==5)  //如果按key5 
			{
				if(tttsec==0) //讓秒數的範圍在0~59
				{
				  tttsec=59;
				}
				else
				tttsec--;  /*按一次就-1*/
		  }
			scan5(); //七段顯示器上顯示''鬧鐘''的時間
	  }
	}
	if(tmp==6) //若按KEY6
	{
		scan4(); //七段顯示器上顯示''時鐘''的時間
		adjust=0; //adjust變回0
	}
	if(tmp==9) //若按key9
	buzc=1; 
	
	if (tmp != 16) 
		key=tmp;
	else
		EX0=1; //關閉外部中斷源INT0
}

void do_update() //''時鐘''進位
{ 
	sec++; //秒數+1
	if(sec==60) //若秒數數到60
	{
		sec=0; //秒歸零
		min++; //分鐘+1
		if(min==60)  //若分鐘數到60
		{
		  min=0; //分鐘歸零
		  hour++; //小時+1
			if(hour==24) //若小時數到24
			{
				hour=0; //小時歸零
			}
		}
	}
	 P0=0xF0; //把P0關掉
   num[0]=hour/10; //七段顯示器上顯示時鐘的時間
	 num[1]=hour%10;
   num[3]=min/10;
	 num[4]=min%10;
	 num[6]=sec/10;
	 num[7]=sec%10;
}
void main() {

	EA=1;  //開啟中斷總開關
	
	EX0=1; //enable timer0 interrupt
	IT0=1; //設定INT0為負源觸發
	P0=0xF0; //關掉P0
	
	ET1=1; //開啟timer1中斷
	TMOD=0x11; // Timer1 and Timer0
	
	ET0=1; //開啟timer0中斷
	TH0=0xFC; //設定TIMER0時間
	TL0=0x80;
	TR0=1; //開始計時
	
	while (1) //wait until interrupted
	{
		if(key_pressed) //如果開啟總中斷
		{
			scan(); //呼叫副程式scan
			key_pressed=0; //掃描後key_pressd再歸零
		}
		
		if((key!=16)&&(int0)) //如果沒有按到任何按鍵了話
		{
			TH1=0x3C; //設定TIMER1時間
			TL1=0x80;
			TR1=1; //TIMER1開始計時
		}
		if ((update==1)&&(adjust==0)) //如果update=1 且 adjust=0
		{
			if((hour==ttthour)&&(min==tttmin)&&(sec==tttsec)) //當鬧鐘=時鐘的時候
				{
					while(1) //無窮迴圈
					{
					 buzzer=0; //蜂鳴器開
					 buzzer_delay(1);// 副程式buzzer_delay執行一次=1ms
					 buzzer=1; //蜂鳴器關
					 buzzer_delay(1); // 副程式buzzer_delay執行一次=1ms
					 if(update==1) //時鐘自動更新
						 {
							 do_update(); //呼叫do_update副程式
							 update=0; //update歸零	
						 }
					if(key_pressed) //如果開啟總中斷
							{
								scan(); //呼叫副程式scan
								key_pressed=0;  //掃描後key_pressd再歸零
							}
					if((key!=16)&&(int0)) //如果沒有按到任何按鍵了話
						{
							TH1=0x3C; //設定TIMER1時間
							TL1=0x80;
							TR1=1; //TIMER1開始計時
							if (buzc==1) break;	//如果buzc=1，跳出迴圈
						}							
					}
				}
				else //若時鐘時間 != 鬧鐘時間，就自動更新:
      {	
        buzc=0;	//把buzc歸零			
				do_update(); //呼叫do_update副程式
				update=0;	//update歸零					
			}
		}
	}
}
