#pragma once

typedef union {
	char buf[12];
	struct {
		unsigned short empty;
		unsigned char version[3];
		unsigned char v1;
		unsigned char v2;
		unsigned char flags;
		unsigned int size;
	}data;

} Tagheader;

typedef union {
	char buf[10];
	struct {
		unsigned char name[4];
		unsigned int size;
		unsigned short flags;
	}data;
} Frameheader;