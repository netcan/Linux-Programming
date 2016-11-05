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

#ifndef BUF_SIZE // cc -D重定义
#define BUF_SIZE 1024
#endif

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

	char buffer[BUF_SIZE];
	ssize_t numRead = -1;
	while((numRead = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0) { // <C-D>
		if(write(STDOUT_FILENO, buffer, numRead) != numRead) errorHandle("couldn't write whole buffer");
		if(fd != -1)
			if(write(fd, buffer, numRead) != numRead) errorHandle("couldn't write whole buffer");
	}
	if(numRead == -1) errorHandle("read");
	if(fd != -1 && close(fd) == -1) errorHandle("close");

	return 0;
}

