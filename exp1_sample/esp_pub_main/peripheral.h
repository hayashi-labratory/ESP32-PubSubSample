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

typedef void (*p_func)(void);
typedef void (*p_callback_sub)(char*, char*, unsigned int);
typedef void (*p_callback_pub)(const char*, const char*, unsigned int);

#endif
