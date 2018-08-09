#include <PubSubClient.h>

#include <Dns.h>
#include <EthernetServer.h>
#include <EthernetUdp.h>
#include <Dhcp.h>
#include <Ethernet.h>
#include <EthernetClient.h>



byte mac [] = {0xDE, 0xED, 0xBA, 0xFE, 0xE5};
IPAddress ip(172, 25, 0, 105);
IPAddress server(172, 25, 0, 110);

EthernetClient ethClient;
PubSubClient client(ethClient);

int pinLed = 8;
int pinldr = A0;

void callback(char* topic, byte* payload, unsigned int  length){
  Serial.print("Message arrived:");
  Serial.println(topic);
  String s = "";
  for (int i=0; i<length; i++){
    s +=((char) payload[i]);
    }
  Serial.println(s);
  if (s.equals("desligar")){
    digitalWrite(pinLed, LOW);
    }
    else if(s.equals("ligar")){
    digitalWrite(pinLed, HIGH);
    }
}
void setup() {

  
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.print("Arduino Inicializado");
  
  client.setServer(server, 1883);

  client.setCallback(callback);

  if(Ethernet.begin(mac)==0){
    Serial.println("DHCP - falhou");
    Ethernet.begin(mac, ip);
  }
  
  pinMode(pinLed, OUTPUT);
  pinMode(pinldr, INPUT);

  

}

void reconnect(){
  while (!client.connected()){
    Serial.println("Tentando conexão MQTT..");
    if(client.connect("ArduinoClient5")){
      Serial.println("Conectado ao MQTT");
      client.subscribe("/ocean/aula/aluno5/listener");
    }else{
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println("tentando novamente em 5s");
      delay(5000);
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if(!client.connected()){
    reconnect();
  }
  client.loop();
 
  int lum = analogRead(pinldr);
  Serial.println(lum);

  char buffer[4];
  sprintf(buffer, "%d", lum);
  client.publish("/ocean/aula/aluno5/lum",buffer);
  delay(1000);
 
 //if(lum > 500){
  //  digitalWrite(pinLed, HIGH);
  //Serial.println("ligou");
  // digitalWrite(8, LOW);
 //Serial.println("desligou");
  //delay(500);
  //}else{
   // digitalWrite(pinldr, LOW);
  //Serial.println("desligou");
  // delay(500);
  //}
  
  
  

}
