#ifndef PARSERS
#define PARSERS

#include "opt-array.h"
#include "file-ops.h"

typedef struct FileArray (*OptParser)(struct FileArray);

struct ParseResult {
	struct FileArray fparsed;
	size_t amount; // of files given in input was parsed
};

struct FileArray parser_n(struct FileArray);
struct FileArray parser_b(struct FileArray);
struct FileArray parser_s(struct FileArray);
struct FileArray parser_E(struct FileArray);
struct FileArray parser_T(struct FileArray);
struct FileArray parser_v(struct FileArray);
struct ParseResult parse(struct FileArray);

void init_parsers(struct FileArray, struct OptArray);
void clear_parsers();

#endif // !PARSERS

