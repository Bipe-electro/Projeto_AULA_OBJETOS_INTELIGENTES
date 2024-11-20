#include <SPI.h>
#include <Ethernet.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "User_LeonardoSV"
#define AIO_KEY         "aio_UPPB12V7G5clIGZKzfg2pb79huro"

// Ethernet Configuration
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
//IPAddress ip(192, 168, 1, 171);
EthernetClient client;

// Adafruit MQTT Client
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// Feeds
Adafruit_MQTT_Publish mq2Feed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/mq2");
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/onoff");

// Sensor MQ2
#define MQ2_PIN A0 // Conecte o sensor MQ2 à entrada analógica A0

// LEDs
#define LED_VERDE 8
#define LED_AMARELO 7
#define LED_VERMELHO 6

// Buzzer
#define BUZZER_PIN 9

// RELE
#define RELE_PIN 3

// Limites do sensor (ajuste conforme necessário)
#define LIMITE_NORMAL 545
#define LIMITE_ALERTA 650


void setup() {
    Serial.begin(115200);
    Serial.println(F("Adafruit MQTT + MQ2 Sensor + LEDs + Buzzer"));

    // Inicializa Ethernet
    Serial.print(F("\nInicializando Ethernet..."));
    Ethernet.begin(mac);
    delay(1000);

    // Inscreve-se no feed de controle
    mqtt.subscribe(&onoffbutton);

    // Configura LEDs e buzzer como saída
    pinMode(LED_VERDE, OUTPUT);
    pinMode(LED_AMARELO, OUTPUT);
    pinMode(LED_VERMELHO, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(RELE_PIN, OUTPUT);

    digitalWrite(RELE_PIN, HIGH);
}

void loop() {
    // Mantém a conexão MQTT ativa
    MQTT_connect();

    // Lê os valores do feed 'onoffbutton'
    Adafruit_MQTT_Subscribe *subscription;
    while ((subscription = mqtt.readSubscription(1000))) {
        if (subscription == &onoffbutton) {
            Serial.print(F("Recebido: "));
            Serial.println((char *)onoffbutton.lastread);
        }
    }

    // Lê o sensor MQ2
    int mq2Value = analogRead(MQ2_PIN);
    Serial.print(F("\nValor do sensor MQ2: "));
    Serial.println(mq2Value);

    // Publica o valor no feed
    Serial.print(F("Enviando valor para o feed..."));
    if (!mq2Feed.publish(String(mq2Value).c_str())) {
        Serial.println(F("Falhou!"));
    } else {
        Serial.println(F("Enviado com sucesso!"));
    }

    // Verifica o estado do sensor e controla LEDs e buzzer
    if (mq2Value <= LIMITE_NORMAL) {
        // Estado normal
        digitalWrite(LED_VERDE, HIGH);
        digitalWrite(LED_AMARELO, LOW);
        digitalWrite(LED_VERMELHO, LOW);
        digitalWrite(RELE_PIN, HIGH);
        //analogWrite(RELE_PIN, LOW);;
        noTone(BUZZER_PIN); // Desliga o buzzer
    } else if (mq2Value <= LIMITE_ALERTA) {
        // Estado de alerta
        digitalWrite(LED_VERDE, LOW);
        digitalWrite(LED_AMARELO, HIGH);
        digitalWrite(LED_VERMELHO, LOW);
        digitalWrite(RELE_PIN, HIGH);
        //analogWrite(RELE_PIN, LOW);
        tone(BUZZER_PIN, 1000, 200); // Tom de alerta (1000 Hz por 500 ms)
    } else {
        // Estado crítico
        digitalWrite(LED_VERDE, LOW);
        digitalWrite(LED_AMARELO, LOW);
        digitalWrite(LED_VERMELHO, HIGH);
        digitalWrite(RELE_PIN, LOW);
        tone(BUZZER_PIN, 2000, 1000); // Tom crítico (2000 Hz por 1 segundo)
        
        
    }
        
        
    // Ping no servidor MQTT
    if (!mqtt.ping()) {
        mqtt.disconnect();
    }

    delay(2000); // Aguarda 2 segundos antes de ler novamente
}

// Mantém a conexão com o servidor MQTT
void MQTT_connect() {
    int8_t ret;

    if (mqtt.connected()) {
        return;
    }

    Serial.print(F("Conectando ao MQTT... "));
    while ((ret = mqtt.connect()) != 0) {
        Serial.println(mqtt.connectErrorString(ret));
        Serial.println(F("Tentando reconectar em 5 segundos..."));
        mqtt.disconnect();
        delay(5000);
    }
    Serial.println(F("Conectado ao MQTT!"));
}
