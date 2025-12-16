#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int mode;

  if(argc != 3){
    fprintf(2, "Usage: chmod mode path\n");
    exit(1);
  }

  mode = atoi(argv[1]);
  if(chmod(argv[2], mode) < 0){
    fprintf(2, "chmod failed\n");
    exit(1);
  }
  exit(0);
}
