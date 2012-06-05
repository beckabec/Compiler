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
Output function
-----------*/

void output()

{
	
	struct statementNode *stmtNode;
	struct statementNode *temp;
  	stmtNode = stmt_head;

  	temp = stmt_head;

	
  	while(stmtNode != NULL)
  	{	
		if(stmtNode->stmtType == ASSIGNSTMT) 
	
		{
		
			if (stmtNode->assignSt->lvalue != NULL)
	
			{		
				if (stmtNode->assignSt->oper != 0)
		
				{
		
					if (stmtNode->assignSt->oper == PLUS)
	
					{
		
						stmtNode->assignSt->lvalue->vValue = stmtNode->assignSt->op1->vValue + stmtNode->assignSt->op2->vValue;

					}
	
					else if ((stmtNode->assignSt->oper == MINUS))
	
					{	
	
						stmtNode->assignSt->lvalue->vValue = stmtNode->assignSt->op1->vValue - stmtNode->assignSt->op2->vValue;
	
					}
	
					else if ((stmtNode->assignSt->oper == DIV))
	
					{	
		
						stmtNode->assignSt->lvalue->vValue = stmtNode->assignSt->op1->vValue / stmtNode->assignSt->op2->vValue;
	
					}
	
					else if ((stmtNode->assignSt->oper == MULT))
	
					{	
		
						stmtNode->assignSt->lvalue->vValue = stmtNode->assignSt->op1->vValue * stmtNode->assignSt->op2->vValue;

					}
	
					else
	
					{
		
						stmtNode->assignSt->lvalue->vValue = 0;
					}
		
				}
		
				else 
		
				{
	 		
					if(stmtNode->assignSt->op1 != NULL)
			
					{
  
             
						stmtNode->assignSt->lvalue->vValue = stmtNode->assignSt->op1->vValue;
			
					}
		
				}

				stmtNode = stmtNode->next;
		 	}

	
		
		} 

		else if(stmtNode->stmtType == PRINTSTMT) 
	
		{	
		
			printf("%d\n",stmtNode->printSt->id->vValue);	
			stmtNode = stmtNode->next;
		
		
		}
	
		else if(stmtNode->stmtType == WHILESTMT) 
	
		{
		
			temp = stmtNode;
			
			if(stmtNode->whileSt->condition->operator == GREATER)
			{
				if(stmtNode->whileSt->condition->op1->vValue > stmtNode->whileSt->condition->op2->vValue)
				{
					stmtNode = stmtNode->whileSt->condition->trueBranch;
				}
				else
				{
					stmtNode = stmtNode->whileSt->condition->falseBranch;
				}
			}
			else if(stmtNode->whileSt->condition->operator == LESS)
			{
				if(stmtNode->whileSt->condition->op1->vValue < stmtNode->whileSt->condition->op2->vValue)
				{
					stmtNode = stmtNode->whileSt->condition->trueBranch;
				}
				else
				{
					stmtNode = stmtNode->whileSt->condition->falseBranch;
				}	
			}
			else if(stmtNode->whileSt->condition->operator == NOTEQUAL)
			{
				if(stmtNode->whileSt->condition->op1->vValue != stmtNode->whileSt->condition->op2->vValue)
				{
					stmtNode = stmtNode->whileSt->condition->trueBranch;
				}
				else
				{
					stmtNode = stmtNode->whileSt->condition->falseBranch;
				}	
			}

		
		
		} 
	
		else if(stmtNode->stmtType == IFSTMT) 
	
		{
					
			temp = stmtNode;
			
			if(stmtNode->ifSt->condition->operator == GREATER)
			{
				if(stmtNode->ifSt->condition->op1->vValue > stmtNode->ifSt->condition->op2->vValue)
				{
					stmtNode = stmtNode->ifSt->condition->trueBranch;
				}
				else
				{
					printf("FALSE!\n");
					stmtNode = stmtNode->ifSt->condition->falseBranch;
				}
			}
			else if(stmtNode->ifSt->condition->operator == LESS)
			{
				if(stmtNode->ifSt->condition->op1->vValue < stmtNode->ifSt->condition->op2->vValue)
				{
					stmtNode = stmtNode->ifSt->condition->trueBranch;
				}
				else
				{
					printf("FALSE!\n");
					stmtNode = stmtNode->ifSt->condition->falseBranch;
				}	
			}
			else if(stmtNode->ifSt->condition->operator == NOTEQUAL)
			{
				if(stmtNode->ifSt->condition->op1->vValue != stmtNode->ifSt->condition->op2->vValue)
				{
					stmtNode = stmtNode->ifSt->condition->trueBranch;
				}
				else
				{
					printf("FALSE!\n");
					stmtNode = stmtNode->ifSt->condition->falseBranch;
				}	
			}
		} 
		else if(stmtNode->stmtType == NOOPSTMT)
		{
			stmtNode = stmtNode->next;
		}
		else if(stmtNode->stmtType ==
GOTOSTMT)
		{
			stmtNode = stmtNode->gotoSt->target;
		}
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

void symAdd(struct varNode* symbol)
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

struct statementNode* findLast(struct statementNode* cur)
{
	if(cur->next == NULL)
	{
		return cur;
	}
	else
	{
		return (findLast(cur->next));
	}
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

struct varNode*	make_var()
{
	
	struct varNode* var;
 
	
	var =  ((struct varNode *) malloc(sizeof(struct varNode)));	

	
	var->vID = (char *) malloc(strlen(token)+1); 
	
	strcpy(var->vID, token);	

	printf("VAR token %s\n", token);
	var->vValue = atoi(token);
	printf("value %d\n", var->vValue);
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
	struct primaryNode* prim;
	prim = ((struct primaryNode*) malloc(sizeof(struct primaryNode)));
	prim->id = (char *) malloc(strlen(token)+1); 
	
	strcpy(prim->id, token);
	prim->ival = atoi(token);;
	return prim;
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
		//ungetToken();  
		prog->decl = decl();
		prog->body = body();

		return prog;
	} 
	else
	{	
		return NULL;
	}
}

struct varNode* decl()
{
	struct varNode* varList;
	varList = ((struct varNode *) malloc(sizeof(struct varNode)));
	
		ttype = getToken();
		if (ttype == ID)
		{	
			symAdd(varList);
		
			ttype = getToken();
			if (ttype == COMMA)
			{	
				varList->next = decl();
				return varList;
			} 
			else
			{	
				//ungetToken();
				return varList;
			}
		} 
		else
		{	
			return NULL;
		}
	
	//return varList;
}

struct bodyNode* body()
{
	struct bodyNode* bod;

	ttype = getToken();
	printf("LBRACE %d\n", ttype);
	if (ttype == LBRACE)
	{	
		bod = make_body();
		bod->stmt_list = stmt_list();
		if (ttype == RBRACE)
		{	
			return bod;
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

struct statementNode* stmt()
{
	struct statementNode* stm;
	struct printStatement* prt;
	struct varNode* temp;

	ttype = getToken();
	
	if (ttype == ID) // assign_stmt
	{	
		stm = make_stmt(ASSIGNSTMT);
		stm->assignSt = assignment();
		//printf("assignSt->op1->vValue: %d\n",stm->assignSt->op1->vValue);
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
		stm = make_stmt(WHILESTMT);
		stm->whileSt = whileStatement();
		stm->stmtType = WHILESTMT;
		
		struct statementNode* noop = make_stmt(NOOPSTMT);
		stm->next = noop;
		
		/*struct statementNode* gt = make_stmt(GOTOSTMT);
		stm->gotoSt->target = gt;*/

		ungetToken();
		if(ttype == RBRACE)
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
		stm = make_stmt(IFSTMT);
		stm->ifSt = ifSt();
		stm->stmtType = IFSTMT;
		
		//struct statementNode* noop = make_stmt(NOOPSTMT);
		stm->next = stm->ifSt->condition->falseBranch;

		ungetToken();
		if(ttype == RBRACE)
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
			printf ("SEMICOLON 22: %d \n", ttype);
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
	struct statementNode* gt;

	st = stmt();
	
	if(ttype == RBRACE)
	{
		getToken();
	}
	
	ttype = getToken();
	printf("TTYPE before %d\n",ttype);
	if (((ttype == ID)||(ttype == WHILE)||(ttype == PRINT)||(ttype == IF)))
	{		
		printf("TTYPE after %d\n",ttype);
		ungetToken();
		st1 = stmt_list();
		
/*		if (st->stmtType == IFSTMT)
		{
			no_op = make_stmt(NOOPSTMT);
			st->next = no_op;

		}
		if(st->stmtType == WHILE)
		{
			gt = make_stmt(GOTOSTMT);
			st->next = gt;
		}*/
		
		findLast(st)->next = st1; //stmt head
		stmt_head = st;
		return st;
	}
	/*else if(ttype == RBRACE)
	{
		ttype = getToken();
		if(ttype != RBRACE)
		{
			//ungetToken();
			printf("ttype %d ln 860\n",ttype);
			st = stmt_list();
		}
		else
		{
			ungetToken();
			return st;
		}
	}*/
	else
	{	
		return st;
	}
}

struct assignNode* assignment() 

{
	
	struct assignNode* assign;
	

	struct varNode* var;
	struct varNode* op;
	int oper;
	
	var = symSearch(token);

	
	if(var != NULL) 
	
	{
		
		printf("var: %s\n",var->vID);
	
	
		assign = make_assign(var);
	
		ttype = getToken();



		if(ttype == EQUAL) 
		
		{
				
			printf("EQUAL: %d\n",ttype);	
			//assign = expr(assign);
	
			
			ttype = getToken();
			if (ttype == NUM)
			{	
				printf("NUM: %d\n",ttype);
				op = make_var();
		
				if(symSearch(token) == NULL)
				{	
					symAdd(op);
				}
		
				assign->op1 = symSearch(token);
		
				/*assign->op1 = primary(assign->op1);

		ttype = getToken();
		if ((ttype == PLUS) | (ttype == MINUS) | (ttype == MULT) | (ttype == DIV))
		{	
			assign->oper = ttype; //op is set + - * /
			assign->op2 = primary(assign->op2);
		}
		
		ungetToken();
		return assign;*/
			} 
			else if (ttype == ID)
			{
		
				assign->op1 = symSearch(token);
				//printf("assign->op1->vValue %d\n", assign->op1->vValue);
		
					/*printf("ID: %d\n",ttype);

		assign->op1 = primary(assign->op1);
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

				assign->op2 = primary(assign->op2);
				return assign;									
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
*/		
	
	 		}
	 
 			ttype = getToken();
 			printf("SIGN %d\n", ttype);
 			if ((ttype == PLUS) | (ttype == MINUS) | (ttype == MULT) | (ttype == DIV))
 			{
				assign->oper = ttype;
				
				ttype = getToken();
				if (ttype == NUM)
				{	
					printf("NUM: %d\n",ttype);
					op = make_var();
		
					if(symSearch(token) == NULL)
					{
						symAdd(op);
					}
		
					assign->op2 = symSearch(token);
		
				/*assign->op1 = primary(assign->op1);

		ttype = getToken();
		if ((ttype == PLUS) | (ttype == MINUS) | (ttype == MULT) | (ttype == DIV))
		{	
			assign->oper = ttype; //op is set + - * /
			assign->op2 = primary(assign->op2);
		}
		
		ungetToken();
		return assign;*/
				} 
				else if (ttype == ID)
				{
		
					assign->op2 = symSearch(token);
					//printf("assign->op2->vValue %d\n", assign->op2->vValue);
		
				/*printf("ID: %d\n",ttype);

		assign->op1 = primary(assign->op1);
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

				assign->op2 = primary(assign->op2);
				return assign;									
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
*/		
	
	 			}
	 			return assign;
	 		}
	 		else
	 		{
				ungetToken();
				return assign;
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

struct assignNode* expr(struct assignNode* assign)
{
	struct varNode* op;
	struct varNode* oper2;
	int oper;

	ttype = getToken();
	if (ttype == NUM)
	{	
		printf("NUM: %d\n",ttype);
		op = make_var();
		
		if(symSearch(token) == NULL)
		{
			symAdd(op);
		}
		
		assign->op1 = symSearch(token);
		
		/*assign->op1 = primary(assign->op1);

		ttype = getToken();
		if ((ttype == PLUS) | (ttype == MINUS) | (ttype == MULT) | (ttype == DIV))
		{	
			assign->oper = ttype; //op is set + - * /
			assign->op2 = primary(assign->op2);
		}
		
		ungetToken();
		return assign;*/
	} 
	else if (ttype == ID)
	{
		
		assign->op1 = symSearch(token);
		
		/*printf("ID: %d\n",ttype);

		assign->op1 = primary(assign->op1);
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

				assign->op2 = primary(assign->op2);
				return assign;									
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
*/		
	
	 }
	 
	 ttype = getToken();
	 if ((ttype == PLUS) | (ttype == MINUS) | (ttype == MULT) | (ttype == DIV))
	 {
		assign->oper = ttype;
	 }
		
	ttype = getToken();
	if (ttype == NUM)
	{	
		printf("NUM: %d\n",ttype);
		op = make_var();
		
		if(symSearch(token) == NULL)
		{
			symAdd(op);
		}
		
		assign->op1 = symSearch(token);
		
		/*assign->op1 = primary(assign->op1);

		ttype = getToken();
		if ((ttype == PLUS) | (ttype == MINUS) | (ttype == MULT) | (ttype == DIV))
		{	
			assign->oper = ttype; //op is set + - * /
			assign->op2 = primary(assign->op2);
		}
		
		ungetToken();
		return assign;*/
	} 
	else if (ttype == ID)
	{
		
		assign->op2 = symSearch(token);
		
		/*printf("ID: %d\n",ttype);

		assign->op1 = primary(assign->op1);
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

				assign->op2 = primary(assign->op2);
				return assign;									
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
*/		
	
	 }
	 return assign;
}

struct conditionNode* condition()
{
	struct conditionNode* cNode;
	//struct varNode* op1;
	//struct varNode* op2;
	struct statementNode* tBranch;
	struct statementNode* fBranch;
	int op;

	cNode = make_condition();

	ttype = getToken();
	printf("ttype %d ln 1272\n", ttype);
	if ((ttype == ID)||(ttype == NUM))
	{
		//ungetToken(); //ungetToken since it still be parsed
		cNode->op1 = symSearch(token); //left operand of a condition is a primary

		ttype = getToken();
		printf("ttype %d ln 1273\n", ttype);
		if ((ttype == GREATER)||(ttype == GTEQ)||(ttype == LESS)
			||(ttype == NOTEQUAL)||(ttype == LTEQ))
		{
			cNode->operator = ttype; //relop is set to >, <, etc.
		
			ttype = getToken();
			printf("ttype %d ln 1280\n", ttype);
			if ((ttype == ID)|(ttype == NUM))
			{
				//ungetToken(); //ungetToken since it still be parsed
				cNode->op2 = symSearch(token); //right operand of a condition is a primary			
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

struct ifStatement* ifSt()
{	
	struct ifStatement* ifstmt;
	struct statementNode* noop;
	ifstmt = make_ifSt();

	ifstmt->condition = condition();
	ttype = getToken();
	if(ttype == LBRACE)
	{
		//body
		ifstmt->stmt_list = stmt_list();
		
		noop = make_stmt(NOOPSTMT);
		
		//find last stmt stmt ->next = noop;
		findLast(ifstmt->stmt_list)->next = noop;
		
		//true
		ifstmt->condition->trueBranch = ifstmt->stmt_list;
		
		//false
		//noop = make_stmt(NOOPSTMT);
		
		//find last stmt stmt ->next = noop;
		//ifstmt->next = noop;
		ifstmt->condition->falseBranch = noop;
		
		return ifstmt;
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
	struct statementNode* noop;
	struct statementNode* gt;
	wSt = make_whileStatement();

	wSt->condition = condition();
	ttype = getToken();
	if(ttype == LBRACE)
	{
		//body
		wSt->stmt_list = stmt_list();
		
		//true
		wSt->condition->trueBranch = wSt->stmt_list;
		
		//goto
		gt = make_stmt(GOTOSTMT);
		gt = wSt->condition->trueBranch->next;
		
		//wSt->stmt_list->gotoSt->target = gt;
		
		//false
		noop = make_stmt(NOOPSTMT);
		
		//find last stmt stmt ->next = noop;
		findLast(wSt->stmt_list)->next = noop;
		
		return wSt;
	}
	else
	{
		return NULL;
	}
}

struct primaryNode* primary(struct primaryNode* prim)
{
	struct varNode* op;
	//struct primaryNode* pNode;
	//pNode = make_primary();
	
	if ((ttype == ID)|(ttype == NUM))
	{	
		if (ttype == ID)
		{	
			op = symSearch(token);

			if(op == NULL)
			{ 	
				prim->id = (char *) malloc(tokenLength+1);
				strcpy(prim->id,token);	//allocating space for ID
				symAdd(op);
			}
			else
			{
				prim->id = op->vID;
				prim->ival = op->vValue;
			}
			
			return prim;
		}
		else if (ttype == NUM)
		{
			prim->ival = atoi(token); //convert string to an integer
			return prim;
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