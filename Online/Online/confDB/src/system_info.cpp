
#if defined( _WIN32) ||defined(WIN32)
#include <winsock2.h>	
#include <stdio.h>
#else
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#endif





#if defined( _WIN32) ||defined(WIN32)

void ErrorExit(const char* FctName,char* ErrMess, int errnum) 
{ 
	if(errnum==0)
	{
		sprintf(ErrMess, "NO_ERROR in %s",FctName);
	}
	else
	{
		LPVOID lpMsgBuf;
		DWORD dw = GetLastError(); 

		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf,
			0, NULL );

		wsprintf(ErrMess, 
			"ERROR in %s failed with error %d: %s", 
			FctName, dw, lpMsgBuf); 
		LocalFree(lpMsgBuf);
		ExitProcess(dw); 
	}
}

int getTerminalName(char* hostname,int len_host,char* errMess)
{
	int res=0;
	DWORD  bufCharCount = len_host;
	if( !GetComputerName( hostname, &bufCharCount ) )
	{
		ErrorExit("getTerminalName",errMess,1);
		strcpy(hostname,"ERROR_IN_GETTING_HOST_NAME");
		res=-1;
	}
	else
	{
		ErrorExit("getTerminalName",errMess,0);
	}
	return res;
}

int getLoginUser(char* username,int len_host,char* errMess)
{
	int res=0;
	DWORD  bufCharCount = len_host;

	if( !GetUserName( username, &bufCharCount ) )
	{
		ErrorExit("getLoginUser",errMess,1);
		strcpy(username,"ERROR_IN_GETTING_LOGIN_NAME");
		res=-1;
	}
	else
	{
		ErrorExit("getLoginUser",errMess,0);
	}
	return res;
}



#else
void ErrorExit(const char* FctName,const char* logMess,char* ErrMess, int errnum) 
{ 
	if(errnum==0)
	{
		sprintf(ErrMess, "NO_ERROR in %s",FctName);
	}
	else
	{
		sprintf(ErrMess,"ERROR in %s : %s ",FctName,logMess);
	}
}


int getTerminalName(char* hostname,int len_host,char* ErrorMess)
{
	int res=0;
	int  bufCharCount =len_host;
	if( gethostname( hostname, bufCharCount )!=0)
	{
		ErrorExit("getTerminalName", "Not successful",ErrorMess,1);
		strcpy(hostname,"ERROR_IN_GETTING_HOST_NAME");
		res=-1;
	}
	else
		ErrorExit("getTerminalName", "Not successful",ErrorMess,0);

	return res;
}

int getLoginUser(char* username,int len_host,char* ErrorMess)
{
	int res=0;
	int bufCharCount = len_host;
	int rescode=0;

	uid_t curUserId=getuid();
	// get the information describing the user with this userID
	struct passwd *userData=getpwuid(curUserId);
	if (userData==0) 
	{
		ErrorExit("getLoginUser ","Not Successful ",ErrorMess,1);
		strcpy(username,"ERROR_IN_GETTING_LOGIN_NAME ");
		res=-1;
	}
	// extract the user name from this record:
	strcpy(username,userData->pw_name);
	ErrorExit("getLoginUser "," ",ErrorMess,0);
	return res;
}

#endif




