#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include "rule.h"
#include "shell.h"

#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#endif
#define	PROM	"-->"
#define	LEN_MAX	1000
#define	HIST_MAX	10
extern int errno; 
static 	char	buff[1024];
static	size_t	len=0;
static	size_t	pos=0;
static	char	prom[1024];
static	char	hist[HIST_MAX][1024];
static	int	tindex=-1;
static	int	upper=0;

static struct termios termattr, save_termattr;
static int ttysavefd = -1;
static enum 
{ 
	RESET, RAW, CBREAK 
} ttystate = RESET;

static void tty_atexit(void);
static int set_tty_cooked(void);

static int set_tty_raw(void) 
{
	int i;
	//TERMIOS Backup
	i = tcgetattr (STDIN_FILENO, &termattr);
	if (i < 0) {
		//printf("tcgetattr fail%d for fildes=%d\n",i,STDIN_FILENO); 
		return -1;
	}
	save_termattr = termattr;
	termattr.c_lflag &= (tcflag_t)~(ECHO | ICANON | IEXTEN | ISIG);
	termattr.c_iflag &= (tcflag_t)~(BRKINT | INPCK | ISTRIP | IXON);
	termattr.c_cflag &= (tcflag_t)~(CSIZE | PARENB);
	termattr.c_cflag |= (tcflag_t)CS8;
	termattr.c_oflag &= (tcflag_t)~(OPOST);
	 
	termattr.c_cc[VMIN] = 1;	/* or 0 for some Unices;	see note 1 */
	termattr.c_cc[VTIME] = 0;

	i = tcsetattr (STDIN_FILENO, TCSANOW, &termattr);
	if (i < 0) 
	{
		return -1;
	}
	 
	ttystate = RAW;
	ttysavefd = STDIN_FILENO;
	atexit(tty_atexit);
	return 0;
}


static int set_tty_cooked(void) 
{
	int i;
	if (ttystate != RAW) 
	{
		return 0;
	}
	i = tcsetattr (STDIN_FILENO, TCSAFLUSH, &save_termattr);
	if(i==0)
		ttystate = RESET;
	return i;
}

static void tty_atexit(void)
{
	set_tty_cooked();
}
	
/* ***************************************************************************
 *
 * kb_getc(), if there's a typed character waiting to be read,
 *	 return it; else return 0.
 *
 *************************************************************************** */
static unsigned char kb_getc(void) 
{
	unsigned char ch;
	ssize_t size;

	size = read (STDIN_FILENO, &ch, 1);
	if (size == 0)
	{
		return 0;
	}
	else
	{
		return ch;
	}
}

/* ***************************************************************************
 *
 * kb_getc_w(), wait for a character to be typed and return it.
 *
 *************************************************************************** */
static unsigned char kb_getc_w(void) 
{
	unsigned char ch;
	ssize_t size;


	while (1)
	{
		//usleep(20000);
		size = read (STDIN_FILENO, &ch, 1);
		if (size > 0)
		{
			break;
		}
	}
	return ch;
}

static void curs_forward(void)
{
	printf("%c%c%c",0x1b,0x5b,0x43);
}

static void curs_backward(void)
{
	printf("%c%c%c",0x1b,0x5b,0x44);
}


static void clearline(void)
{
	int i;

	printf("\r");
	for(i=0;i<len;i++){
		printf(" ");
	}
	for(i=0;i<strlen(prom);i++){
		printf(" ");
	}
	printf("\r");
}



static void set_pos(void)
{
	int i;
	printf("\r");
	for(i=0;i<strlen(prom);i++){
		curs_forward();
	}
	for(i=0;i<pos;i++){
		curs_forward();
	}

}


static void deletech(void)
{
	size_t i;

	if(pos==0) return;
	buff[LEN_MAX]='\0';
	for(i=pos-1;i<LEN_MAX;i++){
		buff[i]=buff[i+1];
	}
	pos--;
	len--;
	curs_backward();
	for(i=pos;i<=len;i++){
		printf("%c",buff[i]);
	}
	printf(" ");
	set_pos();
	return;
}

static void insertch(char ch,int shadow)
{
	size_t i;

	if(len>LEN_MAX){
		return ;
	}
	for(i=len-1;pos<=i;i--){
		buff[i+1]=buff[i];
	}
	buff[pos]=ch;
	len++;
	for(i=pos;i<len+1;i++){
		if(shadow==0){
			printf("%c",buff[i]);
		}else{
			if(i<len) printf("*");
		}
	}
	pos++;
	set_pos();
	return;
}
static void moveforward(void)
{
	if(pos>=len) return;
	pos++;
	curs_forward();
}
static void movebackward(void)
{
	if(pos==0) return;
	pos--;
	curs_backward();
}

static void histdown(void)
{
	if(tindex<=0) return;
	tindex--;
	clearline();
	memcpy(buff,hist[tindex],1024);
	buff[LEN_MAX]='\0';
	len=strlen(buff);
	printf("%s%s",prom,buff);
	pos=len;
	set_pos();
}
static void histup(void)
{
	if(tindex+1>=upper) return;
	tindex++;
	clearline();
	memcpy(buff,hist[tindex],1024);
	buff[LEN_MAX]='\0';
	len=strlen(buff);
	printf("%s%s",prom,buff);
	pos=len;
	set_pos();
}


#define	UP	1
#define	DWN	2
#define	FWD	3
#define	BWD	4
static int escape(void)
{

	unsigned char ch;
	ch = kb_getc_w();	
	if(ch!=0x5b) return -1;
	ch = kb_getc_w();
	if(ch==0x41) return UP;
	if(ch==0x42) return DWN;
	if(ch==0x43) return FWD;
	if(ch==0x44) return BWD;
	return -1;
}
static void update_hist(void)
{
	int	i;

	if(strlen(buff)==0) return;
	if(strcmp(buff,hist[0])==0) return;
	if(upper<(HIST_MAX-1)){
		upper++;
	}
	for(i=upper-1;i>0;i--){
		memcpy(hist[i],hist[i-1],1024);
	}
	memcpy(hist[0],buff,1024);
}
	

int GetString(char *ptr,size_t plen,char *prompt,int pshadow)
{
	unsigned char ch;
	int	cmd;
	int	ret;
	int	c;

	if(prompt==NULL||strlen(prompt)>1000){
		strcpy(prom,PROM);
	}else{
		strcpy(prom,prompt);
	}
	memset(buff,0,1024);
	len=0;
	pos=0;
	tindex=-1;
	ret=set_tty_raw();	 /* set up character-at-a-time */
	printf("%s",prom);
	if(ret!=-1){		
		while (1){
			//usleep(20000);		 /* 20 ms*/
			fflush(stdout);
			ch = kb_getc();	
			switch(ch){
			case 0x03 : /*^C*/
				set_tty_cooked();/* restore mode */
				printf("\n");
				return 1;
#ifndef TEST 
			case 0x09: /* TAB */
				if(pshadow==0){
					if(WordCompensation(buff)==1){
						clearline();
						buff[LEN_MAX]='\0';
						len=strlen(buff);
						printf("%s%s",prom,buff);
						pos=len;
						set_pos();
					}
				}
				break;
#endif
			case 0x0a: /* CR */
				set_tty_cooked();/*restore old mode */
				printf("\n");
				buff[len]='\0';
				strncpy(ptr,buff,plen-1);
				if(pshadow==0){
					update_hist();
				}
				return 0;
			case 0x08 : /*^H*/
			case 0x7F : /*BS*/
				tindex=-1;
				deletech();
				break;
			case 0x1b: /* ecape*/
				if(pshadow==0){
					cmd=escape();
					switch(cmd){
						case UP:
							histup();
							break;
						case DWN:
							histdown();
							break;
						case FWD:	
							moveforward();;
							tindex=-1;
							break;
						case BWD:
							movebackward();
							tindex=-1;
							break;
						default:
							break;
					}
				}
				break;
			default:
				if(isprint(ch)){
					insertch((char)ch,pshadow);
					tindex=-1;
				}
				/*else{
					printf("%02X",ch);
				}
				*/
				break;
			}
		}
	}else{
		for(len=0;;len++){
			c=getchar();
			if(c=='\n'||c==EOF||len>1000){
				buff[len]='\0';
				if(len==0){
					return -1;
				}else{
					strncpy(ptr,buff,plen-1);
					printf("%s\n",buff);
					return 0;
				}
			}else{
				buff[len]=(char)c;
			}
		}	
	}		
}

#ifdef MAIN
int main(int argc, char * argv[])
{
	char	stbuff[1024];
	for(;GetString(stbuff,1000,">",0)==0;){
		printf("str=%s\n",stbuff);
	}
}
#endif

