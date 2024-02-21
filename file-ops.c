#include "file-ops.h"
#include "str-array.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void free_File_content(struct File file) {
	if (file.size) free(file.content);
}

void free_FileArray_files(struct FileArray farray) {
	if (farray.amount == 0) return;

	for (int i = 0; i < farray.amount; i++) {
		free_File_content(farray.files[i]);
	}
	free(farray.files);
}

struct File get_stdin_File() {
	return (struct File) {
		.content = strdup(STDIN_STR),
		.size = -1
	};
}

int is_stdin_File(struct File file) {
	return strcmp(file.content, STDIN_STR) == 0 & file.size == -1;
}

size_t FileArray_stdin_File_index(struct FileArray farray) {
	for (size_t i = 0; i < farray.amount; i++)
		if (is_stdin_File(farray.files[i])) return i;
	return farray.amount;
}

struct File get_file(const char *fname) {
	if (strcmp(fname, STDIN_STR) == 0)
		return get_stdin_File();
	FILE *fstream = fopen(fname, "rb");
	if (fstream == NULL) { // I guess it's fatality
		fprintf(stderr, "%s: ", fname);
		perror(""); // no way for cooking
		exit(-1);
	}

	struct File file;

	int res = fseek(fstream, 0L, SEEK_END);
	if (res != 0) { // it's bad
		perror("fseek() failed");
		file.size = 0;
		file.content = NULL;
		return file;
	}

	file.size = ftell(fstream);
	file.content = (char*)malloc(file.size);

	fseek(fstream, 0L, SEEK_SET);
	int i = 0;
	for (char c = fgetc(fstream);
		c != EOF;
		c = fgetc(fstream)) file.content[i++] = c;

	res = fclose(fstream);
	if (res != 0) { // I guess it's fatality too
		perror("fclose() failed");
		exit(-1);
	}

	return file;
}

struct FileArray get_files(struct StrArray fnames) {
	if (fnames.amount == 0) {
		return (struct FileArray) {
			.files = NULL,
			.amount = 0
		};
	}

	struct FileArray farray = {
		.files = (struct File*)malloc(fnames.amount * sizeof(struct File)),
		.amount = fnames.amount
	};

	for (int i = 0; i < farray.amount; i++) {
		struct File file = get_file(fnames.strs[i]);
		farray.files[i] = file;
	}

	return farray;
}

size_t File_content_ch_amount(struct File file, char _ch) {
	int amount = 0;
	for (size_t j = 0; j < file.size; j++) {
		char ch = file.content[j];
		if (ch == _ch) amount++;
	}
	return amount;
}

size_t File_content_lamount(struct File file) {
	if (is_stdin_File(file)) return 0; // ignore
	return File_content_ch_amount(file, '\n');
}

size_t File_content_tamount(struct File file) {
	return File_content_ch_amount(file, '\t');
}

size_t FileArray_content_lamount(struct FileArray farray) { // lamount -> lines amount
	size_t amount = 0;
	for (size_t i = 0; i < farray.amount; i++) {
		struct File file = farray.files[i];
		amount += File_content_lamount(file);
	}

	return amount;
}

size_t FileArray_content_no_nl_end_amount(struct FileArray farray) { // lamount -> lines amount
	size_t amount = 0;
	for (size_t i = 0; i < farray.amount; i++) {
		struct File file = farray.files[i];
		if (file.content[file.size - 1] != '\n')
			amount++;
	}
	return amount;
}

struct File File_dup(struct File file) {
	if (is_stdin_File(file)) return get_stdin_File();

	if (!file.size) {
		return (struct File) {
			.content = NULL,
			.size = 0
		};
	}

	struct File dup = {
		.content = (char*)malloc(file.size),
		.size = file.size
	};
	char *dest = memcpy(dup.content, file.content, file.size);
	if (dest != dup.content) {
		fprintf(stderr, "memcpy failed\n");
		exit(-1);
	}
	return dup;
}

void File_cpy(struct File f2, struct File f1) {
	char *dest = memcpy(f2.content, f1.content, f1.size);
	if (dest != f2.content) {
		fprintf(stderr, "memcpy failed\n");
		exit(-1);
	}
}

struct FileArray FileArray_dupn(struct FileArray farray, size_t n) {
	if (n == 0) {
		return (struct FileArray) {
			.files = NULL,
			.amount = 0
		};
	}
	struct FileArray dup = {
		.files = (struct File*)malloc(n * sizeof(struct File)),
		.amount = n
	};

	for (size_t i = 0; i < n; i++)
		dup.files[i] = File_dup(farray.files[i]);

	return dup;
}

struct FileArray FileArray_dup(struct FileArray farray) {
	return FileArray_dupn(farray, farray.amount);
}

void print_File(struct File file) {
	for (size_t i = 0; i < file.size; i++) {
		putc(file.content[i], stdout);
	}
}

void print_FileArray(struct FileArray farray) {
	for (size_t i = 0; i < farray.amount; i++) {
		print_File(farray.files[i]);
	}
}

struct FileArray FileArray_content_from_stdin() {
	struct FileArray farray = {
		.files = NULL,
		.amount = 0
	};

	for (int i = 0; !feof(stdin); i++) {
		char buff[255] = {0};
		while(i < 255 && !feof(stdin)) {
			buff[i++] = getc(stdin);
		}

		if (feof(stdin)) i--;
		if (i == 0) return farray;

		struct File file = {
			.content = (char*)malloc(i),
			.size = i
		};

		char *res = memcpy(file.content, buff, i);
		if (res != file.content) {
			fprintf(stderr, "Failed to read from stdin\n");
			exit(-1);
		}

		farray.files = (struct File*)realloc(farray.files, farray.amount + 1);
		farray.files[farray.amount++] = file;
	}
	rewind(stdin);
	return farray;
}

struct FileArray FileArray_combine(struct FileArray ffa, struct FileArray sfa) {
	struct FileArray facombo;
	facombo.amount = ffa.amount + sfa.amount;
	facombo.files = (struct File*)malloc(facombo.amount * sizeof(struct File));
	size_t i = 0;
	for (size_t j = 0; j < ffa.amount; j++)
		facombo.files[i++] = File_dup(ffa.files[j]);

	for (size_t j = 0; j < sfa.amount; j++)
		facombo.files[i++] = File_dup(sfa.files[j]);

	return facombo;
}

