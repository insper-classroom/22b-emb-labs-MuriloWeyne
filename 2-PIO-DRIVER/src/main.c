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

/************************************************************************/
/* defines                                                              */
/************************************************************************/

#define LED_PIO PIOC
#define LED_PIO_ID ID_PIOC
#define LED_PIO_IDX 8
#define LED_PIO_IDX_MASK (1 << LED_PIO_IDX)

#define LED_1 PIOA
#define LED_1_ID ID_PIOA 
#define LED_1_IDX 0
#define LED_1_IDX_MASK (1 << LED_1_IDX)

#define LED_2 PIOC
#define LED_2_ID ID_PIOC
#define LED_2_IDX 30
#define LED_2_IDX_MASK (1 << LED_2_IDX)

#define LED_3 PIOB
#define LED_3_ID ID_PIOB
#define LED_3_IDX 2
#define LED_3_IDX_MASK (1 << LED_3_IDX)

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



/************************************************************************/
/* constants                                                            */
/************************************************************************/

/************************************************************************/
/* variaveis globais                                                    */
/************************************************************************/

/************************************************************************/
/* prototypes                                                           */
/************************************************************************/

void init(void);

/************************************************************************/
/* interrupcoes                                                         */
/************************************************************************/

/************************************************************************/
/* funcoes                                                              */
/************************************************************************/

// Função de inicialização do uC
void init(void)
{
  sysclk_init();

  WDT->WDT_MR = WDT_MR_WDDIS;

  // Ativa os PIOS dos perifericos
  pmc_enable_periph_clk(LED_PIO_ID);
  pmc_enable_periph_clk(BUT_PIO_ID);
  pmc_enable_periph_clk(LED_1_ID);
  pmc_enable_periph_clk(LED_2_ID);
  pmc_enable_periph_clk(LED_3_ID);
  pmc_enable_periph_clk(BUT_LED_1_ID);
  pmc_enable_periph_clk(BUT_LED_2_ID);
  pmc_enable_periph_clk(BUT_LED_3_ID);
  /* 
  Configura os pinos de OUTPUT. Deve ser inicializado como 1 dado que o LED fica apagado quando o pino esta em nivel alto.
  */
  pio_configure(LED_PIO, PIO_OUTPUT_1, LED_PIO_IDX_MASK, PIO_DEFAULT);
  pio_configure(LED_1, PIO_OUTPUT_1, LED_1_IDX_MASK, PIO_DEFAULT);
  pio_configure(LED_2, PIO_OUTPUT_1, LED_2_IDX_MASK, PIO_DEFAULT);
  pio_configure(LED_3, PIO_OUTPUT_1, LED_3_IDX_MASK, PIO_DEFAULT);
  // Configura os pinos de INPUT
  pio_configure(BUT_PIO, PIO_INPUT, BUT_PIO_IDX_MASK, PIO_PULLUP);
  pio_configure(BUT_LED_1, PIO_INPUT, BUT_LED_1_IDX_MASK, PIO_PULLUP);
  pio_configure(BUT_LED_2, PIO_INPUT, BUT_LED_2_IDX_MASK, PIO_PULLUP);
  pio_configure(BUT_LED_3, PIO_INPUT, BUT_LED_3_IDX_MASK, PIO_PULLUP);
}

/************************************************************************/
/* Main                                                                 */
/************************************************************************/

// Funcao principal chamada na inicalizacao do uC.
int main(void)
{
  init();

  // super loop
  // aplicacoes embarcadas não devem sair do while(1).
  while (1)
  {
    if (!pio_get(BUT_PIO, PIO_INPUT, BUT_PIO_IDX_MASK))
    {
      pio_clear(LED_PIO, LED_PIO_IDX_MASK);
    }
    else
    {
      pio_set(LED_PIO, LED_PIO_IDX_MASK);
    };
    if (!pio_get(BUT_LED_1, PIO_INPUT, BUT_LED_1_IDX_MASK))
    {
      pio_clear(LED_1, LED_1_IDX_MASK);
    }
    else
    {
      pio_set(LED_1, LED_1_IDX_MASK);
    };
    if (!pio_get(BUT_LED_2, PIO_INPUT, BUT_LED_2_IDX_MASK))
    {
      pio_clear(LED_2, LED_2_IDX_MASK);
    }
    else
    {
      pio_set(LED_2, LED_2_IDX_MASK);
    };
    if (!pio_get(BUT_LED_3, PIO_INPUT, BUT_LED_3_IDX_MASK))
    {
      pio_clear(LED_3, LED_3_IDX_MASK);
    }
    else
    {
      pio_set(LED_3, LED_3_IDX_MASK);
    };
  }

  return 0;
}
