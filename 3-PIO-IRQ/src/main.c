#include <asf.h>

#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"

#define LED_PIO PIOC
#define LED_PIO_ID ID_PIOC
#define LED_PIO_IDX 8
#define LED_PIO_IDX_MASK (1 << LED_PIO_IDX)

#define BUT_PIO PIOA
#define BUT_PIO_ID ID_PIOA
#define BUT_PIO_IDX 11
#define BUT_PIO_IDX_MASK (1 << BUT_PIO_IDX)

#define BUT_LED_1 PIOD
#define BUT_LED_1_ID ID_PIOD
#define BUT_LED_1_IDX 28
#define BUT_LED_1_IDX_MASK (1 << BUT_LED_1_IDX)

#define BUT_LED_2 PIOC
#define BUT_LED_2_ID ID_PIOC
#define BUT_LED_2_IDX 31
#define BUT_LED_2_IDX_MASK (1 << BUT_LED_2_IDX)

#define BUT_LED_3 PIOA
#define BUT_LED_3_ID ID_PIOA
#define BUT_LED_3_IDX 19
#define BUT_LED_3_IDX_MASK (1 << BUT_LED_3_IDX)

void io_init(void);

volatile char but_flag;
volatile char but1_flag;
volatile char but2_flag;
volatile char but3_flag;

uint32_t delay = 200;
int interrupted_but_1 = 0;
int interrupted_but_3 = 0;
double freq = 0.0;
char str[128];


void but_callback(void)
{
	but_flag = 1;
}

void but1_callback(void)
{
	but1_flag = 1;
}

void but2_callback(void)
{
  but2_flag = 1;
}

void but3_callback(void)
{
  but3_flag = 1;
}

int pisca_led(int n, int t, int use_default) {
	int blinks_left = 0;
  	blinks_left = n;
	gfx_mono_draw_filled_rect(0, 25, 128, 5, GFX_PIXEL_CLR);
  	if (use_default) { n = 30; blinks_left = n; }
    	for (int i = 0; i <= n; i++) {
          pio_clear(LED_PIO, LED_PIO_IDX_MASK);
          delay_ms(t);
          pio_set(LED_PIO, LED_PIO_IDX_MASK);
          delay_ms(t);
		  if (!interrupted_but_1 && !interrupted_but_3) {
			  int width = 0;
			  width = 128 - (4 * blinks_left);
			  gfx_mono_draw_filled_rect(0, 25, 128, 5, GFX_PIXEL_CLR);
			  gfx_mono_draw_filled_rect(0, 25, width, 5, GFX_PIXEL_SET);
			  blinks_left--;
		  }
		  else {
			  int new_pos = 0;
			  int remaining = 0;
			  int new_width = 0;
			  new_pos = 128 - (4*n);
			  remaining = 128 - new_pos;
			  new_width = remaining - (4*blinks_left);
			  gfx_mono_draw_filled_rect(0, 25, 128 - (4*n), 5, GFX_PIXEL_SET);
			  gfx_mono_draw_filled_rect(new_pos, 25, new_width, 5, GFX_PIXEL_CLR);
			  gfx_mono_draw_filled_rect(new_pos, 25, new_width, 5, GFX_PIXEL_SET);
			  blinks_left--;
		  }
		  if (but1_flag) {
			  but1_flag = 0;	
			  interrupted_but_1 = 1;
			  return blinks_left;
		  }
		  else if(but3_flag) {
			but3_flag = 0;
			interrupted_but_3 = 1;
			return blinks_left;
		  }
		  else if(but2_flag) {
			  but2_flag = 0;
			  gfx_mono_draw_filled_rect(0, 25, 128, 5, GFX_PIXEL_CLR);
			  return 0;
		  }
		}
		gfx_mono_draw_filled_rect(0, 25, 128, 5, GFX_PIXEL_CLR);
		interrupted_but_1 = interrupted_but_3 = 0;
		return 0;
}

void write_freq(int delay) {
  	freq = (1.0/delay)*1000;
	sprintf(str, "%.2f Hz", freq);
	gfx_mono_draw_string(str, 0, 0, &sysfont);
}

void io_init(void)
{

  // Configura led
	pmc_enable_periph_clk(LED_PIO_ID);
	pio_configure(LED_PIO, PIO_OUTPUT_0, LED_PIO_IDX_MASK, PIO_DEFAULT);

  // Inicializa clock do periférico PIO responsavel pelos botões
	pmc_enable_periph_clk(BUT_PIO_ID);
	pmc_enable_periph_clk(BUT_LED_1_ID);
	pmc_enable_periph_clk(BUT_LED_2_ID);
	pmc_enable_periph_clk(BUT_LED_3_ID);

  // Configura PIO para lidar com o pino do botão como entrada
  // com pull-up
  	pio_configure(BUT_PIO, PIO_INPUT, BUT_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_configure(BUT_LED_1, PIO_INPUT, BUT_LED_1_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_configure(BUT_LED_2, PIO_INPUT, BUT_LED_2_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_configure(BUT_LED_3, PIO_INPUT, BUT_LED_3_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
  	pio_set_debounce_filter(BUT_PIO, BUT_PIO_IDX_MASK, 60);
  	pio_set_debounce_filter(BUT_LED_1, BUT_PIO_IDX_MASK, 60);
  	pio_set_debounce_filter(BUT_LED_2, BUT_PIO_IDX_MASK, 60);
  	pio_set_debounce_filter(BUT_LED_3, BUT_PIO_IDX_MASK, 60);



  // Configura interrupção no pino referente ao botao e associa
  // função de callback caso uma interrupção for gerada
  // a função de callback é a: but_callback()
  	pio_handler_set(BUT_PIO,
                  BUT_PIO_ID,
                  BUT_PIO_IDX_MASK,
                  PIO_IT_EDGE,
                  but_callback);

	pio_handler_set(BUT_LED_1,
				  BUT_LED_1_ID,
				  BUT_LED_1_IDX_MASK,
				  PIO_IT_EDGE,
				  but1_callback);

	pio_handler_set(BUT_LED_2,
				  BUT_LED_2_ID,
				  BUT_LED_2_IDX_MASK,
				  PIO_IT_EDGE,
				  but2_callback);

	pio_handler_set(BUT_LED_3,
				  BUT_LED_3_ID,
				  BUT_LED_3_IDX_MASK,
				  PIO_IT_FALL_EDGE,
				  but3_callback);
	

  // Ativa interrupção e limpa primeira IRQ gerada na ativacao
  	pio_enable_interrupt(BUT_PIO, BUT_PIO_IDX_MASK);
  	pio_get_interrupt_status(BUT_PIO);

	pio_enable_interrupt(BUT_LED_1, BUT_LED_1_IDX_MASK);
	pio_get_interrupt_status(BUT_LED_1);

	pio_enable_interrupt(BUT_LED_2, BUT_LED_2_IDX_MASK);
	pio_get_interrupt_status(BUT_LED_2);

	pio_enable_interrupt(BUT_LED_3, BUT_LED_3_IDX_MASK);
	pio_get_interrupt_status(BUT_LED_3);

  
  // Configura NVIC para receber interrupcoes do PIO do botao
  // com prioridade 4 (quanto mais próximo de 0 maior)
  	NVIC_EnableIRQ(BUT_PIO_ID);
  	NVIC_SetPriority(BUT_PIO_ID, 4); // Prioridade 4

	NVIC_EnableIRQ(BUT_LED_1_ID);
	NVIC_SetPriority(BUT_LED_1_ID, 3); // Prioridade 5

	NVIC_EnableIRQ(BUT_LED_2_ID);
	NVIC_SetPriority(BUT_LED_2_ID, 1); // Prioridade 2

	NVIC_EnableIRQ(BUT_LED_3_ID);
	NVIC_SetPriority(BUT_LED_3_ID, 2); // Prioridade 3
}

int main (void)
{	
	io_init();
	board_init();
	sysclk_init();
	delay_init();
	gfx_mono_ssd1306_init();

	WDT->WDT_MR = WDT_MR_WDDIS;
	int pisca = 0;
	int resto = 0;
  
	while(1) {
		if (but_flag) {
			pisca = pisca_led(30, delay, 1);
			but_flag = 0;
		}
		if(but1_flag || interrupted_but_1) {
			delay_ms(500);
			if (!pio_get(BUT_LED_1, PIO_INPUT, BUT_LED_1_IDX_MASK)) {
				while(!pio_get(BUT_LED_1, PIO_INPUT, BUT_LED_1_IDX_MASK)) {
					delay += 100;
					delay_ms(500);
				}
			}
			else {
				delay -= 100;
				if (delay < 100) {
					delay = 100;
				}
				write_freq(delay);
				delay_ms(500);
			}
			if (interrupted_but_1) {
				write_freq(delay);
				delay_ms(500);
				pisca_led(pisca, delay, 0);
				interrupted_but_1 = 0;
				but1_flag = 0;
			}
			but1_flag = 0;
		}
		if(but3_flag || interrupted_but_3) {
			if(!pio_get(BUT_LED_3, PIO_INPUT, BUT_LED_3_IDX_MASK)) {
				delay += 100;
				write_freq(delay);
				delay_ms(500);
			}
			if (interrupted_but_3) {
				delay_ms(500);
				pisca_led(pisca, delay, 0);
				interrupted_but_3 = 0;
			}
			but3_flag = 0;
		}
		write_freq(delay);
	}
}
