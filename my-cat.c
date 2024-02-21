#include "input.h"
#include "file-ops.h"
#include "opt-array.h"
#include "parsers.h"
#include "info-opts.h"

int main(int argc, char **argv) {
    if (argc == 1) {
		struct FileArray fstdin = FileArray_content_from_stdin();
		print_FileArray(fstdin);
		free_FileArray_files(fstdin);
        return 0;
    }

	struct StrArray unparsed = {
		.strs = argv + 1,
		.amount = argc - 1
	};
    struct Input input = get_input(unparsed);

	struct FileArray farray = get_files(input.files);
	struct OptArray opta = get_OptArray(input.opts);
	free_Input_members(input);

	if (opta.amount == 1 && is_info_opt(opta.options[0])) {
		print_info_opt(opta.options[0]);
	} else if (farray.amount != 0) {
		init_parsers(farray, opta);

		size_t diff = farray.amount;
		struct File *alast = farray.files + farray.amount;
		struct FileArray fdiff = farray;

		struct ParseResult res = parse(fdiff);
		print_FileArray(res.fparsed);
		diff -= res.amount;
		if (res.amount) free_FileArray_files(res.fparsed);

		while (diff != 0) {
			struct FileArray fstdin = FileArray_content_from_stdin();

			if (--diff == 0) {
				res = parse(fstdin);
				print_FileArray(res.fparsed);
				free_FileArray_files(res.fparsed);
				free_FileArray_files(fstdin);
				break;
			}

			fdiff = (struct FileArray) {
				.files = alast - diff,
				.amount = diff
			};
			fdiff = FileArray_combine(fstdin, fdiff);

			res = parse(fdiff);
			print_FileArray(res.fparsed);

			diff -= (res.amount - fstdin.amount);

			if (res.amount) free_FileArray_files(res.fparsed);
			free_FileArray_files(fdiff);
			free_FileArray_files(fstdin);
		}
		
		clear_parsers();
	}

	free_OptArray_options(opta);
	free_FileArray_files(farray);
    return 0;
}

