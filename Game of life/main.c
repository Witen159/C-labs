#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Header.h"

struct BITMAPFILEHEADER read_BITMAPFILEHEADER(FILE* F);
struct BITMAPINFOHEADER read_BITMAPINFOHEADER(FILE* F);
struct RGBQUAD** pixelArray(FILE* F, struct BITMAPINFOHEADER infoHeader);
void create_bmp(FILE* F, struct BITMAPFILEHEADER header, struct BITMAPINFOHEADER
	infoHeader, struct RGBQUAD** pixels);
struct RGBQUAD** conv_to_pixels(int** in, struct BITMAPINFOHEADER infoHeader);
int Check_black(struct RGBQUAD pixel);
int** read_files_pixels(struct RGBQUAD** pixels, struct BITMAPINFOHEADER infoHeader);

char* input_file_name;
char* output_directory = "";

int main(int argc, char* argv[]) {

	int max_iter = 100, dump_freq = 1;
	int _input = 0, _output = 0;

	//Обработка аргументов командной строки

	for (int i = 1; i < argc; i += 2) {
		if (strcmp(argv[i], "--input") == 0) {
			input_file_name = argv[i + 1];
			_input = 1;
		}

		if (strcmp(argv[i], "--output") == 0) {
			output_directory = argv[i + 1];
			_output = 1;
		}

		if (strcmp(argv[i], "--max_iter") == 0) {
			sscanf(argv[i + 1], "%d", &max_iter);
		}

		if (strcmp(argv[i], "--dump_freq") == 0) {
			sscanf(argv[i + 1], "%d", &dump_freq);
		}
	}

	//Открытие bmp

	FILE* input_file = fopen(input_file_name, "r");

	if (input_file == NULL) {
		printf("Error with reading file");
		return 0;
	}

	//Считывание хедера из bmp

	struct BITMAPFILEHEADER header = read_BITMAPFILEHEADER(input_file);
	struct BITMAPINFOHEADER infoHeader = read_BITMAPINFOHEADER(input_file);
	struct RGBQUAD** pixelscolors = pixelArray(input_file, infoHeader);

	//Считывание пиксилей из bmp

	int** life_pixels = read_files_pixels(pixelscolors, infoHeader);
	int** life_new_pixels = read_files_pixels(pixelscolors, infoHeader);
	fclose(input_file);

	int up, left, down, right, counter;
	char s[50];
	char way[100];

	//Игра жизнь

	for (int k = 0; k < max_iter; k++) {
		for (int i = 0; i < infoHeader.biHeight; i++) {
			up = i - 1;
			down = i + 1;

			if (i == 0)
				up = infoHeader.biHeight - 1;
			if (i == infoHeader.biHeight - 1)
				down = 0;

			for (int j = 0; j < infoHeader.biWidth; j++) {
				left = j - 1;
				right = j + 1;

				if (j == 0)
					left = infoHeader.biWidth - 1;
				if (j == infoHeader.biWidth - 1)
					right = 0;

				counter = life_pixels[up][left] + life_pixels[up][j] +
					life_pixels[up][right] + life_pixels[i][left] + life_pixels[i][right] +
					life_pixels[down][left] + life_pixels[down][j] + life_pixels[down][right];

				if (life_pixels[i][j] == 1) {
					if (counter < 2 || counter > 3)
						life_new_pixels[i][j] = 0;
					else
						life_new_pixels[i][j] = 1;
				}
				else {
					if (counter == 3)
						life_new_pixels[i][j] = 1;
					else
						life_new_pixels[i][j] = 0;
				}
			}
		}

		for (int i = 0; i < infoHeader.biHeight; i++) {
			for (int j = 0; j < infoHeader.biWidth; j++) {
				life_pixels[i][j] = life_new_pixels[i][j];
			}
		}

		//Создание bmp новых поколений

		if (k % dump_freq == 0) {
			memset(s, 0, 50);
			memset(way, 0, 100);

			sprintf(s, "%d.bmp", k);
			strcpy(way, output_directory);
			strcat(way, s);

			input_file = fopen(way, "w");
			create_bmp(input_file, header, infoHeader, conv_to_pixels(life_pixels,
				infoHeader));
			fclose(input_file);
		}

	}
	return 0;
}

struct BITMAPFILEHEADER read_BITMAPFILEHEADER(FILE* F) {
	struct BITMAPFILEHEADER header;

	fread(&header.bfType, 1, 2, F);
	fread(&header.bfSize, 4, 1, F);
	fread(&header.bfReserved1, 2, 1, F);
	fread(&header.bfReserved2, 2, 1, F);
	fread(&header.bfOffBits, 4, 1, F);

	return header;
}

struct BITMAPINFOHEADER read_BITMAPINFOHEADER(FILE* F) {
	struct BITMAPINFOHEADER infoHeader;

	fread(&infoHeader.biSize, 4, 1, F);
	fread(&infoHeader.biWidth, 4, 1, F);
	fread(&infoHeader.biHeight, 4, 1, F);
	fread(&infoHeader.biPlanes, 2, 1, F);
	fread(&infoHeader.biBitCount, 2, 1, F);
	fread(&infoHeader.biCompression, 4, 1, F);
	fread(&infoHeader.biSizeImage, 4, 1, F);
	fread(&infoHeader.biXPelsPerMeter, 4, 1, F);
	fread(&infoHeader.biYPelsPerMeter, 4, 1, F);
	fread(&infoHeader.biClrUsed, 4, 1, F);
	fread(&infoHeader.biClrImportant, 4, 1, F);

	return infoHeader;
}

struct RGBQUAD** pixelArray(FILE* F, struct BITMAPINFOHEADER infoHeader) {
	struct RGBQUAD** pixels = (struct RGBQUAD**)malloc(infoHeader.biHeight *
		sizeof(struct RGBQUAD*));

	for (int i = 0; i < infoHeader.biHeight; ++i)
		pixels[i] = (struct RGBQUAD*)malloc(infoHeader.biWidth * sizeof(struct RGBQUAD));

	for (int i = infoHeader.biHeight - 1; i >= 0; i--) {
		for (int j = 0; j < infoHeader.biWidth; j++) {
			pixels[i][j].rgbBlue = getc(F);
			pixels[i][j].rgbGreen = getc(F);
			pixels[i][j].rgbRed = getc(F);
		}

		if (infoHeader.biWidth % 4 != 0) {
			getc(F);
		}
	}

	return pixels;
}

void create_bmp(FILE* F, struct BITMAPFILEHEADER header, struct BITMAPINFOHEADER
	infoHeader, struct RGBQUAD** pixels) {
	fwrite(&header.bfType, 1, 2, F);
	fwrite(&header.bfSize, 4, 1, F);
	fwrite(&header.bfReserved1, 2, 1, F);
	fwrite(&header.bfReserved2, 2, 1, F);
	fwrite(&header.bfOffBits, 4, 1, F);
	fwrite(&infoHeader.biSize, 4, 1, F);
	fwrite(&infoHeader.biWidth, 4, 1, F);
	fwrite(&infoHeader.biHeight, 4, 1, F);
	fwrite(&infoHeader.biPlanes, 2, 1, F);
	fwrite(&infoHeader.biBitCount, 2, 1, F);
	fwrite(&infoHeader.biCompression, 4, 1, F);
	fwrite(&infoHeader.biSizeImage, 4, 1, F);
	fwrite(&infoHeader.biXPelsPerMeter, 4, 1, F);
	fwrite(&infoHeader.biYPelsPerMeter, 4, 1, F);
	fwrite(&infoHeader.biClrUsed, 4, 1, F);
	fwrite(&infoHeader.biClrImportant, 4, 1, F);

	for (int i = infoHeader.biHeight - 1; i >= 0; i--) {
		for (int j = 0; j < infoHeader.biWidth; j++) {
			putc(pixels[i][j].rgbBlue, F);
			putc(pixels[i][j].rgbGreen, F);
			putc(pixels[i][j].rgbRed, F);
		}

		if (infoHeader.biWidth % 4 != 0) {
			putc(0, F);
		}
	}
}

struct RGBQUAD** conv_to_pixels(int** in, struct BITMAPINFOHEADER infoHeader) {
	struct RGBQUAD** pixels = (struct RGBQUAD**)malloc(infoHeader.biHeight *
		sizeof(struct RGBQUAD*));

	for (int i = 0; i < infoHeader.biHeight; ++i)
		pixels[i] = (struct RGBQUAD*)malloc(infoHeader.biWidth * sizeof(struct RGBQUAD));

	for (int i = infoHeader.biHeight - 1; i >= 0; i--) {
		for (int j = 0; j < infoHeader.biWidth; j++) {
			if (in[i][j] == 1) {
				pixels[i][j].rgbBlue = 0;
				pixels[i][j].rgbGreen = 0;
				pixels[i][j].rgbRed = 0;
			}
			else
			{
				pixels[i][j].rgbBlue = 255;
				pixels[i][j].rgbGreen = 255;
				pixels[i][j].rgbRed = 255;
			}
		}
	}

	return pixels;
}

int Check_black(struct RGBQUAD pixel) {
	if (pixel.rgbBlue == 0 && pixel.rgbGreen == 0 && pixel.rgbRed == 0) {
		return 1;
	}
	return 0;
}

int** read_files_pixels(struct RGBQUAD** pixels, struct BITMAPINFOHEADER infoHeader) {
	int** pixels_array = (int**)malloc(infoHeader.biHeight * sizeof(int*));

	for (int i = 0; i < infoHeader.biHeight; ++i)
		pixels_array[i] = (int*)malloc(infoHeader.biWidth * sizeof(int));

	for (int i = 0; i < infoHeader.biHeight; i++) {
		for (int j = 0; j < infoHeader.biWidth; j++) {
			pixels_array[i][j] = (Check_black(pixels[i][j]) == 1 ? 1 : 0);
		}
	}

	return pixels_array;
}