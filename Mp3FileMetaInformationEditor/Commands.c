#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "commands.h"
#include "header.h"
const int N = 10;

//Вспомогательные функции

void recording(FILE* input, FILE* output, unsigned int from, unsigned int to) {
	for (unsigned int i = from; i < to; i++) {
		int c;
		c = getc(input);
		putc(c, output);
	}
}

void copy(FILE* input, FILE* output) {
	int c;
	while ((c = getc(input)) != EOF)
		putc(c, output);
}

unsigned int reverse(unsigned int number) {
	return (number >> 24) | ((number >> 8) & 0x0000ff00) | ((number << 8) & 0x00ff0000) |
		((number << 24) & 0xff000000);
}

void control(FILE* file) {
	if (file == NULL) {
		printf("The mp3 doesn't exist\n");
		exit(1);
	}
}

void value_Output(unsigned char* value, unsigned int size) {
	for (unsigned int i = 0; i < size; i++) {
		printf("%c", value[i]);

	}
	printf("\n");
}

//Команды

void show(char* file_Name) {
	FILE* mp3;
	mp3 = fopen(file_Name, "rb");

	control(mp3);

	fseek(mp3, 0, SEEK_SET);

	Tagheader tag_Header;

	fread(tag_Header.buf + 2, sizeof(char), N, mp3);
	unsigned int tag_Size = reverse(tag_Header.data.size);
	unsigned int counter = 0;

	printf("%s frames:\n", file_Name);

	while (ftell(mp3) < tag_Size + N) {
		Frameheader frame_Header;
		fread(frame_Header.buf, sizeof(char), N, mp3);

		if (frame_Header.data.name[0] == 0)
			break;

		counter++;
		printf("%d) %s - ", counter, frame_Header.data.name);

		unsigned int frame_Size = reverse(frame_Header.data.size);

		unsigned char* value = malloc(sizeof(char) * frame_Size);
		fread(value, sizeof(char), frame_Size, mp3);

		value_Output(value, frame_Size);

		free(value);
	}
	fclose(mp3);
}

void get(char* file_Name, char frame_Name[4]) {
	FILE* mp3;
	mp3 = fopen(file_Name, "rb");

	control(mp3);

	Tagheader tag_Header;
	fread(tag_Header.buf + 2, sizeof(char), N, mp3);

	unsigned int tag_Size = reverse(tag_Header.data.size);

	while (ftell(mp3) < tag_Size + N) {
		Frameheader frame_Header;
		fread(frame_Header.buf, sizeof(char), N, mp3);
		unsigned int frame_Size = reverse(frame_Header.data.size);

		if (strcmp(frame_Header.data.name, frame_Name) == 0) {
			printf("%s %s: ", file_Name, frame_Header.data.name);
			unsigned char* value = malloc(sizeof(char) * frame_Size);

			fread(value, sizeof(char), frame_Size, mp3);
			value_Output(value, frame_Size);
			free(value);
			fclose(mp3);
			return;
		}
		fseek(mp3, frame_Size, SEEK_CUR);
	}
	fclose(mp3);
	printf("No value in %s!", frame_Name);
}

void set(char* file_Name, char frame_Name[4], char* value) {
	FILE* mp3;
	mp3 = fopen(file_Name, "rb");

	control(mp3);

	Tagheader tag_Header;
	fread(tag_Header.buf + 2, sizeof(char), N, mp3);

	unsigned int tag_Size = reverse(tag_Header.data.size);
	unsigned int old_Position = 0;
	unsigned int old_Size = 0;

	while (ftell(mp3) < tag_Size + N) {
		Frameheader frame_Header;
		fread(frame_Header.buf, sizeof(char), N, mp3);
		unsigned int frame_Size = reverse(frame_Header.data.size);

		if (strcmp(frame_Header.data.name, frame_Name) == 0) {
			old_Position = ftell(mp3) - N;
			old_Size = frame_Size;
			break;
		}

		if (frame_Header.data.name[0] == 0) {
			break;
		}

		fseek(mp3, frame_Size, SEEK_CUR);
	}

	unsigned int value_Size = strlen(value);
	unsigned int new_Tag_Size = tag_Size - old_Size + value_Size + N * (old_Position ==
		0);

	if (old_Position == 0) {
		old_Position = ftell(mp3) - N;
	}

	if (value_Size == 0) {
		new_Tag_Size -= N;
	}

	FILE* mp3_Copy;
	mp3_Copy = fopen("temp.mp3", "wb");

	fseek(mp3, 0, SEEK_SET);
	fseek(mp3_Copy, 0, SEEK_SET);

	copy(mp3, mp3_Copy);
	fclose(mp3);
	fclose(mp3_Copy);

	mp3_Copy = fopen("temp.mp3", "rb");
	mp3 = fopen(file_Name, "wb");
	tag_Header.data.size = reverse(new_Tag_Size);

	fwrite(tag_Header.buf + 2, sizeof(char), N, mp3);
	fseek(mp3_Copy, N, SEEK_SET);
	recording(mp3_Copy, mp3, 0, old_Position - N);

	if (value_Size > 0) {
		Frameheader frame_Header;

		for (unsigned int i = 0; i < 4; i++) {
			frame_Header.data.name[i] = frame_Name[i];
		}

		frame_Header.data.size = reverse(value_Size);
		frame_Header.data.flags = 0;

		fwrite(frame_Header.buf, sizeof(char), N, mp3);
	}

	fwrite(value, sizeof(char), value_Size, mp3);
	fseek(mp3_Copy, old_Position + N + old_Size, SEEK_SET);
	recording(mp3_Copy, mp3, ftell(mp3), new_Tag_Size);

	printf("%s %s frame renewed: %s\n", file_Name, frame_Name, value);

	copy(mp3_Copy, mp3);
	fclose(mp3);
	fclose(mp3_Copy);

	remove("temp.mp3");
}