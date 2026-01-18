#include "app_io.h"
#include "mcp23s17.h"

extern SPI_HandleTypeDef hspi3;

/* ================= CHECK WITH HARDWARE SETUP!!! ================= */

/* IO Expander buses, each device on the bus shares MOSI, SCK, CS */

MCP23S17_Bus_t bus0 = {.hspi = &hspi3, .cs_port = GPIOC, .cs_pin = GPIO_PIN_11};

/*  Two IO Expanders per PCB.
 *   U0 always has addr 0.
 *   U1 always has addr 1.
 *   IO Expander bus can be shared for up to 4 PCBs.
 */

MCP23S17_t pcb0_u0 = {.bus = &bus0, .hw_addr = 0};
MCP23S17_t pcb0_u1 = {.bus = &bus0, .hw_addr = 1};

void IO_InitAll(void) {
  MCP23S17_BusInit(&bus0);

  MCP23S17_Init(&pcb0_u0);
  MCP23S17_Init(&pcb0_u1);
}
