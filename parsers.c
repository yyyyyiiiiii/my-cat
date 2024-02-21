#include "parsers.h"
#include "file-ops.h"
#include "opt-array.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

int ln_size(int lamount) { // ln -> line number
	int size = log10(lamount) + 1;
	int padd = 6 - (size % 6);
	size += padd;
	return size;
}

void set_ln(char *line, size_t ln, size_t lnsize) { // ln -> line number
	memset(line, ' ', lnsize);
	sprintf(line, "%lu", ln);
	int numci = 0; // num character index

	for (int i = lnsize - 1; i >= 0; i--) {
		if (line[i] != ' ') {
		    if (line[i] == '\0') {
				line[i] = ' ';
		    continue;
		    }
		    numci = i;
		    break;
		}
	}

	int numcpi = lnsize - 1; // num character put index
	while (numci >= 0) {
		line[numcpi] = line[numci];
		line[numci] = ' ';
		numcpi--;
		numci--;
	}
}

void set_space(char *buff) {
	buff[0] = ' ';
	buff[1] = ' ';
}

static size_t lnsize = 6;
static size_t ln = 1;
static size_t space_size = 2;

struct FileArray parser_n(struct FileArray farray) {
	size_t lamount = FileArray_content_lamount(farray);
	size_t last_noend_with_nl = farray.files[farray.amount - 1].content[farray.files[farray.amount - 1].size - 1] != '\n';

	struct FileArray pfarray = {
		.amount = (lamount + last_noend_with_nl)
	};
	pfarray.files = (struct File*)malloc(pfarray.amount * sizeof(struct File));

	if (lamount == 0) {
		char *content = (char*)malloc(lnsize + space_size + farray.files[0].size);
		set_ln(content, ln, lnsize);
		set_space(content + lnsize);

		struct File ffile = {
			.content = content + lnsize + space_size,
			.size = farray.files[0].size
		};
		File_cpy(ffile, farray.files[0]);
		ffile.content = content;
		ffile.size += lnsize + space_size;
		pfarray.files[0] = ffile;

		for (size_t i = 1; i < farray.amount; i++)
			pfarray.files[i] = File_dup(farray.files[i]);
		return pfarray;
	}

	size_t pfarray_file_id = 0;
	struct FileArray pre = {
		.files = NULL,
		.amount = 0
	};
	for (size_t i = 0; i < farray.amount; i++) {
		struct File file = farray.files[i];
		size_t prev_nl_i = -1;
		for (size_t j = 0; j < file.size; j++) {
			if (file.content[j] != '\n') continue;

			size_t pre_size = 0;
			if (pre.amount)
				for (size_t k = 0; k < pre.amount; k++)
					pre_size += pre.files[k].size;

			size_t buffsize = j - prev_nl_i;
			size_t size = lnsize + space_size + pre_size + buffsize;
			char *content = (char*)malloc(size);
			set_ln(content, ln++, lnsize);
			set_space(content + lnsize);

			char *pre_start = content + lnsize + space_size;
			for (size_t k = 0, p_off = 0; k < pre.amount; k++) {
				char *dest = memcpy(pre_start + p_off, pre.files[k].content, pre.files[k].size);
				p_off += pre.files[k].size;
			}
			if (pre.amount) {
				free_FileArray_files(pre);
				pre.files = NULL;
				pre.amount = 0;
			}

			char *buff_start = pre_start + pre_size;
			char *dest = memcpy(buff_start, file.content + prev_nl_i + 1, buffsize);
			if (dest != buff_start) {
				fprintf(stderr, "memcpy failed\n");
				exit(-1);
			}

			pfarray.files[pfarray_file_id++] = (struct File) {
				.content = content,
				.size = size
			};
			prev_nl_i = j;
		}

		if (prev_nl_i != file.size - 1) {
			size_t j = file.size - 1;
			size_t buffsize = j - prev_nl_i;

			if (i != farray.amount - 1) {
				pre.files = realloc(pre.files, (pre.amount + 1) * sizeof(struct File));
				char *content = (char*)malloc(buffsize);
				char *dest = memcpy(content, file.content + prev_nl_i + 1, buffsize);
				if (dest != content) {
					fprintf(stderr, "memcpy failed\n");
					exit(-1);
				}
				pre.files[pre.amount++] = (struct File) {
					.content = content,
					.size = buffsize
				};
				continue;
			}

			size_t size = buffsize + lnsize + space_size;
			char *content = (char*)malloc(size);
			set_ln(content, ln++, lnsize);
			set_space(content + lnsize);

			char *dest = memcpy(content + lnsize + space_size, file.content + prev_nl_i + 1, buffsize);
			if (dest != content + lnsize + space_size) {
				fprintf(stderr, "memcpy failed\n");
				exit(-1);
			}

			pfarray.files[pfarray_file_id++] = (struct File) {
				.content = content,
				.size = size
			};
		}
	}

	return pfarray;
}

struct FileArray parser_b(struct FileArray farray) {
	size_t lamount = FileArray_content_lamount(farray);
	size_t last_noend_with_nl = farray.files[farray.amount - 1].content[farray.files[farray.amount - 1].size - 1] != '\n';

	struct FileArray pfarray = {
		.amount = (lamount + last_noend_with_nl)
	};
	pfarray.files = (struct File*)malloc(pfarray.amount * sizeof(struct File));

	if (lamount == 0) {
		char *content = (char*)malloc(lnsize + space_size + farray.files[0].size);
		set_ln(content, ln, lnsize);
		set_space(content + lnsize);

		struct File ffile = {
			.content = content + lnsize + space_size,
			.size = farray.files[0].size
		};
		File_cpy(ffile, farray.files[0]);
		ffile.content = content;
		ffile.size += lnsize + space_size;
		pfarray.files[0] = ffile;

		for (size_t i = 1; i < farray.amount; i++)
			pfarray.files[i] = File_dup(farray.files[i]);
		return pfarray;
	}

	size_t pfarray_file_id = 0;
	struct FileArray pre = {
		.files = NULL,
		.amount = 0
	};
	for (size_t i = 0; i < farray.amount; i++) {
		struct File file = farray.files[i];
		size_t prev_nl_i = -1;
		for (size_t j = 0; j < file.size; j++) {
			if (file.content[j] != '\n') continue;

			size_t pre_size = 0;
			if (pre.amount)
				for (size_t k = 0; k < pre.amount; k++)
					pre_size += pre.files[k].size;

			size_t buffsize = j - prev_nl_i;
			if (buffsize + pre_size == 1) {
				char *content = (char*)malloc(buffsize);
				*content = '\n';

				pfarray.files[pfarray_file_id++] = (struct File) {
					.content = content,
					.size = buffsize
				};
				prev_nl_i = j;
				continue;
			}

			size_t size = lnsize + space_size + pre_size + buffsize;
			char *content = (char*)malloc(size);
			set_ln(content, ln++, lnsize);
			set_space(content + lnsize);

			char *pre_start = content + lnsize + space_size;
			for (size_t k = 0, p_off = 0; k < pre.amount; k++) {
				char *dest = memcpy(pre_start + p_off, pre.files[k].content, pre.files[k].size);
				p_off += pre.files[k].size;
			}
			if (pre.amount) {
				free_FileArray_files(pre);
				pre.files = NULL;
				pre.amount = 0;
			}

			char *buff_start = pre_start + pre_size;
			char *dest = memcpy(buff_start, file.content + prev_nl_i + 1, buffsize);
			if (dest != buff_start) {
				fprintf(stderr, "memcpy failed\n");
				exit(-1);
			}

			pfarray.files[pfarray_file_id++] = (struct File) {
				.content = content,
				.size = size
			};
			prev_nl_i = j;
		}

		if (prev_nl_i != file.size - 1) {
			size_t j = file.size - 1;
			size_t buffsize = j - prev_nl_i;

			if (i != farray.amount - 1) {
				pre.files = realloc(pre.files, (pre.amount + 1) * sizeof(struct File));
				char *content = (char*)malloc(buffsize);
				char *dest = memcpy(content, file.content + prev_nl_i + 1, buffsize);
				if (dest != content) {
					fprintf(stderr, "memcpy failed\n");
					exit(-1);
				}
				pre.files[pre.amount++] = (struct File) {
					.content = content,
					.size = buffsize
				};
				continue;
			}

			size_t size = buffsize + lnsize + space_size;
			char *content = (char*)malloc(size);
			set_ln(content, ln++, lnsize);
			set_space(content + lnsize);

			char *dest = memcpy(content + lnsize + space_size, file.content + prev_nl_i + 1, buffsize);
			if (dest != content + lnsize + space_size) {
				fprintf(stderr, "memcpy failed\n");
				exit(-1);
			}

			pfarray.files[pfarray_file_id++] = (struct File) {
				.content = content,
				.size = size
			};
		}
	}

	return pfarray;
}

size_t lfcut_amount(struct FileArray farray) { // lines for cut amount
	size_t amount = 0;
	int cut = 0;
	for (size_t i = 0; i < farray.amount; i++) {
		struct File file = farray.files[i];
		size_t prev_nl_i = -1;
		for (size_t j = 0; j < file.size; j++) {
			char ch = file.content[j];
			if (ch != '\n') {
				if (cut) cut = 0;
				continue;
			}

			size_t buffsize = j - prev_nl_i;
			if (cut && buffsize == 1) {
				prev_nl_i = j;
				amount++;
				continue;
			}
			if (buffsize == 1) cut = 1;

		}
	}
	return amount;
}

// TODO: fix when use stdin
struct FileArray parser_s(struct FileArray farray) {
	size_t lamount = FileArray_content_lamount(farray) - lfcut_amount(farray);
	size_t no_nl_end_amount = FileArray_content_no_nl_end_amount(farray);

	struct FileArray pfarray = {
		.amount = (lamount + no_nl_end_amount)
	};
	pfarray.files = (struct File*)malloc(pfarray.amount * sizeof(struct File));

	// size_t lnsize = ln_size(lamount);
	// size_t ln = 1;

	int cut = 0;
	size_t pfarray_file_id = 0;
	for (size_t i = 0; i < farray.amount; i++) {
		struct File file = farray.files[i];
		size_t prev_nl_i = -1;
		for (size_t j = 0; j < file.size; j++) {
			char ch = file.content[j];
			if (ch != '\n') {
				if (cut) cut = 0;
				continue;
			}

			size_t buffsize = j - prev_nl_i;

			if (cut && buffsize == 1) {
				prev_nl_i = j;
				continue;
			}
			if (buffsize == 1) cut = 1;

			size_t size = buffsize;
			char *content = (char*)malloc(size);

			char *dest = memcpy(content, file.content + prev_nl_i + 1, buffsize);
			if (dest != content) {
				fprintf(stderr, "memcpy failed\n");
				exit(-1);
			}

			pfarray.files[pfarray_file_id++] = (struct File) {
				.content = content,
				.size = size
			};
			prev_nl_i = j;
		}

		if (prev_nl_i != file.size - 1) {
			size_t j = file.size - 1;

			size_t buffsize = j - prev_nl_i;
			size_t size = buffsize;
			char *content = (char*)malloc(size);

			char *dest = memcpy(content, file.content + prev_nl_i + 1, buffsize);
			if (dest != content) {
				fprintf(stderr, "memcpy failed\n");
				exit(-1);
			}

			pfarray.files[pfarray_file_id++] = (struct File) {
				.content = content,
				.size = size
			};
		}
	}

	return pfarray;
}

struct FileArray parser_E(struct FileArray farray) {
	struct FileArray pfarray = {
		.files = (struct File*)malloc(farray.amount * sizeof(struct File)),
		.amount = farray.amount
	};

	size_t pfarray_file_id = 0;
	for (size_t i = 0; i < farray.amount; i++) {
		struct File file = farray.files[i];
		size_t file_content_lamount = File_content_lamount(file);

		struct File pfile;
		pfile.size = file_content_lamount + file.size;
		pfile.content = (char*)malloc(pfile.size);

		for (size_t j = 0, k = 0; j < file.size; j++, k++) {
			char ch = file.content[j];
			if (ch == '\n')
				pfile.content[k++] = '$';

			pfile.content[k] = ch;
		}

		pfarray.files[pfarray_file_id++] = pfile;
	}
	return pfarray;
}

struct FileArray parser_T(struct FileArray farray) {
	struct FileArray pfarray = {
		.files = (struct File*)malloc(farray.amount * sizeof(struct File)),
		.amount = farray.amount
	};

	size_t pfarray_file_id = 0;
	for (size_t i = 0; i < farray.amount; i++) {
		struct File file = farray.files[i];
		size_t file_content_lamount = File_content_tamount(file);

		struct File pfile;
		pfile.size = file_content_lamount + file.size;
		pfile.content = (char*)malloc(pfile.size);

		for (size_t j = 0, k = 0; j < file.size; j++, k++) {
			char ch = file.content[j];
			if (ch == '\t') {
				pfile.content[k++] = '^';
				pfile.content[k] = 'I';
				continue;
			}

			pfile.content[k] = ch;
		}

		pfarray.files[pfarray_file_id++] = pfile;
	}
	return pfarray;
}

size_t npr_crep_size(struct File file) { // npr_crep_size -> non printing characters repaction size
	size_t size = 0;
	for (size_t i = 0; i < file.size; i++) {
		unsigned char ch = file.content[i];
		if (ch == '\t' || ch == '\n') // ignore tabs + new lines
			continue;
		if (ch >= 128) {
			ch -= 128;
			size += 2;
		}
		if (ch < 32 || ch == 127)
			size++;
	}
	return size;
}

struct FileArray parser_v(struct FileArray farray) {
	struct FileArray pfarray = {
		.files = (struct File*)malloc(farray.amount * sizeof(struct File)),
		.amount = farray.amount
	};

	size_t pfarray_file_id = 0;
	for (size_t i = 0; i < farray.amount; i++) {
		struct File file = farray.files[i];
		size_t crep_size = npr_crep_size(file);

		struct File pfile;
		pfile.size = crep_size + file.size;
		pfile.content = (char*)malloc(pfile.size);

		for (size_t j = 0, k = 0; j < file.size; j++, k++) {
			unsigned char ch = file.content[j];
			if (ch == '\t' || ch == '\n') { // ignore tabs + new lines
				pfile.content[k] = ch;
				continue;
			}

			if (ch >= 128) {
				ch -= 128;
				pfile.content[k++] = 'M';
				pfile.content[k++] = '-';
			}
			if (ch < 32 || ch == 127) {
				pfile.content[k++] = '^';
				ch ^= 0x40;
			}
			pfile.content[k] = ch;
		}

		pfarray.files[pfarray_file_id++] = pfile;
	}
	return pfarray;
}

OptParser get_parser(int opt) {
	switch (opt) {
		case OPT_n: return parser_n;
		case OPT_b: return parser_b;
		case OPT_s: return parser_s;
		case OPT_E: return parser_E;
		case OPT_T: return parser_T;
		case OPT_v: return parser_v;
		default: return NULL;
	}
}

typedef struct OptParsers {
	OptParser *parsers;
	size_t amount;
} OptParsers;

static OptParsers parsers_array = {
	.parsers = NULL,
	.amount = 0
};

void init_parsers(struct FileArray farray, struct OptArray opta) {
	if (!OptArray_contain(opta, OPT_n)
		&& !OptArray_contain(opta, OPT_b))
		return;

	size_t lamount = FileArray_content_lamount(farray);
	lnsize = ln_size(lamount);
	ln = 1;

	if (opta.amount == 0) return;

	parsers_array = (OptParsers) {
		.parsers = (OptParser*)malloc(opta.amount * sizeof(OptParser)),
		.amount = opta.amount
	};

	for (size_t i = 0; i < opta.amount; i++) {
		OptParser parser = get_parser(opta.options[i]);
		parsers_array.parsers[i] = parser;
	}
}

void clear_parsers() {
	if (parsers_array.parsers != NULL)
		free(parsers_array.parsers);
}

struct ParseResult parse(struct FileArray farray) {
	size_t n = FileArray_stdin_File_index(farray);
	struct FileArray res = FileArray_dupn(farray, n);
	if (n == 0) return (struct ParseResult) {
		.fparsed = res,
		.amount = n
	};

	for (size_t i = 0; i < parsers_array.amount; i++) {
		OptParser parser = parsers_array.parsers[i];
		if (parser == NULL) continue;

		struct FileArray pfarray = parser(res);
		free_FileArray_files(res);
		res = pfarray;
	}

	return (struct ParseResult) {
		.fparsed = res,
		.amount = n
	};
}
