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
sbit buzzer=P3^4;	//�]�w���ﾹ
int cnt=0;

	
void ex_int0() interrupt 0
{
  EX0=0; //�}�ҥ~�����_��INT0
	TH1=0x3C; //�]�wtimer1�ɶ�
	TL1=0x80;
	TR1=1; //restart Timer1
}

void timer0() interrupt 1
{
	TR0=0; // ��timer0
	TH0=0xFC; //�]�wtimer0�ɶ�(�j��50ms)
	TL0=0x80;
	TR0=1;  //restart timer0
	
	// 7-seg led
	index=(index+1)%8; //�C��(index���|+1)�B���H8�Aindex=�Ӻ⦡���l��
	P1=index; 
	P2=table[num[index]]; //P2�O��ܫG����A�åB�htable�d��A�����Ʀr���{���X��A��ܥX��
	
	// for updating num[]
	cnt=(cnt+1)%1000; //�]�w�j��1��
	if(cnt==0)
	{
	  update=1;
	}
}

void timer1() interrupt 3 
{
	TR1=0; //restart timer1
	if (key==16) //�p�G������16���䤤�@������
	{
	key_pressed=1; 
	}
	else //�Y�S���h
	{
		EX0=1; //�����~�����_��INT0
		P0=0xF0; //��P0����
		key=16;
	}
}
void buzzer_delay(int time) //�]�w���ﾹ���ɶ��A�򥻳��O1ms
{
	unsigned int i,j;
	for(i=0;i<time;i++)
	for(j=0;j<1275;j++);
}
void scan4() //�C�q��ܾ����''����''���ɶ�
{
   P0=0xF0;	//��P0����
	 num[0]=hour/10; //�p�ɰ��H�Q�o�쪺��
	 num[1]=hour%10; //�p�ɰ��H�Q�o�쪺�l��
   num[3]=min/10;  //�������H�Q�o�쪺��
	 num[4]=min%10;  //�������H�Q�o�쪺�l��
	 num[6]=sec/10;  //���H�Q�o�쪺��
	 num[7]=sec%10;  //���H�Q�o�쪺�l��
}
void scan5()  //�C�q��ܾ����''�x��''���ɶ�
{
   P0=0xF0;	//��P0����
	 num[0]=ttthour/10;  //�p�ɰ��H�Q�o�쪺��
	 num[1]=ttthour%10;  //�p�ɰ��H�Q�o�쪺�l��
   num[3]=tttmin/10;   //�������H�Q�o�쪺��
	 num[4]=tttmin%10;   //�������H�Q�o�쪺�l��
	 num[6]=tttsec/10;   //���H�Q�o�쪺��
	 num[7]=tttsec%10;   //���H�Q�o�쪺�l��
}
void scan() //***��������Ƶ{��***
{
  unsigned char tmp;
	tmp=16;
	
	for (index1=0;index1<4;index1++)   //���y����
	{
		if (index1==0) P0 = 0xFE;
		if (index1==1) P0 = 0xFD;
		if (index1==2) P0 = 0xFB;
		if (index1==3) P0 = 0xF7;
		x=P0 & 0x0F; //�����b
		y=P0 & 0xF0; //���k�b
		if (y!=0xF0)  // a key is pressed
		{
			if (y==0xE0) index2=0;
			if (y==0xD0) index2=1;
			if (y==0xB0) index2=2;
			if (y==0x70) index2=3;
			tmp = mapping[index1][index2]; 
		}
	}	
	if(tmp==7)//�Y��KEY7�Aadjust=1�A�Ȱ�-�վ�''����''�ɶ�
	{
		adjust=1; 
  }
	if(adjust==1) //���Fkey7����
	{
		scan4(); //�I�s�Ƶ{��scan4�A�C�q��ܾ��W��''����''�ɶ�
		if(tmp==1)  //�Y��KEY1�Acondition=1
		condition=1;
		if(tmp==2)  //�Y��KEY2�Acondition=2
		condition=2; 
		if(tmp==3)  //�Y��KEY3�Acondition=3
		condition=3;

		if(condition==1) //�Ycondition=1�A�վ�''�������p��''
		{
			if(tmp==4) //�p�G��key4 
			{
				if(hour==23) //���p�ɪ��d��b0~23
				{
				  hour=0;
				}
				else
					hour++; //���@���N+1
		  }
			if(tmp==5) //�p�G��key5 
			{
				if(hour==0) //���p�ɪ��d��b0~23
				{
				  hour=23;
				}
				else
				  hour--; //���@���N-1
		  }
			scan4(); //�C�q��ܾ��W���''����''���ɶ�
		}
		
		if(condition==2)//�Ycondition=2�A�վ�''����������''
		{
			if(tmp==4)  //�p�G��key4 
			{
				if(min==59) //���������d��b0~59
				{
				  min=0;
				}
				else
			  	min++; //���@���N+1
		  }
			if(tmp==5)  //�p�G��key5 
			{
  			if(min==0) //���������d��b0~59
				{
				  min=59;
				}
				else
				min--; //���@���N-1
		  }
			scan4(); //�C�q��ܾ��W���''����''���ɶ�
		}
		
		if(condition==3)  //�Ycondition=2�A�վ�''����������''
		{
			if(tmp==4) //�p�G��key4 
			{
				if(sec==59) //����ƪ��d��b0~59
				{
				  sec=0;
				}
				else
				sec++;	//���@���N+1			 
		  }
			if(tmp==5)  //�p�G��key5 
			{
				if(sec==0) //����ƪ��d��b0~59
				{
				  sec=59;
				}
				else
				sec--; //���@���N-1
		  }
			scan4();  //�C�q��ܾ��W���''����''���ɶ�
	  }
	}
	
	if(tmp==8)  //�Y��KEY8�Aadjust=2�A�Ȱ�-�վ�''�x��''�ɶ�
	{
		adjust=2; 
  }
	if(adjust==2) //���Fkey8���� 
	{
		scan5(); //�I�s�Ƶ{��scan5�A�C�q��ܾ��W��''�x��''�ɶ�
		if(tmp==1) //�Y��KEY1�Aclockkkk=1
		clockkkk=1;
		if(tmp==2) //�Y��KEY2�Aclockkkk=2
		clockkkk=2;
		if(tmp==3) //�Y��KEY3�Aclockkkk=3
		clockkkk=3;
		
		if(clockkkk==1) //�Yclockkkk=1�A�վ�''�x�����p��''
		{
			if(tmp==4)  //�p�G��key4 
			{
				if(ttthour==23) //���p�ɪ��d��b0~23
				{
				  ttthour=0;
				}
				else
					ttthour++; //���@���N+1
		  }
			if(tmp==5)  //�p�G��key5 
			{
				if(ttthour==0) //���p�ɪ��d��b0~23
				{
				  ttthour=23;
				}
				else
				  ttthour--;   /*���@���N-1*/
		  }
			scan5(); //�C�q��ܾ��W���''�x��''���ɶ�
		}
		
		if(clockkkk==2) //�Yclockkkk=2�A�վ�''�x��������''
		{
			if(tmp==4)  //�p�G��key4 
			{
				if(tttmin==59)  //���������d��b0~59
				{
				  tttmin=0;
				}
				else
			  	tttmin++; //���@���N+1
		  }
			if(tmp==5)  //�p�G��key5 
			{
  			if(tttmin==0) //���������d��b0~59
				{
				  tttmin=59;
				}
				else
				tttmin--; /*���@���N-1*/
		  }
			scan5(); //�C�q��ܾ��W���''�x��''���ɶ�
		}
		
		if(clockkkk==3)//sec
		{
			if(tmp==4)  //�p�G��key4 
			{
				if(tttsec==59) //����ƪ��d��b0~59
				{
				  tttsec=0;
				}
				else
				tttsec++; //���@���N+1
				
		  }
			if(tmp==5)  //�p�G��key5 
			{
				if(tttsec==0) //����ƪ��d��b0~59
				{
				  tttsec=59;
				}
				else
				tttsec--;  /*���@���N-1*/
		  }
			scan5(); //�C�q��ܾ��W���''�x��''���ɶ�
	  }
	}
	if(tmp==6) //�Y��KEY6
	{
		scan4(); //�C�q��ܾ��W���''����''���ɶ�
		adjust=0; //adjust�ܦ^0
	}
	if(tmp==9) //�Y��key9
	buzc=1; 
	
	if (tmp != 16) 
		key=tmp;
	else
		EX0=1; //�����~�����_��INT0
}

void do_update() //''����''�i��
{ 
	sec++; //���+1
	if(sec==60) //�Y��Ƽƨ�60
	{
		sec=0; //���k�s
		min++; //����+1
		if(min==60)  //�Y�����ƨ�60
		{
		  min=0; //�����k�s
		  hour++; //�p��+1
			if(hour==24) //�Y�p�ɼƨ�24
			{
				hour=0; //�p���k�s
			}
		}
	}
	 P0=0xF0; //��P0����
   num[0]=hour/10; //�C�q��ܾ��W��ܮ������ɶ�
	 num[1]=hour%10;
   num[3]=min/10;
	 num[4]=min%10;
	 num[6]=sec/10;
	 num[7]=sec%10;
}
void main() {

	EA=1;  //�}�Ҥ��_�`�}��
	
	EX0=1; //enable timer0 interrupt
	IT0=1; //�]�wINT0���t��Ĳ�o
	P0=0xF0; //����P0
	
	ET1=1; //�}��timer1���_
	TMOD=0x11; // Timer1 and Timer0
	
	ET0=1; //�}��timer0���_
	TH0=0xFC; //�]�wTIMER0�ɶ�
	TL0=0x80;
	TR0=1; //�}�l�p��
	
	while (1) //wait until interrupted
	{
		if(key_pressed) //�p�G�}���`���_
		{
			scan(); //�I�s�Ƶ{��scan
			key_pressed=0; //���y��key_pressd�A�k�s
		}
		
		if((key!=16)&&(int0)) //�p�G�S������������F��
		{
			TH1=0x3C; //�]�wTIMER1�ɶ�
			TL1=0x80;
			TR1=1; //TIMER1�}�l�p��
		}
		if ((update==1)&&(adjust==0)) //�p�Gupdate=1 �B adjust=0
		{
			if((hour==ttthour)&&(min==tttmin)&&(sec==tttsec)) //��x��=�������ɭ�
				{
					while(1) //�L�a�j��
					{
					 buzzer=0; //���ﾹ�}
					 buzzer_delay(1);// �Ƶ{��buzzer_delay����@��=1ms
					 buzzer=1; //���ﾹ��
					 buzzer_delay(1); // �Ƶ{��buzzer_delay����@��=1ms
					 if(update==1) //�����۰ʧ�s
						 {
							 do_update(); //�I�sdo_update�Ƶ{��
							 update=0; //update�k�s	
						 }
					if(key_pressed) //�p�G�}���`���_
							{
								scan(); //�I�s�Ƶ{��scan
								key_pressed=0;  //���y��key_pressd�A�k�s
							}
					if((key!=16)&&(int0)) //�p�G�S������������F��
						{
							TH1=0x3C; //�]�wTIMER1�ɶ�
							TL1=0x80;
							TR1=1; //TIMER1�}�l�p��
							if (buzc==1) break;	//�p�Gbuzc=1�A���X�j��
						}							
					}
				}
				else //�Y�����ɶ� != �x���ɶ��A�N�۰ʧ�s:
      {	
        buzc=0;	//��buzc�k�s			
				do_update(); //�I�sdo_update�Ƶ{��
				update=0;	//update�k�s					
			}
		}
	}
}