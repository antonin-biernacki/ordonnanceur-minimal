#ifndef ORDONNANCEUR_H
#define ORDONNANCEUR_H

// Inclusions standard
#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// Définitions générales
#define F_CPU       16000000
#define BAUD        9600
#define MYUBRR      F_CPU/16/BAUD-1
#define SLEEP_DEC   1
#define DIV         8
#define PER         1
#define DELAY       1000
#define NBTASKS     15
#define STACK_LENGTH 100
#define FIRST_STACK_POSITION 0x800
#define ENDED 1
#define RUN 0

// Broches LEDs
#define PLED1 0
#define PLED2 3
#define PLED3 0
#define PLED4 4
#define PLED5 7

// Broches SPI
#define SPI_DDR     DDRB
#define SPI_PORT    PORTB
#define SPI_MOSI    3
#define SPI_MISO    4
#define SPI_SCK     5
#define CS1         2
#define CS2         0
#define CS3         3
#define CS4         0
#define CS5         4
#define CS6         7

// Timer
#define CTC1        WGM12

// Macros pour sauvegarde/restauration des registres
#define SAVE_REGISTERS() \
    asm volatile ( \
        "push r0 \n\t" \
        "in r0, __SREG__ \n\t" \
        "push r0 \n\t" \
        "push r1 \n\t" \
        "push r2 \n\t" \
        "push r3 \n\t" \
        "push r4 \n\t" \
        "push r5 \n\t" \
        "push r6 \n\t" \
        "push r7 \n\t" \
        "push r8 \n\t" \
        "push r9 \n\t" \
        "push r10 \n\t" \
        "push r11 \n\t" \
        "push r12 \n\t" \
        "push r13 \n\t" \
        "push r14 \n\t" \
        "push r15 \n\t" \
        "push r16 \n\t" \
        "push r17 \n\t" \
        "push r18 \n\t" \
        "push r19 \n\t" \
        "push r20 \n\t" \
        "push r21 \n\t" \
        "push r22 \n\t" \
        "push r23 \n\t" \
        "push r24 \n\t" \
        "push r25 \n\t" \
        "push r26 \n\t" \
        "push r27 \n\t" \
        "push r28 \n\t" \
        "push r29 \n\t" \
        "push r30 \n\t" \
        "push r31 \n\t" \
    )

#define RESTORE_REGISTERS() \
    asm volatile ( \
        "pop r31 \n\t" \
        "pop r30 \n\t" \
        "pop r29 \n\t" \
        "pop r28 \n\t" \
        "pop r27 \n\t" \
        "pop r26 \n\t" \
        "pop r25 \n\t" \
        "pop r24 \n\t" \
        "pop r23 \n\t" \
        "pop r22 \n\t" \
        "pop r21 \n\t" \
        "pop r20 \n\t" \
        "pop r19 \n\t" \
        "pop r18 \n\t" \
        "pop r17 \n\t" \
        "pop r16 \n\t" \
        "pop r15 \n\t" \
        "pop r14 \n\t" \
        "pop r13 \n\t" \
        "pop r12 \n\t" \
        "pop r11 \n\t" \
        "pop r10 \n\t" \
        "pop r9 \n\t" \
        "pop r8 \n\t" \
        "pop r7 \n\t" \
        "pop r6 \n\t" \
        "pop r5 \n\t" \
        "pop r4 \n\t" \
        "pop r3 \n\t" \
        "pop r2 \n\t" \
        "pop r1 \n\t" \
        "pop r0 \n\t" \
        "out __SREG__, r0 \n\t" \
        "pop r0 \n\t" \
    )

// Structure pour les tâches
typedef struct {
    uint16_t stackPointer;
    void (*functionAddress)(void);
    int sleep_time;
    int state;
} process;

// Variables globales
extern int Current_task;
extern process Tasks_list[NBTASKS];

// Prototypes de fonctions
void init_minuteur(int diviseur, long periode);
void led_init();
void led1_blink();
void led2_blink();
void led3_blink();
void led4_blink();
void led5_blink();
void main_leds();
void call_process();
void seven_seg();
void USART_Init(unsigned int ubrr);
void USART_Transmit(unsigned char data);
unsigned char USART_Receive(void);
void spi_init(void);
void spi_activer(uint16_t port, uint8_t cs);
void spi_desactiver(uint16_t port, uint8_t cs);
uint8_t spi_echange(uint8_t envoi);
void scheduler();
void finish();
void kill(void (*function)(void));
void add(void (*newFunction)(void));
void wait(int time);
void init_stackPointer_tasks(int Cprocess);
void init_piles(int Cprocess);
void init_state();

#endif 
