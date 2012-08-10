#include <dlfcn.h>
#include <gnu/lib-names.h>   /* defines LIBC_SO */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>

/* Wrap libc's fopen() and fopen64() to add locking.

   If a file is opened for writing (or r+w), we obtain an exclusive
   lock via flock() on the whole file before returning the file
   pointer.

   If a file is opened only for reading, we obtain a shared lock.

   This only applies if the environment variable FLOCKIT_FILE_PREFIX
   is set, and it only applies to files whose names start with
   FLOCKIT_FILE_PREFIX. Other files are unaffected. If
   FLOCKIT_FILE_PREFIX is not set, no locking is done.
*/

static FILE *(*libc_fopen)(const char *, const char *);
static FILE *(*libc_fopen64)(const char *, const char *);

void
flockit_setup(void) {
  void* libc_handle;

  if (!libc_fopen || !libc_fopen64) {
    dlerror();                  /* clear any existing error */

    libc_handle = dlopen(LIBC_SO, RTLD_LAZY);
    if (!libc_handle) {
      fprintf(stderr, "flockit can't find libc: %s\n", dlerror());
      exit(1);
    }

    libc_fopen = dlsym(libc_handle, "fopen");
    if (!libc_fopen) {
      fprintf(stderr, "flockit can't find fopen in libc: %s\n", dlerror());
      exit(1);
    }

    libc_fopen64 = dlsym(libc_handle, "fopen64");
    if (!libc_fopen64) {
      fprintf(stderr, "flockit can't find fopen in libc: %s\n", dlerror());
      exit(1);
    }

    dlclose(libc_handle);
    dlerror();                  /* clear any existing error */
  }
}



FILE*
flockit_call(FILE *(*fopener)(const char *, const char *),
             const char *file, const char *mode) {
  FILE *fp;
  int flock_operation;
  const char *flockit_file_prefix;

  fp = fopener(file, mode);

  if (strstr(mode, "w")) {
    flock_operation = LOCK_EX;
  } else {
    flock_operation = LOCK_SH;
  }

  flockit_file_prefix = getenv("FLOCKIT_FILE_PREFIX");

  if (fp && flockit_file_prefix &&
      !strncmp(file, flockit_file_prefix, strlen(flockit_file_prefix))) {
    flock(fileno(fp), flock_operation);
  }

  return fp;
}


/* Here's the actual functions we're wrapping */
FILE*
fopen64(const char *file, const char *mode) {
  flockit_setup();
  return flockit_call(libc_fopen64, file, mode);
}


FILE*
fopen(const char *file, const char *mode) {
  flockit_setup();
  return flockit_call(libc_fopen, file, mode);
}
