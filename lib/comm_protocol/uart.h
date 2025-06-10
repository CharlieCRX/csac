#ifndef _UART_H_
#define _UART_H_
int uart_send(const int id, const unsigned char *send_buf, const unsigned int len);
int uart_recv(const int id, unsigned char *recv_buf, const unsigned int len);
int uart_exit(const int id);
int uart_init(const int id);
int uart_set_speed(const int id ,const int speed);

#endif //_UART_H_

