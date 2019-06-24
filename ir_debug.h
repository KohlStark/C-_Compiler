//  Kohl Stark
//  cpp_Compiler
//  11/17/18
//  ir_debug.h
//
#ifndef __IR_DEBUG_H__
#define __IR_DEBUG_H__

#include "compiler.h"

/*
 * This function can be used to print out the intermediate representation
 * in a human-readable form, useful for debugging purposes.
 */
void print_program(struct StatementNode* program);
//static void print_if(struct StatementNode* st, int indent);


#endif

