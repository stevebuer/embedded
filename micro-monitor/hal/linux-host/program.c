#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "dictionary.h"

#define MAXTOK 64

token_t prog[MAXTOK];   // pre-tokenized program, built once
int ntok;

const word_entry_t words[] = {
    {"DUP", NULL},
    {"+", NULL},
    {NULL, NULL}
};

void run(void)
{
    int ip = 0;
    int begin_ip = -1;          // only need one, no nesting for blink
    while (ip < ntok) {
        token_t t = prog[ip];
        if (t.type == TOK_BEGIN) {
            begin_ip = ip;
            ip++;
        } else if (t.type == TOK_AGAIN) {
            ip = begin_ip;      // jump back — the whole "compiler"
        } else {
            /* dispatch(t); */        // your existing linear-scan word lookup
            ip++;
        }
    }
}

/* ---- tokenizer ---- */

/*
 * Tokenizes the whole input buffer in one pass.
 * Returns 0 on success, -1 on error (unknown word, bad literal, overflow).
 * On success, *prog_out is filled and *ntok_out holds the token count.
 */
int tokenize(const char *src, token_t *prog, int *ntok_out)
{
    int ntok    = 0;
    int begin_ip = -1;          /* single, unnested BEGIN marker   */
    const char *p = src;

    while (*p) {
        char word[16];
        int  wlen = 0;

        /* skip whitespace */
        while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++;
        if (!*p) break;

        /* grab one space-delimited token */
        while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') {
            if (wlen < (int)sizeof(word) - 1) word[wlen++] = *p;
            p++;
        }
        word[wlen] = '\0';

        if (ntok >= MAXTOK) return -1;   /* program too long */

        /* ---- control words, handled before the dictionary scan ---- */

        if (strcmp(word, "BEGIN") == 0) {
            begin_ip = ntok;             /* record position, emit nothing */
            continue;
        }

        if (strcmp(word, "AGAIN") == 0) {
            if (begin_ip < 0) return -1; /* AGAIN with no matching BEGIN */
            prog[ntok].type = TOK_AGAIN;
            prog[ntok].u.jump_target = begin_ip;
            ntok++;
            continue;
        }

        if (strcmp(word, "?EXIT") == 0) {
            prog[ntok].type = TOK_ZEXIT;
            ntok++;
            continue;
        }

        if (strcmp(word, "EXIT") == 0) {
            prog[ntok].type = TOK_JUMP;
            prog[ntok].u.jump_target = -1;   /* -1 == halt sentinel */
            ntok++;
            continue;
        }

        /* ---- literal number? ---- */

        {
            char *endptr;
            long val = strtol(word, &endptr, 10);
            if (*word != '\0' && *endptr == '\0') {
                prog[ntok].type   = TOK_LIT;
                prog[ntok].u.lit  = (int16_t)val;
                ntok++;
                continue;
            }
        }

        /* ---- ordinary word: linear scan the dictionary ---- */

        {
            int i, found = 0;
            for (i = 0; i < NWORDS; i++) {
                if (strcmp(word, words[i].name) == 0) {
                    prog[ntok].type = TOK_WORD;
                    prog[ntok].u.word_idx = (uint8_t)i;
                    ntok++;
                    found = 1;
                    break;
                }
            }
            if (!found) return -1;   /* unknown word */
        }
    }

    *ntok_out = ntok;
    return 0;
}
