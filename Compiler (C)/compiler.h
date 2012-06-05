//compiler.h

struct programNode {
       struct varNode* decl;
       struct bodyNode* body;
};

struct declNode { 
	struct varNode* var_list;      
};

struct var_declNode {
	struct varNode* var_list;
};

struct bodyNode {
	struct statementNode* stmt_list;
};

struct var_listNode {
	char *id;
	struct var_listNode* var_list;
};

struct varNode {
	char* vID;
	int vValue;
	int vType;
	struct varNode* next;
};

struct statementNode {
	int stmtType; // NOOPSTMT, GOTOSTMT, ASSIGNSTMT, WHILESTMT, IFSTMT
	struct assignNode* assignSt;
	struct printStatement* printSt;
	struct whileStatement* whileSt;
	struct ifStatement* ifSt;
	struct gotoStatement* gotoSt;
	struct statementNode* next;
};

struct assignNode {
	struct varNode* lvalue;
	struct varNode* op1;
	struct varNode* op2;
	int oper;
	//struct assignNode* next;
};

struct printStatement {
	struct varNode* id;
};

struct ifStatement {
	struct conditionNode* condition; // condition of the if
	struct statementNode* stmt_list; // body of the if statement
};

struct whileStatement {
	struct conditionNode* condition; // condition of the while
	struct statementNode* stmt_list; // body of the while statement
};

struct conditionNode {
	int operator;
	struct varNode* op1;
	struct varNode* op2;
	struct statementNode* trueBranch;
	struct statementNode* falseBranch;
};

struct primaryNode {
	int ival;
	char *id;
};

struct gotoStatement {
	struct statementNode* target;
};

struct symbolTableNode { 
	char *sID;
	int sType;
	int sVal;
	char symList[100];
	struct varNode* var;
	struct symbolTableNode* list; // list of directly enclosed blocks
	struct symbolTableNode* head;
	struct symbolTableNode* next;
	struct statementNode* stmt;
};


/*------------------------------------------------------------------------
  PARSE TREE FUNCTIONS
--------------------------------------------------------------------------*/
struct programNode* 			make_programNode();
struct declNode* 				make_declNode();
struct var_declNode*			make_var_decl();
struct bodyNode*				make_body();
struct var_listNode*			make_var_list();
struct varNode*					make_var();
struct statementNode*			make_stmt(int sType);
struct assignNode*				make_assign(struct varNode* var);
struct conditionNode*			make_condition();
struct ifStatement*				make_ifSt();
struct gotoStatement*			make_gotoSt();
struct whileStatement*			make_whileSt();
struct printStatement*			make_printSt(struct varNode* i);
struct primaryNode*				make_primary();

//Print function//
//int symAdd(struct symbolTableNode* s, struct symbolTableNode** list);
//void varAdd(struct varNode* v, struct varNode** list);
//int idAdd(struct id_listNode* i, struct id_listNode** list);
//struct varNode* lookupVar(char* v, struct varNode* list);
//struct varNode* findVar(char* t, struct symbolTableNode* sym);
int assign_list();
int assignVal();
//int output(struct statementNode* stmt);
void output();
void outputPrint(struct printStatement* p);
void outputIf(struct ifStatement* i);
void outputAssignment(struct assignNode* a);
void outputWhile(struct whileStatement* w);
int outputCondition( struct conditionNode* c);
void outputExpr(struct assignNode* assign);

/*------------------------------------------------------------------------
  LINKED LIST FUNCTIONS
--------------------------------------------------------------------------*/
struct varNode* symSearch(char* id);
void symAdd(struct varNode* symbol);
struct statementNode* findLast(struct statementNode* cur);

/*------------------------------------------------------------------------
  PARSING FUNCTIONS
--------------------------------------------------------------------------*/
struct programNode* program();
struct varNode* decl();
struct var_declNode* var_decl();
struct statementNode* stmt();
struct statementNode* stmt_list();
struct bodyNode* body();
struct varNode* id_list();
struct varNode* var();
struct assignNode* assignment();
struct conditionNode* condition();
struct ifStatement* ifSt();
struct gotoStatement* gotoSt();
struct whileStatement* whileStatement();
struct printStatement* printSt();
struct primaryNode*	primary(struct primaryNode* prim);
struct assignNode* expr(struct assignNode* assign);

