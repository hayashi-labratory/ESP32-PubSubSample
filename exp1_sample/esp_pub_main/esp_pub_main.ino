#include <WiFi.h>
#include <PubSubClient.h>
#include <string.h>
#include "peripheral.h"

const char* ssid = "ssid";    		// WiFiのSSID
const char* password = "password";	// WiFiのパスワード
const char* broker = "localhost";	// BrokerのIPアドレス
int port = 1883;					// Brokerのポート番号
const char* topic = "data/学籍番号"; // 送信するTopic


/* メッセージ受信完了時に呼び出される関数 */
void on_publish(const char* topic, const char* payload, unsigned int len) {
    Serial.print("publish topic  : ");
    Serial.println(topic);
    Serial.print("publish payload: ");
    Serial.println(payload);
}

/* Brokerへの接続完了時に呼び出される関数 */
void on_connect(){
    Serial.println("Connect OK");

    /* メッセージを送信する */
    mqttPublish(topic, "Hello MQTT World!\n");
}


/* 起動時に一回だけ実行される関数 */
void setup() {
    /* 各種機能の初期化 */
    peripheralSetup();

    /* Brokerの設定 */
    mqttSetServer(broker, port);
    /* コールバック関数（呼び出される関数）の設定 */
    mqttSetConnectCallback(on_connect);     // Brokerへの接続完了時
    mqttSetPublishCallback(on_publish);     // メッセージ送信完了時
}

/* 起動中繰り返し実行される関数 */
void loop() {
    /* WiFiネットワークに接続＆接続監視 */
    if (!checkWiFiConnection(ssid, password)){
        /* アクセスポイントに接続出来ない場合はしばらく待ってから再度接続する */
        delay(3000);
        return;
    }

    /* Brokerに接続＆接続監視 */
    if (checkMqttConnection()){
        /* 内部でMQTTの処理を行う */
        mqttLoop();
    }
    else{
        /* Brokerに接続出来ない場合は終了 */
        delay(3000);
        return;
    }
}
