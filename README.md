# getstring
## Description
This a front end library for an application having CLI (Command Line Interface) on POSIX OS like Linux or MacOSX. You can replace the gets() or scanf() functions of the C standard library with the GetString() function of this library when you want to get a command line string from user CLI.  
This library also provides you with **history**, **completion** and **candidate** functions. In addition to, the "login:" prompting is available. In the login prompt mode,

## Example Build
This libray consists of three source files of C language. The shell.c has an example main function and it is compiled only if you give -DMAIN option to gcc. Just enter make command without option to build example application shell.  
`$ make`   
Please do not remove -DMAIN option when you use the shell.c for a library.
Invoke the built example application, shell.  
`$ ./shell`
`login:`  
Then login prompt is displayed.  
`login:*****`  
`=>`  
The password is 'admin'. The characters you entered are masked with asterisks. If you are prompted with '=>',  the authentication was successful. When the wrong password is given, the console blocks you for 2 seconds. If you push any key on your keyboard, no character is echo backed including 'ctl-C' during blocking.  
### command grammar
At first, you has to define the command grammar of your system. In this example, it is defined as the following table. You may write down your own grammar in the rule.c and rule.h.

|cmd#| col1 | col2 |  col3| col4 |
|:-------|------:|:--------:|:--------:|
|1001|show   |status |
|1002|show   |agent |   < id >    |
|1003 |show   |agent |traf| < id > |
|1004|show   |agent |route| < id >|
|1005 |show   |terminal |  < id >|
|1005 |show   |traf |
|2001 |connect|open | < id > |
|2002 |connect|open | < id > |
|3001|add|agent| < id > |
|3002|add|terminal| < id > |
|4001|delete|agent| < id > |
|4002|delete|terminal| < id > |
|8000|log|
|9000|exit|

## Functions
### candidate
Enter *tab* key on the prompt '=>', then the candidate commands are indicated.  
`=>*tab*`   
`show connect add delete log exit `  
If you enter a word, 'show' and *tab* key, then subcommands of the show command are  
`=>show *tab*`    
`status agent traf terminal `  
These subcommands are acceptable only if the command is 'show'.

### completion
Enter *tab*  following 's' key on the prompt '=>' , and then the s is replaced with the word 'show' automatically. This is the completion function.  
`=>s *tab* `  
`=>show`  
When you would like to enter 'delete terminal' command, you don't have to enter 14 letters.  Instead only four keys "d *tab* t *tab*" are enough.

### history
After entering several command, you can call the command line you entered previously when pressing the up arrow key. The command line is editable and you can move the cursor by the right and left arrow keys. When you have to enter the same command repeatedly such like show status, this historical function is helpful.

## Arrangement
### key words
After definition of your command grammar, extract reserved key words from it. The words must be numbered in the enum W_WORD and registered to the WORD_LIST array in the rule.c.
## grammar
Each command line in the above grammar table has the command ID shown in the cmd# column. This ID are declared in the rule.h. You may arrange these ID according your own command grammar. These IDs are not ordinal but cardinal, so they don't have to be sequential as long as if they are unique.  
Finally, you must declare each command line in the tree of COMMAND_TREE structure in the rule.c. Each word sequence must be terminated with W_E(=0)and the bottom line is marked with W_B(=-1).

## Usage
### APIs
***proto typing***   
`int GetString(char *ptr,size_t plen,char *prompt,int pshadow);`
***description***  
Get one string from user. it returns when user presses the enter key. This means completion, comlete and history functions are supported in this API.
***parameters***  
ptr: the buffer address in which the string is stored.  
plen: the size of the buffer. It is recommended more than 1KB.
prompt: command prompt string.
pshadow: 0=normal mode, 1=password mode
***returns***  
0=successful. A string has be got.
1=Ctl-C was entered.  

***proto typing***   
`void SetCloseConsole(unsigned int sec);`  
***description***  
Block the console during the specified seconds. When an authentication failed, it is recommended to invoke this function.
***parameters***  
sec: sleeping time as second.  

***proto typing***   
`int AnalyzeCmdLine(char *ptr,char params[COL_MAX][PARAM_LEN]);`  
***description***  
Converting the entered string to command ID.
When the user enter a string 'connect open 124 567', this API convert this string to ID, 2001 and store parameters '123' and '567' to two dimension string array in this sample. You don't have to build your command analyzer.  
***parameters***  
ptr:command string which is got through the GetString API.
params: two dimension array for chars in which parameters are stored.  *COL_MAX* is the maximum number of the parameters and *PARAM_LEN* is the maximum length of each parameter.  
***returns***   
Command ID.

## Sample Usage
The sample main function in the shell.c might be useful for your application.
When login prompt, the forth parameter of GetString API is set 1. If the authentication gets the successful result, the loop is broken. Otherwise the console is blocked during 2 sec.

```
for(;;){
  if(GetString(stbuff,1000,"login:",1)==1){
    exit(1);
  }   
  if(strcmp(stbuff,"admin")==0){
    break;
  }   
  SetCloseConsole(2);
}
```
After the logged in, the normal prompt is provided to the user. When a string retrieved, you can convert it to command ID and extract parameters to params array by the AnalyzeCmdLine API.
```
for(;;){
    if(GetString(stbuff,1000,"=>",0)==1){
      printf("enter \"exit\" to quit\n");
      continue;
    }   
    printf("str=%s|\n",stbuff);
    for(int i=0;i<COL_MAX;i++) params[i][0]=0;
    cmd=AnalyzeCmdLine(stbuff,params);  
    printf("cmd=%d\r\n",cmd);
    for(int i=0;i<COL_MAX && params[i][0];i++){
      printf("P%d:%s|\n",i+1,params[i]);
    }
    /*
    Call your Service Functions at this place.
    */
    if(cmd==CMD_EXIT)
      break;
  }
```
## Remarks
- All strings are output to the standard output int this library. If you would like remote access to your application through network, there are major two ways to modified this library. One is to implement this library on the client side application. Another is on server side.
In the first one, this library can be used as it is. On the other hand, the modification of this library becomes more complex because the standard out put is no longer used.
