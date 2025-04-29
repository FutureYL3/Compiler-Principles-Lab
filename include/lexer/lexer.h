#ifndef LEXER_H
#define LEXER_H

#include <lexer/token.h>
#include <cstdio>

#ifdef __cplusplus
extern "C" {
#endif

/* input: a file pointer to the source code, output: the next token */
Token next_token(FILE *fp);

#ifdef __cplusplus
}
#endif

#endif
