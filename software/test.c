// avr-gcc -mmcu=atmega328p -Wall -Werror -I. -DF_CPU=16000000 -Os -o test.elf test.c
// avr-objcopy -j .text -j .data -O ihex test.elf test.hex
// avrdude -p atmega328p -c arduino -P /dev/ttyACM0 -b 115200 -D -U flash:w:test.hex

// Include
 
#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
 
// Define
 
#define PLED1 0
#define PLED2 3
#define PLED3 0
#define PLED4 7
#define PLED5 4

#define SPI_DDR         DDRB
#define SPI_PORT        PORTB
#define SPI_SS          2
#define SPI_MOSI        3
#define SPI_MISO        4
#define SPI_SCK         5
#define CS 		0



/*
void led_init() {
        DDRC |= (1<<PLED1);
	DDRC |= (1<< PLED2);
	DDRB |= (1<< PLED3);
	DDRD |= (1<< PLED4);
	DDRD |= (1<< PLED5);

}


void leds_blink() {
	PORTC ^= (1<<PLED1);
	PORTC ^= (1<<PLED2);
	PORTB ^=avrdude -p atmega328p -c arduino -P /dev/ttyACM0 -b 115200 -D -U flash:w:test.hex (1<<PLED3);
	PORTD ^= (1<<PLED4);
	PORTD ^= (1<<PLED5);

	_delay_ms(200);
}
*/

/*
void select_spi(int cs, void (*f)(void)) {
	DDRB |= (1<<cs);
	PORTB |= (1<<cs);
	f();
}
*/

void spi_init(void) {                                 // Initialisation du bus SPI
        SPI_DDR |= (1<<SPI_MOSI)|(1<<SPI_SCK)|(1<<SPI_SS);   // Définition des sorties
        SPI_DDR &= ~(1<<SPI_MISO);                           // Définition de l'entrée
        SPI_PORT |= (1<<SPI_SS);                             // Désactivation du périphérique
        SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR1);                 // Activation SPI (SPE) en état maître (MSTR)
                                                             // horloge F_CPU/64 (SPR1=1,SPR0=0)
							     
	SPI_DDR |= (1<<CS);
}


void spi_activer(uint8_t address, uint8_t cs) {                              // Activer le périphérique
        volatile uint8_t *reg = (volatile uint8_t*)address;
   	*reg &= ~(1<<cs);                            // Ligne SS à l'état bas
}


void spi_desactiver(uint8_t address, uint8_t cs) {                           // Désactiver le périphérique
         volatile uint8_t *reg = (volatile uint8_t*)address;
   	*reg |= (1<<cs);                               // Ligne SS à l'état haut
}


uint8_t spi_echange(uint8_t envoi) {                  // Communication sur le bus SPI
        SPDR = envoi;                                        // Octet a envoyer
        while(!(SPSR & (1<<SPIF)));                          // Attente fin envoi (drapeau SPIF du statut)
        return SPDR;                                         // Octet reçu
}

/*
void writeDigit(uint8_t pos, char c) {
    spi_desactiver();
    spi_echange(0x01); // opcode "write digit"
    spi_echange(pos);  // 0..3
    spi_echange(c);    // ASCII
    spi_activer();
}
*/
/*
void displayNumber(int num) {
    char buf[5];
    // conversion simple en texte
    buf[0] = (num / 1000) % 10 + '0';
    buf[1] = (num / 100) % 10 + '0';
    buf[2] = (num / 10) % 10 + '0';
    buf[3] = (num % 10) + '0';

    for (uint8_t i = 0; i < 4; i++)
        writeDigit(i, buf[i]);
}
*/

void seven_seg(){
	while (1) {
        	spi_activer(&PORTB, CS);
		
		spi_echange(0x7E);
		spi_echange(0b1101111);
		
	        spi_desactiver(&PORTB, CS);	
			
		_delay_ms(500);

		spi_activer((uint8_t)&PORTB, CS);
		spi_echange(0x76);
		spi_desactiver((uint8_t)&PORTB, CS);


       		_delay_ms(1000);
    	}
}



int main() {
	spi_init();
	//int v = 0;
	while (1) {
		seven_seg();
		/*
		displayNumber(v);
		v = (v+1) % 10000;
		_delay_ms(200);
		*/
	}
	//select_spi(0, seven_seg);
	return 0;
}
