  void setup()
 {
    //设置引脚为输出模式
    pinMode(3,OUTPUT);
    pinMode(4,OUTPUT);
    pinMode(5,OUTPUT);
    //GSM开机时序 
    digitalWrite(5,HIGH);
    delay(1500);
    digitalWrite(5,LOW); 
 }
 void loop()    
 {
    digitalWrite(3,LOW);//使能GSM的TX、RX
    digitalWrite(4,HIGH);//关闭GPS的TX、RX
 }
