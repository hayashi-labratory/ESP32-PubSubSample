/*******************************************
 * ネットワーク実験　実験1サンプルプログラム
********************************************/
#include <WiFi.h>
#include <PubSubClient.h>
#include "peripheral.h"


/* 起動時に一回だけ実行される関数 */
void setup() {
    /* 各種機能の初期化 */
    peripheralSetup();
}

/* 起動中繰り返し実行される関数 */
void loop() {
    /* スイッチ1の状態をチェックする */
    if (checkSwitch(SWITCH1) == PUSHED){
        /* スイッチが押されていたらLED1を点ける */
        buzzerOn();
        ledOn(LED1);
    }
    else{
        /* スイッチが押されていなければLEDを消す */
        buzzerOff();
        ledOff(LED1);
    }

    /* シリアルモニタ（PC）へ文字を送信する */
    Serial.print("Hello World!\n");
}
