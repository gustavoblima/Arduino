//CODIGO VALIDADO!
//FUNCIONANDO!
 
#include <SoftwareSerial.h>

String meu_IP = "192.168.25.31"; // Escolha um IP compativel com sua rede local
String minha_SSID = "GVT-3B1D"; // SSID da sua rede wifi
String minha_senha = "8007419597"; // senha da sua rede wifi
//RX pino 2, TX pino 3
SoftwareSerial esp8266(2, 3);
 
#define DEBUG false

String sendData(String command, const int timeout, boolean debug)
{
  // Envio dos comandos AT para o modulo
  String response = "";
  esp8266.print(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (esp8266.available())
    {
      // The esp has data so display its output to the serial window
      char c = esp8266.read(); // read the next character.
      response += c;
    }
  }
  if (debug)
  {
    Serial.print(response);
  }
  return response;
}
 
void setup()
{
  Serial.begin(9600);
  esp8266.begin(115200);
  
  sendData("AT+RST\r\n", 3000, DEBUG); // reseta o modulo
  sendData("AT+CWMODE=1\r\n", 1000, DEBUG); // configura o modulo como station
  sendData("AT+CWJAP=\"" + minha_SSID + "\",\"" + minha_senha + "\"\r\n", 10000, DEBUG); // loga na rede wifi
  sendData("AT+CIPSTA=\"" + meu_IP + "\"\r\n", 1000, DEBUG); // Define um endereco de IP para o modulo
  sendData("AT+CIFSR\r\n", 3000, DEBUG); // Informa endereco de IP
  sendData("AT+CIPMUX=1\r\n", 1000, DEBUG); // Configura para aceitar mais de uma conexão por vez.
  sendData("AT+CIPSERVER=1,80\r\n", 1000, DEBUG); // Habilita servidor na porta 80. O modulo passa a responder nessa porta
}

 
void loop()
{
  // Verifica se o ESP8266 esta enviando dados
  if (esp8266.available())
  {
    if (esp8266.find("+IPD,"))
    {
      delay(300);
      int connectionId = esp8266.read() - 48;
 
      String webpage = "<head><meta http-equiv=""refresh"" content=""3"">";
      webpage += "</head><h1><u>ESP8266 - Web Server</u></h1><h2>Porta";
      webpage += "Digital 8: ";
      int a = digitalRead(8);
      webpage += a;
      webpage += "<h2>Porta Digital 9: ";
      int b = digitalRead(9);
      webpage += b;
      webpage += "</h2>";
 
      String cipSend = "AT+CIPSEND=";
      cipSend += connectionId;
      cipSend += ",";
      cipSend += webpage.length();
      cipSend += "\r\n";
 
      sendData(cipSend, 1000, DEBUG);
      sendData(webpage, 1000, DEBUG);
 
      String closeCommand = "AT+CIPCLOSE=";
      closeCommand += connectionId; // append connection id
      closeCommand += "\r\n";
 
      sendData(closeCommand, 3000, DEBUG);
    }
  }
}
 

