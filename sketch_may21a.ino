#include <SoftwareSerial.h>
SoftwareSerial mySerial(3, 2); //블루투스의 Tx, Rx핀을 2번 3번핀으로 설정

int onoff = -1;
int mode = 1; //1 auto
void blind(int ac)//on일때 
{
        if (ac == 0)
        {
                if (onoff != ac)
                {
                                                onoff = ac;
                        digitalWrite(12 , 1);
                        delay(6300);
                        digitalWrite(12 , 0);
                }
        }
        else if (ac == 1)
        {
                if (onoff != ac)
                {
                        onoff = ac;
                        digitalWrite(13 , 1);
                        delay(6300);
                        digitalWrite(13 , 0);
                }
        }

}
void setup() {
        // 시리얼 통신의 속도를 9600으로 설정 시리얼통신 :1:1 통신
        Serial.begin(9600);
        //블루투스와 아두이노의 통신속도를 9600으로 설정
        mySerial.begin(9600); //블루투스 모듈과 통신속도를 맞춤
        pinMode(12, OUTPUT);
        pinMode(13, OUTPUT);
}

long sendTime;
long diffTime;
long reserveTime;
char reserveAc = '-';
void loop() { //코드를 무한반복합니다.
        long now = millis();

        if (diffTime > 0 && now - reserveTime > diffTime)
        {
                mode = 0;
                reserveTime = 0;
                diffTime = 0;
                if (reserveAc == 'o')
                {
                        Serial.println("r1");
                        blind(1);
                }
                                else if (reserveAc == 'c')
                {
                        Serial.println("r0");
                        blind(0);
                }
        }

        int cds = analogRead(A0);//조도
        if (mode == 1)
        {
                if (cds > 800)
                        blind(1);
                else if (cds < 300)
                        blind(0);
        }
        if (now - sendTime > 1000)
        {
                sendTime = now;
                mySerial.println(cds);
        }

        /*      if (Serial.available() > 0) { //블루투스에서 넘어온 데이터가 있다면
                char data = Serial.read();
                mySerial.write(data); //시리얼모니터에 데이터를 출력
                }*/
        if (mySerial.available() > 0) { //블루투스에서 넘어온 데이터가 있다면
                String inString = mySerial.readStringUntil('\n');

                Serial.println(inString); //시리얼모니터에 데이터를 출력
                Serial.println(inString[0]); //시리얼모니터에 데이터를 출력
                if (inString.length() > 2) //예약
                {
                        diffTime = atol(inString.substring(2, inString.length()).c_str());
                        Serial.println(diffTime);
                        reserveTime = now;
                        reserveAc = inString[0];
                        //이렇게 예약된시간과 현재시간의 차이를 구해서 차이값을 보내게되면
                        // or49128 cr49128 메세지가 전송되는데
                        //or은 open 예약 cr 은 클로즈 예약
                                                // 뒤의 숫자는 millis 초 후에 동작을 하도록 보냄
                        // 바로 다음 분인 현재 44분 예약 45 분일경우
                        // 45분 00초 00에서 44분 현재초 현재 밀리초를 뺼경우
                        //60000이하의 값이 나오게되고 49128 은  49초 128후에 동작을 하게됨

                }
                else
                {
                        switch (inString[0])
                        {
                                case 'o':
                                        mode = 0;
                                        blind(1);
                                        break;
                                case 'c':
                                        blind(0);
                                        mode = 0;
                                        break;
                                case 'a':
                                        mode = 1;
                                        break;
                        }
                }

        }


}
