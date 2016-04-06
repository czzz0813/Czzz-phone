#include <Keypad.h>
#include "LCD12864RSPI.h"
#define AR_SIZE( a ) sizeof( a ) / sizeof( a[0] )
const byte ROWS=4;
const byte COLS=4;
char keys[ROWS][COLS]=
{
  {'*','0','#','D'},
  {'7','8','9','C'},
   {'4','5','6','B'},
  {'1','2','3','A'}
};
boolean ring=false;
byte rowPins[ROWS] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {10, 11, 12, 13}; //connect to the column pinouts of the keypad
Keypad keypad1= Keypad(makeKeymap(keys),rowPins,colPins,ROWS,COLS);
String nums="";
String tmp="";
String gret="";
String   xh="No Signal";
boolean isGprs=false;
int reset=0;
unsigned long resetTime;
unsigned long xhStarTime=0;
String commingTel="";
int smsNo=13;
unsigned long noOpStar=0;
//millis();
void setup()
  { 
    LCDA.Initialise(); // 屏幕初始化

delay(100);
  Serial.begin(9600); 
   initGSM();
   noOpStar=millis();
    pinMode(2,OUTPUT); // bebe
  }
 void initGSM()
{
       xh="No Signal";
   dlcd("CzzzPhone init");
 
      //端口模式设置
      pinMode(3,OUTPUT);
      pinMode(4,OUTPUT);
      pinMode(5,OUTPUT);
     //GSM开机时序
     digitalWrite(5,HIGH);
     delay(1500);
     digitalWrite(5,LOW);  
    
     //使能GSM串口
     digitalWrite(3,LOW);
     digitalWrite(4,HIGH);
     delay(2000);
     //设置波特率
    
   //  Serial1.begin(9600);
     //等待call ready
     delay(5000);delay(5000);delay(5000);
     int i=0;
     if (!atOp(5,"AT","OK",true))
     {  
       return;
     }
     else
     {
       dlcd("AT OK");
     }
     delay(100);
     if (!atOp(5,"AT+CPIN?","READY",true))
     {
       return;
     }
     else
     {
       dlcd("pin ok");
     }
     delay(100);
     if (!atOp(5,"AT+CSQ",",0",true))
     {
       return;
     }
     else
     {
       xh=gret.substring(gret.indexOf(":",0)+2);
       xh=xh.substring(0,xh.indexOf(",",0));
       if (xh.toInt()<18)
        xh=xh+" Low"; 
      // dlcd(xh,0,1);
       dlcd("CSQ ok");
      //return;
      // dlcd("CSQ ok");
     }
     delay(100);
     if (!atOp(5,"AT+CREG?","0,1",true))
     {
       return;
     }
     else
     {
       
       dlcd("reg ok");
     }
     delay(100);
     if (atOp(1,"AT+CGREG?","0,1",false))
     {
       isGprs=true;
       dlcd("greg ok");
     }
     delay(100);
     //来电显示
    if (atOp(1,"AT+CLIP=1","OK",false))
     {
       dlcd("clip ok");
     }
//rr     return;
    if (isGprs)
      xh=xh + " GPRS";
    else
      xh=xh+ " GSM";
      dlcd("   CzzzPhone");
     //Serial.println("ATD18601128027;");
      //    Serial.println("ATD18601220139;");
     //Serial.flush();
     // delay(6000);
     // d();

} 

void getxh()
{
  if (xhStarTime==0)   xhStarTime=millis();
  if (millis()-xhStarTime<60000)
    return;
  xhStarTime=millis();
  
   if (!atOp(2,"AT+CSQ",",0",false))
     {
       xh="No Signal";
     }
     else
     {
       xh=gret.substring(gret.indexOf(":",0)+2);
       xh=xh.substring(0,xh.indexOf(",",0));
       if (xh.toInt()<18)
        xh=xh+" Low"; 
    if (isGprs)
      xh=xh + " GPRS";
    else
       xh=xh+ " GSM";
      // dlcd(xh,0,1);
      // dlcd("CSQ ok");
      //return;
      // dlcd("CSQ ok");
     }
  
}

  
  
  void loop()
  {    
   d();
  call();
  if (millis()-noOpStar>10000)
    {
        dlcd("   CzzzPhone");
        noOpStar=millis();
    }
  
  
 // getxh();
 
//     //发送AT命令同步
//     call();
//     

     //打电话
     
    // while(1);
  }
  
  void d()
{
  
  tmp="";
    while(Serial.available()>0)
  {
       
    char c=Serial.read();
    if (c != '\n' && c!='\r')
        tmp=tmp+c;
    delay(2);
    
      //Serial.write(mySerial.read());
    // if (c = '\n') break;  // got one Line 
  }
    Serial.flush();
  if(tmp!="")
  {
   // tmp=tmp+'\0'
   //call in
    if (tmp.indexOf("RING",0)>0 || tmp.indexOf("CLIP:",0)>0 ) 
    
    {
      if (tmp.indexOf("CLIP:",0)>0)
      {
        int star=tmp.indexOf("+CLIP:")+8;
        commingTel=tmp.substring(star,star+11);
        tmp=commingTel+" comming";
        ring=true;
        dlcd(tmp);
        digitalWrite(2,HIGH);
        ///beebee
      }
    }
    else
    {
       ring=false;
         digitalWrite(2,LOW);
       ///no bebe
    }
   if (tmp.indexOf("ATA",0)>=0)
   {
     tmp=commingTel+" talking";
     dlcd(tmp);
   }
    
    
    
    if (tmp.indexOf("ATD",0)>=0)
    {
       tmp="Dialing "+tmp.substring(3,14);
        dlcd(tmp);
    }
   if (tmp.indexOf("ATH",0)>=0)
   {
     tmp="Hung up";
     dlcd(tmp);
   }
   if (tmp.indexOf("BUSY",0)>=0)
   {
     tmp="Busy";
     dlcd(tmp);
   }
   if (tmp.indexOf("NO CARRIER",0)>=0)
   {
     tmp="Hung up!";
     dlcd(tmp);
   }
      if (tmp.indexOf("COLP",0)>=0)
   {
     tmp=tmp.substring(8,19);
     dlcd(tmp +" talking");
   }
     //new message
     if (tmp.indexOf("CMTI",0)>=0)
    {
      tmp=tmp.substring(12);
      smsNo=tmp.toInt();
      tmp="new message press,press C read" ;
       dlcd(tmp);
    }
    //read sms
     if (tmp.indexOf("+CMGR: ",0)>=0)
    {
     int st=tmp.indexOf("+CMGR: \"REC");
     if (tmp.indexOf("REC UNREAD",0)>=0)
     tmp=tmp.substring(st+62,tmp.length()-2);
     else
     tmp=tmp.substring(st+60,tmp.length()-2);
      dlcd(tmp);
    }
   // tmp="//"+tmp;
  //Serial.println("tmp");
  //Serial.println("fuck");
 
  }
  
}
//+CMGR: "REC UNREAD","+8618601220139","","16/03/27,04:18:41+32"
//+CMGR: "REC READ","+8618601220139","","16/03/27,03:49:46+32"
  void call()
  {
      char key=keypad1.getKey();

      if(key!=NO_KEY)
      {
        if( key=='A' || key=='B' || key=='C' || key=='D')
        {
          if (key=='A')  //ata
          {
             if (ring)
             {
                Serial.println("ATA");
                ring=false;
             }
             else  if (nums!="")
             {
              
                  nums=nums+';';
                  Serial.println("ATD"+nums);//对方电话号码
                  dlcd(nums+" calling");
                  nums="";
              
              }
              else if (nums=="")
              {
                  dlcd("nums error");
              }
          }
          else if (key=='B')
          {
              Serial.println("ATH");
              ring=false;            
          }
          else if (key=='C')
          {
          //  if (smsNo>=0)
           //{
            Serial.println("AT+CMGR="+String(smsNo));
            delay(200);
          // }  
        }
          else if(key=='D') //reset gsm
          {
            
            if (reset==0)
            {
              resetTime=millis();
              dlcd("press D again reset GSM!");
              reset++;
            }
            else
            {
              if (millis()-resetTime<2000)
              initGSM();
              else
              {
                 dlcd("press D again reset GSM!");
                 reset=0;
              }
            }
          }
          return;
        }
        
        if (key=='*')
        {
           //Serial.println("*");
            nums="";
            dlcd(nums);
            //Serial.print("ok*");
         }
        else if (key!='#')
        {
            nums=nums+key;
            dlcd(nums);     
        }
        
      
  }
  }
  
  void dlcd(String x)
  {
    noOpStar=millis();
        //  LCDA.Initialise(); // 屏幕初始化
//LCDA.CLEAR();//清屏
//LCDA.DisplayString(m,n,str,length));//m代表的是行，n代表的是列，str代表的是要显示内容的名字，length是字符串的长度
//LCDA.DrawFullScreen(logo0)//满屏显示内容
LCDA.CLEAR();//清屏
delay(100);


int len=xh.length();
char showx1[len+1];
///String.toCharArray(x,showx,x.length);
 //char clientStr[34];
  xh.toCharArray(showx1,len+1);
  unsigned char ushowx1[len];
  for (int i=0;i<=len;i++)
  {
    ushowx1[i]=showx1[i];
  }
//Serial.println(x.length());
LCDA.DisplayString(0,0,ushowx1,AR_SIZE(ushowx1));//第一行第三格开始，显示文智位机器人

//delay(20);


len=x.length();
char showx[len+1];
///String.toCharArray(x,showx,x.length);
 //char clientStr[34];
  x.toCharArray(showx,len+1);
  unsigned char ushowx[len];
  for (int i=0;i<=len;i++)
  {
    ushowx[i]=showx[i];
  }

LCDA.DisplayString(1,0,ushowx,AR_SIZE(ushowx));//第一行第三格开始，显示文智位机器人
//delay(2000);

  }
  
  


///a  
boolean atOp(int num,String command, String ret,boolean needDisplay)
{
  for (int i=0;i<num;i++)
     {
       Serial.println(command);
       delay(500);
       if (getrtn(ret)==true)
       {
            Serial.flush();
         return true;
       }
       if (i<num-1)
       {
        
         delay(500);
    
       }
       else
       {
         if(gret!="")
         {
           if (needDisplay)
           dlcd(gret);
         }
         else
         {
           if (needDisplay)
           dlcd(command+" : unknow error!");
         }
          Serial.flush();
         return false;
       }

     }
      Serial.flush();
     return true;
}

  
boolean getrtn(String info)
{
   tmp="";
    while(Serial.available()>0)
  {
       
    char c=Serial.read();
   if (c != '\n' && c!='\r')
        tmp=tmp+c;
    
    //Serial.print(c);  
    delay(2);
    
      //Serial.write(mySerial.read());
    // if (c = '\n') break;  // got one Line 
  }
  gret =tmp;
  Serial.flush();
 if (tmp.indexOf(info,0)>0)
{
  return true;
} 
else
{
  return false;
}
  
}

