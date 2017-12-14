#define CMD_NOP  0
#define CMD_SHOW_STATUS 1001
#define CMD_SHOW_AGENT_TRAF 1002
#define CMD_SHOW_AGENT_ROUTE 1003
#define CMD_SHOW_TERM 1004
#define CMD_SHOW_TRAF 1005
#define CMD_CONN_OPEN 2001
#define CMD_CONN_CLOSE 2002
#define CMD_ADD_AGENT 3001
#define CMD_ADD_TERM 3004
#define CMD_DEL_AGENT 4001
#define CMD_DEL_TERM 4002
#define CMD_REG_PASS  5001  
#define CMD_UPDATE 6000 
#define CMD_EXIT 9000 
#define CMD_QUIT 9001 
#define COL_MAX	6
#define PARAM_LEN 100

int GetParams(char **buff,int level, int num,size_t len,char **params);
int GetCmdID(char **buff,int *paran,int *mode,int *level);
int WordCompensation(char *buff);
int WordsCandidate(char *buff,char *cand);
int AnalyzeCmdLine(char *ptr,char params[6][100]);
