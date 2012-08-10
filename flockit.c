#include <dlfcn.h>
#include <gnu/lib-names.h>   /* defines LIBC_SO */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>

static int (*libc_open)(const char *, int, ...);

void
flockit_setup(void) {
  void *libc_handle;

  if (!libc_open) {
    dlerror();                  /* clear any existing error */

    libc_handle = dlopen(LIBC_SO, RTLD_LAZY);
    if (!libc_handle) {
      fprintf(stderr, "flockit can't find libc: %s\n", dlerror());
      exit(1);
    }

    libc_open = dlsym(libc_handle, "open");
    if (!libc_open) {
      fprintf(stderr, "flockit can't find fopen in libc: %s\n", dlerror());
      exit(1);
    }

    dlclose(libc_handle);
    dlerror();                  /* clear any existing error */
  }
}


int
open(const char *file, int flags, ...) {
  int fd, flock_operation;
  mode_t mode;
  va_list argp;
  char *flockit_file_prefix;

  printf("called for %s\n", file);

  va_start(argp, flags);

  flockit_setup();

  if (flags & O_CREAT) {
    mode = va_arg(argp, mode_t);
    fd = libc_open(file, flags, mode);
  } else {
    fd = libc_open(file, flags);
  }
  va_end(argp);

  if (flags & O_WRONLY || flags & O_RDWR) {
    flock_operation = LOCK_EX;
  } else {
    flock_operation = LOCK_SH;
  }

  flockit_file_prefix = getenv("FLOCKIT_FILE_PREFIX");

  if (fd >= 0 && flockit_file_prefix &&
      !strncmp(file, flockit_file_prefix, strlen(flockit_file_prefix))) {
    flock(fd, flock_operation);
  }

  return fd;
}
