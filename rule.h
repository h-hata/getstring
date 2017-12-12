#define CMD_NOP  0
#define SET_DISP 103
#define SET_MAX  104
#define SET_PORT 105
#define SET_CRYPT_A 106
#define SET_CRYPT_C 107
#define SET_SEG  108
#define SET_BIND 109 
#define SET_IP  110
#define SET_RELAY_ON 111
#define SET_RELAY_OFF 112
#define CLR_ALL  203
#define LST_TUN  502
#define ADD_POOL 601
#define ADD_ROUTE 602
#define ADD_DNS  603
#define ADD_PERMIT 604
#define ADD_DENY 605
#define DEL_POOL 701
#define DEL_ROUTE 702
#define DEL_DNS  703
#define DEL_PERMIT 704
#define DEL_DENY 705

#define SHW_CUG_ALL  901
#define SHW_CUG_DISP 902
#define SHW_CUG_MAX  903
#define SHW_CUG_PORT 904
#define SHW_CUG_CRYPT 905
#define SHW_CUG_RELAY 906
#define SHW_CUG_SEG  907
#define SHW_CUG_POOL 908 
#define SHW_CUG_ROUTE 909
#define SHW_CUG_DNS  910
#define SHW_CUG_PERMIT 911
#define SHW_CUG_DENY 912
#define SHW_CUG_MODE 913
#define SHW_CUG_QOS  914
#define SHW_CUG_TRF  915
#define SHW_CUG_ACL  916
#define SHW_CUG_BIND 917
#define SHW_CUG_IP  918
#define SHW_CUG_LINK  919

#define SHW_TUN_ALL  1001
#define SHW_TUN_DISP 1002
#define SHW_TUN_MAX  1003
#define SHW_TUN_PORT 1004
#define SHW_TUN_CRYPT 1005
#define SHW_TUN_RELAY 1006
#define SHW_TUN_SEG  1007
#define SHW_TUN_POOL 1008 
#define SHW_TUN_ROUTE 1009
#define SHW_TUN_DNS  1010
#define SHW_TUN_PERMIT 1011
#define SHW_TUN_DENY 1012
#define SHW_TUN_MODE 1013
#define SHW_TUN_QOS  1014
#define SHW_TUN_TRF  1015
#define SHW_TUN_ACL  1016
#define SHW_TUN_BIND 1017
#define SHW_TUN_IP  1018
#define SHW_TUN_LINK  1019

#define CMD_START 2001
#define CMD_STOP 2002
#define CMD_GDR  2003

#define CMD_EXIT 9000 
#define CMD_QUIT 9001 

#define	M_LOGIN     0
#define M_NORMAL	1
#define M_CONF_S	2
#define M_CONF_C	4
#define M_OP_S		8
#define M_OP_C		16
#define	M_CONF_T	32

int GetParams(char **buff,int level, int num,size_t len,char **params);
int GetCmdID(char **buff,int *paran,int *mode,int *level);
int WordCompensation(char *buff);

