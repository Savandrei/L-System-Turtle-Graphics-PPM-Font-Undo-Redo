// Savlovschi Andrei-Bogdan 311CB
#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define STR_SIZE 100
#define PI 3.14159265358979323846

typedef struct {
	int *nr_rules;
	int *loaded;
	char *axiom;
	char *symbols;
	char *file_location;
	char **succesor;
} LSYSTEM;

typedef struct {
	int **pixels;
	int *width, *height;
	char *file_location;
} PPM;

typedef struct {
	double x, y;
	double orientation;
	int used;
} TURTLE_STATE;

typedef struct {
	char *name;
	char *file_location;
	unsigned char **bitmap;
	int *encoding;
	int *nr_chars;
	int *dwx;
	int *dwy;
	int *bbw;
	int *bbh;
	int *bbxoff;
	int *bbyoff;
	int *loaded;
} FONT;

char *allocate_string(int size)
{
	char *str = calloc(size, 1);
	if (!str) {
		perror("Allocation failed");
		free(str);
		exit(77);
	}
	return str;
}

char *reallocate_string(char *str, int new_size)
{
	char *re_str = realloc(str, new_size);
	if (!re_str) {
		perror("Reallocation failed");
		free(re_str);
		exit(77);
	}
	return re_str;
}

char **alloc_str_arr(int size, int number)
{
	char **str_arr = malloc(number * sizeof(char *));
	if (!str_arr) {
		perror("Allocation failed");
		free(str_arr);
		exit(77);
	}
	for (int i = 0; i < number; i++) {
		str_arr[i] = calloc(size, 1);
		if (!str_arr[i]) {
			perror("Allocation failed");
			for (int j = 0; j <= i; j++) {
				free(str_arr[j]);
			}
			free(str_arr);
			exit(77);
		}
	}
	return str_arr;
}

int **allocate_pixel_arr(int width, int height)
{
	int **pixel_arr = malloc(height * sizeof(int *));
	if (!pixel_arr) {
		perror("Allocation failed");
		free(pixel_arr);
		exit(77);
	}
	for (int i = 0; i < height; i++) {
		pixel_arr[i] = calloc(width, sizeof(int));
		if (!pixel_arr[i]) {
			for (int j = 0; j <= i; j++) {
				perror("Allocation failed");
				free(pixel_arr[j]);
				exit(77);
			}
		}
	}
	return pixel_arr;
}

LSYSTEM allocate_lsys(void)
{
	LSYSTEM lsys;
	lsys.nr_rules = calloc(1, sizeof(int));
	lsys.loaded = calloc(1, sizeof(int));
	if (!lsys.nr_rules || !lsys.loaded) {
		perror("Allocation failed");
		free(lsys.nr_rules);
		free(lsys.loaded);
		exit(77);
	}
	lsys.axiom = allocate_string(STR_SIZE);
	lsys.symbols = allocate_string(STR_SIZE);
	lsys.file_location = allocate_string(STR_SIZE);
	lsys.succesor = alloc_str_arr(STR_SIZE, STR_SIZE);
	return lsys;
}

PPM allocate_ppm(void)
{
	PPM ppm;
	ppm.height = calloc(1, sizeof(int));
	ppm.width = calloc(1, sizeof(int));
	if (!ppm.height || !ppm.width) {
		perror("Allocation failed");
		free(ppm.height);
		free(ppm.width);
		exit(77);
	}
	ppm.file_location = allocate_string(STR_SIZE);
	ppm.pixels = NULL;
	return ppm;
}

int *allocate_int_arr(int size)
{
	int *p = malloc(size * sizeof(int));
	if (!p) {
		perror("Allocation failed");
		free(p);
		exit(77);
	}
	return p;
}

FONT allocate_font(FONT fnt)
{
	fnt.encoding = allocate_int_arr(*fnt.nr_chars);
	fnt.dwx = allocate_int_arr(*fnt.nr_chars);
	fnt.dwy = allocate_int_arr(*fnt.nr_chars);
	fnt.bbh = allocate_int_arr(*fnt.nr_chars);
	fnt.bbw = allocate_int_arr(*fnt.nr_chars);
	fnt.bbxoff = allocate_int_arr(*fnt.nr_chars);
	fnt.bbyoff = allocate_int_arr(*fnt.nr_chars);
	fnt.bitmap = malloc(*fnt.nr_chars * sizeof(unsigned char *));
	fnt.loaded = calloc(1, sizeof(int));
	if (!fnt.loaded) {
		perror("Allocation failed");
		free(fnt.loaded);
		exit(77);
	}
	return fnt;
}

void free_2d_char_arr(char **arr, int nr_lines)
{
	for (int i = 0; i < nr_lines; i++) {
		free(arr[i]);
	}
	free(arr);
}

void free_2d_uchar_arr(unsigned char **arr, int nr_lines)
{
	for (int i = 0; i < nr_lines; i++) {
		free(arr[i]);
	}
	free(arr);
}

int **free_2d_int_arr(int **arr, int nr_lines)
{
	if (arr) {
		for (int i = 0; i < nr_lines; i++) {
			free(arr[i]);
		}
		free(arr);
	}
	arr = NULL;
	return arr;
}

void free_lsys(LSYSTEM lsys)
{
	free(lsys.nr_rules);
	free(lsys.loaded);
	free(lsys.axiom);
	free(lsys.symbols);
	free(lsys.file_location);
	free_2d_char_arr(lsys.succesor, STR_SIZE);
}

void free_ppm(PPM ppm)
{
	if (ppm.pixels)
		free_2d_int_arr(ppm.pixels, *ppm.height);
	free(ppm.width);
	free(ppm.height);
	free(ppm.file_location);
}

FONT free_font(FONT fnt)
{
	if (fnt.nr_chars) {
		free_2d_uchar_arr(fnt.bitmap, *fnt.nr_chars);
		free(fnt.bbh);
		free(fnt.bbw);
		free(fnt.bbxoff);
		free(fnt.bbyoff);
		free(fnt.dwx);
		free(fnt.dwy);
		free(fnt.encoding);
		free(fnt.name);
		free(fnt.loaded);
	}
	free(fnt.nr_chars);
	fnt.nr_chars = NULL;
	return fnt;
}

int rgb_to_colour(int red, int green, int blue)
{
	int colour = 0;
	colour += red * 256 * 256;
	colour += green * 256;
	colour += blue;
	return colour;
}

int is_symbol(LSYSTEM lsys, char character)
{
	for (int i = 0; i < *lsys.nr_rules; i++) {
		if (lsys.symbols[i] == character)
			return i;
	}
	return -1;
}

// Stores undoable commands
// '?' - shows last command
// '-' - shows last command that can be undone
// '+' - shows command that can be redo'd
// 'a' - allocates the char array
// 'f' - free's the char array
// saves any other symbol (CAPITAL LETTERS)
char last_cmd(char action)
{
	static char *last_command;

	static int counter = 1, size = 100;
	int undo_counter, redo_counter, i;
	if (action == 'a') {
		last_command = allocate_string(size);
		return 0;
	}
	if (action == 'f') {
		free(last_command);
		return 0;
	}
	if (action == '?') {
		return last_command[counter - 1];
	}
	if (action == '-') {
		i = 1;
		do {
			undo_counter = 1;
			while (undo_counter) {
				if (last_command[counter - i] == 'U')
					undo_counter += 2;
				undo_counter--;
				i++;
			}
		} while (last_command[counter - i + 1] == 'R');
		if (counter - i + 1 >= 0)
			return last_command[counter - i + 1];
		else
			return 0;
	}
	if (action == '+') {
		i = 1;
		redo_counter = 1;
		undo_counter = 0;
		while (redo_counter != undo_counter) {
			if (last_command[counter - i] == 'U')
				undo_counter++;
			else if (last_command[counter - i] == 'R')
				redo_counter++;
			else if (last_command[counter - i] == 'L')
				return 0;
			else if (last_command[counter - i] == 'Y')
				return 0;
			i++;
		}
		do {
			undo_counter = 1;
			while (undo_counter) {
				if (last_command[counter - i] == 'U')
					undo_counter += 2;
				undo_counter--;
				i++;
			}
		} while (last_command[counter - i + 1] == 'R');
		if (counter - i + 1 >= 0)
			return last_command[counter - i + 1];
		else
			return 0;
	}
	last_command[counter] = action;
	counter++;
	if (counter >= size) {
		size *= 2;
		last_command = reallocate_string(last_command, size);
	}
	return 0;
}

// possible actions:
// a - allocate
// s - save location
// u - undo
// r - redo
// f - free
void lsys_file_location_stack(LSYSTEM lsys, char action)
{
	static char **file_location_stack;
	static int counter = -1;
	if (action == 'a') {
		file_location_stack = alloc_str_arr(STR_SIZE, STR_SIZE);

	// save last location
	} else if (action == 's') {
		counter++;
		strcpy(file_location_stack[counter], lsys.file_location);

	// replace current location with
	// the one before it
	} else if (action == 'u') {
		if (counter >= 0)
			counter--;
		if (counter == -1)
			lsys.file_location[0] = '\0';
		else
			strcpy(lsys.file_location, file_location_stack[counter]);

	} else if (action == 'r') {
		counter++;
		strcpy(lsys.file_location, file_location_stack[counter]);

	} else if (action == 'f') {
		if (file_location_stack) {
			free_2d_char_arr(file_location_stack, STR_SIZE);
			file_location_stack = NULL;
		}
		counter = -1;
	}

}

// mode:
// 'n' - normal
// 'u' - undo
int lsystem(LSYSTEM lsys, char mode)
{
	// undo-related
	if (lsys.file_location[0] == '\0') {
		*lsys.loaded = 0;
		return 0;
	}

	FILE *doc = fopen(lsys.file_location, "rt");
	if (!doc) {
		printf("Failed to load %s\n", lsys.file_location);
		*lsys.loaded = 0;
		last_cmd('L');
		return -1;
	}
	*lsys.loaded = 1;
	char endl_remover;
	fscanf(doc, "%s", lsys.axiom);
	fscanf(doc, "%d", lsys.nr_rules);
	for (int i = 0; i < *lsys.nr_rules; i++) {
		fscanf(doc, "%c", &endl_remover);
		fscanf(doc, "%c%s", &lsys.symbols[i], lsys.succesor[i]);
	}
	if (mode != 'u') {
		lsys_file_location_stack(lsys, 's');
		printf("Loaded %s ", lsys.file_location);
		printf("(L-system with %d rules)\n", *lsys.nr_rules);
	}

	last_cmd('L');
	fclose(doc);
	return 1;
}

char *derive(LSYSTEM lsys, int order)
{
	if (!(*lsys.loaded)) {
		printf("No L-system loaded\n");
		return NULL;
	}

	int memmory = STR_SIZE;
	char *starting = allocate_string(memmory);
	char *derived = allocate_string(memmory);
	strcpy(starting, lsys.axiom);
	strcpy(derived, lsys.axiom);
	if (order == 0) {
		free(starting);
		return derived;
	}
	for (int j = 0; j < order; j++) {
		int len = strlen(starting);
		int index = 0, succ_len;
		for (int i = 0; i < len; i++) {
			int symbol_line = is_symbol(lsys, starting[i]);
			if (symbol_line >= 0) {
				succ_len = strlen(lsys.succesor[symbol_line]);
				if (index + succ_len >= memmory) {
					memmory *= 2;
					starting = reallocate_string(starting, memmory);
					derived = reallocate_string(derived, memmory);
				}
				memcpy(&derived[index], lsys.succesor[symbol_line], succ_len);

				index += succ_len;
			} else {
				if (index + 2 >= memmory) {
					memmory *= 2;
					starting = reallocate_string(starting, memmory);
					derived = reallocate_string(derived, memmory);
				}
				derived[index] = starting[i];
				index++;
			}
		}
		derived[index] = '\0';
		strcpy(starting, derived);
	}

	free(starting);
	return derived;
}

int read_dimensions(FILE *image)
{
	int number = 0;
	char digit;
	fread(&digit, sizeof(char), 1, image);

	// endline and spacace characters,
	// written in ascii code
	while ((digit != 32) && (digit != 10)) {
		number = number * 10 + (int)digit - '0';
		fread(&digit, sizeof(char), 1, image);
	}
	return number;
}

void write_dimensions(FILE *ppm, int width, int height)
{
	char digit[20];
	int nr = 0;
	do {
		digit[nr] = (char)(width % 10) + '0';
		width /= 10;
		nr++;
	} while (width);
	for (int i = nr - 1; i >= 0; i--) {
		fwrite(&digit[i], sizeof(char), 1, ppm);
		digit[i] = 0;
	}
	fwrite(" ", sizeof(char), 1, ppm);

	nr = 0;
	do {
		digit[nr] = (char)(height % 10) + '0';
		height /= 10;
		nr++;
	} while (height);
	for (int i = nr - 1; i >= 0; i--) {
		fwrite(&digit[i], sizeof(char), 1, ppm);
	}
	fwrite("\n", sizeof(char), 1, ppm);
}

void write_image(FILE *ppm_file, PPM ppm)
{
	unsigned char red, green, blue;
	for (int i = *ppm.height - 1; i >= 0; i--) {
		for (int j = 0; j < *ppm.width; j++) {
			int temp = ppm.pixels[i][j];
			blue = temp % 256;
			temp /= 256;
			green = temp % 256;
			temp /= 256;
			red = temp % 256;
			fwrite(&red, 1, 1, ppm_file);
			fwrite(&green, 1, 1, ppm_file);
			fwrite(&blue, 1, 1, ppm_file);
		}
	}
}

// pixel = red * 256^2 + green * 256 + blue
void read_pixels(FILE *image, int **pixels, int width, int height)
{
	unsigned char red, green, blue;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			fread(&red, sizeof(char), 1, image);
			fread(&green, sizeof(char), 1, image);
			fread(&blue, sizeof(char), 1, image);
			pixels[i][j] = rgb_to_colour(red, green, blue);
		}
	}
}

void add_state(TURTLE_STATE *S, double x, double y, double orientation)
{
	int i = 0;
	while (S[i].used) {
		i++;
	}

	S[i].orientation = orientation;
	S[i].x = x;
	S[i].y = y;
	S[i].used = 1;
}

TURTLE_STATE get_state(TURTLE_STATE *S)
{
	int i = 0;
	while (S[i].used) {
		i++;
	}
	return S[i - 1];
}

void remove_state(TURTLE_STATE *S)
{
	int i = 0;
	while (S[i].used) {
		i++;
	}
	i--;
	S[i].orientation = 0;
	S[i].x = 0;
	S[i].y = 0;
	S[i].used = 0;
}

void draw_line(PPM ppm, int colour, int x0, int y0, int x1, int y1)
{
	int dx = abs(x1 - x0);
	int dy = -abs(y1 - y0);
	int sx, sy;
	if (x0 < x1)
		sx = 1;
	else
		sx = -1;
	if (y0 < y1)
		sy = 1;
	else
		sy = -1;
	int err = dx + dy;

	while (1) {
		if (x0 >= 0 && x0 < *ppm.width && y0 >= 0 && y0 < *ppm.height) {
			ppm.pixels[y0][x0] = colour;
		}

		if (x0 == x1 && y0 == y1)
			break;

		int e2 = 2 * err;

		if (e2 >= dy) {
			err += dy;
			x0 += sx;
		}
		if (e2 <= dx) {
			err += dx;
			y0 += sy;
		}
	}
}

PPM turtle(PPM ppm, LSYSTEM lsys)
{
	last_cmd('T');
	double x, y;
	double distance, orienation, angular_step;
	int derive_nr;
	int red, green, blue;
	scanf("%lf%lf", &x, &y);
	scanf("%lf%lf%lf", &distance, &orienation, &angular_step);
	scanf("%d", &derive_nr);
	scanf("%d%d%d", &red, &green, &blue);
	int colour = rgb_to_colour(red, green, blue);

	if (*ppm.width == 0 || *ppm.height == 0) {
		printf("No image loaded\n");
		return ppm;
	}
	if (*lsys.loaded == 0) {
		printf("No L-system loaded\n");
		return ppm;
	}

	char *commands = derive(lsys, derive_nr);
	int i = 0;
	TURTLE_STATE curr;
	TURTLE_STATE *S = calloc(strlen(commands), sizeof(TURTLE_STATE));

	add_state(S, x, y, orienation);
	while (commands[i]) {
		if (commands[i] == 'F') {
			double rad = orienation * M_PI / 180.0;
			double x1 = x + distance * cos(rad);
			double y1 = y + distance * sin(rad);
			draw_line(ppm, colour, (int)round(x)
			, (int)round(y), (int)round(x1), (int)round(y1));
			x = x1;
			y = y1;

		} else if (commands[i] == '+') {
			orienation += angular_step;

		} else if (commands[i] == '-') {
			orienation -= angular_step;

		} else if (commands[i] == '[') {
			add_state(S, x, y, orienation);

		} else if (commands[i] == ']') {
			curr = get_state(S);
			x = curr.x;
			y = curr.y;
			orienation = curr.orientation;
			remove_state(S);
		}
		i++;
	}
	printf("Drawing done\n");
	free(S);
	free(commands);
	return ppm;
}

// makes a copy of a PPM struct into another
PPM copy_ppm(PPM copy, PPM ppm)
{
	if (copy.pixels && copy.height) {
		copy.pixels = free_2d_int_arr(copy.pixels, *copy.height);
	}
	if (copy.width && copy.height && ppm.width && ppm.height) {
		*copy.width = *ppm.width;
		*copy.height = *ppm.height;
	}
	if (copy.file_location && ppm.file_location)
		strcpy(copy.file_location, ppm.file_location);

	if (ppm.pixels) {
		copy.pixels = allocate_pixel_arr(*ppm.width, *ppm.height);
		for (int i = 0; i < *ppm.height; i++) {
			for (int j = 0; j < *ppm.width; j++) {
				copy.pixels[i][j] = ppm.pixels[i][j];
			}
		}
	}
	return copy;
}

// possible actions:
// a - allocate
// s - save location
// u - undo
// r - redo
// f - free
PPM ppm_file_location_stack(PPM ppm, char action)
{
	static PPM *ppm_stack;
	static int max_counter = -1, counter = -1;
	static int size = 20;
	if (action == 'a') {
		ppm_stack = calloc(size, sizeof(PPM));

	// save last location
	} else if (action == 's') {
		counter++;
		if (counter >= size) {
			size *= 2;
			ppm_stack = realloc(ppm_stack, size * sizeof(PPM));
			if (!ppm_stack) {
				perror("Reallocation failed");
				free(ppm_stack);
				exit(77);
			}
		}
		if (counter > max_counter) {
			max_counter = counter;
		} else
			free_ppm(ppm_stack[counter]);
		ppm_stack[counter] = allocate_ppm();
		ppm_stack[counter] = copy_ppm(ppm_stack[counter], ppm);
	// replace current location with
	// the last one
	} else if (action == 'u') {
		if (counter >= 0)
			counter--;
		if (counter == -1) {
			free_2d_int_arr(ppm.pixels, *ppm.height);
			*ppm.width = 0;
			*ppm.height = 0;
			ppm.pixels = NULL;
		} else {
			ppm = copy_ppm(ppm, ppm_stack[counter]);
		}

	} else if (action == 'r') {
		counter++;
		ppm = copy_ppm(ppm, ppm_stack[counter]);

	} else if (action == 'f') {
		for (int i = 0; i <= max_counter; i++) {
			free_ppm(ppm_stack[i]);
		}
		free(ppm_stack);
		counter = -1;
	}
	return ppm;
}

void save(PPM ppm)
{
	char *file_destination = allocate_string(STR_SIZE);
	scanf("%s", file_destination);

	FILE *ppm_file = fopen(file_destination, "wb");
	if (!ppm_file) {
		perror("IDK how can this even happen");
		// maybe the file already exists with no
		// permisions
		exit(87);
	}

	fwrite("P6\n", sizeof(char), 3, ppm_file);
	write_dimensions(ppm_file, *ppm.width, *ppm.height);
	fwrite("255\n", sizeof(char), 4, ppm_file);

	printf("Saved %s\n", file_destination);
	write_image(ppm_file, ppm);
	free(file_destination);
	fclose(ppm_file);
}

char *scan_font_name(FILE *bdf)
{
	int size = 100;
	char *name = allocate_string(size);

	// removing all text until the font name
	do {
		fscanf(bdf, "%s", name);
	} while (strcmp(name, "FONT"));

	int i = 0;
	fscanf(bdf, "%c", &name[i]); // space
	fscanf(bdf, "%c", &name[i]);
	while (name[i] != '\n') {
		i++;
		if (i >= size) {
			size *= 2;
			name = reallocate_string(name, size);
		}
		fscanf(bdf, "%c", &name[i]);
	}
	name[i] = '\0';

	return name;
}

char scan_hex_to_char(FILE *bdf)
{
	char char1, char2;
	int hex_digit1, hex_digit2, number;
	fscanf(bdf, "%c%c", &char1, &char2);
	if (char1 <= '9')
		hex_digit1 = char1 - '0';
	else
		hex_digit1 = char1 - 'A' + 10;
	if (char2 <= '9')
		hex_digit2 = char2 - '0';
	else
		hex_digit2 = char2 - 'A' + 10;
	number = hex_digit1 * 16 + hex_digit2;
	return (char)number;

}

FONT scan_and_alloc_char_info(FONT fnt, FILE *bdf)
{
	char *useless_info = allocate_string(100);
	do {
		fscanf(bdf, "%s", useless_info);
	} while (strcmp(useless_info, "CHARS"));
	fnt.nr_chars = malloc(sizeof(int));
	fscanf(bdf, "%d", fnt.nr_chars);

	fnt = allocate_font(fnt);

	for (int i = 0; i < *fnt.nr_chars; i++) {
		do {
			fscanf(bdf, "%s", useless_info);
		} while (strcmp(useless_info, "ENCODING"));
		fscanf(bdf, "%d", &fnt.encoding[i]);

		fscanf(bdf, "%s%s", useless_info, useless_info); // "SWIDTH X X"
		fscanf(bdf, "%s%s", useless_info, useless_info); // "DWIDTH"
		fscanf(bdf, "%d%d", &fnt.dwx[i], &fnt.dwy[i]);

		fscanf(bdf, "%s", useless_info); // "BBX"
		fscanf(bdf, "%d%d", &fnt.bbw[i], &fnt.bbh[i]);
		fscanf(bdf, "%d%d", &fnt.bbxoff[i], &fnt.bbyoff[i]);

		fscanf(bdf, "%s", useless_info); //"BITMAP"
		fscanf(bdf, "%c", useless_info); //endl char
		int row_size = fnt.bbw[i] / 8;
		if (fnt.bbw[i] % 8)
			row_size++;
		fnt.bitmap[i] = malloc(row_size * fnt.bbh[i]);
		for (int j = 0; j < fnt.bbh[i]; j++) {
			for (int k = 0; k < row_size; k++) {
				fnt.bitmap[i][j * row_size + k] = scan_hex_to_char(bdf);
			}
			fscanf(bdf, "%c", useless_info); //endl char
		}
	}

	free(useless_info);
	return fnt;
}

// possible actions:
// a - allocate
// s - save location
// u - undo
// r - redo
// f - free
FONT font_file_location_stack(FONT fnt, char action)
{
	static char **file_location_stack;
	static int counter = -2;
	if (action == 'a') {
		file_location_stack = alloc_str_arr(STR_SIZE, STR_SIZE);

	// save last location
	} else if (action == 's') {
		if (counter == -2) {
			counter = 0;
			strcpy(file_location_stack[counter], fnt.file_location);
		}
		counter++;
		strcpy(file_location_stack[counter], fnt.file_location);

	// replace current location with
	// the last one
	} else if (action == 'u') {
		if (counter >= 0)
			counter--;
		if (counter == -1)
			fnt.file_location[0] = '\0';
		else
			strcpy(fnt.file_location, file_location_stack[counter]);

	} else if (action == 'r') {
		counter++;
		strcpy(fnt.file_location, file_location_stack[counter]);

	} else if (action == 'f')
		free_2d_char_arr(file_location_stack, STR_SIZE);

	return fnt;
}

FONT font(FONT fnt)
{
	if (fnt.file_location[0] == '\0') {
		*fnt.loaded = 0;
		return fnt;
	}

	FILE *bdf = fopen(fnt.file_location, "rt");
	if (!bdf) {
		printf("Failed to load %s\n", fnt.file_location);
		*fnt.loaded = 0;
		return fnt;
	}

	fnt.name = scan_font_name(bdf);
	fnt = scan_and_alloc_char_info(fnt, bdf);

	if (last_cmd('?') != 'U') {
		printf("Loaded %s ", fnt.file_location);
		printf("(bitmap font %s)\n", fnt.name);
		last_cmd('F');
		font_file_location_stack(fnt, 's');
	}
	*fnt.loaded = 1;
	fclose(bdf);
	return fnt;
}

PPM load(PPM ppm)
{
	if (ppm.pixels) {
		free_2d_int_arr(ppm.pixels, *ppm.height);
		ppm.pixels = NULL;
	}

	FILE *image = fopen(ppm.file_location, "rb");
	if (!image) {
		printf("Failed to load %s\n", ppm.file_location);
		*ppm.width = 0;
		*ppm.height = 0;
		return ppm;
	}
	last_cmd('D');

	fseek(image, 3, SEEK_SET); // skips over "P6\n"
	*ppm.width = read_dimensions(image);
	*ppm.height = read_dimensions(image);

	ppm.pixels = allocate_pixel_arr(*ppm.width, *ppm.height);
	fseek(image, 4, SEEK_CUR); // skips over "255\n"
	read_pixels(image, ppm.pixels, *ppm.width, *ppm.height);

	printf("Loaded %s ", ppm.file_location);
	printf("(PPM image %dx%d)\n", *ppm.width, *ppm.height);

	ppm = ppm_file_location_stack(ppm, 's');
	fclose(image);
	return ppm;
}

char *scan_text(void)
{
	int size = 100;
	char *text = allocate_string(size);
	char useless_char;
	scanf("%c%c", &useless_char, &useless_char);
	int i = 0;
	scanf("%c", &text[i]);
	while (text[i] != '"') {
		i++;
		if (i >= size) {
			size *= 2;
			text = reallocate_string(text, size);
		}
		scanf("%c", &text[i]);
	}
	text[i] = '\0';
	return text;
}

void write_char(PPM ppm, FONT fnt, int index, int x, int y, int color)
{
	x += fnt.bbxoff[index];
	y += -fnt.bbyoff[index] - fnt.bbh[index] + 1;

	int row_size = fnt.bbw[index] / 8;
	if (fnt.bbw[index] % 8)
		row_size++;

	int value, temp_x, temp_y;
	// bitmap chars
	for (int i = 0; i < fnt.bbh[index]; i++) {
		for (int j = 0; j < row_size; j++) {
			// for each bit in a char
			for (int k = 0; k < 8; k++) {
				value = (int)fnt.bitmap[index][i * row_size + j];
				// gets the bits in order from the left to the right
				value = (value << k) & 128;
				temp_x = x + j * 8 + k;
				temp_y = y + i;
				temp_y = *ppm.height - temp_y - 1;
				// checks value of the bit and bounds of the image
				if (value && temp_x < *ppm.width && temp_x >= 0)
					if (temp_y < *ppm.height && temp_y >= 0)
						ppm.pixels[temp_y][temp_x] = color;
			}
		}
	}
}

PPM type(PPM ppm, FONT fnt)
{
	char *text = scan_text();
	int start_x, start_y;
	int red, green, blue;
	scanf("%d%d", &start_x, &start_y);
	scanf("%d%d%d", &red, &green, &blue);
	int colour = rgb_to_colour(red, green, blue);

	if (*ppm.width == 0 || *ppm.height == 0) {
		printf("No image loaded\n");
		free(text);
		return ppm;
	}
	if (!fnt.nr_chars || *fnt.loaded == 0) {
		printf("No font loaded\n");
		free(text);
		return ppm;
	}

	last_cmd('Y');
	int len = strlen(text);
	for (int i = 0; i < len; i++) {
		int j = 0;
		while (text[i] != (char)fnt.encoding[j]) {
			j++;
		}
		write_char(ppm, fnt, j, start_x, *ppm.height - start_y - 1, colour);
		start_x += fnt.dwx[j];
		start_y += fnt.dwy[j];
	}

	ppm = ppm_file_location_stack(ppm, 's');
	printf("Text written\n");
	free(text);
	return ppm;
}

PPM undo(LSYSTEM lsys, PPM ppm, FONT *fnt)
{
	char undo_command = last_cmd('-');
	last_cmd('U');

	// LSYSTEM
	if (undo_command == 'L') {
		lsys_file_location_stack(lsys, 'u');
		lsystem(lsys, 'u');
	// TURTLE
	} else if (undo_command == 'T') {
		ppm = ppm_file_location_stack(ppm, 'u');
	// FONT
	} else if (undo_command == 'F') {
		*fnt = free_font(*fnt);
		*fnt = font_file_location_stack(*fnt, 'u');
		*fnt = font(*fnt);
	// TYPE
	} else if (undo_command == 'Y') {
		ppm = ppm_file_location_stack(ppm, 'u');
	// LOAD
	}  else if (undo_command == 'D') {
		ppm = ppm_file_location_stack(ppm, 'u');

	} else
		printf("Nothing to undo\n");

	return ppm;
}

PPM redo(LSYSTEM lsys, FONT *fnt, PPM ppm)
{
	char redo_command = last_cmd('+');
	last_cmd('R');

	// LSYSTEM
	if (redo_command == 'L') {
		lsys_file_location_stack(lsys, 'r');
		lsystem(lsys, 'n');
	// TURTLE
	} else if (redo_command == 'T') {
		ppm = ppm_file_location_stack(ppm, 'r');
		if (*ppm.width == 0 || *ppm.height == 0)
			printf("No image loaded\n");
		else if (*lsys.loaded == 0)
			printf("No L-system loaded\n");
		else
			printf("Drawing done\n");
	// LOAD
	} else if (redo_command == 'D') {
		ppm = ppm_file_location_stack(ppm, 'r');
	// FONT
	} else if (redo_command == 'F') {
		*fnt = free_font(*fnt);
		*fnt = font_file_location_stack(*fnt, 'r');
		*fnt = font(*fnt);
	// TYPE
	} else if (redo_command == 'Y') {
		FONT temp = *fnt;
		ppm = ppm_file_location_stack(ppm, 'r');
		if (*ppm.width == 0 || *ppm.height == 0)
			printf("No image loaded\n");
		else if (!temp.nr_chars || *temp.loaded == 0)
			printf("No font loaded\n");
		else
			printf("Text written\n");

	} else
		printf("Nothing to redo\n");

	return ppm;
}

void print_bit_warning(PPM ppm, int color, int i, int j)
{
	int red, green, blue;
	blue = color % 256;
	color /= 256;
	green = color % 256;
	color /= 256;
	red = color % 256;
	printf("Warning: pixel at (%d, %d) ", j, *ppm.height - i - 1);
	printf("may be read as (%d, %d, %d)\n", red, green, blue);
}

int bitcheck(PPM ppm)
{
	if (*ppm.width == 0 || *ppm.height == 0) {
		printf("No image loaded\n");
		return -1;
	}
	int color = -1, last_color_remainder = -1;
	int bitdivider, halfbyte, error;
	int last_color, last_i, last_j;

	for (int i = 0; i < *ppm.height; i++) {
		for (int j = 0; j < *ppm.width; j++) {
			color = ppm.pixels[i][j];
			bitdivider = 20;
			if (last_color_remainder != -1) {
				bitdivider = 23;
				color += last_color_remainder << 24;
			}
			while (bitdivider != -1) {
				halfbyte = 15 & (color >> bitdivider);

				if (halfbyte == 13) {
					error = 1 << (bitdivider + 1);
					if (bitdivider == 23)
						print_bit_warning(ppm, last_color + 1, last_i, last_j);
					else
						print_bit_warning(ppm, color + error, i, j);
				}

				if (halfbyte == 2) {
					error = 1 << (bitdivider + 1);
					if (bitdivider == 23)
						print_bit_warning(ppm, last_color - 1, last_i, last_j);
					else
						print_bit_warning(ppm, color - error, i, j);
				}
				bitdivider--;
			}
			last_color = color;
			last_i = i;
			last_j = j;
			last_color_remainder = color % 8;
		}
	}
	return 1;
}

int main(void)
{
	char *command = allocate_string(STR_SIZE);
	int order;
	LSYSTEM lsys = allocate_lsys();
	PPM ppm = allocate_ppm();
	FONT fnt;
	lsys_file_location_stack(lsys, 'a');
	font_file_location_stack(fnt, 'a');
	ppm_file_location_stack(ppm, 'a');
	last_cmd('a');
	fnt.file_location = allocate_string(STR_SIZE);
	fnt.nr_chars = NULL;

	scanf("%s", command);
	while (strcmp(command, "EXIT")) {
		if (strcmp(command, "LSYSTEM") == 0) {
			scanf("%s", lsys.file_location);
			lsystem(lsys, 'n');

		} else if (strcmp(command, "DERIVE") == 0) {
			scanf("%d", &order);
			char *derived_axiom = derive(lsys, order);
			if (derived_axiom) {
				printf("%s\n", derived_axiom);
				free(derived_axiom);
			}

		} else if (strcmp(command, "UNDO") == 0) {
			ppm = undo(lsys, ppm, &fnt);

		} else if (strcmp(command, "REDO") == 0)
			ppm = redo(lsys, &fnt, ppm);

		else if (strcmp(command, "LOAD") == 0) {
			scanf("%s", ppm.file_location);
			ppm = load(ppm);

		} else if (strcmp(command, "TURTLE") == 0) {
			turtle(ppm, lsys);
			ppm = ppm_file_location_stack(ppm, 's');

		} else if (strcmp(command, "SAVE") == 0)
			save(ppm);

		else if (strcmp(command, "FONT") == 0) {
			fnt = free_font(fnt);
			scanf("%s", fnt.file_location);
			fnt = font(fnt);

		} else if (strcmp(command, "TYPE") == 0) {
			ppm = type(ppm, fnt);

		} else if (strcmp(command, "BITCHECK") == 0)
			bitcheck(ppm);

		scanf("%s", command);
	}

	free(command);
	free_lsys(lsys);
	free_ppm(ppm);
	free(fnt.file_location);
	free_font(fnt);
	lsys_file_location_stack(lsys, 'f');
	font_file_location_stack(fnt, 'f');
	ppm_file_location_stack(ppm, 'f');
	last_cmd('f');
	return 0;
}
