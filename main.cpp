#include <emscripten/emscripten.h>
#include <sys/stat.h>
#include <xlsxwriter.h>
#include <xlsxwriter/packager.h>

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <thread>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

void get_pixel(stbi_uc *image, size_t imageWidth, size_t x, size_t y, stbi_uc *r, stbi_uc *g, stbi_uc *b, stbi_uc *a) {
	// Returns the rgb value of an image loaded through stb_image
	const stbi_uc *p = image + (4 * (y * imageWidth + x));
	*r = p[0];
	*g = p[1];
	*b = p[2];
	*a = p[3];
}

char *file_name;
int curr_column = 0;
int width, height;
char cell_width = 5;
unsigned char *data = 0;
float percent = 0.0f;
bool debug = false;
bool closed = true;

lxw_workbook *workbook; // workbook_new("excel.xlsx");
lxw_worksheet *worksheet = NULL;

std::map<uint32_t, lxw_format *> formats;
// each format is stored in the excel spreadsheet.
// if two formats with the same color are stored, they can share the same format

extern "C" {
EMSCRIPTEN_KEEPALIVE int load_img(char *filename, float scale) {
	workbook = workbook_new("excel.xlsx");
	if (scale == 0) {
		return -2;
	}
	// loads the image into data and returns -1 if the image can't be loaded
	if (data != NULL) {
		free(data);
	}

	int comps = 0;
	unsigned char *unresized_data = stbi_load(filename, &width, &height, &comps, 4);
	if (unresized_data == NULL) {
		return -1;
	}
	int new_width = width * scale;
	int new_height = height * scale;

	data = (unsigned char *)malloc(sizeof(unsigned char) * new_height * new_width * 4);
	if (data == NULL) {
		std::cout << "Failed To Alloc\n";
		return -1;
	}

	stbir_resize_uint8(unresized_data, width, height, 0, data, new_width, new_height, 0, 4);
	width = new_width;
	height = new_height;

	free(unresized_data);

	worksheet = workbook_add_worksheet(workbook, NULL);
	closed = false;
	return width;
}

EMSCRIPTEN_KEEPALIVE bool next() {
	// writes the next column to the sheet
	// this is chunked rather than all done at once so the DOM can update
	if (curr_column % cell_width == 0) {
		for (int j = 0; j < height; j++) {
			unsigned char r, g, b, a;

			get_pixel(data, width, curr_column, j, &r, &g, &b, &a);
			lxw_format *format;

			unsigned int hex = ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);

			if (formats.find(hex) != formats.end()) {
				format = formats[hex];
			} else {
				format = workbook_add_format(workbook);
				format_set_bg_color(format, hex);
				formats[hex] = format;
			}

			format_set_bg_color(format, hex);
			worksheet_write_string(worksheet, j, curr_column / cell_width, "", format);
		}
	}
	curr_column += 1;

	if (curr_column == width) {
		return false;
	}
	return true;
}

bool done = false;

void internal_save() {
	std::cout << "starting internal save\n";
	workbook_close(workbook);
	done = true;
	std::cout << "done saving internally\n";
}

EMSCRIPTEN_KEEPALIVE bool is_done() { return done; }

EMSCRIPTEN_KEEPALIVE void save() {
	// saves the workbook so JS can read it
	if (!closed) {
		std::thread thread = std::thread(internal_save);
		thread.detach();
		closed = false;
		done = false;
	}
}
}
