#include <Log.h>
#include <errno.h>
#include <unistd.h>
#include <uxr/client/profile/transport/serial/serial_transport_platform.h>

#include "Hardware.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define RX_BUF_SIZE 1024
#define PIN_TXD 19
#define PIN_RXD 36

bool uxr_init_serial_platform(void *args, int fd, uint8_t remote_addr,
                              uint8_t local_addr) {
  (void)remote_addr;
  (void)local_addr;
  struct uxrSerialPlatform *platform = (struct uxrSerialPlatform *)args;
  UART &uart = UART::create(UART_NUM_1, PIN_TXD, PIN_RXD);
  uart.setClock(115200);
  uart.mode("8N1");
  platform->handle = &uart;
  int rc = uart.init();
  return rc==0 ? true:false;
}

bool uxr_close_serial_platform(void *args) {
  struct uxrSerialPlatform *platform = (struct uxrSerialPlatform *)args;
  UART *pUart = (UART *)platform->handle;
  pUart->deInit();
  return true;
}

size_t uxr_write_serial_data_platform(void *args, const uint8_t *buf,
                                      size_t len, uint8_t *errcode) {
  struct uxrSerialPlatform *platform = (struct uxrSerialPlatform *)args;
  UART *pUart = (UART *)platform->handle;
  int rc = pUart->write(buf, len);
  if ( rc )  WARN(" UART write failed.");
  *errcode = rc;
  return len;
}

size_t uxr_read_serial_data_platform(void *args, uint8_t *buf, size_t len,
                                     int timeout, uint8_t *errcode) {
  struct uxrSerialPlatform *platform = (struct uxrSerialPlatform *)args;
  UART *pUart = (UART *)platform->handle;
  size_t sz=0;
  while(pUart->hasData()) {
    buf[sz++]=pUart->read();
  }
  vTaskDelay(1);
  *errcode = 0;
  return sz;
}
