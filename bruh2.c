#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<unistd.h>
#include<string.h>
static inline const unsigned int crc32(const unsigned char* buf, unsigned int si){
	unsigned int crc = 0xffffffff;
	while(si--){
		crc ^= *buf++;
		for(unsigned int i = 0; i < 8; i++){
			if(crc & 1){crc = (crc >> 1) ^ 0xedb88320; continue;}
			crc = (crc >> 1);
		}
	}
	return crc ^ 0xffffffff;
}
static inline unsigned int startsWith(const char* a, const char* b, const unsigned int a_si, const unsigned int b_si){
	for(unsigned int i = 0; i < a_si && i < b_si; i++){
		if(a[i] != b[i]){return 0;}
	}
	return 1;
}
int main(int argc, char** argv){
	if(argc < 2){printf("[ERR][DEBUG][%s:%p] not enough arguments\n", __FUNCTION__, main); return -1;}
	const char* path = 0;
	for(unsigned int i = 0; i < argc; i++){
		unsigned int len = strlen(argv[i]);
		if(!path && startsWith(argv[i], "--file=", len, sizeof("--file=") - 1)){
			if((len - (sizeof("--file=") - 1)) <= 0){
				printf("[ERR][DEBUG][%s:%p] Incorrect argument\n", __FUNCTION__, main);
				return -1;
			}
			path = &argv[i][sizeof("--file=") - 1];
			continue;
		}
	}
	if(!path){
		printf("[ERR][DEBUG][%s:%p] the argument of \"--file=\" isn't set\n", __FUNCTION__, main);
		return -1;
	}
	FILE* file = fopen(path, "rb");
	if(!file){printf("[ERR][DEBUG][%s:%p] cannot open file:[%s], err=[%s]\n", __FUNCTION__, main, path, strerror(errno)); return -1;}
	fseek(file, 0, SEEK_END);
	unsigned long size_file = ftell(file);
	fseek(file, 0, SEEK_SET);
	for(unsigned long offset = 0; offset < size_file;){
		char buf[4096];
		int ret = fread(buf, 1, 4096, file);
		if(ret < 0){printf("[ERR][DEBUG][%s:%p] Bruh, err=[%s]\n", __FUNCTION__, main, strerror(errno)); fclose(file); return -1;}
		const unsigned int crc = crc32(buf, (unsigned int)ret);
		printf("[INF][DEBUG][%s:%p] crc32:[%x], offset: [%lu], size: [%d]\n", __FUNCTION__, main, crc, offset, ret);
		offset += ret;
	}
	fclose(file);
	return 0;
}
