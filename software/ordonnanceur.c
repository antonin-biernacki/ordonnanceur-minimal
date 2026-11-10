#include "ordonnanceur.h"


// Variable

int Current_task = 0;
process Tasks_list[NBTASKS] = {/*
				{0 , led1_blink, 0,0},
				{0 , led2_blink, 0,0},
				{0 , led3_blink, 0,0},
				{0 , led4_blink, 0,0},
				{0 , led5_blink, 0,0},
				*/
				//{0, ANTI_BLOCK, 0,0},  obsolète après l'ajout de call_process (qui est toujours appelé)
                {0, call_process, 0,0},
				//{0 , main_leds, 0,0},  
				//{0 ,Serial_Message, 0,0}, 
				//{0 ,seven_seg, 0,0}
			       };



// Fonction


ISR(TIMER1_COMPA_vect, ISR_NAKED) {
	SAVE_REGISTERS();
	Tasks_list[Current_task].stackPointer = SP;
	scheduler();
	SP = Tasks_list[Current_task].stackPointer;
	RESTORE_REGISTERS();
	asm volatile("reti");
}


void init_minuteur(int diviseur,long periode){
	TCCR1A=0;               // Le mode choisi n'utilise pas ce registre
	TCCR1B=(1<<CTC1);       // Réinitialisation du minuteur sur expiration
	switch(diviseur){
		case    8: TCCR1B |= (1<<CS11); break;
		case   64: TCCR1B |= (1<<CS11 | 1<<CS10); break;
		case  256: TCCR1B |= (1<<CS12); break;
 		case 1024: TCCR1B |= (1<<CS12 | 1<<CS10); break;
 	}
	// Un cycle prend 1/F_CPU secondes.
	// Un pas de compteur prend diviseur/F_CPU secondes.
	// Pour une periode en millisecondes, il faut (periode/1000)/(diviseur/F_CPU) pas
	// soit (periode*F_CPU)/(1000*diviseur)
	OCR1A=F_CPU/1000*periode/diviseur;  // Calcul du pas
	TCNT1=0;                // Compteur initialisé
	TIMSK1=(1<<OCIE1A);     // Comparaison du compteur avec OCR1A
}


void led_init() {
	DDRC |= (1<<PLED1) | (1<< PLED2);
	DDRB |= (1<<PLED3);
	DDRD |= (1<<PLED4) | (1<<PLED5);
}


void led1_blink() {
	while(1) {
		PORTC ^= (1<<PLED1);
		_delay_ms(50);
	//	wait(0);
	}
}


void led2_blink() {
	while(1) {
		for(int i =0; i<20; i++) {
			PORTC ^= (1<<PLED2);
			_delay_ms(80);
		}
		finish(); //le process se termine au bourt de 20 répétiton
	}
}


void led3_blink() {
	while(1) {
		for(int i =0; i<5; i++) {
			PORTB ^= (1<<PLED3);
			_delay_ms(110);
		}
		wait(10);
	}
}


void led4_blink() {
	while(1) {
		PORTD ^= (1<<PLED4);
		_delay_ms(140);
	}
}


void led5_blink() {
	while(1) {
		for(int i =0; i<5; i++) {
			PORTD ^= (1<<PLED5);
			_delay_ms(170);
		}
		wait(20);
	}
}




void main_leds(){
		add(led1_blink);
		_delay_ms(DELAY);
		add(led2_blink);
       		_delay_ms(DELAY);
 		add(led3_blink);
		_delay_ms(DELAY);
		add(led4_blink); // Ce process démare après la supression du process led2_blink, il prend donc sa place dans le tableau
		_delay_ms(DELAY);
		add(led5_blink);
		_delay_ms(DELAY);
		kill(led1_blink);
		finish();
}

void call_process() {
    unsigned char data;
    while(1) {
        data = USART_Receive();
        if(data == '1') {
            add(led1_blink);
        }
        else if(data == '2') {
            add(led2_blink);
        }
        else if(data == '3') {
            add(led3_blink);
        }
        else if(data == '4') {
            add(led4_blink);
        }
        else if(data == '5') {
            add(led5_blink);
        }
        else if(data == '6') {
            kill(led1_blink);
        }
        else if(data == '7') {
            kill(led2_blink);
        }
        else if(data == '8') {
            kill(led3_blink);
        }
        else if(data == '9') {
            kill(led4_blink);
        }
        else if(data == '0') {
            kill(led5_blink);
        }
        else if(data == 's') {
            add(seven_seg);
        }
        else if(data == 'd') {
            kill(seven_seg);
        }
        USART_Transmit(data);
    }
}
       
void USART_Init(unsigned int ubrr) {
	/*Set baud rate */
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	/*Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

void Serial_Message(){ // call_process est une sorte d'amélioration de Serial_Message, son appel devient donc inutile
    unsigned char data;
    while(1){
        data = USART_Receive();
        USART_Transmit(data);
    }
}

void USART_Transmit(unsigned char data) {
	/* Wait for empty transmit buffer */
	while (!(UCSR0A & (1<<UDRE0)));
	/* Put data into buffer, sends the data */
	UDR0 = data;
	}

unsigned char USART_Receive(void) {
	/* Wait for data to be received */
	while (!(UCSR0A & (1<<RXC0)));
	/* Get and return received data from buffer */
	return UDR0;
}


void spi_init(void) {                                 // Initialisation du bus SPI
	SPI_DDR |= (1<<SPI_MOSI)|(1<<SPI_SCK);   // Définition des sorties
	SPI_DDR &= ~(1<<SPI_MISO);                           // Définition de l'entrée                          
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR1);                 // Activation SPI (SPE) en état maître (MSTR)
        	                                             // horloge F_CPU/64 (SPR1=1,SPR0=0)
	SPI_DDR |= (1<<CS1) | (1<<CS4);;
	DDRD |= (1<<CS5) | (1<<CS6);
	DDRC |= (1<<CS2) | (1<<CS3);
}


void spi_activer(uint16_t port, uint8_t cs) { 	// Activer le périphérique
	volatile uint8_t *reg = (volatile uint8_t*)port;
	*reg &= ~(1<<cs);                            // Ligne SS à l'état bas
}


void spi_desactiver(uint16_t port, uint8_t cs) {                           // Désactiver le périphérique
	volatile uint8_t *reg = (volatile uint8_t*)port;
	*reg |= (1<<cs);                             // Ligne SS à l'état haut
}


uint8_t spi_echange(uint8_t envoi) {                  // Communication sur le bus SPI
	SPDR = envoi;                                        // Octet a envoyer
	while(!(SPSR & (1<<SPIF)));                          // Attente fin envoi (drapeau SPIF du statut)
	return SPDR;                                         // Octet reçu
}


void ANTI_BLOCK(){ //Empèche le scheduler de boucler à l'infini si tout les autres process sont suspendus, inutile après ajout de call_process (qui n'est jamais arrété au suspendu)
	while(1) {
		TCNT1=0;
		TIMER1_COMPA_vect();
	}
}


void scheduler() {
	for (int i=0 ; i<NBTASKS ; i++) {
		if (Tasks_list[i].sleep_time - SLEEP_DEC < 0) {
			Tasks_list[i].sleep_time = 0;
		} else {
			Tasks_list[i].sleep_time -= SLEEP_DEC;
		}
	}


	do {
		Current_task++;
		if (Current_task >= NBTASKS) {
			Current_task = 0;
		}
	} while (Tasks_list[Current_task].sleep_time > 0 || Tasks_list[Current_task].state == ENDED);
}


void seven_seg() {
	while (1) {
        	spi_activer((uint16_t)&PORTB, CS4);
		spi_echange(0x7E);
		spi_echange(0b1101111);
		spi_desactiver((uint16_t)&PORTB, CS4);

		wait(500);

		spi_activer((uint16_t)&PORTB, CS4);
		spi_echange(0x76);
		spi_desactiver((uint16_t)&PORTB, CS4);

		wait(500);
    	}
}


void finish() { //supprime un process lorsque celui-ci est terminé
	cli();
		Tasks_list[Current_task].state =ENDED;
		Tasks_list[Current_task].sleep_time = 0;
		Tasks_list[Current_task].functionAddress = NULL;
	TCNT1=0; 
	sei();
	TIMER1_COMPA_vect();
}

void kill(void (*function)(void)) { //tue le process passé en paramètre
	cli();
	for(int i = 0; i<NBTASKS; i++) {
		if(Tasks_list[i].functionAddress == function) {
			Tasks_list[i].state =ENDED;
			Tasks_list[i].sleep_time = 0;
			Tasks_list[i].functionAddress = NULL;
		}
	} 
	sei();
}


void add(void (*newFunction)(void)) { //ajoute un process
	cli();
	int i =0;
	while(Tasks_list[i].state != ENDED) {
		i++;
	}
	Tasks_list[i].functionAddress = newFunction;
	Tasks_list[i].state = RUN;
	init_stackPointer_tasks(i);
	init_piles(i);

	sei();
	
}




void wait(int time) { // met un process en veille pour une durée donné
	// valeur max du wait 32000 car int sur atmega328p code sur 16 bits donc 32000 max
	cli();
	Tasks_list[Current_task].sleep_time = time;
	TCNT1=0;
	sei();
	TIMER1_COMPA_vect();

}


void init_stackPointer_tasks(int Cprocess) { //initialise le pointer de pile
	if(Tasks_list[Cprocess].state == RUN) {
		Tasks_list[Cprocess].stackPointer = FIRST_STACK_POSITION - (Cprocess * STACK_LENGTH);
	}
}


void init_piles(int Cprocess){ //initilalise la pile
	if(Tasks_list[Cprocess].state == RUN) {
		int save = SP;
		SP = Tasks_list[Cprocess].stackPointer;
		uint16_t address = (uint16_t)Tasks_list[Cprocess].functionAddress;
		asm volatile("push %0" : : "r" (address & 0x00ff) );
		asm volatile("push %0" : : "r" ((address & 0xff00)>>8) );
		SAVE_REGISTERS();
		Tasks_list[Cprocess].stackPointer = SP;
		SP = save;
	}
}

void init_state() { //initialise state
	for (int i= 0; i<NBTASKS; i++) {
		if (Tasks_list[i].functionAddress == NULL) {
			Tasks_list[i].state = ENDED;
		}
		else {
			Tasks_list[i].state = RUN;
		}
	}
}


// Main

int main() {
	cli();
	led_init();
	spi_init();
	USART_Init(MYUBRR);
	init_minuteur(DIV, PER);
	for(int Cprocess =0 ; Cprocess<NBTASKS; Cprocess++) {
		init_stackPointer_tasks(Cprocess);	
		init_piles(Cprocess);
	}
	init_state();
	sei();
	
	SP = Tasks_list[Current_task].stackPointer;
	Tasks_list[Current_task].functionAddress();
	
	return 0;
}
