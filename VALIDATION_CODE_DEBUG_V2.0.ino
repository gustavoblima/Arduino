//ESTE CODIGO POSSUI SENSORES DE TEMPERATURA E UMIDADE!
#include <DHT.h>
#include <SoftwareSerial.h>
#define DEBUG true        // Se "true" o codigo printy6a a resposta do modulo a cada operacao
#define DST_IP "192.168.25.23"
#define DHTPIN A2 // pino que estamos conectado
#define DHTTYPE DHT11 // DHT 11

DHT dht(DHTPIN, DHTTYPE);

//########## MUDE AQUI PARA OS PARAMETROS DA SUA REDE WIFI #################
String meu_IP = "192.168.25.60"; // Escolha um IP compativel com sua rede local
String minha_SSID = "GVT-3B1D"; // SSID da sua rede wifi
String minha_senha = "8007419597"; // senha da sua rede wifi
//##########################################################################
  
SoftwareSerial Debug(10, 11); // Cria serial em 10 e 11 para debug. Permite leitura das respostas do modulo
 
void setup()
{
  Debug.begin(115200);
  Serial.begin(115200); // your esp's baud rate might be different

  Serial.println("DHTxx test!");
  dht.begin();
  
  sendData("AT+RST\r\n", 3000, DEBUG); // reseta o modulo
  sendData("AT+CWMODE=1\r\n", 1000, DEBUG); // configura o modulo como station
  sendData("AT+CWJAP=\"" + minha_SSID + "\",\"" + minha_senha + "\"\r\n", 10000, DEBUG); // loga na rede wifi
  sendData("AT+CIPSTA=\"" + meu_IP + "\"\r\n", 1000, DEBUG); // Define um endereco de IP para o modulo
  sendData("AT+CIFSR\r\n", 3000, DEBUG); // Informa endereco de IP
  sendData("AT+CIPMUX=1\r\n", 1000, DEBUG); // Configura para aceitar mais de uma conexão por vez.
  //sendData("AT+CIPSERVER=1,8095\r\n", 1000, DEBUG); // Habilita servidor na porta 80. O modulo passa a responder nessa porta
  
}
 
 
void loop()
{ 
  Serial.available(); // Se o ESP8266 estiver mandando caracteres para a AutomationBoard
  Serial.find("+IPD,"); // Se encontrou "+IPD," o proximo caracter sera o ID da conexao
  delay(400);
  // O atraso do sensor pode chegar a 2 segundos.
  int h = dht.readHumidity();
  int t = dht.readTemperature();
 
  //int connectionId = Serial.read() - 48; // subtrai 48 porque a funcao read() retorna um caracter ASCII, subtraindo 48 teremos um int correspondente (primeiro decimal comeca em 48)      
      String webpage = "GET /receiver.php?apples=" + String(t) + "&oranges=" + String(h);
             webpage +=" HTTP/1.1";
             webpage += "\r\n";
             webpage += "Host: 192.168.20.23";
             webpage += "\r\n\r\n";

      String cmd = "AT+CIPSTART=4"",\"TCP\",\"";
             cmd += DST_IP;
             cmd += "\",80";
             cmd += "\r\n"; 
      //String cmd = "AT+CIPSTART=\"TCP\",\"";// Setup TCP connection
      //       cmd += DST_IP;
      //       cmd += "\",80";
      sendData(cmd, 1000, DEBUG);
             delay(2000);

      String cipSend = "AT+CIPSEND=";  // Variavel com o comando para enviar dados para o dispositivo que se comunica com o modulo
      //cipSend += connectionId;
      cipSend += "4";
      cipSend += ",";                  // separacao
      cipSend += webpage.length();     // tamanho do pacote de dados a ser enviado
      cipSend += "\r\n";               // fim do comando

      sendData(cipSend, 1000, DEBUG);  // envia comando para o modulo
      sendData(webpage, 3000, DEBUG);  // envia a pagina para responder ao acesso de algum dispositivo (ex: usuario)
 
      String closeCommand = "AT+CIPCLOSE=";   // Variavel com o comando para fechar a conexao
      //closeCommand += connectionId;           // ID da conexao
      closeCommand += "4";
      closeCommand += "\r\n";                 // fim do comando
      sendData(closeCommand, 1000, DEBUG);    // Envia comando para fechar a conexao
}
 
 
String sendData(String command, const int timeout, boolean debug)
{
  String response = "";  // variavel que armazena a resposta do modulo
 
  Serial.print(command); // envia o comando para o modulo
 
  long int tempo = millis();
 
  while ( (tempo + timeout) > millis())
  {
    while (Serial.available() > 0)
    {
      char c = Serial.read(); // le e armazena os caracteres da resposta do modulo
      response += c;
    }
  }
 
  if (debug)    // Se debug estiver habilitado, imprime mensagens de resposta do modulo
  {
    Debug.print(response);
  }
 
  return response;
}
