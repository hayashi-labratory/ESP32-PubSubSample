#include "peripheral.h"
#define LED_WIFI 14
#define BUZZER A10
#define PWM_CH 0
// PubSubClientライブラリでのパケットサイズは128バイトなのを拡張
#define MQTT_MAX_PACKET_SIZE 1024

WiFiClient espClient;
PubSubClient client(espClient);

p_callback_sub _p_message = NULL;
p_callback_pub _p_publish = NULL;
p_func _p_connect = NULL;

void ledOn(int led){
    digitalWrite(led, HIGH);
}

void ledOff(int led){
    digitalWrite(led, LOW);
}

int checkSwitch(int sw){
    if (sw != SWITCH1 && sw != SWITCH2){
        return FREE;
    }
    return digitalRead(sw);
}

void buzzerOn(){
    ledcWrite(PWM_CH, 128);
}

void buzzerOff(){
    ledcWrite(PWM_CH, 0);
}

bool checkWiFiConnection(const char* ssid, const char* pass){
    if (WiFi.status() != WL_CONNECTED) {
        WiFi.begin(ssid, pass);

        if (WiFi.waitForConnectResult() != WL_CONNECTED) {
            Serial.println("failed to connect");
            digitalWrite(LED_WIFI, LOW);
            delay(1000);
            return false;
        }
        else {
            Serial.print("WiFi connected: ");
            Serial.println(WiFi.localIP());
            digitalWrite(LED_WIFI, HIGH);
        }
    }
    return true;
}

bool checkMqttConnection(){
    // MQTT connection
    if (!client.connected()) {
        String clientID = "ESP32_DevkitC_32D-" + String(esp_random());
        Serial.println(clientID);
        client.connect(clientID.c_str());
        if (client.connected()) {
            if (_p_connect != NULL){
                _p_connect();   
            }
            client.setCallback(_localCallbackFunction);
        }
        else {
            Serial.println("MQTT connection failed: ");
            Serial.println(client.state());
            delay(1000);
            return false;
        }
    }
    return true;
}

void mqttLoop(){
    client.loop();
}

void mqttSetServer(const char* broker, int port){
    client.setServer(broker, port);
}

void _localCallbackFunction(char* topic, byte* payload, unsigned int len){
    char buf[MQTT_MAX_PACKET_SIZE];
    payload[len] = '\0';
    snprintf(buf, sizeof(buf), "%s", payload);
    if (_p_message != NULL){
        _p_message(topic, buf, len);
    }
}

void mqttSetConnectCallback(p_func on_connect){
    _p_connect = on_connect;
}

void mqttSetSubscribeCallback(p_callback_sub on_message){
    _p_message = on_message;
}

void mqttSetPublishCallback(p_callback_pub on_publish){
    _p_publish = on_publish;
}

void mqttSubscribe(const char* topic){
    client.subscribe(topic);
}

void mqttPublish(const char* topic, const char* payload){
    client.publish(topic, payload);
    if (_p_publish != NULL){
        _p_publish(topic, payload, sizeof(payload));
    }
}

void peripheralSetup(){
    // IO
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
    pinMode(LED4, OUTPUT);
    pinMode(LED5, OUTPUT);
    pinMode(LED_WIFI, OUTPUT);
    pinMode(SWITCH1, INPUT);
    pinMode(SWITCH2, INPUT);

    // PWM
    ledcSetup(PWM_CH, 440, 8);
    ledcAttachPin(BUZZER, PWM_CH);
    buzzerOff();

    // Serial
    Serial.begin(115200);

    // WiFisetup
    WiFi.mode(WIFI_STA);
}
