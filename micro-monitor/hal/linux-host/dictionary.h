/*
 * Forth dictionary and program token storage
 */

#include <stdint.h>

/* on sdcc declare this as code storage: todo (saves 1 byte) */

typedef void (*word_fn)(void);

/* dictionary words[] */

typedef struct {

	const char *name;
	word_fn fn;

} word_entry_t;

extern const word_entry_t words[];

#define NWORDS (sizeof(words) / sizeof(word_entry_t))

/* ---- token / dictionary types ---- */

typedef enum {
	TOK_BEGIN,
	TOK_WORD,    /* call a dictionary function          */
	TOK_LIT,     /* push a literal onto the data stack   */
	TOK_AGAIN,   /* unconditional jump back to begin_ip  */
	TOK_ZEXIT,   /* pop flag; if true, halt program      */
	TOK_JUMP     /* unconditional halt (plain EXIT)      */
} tok_type_t;

typedef struct {
	tok_type_t type;
	union {
		uint8_t  word_idx;      /* TOK_WORD  */
		int16_t  lit;           /* TOK_LIT   */
		int16_t  jump_target;   /* TOK_AGAIN */
	} u;
} token_t;
