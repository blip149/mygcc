#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "include/lexer.h"
#include "include/parser.h"
#include "include/cst.h"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: ./mygcc compile <filename.c>\n");
        return 1;
    }

    if (strcmp(argv[1], "compile") != 0) {
        fprintf(stderr, "Unknown command: %s\n", argv[1]);
        return 1;
    }

    printf("Starting compilation of %s...\n", argv[2]);

    FILE* stream = fopen(argv[2], "rb");
    if (!stream) {
        printf("Failed to open file: %s\n", argv[2]);
        return 1;
    }

    fseek(stream, 0, SEEK_END);
    size_t len = ftell(stream);
    rewind(stream);

    char* buffer = (char*)malloc(len + 1);
    if (!buffer) {
        fclose(stream);
        return 1;
    }

    size_t result = fread(buffer, 1, len, stream);
    fclose(stream);

    if (result != len) {
        printf("Error reading the complete file\n");
        free(buffer);
        return 1;
    }
    buffer[len] = '\0';

    Lexer lex;
    lexer_init(&lex, buffer, len);

    Parser parser;
    parser_init(&parser, &lex);

    CstNode* ast_root = parse_translation_unit(&parser);

    if (parser.has_error) {
        printf("Compilation finished with errors.\n");
    } else {
        printf("Compilation finished successfully.\n");
    }

    cst_free(ast_root);
    free(buffer);

    return parser.has_error ? 1 : 0;
}
    //  gcc -Wall -Wextra -Isrc/include -g src/lexer.c src/main.c src/cst.c src/parser.c src/parse_helper.c -o bin/mygcc.exe
    // gcc -E -std=c99 

