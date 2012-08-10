#include <stdio.h>

int
main(int argc, char **argv) {
  FILE *fp;

  fp = fopen("testfile", "w");
  fprintf(fp, "woooooooo\n");
  fclose(fp);

  fp = fopen("othertestfile", "w");
  fprintf(fp, "woooooooo\n");
  fclose(fp);
  return 0;
}
