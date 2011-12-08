#include "walker.h"

int main(int argc, char* argv[])
{
	struct stat fbuf;
	char path[PATH_MAX];
	int res = -1;

	if (argc > 1) {
		strcpy(path, argv[1]);
		res = stat(argv[1], &fbuf); 
	}

	if (res == -1) {
		 perror(NULL);
	}

	while ((res = getopt(argc, argv, "ispom")) != -1) {
		switch (res) {
		case 'i':
			options |= OPT_INODE; 
			break;
		case 's': 
			options |= OPT_SIZE; 
			break;
		case 'o': 
			options |= OPT_ID; 
			break;
		case 'm': 
			options |= OPT_MTIME; 
			break;
		default: 
			break;
		}
	}

	read_dir_rec(path, 0, 0);
}
void read_dir_rec(char* buf, int indent, int pos)
{	
	DIR* dir = opendir(buf);
	struct dirent* tmp;
	struct stat fbuf;
	int res;

	if (dir == NULL) {
		perror(buf);
		return;
	}

	print_info(print_dir(buf, pos + 1), indent);

	while ((tmp = readdir(dir)) != NULL)
	{

		if (strcmp(tmp->d_name, ".") == 0 ||
			 strcmp(tmp->d_name, "..")==0)
			continue;
		if (strcmp(tmp->d_name, "license_uri") == 0)
			res = 0;

		pos = strlen(buf);
		strcat(buf, "/");
		strcat(buf, tmp->d_name);
		res = stat(buf, &fbuf);	
		if (res == -1) return;			

		if (S_ISDIR(fbuf.st_mode))
			read_dir_rec(buf, indent + 1, pos); 
		if (S_ISREG(fbuf.st_mode))
			print_info(print_file(fbuf, buf, pos + 1), indent);

		buf[pos] = 0;
	}

	closedir(dir);
}

char* print_dir(char* buf, int pos)
{
	static char tmp[MAX_LEN];
	tmp[0] = 0;
	strcpy(tmp, buf+pos);
	strcat(tmp, "/");
	return tmp;
}
char* print_file(struct stat fbuf, char* buf, int start)
{
	static char output[MAX_OUTPUT];
	static char tmp[MAX_LEN];
	strcpy(output, buf + start);

	if (options & OPT_INODE) {
		sprintf(tmp, " INODE = %lu", fbuf.st_ino);
		strcat(output, tmp);
	}

	if (options & OPT_SIZE) {
		sprintf(tmp, " SIZE = %ld", fbuf.st_size);
		strcat(output, tmp);
	}

	if (options & OPT_ID) {
		sprintf(tmp, " UID=%d GID=%d", fbuf.st_uid, fbuf.st_gid);
		strcat(output, tmp);
	}

	if (options & OPT_MTIME) {
		struct tm *local;
		time_t t = fbuf.st_mtime;
		local = localtime(&t);
		sprintf(tmp, " MTIME = %s", asctime(local));
		strcat(output, tmp);
	}

	return output;
}		
void print_info (char* buffer, int indent)
{
	int i = 0;
	for (i = 0; i < indent; i++)
		printf("%s", "\t");
	printf("%s\n", buffer);
}
