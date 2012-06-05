#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"

#define TRUE 1
#define FALSE 0

#define PRINTSTMT 0
#define ASSIGNSTMT 1
#define WHILESTMT 2
#define IFSTMT 3
#define GOTOSTMT 4
#define NOOPSTMT 5

#define KEYWORDS 14
#define RESERVED 38
#define VAR 1
#define BEGIN 2
#define END 3
#define ASSIGN 4 
#define IF 5
#define WHILE 6 
#define DO 7
#define THEN 8
#define PRINT 9
#define INT 10
#define REAL 11
#define STRING 12
#define BOOLEAN 13
#define TYPE 14
#define PLUS 15
#define MINUS 16
#define DIV 17
#define MULT 18
#define EQUAL 19
#define COLON 20 
#define COMMA 21
#define SEMICOLON 22
#define LBRAC 23
#define RBRAC 24
#define LPAREN 25
#define RPAREN 26
#define NOTEQUAL 27
#define GREATER 28
#define LESS 29
#define LTEQ 30
#define GTEQ 31
#define DOT 32
#define ID 33
#define NUM 34
#define REALNUM 35
#define ERROR 36
#define LBRACE 37
#define RBRACE 38
#define NOOP 39

//------------------- reserved words and token strings -----------------------
char *reserved[] = 
	{	"",
		"VAR", 
		"BEGIN", 
		"END", 
		"ASSIGN", 
		"IF", 
		"WHILE", 
		"DO", 
		"THEN", 
		"print", 
        "INT",
		"REAL",
		"STRING",
		"BOOLEAN",
		"TYPE",
		"+",
		"-", 
		"/", 
		"*", 
		"=", 
		":", 
		",", 
		";", 
		"[", 
		"]", 
		"(", 
		")", 
		"<>", 
		">", 
		"<",
		"<=",
		">=",
        ".",
        "ID",
        "NUM",
        "REALNUM",		
        "ERROR",
		"{",
		"}"
		};

int printToken(int ttype)
{
   if (ttype <= RESERVED)
   {   printf("%s\n",reserved[ttype]);
       return 1;
   } else
       return 0; 
}
//---------------------------------------------------------

//---------------------------------------------------------
// Global Variables associated with the next input token
#define MAX_TOKEN_LENGTH 100

char token[MAX_TOKEN_LENGTH];      // token string
int  ttype;                        // token type
int  activeToken = FALSE;                  
int  tokenLength;

struct varNode *symbol_head = NULL;
struct varNode *symbol_tail = NULL;
struct varNode *symbol;
struct varNode *newSym;

struct statementNode *stmt_head = NULL;
struct statementNode *stmt_tail = NULL;

int line_no = 1;
int typeCount = 15;

//----------------------------------------------------------
int skipSpace()
{
   char c;

   c = getchar(); 
   line_no += (c == '\n');
   while (!feof(stdin) && isspace(c))
   {    c = getchar(); 
        line_no += (c == '\n');
   }

   // return character to input buffer if eof is not reached
   if (!feof(stdin)) 
        ungetc(c,stdin);
}

int isKeyword(char *s)
{
     int i;

     for (i = 1; i <= KEYWORDS; i++)
	if (strcmp(reserved[i],s) == 0)
	   return i;
     return FALSE;
}

// ungetToken() simply sets a flag so that when getToken() is called
// the old ttype is returned and the old token is not overwritten 
// NOTE: BETWEEN ANY TWO SEPARATE CALLS TO ungetToken() THERE MUST BE
// AT LEAST ONE CALL TO getToken()
// CALLING TWO ungetToken() WILL NOT UNGET TWO TOKENS  
void ungetToken()
{
    activeToken = TRUE;
}

int scan_number()
{
	char c;
	
	c = getchar();
	if (isdigit(c))
	{	// First collect leading digits before dot
		// 0 is a nNUM by itself
		if (c == '0')	      
		{	token[tokenLength] = c;
			tokenLength++;
			token[tokenLength] = '\0';
		} else
		{	while (isdigit(c))
			{	token[tokenLength] = c;
				tokenLength++;;
				c = getchar();
			}
			ungetc(c,stdin);
			token[tokenLength] = '\0';
		}

		// Check if leading digits are integer part of a REALNUM
		c = getchar();
		if (c == '.')
		{	c = getchar();
			if (isdigit(c))
			{	token[tokenLength] = '.';
				tokenLength++;
				while (isdigit(c))
				{	token[tokenLength] = c;
					tokenLength++;
					c = getchar();
				}
				token[tokenLength] = '\0';
				if (!feof(stdin)) 
					ungetc(c,stdin);
				return REALNUM;
			} else
			{	ungetc(c, stdin);    // note that ungetc returns characters on a stack, so we first
				c = '.';             // return the second character and set c to '.' and return c again
				ungetc(c,stdin);				                                 
				return  NUM;         
                        }
		} else
		{	ungetc(c, stdin);
			return NUM;
		}
	} else
		return ERROR;   
}

int scan_id_or_keyword()
{
	int ttype;
	char c;

	c = getchar();
	if (isalpha(c))
	{	while (isalnum(c))
		{	token[tokenLength] = c;
			tokenLength++;
			c = getchar();
		}
		if (!feof(stdin)) 
			ungetc(c,stdin); 
             
		token[tokenLength] = '\0';		                
		ttype = isKeyword(token); 
		if (ttype == 0) 
			ttype = ID;
		return ttype;
	} else
		return ERROR;
}          

int getToken()
{	char c;
 
       if (activeToken)
       { activeToken = FALSE;
         return ttype;
       }   // we do not need an else because the function returns in the body 
           // of the if
   
       skipSpace();   
       tokenLength = 0;
       c = getchar();
       switch (c)
       {   case '.': return DOT;
           case '+': return PLUS;
           case '-': return MINUS;
           case '/': return DIV;
           case '*': return MULT;
           case '=': return EQUAL;
           case ':': return COLON;
           case ',': return COMMA;
           case ';': return SEMICOLON;
           case '[': return LBRAC;
           case ']': return RBRAC;
           case '(': return LPAREN;
           case ')': return RPAREN;
	   	   case '{': return LBRACE;
	   	   case '}': return RBRACE;
           case '<':
                      c = getchar();
                       if (c == '=')
                          return LTEQ;
                       else
                       if (c == '>')
                          return NOTEQUAL;
                       else
                       {
                          ungetc(c,stdin);
                          return LESS;
                       }
           case '>': 
                        c = getchar();
                        if (c == '=')
                           return GTEQ;
                        else
                        {
                           ungetc(c, stdin);
                           return GREATER;
                        }
           
           default :
			if (isdigit(c))
			{	ungetc(c,stdin);
				return scan_number();
			}
			else
			if (isalpha(c))
			{	ungetc(c,stdin);
				return scan_id_or_keyword();
			}
			else
			if (c == EOF)
				return EOF;
			else
				return ERROR;
	}
}

/*----------------
Other functions
-----------*/

void varAdd(struct varNode* v, struct varNode** list)
{
	struct varNode* var;
    
	var = *list;
	
	if (var == NULL)
	
	{	
		
		*list = v;
		
		(*list)->next = NULL;

	} 
	
	else
	
	{	
		
		while (var->next != NULL) 
		
		{
			
			var = var->next;
		
		}

		
		var->next = v;
	
	}
}

int idAdd(struct id_listNode* i, struct id_listNode** list)

{
	
	struct id_listNode* id;

	
	id = *list;
	
	
	if (id == NULL)
	
	{
		
		*list = i;
	
	}
	
	else
	
	{	
		
		while (id->id_list != NULL) 
		
		{
			
			id = id->id_list;
		
		}
		
		
			id->id_list = i;
	
	}

}



struct varNode* lookupVar(char* v, struct varNode* list)

{
	
	while (list != NULL)
	
	{
		
		if (strcmp(list->vID, v) == 0)
		
		{
			
			return list;
		
		}
		
		
		list = list->next;
	
	}

	
	
	return NULL;

}



struct varNode* findVar(char* t, struct symbolTableNode* sym)

{
	
	struct varNode* list;
	
	struct varNode* temp;
	

	
	list = sym->var;
		
	temp =  lookupVar(t, list);
	
	
	return temp;

}

void output()

{
	
	struct statementNode *stmtNode;
  	stmtNode = stmt_head;

	
  	while(stmtNode != NULL)
  	{	
		if(stmtNode->stmtType == ASSIGNSTMT) 
	
		{
		
			outputAssignment(stmtNode->assignSt);
	
		
		} 

		else if(stmtNode->stmtType == PRINTSTMT) 
	
		{	
		
			outputPrint(stmtNode->printSt);
		
		
		}
	
		else if(stmtNode->stmtType == WHILESTMT) 
	
		{
		
			outputWhile(stmtNode->whileSt);
		
		
		} 
	
		else if(stmtNode->stmtType == IFSTMT) 
	
		{
		
			if(stmtNode->ifSt != NULL)
		
			{
			
				outputIf(stmtNode->ifSt);
			
			}
	
		} 
	
	
		stmtNode = stmtNode->next;
  	}
}

int outputPrint(struct printStatement* p)

{
	
	printf("%d\n",p->id->vValue);
}



int outputIf(struct ifStatement* i)

{
	
	if(i->condition->op1 != NULL)
	
	{
	  
		if(i->condition->operator != 0)
	  
		{
		
			if(outputCondition(i->condition))
		
			{
			
				output(i->condition->trueBranch);
		
			}
	  
		}
	  
		else
	  
		{
		
			i->stmt_list = i->condition->falseBranch;
	  
		}
	
	}

}



int outputWhile(struct whileStatement* w)

{
	
	if(w->condition->op1 != NULL)
	
	{
	  
		if(w->condition->operator != 0)
	  
		{
		
			if(outputCondition(w->condition))
		
			{
			
				output(w->condition->trueBranch);
		
			}
	  
		}
	  
		else
	  
		{
		
			w->stmt_list = w->condition->falseBranch;
	  
		}
	
	}

}



int outputAssignment(struct assignNode* a)

{
	
	if (a->lvalue != NULL)
	
	{		
		if (a->oper != 0)
		
		{
		
			outputExpr(a);
		
		}
		
		else 
		
		{
	 		
			if(a->op1 != NULL)
			
			{
  
             
				a->lvalue->vValue = a->op1->ival;
			
			}
		
		}
	
	}

}



int outputCondition(struct conditionNode* condition)

{
	
	if (condition->operator == GREATER)
	
	{
		
		if(condition->op1->ival > condition->op2->ival)
		
		{
			
			return TRUE;
		
		} 
		
		else
		
		{
			
			return FALSE;
		
		}
	
	}

}

int outputExpr(struct assignNode* assign)

{	
	
	if (assign->oper == PLUS)
	
	{
		
		printf("%d PLUS %d\n", assign->op1->ival, assign->op2->ival);
		assign->lvalue->vValue = assign->op1->ival + assign->op2->ival;

		//assign->lvalue->vValue = assign->lvalue->vValue + assign->op2->ival;

		//return assign->lvalue->vValue;
	}
	
	else if ((assign->oper == MINUS))
	
	{	
	
		printf("%d MINUS %d\n", assign->op1->ival, assign->op2->ival);
		assign->lvalue->vValue = assign->op1->ival - assign->op2->ival;
	
		//return assign->lvalue->vValue;
	}
	
	else if ((assign->oper == DIV))
	
	{	
		
		assign->lvalue->vValue = assign->op1->ival / assign->op2->ival;
	
		//return assign->lvalue->vValue;
	}
	
	else if ((assign->oper == MULT))
	
	{	
		
		assign->lvalue->vValue = assign->op1->ival * assign->op2->ival;

		//return assign->lvalue->vValue;
	}
	
	else
	
	{
		
		assign->lvalue->vValue = 0;
		//return NULL;
	
	}

}

/*--------------------------------------------------------------------
  LINKED LIST FUNCTIONS
---------------------------------------------------------------------*/
struct varNode* symSearch(char* id)
{
	symbol = symbol_head;

    while(symbol != NULL)
    {
        if(strcmp(symbol->vID, id) == 0)
        {
            //n = symbol->sID;
            //n = 1;
            return symbol;
        }
        
		symbol = symbol->next;
    }
    
	return NULL;
}

void symAdd(char *id)
{
	symbol = make_var();

    if(!symbol_head)
	{
        symbol_head = symbol;
	}
    else
	{
        symbol_tail->next = symbol;
	}

	symbol_tail = symbol;
}

/*--------------------------------------------------------------------
  CREATING PARSE TREE NODE
---------------------------------------------------------------------*/
struct programNode* make_programNode()
{	
	return (struct programNode*) malloc(sizeof(struct programNode));
}

struct declNode* make_declNode()
{
	return (struct declNode*) malloc(sizeof(struct declNode));
}

struct var_declNode* make_var_decl()
{
	return (struct var_declNode*) malloc(sizeof(struct var_declNode));
}

struct statementNode* make_stmt_list()
{
	return (struct statementNode*) malloc(sizeof(struct statementNode));
}

struct bodyNode* make_body()
{
	return (struct bodyNode*) malloc(sizeof(struct bodyNode));
}

struct id_listNode* make_id_list()
{
	struct id_listNode* idlist;

	
	idlist =  ((struct id_listNode *) malloc(sizeof(struct id_listNode)));
	
	idlist->id = (char *) malloc(strlen(token)+1);
	
	strcpy(idlist->id, token);
	
	idlist->id_list = NULL;
	
	return idlist;
}

struct varNode*	make_var()
{
	
	struct varNode* var;
 
	
	var =  ((struct varNode *) malloc(sizeof(struct varNode)));	
	
	var->vID = (char *) malloc(strlen(token)+1); 
	
	strcpy(var->vID, token);	
	
	var->vType = INT;
	
	var->vValue = 0; 
	
	var->next = NULL;
		
	return var;
}

struct statementNode* make_stmt(int sType)
{
	struct statementNode* stmt;
	
	stmt = ((struct statementNode *) malloc(sizeof(struct statementNode)));
	
	stmt->stmtType = sType;
	stmt->assignSt = NULL;
	stmt->printSt = NULL;
	stmt->whileSt = NULL;
	stmt->ifSt = NULL;
	stmt->gotoSt = NULL;
	stmt->next = NULL;

	return stmt;
}

struct assignNode* make_assign(struct varNode* var)
{
	struct assignNode* assign;

	
	assign = ((struct assignNode *) malloc(sizeof(struct assignNode)));
	
	assign->lvalue = var;
	
	assign->op1 = NULL;
	
	assign->op2 = NULL;
	
	assign->oper = 0;
		
	return assign;
}

struct conditionNode* make_condition()
{
	struct conditionNode* cond;
	
	cond = ((struct conditionNode*) malloc(sizeof(struct conditionNode)));

	cond->op1 = NULL;
	cond->op2 = NULL;
	cond->operator = 0;
	cond->trueBranch = NULL;
	cond->falseBranch = NULL;

	return cond;
}

struct ifStatement* make_ifSt()
{
	struct ifStatement* ifSt;
	
	ifSt = ((struct ifStatement*) malloc(sizeof(struct ifStatement)));
	ifSt->condition = NULL;
	ifSt->stmt_list = NULL;
	return ifSt;
}

struct gotoStatement* make_gotoSt()
{
	return (struct gotoStatement*) malloc(sizeof(struct gotoStatement));
}

struct whileStatement* make_whileStatement()
{
	struct whileStatement* whileSt;

	whileSt = ((struct whileStatement *) malloc(sizeof(struct whileStatement)));
	whileSt->condition = NULL;
	whileSt-> stmt_list = NULL;
	return whileSt;
}

struct printStatement* make_printSt(struct varNode* i)
{
	struct printStatement* printSt;

	printSt = ((struct printStatement *) malloc(sizeof(struct printStatement)));
	printSt->id = i; //get id i->vID?

	return printSt;
}

struct primaryNode*	make_primary()
{
	return (struct primaryNode*) malloc(sizeof(struct primaryNode));
}

/*--------------------------------------------------------------------
  PARSING AND BUILDING PARSE TREE
---------------------------------------------------------------------*/
struct programNode* program()
{	
	struct programNode* prog;

	prog = make_programNode();
	ttype = getToken();
	if (ttype == VAR)
	{	
		ungetToken();  
		prog->decl = decl();
		prog->body = body();
		return prog;
	} 
	else
	{	
		return NULL;
	}
}

struct declNode* decl()
{
	struct declNode* decl;
	//struct var_declNode* varDecl;
	
	decl = make_declNode();
	//varDecl = make_var_decl();
	
	decl->var_decl = NULL;
	
	ttype = getToken();
	
	if(ttype == VAR)
	{
		decl->var_decl = var_decl();
	}
	
	return decl;
}

struct bodyNode* body()
{
	struct bodyNode* bod;

	ttype = getToken();
	if (ttype == LBRACE)
	{	
		bod = make_body();
		bod->stmt_list = stmt_list();

		//ttype = getToken();
		printf("ttype: %d\n",ttype);
		if (ttype == RBRACE)
			return bod;
		else
		{	
			return NULL;
		}
	} 
	else
	{	
		return NULL; 
	}
}

struct var_declNode* var_decl()
{
	struct var_declNode* varDecl;
	struct id_listNode* idList;

	varDecl = make_var_decl();
	idList = make_id_list();

	int i = 0;
	varDecl->id_list = NULL;

	ttype = getToken();
	if (ttype == ID)
	{	
		ungetToken();
		varDecl->id_list = id_list();

		ttype = getToken();
		if (ttype == SEMICOLON)
		{	//printf("ttype %d\n", ttype);

			return varDecl;
		}
	}

	return varDecl;
}

struct id_listNode* id_list()
{
	struct id_listNode* idList;
	
	idList = make_id_list();

	ttype = getToken();
	if (ttype == ID)
	{	
		idList->id = (char*) malloc(tokenLength+1);
		strcpy(idList->id, token);
		symAdd(idList->id);
		
		ttype = getToken();
		if (ttype == COMMA)
		{
			idList->id_list = id_list();
			return idList;
			
		} 
		else
		{	
			ungetToken();
			return idList;
		}
	} 
	else
	{	
		return NULL;
	}
}

struct statementNode* stmt()
{
	struct statementNode* stm;
	struct printStatement* prt;
	struct varNode* temp;

	ttype = getToken();
	
	if (ttype == ID) // assign_stmt
	{	printf("stmt ID: %d\n",ttype);
		stm = make_stmt(ASSIGNSTMT);
		
		printf("token: %s\n",token);
		stm->assignSt = assignment(token);
		stm->stmtType = ASSIGNSTMT;

		ttype = getToken();
		if (ttype == SEMICOLON)
		{	printf("SEMICOLON: %d\n",ttype);
			return stm;
		}
		else
		{	
			return NULL;
		}
	} 
	else if (ttype == WHILE) // while_stmt
	{	
		ungetToken();
		stm = make_stmt(WHILESTMT);
		stm->whileSt = whileStatement();
		stm->stmtType = WHILESTMT;

		ttype = getToken();
		if(ttype = RBRACE)
		{
			return stm;
		}
		else
		{
			return NULL;
		}
	} 
	else if (ttype == IF)
	{
		ungetToken();
		stm = make_stmt(IFSTMT);
		stm->ifSt = ifSt();
		stm->stmtType = IFSTMT;

		ttype = getToken();
		if(ttype = RBRACE)
		{
			return stm;
		}
		else
		{
			return NULL;
		}
	}
	else if (ttype == PRINT)
	{
		printf("stmt PRINT: %d\n",ttype);
		stm = make_stmt(PRINTSTMT);

		ttype = getToken();
		printf("token: %s\n",token);
		temp = symSearch(token);
		stm->stmtType = PRINTSTMT;

		if(temp != NULL)
		{	
			prt = make_printSt(temp);
			stm->printSt = prt;
			
			ttype = getToken();
			if(ttype == SEMICOLON)
			{
				return stm;
			}
			else
			{
				return NULL;
			}
		}
		else
		{
			return NULL;
		}

		//return stm;
	}
	else 
	{
		return NULL;
	}	
}

struct statementNode* stmt_list()
{ 
	struct statementNode* st; // statement
	struct statementNode* st1; // statement list
	struct statementNode* no_op;
	struct gotoStatement* gt;

	st = stmt();
	st1 = NULL;
	no_op = NULL; //call makestmt?
	
	ttype = getToken();
	if (!stmt_head && (ttype == ID)|(ttype == WHILE)|(ttype == PRINT)|(ttype == IF))
	{	
		ungetToken();
		st1 = stmt_list();
		
		if (st->stmtType == IFSTMT)
		{
			st->ifSt->stmt_list->next = no_op;
			st->ifSt->condition->falseBranch = no_op;
			st->ifSt->condition->trueBranch = st->ifSt->stmt_list; //true? 
			st->next = no_op; //append no_op to st?
			no_op->next = st1; 
		}
		if(st->stmtType == WHILE)
		{
			gt->target = st;
			st->whileSt->stmt_list->next = gt->target;
			st->whileSt->condition->falseBranch = no_op;
			st->whileSt->condition->trueBranch = st->whileSt->stmt_list;
			st->next = no_op;
			no_op->next = st1;
		}
		
		st->next = st1; //stmt head
		stmt_head = st;
		return st;
	}
	else
	{
		return st;
	}
}

/*struct varNode* var()
{
	struct varNode* var;
	var = make_var();

	ttype = getToken();
	if (ttype == VAR)
	{	// no need to ungetToken() 
		var->vID = var_decl();  
		return var;
	} 
	else
	{
		return NULL;
	}
}

int assign_list()

{
   
	ttype = getToken();

   
	if ((ttype == ID)|(ttype == NUM))
   
	{  
	  
		ungetToken();
      
		assign_list();
   
	} 
   
	else
   
	{ 
	   
		ungetToken();
   
	}

}



int assignVal()

{
	
	ttype = getToken();
   
	if (ttype == ASSIGN)
   
	{
     
		//symbol->stmt = stmt_list();
	   
		stmt_list();
   
	}   
   
	else
   
	{
  		
		return NULL;
   
	}

}*/

struct assignNode* assignment() 

{
	
	struct assignNode* assign;
	

	struct varNode* var;
	
	var = symSearch(token);

	
	if(var != NULL) 
	
	{
		
		printf("var: %s\n",var->vID);
	
	
		assign = make_assign(var);
		
		ttype = getToken();



		if(ttype == EQUAL) 
		
		{
				
			printf("EQUAL: %d\n",ttype);	
			assign = expr(assign);
	
			printf("assign->op1->ival %d\n", assign->op1->ival);
			return assign;


		
		} 
		
		else 
		
		{
			
			return NULL;
		}
	
	} 
	
	else 
	
	{
		
		return NULL;
	
	}

}

struct assignNode* expr(struct assignNode* assign)
{
	struct varNode* oper1;
	struct varNode* oper2;
	int oper;

	ttype = getToken();
	if (ttype == NUM)
	{	
		printf("NUM: %d\n",ttype);
		assign->op1 = primary();

		ttype = getToken();
		if ((ttype == PLUS) | (ttype == MINUS) | (ttype == MULT) | (ttype == DIV))
		{	
			assign->oper = ttype; //op is set + - * /
			assign->op2 = primary();
		}
		
		ungetToken();
		return assign;
	} 
	else if (ttype == ID)
	{
		printf("ID: %d\n",ttype);
		//oper1 = symSearch(token);

		/*if(oper1 != NULL)
		//{
			//assign->op1->id = oper1->vID;*/
			assign->op1 = primary();
			printf("ASSIGN ID %s\n", assign->op1->id);
			
			ttype = getToken();
			printf("PLUS: %d\n",ttype);
			if ((ttype == PLUS) | (ttype == MINUS) | (ttype == MULT) | (ttype == DIV))
			{
				assign->oper = ttype;
				
				ttype = getToken();
				if(ttype == ID) 
				
				{
	printf("ID: %d\n",ttype);				
					/*oper2 = symSearch(token);
					
					
					//printf("oper2: %s\n",oper2->vID);
					//if(oper2 != NULL) 
					
					//{
						
						//assign->op2 = oper2;
						//assign->op2->id = oper1->vID;
*/							
						
						/*ttype = getToken();
						
						printf("SEMICOLON: %d\n",ttype);
						if(ttype == SEMICOLON)
						
						{
		
							//ungetToken();
*/
							assign->op2 = primary();
							return assign;						
						/*}
						
						else
						
						{
							
							return NULL;
						
						}
	*/				
					/*} 
					
					//else 
					
					//{
						
					//	return NULL;
					
					//}
*/				
				} 
				
				else
				
				{
					
					ungetToken();
				
				}
		
			} 
			
			else 
			
			{
				
				return NULL;
			
			}
		
		//}
	
	 }
}

struct conditionNode* condition()
{
	struct conditionNode* cNode;
	struct varNode* op1;
	struct varNode* op2;
	struct statementNode* tBranch;
	struct statementNode* fBranch;
	int op;

	cNode = make_condition();

	ttype = getToken();
	if ((ttype == ID)|(ttype == NUM))
	{
		ungetToken(); //ungetToken since it still be parsed
		cNode->op1 = primary(); //left operand of a condition is a primary

		ttype = getToken();
		if ((ttype == GREATER)|(ttype == GTEQ)|(ttype == LESS)
			|(ttype == NOTEQUAL)|(ttype == LTEQ))
		{
			cNode->operator = ttype; //relop is set to >, <, etc.
		
			ttype = getToken();
			if ((ttype == ID)|(ttype == NUM))
			{
				ungetToken(); //ungetToken since it still be parsed
				cNode->op2 = primary(); //right operand of a condition is a primary
			
				ttype = getToken();
				if(ttype == RPAREN)
				{
					ttype = getToken();
					if(ttype == THEN)
					{
						cNode->trueBranch = stmt_list();
						return cNode;
					}
					else
					{
						return NULL;
					}
				}
				else
				{
					return NULL;
				}
			}
			else
			{
				return NULL;
			}
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

struct ifStatement* ifSt()
{
	struct ifStatement* ifSt;
	ifSt = make_ifSt();

	ttype = getToken();
	if (ttype == IF)
	{
		ifSt->condition = condition();
		ttype = getToken();

		if(ttype == LBRACE)
		{
			ungetToken();
			ifSt->stmt_list = body();
			return ifSt;
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

struct gotoStatement* gotoSt()
{
	struct gotoStatement* gt;
	return gt;
}

struct whileStatement* whileStatement()
{
	struct whileStatement* wSt;
	wSt = make_whileStatement();

	ttype = getToken();
	if(ttype == WHILE)
	{
		wSt->condition = condition();
		ttype = getToken();
		if(ttype == LBRACE)
		{
			wSt->stmt_list = body();
			return wSt;
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

struct primaryNode* primary()
{
	struct varNode* op;
	struct primaryNode* pNode;
	pNode = make_primary();

	if ((ttype == ID)|(ttype == NUM))
	{	
		if (ttype == ID)
		{	
			op = symSearch(token);

			if(op == NULL)
			{
				pNode->id = (char *) malloc(tokenLength+1);
				strcpy(pNode->id,token);	//allocating space for ID
				symAdd(pNode->id);
			}
			else
			{
				pNode->id = op->vID;
				pNode->ival = op->vValue;
			}
			
			return pNode;
		}
		else if (ttype == NUM)
		{
			pNode->ival = atoi(token); //convert string to an integer
			return pNode;
		}
		else
		{
			return NULL;
		}
	} 
	else
	{	
		return NULL;
	}
}

main()
{       
	struct programNode* parseTree;
	parseTree = program();
	
	printf("\n\noutput:\n");
	output();
}