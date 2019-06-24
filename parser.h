//  Kohl Stark
//  cpp_Compiler
//  11/17/18
//  parser.h
//

#ifndef parser_h
#define parser_h

#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
//#include <sstream>
#include "lexer.h"
#include "compiler.h"
//#include "ir_debug.h"

using namespace std;

class Parser
{
    private:
    Token expect(TokenType expected_type);
    Token peek();
    Token doublePeek();
    struct StatementNode* head;
    //struct StatementNode* tail;
    //struct StatementNode* next;
    
    public:
    
    void syntax_error();
    // struct statment node
    struct StatementNode* parse_program();
    struct ValueNode* lookup(string found);
    void parse_var_section();
    void parse_id_list(); // fill vector of ID's
    struct StatementNode* parse_body();
    struct StatementNode* parse_stmt_list();
    struct StatementNode* parse_stmt();
    struct AssignmentStatement* parse_assign_stmt();
    void parse_expr(AssignmentStatement* myAssign);
    struct ValueNode* parse_primary();
    void parse_op(AssignmentStatement* newPass);
    struct PrintStatement* parse_print_stmt();
    void parse_while_stmt(StatementNode* passIn);
    struct IfStatement* parse_if_stmt();
    void parse_condition(IfStatement* myIf);
    void parse_relop(IfStatement* newPass);
    struct StatementNode* parse_switch_stmt();
    void parse_for_stmt(StatementNode* myPass);
    struct StatementNode* parse_case_list();
    struct StatementNode* parse_case();
    struct StatementNode* parse_default_case();
    
    int myString(string num);
    
    LexicalAnalyzer lexer;
    Parser();
};

#endif /* parser_h */
