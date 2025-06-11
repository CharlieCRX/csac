#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <uart.h>

typedef struct uart_info{
  char *dev;
  int speed;
  int fd;
}T_uart_info;

#ifdef DEBUG
#define ERR_LOG(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define ERR_LOG(fmt, ...) do {} while (0)
#endif

#define CHECK_ID(ID)  if(ID>=(sizeof(s_uart_dev)/sizeof(T_uart_info))){  \
              ERR_LOG("uart dev id error!"); \
              return -1;}
#define CHECK_FD(ID)    if(-1 == s_uart_dev[ID].fd)\
              if(-1==(s_uart_dev[ID].fd = uart_init(ID))){\
                return -1;}
static T_uart_info s_uart_dev[] = {
  {NULL, -1, -1},
  {"/dev/ttymxc0", B9600, -1},
  {"/dev/ttymxc1", B57600, -1},
  {"/dev/ttymxc2", B115200, -1},
  {"/dev/ttymxc3", B9600, -1},
  {"/dev/ttymxc4", B9600, -1}
};

int uart_set_speed(const int id ,const int speed)
{
    CHECK_ID(id)
    s_uart_dev[id].speed = speed;
    return 0;
}
int uart_init(const int id)
{
  int ret;
  struct termios options;
  
  CHECK_ID(id)

  s_uart_dev[id].fd = open(s_uart_dev[id].dev, O_RDWR|O_NOCTTY|O_NDELAY);
  if (s_uart_dev[id].fd == -1) {
    printf("[UART] open %s failed!\n", s_uart_dev[id].dev);
    ERR_LOG("[UART] open %s failed!\n", s_uart_dev[id].dev);
    return -1;
  }
   
  // 设置非阻塞模式
  ret = fcntl(s_uart_dev[id].fd, F_SETFL, O_NONBLOCK);
  if (ret == -1) {
    printf("[UART] set uart mode failed!\n");
    ERR_LOG("[UART] set uart mode failed!\n");
    goto out;
  }
  
  /* set baud rate */
  ret = tcgetattr(s_uart_dev[id].fd, &options);
  if (ret == -1) {
    printf("[UART] tcgetattr() failed!\n");
    ERR_LOG("[UART] set uart baud failed!\n");
    goto out;
  }
  /* 57600 */
  ret = cfsetispeed(&options, s_uart_dev[id].speed);
  if (ret == -1) {
    printf("[UART] set uart ispeed(%d) failed\n", s_uart_dev[id].speed);
    ERR_LOG("[UART] set uart ispeed(%d) failed\n", s_uart_dev[id].speed);
    goto out;
  }
  ret = cfsetospeed(&options, s_uart_dev[id].speed);
  if (ret == -1) {
    printf("[UART] set uart ospeed(%d) failed!\n", s_uart_dev[id].speed);
    ERR_LOG("[UART] set uart ospeed(%d) failed!\n", s_uart_dev[id].speed);
    goto out;
  }

  options.c_cflag &= ~CBAUD;
  options.c_cflag |= s_uart_dev[id].speed;
  /* set data bits, 8N,NOPA,1Stop */
  options.c_cflag &= ~CSIZE;
  options.c_cflag |= CS8;
  options.c_cflag &= ~PARENB;  //8b data
  options.c_cflag &= ~CSTOPB;  //1b stop
  options.c_cflag &= ~CRTSCTS; //no flow
  options.c_iflag &= ~(IXON | IXOFF | IXANY);
  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); /* set raw input mode */
  options.c_cflag |= (CLOCAL | CREAD);                /* enable receiver and set local mode */
  options.c_oflag &= ~OPOST;                          /* set raw output */
  options.c_cc[VTIME] = 10;
  options.c_cc[VMIN] = 0;
  ret = tcsetattr(s_uart_dev[id].fd, TCSANOW, &options);
  if (ret == -1) {
    ERR_LOG("[UART] set uart attr failed!\n");
    goto out;
  }
  return 0;

out:
    close(s_uart_dev[id].fd);
    s_uart_dev[id].fd = -1;
    return -1;
}

/**
 * 关闭串口设备
 */
int uart_exit(const int id)
{
  CHECK_ID(id)
  if(s_uart_dev[id].fd != -1)
    close(s_uart_dev[id].fd);
    s_uart_dev[id].fd = -1;
  return 0;
}

int uart_send(const int id, const unsigned char * send_buf, const unsigned int len)
{
  int ret;
  CHECK_ID(id)
  CHECK_FD(id)
  ret = write(s_uart_dev[id].fd, send_buf, len);  
  if (ret == -1 || ret != len) {
    ERR_LOG("[UART] uart%d write failed!\n", id);
    return -1;
  }
   
  return 0;
}

int uart_recv(const int id, unsigned char *recv_buf, const unsigned int len)
{
  int ret;

  CHECK_ID(id)
  CHECK_FD(id)
  //uart_init(id);
  ret = read(s_uart_dev[id].fd, recv_buf, len);
  if (ret == -1) {
      return -1;
  }
  return ret;
}

