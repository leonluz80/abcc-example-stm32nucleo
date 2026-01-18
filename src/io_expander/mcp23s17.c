#include "mcp23s17.h"

/* Registers */
#define MCP_IODIRA 0x00
#define MCP_IODIRB 0x01
#define MCP_IOCON 0x0A
#define MCP_GPIOA 0x12
#define MCP_GPIOB 0x13

/* IOCON bits */
#define IOCON_HAEN (1U << 3)  // Hardware Address Enable
#define IOCON_SEQOP (1U << 5) // Sequential Operation Disabled

/* Opcodes */
#define MCP_OPCODE_WRITE(addr) (0x40 | ((addr & 0x07) << 1))

/* Chip Select control */
static inline void cs_low(MCP23S17_Bus_t *bus) {
  HAL_GPIO_WritePin(bus->cs_port, bus->cs_pin, GPIO_PIN_RESET);
}
static inline void cs_high(MCP23S17_Bus_t *bus) {
  HAL_GPIO_WritePin(bus->cs_port, bus->cs_pin, GPIO_PIN_SET);
}

/* Low-level SPI write for a specific device */
static void write_reg(MCP23S17_t *dev, uint8_t reg, uint8_t data) {
  uint8_t tx[3] = {MCP_OPCODE_WRITE(dev->hw_addr), reg, data};

  cs_low(dev->bus);
  HAL_SPI_Transmit(dev->bus->hspi, tx, 3, HAL_MAX_DELAY);
  cs_high(dev->bus);
}

/* SPI write used before adress is enabled */
static void write_reg_broadcast(MCP23S17_Bus_t *bus, uint8_t reg,
                                uint8_t data) {
  /* When HAEN=0, address bits are ignored -> both chips respond */
  uint8_t tx[3] = {MCP_OPCODE_WRITE(0), reg, data};

  cs_low(bus);
  HAL_SPI_Transmit(bus->hspi, tx, 3, HAL_MAX_DELAY);
  cs_high(bus);
}

void MCP23S17_BusInit(MCP23S17_Bus_t *bus) {
  /* Enable hardware addressing on ALL MCP23S17 sharing this CS */
  uint8_t iocon = (uint8_t)(IOCON_HAEN | IOCON_SEQOP);
  write_reg_broadcast(bus, MCP_IOCON, iocon);
}

void MCP23S17_Init(MCP23S17_t *dev) {
  /* set all outputs */
  write_reg(dev, MCP_IODIRA, 0x00);
  write_reg(dev, MCP_IODIRB, 0x00);

  /* clear outputs */
  dev->porta_shadow = 0x00;
  dev->portb_shadow = 0x00;
  write_reg(dev, MCP_GPIOA, dev->porta_shadow);
  write_reg(dev, MCP_GPIOB, dev->portb_shadow);
}

void MCP23S17_WritePortA(MCP23S17_t *dev, uint8_t value) {
  dev->porta_shadow = value;
  write_reg(dev, MCP_GPIOA, dev->porta_shadow);
}

void MCP23S17_WritePortB(MCP23S17_t *dev, uint8_t value) {
  dev->portb_shadow = value;
  write_reg(dev, MCP_GPIOB, dev->portb_shadow);
}

void MCP23S17_Write16(MCP23S17_t *dev, uint16_t value) {
  dev->porta_shadow = (uint8_t)(value & 0xFF);
  dev->portb_shadow = (uint8_t)((value >> 8) & 0xFF);

  write_reg(dev, MCP_GPIOA, dev->porta_shadow);
  write_reg(dev, MCP_GPIOB, dev->portb_shadow);
}

void MCP23S17_SetAll(MCP23S17_t *dev, uint8_t state) {
  uint8_t v = state ? 0xFF : 0x00;
  dev->porta_shadow = v;
  dev->portb_shadow = v;

  write_reg(dev, MCP_GPIOA, dev->porta_shadow);
  write_reg(dev, MCP_GPIOB, dev->portb_shadow);
}

void MCP23S17_WritePin(MCP23S17_t *dev, uint8_t pin, uint8_t state) {
  if (pin < 8) {
    if (state)
      dev->porta_shadow |= (1U << pin);
    else
      dev->porta_shadow &= ~(1U << pin);
    write_reg(dev, MCP_GPIOA, dev->porta_shadow);
  } else if (pin < 16) {
    uint8_t p = (uint8_t)(pin - 8);
    if (state)
      dev->portb_shadow |= (1U << p);
    else
      dev->portb_shadow &= ~(1U << p);
    write_reg(dev, MCP_GPIOB, dev->portb_shadow);
  }
}
