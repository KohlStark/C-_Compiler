//  Kohl Stark
//  cpp_Compiler
//  11/17/18
//  parser.cpp
// 

#include "parser.h"

vector<struct ValueNode*> globalVect;

int Parser::myString(string num)
{
    int Integer = 0;
    for (int i = 0; i < num.size(); i++)
       Integer = Integer * 10 + (num[i] - '0');
    
    return Integer;
}

struct StatementNode * parse_generate_intermediate_representation()
{
    Parser *kohl = new Parser();
    return kohl->parse_program();
}

Parser::Parser()
{
    
}
void Parser::syntax_error()
{
    cout << "Syntax Error";
    exit(1);
}
// checks next token without conusming input
Token Parser::peek()
{
    Token t = lexer.GetToken();
    lexer.UngetToken(t);
    return t;
}
Token Parser::doublePeek()
{
    Token t1 = lexer.GetToken();
    Token t2 = lexer.GetToken();
    lexer.UngetToken(t2);
    lexer.UngetToken(t1);
    return t2;
}
Token Parser::expect(TokenType expected_type)
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
    {
        syntax_error();
    }
    return t;
}
struct ValueNode* Parser::lookup(string found)
{
    vector<struct ValueNode*> :: iterator i;
    for(i = globalVect.begin(); i != globalVect.end(); i++)
    {
        if(found == (*i)->name)
        {
            return (*i);
        }
    }
    return NULL;
}
struct StatementNode* Parser::parse_program()
{
    //program -> var_section body
    parse_var_section();
    head = parse_body();
    expect(END_OF_FILE);
    return head;
    
}
void Parser::parse_var_section()
{
    //var_section ->  id_list SEMICOLON
    parse_id_list();
    expect(SEMICOLON);
    
}
void Parser::parse_id_list()
{
    //id_list ->  ID COMMA id_list | ID
    
    struct ValueNode* myPush = new struct ValueNode;
    myPush->name = expect(ID).lexeme;
    globalVect.push_back(myPush);
    //expect(ID);
    if(peek().token_type == COMMA)
    {
        expect(COMMA);
        parse_id_list();
    }
}
struct StatementNode* Parser::parse_body()
{
    //body ->  LBRACE stmt_list RBRACE
    struct StatementNode* myReturn;
    expect(LBRACE);
    myReturn = parse_stmt_list();
    expect(RBRACE);
    return myReturn;

}
// returns a statement node,
struct StatementNode* Parser::parse_stmt_list()
{
    //stmt_list ->  stmt stmt_list | stmt
    // recursively builds the list of stmt's
    struct StatementNode* myResult = parse_stmt(); // returns if stmt
    //parse_stmt();
    //cout << "HI" << endl;
    if(peek().token_type != RBRACE)
    {
        if(myResult->type == IF_STMT)
        {
            myResult->next->next = parse_stmt_list(); // append st1 to  noop
        }else if(myResult->type == ASSIGN_STMT)
        {
            //cout << "FIRST" << endl;
            // check if it is a normal assign stmt or a for loop
           if(myResult->next == NULL)
           {
               //cout << "SECOND" << endl;
               myResult->next = parse_stmt_list();

           }else
           {
               //cout << "THIRD" << endl;
               myResult->next->next->next = parse_stmt_list();
           }
        }
        else
        {
            myResult->next = parse_stmt_list(); // append st1 to st
            //cout << "LAST ELSE" << endl;
        }
    }
    return myResult;
}
struct StatementNode* Parser::parse_stmt()
{
    //stmt ->  assign_stmt | print_stmt | while_stmt | if_stmt | switch_stmt | for_stmt
    // ID
    // print
    // WHILE
    // IF
    // SWITCH
    // FOR
    struct StatementNode* myResult = new struct StatementNode;
    // default set the instances of myResult
    myResult->assign_stmt = NULL;
    /*myResult->if_stmt = NULL;
    myResult->print_stmt = NULL;
    myResult->type = NOOP_STMT;
    myResult->goto_stmt = NULL;*/
    
    Token t = peek();
    if(t.token_type == ID)
    {
        myResult->assign_stmt = parse_assign_stmt();
        myResult->type = ASSIGN_STMT;
    }else if(t.token_type == PRINT)
    {
        myResult->print_stmt = parse_print_stmt();
        myResult->type = PRINT_STMT;
    }else if(t.token_type == WHILE)
    {
        parse_while_stmt(myResult);
        myResult->type = IF_STMT;
        myResult->next = myResult->if_stmt->false_branch;
    }else if(t.token_type == IF)
    {
        myResult->if_stmt = parse_if_stmt();
        myResult->type = IF_STMT;
        myResult->next = myResult->if_stmt->false_branch;
        //print_if(myResult, 0);
        // jiberish
    }else if(t.token_type == SWITCH)
    {
        myResult = parse_switch_stmt();
        //myResult->type = IF_STMT;
    }else if(t.token_type == FOR)
    {
        parse_for_stmt(myResult);
        //myResult->type = IF_STMT;
        //myResult->next = myResult->if_stmt->false_branch;

    }else
    {
        syntax_error();
    }
    return myResult;
}
struct AssignmentStatement* Parser::parse_assign_stmt()
{
    //assign_stmt ->  ID EQUAL primary SEMICOLON
    //assign_stmt ->  ID EQUAL expr SEMICOLON
    //assign_stmt ->  ID EQUAL primary op primary SEMICOLON
    // primary, ID,NUM
    // expr ID, NUM
    // first of op PLUS, MINUS, MULT, DIV
    struct AssignmentStatement* myResult = new struct AssignmentStatement;
    /*
    myResult->left_hand_side = NULL;
    myResult->op = OPERATOR_NONE;
    myResult->operand1 = NULL;
    myResult->operand2 = NULL;*/
    
    myResult->left_hand_side = lookup(expect(ID).lexeme);
    //expect(ID);
    expect(EQUAL);
    TokenType pT = doublePeek().token_type;
    if(pT == PLUS || pT == MINUS || pT == MULT || pT == DIV)
    {
        parse_expr(myResult);
    }else
    {
        myResult->operand1 = parse_primary();
        myResult->op = OPERATOR_NONE;
        myResult->operand2 = NULL;
    }
    expect(SEMICOLON);
    return myResult;

}
void Parser::parse_expr(AssignmentStatement * myAssign)
{
    //expr ->  primary op primary
    //struct ValueNode* myResult = new struct ValueNode;
    myAssign->operand1 = parse_primary();
    //parse_primary();
    parse_op(myAssign);
    //parse_op();
    myAssign->operand2 = parse_primary();
    //parse_primary();

}
struct ValueNode* Parser::parse_primary()
{
    //primary ->  ID | NUM
    struct ValueNode* myResult = new struct ValueNode;
    
    Token t = peek();
    if(t.token_type == ID)
    {
        myResult = lookup(expect(ID).lexeme);
        
    }else if(t.token_type == NUM)
    {
        myResult->value = myString(expect(NUM).lexeme);
    }else
    {
        syntax_error();
    }
    return myResult;

}
void Parser::parse_op(AssignmentStatement* newPass)
{
    //op ->  PLUS | MINUS | MULT | DIV
    Token t = peek();
    if(t.token_type == PLUS)
    {
        expect(PLUS);
        newPass->op = OPERATOR_PLUS;
    }else if(t.token_type == MINUS)
    {
        expect(MINUS);
        newPass->op = OPERATOR_MINUS;
    }else if(t.token_type == MULT)
    {
        expect(MULT);
        newPass->op = OPERATOR_MULT;
    }else if(t.token_type == DIV)
    {
        expect(DIV);
        newPass->op = OPERATOR_DIV;
    }else
    {
        syntax_error();
    }

}
struct PrintStatement* Parser::parse_print_stmt()
{
    //print_stmt ->  print ID SEMICOLON
    struct PrintStatement * myResult = new struct PrintStatement;
    expect(PRINT);
    myResult->id = lookup(expect(ID).lexeme);
    expect(SEMICOLON);
    return myResult;
}
void Parser::parse_while_stmt(StatementNode* passIn)
{
    //while_stmt -> WHILE condition body
    struct IfStatement * myReturn = new struct IfStatement;
    
    // create a no op node
    struct StatementNode* noOp = new struct StatementNode;
    noOp->type = NOOP_STMT;
    
    struct StatementNode * gt = new struct StatementNode;
    gt->type = GOTO_STMT;
    gt->next = NULL;
    
    struct GotoStatement * gtNode = new struct GotoStatement;
    gtNode->target = passIn;
    
    gt->goto_stmt = gtNode;

    expect(WHILE);
    parse_condition(myReturn);
    myReturn->true_branch = parse_body();

    passIn->if_stmt = myReturn; // flip?

    struct StatementNode* iterator;
    iterator = myReturn->true_branch;
    
    while(iterator->next != NULL)
    {
        iterator = iterator->next;
    }
    if(iterator->next == NULL)
    {
        iterator->next = gt;
    }
    myReturn->false_branch = noOp;

}
struct IfStatement* Parser::parse_if_stmt()
{
    //if_stmt -> IF condition body
 
    struct IfStatement * myReturn = new struct IfStatement;
    
    // create a no op node
    struct StatementNode* noOp = new struct StatementNode;
    noOp->type = NOOP_STMT;
    
    expect(IF);
    parse_condition(myReturn);
    myReturn->true_branch = parse_body();
    
    struct StatementNode* iterator;
    iterator = myReturn->true_branch;
    
    while(iterator->next != NULL) // bad access here when iterator next is null
    {
        iterator = iterator->next; // error could be here
    }
    if(iterator->next == NULL)
    {
        iterator->next = noOp;
    }
    myReturn->false_branch = noOp;
    //myReturn->next = noOp;
    return myReturn;
    
}
void Parser::parse_condition(IfStatement* myIf)
{
    //condition ->  primary relop primary
    myIf->condition_operand1 = parse_primary();
    //parse_primary();
    parse_relop(myIf);
    //parse_relop();
    myIf->condition_operand2 = parse_primary();
    //parse_primary();

}
void Parser::parse_relop(IfStatement* newPass)
{
    //relop → GREATER | LESS | NOTEQUAL
    Token t = peek();
    if(t.token_type == GREATER)
    {
        expect(GREATER);
        newPass->condition_op = CONDITION_GREATER;
    }else if (t.token_type == LESS)
    {
        expect(LESS);
        newPass->condition_op = CONDITION_LESS;
    }else if(t.token_type == NOTEQUAL)
    {
        expect(NOTEQUAL);
        newPass->condition_op = CONDITION_NOTEQUAL;
    }else
        syntax_error();
}
struct StatementNode* Parser::parse_switch_stmt()
{
    //switch_stmt -> SWITCH ID LBRACE case_list RBRACE
    //switch_stmt -> SWITCH ID LBRACE case_list default_case RBRACE
    //struct IfStatement *myIf = new struct IfStatement;
    
    struct StatementNode* noOp = new struct StatementNode;
    noOp->type = NOOP_STMT;
    
    struct ValueNode* myVal = new struct ValueNode;
    
    expect(SWITCH);
    myVal = lookup(expect(ID).lexeme); // save in value node
    
    //expect(ID);
    
    expect(LBRACE);
    struct StatementNode* body;
    body = parse_case_list();
    
    struct StatementNode* iterator;
  
    struct StatementNode* second_iterator;
    //second_iterator = body->if_stmt->true_branch;
 

    struct StatementNode* temp;
    iterator = body;
    
    // APPEND
    while(iterator != NULL)
    {
        second_iterator = iterator->if_stmt->true_branch;
        while(second_iterator->next != NULL)
        {
            second_iterator = second_iterator->next;
        }
        second_iterator->next = noOp;
        iterator->if_stmt->false_branch = noOp;
        
        iterator = iterator->next;
    }
    // APPEND FINISH

    
    iterator = body;
    struct StatementNode* prev = NULL;
    
    // UNRAVEL

    while(iterator->next != NULL)
    {
        prev = iterator;
        iterator = iterator->next;
    }
    if(prev != NULL && iterator->if_stmt->condition_operand2 == NULL)
    {
        prev->next = NULL;
        prev->if_stmt->false_branch = iterator->if_stmt->true_branch;
    }

    // UNRAVEL FINSIHED
    
    //LINK
    iterator = body;
    while(iterator->next != NULL)
    {
        iterator->if_stmt->false_branch = iterator->next;
        iterator = iterator->next;
    }
    // LINK FINISHED
    
    
    // SWAP
    iterator = body;

    while(iterator != NULL)
    {
        temp = iterator->if_stmt->true_branch;
        iterator->if_stmt->true_branch = iterator->if_stmt->false_branch;
        iterator->if_stmt->false_branch = temp;
        
        iterator->if_stmt->condition_op = CONDITION_NOTEQUAL;
        iterator->if_stmt->condition_operand1 = myVal;
        iterator = iterator->next;
    }
    
    // SWAP FINISHED
    expect(RBRACE);
    body->next = noOp;
    return body;
}
void Parser::parse_for_stmt(StatementNode* myPass)
{
    //for_stmt -> FOR LPAREN assign_stmt condition SEMICOLON assign_stmt RPAREN body
    // assign stmt, while stmt
    // turns it into an assignment where its next instruction is an if
    // runs the body, append assignment statment and append goto
    //struct AssignmentStatement * myReturn = new struct AssignmentStatement;
    myPass->type = ASSIGN_STMT;
    //cout << "1" << endl;
    
    struct StatementNode * mySecondReturn = new struct StatementNode;
    mySecondReturn->type = ASSIGN_STMT;
    
    struct StatementNode * myIf = new struct StatementNode;
    myIf->type = IF_STMT;
    
    myIf->if_stmt = new struct IfStatement;
    
    struct StatementNode * gt = new struct StatementNode;
    gt->type = GOTO_STMT;
    gt->next = NULL;
    
    struct GotoStatement * gtNode = new struct GotoStatement;
    gtNode->target = myIf;
    
    gt->goto_stmt = gtNode;
    
    // create a no op node
    struct StatementNode* noOp = new struct StatementNode;
    noOp->type = NOOP_STMT;
    
    //cout << "2" << endl;

    expect(FOR);
    expect(LPAREN);
    myPass->assign_stmt = parse_assign_stmt();
    parse_condition(myIf->if_stmt);
    expect(SEMICOLON);
    mySecondReturn->assign_stmt = parse_assign_stmt();
    expect(RPAREN);
    
    //cout << "3" << endl;
    
    myPass->next = myIf;
    myIf->if_stmt->true_branch = parse_body();
    //myPass->if_stmt = myIf->if_stmt; // flip?
    
    //cout << "4" << endl;
    

    struct StatementNode* iterator;
    iterator = myIf->if_stmt->true_branch;
    //cout << "5" << endl;
    
    while(iterator->next != NULL) // bad access here when iterator next is null
    {
        iterator = iterator->next; // error could be here
    }
    if(iterator->next == NULL)
    {
        iterator->next = mySecondReturn;
        mySecondReturn->next = gt;
    }
    myIf->if_stmt->false_branch = noOp;
    myIf->next = noOp;
    //cout << "6" << endl;


}
struct StatementNode* Parser::parse_case_list()
{
    // have parse list create this node
    //case_list -> case case_list | case

    struct StatementNode* newStat = parse_case();
    //parse_case();
    if(peek().token_type == CASE)
    {
       newStat->next = parse_case_list();
    }else if(peek().token_type == DEFAULT)
    {
        newStat->next = parse_default_case();
    }
    return newStat;
   

}
struct StatementNode* Parser::parse_case()
{

    //case -> CASE NUM COLON body
    struct ValueNode* numHolder = new struct ValueNode;
    struct StatementNode *myCase = new struct StatementNode;
    myCase->type = IF_STMT;
    myCase->if_stmt = new struct IfStatement;
    expect(CASE);
    numHolder->value = myString(expect(NUM).lexeme);
    myCase->if_stmt->condition_operand2 = numHolder;
    //expect(NUM);
    expect(COLON);
    myCase->if_stmt->true_branch = parse_body();
    return myCase;
    


}
struct StatementNode* Parser::parse_default_case()
{
    //default_case -> DEFAULT COLON body
    //myPass->type = IF_STMT;
    //myPass->if_stmt = new struct IfStatement;
    struct StatementNode* myCase = new struct StatementNode;
    myCase->type = IF_STMT;
    myCase->if_stmt = new struct IfStatement;
    expect(DEFAULT);
    expect(COLON);
    myCase->if_stmt->true_branch = parse_body();
    myCase->if_stmt->condition_operand2 = NULL;
    
    //parse_body();
    struct StatementNode* noOp = new struct StatementNode;
    noOp->type = NOOP_STMT;
    
    struct StatementNode* iterator;
    iterator = myCase->if_stmt->true_branch;
    
    while(iterator->next != NULL)
    {
        iterator = iterator->next;
    }
    iterator->next = noOp;
    return myCase;
}
