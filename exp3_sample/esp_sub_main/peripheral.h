/********************************
 * 実験基板用Peripheralライブラリ
 * 
 * 管理者以外は変更しないこと！
*********************************/
#ifndef PERIPHERAL_H
#define PERIPHERAL_H

#define LED1 32
#define LED2 33
#define LED3 25
#define LED4 26
#define LED5 27
#define SWITCH1 17
#define SWITCH2 16
#define PUSHED LOW
#define FREE HIGH
#define DEFAULT_BUZZER_HZ 2400

/* コールバック関数用 */
typedef void (*p_func)(void);
typedef void (*p_callback_sub)(char*, char*, unsigned int);
typedef void (*p_callback_pub)(const char*, const char*, unsigned int);

/* デフォルト引数を設定 */
void buzzerOn(unsigned int hz = DEFAULT_BUZZER_HZ);

#endif
