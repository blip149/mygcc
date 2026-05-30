#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "include/lexer.h"

int main(int argc, char* argv[]) {
    
    if (argc < 3){
        fprintf(stderr, "Usage: ./mygcc compile <Filename.rsc>\n");
        return 1;
    }
    
    if (strcmp(argv[1], "compile") == 0) {
        printf("Starting compilation of %s..\n", argv[2]);

        char *file_path = argv[2];
        FILE *stream = fopen(file_path, "rb");

        if (stream == NULL){
            printf("Failed to open file: %s\n", file_path);
            return 1;
        }
        
        fseek(stream, 0, SEEK_END);
        size_t len = ftell(stream);
        rewind(stream);

        char* buffer = (char*)malloc(len + 1);
        if (!buffer){
            fclose(stream);
            return 1;
        }

        size_t result = fread(buffer, 1, len, stream);
        fclose(stream);

        if (result != len){
            printf("Error reading the complete file\n");
            free(buffer);
            return 1;
        }

        buffer[len] = '\0';

        Lexer lex;
        lexer_init(&lex, buffer, len); 
        
        Token* t;

        while ((t = next_token(&lex))->type != TOKEN_EOF) {
            if (t->type == TOKEN_ERROR) {
                printf("Error at line %d\n", t->line);
                break;
            }
            

            printf("Token type: %d | Lexeme: %.*s\n", t->type, t->length, t->start);
        }
        
        free(buffer);
        return 0;
    }
    
    return 0;
    //  gcc -Wall -Wextra -Isrc/include -g src/lexer.c src/main.c -o bin/myrustc.exe
    // gcc -E -std=c99 

}
