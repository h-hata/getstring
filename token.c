#include <stdio.h>
#include <stdlib.h>
#include "token.h"



static char mygetchar(char **inp)
{
	char c;
	char *p;

	p=*inp;
	c=*p;
	p++;
	*inp=p;
	if(c=='\0') return EOF;
	else	return c;
}
	
	
static void myungetc(char c,char **inp)
{
	char	*p;

	p=*inp;
	p--;
	*p=c;
	*inp=p;
}
	


TOKEN GetToken(char *word,int len,char **inp)
{
	enum {NEUTRAL,GTGT,INQUOTE,INWORD}state = NEUTRAL;
	char	c;
	char	*w;
	int	l=0;
	w=word;	
	for(*w=0;EOF!=(c=mygetchar(inp));){
		switch(state){
		case NEUTRAL:
			switch(c){
			case ',':
				return T_COMMA;
			case ';':
				return T_SEMI;
			case '&':
				return T_AMP;
			case '|':
				return T_BAR;
			case '<':
				return T_LT;
			case '=':
				return T_EQU;
			case '\n':
				return T_NL;
			case '\t':
			case ' ':
				continue;
			case '>':
				state=GTGT;
				continue;
			case '"':
				state=INQUOTE;
				continue;
			default:
				state=INWORD;
				*w++=c;l++;
				if(l >=len) return T_ERROR;
				continue;
			}
		case GTGT:
			if(c=='>')
				return T_GTGT;
			myungetc(c,inp);
			return T_GT;
		case INQUOTE:
			switch(c){
			case '\\':
				*w++=mygetchar(inp);l++;
				if(l >=len) return T_ERROR;
				continue;
			case '"':
				*w='\0';
				return T_WORD;
			default:		
				*w++=c;l++;
				if(l >=len) return T_ERROR;
				continue;
			}
		case INWORD:
			switch(c){
				case ',':
				case ';':
				case '&':
				case '|':
				case '<':
				case '>':
				case '\n':
				case ' ':
				case '\t':
				case '=':
					myungetc(c,inp);
					*w='\0';
					return T_WORD;
				default:
					*w++=c;l++;
					if(l >=len) return T_ERROR;
					continue;
			}
		}
	}
	*w='\0';
	return (T_EOF);
}


					
#ifdef TEST 

main()
{
	char word[200];
	int	ret;
	char	buff[1024];
	char	*p;
	int		break_flag;

	for(;;){
		p=buff;
		ret=GetString(buff,1000,"$",1);
		if(ret!=0) {
			exit(0);
		}
		for(break_flag=0;!break_flag;){
			switch(GetToken(word,150,&p)){
			case T_WORD:
				printf("T_WORD<%s>\n",word);
				break;
			case T_BAR:
				printf("T_BAR\n");
				break;
			case T_AMP:
				printf("T_AMP\n");
				break;
			case T_SEMI:
				printf("T_SEMI\n");
				break;
			case T_COMMA:
				printf("T_COMMA\n");
				break;
			case T_GT:
				printf("T_GT\n");
				break;
			case T_GTGT:
				printf("T_GTGT\n");
				break;
			case T_EQU:
				printf("T_EQU\n");
				break;
			case T_LT:
				printf("T_LT\n");
				break;
			case T_NL:
				printf("T_NL\n");
				break;
			case T_EOF:
				printf("T_EOF<%s>\n",word);
				break_flag=1;
				break;
				//exit(0);
			}
		}
	}
}
#endif
					
