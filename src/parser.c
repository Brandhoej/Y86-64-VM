#include "parser.h"

typedef void (*ParseFn)(Parser *parser, Chunk *chunk);
typedef struct {
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
} ParseRule;

ParseRule rules[] = {
    [TOKEN_LEFT_PAREN]    = {grouping, NULL,   PREC_NONE},
    [TOKEN_RIGHT_PAREN]   = {NULL,     NULL,   PREC_NONE},
    [TOKEN_LEFT_BRACE]    = {NULL,     NULL,   PREC_NONE}, 
    [TOKEN_RIGHT_BRACE]   = {NULL,     NULL,   PREC_NONE},
    [TOKEN_COMMA]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_DOT]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_MINUS]         = {unary,    binary, PREC_TERM},
    [TOKEN_PLUS]          = {NULL,     binary, PREC_TERM},
    [TOKEN_SEMICOLON]     = {NULL,     NULL,   PREC_NONE},
    [TOKEN_SLASH]         = {NULL,     binary, PREC_FACTOR},
    [TOKEN_STAR]          = {NULL,     binary, PREC_FACTOR},
    [TOKEN_BANG]          = {NULL,     NULL,   PREC_NONE},
    [TOKEN_BANG_EQUAL]    = {NULL,     NULL,   PREC_NONE},
    [TOKEN_EQUAL]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_EQUAL_EQUAL]   = {NULL,     NULL,   PREC_NONE},
    [TOKEN_GREATER]       = {NULL,     NULL,   PREC_NONE},
    [TOKEN_GREATER_EQUAL] = {NULL,     NULL,   PREC_NONE},
    [TOKEN_LESS]          = {NULL,     NULL,   PREC_NONE},
    [TOKEN_LESS_EQUAL]    = {NULL,     NULL,   PREC_NONE},
    [TOKEN_IDENTIFIER]    = {NULL,     NULL,   PREC_NONE},
    [TOKEN_STRING]        = {NULL,     NULL,   PREC_NONE},
    [TOKEN_NUMBER]        = {number,   NULL,   PREC_NONE},
    [TOKEN_AND]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_CLASS]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_ELSE]          = {NULL,     NULL,   PREC_NONE},
    [TOKEN_FALSE]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_FOR]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_FUN]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_IF]            = {NULL,     NULL,   PREC_NONE},
    [TOKEN_NIL]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_OR]            = {NULL,     NULL,   PREC_NONE},
    [TOKEN_PRINT]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_RETURN]        = {NULL,     NULL,   PREC_NONE},
    [TOKEN_SUPER]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_THIS]          = {NULL,     NULL,   PREC_NONE},
    [TOKEN_TRUE]          = {NULL,     NULL,   PREC_NONE},
    [TOKEN_VAR]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_WHILE]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_ERROR]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_EOF]           = {NULL,     NULL,   PREC_NONE},
};

static ParseRule* getRule(TokenType type) {
    return &rules[type];
}

void initParser(Parser* parser, Scanner* scanner) {
    parser->scanner  = scanner;
    parser->errorCount = 0;
    parser->panicMode = false;
}

void freeParser(Parser* parser) {
    initParser(parser, NULL);
}

static void errorAt(Parser* parser, const Token* token, const char* message) {
    if (parser->panicMode) return;
    parser->panicMode = true;

    CERO_ERROR("[line %u] Error", token->line);
    if (token->type == TOKEN_EOF) {
        CERO_PRINT(" at end", token->line);
    } else if (token->type == TOKEN_ERROR) {

    } else {
        CERO_PRINT(" at '%.*s'", token->length, token->start);
    }
    CERO_PRINT(": %s\n", message);
    parser->errorCount++;
}

static void errorAtCurrent(Parser *parser, const char* message) {
    errorAt(parser, &parser->current, message);
}

static void errorAtPrevious(Parser *parser, const char* message) {
    errorAt(parser, &parser->previous, message);
}

static void advance(Parser *parser) {
    parser->previous = parser->current;
    while (true) {
        parser->current = scanToken(parser->scanner);
        if (parser->current.type != TOKEN_ERROR) break;
        errorAtCurrent(parser, parser->current.start);
    }
}

static void consume(Parser *parser, TokenType type, const char *message) {
    if (parser->current.type == type) {
        advance(parser);
        return;
    }
    errorAtCurrent(parser, message);
}

static uint8_t makeConstant(Chunk *chunk, Value value) {
    /* Does not support "constant long" */
    uint32_t constant = addConstant(chunk, value);
    if (constant > UINT8_MAX) {
        CERO_ERROR("Too many constants in one chunk.");
        return 0;
    }
    return (uint8_t)constant;
}

static void emitByte(Parser *parser, Chunk *chunk, uint8_t byte) {
    writeChunkInsReg(chunk, byte, parser->previous.line);
}

static void emitBytes(Parser *parser, Chunk *chunk, uint8_t byte1, uint8_t byte2) {
    emitByte(parser, chunk, byte1);
    emitByte(parser, chunk, byte2);
}

static void emitReturn(Parser *parser, Chunk *chunk) {
    emitByte(parser, chunk, OP_RETURN);
}

static void emitConstant(Parser *parser, Chunk *chunk, Value value) {
    emitBytes(parser, chunk, OP_CONSTANT, makeConstant(chunk, value));
}

static void parsePrecedence(Parser *parser, Chunk *chunk, Precedence precedence) {
    advance(parser);
    ParseFn prefixRule = getRule(parser->previous.type)->prefix;
    if (prefixRule == NULL) {
        errorAtCurrent(parser, "Expected expression.");
        return;
    }
    prefixRule(parser, chunk);
    while (precedence <= getRule(parser->current.type)->precedence) {
        advance(parser);
        ParseFn infixRule = getRule(parser->previous.type)->infix;
        infixRule(parser, chunk);
    }
}

static void expression(Parser *parser, Chunk *chunk) {
    parsePrecedence(parser, chunk, PREC_ASSIGNMENT);
}

void binary(Parser *parser, Chunk *chunk) {
    TokenType operatorType = parser->previous.type;
    ParseRule *rule = getRule(operatorType);
    parsePrecedence(parser, chunk, (Precedence)(rule->precedence + 1));

    switch (operatorType) {
        case TOKEN_PLUS:  emitByte(parser, chunk, OP_ADD);      break;
        case TOKEN_MINUS: emitByte(parser, chunk, OP_SUBTRACT); break;
        case TOKEN_STAR:  emitByte(parser, chunk, OP_MULTIPLY); break;
        case TOKEN_SLASH: emitByte(parser, chunk, OP_DIVIDE);   break;
        default: return;
    }
}

void unary(Parser *parser, Chunk *chunk) {
    TokenType operatorType = parser->previous.type;
    parsePrecedence(parser, chunk, PREC_UNARY);

    switch (operatorType) {
        case TOKEN_MINUS: emitByte(parser, chunk, OP_NEGATE); break;
        default: return;
    }
}

void number(Parser* parser, Chunk *chunk) {
    double value = strtod(parser->previous.start, NULL);
    emitConstant(parser, chunk, value);
}

void grouping(Parser *parser, Chunk *chunk) {
    expression(parser, chunk);
    consume(parser, TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

void parseTokenStream(Parser *parser, Chunk *chunk) {
    do {
        parser->previous = parser->current;
        expression(parser, chunk);
        parser->current = scanToken(parser->scanner);
    } while (parser->current.type == TOKEN_EOF);
    emitReturn(parser, chunk);
}
