#include "stm32h7xx_hal.h"
#include <stdint.h>

typedef struct {
  SPI_HandleTypeDef *hspi;
  GPIO_TypeDef *cs_port;
  uint16_t cs_pin;
} MCP23S17_Bus_t;

typedef struct {
  MCP23S17_Bus_t *bus;
  uint8_t hw_addr;      // 0..7 (A2..A0)
  uint8_t porta_shadow; // write-only mirror
  uint8_t portb_shadow; // write-only mirror
} MCP23S17_t;

/* Init Bus (multiple devices): enables HAEN so chip select address works */
void MCP23S17_BusInit(MCP23S17_Bus_t *bus);

/* Init IO Expander: all 16 pins output, all LOW */
void MCP23S17_Init(MCP23S17_t *dev);

/* Write full 8-bit ports */
void MCP23S17_WritePortA(MCP23S17_t *dev, uint8_t value);
void MCP23S17_WritePortB(MCP23S17_t *dev, uint8_t value);

/* Write all 16 pins with bitmask: bits 0..7->A, 8..15->B */
void MCP23S17_Write16(MCP23S17_t *dev, uint16_t value);

/* Set all pins HIGH(1) or LOW(0) */
void MCP23S17_SetAll(MCP23S17_t *dev, uint8_t state);

/* Write single pin 0..15 */
void MCP23S17_WritePin(MCP23S17_t *dev, uint8_t pin, uint8_t state);