#include <lib.h>
#include <unistd.h>

PUBLIC int hello (void) {
  message m;
  return (_syscall(PM_PROC_NR, HELLO, &m));
}
