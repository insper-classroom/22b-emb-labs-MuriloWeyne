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
void _pio_set(Pio *p_pio, const uint32_t ul_mask)
{
  p_pio-> PIO_SODR = ul_mask;
}

void _pio_clear(Pio *p_pio, const uint32_t ul_mask)
{
  p_pio -> PIO_CODR = ul_mask;
}

void _pio_pull_up(Pio *p_pio, const uint32_t ul_mask, const uint32_t ul_pull_up_enable) {
  if (ul_pull_up_enable) {
    p_pio -> PIO_PUER = ul_mask;
    return;
  }
  p_pio -> PIO_PUDR = ul_mask;
}

void _pio_set_input(Pio *p_pio, const uint32_t ul_mask,
        const uint32_t ul_attribute)
{
  if (ul_attribute & (PIO_DEBOUNCE | PIO_DEGLITCH)) {
    p_pio -> PIO_IFER = ul_mask;
  } else {
    p_pio -> PIO_IFDR = ul_mask;
  }

}

void _pio_set_output(Pio *p_pio, const uint32_t ul_mask,
        const uint32_t ul_default_level,
        const uint32_t ul_multidrive_enable,
        const uint32_t ul_pull_up_enable)
{ 
  if (ul_multidrive_enable) {
    p_pio -> PIO_MDER = ul_mask;
  } else {
    p_pio -> PIO_MDDR = ul_mask;
  }

  if (ul_default_level) {
    _pio_set(p_pio, ul_mask);
  } else {
    _pio_clear(p_pio, ul_mask);
  }
  
  if (ul_pull_up_enable) {
    _pio_pull_up(p_pio, ul_mask, 1);
  } else {
    _pio_pull_up(p_pio, ul_mask, 0);
  }

  p_pio -> PIO_PER = ul_mask;
  p_pio -> PIO_OER = ul_mask;

}

uint32_t _pio_configure(Pio *p_pio, const pio_type_t ul_type, const uint32_t ul_mask, const uint32_t ul_attribute) {
  switch (ul_type) {
    case PIO_INPUT:
      _pio_set_input(p_pio, ul_mask, ul_attribute);
      break;
    case PIO_OUTPUT_0:
    case PIO_OUTPUT_1:
      _pio_set_output(p_pio, ul_mask, (ul_type == PIO_OUTPUT_1),
      (ul_attribute & PIO_OPENDRAIN) ? 1 : 0,
      (ul_attribute & PIO_PULLUP) ? 1 : 0);
      break;
    default:
      return 0;
  }
  return 1;
}

uint32_t _pio_get(Pio *p_pio, const pio_type_t ul_type,
        const uint32_t ul_mask)
{
  uint32_t ul_reg;

  if ((ul_type == PIO_OUTPUT_0) || (ul_type == PIO_OUTPUT_1)) {
    ul_reg = p_pio -> PIO_ODSR;
  } else {
    ul_reg = p_pio -> PIO_PDSR;
  }

  if((ul_reg & ul_mask) == 0) {
    return 0;
  } else {
    return 1;
  }
}

void _delay_ms(int ms) {
  int sysclock = sysclk_get_cpu_hz();
  int delay = (sysclock/1000) * ms;
  while (delay--) {
    __asm__ __volatile__ ("nop");
  };
}

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
  _pio_configure(LED_PIO, PIO_OUTPUT_1, LED_PIO_IDX_MASK, PIO_DEFAULT);
  _pio_configure(LED_1, PIO_OUTPUT_1, LED_1_IDX_MASK, PIO_DEFAULT);
  _pio_configure(LED_2, PIO_OUTPUT_1, LED_2_IDX_MASK, PIO_DEFAULT);
  _pio_configure(LED_3, PIO_OUTPUT_1, LED_3_IDX_MASK, PIO_DEFAULT);
  // Configura os pinos de INPUT
  _pio_configure(BUT_PIO, PIO_INPUT, BUT_PIO_IDX_MASK, PIO_PULLUP);
  _pio_configure(BUT_LED_1, PIO_INPUT, BUT_LED_1_IDX_MASK, PIO_PULLUP);
  _pio_configure(BUT_LED_2, PIO_INPUT, BUT_LED_2_IDX_MASK, PIO_PULLUP);
  _pio_configure(BUT_LED_3, PIO_INPUT, BUT_LED_3_IDX_MASK, PIO_PULLUP);
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
    if (!_pio_get(BUT_PIO, PIO_INPUT, BUT_PIO_IDX_MASK))
    {
      for (int i = 0; i < 10; i++) {
        _pio_clear(LED_PIO, LED_PIO_IDX_MASK);
        _delay_ms(100);
        _pio_set(LED_PIO, LED_PIO_IDX_MASK);
        _delay_ms(100);
      }
    }
    else
    {
      _pio_set(LED_PIO, LED_PIO_IDX_MASK);
    };
    if (!_pio_get(BUT_LED_1, PIO_INPUT, BUT_LED_1_IDX_MASK))
    {
      for (int i = 0; i < 10; i++) {
        pio_clear(LED_1, LED_1_IDX_MASK);
        delay_ms(100);
        _pio_set(LED_1, LED_1_IDX_MASK);
        delay_ms(100);
      }
    }
    else
    {
      _pio_set(LED_1, LED_1_IDX_MASK);
    };
    if (!pio_get(BUT_LED_2, PIO_INPUT, BUT_LED_2_IDX_MASK))
    {
      for (int i = 0; i < 10; i++) {
        _pio_clear(LED_2, LED_2_IDX_MASK);
        _delay_ms(100);
        _pio_set(LED_2, LED_2_IDX_MASK);
        _delay_ms(100);
      } 
    }
    else  
    {
      _pio_set(LED_2, LED_2_IDX_MASK);
    };
    if (!_pio_get(BUT_LED_3, PIO_INPUT, BUT_LED_3_IDX_MASK))
    {
      for (int i = 0; i < 10; i++) {
        _pio_clear(LED_3, LED_3_IDX_MASK);
        _delay_ms(100);
        _pio_set(LED_3, LED_3_IDX_MASK);
        _delay_ms(100);
      }
    }
    else
    {
      _pio_set(LED_3, LED_3_IDX_MASK);
    };
  }

  return 0;
}
