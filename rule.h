#ifndef RULE_H
#define CMD_NOP  0
#define CMD_BASE  1000
#define CMD_SHOW	10000
#define CMD_SHOW_STATUS 10010
#define CMD_SHOW_AGENT	10020
#define CMD_SHOW_AGENT_TRAF 10030
#define CMD_SHOW_AGENT_ROUTE 10040
#define CMD_SHOW_TERM 10050
#define CMD_SHOW_TRAF 10060
#define CMD_CONN 12000
#define CMD_CONN_OPEN 12001
#define CMD_CONN_CLOSE 12002
#define CMD_ADD 13000
#define CMD_ADD_AGENT 13002
#define CMD_ADD_TERM 13004
#define CMD_DEL 14000
#define CMD_DEL_AGENT 14001
#define CMD_DEL_TERM 14002
#define CMD_REG 15000
#define CMD_REG_PASS  15001  
#define CMD_UPDATE 16000 
#define CMD_LOG 17000 
#define CMD_EXIT 19000 
#define CMD_QUIT 19001 

#define COL_MAX	6
#define PARAM_LEN 100

int GetParams(char **buff,int level, int num,size_t len,char **params);
int GetCmdID(char **buff,int *paran,int *mode,int *level);
int WordCompensation(char *buff);
int WordsCandidate(char *buff,char *cand);
int AnalyzeCmdLine(char *ptr,char params[COL_MAX][PARAM_LEN]);
#define RULE_H
#endif
