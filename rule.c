#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "rule.h"
#include "token.h"
#define	WLEN	16

typedef enum{
	W_B=-1,
	W_E =0,
	W_SHOW,
	W_STATUS,
	W_AGENT,
	W_TERM,
	W_TRAF,
	W_TS,
	W_SS,
	W_CONN,
	W_OPEN,
	W_CLOSE,
	W_ADD,
	W_DEL,
	W_REG,
	W_PASS,
	W_UPDATE,
	W_ROUTE,
	W_LOG,
	W_EXIT,
	W_OMIT,
	W_COMMA,
} W_WORD;

typedef struct {
	W_WORD	w_word;
	char	word[WLEN];
}WORD_LIST ;
static WORD_LIST wlist[]={
	{W_SHOW,"show"},
	{W_STATUS,"status"},
	{W_AGENT,"agent"},
	{W_TERM,"terminal"},
	{W_CONN,"connect"},
	{W_OPEN,"open"},
	{W_CLOSE,"close"},
	{W_ADD,"add"},
	{W_DEL,"delete"},
	{W_TRAF,"traf"},
	{W_REG,"register"},
	{W_PASS,"password"},
	{W_LOG,"log"},
	{W_UPDATE,"update"},
	{W_ROUTE,"route"},
	{W_EXIT,"exit"},
	{W_E,""}
} ;
typedef struct {
	int	CMD_ID;
	W_WORD	word[COL_MAX];
}COMMAND_TREE;
COMMAND_TREE	tree[]=
{
	{CMD_SHOW_STATUS,{W_SHOW,W_STATUS,W_E}},
	{CMD_SHOW_AGENT ,{W_SHOW,W_AGENT,W_E}},
	{CMD_SHOW_AGENT_TRAF ,{W_SHOW,W_AGENT,W_TRAF,W_E}},
	{CMD_SHOW_AGENT_ROUTE,{W_SHOW,W_AGENT,W_ROUTE,W_E}},
	{CMD_SHOW_TRAF,{W_SHOW,W_TRAF,W_E}},
	{CMD_SHOW_TERM,{W_SHOW,W_TERM,W_E}},
	{CMD_CONN_OPEN,{W_CONN,W_OPEN,W_E}},
	{CMD_CONN_CLOSE,{W_CONN,W_CLOSE,W_E}},
	{CMD_ADD_AGENT,{W_ADD,W_AGENT,W_E}},
	{CMD_ADD_TERM,{W_ADD,W_TERM,W_E}},
	{CMD_DEL_AGENT,{W_DEL,W_AGENT,W_E}},
	{CMD_DEL_TERM,{W_DEL,W_TERM,W_E}},
	{CMD_REG_PASS,{W_REG,W_PASS,W_E}},
	{CMD_UPDATE,{W_UPDATE,W_E}},
	{CMD_LOG,{W_LOG,W_E}},
	{CMD_EXIT ,{W_EXIT,W_E}},
	{CMD_NOP,{W_B,W_B,W_B,W_B,W_B}}
};


static char *get_word(W_WORD w)
{
	int i;
	for(i=0;;i++){
		if(wlist[i].w_word==w){
			return wlist[i].word;
		}
		if(wlist[i].w_word==W_E){
			break;
		}
	}
	return NULL;
}

static W_WORD check_reserved(char *word)
{
	int i;
	for(i=0;;i++){
		if(wlist[i].w_word==W_E){
			break;
		}
		if(strcmp(wlist[i].word,word)==0){
			break;
		}
	}
	return wlist[i].w_word;
}

static int check_hist(W_WORD *stack, int raw)
{
	int ret=0;
	int col;
	for(col=0;col<COL_MAX;col++){
		if(*stack==W_E){
			break;
		}
		if(*stack==tree[raw].word[col]){
			stack++;
			continue;
		}else{
			ret=-1;
			break;
		}
	}
	return ret;
}

int WordsCandidate(char *buff,char *cand)
{
	int ret=0;
	int col,raw;
	W_WORD stack[COL_MAX];
	char prev[PARAM_LEN];
	TOKEN	token;
	W_WORD code;
	char	word[PARAM_LEN];
	char *ptr;
	char *ptr2;
	for(col=0;col<COL_MAX;col++){
		stack[col]=W_E;
	}
	ptr=buff;
	prev[0]=0;
	//make stack 
	for(col=0;col<COL_MAX;col++){
		token=GetToken(word,100,&ptr);
		if(token==T_WORD || token==T_EOF){
			if(strlen(word)!=0){
				code=check_reserved(word);
				if(code==W_E){//Not Matched, Incomplete word to be compensated
					return 0;
				}
				stack[col]=code;
			}
		}
	}
	//Search Next Word
	for(raw=0;tree[raw].word[0]!=W_B;raw++){
		for(col=0;col<COL_MAX;col++){
			if(stack[col]==W_E){
				if(tree[raw].word[col]!=W_E){
					ptr2=get_word(tree[raw].word[col]);
					if(ptr2!=0 && strcmp(ptr2,prev)!=0){
						strcat(cand,ptr2);
						strcpy(prev,ptr2);
						strcat(cand," ");
						ret=1;
					}
				}
				break;
			}else if(stack[col]!=tree[raw].word[col]){
				break;
			}
		}
	}
	return ret;
}


int WordCompensation(char *buff)
{
	int col,raw;
	int comma=0;
	char	word[100];
	TOKEN	token;
	W_WORD	code,wword;
	char	*ptr;
	char	*ptr2;
	char	*ptr3;
	char	*ptr4;
	W_WORD stack[COL_MAX];
	for(col=0;col<COL_MAX;col++){
		stack[col]=W_E;
	}
	ptr=buff;
	if(strlen(buff)==0) return 0;
	for(col=0;col<COL_MAX;col++){
		token=GetToken(word,100,&ptr);
		if(token==T_COMMA){
			if(comma==0){
				comma++;
				stack[col]=W_COMMA;
			}else{
				//double commas mean there is an omited parameter between them.
				stack[col++]=W_OMIT;
				stack[col]=W_COMMA;
			}
			continue;
		}
		if(token==T_WORD || token==T_EOF){
			if(strlen(word)==0)
				return 0;
			code=check_reserved(word);
			if(code==W_E){//Not Matched, Incomplete word to be compensated
				for(raw=0;;raw++){
					//extract candidate word
					wword=tree[raw].word[col];
					if(wword==W_B){
						return 0;//No candidate
					}
					if(check_hist(stack,raw)==-1){
						continue;
					}
					//conver to string
					ptr2=get_word(wword);
					if(ptr2==NULL){
						return 0;
					}
					//Compare sub string
					if(strncmp(word,ptr2,strlen(word))==0){
						//Candidate found
						ptr3=strstr(buff,word);
						if(ptr3==NULL){
							return 0;
						}
						for(;;){
							ptr4=strstr(ptr3+1,word);
							if(ptr4==NULL){
								break;
							}else{
								ptr3=ptr4;
							}
						}
						strcpy(ptr3,ptr2);
						strcat(buff," ");
						return 1;
					}else{
						//Unmatch
						continue;
					}
				}
			}else{//Match reserved word
				stack[col]=code;
				continue;
			}
		}else{
			return 0;
		}
	}
	return 0;
}

int AnalyzeCmdLine(char *buff,char params[COL_MAX][PARAM_LEN])
{
	int col,raw;
	int cmd;
	W_WORD stack[COL_MAX];
	TOKEN	token;
	W_WORD code;
	char	word[PARAM_LEN];
	char *ptr;
	int  depth;
	int n;

	for(col=0;col<COL_MAX;col++){
		stack[col]=W_E;
	}
	ptr=buff;
	//make stack 
	for(col=0;col<COL_MAX;col++){
		token=GetToken(word,100,&ptr);
		if(token==T_WORD || token==T_EOF){
			if(strlen(word)!=0){
				code=check_reserved(word);
				if(code==W_E){//Not Matched, Incomplete word to be compensated
					break;
				}
				stack[col]=code;
			}else{
				break;
			}
		}
	}

	//Search Next Word
	depth=0;
	for(raw=0;tree[raw].word[0]!=W_B;raw++){
		for(col=0;col<COL_MAX;col++){
			if(stack[col]==W_E&&tree[raw].word[col]==W_E){
				if(depth<col){
					cmd=raw;
					depth=col;
				}else{
					break;
				}
			}else if(stack[col]!=tree[raw].word[col]){
				break;
			}
		}
	}
	if(depth==0){
		return CMD_NOP;
	}
	for(n=0;*word;){
		strcpy(params[n++],word);
		if(n>=COL_MAX){
			break;
		}
		token=GetToken(word,100,&ptr);
		if(token==T_WORD || token==T_EOF){
			continue;
		}
		break;
	}
	return tree[cmd].CMD_ID;
}

