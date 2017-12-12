#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "rule.h"
#include "token.h"
#define	WLEN	16

typedef enum{
	W_SET,
	W_CLEAR,
	W_LIST,
	W_CONFIG,
	W_OP,
	W_CUG,
	W_TUN,
	W_LINK,
	W_DISP,
	W_MAX,
	W_PORT,
	W_CRYPT,
	W_AES,
	W_CAMELLIA,
	W_RELAY,
	W_ON,
	W_OFF,
	W_SEG,
	W_POOL,
	W_ROUTE,
	W_DNS,
	W_PERMIT,
	W_DENY,
	W_ALL,
	W_DEL,
	W_SHOW,
	W_MODE,
	W_QOS,
	W_TRF,
	W_ACL,
	W_ADD,
	W_GDR,
	W_START,
	W_STOP,
	W_IP,
	W_BIND,
	W_EXIT,
	W_QUIT,
	W_E,
	W_S,
} W_WORD;

typedef struct {
	W_WORD	w_word;
	char	word[WLEN];
}WORD_LIST;

WORD_LIST wlist[]={
	{W_SET,"set"},
	{W_CLEAR,"clear"},
	{W_LIST,"list"},
	{W_CONFIG,"conf"},
	{W_CONFIG,"config"},
	{W_OP,"op"},
	{W_CUG,"cug"},
	{W_LINK,"link"},
	{W_TUN,"tun"},
	{W_DISP,"disp"},
	{W_MAX,"max"},
	{W_PORT,"port"},
	{W_CRYPT,"crypt"},
	{W_AES,"aes"},
	{W_CAMELLIA,"camellia"},
	{W_CAMELLIA,"came"},
	{W_RELAY,"relay"},
	{W_ON,"on"},
	{W_OFF,"off"},
	{W_SEG,"seg"},
	{W_IP,"ip"},
	{W_BIND,"bind"},
	{W_POOL,"pool"},
	{W_ROUTE,"route"},
	{W_DNS,"dns"},
	{W_PERMIT,"permit"},
	{W_DENY,"deny"},
	{W_ALL,"all"},
	{W_ADD,"add"},
	{W_DEL,"del"},
	{W_SHOW,"show"},
	{W_MODE,"mode"},
	{W_QOS,"qos"},
	{W_TRF,"trf"},
	{W_ACL,"acl"},
	{W_GDR,"gdr"},
	{W_START,"start"},
	{W_STOP,"stop"},
	{W_EXIT,"exit"},
	{W_QUIT,"quit"},
	{W_E,""}
} ;
#define	COM_LINE	5
typedef struct {
	int	CMD_ID;
	W_WORD	word[COM_LINE];
	int		mode;
	int		params;//最大パラメータ数
	int		parav;//0:最大パラメータ数必須 1:パラメータオプション
}COMMAND_TREE;
COMMAND_TREE	tree[]=
{
	{SET_DISP,{W_S,W_DISP,W_E},M_CONF_S,1,0},
	{SET_MAX,{W_S,W_MAX,W_E},M_CONF_S,1,0},
	{SET_PORT,{W_S,W_PORT,W_E},M_CONF_S,1,0},
	{SET_CRYPT_A,{W_S,W_CRYPT,W_AES,W_E},M_CONF_S,2,0},
	{SET_CRYPT_C,{W_S,W_S,W_CAMELLIA,W_E},M_CONF_S,2,0},
	{SET_RELAY_ON,{W_S,W_RELAY,W_ON,W_E},M_CONF_S,1,0},
	{SET_RELAY_OFF,{W_S,W_RELAY,W_OFF,W_E},M_CONF_S,1,0},
	{SET_SEG,{W_S,W_SEG,W_E},M_CONF_S,2,0},
	{SET_BIND,{W_S,W_BIND,W_E},M_CONF_C,1,0},
	{SET_IP,{W_S,W_IP,W_E},M_CONF_C,2,0},
	{CLR_ALL,{W_S,W_ALL,W_E},M_CONF_C|M_CONF_S,0,0},
	{LST_TUN,{W_S,W_TUN,W_E},M_CONF_S,0,0},
	{ADD_POOL,{W_ADD,W_POOL,W_E},M_CONF_S,2,0},
	{ADD_ROUTE,{W_S,W_ROUTE,W_E},M_CONF_S,3,0},
	{ADD_DNS,{W_S,W_DNS,W_E},M_CONF_S,2,0},
	{ADD_PERMIT,{W_S,W_PERMIT,W_E},M_CONF_S,2,0},
	{ADD_DENY,{W_S,W_DENY,W_E},M_CONF_S,2,0},
	{DEL_POOL,{W_DEL,W_POOL,W_E},M_CONF_S,2,0},
	{DEL_ROUTE,{W_S,W_ROUTE,W_E},M_CONF_S,3,0},
	{DEL_DNS,{W_S,W_DNS,W_E},M_CONF_S,2,0},
	{DEL_PERMIT,{W_S,W_PERMIT,W_E},M_CONF_S,2,0},
	{DEL_DENY,{W_S,W_DENY,W_E},M_CONF_S,2,0},
	{SHW_CUG_MODE,{W_S,W_S,W_MODE,W_E},M_CONF_S,0,0},
	{SHW_CUG_QOS,{W_S,W_S,W_QOS,W_E},M_CONF_S,0,0},
	{SHW_CUG_TRF,{W_S,W_S,W_TRF,W_E},M_CONF_S|M_CONF_C,0,0},
	{SHW_CUG_ACL,{W_S,W_S,W_ACL,W_E},M_CONF_S,0,0},
	{SHW_CUG_BIND,{W_S,W_S,W_BIND,W_E},M_CONF_C,0,0},
	{SHW_CUG_IP,{W_S,W_S,W_IP,W_E},M_CONF_C,0,0},

	{SHW_TUN_ALL,{W_S,W_TUN,W_ALL,W_E},M_CONF_T,0,0},
	{SHW_TUN_DISP,{W_S,W_S,W_DISP,W_E},M_CONF_T,0,0},
	{SHW_TUN_MAX,{W_S,W_S,W_MAX,W_E},M_CONF_T,0,0},
	{SHW_TUN_PORT,{W_S,W_S,W_PORT,W_E},M_CONF_T,0,0},
	{SHW_TUN_CRYPT,{W_S,W_S,W_CRYPT,W_E},M_CONF_T,0,0},
	{SHW_TUN_RELAY,{W_S,W_S,W_RELAY,W_E},M_CONF_T,0,0},
	{SHW_TUN_SEG,{W_S,W_S,W_SEG,W_E},M_CONF_T,0,0},
	{SHW_TUN_POOL,{W_S,W_S,W_POOL,W_E},M_CONF_T,0,0},
	{SHW_TUN_ROUTE,{W_S,W_S,W_ROUTE,W_E},M_CONF_T,0,0},
	{SHW_TUN_DNS,{W_S,W_S,W_DNS,W_E},M_CONF_T,0,0},
	{SHW_TUN_PERMIT,{W_S,W_S,W_PERMIT,W_E},M_CONF_T,0,0},
	{SHW_TUN_DENY,{W_S,W_S,W_DENY,W_E},M_CONF_T,0,0},
	{SHW_TUN_MODE,{W_S,W_S,W_MODE,W_E},M_CONF_T,0,0},
	{SHW_TUN_QOS,{W_S,W_S,W_QOS,W_E},M_CONF_T,0,0},
	{SHW_TUN_TRF,{W_S,W_S,W_TRF,W_E},M_CONF_T,0,0},
	{SHW_TUN_ACL,{W_S,W_S,W_ACL,W_E},M_CONF_T,0,0},
	{SHW_TUN_BIND,{W_S,W_S,W_BIND,W_E},M_CONF_T,0,0},
	{SHW_TUN_IP,{W_S,W_S,W_IP,W_E},M_CONF_T,0,0},

	{CMD_GDR,{W_GDR,W_E},M_OP_S|M_OP_C,0,0},
	{CMD_START,{W_START,W_E},M_OP_S|M_OP_C,0,0},
	{CMD_STOP,{W_STOP,W_E},M_OP_S|M_OP_C,0,0},
	{CMD_EXIT,{W_EXIT,W_E},M_OP_S|M_OP_C|M_CONF_S|M_CONF_C,0,0},
	{CMD_QUIT,{W_QUIT,W_E},M_NORMAL,0,0},
	{CMD_NOP,{W_E,W_E,W_E,W_E,W_E},0,0}
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

static W_WORD check_researve(char *word)
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



int WordCompensation(char *buff)
{
	int i,j,k;
	int comma=0;
	char	word[100];
	TOKEN	token;
	W_WORD	code,wword;
	char	*ptr;
	char	*ptr2;
	char	*ptr3;
	char	*ptr4;
	ptr=buff;
	j=0;
	if(strlen(buff)==0) return 0;
	for(i=0;i<COM_LINE;){
		token=GetToken(word,100,&ptr);
		if(i>=2 && token==T_COMMA){
			if(comma==0){
				comma++;
			}else{
				i++;
			}
			continue;
		}
		if(token==T_WORD || token==T_EOF){
			if(strlen(word)==0)
				return 0;
			code=check_researve(word);
			if(code==W_E){//Incomplete word to be compensated
				for(k=0;;k++,j++){
					//extract candidate word
					wword=tree[j].word[i];
					if(i==0){//top command
						if(wword==W_E){
							return 0;
						}else if(wword==W_S){
							continue;
						}
					}else if(k!=0 && tree[j].word[i-1]!=W_S){
						//Change preceding command
						return 0;
					}else if(wword==W_S){
						continue;
					}
					//conver to string
					ptr2=get_word(wword);
					if(ptr2==NULL){
						return 0;
					}
					//
					if(strncmp(word,ptr2,strlen(word))==0){
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
					}
				}
			}
			for(;;){
				if(tree[j].word[i]==code){
					break;
				}
				j++;
				if(i==0){
					if(tree[j].word[0]==W_E){
						return 0;
					}
				}else{
					if(tree[j].word[i-1]!=W_S){
						return 0;
					}
				}
			}
			i++;
			//Hit word;
			if(token==T_EOF ){
				return 0;
			}
			//T_WORD
			if(tree[j].word[i]==W_E){
				return 0;
			}
		}else{
			return 0;
		}
	}
	return 0;
}

int GetCmdID(char **buff,int *paran,int *mode,int *level)
{
	int i,j;
	int comma=0;
	char	word[100];
	TOKEN	token;
	W_WORD	code;
	j=0;
	for(i=0;i<COM_LINE;){
		token=GetToken(word,100,buff);
		if(i>=2 && token==T_COMMA){
			if(comma==0){
				comma++;
			}else{
				i++;
			}
			continue;
		}
		if(token==T_WORD || token==T_EOF){
			code=check_researve(word);
			if(code==W_E){
				return -11;
			}
			for(;;){
				if(tree[j].word[i]==code){
					break;
				}
				j++;
				if(i==0){
					if(tree[j].word[0]==W_E){
						return -12;
					}
				}else{
					if(tree[j].word[i-1]!=W_S){
						return -13;
					}
				}
			}
			i++;
			//Hit word;
			if(token==T_EOF ){
				if(tree[j].word[i]!=W_E){
					return -14;
				}else{
					if(paran) *paran=tree[j].params;
					if(mode) *mode=tree[j].mode;
					if(level) *level=i;
					return tree[j].CMD_ID;
				}
			}
			//T_WORD
			if(tree[j].word[i]==W_E){
				if(paran) *paran=tree[j].params;
				if(mode) *mode=tree[j].mode;
				if(level) *level=i;
				return tree[j].CMD_ID;
			}
		}else{
			printf("E:token:%d\n",token);
			return -19;
		}
	}
	return -20;
}
int GetParams(char **buff,int level,int para_num,size_t para_len,char **params)
{
	int		i;
	char	word[128];
	char	*p;
	TOKEN	token;
	size_t	len;
	int		comma=0;
	p=(char *)params;
	if(level==1) comma=1;
	for(i=0;;){
		token=GetToken(word,100,buff);
		if(token==T_COMMA){
			if(comma==0){
				comma=1;
				continue;
			}else{
				comma=1;
			}
		}else{
			comma=0;
		}
		len=strlen(word);
		if(token==T_EOF && len==0){
			break;
		}
		if(len<para_len-1){
			//printf("p=%X w=%s token=%d\n",p,word,token);
			strncpy(p,word,para_len-1);
			i++;
			p+=para_len;
			if((i>=para_num) || (token==T_EOF)){
				break;
			}
		}else{
			return -1;
		}
	}
	return i;
}


