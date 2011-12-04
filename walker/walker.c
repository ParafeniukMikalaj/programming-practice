#include "stdio.h"
#include "sys/types.h"
#include "dirent.h"
#include "sys/stat.h"
#include "unistd.h"
#include "string.h"
#include <errno.h>
#include <time.h>

#define MAX_LEN 256
#define MAX_STRING 16356
#define MAX_OUTPUT 2048
#define INODE 1
#define ID 2
#define MODIFIED 4
#define SIZE 8

//buffer used to save current full path
static char PATH[MAX_STRING];
int options;
//store the depth of PATH
int level;

//retrieves file type and call appropriate method
void proc_entry(char* path, int pos);
//retrieves directory name and prints it
void proc_dir(char* path, int pos);
//retrieves file name and another info depending on cmd parameters and prints it
void proc_file(char* buf, int pos);
//prints directory name and reads all incoming files
void read_dir_req(char *buf, int pos);
//shifts buffer to level characters and fill them with tabs
void make_output(char* buffer);

int main(int argc, char* argv[])
{
	level = -1;
	int res = -1;
	struct stat fbuf;
	if (argc > 1) {
		strcpy(PATH, argv[1]);
		res = stat(argv[1], &fbuf); 
	}
	if (res == -1) getcwd(PATH, MAX_STRING);
	while ((res = getopt(argc, argv, "ispom")) != -1) {
		switch (res) {
		case 'i':
			options |= INODE; 
			break;
		case 's': 
			options |= SIZE; 
			break;
		case 'o': 
			options |= ID; 
			break;
		case 'm': 
			options |= MODIFIED; 
			break;
		default: 
			break;
		}
	}
	proc_entry(PATH, 0);
}
void proc_entry(char* buf, int pos)
{
	struct stat fbuf;
	int res = stat(buf, &fbuf);
	if (res == -1) {
		printf("%s", strerror(errno));
		return;
	}
	if (S_ISDIR(fbuf.st_mode)) {
		level++;
		read_dir_req(buf, pos);	
		level--;
	}
	if (S_ISREG(fbuf.st_mode))
		proc_file(buf, pos);
}
void read_dir_req(char* buf, int pos)
{
	DIR* dir = opendir(buf);
	struct dirent* tmp;
	proc_dir(buf, pos);
	while ((tmp = readdir(dir)) != NULL)
	{
		if (strcmp(tmp->d_name, ".") == 0 ||
			 strcmp(tmp->d_name, "..")==0)
			continue;
		int len = strlen(buf);
		strcat(buf, "/");
		pos = strlen(buf);
		strcat(buf, tmp->d_name);
		proc_entry(buf, pos);
		buf[len] = 0;
	}
	closedir(dir);
}
void proc_dir(char* buf, int pos)
{
	static char tmp[MAX_OUTPUT];
	memset(tmp, 0, MAX_OUTPUT);
	strcpy(tmp, buf+pos);
	strcat(tmp, "/");
	make_output(tmp);
}
void proc_file(char* buf, int pos)
{
	char tmp[MAX_OUTPUT];
	static char output[MAX_OUTPUT];
	memset(output, 0, MAX_OUTPUT);
	char file_name[MAX_LEN];
	strcpy(file_name, buf+pos);
	strcat(output, file_name);	
	struct stat fbuf;
	stat(buf, &fbuf);
	if (options & INODE) {
		sprintf(tmp, " INODE = %lu", fbuf.st_ino);
		strcat(output, tmp);
	}
	if (options & SIZE) {
		sprintf(tmp, " SIZE = %ld", fbuf.st_size);
		strcat(output, tmp);
	}
	if (options & ID) {
		sprintf(tmp, " UID=%d GID=%d", fbuf.st_uid, fbuf.st_gid);
		strcat(output, tmp);
	}
	if (options & MODIFIED) {
		struct tm *local;
		time_t t = fbuf.st_mtime;
		local = localtime(&t);
		sprintf(tmp, " MTIME = %s", asctime(local));
		strcat(output, tmp);
	}
	make_output(output);
}		
void make_output(char* buffer)
{
	static char tmp[MAX_OUTPUT];
	memset(tmp, 0, MAX_OUTPUT);
	strcpy(tmp, buffer);
	memmove(tmp+level, tmp, MAX_OUTPUT);
	memset(tmp, '\t', level);
	printf("%s\n", tmp);
}

