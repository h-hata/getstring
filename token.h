typedef enum {
	T_ERROR=0,
	T_COMMA,
	T_WORD,
	T_BAR,
	T_AMP,
	T_SEMI,
	T_GT,
	T_GTGT,
	T_EQU,
	T_LT,
	T_NL,
	T_EOF}
TOKEN;

TOKEN GetToken(char *word,int len,char **p);

