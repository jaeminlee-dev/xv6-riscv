#define T_DIR     1   // Directory
#define T_FILE    2   // File
#define T_DEVICE  3   // Device
#define T_SYMLINK 4   // Symbolic Link

#define M_READ  0x1
#define M_WRITE 0x2
#define M_ALL   (M_READ | M_WRITE)

struct stat {
  int dev;     // File system's disk device
  uint ino;    // Inode number
  short type;  // Type of file
  short nlink; // Number of links to file
  uint64 size; // Size of file in bytes
  short mode;  // Protection mode
};

