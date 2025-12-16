#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define O_NOFOLLOW 0x800
#define M_READ  0x1
#define M_WRITE 0x2

void
test_permissions()
{
  int fd;
  char *name = "perm_test";

  printf("Test Permissions starting...\n");
  
  // 1. Create file (default rw)
  if((fd = open(name, O_CREATE | O_RDWR)) < 0){
    printf("FAIL: create %s failed\n", name);
    exit(1);
  }
  write(fd, "hello", 5);
  close(fd);

  // 2. Chmod to read-only
  if(chmod(name, M_READ) < 0){
    printf("FAIL: chmod M_READ failed\n");
    exit(1);
  }

  // 3. Try to write (should fail)
  if((fd = open(name, O_WRONLY)) >= 0){
    printf("FAIL: open O_WRONLY succeeded on read-only file\n");
    close(fd);
    exit(1);
  }
  printf("PASS: write protection works\n");

  // 4. Try to read (should succeed)
  if((fd = open(name, O_RDONLY)) < 0){
    printf("FAIL: open O_RDONLY failed on read-only file\n");
    exit(1);
  }
  close(fd);
  printf("PASS: read access works\n");

  // 5. Chmod back to rw
  if(chmod(name, M_READ | M_WRITE) < 0){
    printf("FAIL: chmod M_READ|M_WRITE failed\n");
    exit(1);
  }

  // 6. Try to write (should succeed)
  if((fd = open(name, O_WRONLY)) < 0){
    printf("FAIL: open O_WRONLY failed after chmod rw\n");
    exit(1);
  }
  write(fd, "world", 5);
  close(fd);
  printf("PASS: write access restored\n");

  unlink(name);
  printf("Test Permissions DONE\n\n");
}

void
test_symlinks()
{
  int fd;
  char buf[32];
  
  printf("Test Symlinks starting...\n");
  
  // 1. Simple symlink
  // target -> link
  unlink("target");
  unlink("link");
  
  if((fd = open("target", O_CREATE | O_RDWR)) < 0){
    printf("FAIL: create target failed\n");
    exit(1);
  }
  write(fd, "target_content", 14);
  close(fd);
  
  if(symlink("target", "link") < 0){
    printf("FAIL: symlink failed\n");
    exit(1);
  }
  
  if((fd = open("link", O_RDONLY)) < 0){
    printf("FAIL: open link failed\n");
    exit(1);
  }
  memset(buf, 0, sizeof(buf));
  read(fd, buf, sizeof(buf));
  if(strcmp(buf, "target_content") != 0){
    printf("FAIL: content mismatch via link. Got %s\n", buf);
    exit(1);
  }
  close(fd);
  printf("PASS: Simple symlink read\n");
  
  // 2. Recursive symlink
  // target <- link1 <- link2
  unlink("link1");
  unlink("link2");
  if(symlink("target", "link1") < 0) exit(1);
  if(symlink("link1", "link2") < 0) exit(1);
  
  if((fd = open("link2", O_RDONLY)) < 0){
     printf("FAIL: recursive symlink failed\n");
     exit(1);
  }
  memset(buf, 0, sizeof(buf));
  read(fd, buf, sizeof(buf));
  if(strcmp(buf, "target_content") != 0){
    printf("FAIL: recursive content mismatch. Got %s\n", buf);
    exit(1);
  }
  close(fd);
  printf("PASS: Recursive symlink read\n");
  
  // 3. Cyclic symlink
  // cycle1 -> cycle2 -> cycle1
  unlink("cycle1");
  unlink("cycle2");
  if(symlink("cycle2", "cycle1") < 0) exit(1);
  if(symlink("cycle1", "cycle2") < 0) exit(1);
  
  if((fd = open("cycle1", O_RDONLY)) >= 0){
    printf("FAIL: open cyclic link succeeded (should fail)\n");
    close(fd);
    exit(1);
  }
  printf("PASS: Cyclic symlink detected\n");
  
  // 4. O_NOFOLLOW
  if((fd = open("link", O_NOFOLLOW | O_RDONLY)) < 0){
      printf("FAIL: open O_NOFOLLOW failed\n");
      exit(1);
  }
  memset(buf, 0, sizeof(buf));
  read(fd, buf, sizeof(buf)); // Should read "target" string
  // Note: sys_symlink writes target path without null terminator if we used strlen, 
  // but open/read might differ depending on implementation.
  // In implementation: writei(ip, 0, (uint64)target, 0, strlen(target))
  // so it contains "target".
  buf[6] = 0;
  if(strcmp(buf, "target") != 0){
      printf("FAIL: O_NOFOLLOW read mismatch. Got %s\n", buf);
      exit(1);
  }
  close(fd);
  printf("PASS: O_NOFOLLOW works\n");

  unlink("target");
  unlink("link");
  unlink("link1");
  unlink("link2");
  unlink("cycle1");
  unlink("cycle2");
  
  printf("Test Symlinks DONE\n");
}

int
main(int argc, char *argv[])
{
  printf("Running File System Extensions Tests...\n");
  test_permissions();
  test_symlinks();
  printf("ALL TESTS PASSED\n");
  exit(0);
}
