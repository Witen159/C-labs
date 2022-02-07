#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include "commands.h"
char* file_Name;
char* frame_Name;
char* value;
int main(int argc, char* argv[]) {
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--show") == 0) {
			show(file_Name);
			break;
		}
		if (argv[i][2] == 'f') {
			file_Name = strpbrk(argv[i], "=") + 1;
			continue;
		}
		if (argv[i][2] == 'g') {
			frame_Name = strpbrk(argv[i], "=") + 1;
			get(file_Name, frame_Name);
			break;
		}
		if (argv[i][2] == 's') {
			frame_Name = strpbrk(argv[i], "=") + 1;
			continue;
		}
		if (argv[i][2] == 'v') {
			value = strpbrk(argv[i], "=") + 1;
			set(file_Name, frame_Name, value);
			break;
		}
	}
	return 0;
}