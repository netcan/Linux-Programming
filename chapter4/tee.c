/*************************************************************************
	> File Name: tee.c
	  > Author: Netcan
	  > Blog: http://www.netcan666.com
	  > Mail: 1469709759@qq.com
	  > Created Time: 2016-11-05 六 17:40:56 CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

void usage(const char *bin) {
	printf("%s [-a filename|filename]\n", bin);
	exit(EXIT_FAILURE);
}

void errorHandle(const char *msg) {
	perror(msg);
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
	char *fileName = NULL;
	int flags = O_WRONLY | O_CREAT;
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; // rw-rw-rw-

	int opt;
	while((opt = getopt(argc, argv, "+:a:")) != -1)// 第一个字符为:，表示禁止getopt显示错误信息。+:表示不对argv排序
		switch(opt) {
			case 'a':
				fileName = optarg;
				flags |= O_APPEND; // 追加到末尾
				break;
			case '?':
			case ':': usage(argv[0]); break;
		}

	if(optind < argc) fileName = argv[optind]; // 若没有-a选项

	int fd = -1;
	if(fileName) { // 是否指定文件名
		if((flags & O_APPEND) != O_APPEND) flags |= O_TRUNC; // 如果没有附加，那就清空文件
		fd = open(fileName, flags, mode);
		if(fd == -1) errorHandle("open");
	}

	char buffer[1024];
	ssize_t numRead = -1;
	while(numRead != 0) { // <C-D>
		numRead = read(STDIN_FILENO, buffer, sizeof(buffer));
		if(numRead == -1) errorHandle("read");
		ssize_t numWrite = write(STDOUT_FILENO, buffer, numRead);
		if(numWrite == -1) errorHandle("write");
		if(fd != -1) {
			ssize_t numWrite = write(fd, buffer, numRead);
			if(numWrite == -1) errorHandle("write");
		}
	}
	if(fd != -1 && close(fd) == -1) errorHandle("close");

	return 0;
}

