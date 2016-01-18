#include <ctype.h>
#include <stdio.h>
#include <dic.h>

int no_link = 0x0afefead;
int version;
	
char str[132];
int type, mode;
int received = 0;

void rout(tag, buffer, size)
int *tag, *size;
int *buffer;
{
void print_service();

	if(tag){}
	if((*size == 4 ) && (*buffer == no_link))
	{
		printf("Service %s Not Available\n", str);
	}
	else
	{
		printf("Service %s Contents :\n", str);
		print_service(buffer, ((*size - 1)/4) + 1);
	}
	fflush(stdout);
	received = 1;
#ifdef WIN32
	wake_up();
#endif
}

int main(argc,argv)
int argc;
char **argv;                    
{

	if(argc < 2)
	{
		printf("Service Name > ");
		fflush(stdout);
		scanf("%s", str);
	}
	else
	{
		sprintf(str,"%s",argv[1]);
	}
	dic_info_service(str,ONCE_ONLY,60,0,0,rout,0,&no_link,4);
	while(!received)
	  dim_wait();
	return(1);
}

void print_service(buff, size)
int *buff, size;
{
int i,j;
char *asc;
int last[4];

	asc = (char *)buff;
	for( i = 0; i < size; i++)
	{
		if(!(i%4))
			printf("H");
		printf("   %08X ",buff[i]);
		last[i%4] = buff[i];
		if(i%4 == 3)
		{
			printf("    '");
			for(j = 0; j <16; j++)
			{
				if(isprint(asc[j]))
					printf("%c",asc[j]);
				else
					printf(".");
			}
			printf("'\n");
			for(j = 0; j <4; j++)
			{
				if(j == 0)
					printf("D");
				printf("%11d ",last[j]);
			}
			printf("\n");
			asc = (char *)&buff[i+1];
		}
	}
	if(i%4)
	{

			for(j = 0; j < 4 - (i%4); j++)
				printf("            ");
			printf("    '");
			for(j = 0; j < (i%4) * 4; j++)
			{
				if(isprint(asc[j]))
					printf("%c",asc[j]);
				else
					printf(".");
			}
			printf("'\n");
			for(j = 0; j < (i%4); j++)
			{
				if(j == 0)
					printf("D");
				printf("%11d ",last[j]);
			}
			printf("\n");
	}
}
