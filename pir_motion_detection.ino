#include <M5StickCPlus.h>
#include <Wire.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <ESP_Mail_Client.h>

bool Flag = 0;
const int LED = 10;
int vbat;
int discharge;
int PIR = 36;
int RL = 33;

#define WIFI_SSID "*****"
#define WIFI_PASSWORD "*****"

#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

#define AUTHOR_EMAIL "********"
#define AUTHOR_PASSWORD "******"

#define RECIPIENT_EMAIL "********"

SMTPSession smtp;
String htmlMsg = "<div style=\"color:#2f4468;\"><h1>Motion Detected!!!!!</h1><p>- Sent from ESP board</p></div>";
SMTP_Message message;
ESP_Mail_Session session;

//void smtpCallback(SMTP_Status status);

uint8_t bat_1[] =
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x6e, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdb, 0x00, 0x00,
  0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
  0x00, 0xdb, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
  0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
  0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
  0x00, 0xb7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

uint8_t bat_2[] =
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x6e, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdb, 0x00, 0x00,
  0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00,
  0x00, 0xdb, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00,
  0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00,
  0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00,
  0x00, 0xb7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

uint8_t bat_3[] =
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x6e, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdb, 0x00, 0x00,
  0x00, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00,
  0x00, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00,
  0x00, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00,
  0x00, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00,
  0x00, 0xb7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};


void setup()
{
  pinMode(10, OUTPUT);          //led pin 10
  pinMode(PIR, INPUT_PULLUP);   // Pin PIR
  pinMode(RL, OUTPUT);          // pin RL grove relay
  M5.begin();
  M5.Lcd.println();
  //M5.Lcd.print("Connecting to AP");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED){
    //M5.Lcd.print(".");
    delay(200);
  }
  
  smtp.debug(1);
  //smtp.callback(smtpCallback);
  
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "";
  
  message.sender.name = "ESP";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "ESP Test Email";
  message.addRecipient("Manvi", RECIPIENT_EMAIL);
  M5.Lcd.setRotation(3);        //  1 ou 3
  M5.Lcd.fillScreen(BLACK);
  M5.Axp.ScreenBreath(9);       // brightness
  M5.Lcd.setCursor(0, 0, 2);
  M5.Lcd.setTextColor(LIGHTGREY, BLACK);
  M5.Lcd.println(" Detection PIR -> USB");
  M5.Lcd.drawLine(0, 17, 160, 17, YELLOW);
  M5.Lcd.drawLine(0, 38, 160, 38, DARKGREY);
  M5.Lcd.drawLine(0, 58, 160, 58, DARKGREY);
  M5.Lcd.drawLine(80, 21, 80, 94, DARKGREY);
  // delay(2940);
  M5.Lcd.fillRect(120, 20, 36, 16, RED);
  M5.Lcd.fillRect(120, 40, 36, 16, RED);
  M5.Lcd.fillRect(120, 60, 36, 16, RED);
  

  // delay(2940);
  M5.Lcd.fillRect(120, 20, 36, 16, GREENYELLOW);   
  M5.Lcd.fillRect(120, 40, 36, 16, GREENYELLOW);  
  M5.Lcd.fillRect(120, 60, 36, 16, GREENYELLOW);  

  


  
   
  //  delay(2940);
  M5.Lcd.fillRect(120, 20, 36, 16, BLACK);   
  M5.Lcd.fillRect(120, 40, 36, 16, BLACK);   
  M5.Lcd.fillRect(120, 60, 36, 16, BLACK);
  // delay(2940);
  digitalWrite(LED , LOW);       //led active low
  delay(940);
  digitalWrite(LED , HIGH);
}

void loop()
{
  // ************* Batterie **************************
  vbat = M5.Axp.GetVbatData() * 1.1 / 1000;
  discharge = M5.Axp.GetIdischargeData() / 2;
  if (vbat >= 4)
  {

    M5.Lcd.setCursor(0, 0, 2); M5.Lcd.setTextColor(LIGHTGREY, BLACK);
    M5.Lcd.println(" Detection PIR -> USB");
    M5.Lcd.pushImage(145, 1, 14, 8, bat_3);
  }
  else if (vbat >= 3.7)
  {
    M5.Lcd.setCursor(0, 0, 2);
    M5.Lcd.setTextColor(ORANGE, BLACK);
    M5.Lcd.println(" Detection PIR -> USB");
    M5.Lcd.pushImage(145, 1, 14, 8, bat_2);
  }
  else if (vbat < 3.7)
  {
    M5.Lcd.setCursor(0, 0, 2);
    M5.Lcd.setTextColor(YELLOW, BLACK);
    M5.Lcd.println(" Detection PIR -> USB");
    M5.Lcd.pushImage(145, 1, 14, 8, bat_1);
  }


  // *******************************************
  while (digitalRead(PIR) == HIGH)
  {
    digitalWrite(RL, 1);
    digitalWrite(LED , !(digitalRead(PIR)));       //led active low
    Flag = 1;
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.setCursor(5, 20, 2);
    M5.Lcd.print("PIR");
    M5.Lcd.setCursor((94 + 1), 20, 2);
    M5.Lcd.print(digitalRead(PIR));
    if (digitalRead (PIR) == LOW)
    {
      M5.Lcd.fillRect(120, 20, 36, 16, GREENYELLOW);   
    }
    else
    {
      M5.Lcd.fillRect(120, 20, 36, 16, RED);
      message.html.content = htmlMsg.c_str();
      message.html.content = htmlMsg.c_str();
      message.text.charSet = "us-ascii";
      message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
  
        if (!smtp.connect(&session))
          return;
        if (!MailClient.sendMail(&smtp, &message))
        M5.Lcd.println("Error sending Email, " + smtp.errorReason());

    }
    M5.Lcd.setCursor(5, 40, 2);
    M5.Lcd.print("Flag");
    M5.Lcd.setCursor((94 + 1), 40, 2);
    M5.Lcd.print(Flag);
    if (Flag == 0)
    {
      M5.Lcd.fillRect(120, 40, 36, 16, GREENYELLOW);   
    }
    else
    {
      M5.Lcd.fillRect(120, 40, 36, 16, RED);
    }
    M5.Lcd.setCursor(5, 60, 2);
    M5.Lcd.print("RL/Mosfet");
    M5.Lcd.setCursor((94 + 1), 60, 2);
    M5.Lcd.print(digitalRead(RL));
    if (digitalRead(RL) == LOW)
    {
      M5.Lcd.fillRect(120, 60, 36, 16, GREENYELLOW);   
    }
    else
    {
      M5.Lcd.fillRect(120, 60, 36, 16, RED);
    }
    digitalWrite(LED, !(digitalRead(PIR)));
  }


  // **********************************************
  if (digitalRead(PIR) == LOW && Flag == 0)
  {
    digitalWrite(RL, 0);
    digitalWrite(LED , !(digitalRead(PIR)));
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.setCursor(5, 20, 2);
    M5.Lcd.print("PIR");
    M5.Lcd.setCursor((94 + 1), 20, 2);
    M5.Lcd.print(digitalRead(PIR));
    if (digitalRead(PIR) == LOW)
    {
      M5.Lcd.fillRect(120, 20, 36, 16, GREENYELLOW);   
    }
    else
    {
      M5.Lcd.fillRect(120, 20, 36, 16, RED);
      message.html.content = htmlMsg.c_str();
      message.html.content = htmlMsg.c_str();
      message.text.charSet = "us-ascii";
      message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
  
    if (!smtp.connect(&session))
        return;
    if (!MailClient.sendMail(&smtp, &message))
    M5.Lcd.println("Error sending Email, " + smtp.errorReason());

    }
    M5.Lcd.setCursor(5, 40, 2);
    M5.Lcd.print("Flag");
    M5.Lcd.setCursor((94 + 1), 40, 2);
    M5.Lcd.print(Flag);
    if (Flag == 0)
    {
      M5.Lcd.fillRect(120, 40, 36, 16, GREENYELLOW);   
    }
    else
    {
      M5.Lcd.fillRect(120, 40, 36, 16, RED);  
    }
    M5.Lcd.setCursor(5, 60, 2);
    M5.Lcd.print("RL/Mosfet");
    M5.Lcd.setCursor((94 + 1), 60, 2);
    M5.Lcd.print(digitalRead(RL));
    if (digitalRead(RL) == LOW)
    {
      M5.Lcd.fillRect(120, 60, 36, 16, GREENYELLOW);   
    }
    else
    {
      M5.Lcd.fillRect(120, 60, 36, 16, RED);
    }
  }


  // **********************************************
  if (digitalRead(PIR) == LOW && Flag == 1)
  {
    digitalWrite(RL, 1);
    digitalWrite(LED, !(digitalRead(PIR)));
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.setCursor(5, 20, 2);
    M5.Lcd.print("PIR");
    M5.Lcd.setCursor((94 + 1), 20, 2);
    M5.Lcd.print(digitalRead(PIR));
    if (digitalRead(PIR) == LOW)
    {
      M5.Lcd.fillRect(120, 20, 36, 16, GREENYELLOW);   
    }
    else
    {
      M5.Lcd.fillRect(120, 20, 36, 16, RED);
      message.html.content = htmlMsg.c_str();
      message.html.content = htmlMsg.c_str();
      message.text.charSet = "us-ascii";
      message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
  
    if (!smtp.connect(&session))
        return;
    if (!MailClient.sendMail(&smtp, &message))
    M5.Lcd.println("Error sending Email, " + smtp.errorReason());
    }
    M5.Lcd.setCursor(5, 40, 2);
    M5.Lcd.print("Flag");
    M5.Lcd.setCursor((94 + 1), 40, 2);
    M5.Lcd.print(Flag);
    if (Flag == 0)
    {
      M5.Lcd.fillRect(120, 40, 36, 16, GREENYELLOW);   
    }
    else
    {
      M5.Lcd.fillRect(120, 40, 36, 16, RED);
    }
    M5.Lcd.setCursor(5, 60, 2);
    M5.Lcd.print("RL/Mosfet");
    M5.Lcd.setCursor((94 + 1), 60, 2);
    M5.Lcd.print(digitalRead(RL));
    if (digitalRead(RL) == LOW)
    {
      M5.Lcd.fillRect(130, 60, 36, 16, GREENYELLOW);   
    }
    else
    {
      M5.Lcd.fillRect(120, 60, 36, 16, RED);
    }
    for (byte i = 0 ; i <= 57 ; i++)
    {
      digitalWrite(LED, !(digitalRead(PIR)));
      digitalRead(PIR);
      M5.Lcd.setCursor(5, 20, 2);
      M5.Lcd.print("PIR");
      M5.Lcd.setCursor((94 + 1), 20, 2);
      M5.Lcd.print(digitalRead(PIR));
      if (digitalRead(PIR) == LOW)
      {
        M5.Lcd.fillRect(120, 20, 36, 16, GREENYELLOW);   
      }
      else
      {
        M5.Lcd.fillRect(120, 20, 36, 16, RED);
      }
      delay (760);
      digitalWrite(LED, !(digitalRead(PIR)));
    }
  }
  Flag = 0;
  digitalWrite(LED, !(digitalRead(PIR)));
}
