#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dic.h>

int received = 0;
char str[132] = {'\0'};

void rout(tag, code)
int *tag;
int *code;
{
  int silent;

  silent = *tag;
  if(!silent)
      dim_print_date_time();
  if(*code)
  {
	  if(!silent)
		printf(" Command %s Successfully Sent\n", str);
  }
  else
  {
	  if(!silent)
		printf(" Command %s Unsuccessfull\n", str);
  }
  received = 1;
#ifdef WIN32
  wake_up();
#endif
}
/*
main(argc,argv)
int argc;
char **argv;                    
{
  char data[1024] = {'\0'};
  int silent = 0;

	if(argc < 2)
	{
		printf("Command Name > ");
		fflush(stdout);
		scanf("%s", str);
		printf("Command String > ");
		fflush(stdout);
		scanf("%s", data);
	}
	else
	{
		sprintf(str,argv[1]);
		if(argc < 3)
			data[0] = '\0';
		else
		{
			sprintf(data,argv[2]);
			if(argc > 3)
				silent = 1;
		}
	}
	dic_cmnd_callback(str,data,(int)strlen(data)+1, rout, silent);
	while(!received)
	  dim_wait();
	sleep(1);
}
*/
int main(argc,argv)
int argc;
char **argv;                    
{
int i;
int silent = 0;
char data[1024] = {'\0'};
int data_int, data_int_flag = 0;
char dns_node[128], *ptr;
int dns_port = 0;

	dns_node[0] = '\0'; 
	for(i = 1; i < argc; i++)
	{
		if(!strcmp(argv[i],"-dns"))
		{
			strcpy(dns_node,argv[i+1]);
			if((ptr = strchr(dns_node,':')))
			{
				*ptr = '\0';
				ptr++;
				sscanf(ptr,"%d",&dns_port);
			}
			i++;
		}
		else if(!strcmp(argv[i],"-s"))
		{
			silent = 1;
		}
		else if(!strcmp(argv[i],"-i"))
		{
			data_int_flag = 1;
		}
		else
		{
			if(!str[0])
			{
				strcpy(str, argv[i]);
			}
			else if(!data[0])
			{
				strcpy(data,argv[i]);
			}
		}
	}
	if(dns_node[0])
	{
		dic_set_dns_node(dns_node);
	}
	if(dns_port)
	{
		dic_set_dns_port(dns_port);
	}
	if(!str[0])
	{
		printf("dim_send_command: Insufficient parameters\n");
		printf("usage: dim_send_command <cmnd_name> [<data>] [-dns <dns_node>] [-s] [-i]\n");
		exit(0);
	}
	if(!data[0])
		data[0] = '\0';
	if(data_int_flag)
	{
		sscanf(data,"%d",&data_int);
		dic_cmnd_callback(str,&data_int,sizeof(int), rout, silent);
	}
	else
	{
		dic_cmnd_callback(str,data,(int)strlen(data)+1, rout, silent);
	}
	while(!received)
	  dim_wait();
	sleep(1);
	return(1);
}
