/**
 * 5 semestre - Eng. da Computação - Insper
 * Rafael Corsi - rafael.corsi@insper.edu.br
 *
 * Projeto 0 para a placa SAME70-XPLD
 *
 * Objetivo :
 *  - Introduzir ASF e HAL
 *  - Configuracao de clock
 *  - Configuracao pino In/Out
 *
 * Material :
 *  - Kit: ATMEL SAME70-XPLD - ARM CORTEX M7
 *
 * Dupla:
 *  - Iago Mendes e Jorge Augusto
 */

/************************************************************************/
/* includes                                                             */
/************************************************************************/

#include "asf.h"
#include "notas.h"

/************************************************************************/
/* defines                                                              */
/************************************************************************/

#define LED_PIO           PIOC                  // periferico que controla o LED
#define LED_PIO_ID        ID_PIOC               // ID do periférico PIOC (controla LED)
#define LED_PIO_IDX       8u                    // ID do LED no PIO
#define LED_PIO_IDX_MASK  (1u << LED_PIO_IDX)   // Mascara para CONTROLARMOS o LED

#define BUT1_PIO	          PIOC
#define BUT1_PIO_ID        ID_PIOC
#define BUT1_PIO_IDX       13u
#define BUT1_PIO_IDX_MASK (1u << BUT1_PIO_IDX)

#define BUT2_PIO	          PIOA
#define BUT2_PIO_ID        ID_PIOA
#define BUT2_PIO_IDX       4u
#define BUT2_PIO_IDX_MASK (1u << BUT2_PIO_IDX)

#define BUZ_PIO	          PIOD
#define BUZ_PIO_ID        ID_PIOD
#define BUZ_PIO_IDX       30u
#define BUZ_PIO_IDX_MASK (1u << BUZ_PIO_IDX)


/************************************************************************/
/* variaveis globais                                                    */
/************************************************************************/

volatile Bool flag;
volatile int musica;

/************************************************************************/
/* prototypes                                                           */
/************************************************************************/

void init(void);

/************************************************************************/
/* callbacks                                                            */
/************************************************************************/

void but1_callback(void){
	flag = !flag;
}

void but2_callback(void){
	flag = false;
	musica++;
	if (musica == 3) musica = 0;
}
/************************************************************************/
/* funcoes                                                              */
/************************************************************************/

// Função de inicialização do uC
void init(void)
{
	// Initialize the board clock
	sysclk_init();
	
	// Desativa WatchDog Timer
	WDT->WDT_MR = WDT_MR_WDDIS;
	
	// Ativa o PIO na qual o LED foi conectado
	// para que possamos controlar o LED.
	pmc_enable_periph_clk(LED_PIO_ID);
	
	//Inicializa PC8 como saída
	pio_set_output(LED_PIO, LED_PIO_IDX_MASK, 0, 0, 0);
	
	pmc_enable_periph_clk(BUZ_PIO_ID);
		
	pio_set_output(BUZ_PIO, BUZ_PIO_IDX_MASK, 0, 0, 0);
	
	// Inicializa PIO do botao
	pmc_enable_periph_clk(BUT1_PIO_ID);
	pmc_enable_periph_clk(BUT2_PIO_ID);
	
	pio_configure(BUT1_PIO, PIO_INPUT, BUT1_PIO_IDX_MASK, PIO_PULLUP);
	pio_configure(BUT2_PIO, PIO_INPUT, BUT2_PIO_IDX_MASK, PIO_PULLUP);

	// Configura interrupção no pino referente ao botao e associa
	// função de callback caso uma interrupção for gerada
	// a função de callback é a: but_callback()
	pio_handler_set(BUT1_PIO,
	BUT1_PIO_ID,
	BUT1_PIO_IDX_MASK,
	PIO_IT_RISE_EDGE,
	but1_callback);
	
	pio_handler_set(BUT2_PIO,
	BUT2_PIO_ID,
	BUT2_PIO_IDX_MASK,
	PIO_IT_RISE_EDGE,
	but2_callback);

	// Ativa interrupção
	pio_enable_interrupt(BUT1_PIO, BUT1_PIO_IDX_MASK);
	pio_enable_interrupt(BUT2_PIO, BUT2_PIO_IDX_MASK);

	// Configura NVIC para receber interrupcoes do PIO do botao
	// com prioridade 4 (quanto mais próximo de 0 maior)
	NVIC_EnableIRQ(BUT1_PIO_ID);
	NVIC_SetPriority(BUT1_PIO_ID, 4); // Prioridade 4
	
	NVIC_EnableIRQ(BUT2_PIO_ID);
	NVIC_SetPriority(BUT2_PIO_ID, 4); // Prioridade 4
}

// função para o buzzer tocar na frequência certa por certo tempo
void buzz(int freq, int len) {
	pio_set(BUZ_PIO, BUZ_PIO_IDX_MASK);
	
	int delay = 1000000 / freq / 2;
	
	int ciclos = freq * len / 1000;
	
	for (int i = 0; i < ciclos; i++) {
		pio_set(BUZ_PIO, BUZ_PIO_IDX_MASK);
		pio_clear(LED_PIO, LED_PIO_IDX_MASK);
		delay_us(delay);
		pio_clear(BUZ_PIO, BUZ_PIO_IDX_MASK);
		pio_set(LED_PIO, LED_PIO_IDX_MASK);
		delay_us(delay);
		
	}
	
	pio_clear(BUZ_PIO, BUZ_PIO_IDX_MASK);
}

/************************************************************************/
/* Musicas																*/
/* autoria:																*/
/* musicas mario: https://www.hackster.io/techarea98/super-mario-theme-song-with-piezo-buzzer-and-arduino-2cc461 */
/* fur elise: http://repairmypc.net/2017/08/test-post/					*/
/************************************************************************/

const int melody[] = {
	NOTE_E7, NOTE_E7, 0, NOTE_E7,
	0, NOTE_C7, NOTE_E7, 0,
	NOTE_G7, 0, 0,  0,
	NOTE_G6, 0, 0, 0,
	
	NOTE_C7, 0, 0, NOTE_G6,
	0, 0, NOTE_E6, 0,
	0, NOTE_A6, 0, NOTE_B6,
	0, NOTE_AS6, NOTE_A6, 0,
	
	NOTE_G6, NOTE_E7, NOTE_G7,
	NOTE_A7, 0, NOTE_F7, NOTE_G7,
	0, NOTE_E7, 0, NOTE_C7,
	NOTE_D7, NOTE_B6, 0, 0,
	
	NOTE_C7, 0, 0, NOTE_G6,
	0, 0, NOTE_E6, 0,
	0, NOTE_A6, 0, NOTE_B6,
	0, NOTE_AS6, NOTE_A6, 0,
	
	NOTE_G6, NOTE_E7, NOTE_G7,
	NOTE_A7, 0, NOTE_F7, NOTE_G7,
	0, NOTE_E7, 0, NOTE_C7,
	NOTE_D7, NOTE_B6, 0, 0
};

const int tempo[] = {
	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12,
	
	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12,
	
	9, 9, 9,
	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12,
	
	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12,
	
	9, 9, 9,
	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12
};

const int underworld_melody[] = {
	125,
	NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
	NOTE_AS3, NOTE_AS4, 0,
	0,
	NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
	NOTE_AS3, NOTE_AS4, 0,
	0,
	NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
	NOTE_DS3, NOTE_DS4, 0,
	0,
	NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
	NOTE_DS3, NOTE_DS4, 0,
	0, NOTE_DS4, NOTE_CS4, NOTE_D4,
	NOTE_CS4, NOTE_DS4,
	NOTE_DS4, NOTE_GS3,
	NOTE_G3, NOTE_CS4,
	NOTE_C4, NOTE_FS4, NOTE_F4, NOTE_E3, NOTE_AS4, NOTE_A4,
	NOTE_GS4, NOTE_DS4, NOTE_B3,
	NOTE_AS3, NOTE_A3, NOTE_GS3,
	0, 0, 0
};

const int underworld_tempo[] = {
	125,
	12, 12, 12, 12,
	12, 12, 6,
	3,
	12, 12, 12, 12,
	12, 12, 6,
	3,
	12, 12, 12, 12,
	12, 12, 6,
	3,
	12, 12, 12, 12,
	12, 12, 6,
	6, 18, 18, 18,
	6, 6,
	6, 6,
	6, 6,
	18, 18, 18, 18, 18, 18,
	10, 10, 10,
	10, 10, 10,
	3, 3, 3
};

const int fur_melody[] = {
	NOTE_B4, NOTE_A4, NOTE_GS4, NOTE_A4, NOTE_C5,
	NOTE_D5, NOTE_C5, NOTE_B4, NOTE_C5, NOTE_E5,
	NOTE_F5, NOTE_E5, NOTE_DS5, NOTE_E5,
	NOTE_B5, NOTE_A5, NOTE_GS5, NOTE_A5, NOTE_B5, NOTE_A5, NOTE_GS5, NOTE_A5, NOTE_C6,
	NOTE_A5, NOTE_C6, NOTE_G5, NOTE_A5, NOTE_B5, NOTE_A5,
	NOTE_G5, NOTE_A5, NOTE_G5, NOTE_A5, NOTE_B5, NOTE_A5, NOTE_G5, NOTE_A5, NOTE_G5, NOTE_A5, NOTE_B5, NOTE_A5, NOTE_G5, NOTE_FS5, NOTE_E5,
	NOTE_E5, NOTE_F5, NOTE_G5, NOTE_G5, NOTE_A5, NOTE_G5, NOTE_F5, NOTE_E5, NOTE_D5,
	NOTE_E5, NOTE_F5, NOTE_G5, NOTE_G5, NOTE_A5, NOTE_G5, NOTE_F5, NOTE_E5, NOTE_D5,
	NOTE_C5, NOTE_D5, NOTE_E5, NOTE_E5, NOTE_F5, NOTE_E5, NOTE_D5, NOTE_C5, NOTE_B4,
	NOTE_C5, NOTE_D5, NOTE_E5, NOTE_E5, NOTE_F5, NOTE_E5, NOTE_D5, NOTE_C5, NOTE_B4,
	NOTE_B4, NOTE_A4, NOTE_GS4, NOTE_A4, NOTE_C5,
	NOTE_D5, NOTE_C5, NOTE_B4, NOTE_C5, NOTE_E5,
	NOTE_F5, NOTE_E5, NOTE_DS5, NOTE_E5,
	NOTE_B5, NOTE_A5, NOTE_GS5, NOTE_A5, NOTE_B5, NOTE_A5, NOTE_GS5, NOTE_A5, NOTE_C6,
	NOTE_A5, NOTE_B5, NOTE_C6,  NOTE_B5, NOTE_A5, NOTE_GS5, NOTE_A5, NOTE_E5, NOTE_F5, NOTE_D5, NOTE_C5, NOTE_B4, NOTE_A4, NOTE_B4, NOTE_A4,
	NOTE_A5, NOTE_B5, NOTE_CS6, NOTE_A5, NOTE_B5, NOTE_CS6,  NOTE_B5,NOTE_A5,NOTE_GS5,NOTE_FS5,NOTE_GS5,NOTE_A5, NOTE_B5,NOTE_GS5,NOTE_E5,
	NOTE_A5, NOTE_B5, NOTE_CS6, NOTE_A5, NOTE_B5, NOTE_CS6,  NOTE_B5,NOTE_A5,NOTE_GS5,NOTE_FS5, NOTE_B5,NOTE_GS5,NOTE_E5,NOTE_A5,
	NOTE_A5, NOTE_B5, NOTE_CS6, NOTE_A5, NOTE_B5, NOTE_CS6,  NOTE_B5,NOTE_A5,NOTE_GS5,NOTE_FS5,NOTE_GS5,NOTE_A5, NOTE_B5,NOTE_GS5,NOTE_E5,
	NOTE_A5, NOTE_B5, NOTE_CS6, NOTE_A5, NOTE_B5, NOTE_CS6,  NOTE_B5,NOTE_A5,NOTE_GS5,NOTE_FS5, NOTE_B5,NOTE_GS5,NOTE_E5,NOTE_A5,
	
};
const int fur_tempo[] = {
	8, 8, 8, 8, 2,
	8, 8, 8, 8, 2,
	8, 8, 8, 8,
	8,8,8,8,8,8,8,8,2,
	4,4,16,16,16,4,
	4,4,16,16,16,4,4,4,16,16,16,4,4,4,2,
	4,4,4,4,8,8,8,8,2,
	4,4,4,4,8,8,8,8,2,
	4,4,4,4,8,8,8,8,2,
	4,4,4,4,8,8,8,8,2,
	8, 8, 8, 8, 2,
	8, 8, 8, 8, 2,
	8, 8, 8, 8,
	8,8,8,8,8,8,8,8,2,
	4,4,4,4,4,4,4,4,4,4,2,4,8,8,2,
	4,4,2,4,4,4,4,4,4,4,4,4,4,4,4,
	4,4,2,4,4,4,4,4,4,4,4,4,4,2,
	4,4,2,4,4,4,4,4,4,4,4,4,4,4,4,
	4,4,2,4,4,4,4,4,4,4,4,4,4,2,
};


int array_melody[] = {&melody, &underworld_melody, &fur_melody};
int array_tempo[] = {&tempo, &underworld_tempo, &fur_tempo};
float array_velocidade[] = {1.30, 1.30, 0.20};

// Funcao principal chamada na inicalizacao do uC.
int main(void)
{	
  init();
  flag = false;
  melody[0] = sizeof(melody) / sizeof(melody[0]);
  underworld_melody[0] = sizeof(underworld_tempo) / 4;

  // super loop
  while (1)
  {
	  if(!flag) pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
	  if (flag){
		  int size = *((int *) array_melody[musica] + 0);
		  for (int i = 0; (i <= size) && flag; i++) {
				buzz(*((int *) array_melody[musica] + i), 1000 / *((int *) array_tempo[musica] + i));
				float pause = 1000 / *((int *) array_tempo[musica] + i) * array_velocidade[musica];
				delay_ms(pause);  
		}
		flag = false;
	}
  }
  return 0;
}

