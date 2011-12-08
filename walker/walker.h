#include <stdio.h>
#include <unistd.h>

#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <errno.h>
#include <time.h>

#define MAX_LEN 256
#define MAX_OUTPUT 2048
#define OPT_INODE 1
#define OPT_ID 2
#define OPT_MTIME 4
#define OPT_SIZE 8

int options;

void read_dir_rec(char* buf, int indent, int pos);
char* print_dir(char* buf, int pos);
char* print_file(struct stat fbuf, char* buf, int start);
void print_info (char* buffer, int indent);
