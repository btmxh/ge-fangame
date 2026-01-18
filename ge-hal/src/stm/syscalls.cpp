#include <cstddef>
#include <sys/stat.h>

#include "ge-hal/stm/time.hpp"
#include "ge-hal/stm/uart.hpp"

extern ge::hal::stm::UARTHandle stdout_usart;

extern "C" {
int _fstat(int fd, struct stat *st) {
  if (fd < 0)
    return -1;
  st->st_mode = S_IFCHR;
  return 0;
}

void *_sbrk(int incr) {
  extern char _end;
  static unsigned char *heap = NULL;
  unsigned char *prev_heap;
  if (heap == NULL)
    heap = (unsigned char *)&_end;
  prev_heap = heap;
  heap += incr;
  return prev_heap;
}

int _open(const char *path) {
  (void)path;
  return -1;
}

int _close(int fd) {
  (void)fd;
  return -1;
}

int _isatty(int fd) {
  (void)fd;
  return 1;
}

int _lseek(int fd, int ptr, int dir) {
  (void)fd, (void)ptr, (void)dir;
  return 0;
}

void _exit(int status) {
  (void)status;
  for (;;)
    __BKPT(0);
}

void _kill(int pid, int sig) { (void)pid, (void)sig; }

int _getpid(void) { return -1; }

int _write(int fd, char *ptr, int len) {
  (void)fd, (void)ptr, (void)len;
  if (stdout_usart.uart && fd == 1) {
    for (int i = 0; i < len; ++i) {
      stdout_usart.write(static_cast<ge::u8>(ptr[i]));
      while (!stdout_usart.is_written())
        ge::hal::stm::delay_spin(1);
    }
  }
  return -1;
}

int _read(int fd, char *ptr, int len) {
  (void)fd, (void)ptr, (void)len;
  return -1;
}

int _link(const char *a, const char *b) {
  (void)a, (void)b;
  return -1;
}

int _unlink(const char *a) {
  (void)a;
  return -1;
}

int _stat(const char *path, struct stat *st) {
  (void)path, (void)st;
  return -1;
}

int mkdir(const char *path, mode_t mode) {
  (void)path, (void)mode;
  return -1;
}
}
