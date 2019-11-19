/********************************
 * 実験基板用Peripheralライブラリ
 * 
 * 管理者以外は変更しないこと！
*********************************/
#include "peripheral.h"
#define LED_WIFI 14
#define BUZZER A10
#define PWM_CH 0
// PubSubClientライブラリでのパケットサイズは128バイトなのを拡張
#define MQTT_MAX_PACKET_SIZE 1024

WiFiClient espClient;
PubSubClient client(espClient);

bool _mqtt_broker_connection = false;
unsigned int _now_buzzer_hz = DEFAULT_BUZZER_HZ;
p_callback_sub _p_message = NULL;
p_callback_pub _p_publish = NULL;
p_func _p_connect = NULL;

/* LED点灯関数 */
void ledOn(int led){
    switch(led){
        case LED1:
        case LED2:
        case LED3:
        case LED4:
        case LED5:
            digitalWrite(led, HIGH);
            break;
        default:
            break;
    }
}

/* LED消灯関数 */
void ledOff(int led){
    switch(led){
        case LED1:
        case LED2:
        case LED3:
        case LED4:
        case LED5:
            digitalWrite(led, LOW);
            break;
        default:
            break;
    }
}

/* スイッチ状態を取得する関数 */
int checkSwitch(int sw){
    if (sw != SWITCH1 && sw != SWITCH2){
        return FREE;
    }
    return digitalRead(sw);
}

/* ブザーオン関数 */
void buzzerOn(unsigned int hz){
    if (hz != _now_buzzer_hz){
        /* 同じ周波数を再設定すると動作が止まるのでチェック */
        _now_buzzer_hz = hz;
        ledcSetup(PWM_CH, hz, 8);
        ledcAttachPin(BUZZER, PWM_CH);
    }
    /* Duty比50% */
    ledcWrite(PWM_CH, 128);
}

/* ブザーオフ関数 */
void buzzerOff(){
    /* Duty比0% */
    ledcWrite(PWM_CH, 0);
}

/* WiFi接続関数 */
bool checkWiFiConnection(const char* ssid, const char* pass){
    if (WiFi.status() != WL_CONNECTED) {
        WiFi.begin(ssid, pass);

        if (WiFi.waitForConnectResult() != WL_CONNECTED) {
            /* WiFi接続失敗 */
            Serial.println("Failed to connect WiFi...\n Retry after several seconds");
            digitalWrite(LED_WIFI, LOW);
            delay(1000);
            return false;
        }
        else {
            /* WiFi接続成功 */
            Serial.print("WiFi connected: ");
            Serial.println(WiFi.localIP());
            digitalWrite(LED_WIFI, HIGH);
        }
    }
    else if (digitalRead(LED_WIFI) == LOW) {
        /* WiFi接続成功 */
        Serial.print("WiFi connected: ");
        Serial.println(WiFi.localIP());
        digitalWrite(LED_WIFI, HIGH);
    }
    return true;
}

/* MQTT接続関数 */
bool checkMqttConnection(){
    // MQTT connection
    if (!client.connected()) {
        // IDが重複しないように乱数を追加
        String clientID = "ESP32_DevkitC_32D-" + String(esp_random());
        Serial.println(clientID);
        client.connect(clientID.c_str());
        if (client.connected()) {
            // Broker接続成功
            _mqtt_broker_connection = true;
            if (_p_connect != NULL){
                _p_connect();   
            }
            client.setCallback(_localCallbackFunction);
        }
        else {
            Serial.println("MQTT connection failed: ");
            switch(client.state()){
                case -4:
                    Serial.println("[MQTT Error] Broker connection timeout.");
                    break;
                case -3:
                    Serial.println("[MQTT Error] Network connection broken.");
                    break;
                case -2:
                    Serial.println("[MQTT Error] Network connection failed.");
                    break;
                case -1:
                    Serial.println("[MQTT Warning] Disconnect OK.");
                    break;
                case 0:
                    break;
                default:
                    Serial.print("[MQTT Error] status code = ");
                    Serial.println(client.state());
                    break;
            }
            //Serial.println(client.state());
            delay(1000);
            _mqtt_broker_connection = false;
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

/* MQTTメッセージ受信時のコールバック関数（1次） */
void _localCallbackFunction(char* topic, byte* payload, unsigned int len){
    /* バイト列を文字列に変換する */
    char buf[MQTT_MAX_PACKET_SIZE];
    payload[len] = '\0';
    snprintf(buf, sizeof(buf), "%s", payload);
    if (_p_message != NULL){
        _p_message(topic, buf, len);
    }
}

/* Broker接続成功時のコールバック設定関数 */
void mqttSetConnectCallback(p_func on_connect){
    _p_connect = on_connect;
}

/* message受信時のコールバック設定関数 */
void mqttSetSubscribeCallback(p_callback_sub on_message){
    _p_message = on_message;
}

/* message送信時のコールバック設定関数 */
void mqttSetPublishCallback(p_callback_pub on_publish){
    _p_publish = on_publish;
}

void mqttSubscribe(const char* topic){
    if(_mqtt_broker_connection == false){
        Serial.println("[Error] No broker connection!");
        return;
    }
    client.subscribe(topic);
}

void mqttPublish(const char* topic, const char* payload){
    if(_mqtt_broker_connection == false){
        Serial.println("[Error] No broker connection!");
        return;
    }
    client.publish(topic, payload);
    if (_p_publish != NULL){
        _p_publish(topic, payload, sizeof(payload));
    }
}

void peripheralSetup(){
    // IO設定
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
    pinMode(LED4, OUTPUT);
    pinMode(LED5, OUTPUT);
    pinMode(LED_WIFI, OUTPUT);
    pinMode(SWITCH1, INPUT);
    pinMode(SWITCH2, INPUT);

    // PWM設定
    ledcSetup(PWM_CH, DEFAULT_BUZZER_HZ, 8);
    ledcAttachPin(BUZZER, PWM_CH);
    buzzerOff();

    // Serial
    Serial.begin(115200);

    // WiFisetup
    WiFi.mode(WIFI_STA);
}
