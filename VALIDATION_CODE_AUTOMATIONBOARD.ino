#include <DHT.h>
#include <SoftwareSerial.h>

#define DHTPIN A2 // pino que estamos conectado
#define DHTTYPE DHT11 // DHT 11
#define DEBUG true        // Se "true" o codigo printy6a a resposta do modulo a cada operacao
#define rele1_pino 5       // Pino conectado ao rele 1
#define rele2_pino 6
#define rele3_pino 7
#define rele4_pino 8

DHT dht(DHTPIN, DHTTYPE);
String valorUmidade;
String valorTemperatura;
 
//########## MUDE AQUI PARA OS PARAMETROS DA SUA REDE WIFI #################
String meu_IP = "192.168.25.60"; // Escolha um IP compativel com sua rede local
String minha_SSID = "GVT-3B1D"; // SSID da sua rede wifi
String minha_senha = "8007419597"; // senha da sua rede wifi
//##########################################################################
 
 
SoftwareSerial Debug(10, 11); // Cria serial em 10 e 11 para debug. Permite leitura das respostas do modulo
 
bool rele1 = false;        // Estado do rele. true/ligado ou false/desligado
bool rele2 = false;
bool rele3 = false;
bool rele4 = false;
 
void setup()
{
  pinMode(rele1_pino, OUTPUT);      // Define pino como saida
  pinMode(rele2_pino, OUTPUT);
  pinMode(rele3_pino, OUTPUT);
  pinMode(rele4_pino, OUTPUT);
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
  sendData("AT+CIPSERVER=1,80\r\n", 1000, DEBUG); // Habilita servidor na porta 80. O modulo passa a responder nessa porta
}
 
 
void loop()
{
  //Limpa as variaveis para receber os dados (temperatura e Umidade)
  valorUmidade ="";
  valorTemperatura ="";
  
  if (Serial.available()) // Se o ESP8266 estiver mandando caracteres para a AutomationBoard
  {
 
    if (Serial.find("+IPD,")) // Se encontrou "+IPD," o proximo caracter sera o ID da conexao
    {
      delay(400);
      int connectionId = Serial.read() - 48; // subtrai 48 porque a funcao read() retorna um caracter ASCII, subtraindo 48 teremos um int correspondente (primeiro decimal comeca em 48)
 
      int muda_rele = 0; // Variavel que define qual rele sera alterado, de acordo com o link que o usuario clicou
      if (Serial.find("rele")) {
        muda_rele = Serial.read() - 48;
      }
      if (muda_rele == 1) {
        digitalWrite(rele1_pino, !rele1);  // Se usuario clicou no 1o link, muda estado do rele1
        rele1 = !rele1;
      }
      if (muda_rele == 2) {
        digitalWrite(rele2_pino, !rele2);  // Se usuario clicou no 2o link, muda estado do rele2
        rele2 = !rele2;
      }
      if (muda_rele == 3) {
        digitalWrite(rele3_pino, !rele3);  // Se usuario clicou no 3o link, muda estado do rele3
        rele3 = !rele3;
      }
      if (muda_rele == 4) {
        digitalWrite(rele4_pino, !rele4);  // Se usuario clicou no 4o link, muda estado do rele4
        rele4 = !rele4;
      }

 
     // A leitura da temperatura e umidade pode levar 250ms!
  // O atraso do sensor pode chegar a 2 segundos.
  float h = dht.readHumidity();
  float t = dht.readTemperature();
   valorUmidade+=String(float(h))+"."+String(getDecimal(h));
  Serial.println("Arduino Umidade : "+valorUmidade);
  valorTemperatura+=String(float(t))+"."+String(getDecimal(t));
  Serial.println("Arduino Temperatura : "+valorTemperatura);  
  valorUmidade = String(h);
  valorTemperatura = String(t);
  // testa se retorno é valido, caso contrário algo está errado.
  if (isnan(t) || isnan(h)) 
  {
    Serial.println("Failed to read from DHT");
  } 
  else
  {
    Serial.print("Umidade: ");
    Serial.print(h);
    Serial.print(" %t");
    Serial.print("Temperatura: ");
    Serial.print(t);
    Serial.println(" *C");
  }
    
      // Monta a pagina que sera exibida para o usuario
       String webpage = "<head><meta http-equiv=""refresh"" content=""3""></head>";
       webpage += "<meta http-equiv=\"refresh\" content=\"5\" />";
       webpage += "<html><head></head><body><center>";
       webpage += "<img src=\"https://ifce.edu.br/imagens/imagens_menu_rapido/logo_ifce.jpg\" width=\"250\">";
       webpage += "<br><br><h2>Web Server com ESP8266 e AutomationBoard</h2><br>";
       webpage += " </br></h1> Umidade: ";
       webpage += valorUmidade + "</h1>";
       webpage += " </br></h1> Temperatura: ";
       webpage += valorTemperatura + "</h1>";           

      String cipSend = "AT+CIPSEND=";  // Variavel com o comando para enviar dados para o dispositivo que se comunica com o modulo
      cipSend += connectionId;         // ID da conexao
      cipSend += ",";                  // separacao
      cipSend += webpage.length();     // tamanho do pacote de dados a ser enviado
      cipSend += "\r\n";               // fim do comando
 
      sendData(cipSend, 1000, DEBUG);  // envia comando para o modulo
      sendData(webpage, 3000, DEBUG);  // envia a pagina para responder ao acesso de algum dispositivo (ex: usuario)
 
      String closeCommand = "AT+CIPCLOSE=";   // Variavel com o comando para fechar a conexao
      closeCommand += connectionId;           // ID da conexao
      closeCommand += "\r\n";                 // fim do comando
 
      sendData(closeCommand, 1000, DEBUG);    // Envia comando para fechar a conexao
    }
  }
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

long getDecimal(float val)
{
  //converte o float para inteiro
  int intPart = int(val);
 
  //multiplica por 100
  //precisão de 2 casas decimais
  long decPart = 100*(val-intPart);
 
  //Se o valor for maior que 0 retorna
  if(decPart>0)
  //retorna a variavel decPart
  return(decPart);          
 
  //caso contrario retorna o valor 0
  else if(decPart=0)
     return(00);          
}

