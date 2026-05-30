#include "include/cst.h"
#include <stdlib.h>

CstNode* cst_make_terminal(CstKind kind, const char* start, int length, int line){
    CstNode* node = malloc(sizeof(CstNode));
    if(!node) return NULL;

    node->kind = kind;
    node->line = line;
    node->u.token.length = length;
    node->u.token.start = start;

    return node;

}

CstNode* cst_make_rule(CstKind kind, int line){
    CstNode* node = malloc(sizeof(CstNode));
    if(!node) return NULL;

    node->kind = kind;
    node->line = line;
    node->u.rule.capacity = 0;
    node->u.rule.children = NULL;
    node->u.rule.count = 0;

    return node;
}


void cst_add_child(CstNode* parent, CstNode* child) {

    int count = parent->u.rule.count;
    int capacity = parent->u.rule.capacity;

    if (count >= capacity) {
        int new_cap = (capacity == 0) ? 4 : capacity * 2;
        
        CstNode** new_children = realloc(parent->u.rule.children, new_cap * sizeof(CstNode*));
        if (!new_children) return; 

        parent->u.rule.children = new_children;
        parent->u.rule.capacity = new_cap;
    }

    parent->u.rule.children[count++] = child;

}


void cst_free(CstNode* node){
    if (!node) return;

    if(node != NULL){
        if(node->u.rule.children){
            for(int i = 0; i < node->u.rule.count; i++){
                cst_free(node->u.rule.children[i]);
            }
            free(node->u.rule.children);
        }
        free(node);
    }
}