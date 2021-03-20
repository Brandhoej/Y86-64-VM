#ifndef cero_parser_h
#define cero_parser_h

#include "common.h"
#include "scanner.h"
#include "chunk.h"

typedef struct {
    Token current;
    Token previous;
    bool panicMode;
    uint8_t errorCount;
    Scanner* scanner;
} Parser;

typedef enum {
  PREC_NONE,
  PREC_ASSIGNMENT,  // =
  PREC_OR,          // or
  PREC_AND,         // and
  PREC_EQUALITY,    // == !=
  PREC_COMPARISON,  // < > <= >=
  PREC_TERM,        // + -
  PREC_FACTOR,      // * /
  PREC_UNARY,       // ! -
  PREC_CALL,        // . ()
  PREC_PRIMARY
} Precedence;

void initParser(Parser* parser, Scanner* scanner);
void freeParser(Parser* parser);
void parseTokenStream(Parser* parser, Chunk* chunk);

void binary(Parser *parser, Chunk *chunk);
void unary(Parser *parser, Chunk *chunk);
void number(Parser* parser, Chunk *chunk);
void grouping(Parser *parser, Chunk *chunk);

#endif