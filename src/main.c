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
 */

/************************************************************************/
/* includes                                                             */
/************************************************************************/

#include "asf.h"
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define  a3f    208     // 208 Hz
#define  b3f    233     // 233 Hz
#define  b3     247     // 247 Hz
#define  c4     261     // 261 Hz MIDDLE C
#define  c4s    277     // 277 Hz
#define  e4f    311     // 311 Hz
#define  f4     349     // 349 Hz
#define  a4f    415     // 415 Hz
#define  b4f    466     // 466 Hz
#define  b4     493     // 493 Hz
#define  c5     523     // 523 Hz
#define  c5s    554     // 554 Hz
#define  e5f    622     // 622 Hz
#define  f5     698     // 698 Hz
#define  f5s    740     // 740 Hz
#define  a5f    831     // 831 Hz
#define rest    -1

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
/* constants                                                            */
/************************************************************************/

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
/* interrupcoes                                                         */
/************************************************************************/

void but1_callback(void){
	flag = !flag;
}

void but2_callback(void){
	musica++;
	if (musica == 3) musica = 0;
}
/************************************************************************/
/* funcoes                                                              */
/************************************************************************/

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


/************************************************************************/
/* Main                                                                 */
/************************************************************************/

int melody[] = {
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

int tempo[] = {
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

int underworld_melody[] = {
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

int underworld_tempo[] = {
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

int fur_melody[] = {
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
int fur_tempo[] = {
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
  melody[0] = sizeof(melody) / 4; // / sizeOf(int);
  underworld_melody[0] = sizeof(underworld_tempo) / 4;

  // super loop
  // aplicacoes embarcadas não devem sair do while(1).
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

