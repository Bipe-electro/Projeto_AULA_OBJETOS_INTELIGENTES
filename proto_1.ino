/*
 Web Server

 A simple web server that shows the value of the analog input pins.
 using an Arduino WIZnet Ethernet shield.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)

 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 modified 02 Sept 2015
 by Arturo Guadalupi
 
 */

#include <SPI.h>
#include <Ethernet.h>

// Configuração do Ethernet Shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Substitua pelo MAC do seu Ethernet Shield
IPAddress ip(192, 168, 1, 171); // Substitua pelo IP desejado
EthernetServer server(80); // Servidor na porta 80

// Configurações do sensor e LED
const int mq2Pin = A0; // Pino analógico conectado ao sensor MQ-2
      int ledPin_vermelho    = 6;  // Pino digital conectado ao LED
      int ledPin_amarelo     = 7; 
      int ledPin_verde       = 8; 
      int buzzer = 9;// 
      int RelePin = 4;      
const int gasThreshold = 400; // Limite de detecção de gás (ajuste conforme necessário)

void setup() {
  // Inicializa o Ethernet
  Ethernet.begin(mac, ip);
  server.begin();
  
  // Inicializa pinos
  pinMode(ledPin_vermelho,OUTPUT);
  pinMode(ledPin_amarelo, OUTPUT);
  pinMode(ledPin_verde,OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(RelePin,OUTPUT);
  digitalWrite(ledPin_vermelho, LOW);
  digitalWrite(ledPin_amarelo,LOW);
  digitalWrite(ledPin_verde, LOW);
  digitalWrite(RelePin, LOW); 
  // LED desligado por padrão

  // Comunicação serial para depuração
  Serial.begin(9600);
  Serial.println("Servidor iniciado...");
  Serial.print("Acesse: http://");
  Serial.println(ip);
}

void loop() {
  // Leitura do sensor MQ-2
  int gasValue = analogRead(mq2Pin);
  Serial.print("Valor do gas: ");
  Serial.println(gasValue);

  // Verifica se o nível de gás ultrapassa o limite
  if (gasValue > 590) {
    digitalWrite(ledPin_vermelho, HIGH);
    digitalWrite(ledPin_amarelo, LOW);
    digitalWrite(RelePin, HIGH);
    delay(4000);

     tone(buzzer, 1500);
     delay(200);
     noTone(buzzer);
     delay(200);
     
     // Liga o LED
  } else {
    digitalWrite(ledPin_vermelho, LOW); // Desliga o LED
    digitalWrite(RelePin, LOW);
    delay(2000);
  
  }
  if (gasValue > 575 && gasValue < 589) {
     digitalWrite(ledPin_amarelo,HIGH);
     digitalWrite(ledPin_verde, LOW);
     

     tone(buzzer, 1000);
     delay(100);
     noTone(buzzer);
     delay(100);
     
  }else{
     digitalWrite(ledPin_amarelo,LOW);
     digitalWrite(RelePin, LOW);
     delay(2000);
    
  if(gasValue < 574){
    digitalWrite(ledPin_verde, HIGH);
  }else{
    digitalWrite(ledPin_verde, LOW);
  
  }   



  // Responde ao cliente HTTP
  EthernetClient client = server.available();
  if (client) {
    Serial.println("Cliente conectado");
    String request = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;

        // Final do cabeçalho HTTP
        if (c == '\n') {
          // Resposta ao cliente
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("");
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<head><title>Monitoramento de Gas</title></head>");
          client.println("<body style='background-color:#ADD8E6; text-align:center;'>");
          client.println("<h1>Monitoramento de Gas</h1>");
          client.print("<p>Nivel do gas: ");
          client.print(gasValue);
          client.println("</p>");

          if (gasValue > 590) {
            client.println("<p style='color:red;'><b>!!GAS DETECTADO!!</b></p>");
          } 
          if (gasValue > 575 && gasValue < 589) {
            client.println("<p style='color:yellow;'><b>ALERTA DE GAS!</b></p>"); 
          }
          if (gasValue < 574){
             client.println("<p style='color:green;'><b>NIVEL NORMAL</b></p>");
           }  
          client.println("</body></html>");
          break;
        }
      }
    }
    delay(1); // Pequeno atraso para garantir que o cliente receba a resposta
    client.stop();
    Serial.println("Cliente desconectado");
  }
}}