#include <dis.hxx>
#include <sys/stat.h>
#include <sys/types.h>
#ifndef WIN32
#include <sys/wait.h>
#include <unistd.h>
#else
#include <process.h>
#endif
#include <ctype.h>

#ifndef WIN32
char outfile[] = "/tmp/dim_rsh_server.dat";
#else
char outfile[] = "c:\\dim_rch_server.dat";
#endif

int my_system(char *);

class Cmd: public DimCommand
{
	char *result;
	DimService *resultSrvc;

	void commandHandler();
public:
	Cmd(char *cmdName, char *resultName): DimCommand(cmdName, "C") 
	{
		result = new char[10];
		strcpy(result,"empty");
		resultSrvc = new DimService(resultName, "C", result, strlen(result)+1);
	};
};

void Cmd::commandHandler()
{
	char *str, *client;
	char commandString[256];
	struct stat buf;
	FILE *fd;
	int ret, size, index, sz;

	str = getString();

	client = DimServer::getClientName();

	cout << "Received " << str << " from " << client << endl;
	unlink(outfile);
	strcpy(commandString,str);
#ifndef WIN32
	strcat(commandString," >& ");
	strcat(commandString,outfile);
	strcat(commandString,"< /dev/null");
#else
	strcat(commandString," > ");
	strcat(commandString,outfile);
	strcat(commandString," 2>&1 ");
#endif
	my_system(commandString);

	delete result;
	ret = stat(outfile, &buf);
	if(ret == -1)
    {
		result = new char[20];
		strcpy(result,"File does not exist");
    }
	else
    {
		size = buf.st_size;
		result = new char[size +1];
		fd = fopen(outfile, "r");
		index = 0;
		while(!feof(fd) && size)
		{
			sz = fread(&result[index], 1, 512, fd);
			size -= sz;
			index += sz;
			if(!sz)
			break;
		}
		fclose(fd);
		result[index] = '\0';
	}
	resultSrvc->updateService(result, strlen(result)+1);
}

#ifndef WIN32

extern char **environ;

int my_system (char *command)
{
    int pid, status, ret, n = 0;

    if (command == 0)
        return 1;
    pid = fork();
    if (pid == -1)
        return -1;
    if (pid == 0) {
        char *argv[4];
        argv[0] = "sh";
        argv[1] = "-c";
        argv[2] = command;
        argv[3] = 0;
        execve("/bin/sh", argv, environ);
        exit(127);
    }
    do {
      ret = waitpid(pid, &status,WNOHANG);
      if(ret == -1)
	break;
      usleep(100000);
      n++;
    } while(n < 1200);
    return status;
}

#else
int my_system (char *command)
{
	return system(command);
}

#endif

#ifdef WIN32
int init_sock()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	static int sock_init_done = 0;

	if(sock_init_done) return(1);
 	wVersionRequested = MAKEWORD( 2, 0 );
	err = WSAStartup( wVersionRequested, &wsaData );

	if ( err != 0 ) {
    	return(0);
	}

	if ( LOBYTE( wsaData.wVersion ) != 2 ||
        HIBYTE( wsaData.wVersion ) != 0 ) {

	    WSACleanup( );
    	return(0); 
	}
	sock_init_done = 1;
	return(1);
}
#endif

void main(int argc, char **argv)
{
	char host[64], cmd_name[128], result_name[128];
	Cmd *cmd;

#ifdef WIN32
	init_sock();
#endif
	gethostname(host, 64);

//Setup command reception and result publishing
	sprintf(cmd_name,"%s/ExecuteCmd",host);
	sprintf(result_name,"%s/CmdResult", host);
	cmd = new Cmd(cmd_name, result_name);

	sprintf(cmd_name,"%s_server",host);
	DimServer::start(cmd_name);
	while(1)
    {
		pause();
    }
}
