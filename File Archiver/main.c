#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include "commands.h"

char* archiv_name;

int main(int argc, char* argv[]) {

	for (int i = 0; i < argc; i++) {
		if (strcmp("--file", argv[i]) == 0) {
			archiv_name = argv[i + 1];
		}

		if (strcmp("--create", argv[i]) == 0) {
			create(archiv_name, argc, argv);
			break;
		}

		if (strcmp("--extract", argv[i]) == 0) {
			extract(archiv_name);
		}

		if (strcmp("--list", argv[i]) == 0) {
			list(archiv_name);
		}
	}
	return 0;
}