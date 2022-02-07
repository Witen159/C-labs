#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include "commands.h"

//Вспомогательные функции

void list_output(FILE* archiv, int counter) {
	char file_name[128];

	for (int i = 0; i < counter; i++) {
		fscanf(archiv, "%s", file_name);
		fscanf(archiv, "%s", file_name);

		printf("%d) %s\n", i + 1, file_name);

		fscanf(archiv, "%s", file_name);
		fscanf(archiv, "%s", file_name);
	}
}

int number_of_files(FILE* archiv) {
	int c, counter = 0, buf_count = 3;

	while ((c = getc(archiv)) != EOF) {
		if (c == ']') {
			counter++;
			buf_count = 0;
		}

		if (buf_count == 2 && c != '[')
			break;

		buf_count++;
	}
	return counter;
}

unsigned long return_file_size(char* file_name) {
	FILE* file;
	unsigned long file_size;

	file = fopen(file_name, "rb");

	fseek(file, 0, SEEK_END);
	file_size = ftell(file);

	fseek(file, 0, SEEK_SET);
	fclose(file);

	return file_size;
}

void archiving(FILE* archive, int argc, char* argv[]) {
	FILE* file;
	int temp;

	for (int i = 4; i < argc; i++) {
		file = fopen(argv[i], "rb");

		while ((temp = getc(file)) != EOF)
			putc(temp, archive);

		fclose(file);
	}
}

unsigned long return_reading_position(FILE* archive) {
	int c;
	unsigned long reading_position = 0;

	while ((c = getc(archive)) != EOF) {
		reading_position++;

		if (c == '\n')
			break;
	}

	return reading_position;
}

write_file(FILE* archiv, unsigned long size, char* name) {
	FILE* file;
	file = fopen(name, "wb");

	int c;
	while ((size--) > 0)
		putc((c = getc(archiv)), file);

	fclose(file);
}

void unarchiving(FILE* archiv, unsigned long reading_position) {
	char file_name[128] = { 0 };
	unsigned long file_size;
	unsigned long current_position;

	while (fscanf(archiv, "[ %s %lu ] ", file_name, &file_size) != 0) {
		current_position = ftell(archiv);
		fseek(archiv, reading_position, SEEK_SET);
		reading_position += file_size;
		write_file(archiv, file_size, file_name);
		fseek(archiv, current_position, SEEK_SET);
	}
}

//Команды

void extract(char* archiv_name) {
	FILE* archiv = fopen(archiv_name, "rb+");

	unsigned long reading_position;
	reading_position = return_reading_position(archiv);

	fseek(archiv, 0, SEEK_SET);
	unarchiving(archiv, reading_position);

	fclose(archiv);
	printf("Extraction completed.\n");
}

void list(char* archiv_name) {
	FILE* archiv = fopen(archiv_name, "rb");
	int counter = number_of_files(archiv);

	fclose(archiv);
	archiv = fopen(archiv_name, "rb");
	list_output(archiv, counter);
	fclose(archiv);
}

void create(char* archiv_name, int argc, char* argv[]) {
	FILE* archiv = fopen(archiv_name, "wb");
	unsigned long file_size[128];

	for (int i = 4; i < argc; i++) {
		file_size[i - 4] = return_file_size(argv[i]);
	}

	for (int i = 0; i < argc - 4; i++)
		fprintf(archiv, "[ %s %lu ] ", argv[i + 4], file_size[i]);

	fprintf(archiv, "\n");
	archiving(archiv, argc, argv);
	fclose(archiv);

	printf("Archiving is done.\n");
}