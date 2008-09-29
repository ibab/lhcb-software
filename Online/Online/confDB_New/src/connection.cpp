/********************************************************************************/
//Author: L.Abadie
// version:v3.5
// changes: handling error such that can be extended to C++: (create a class SQLException) (strlen ck + update stmt)
// file which initilazes and closes the DB connection
//rules return 0 if it's successful or if there is rows selected (it's not an error)
// otherwise we return -1
/********************************************************************************/

//#include "test33.h"
#if !defined( _TEST33_) 
#define _TEST33_
#endif

#include "list_structures.h"

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#if defined( _WIN32) ||defined (WIN32)
#define EXTERN_CONFDB  _declspec(dllexport)
#else
#define EXTERN_CONFDB 
#include <unistd.h>
#endif
#define CONFDBLIB_VERSION "v3.5"  

OCIError* ociError=0; /* the error handle */ 
OCIEnv* ociEnv=0; /* the environment handle */ 
OCISvcCtx* ociHdbc=0; /* the context handle */
OCIServer *mysrvhp=0; /* the server handle */
OCISession *myusrhp=0; /* user session handle */
/**************************************************************************************/
extern char  _list_of_subsystemname[50][10];
extern int _list_of_subsystemID[50];
extern int _nb_of_subsystem;

OCIError* get_ErrorHandle()
{
	return ociError;
};

OCIEnv* get_OCIEnv()
{
	return ociEnv;
};

OCISvcCtx* get_ContxtHdl()
{
	return ociHdbc;
}
OCIServer* get_ServerHandle()
{
	return mysrvhp;
}
OCISession *get_SessionHandle()
{
	return myusrhp;
};




//internal fct to disconnect to DB
sword Disconnect(OCIEnv* env,OCIError* err,OCIServer* mysrvhp1,OCISession* myusrhp1, OCISvcCtx* mysvchp, int count_free, char* ErrorMessage)   
{
	char appliName[100]="Disconnect";
	sword status=OCI_SUCCESS;
	
	if(count_free>4)
	{
		status=OCISessionEnd (mysvchp,err,myusrhp1,OCI_DEFAULT);
	}
	if(count_free>3)
	{
		status=OCIServerDetach (mysrvhp1,err,OCI_DEFAULT);
	}                      
	if(count_free>4)
	{
		status=OCIHandleFree(myusrhp1,OCI_HTYPE_SESSION);
	}
	if(count_free>3)
	{
		status+=OCIHandleFree(mysvchp,OCI_HTYPE_SVCCTX);
	}
	if(count_free>2)
	{
		status+=OCIHandleFree (err, OCI_HTYPE_ERROR);
	}
	if(count_free>1)
	{
		status+=OCIHandleFree(mysrvhp1,OCI_HTYPE_SERVER);
	}
	if(count_free>0)
	{
		status+=OCIHandleFree (env, OCI_HTYPE_ENV);
	}
	if(status== OCI_SUCCESS)
	{
		GetErrorMess(appliName,"DISCONNECTION SUCCESSFUL",ErrorMessage,0);
		std::cout << "OCI Connection successfully terminated...." << count_free << std::endl;
	}
	else
	{	
		GetErrorMess(appliName,"DISCONNECTION UNSUCCESSFUL",ErrorMessage,1);
		std::cout << "OCI Connection not successfully terminated...." << count_free << std::endl;
	}

	return status;
}



//internal fct to connect to DB
sword Connect(char* server,char* usr,char* pwd,OCIEnv*& myenvhp,OCIServer*& mysrvhp1,OCISession*& myusrhp1, OCISvcCtx*& mysvchp, OCIError*& myerrhp, char* ErrorMessage)      
{
	char appliName[100]="Connect";
	sword status=0;
	int count_free=0;
	int rescode=0;
	char* errmessg=(char*)malloc(ERROR_BUFFER*sizeof(char));
	if(errmessg==NULL)
	{
		GetErrorMess(appliName, "Malloc Unsuccessful",ErrorMessage,1);
		return -1;
	}
	//create an environment to handle a session:first thing to do 
	//use of the default to initiliaze the environment
	//use of OCIHandleAlloc because we want the application to support multiple connections and users so we can't use OCILogon fct
	//We initialize the session as follows
	/* initialize the mode to be not threaded and default environment */

	try{
		EnvCreate(myenvhp, &status);
		count_free++;
		//allocate a server handle
		HandleAlloc(myenvhp,(dvoid**)&mysrvhp1,OCI_HTYPE_SERVER,&status);
		count_free++;
		//allocate an error handle 
		HandleAlloc(myenvhp,(dvoid**)&myerrhp,OCI_HTYPE_ERROR,&status);
		count_free++;
		//create a server context
		ServerAttach(mysrvhp1,myerrhp,server, &status);
		//allocate a service handle
		HandleAlloc(myenvhp,(dvoid**)&mysvchp,OCI_HTYPE_SVCCTX,&status);
		count_free++;
		//set the server attribute in the service context handle
		AttrSet(mysvchp, OCI_HTYPE_SVCCTX,mysrvhp1, 0, OCI_ATTR_SERVER,myerrhp, &status);
		//allocate a user session handle 
		HandleAlloc(myenvhp,(dvoid**)&myusrhp1,OCI_HTYPE_SESSION,&status);
		//set user name attribute in user session handle
		AttrSet(myusrhp1, OCI_HTYPE_SESSION,usr,(ub4) strlen(usr), OCI_ATTR_USERNAME,myerrhp, &status);
		count_free++;
		//set password attribute in user session handle
		AttrSet(myusrhp1, OCI_HTYPE_SESSION,pwd,(ub4) strlen(pwd), OCI_ATTR_PASSWORD,myerrhp, &status);
		SessionBegin(mysvchp,myerrhp,myusrhp1,OCI_CRED_RDBMS,&status);		
		//set the user session attribute in the service context handle
		AttrSet(mysvchp, OCI_HTYPE_SVCCTX,myusrhp1,0, OCI_ATTR_SESSION,myerrhp, &status);
		
	}catch(Error err){
			rescode=ShowErrors (status, myerrhp, err.log);
			
			status=Disconnect(myenvhp,myerrhp,mysrvhp1,myusrhp1, mysvchp,count_free,errmessg);
			if(status==OCI_SUCCESS)
				status=-1;
			rescode= -1;
		}

	if(rescode==0)
	{
		OCIReportError(myerrhp,appliName,ErrorMessage,0);
		std::cout << " OCI Connection successfully established and current version of the library " << CONFDBLIB_VERSION << std::endl;
	}
	else
	{
		OCIReportError(myerrhp,appliName,ErrorMessage,1);
		std::cout << "OCI Connection not successfully established ...." <<  std::endl;
	}
	free(errmessg);
	return (rescode);
}
/**
* Load the subsystem table  returning an integer value.
* @param sysnameList :list of system_name accroding to the ECS convention.
* @param sysIDlist : user login.
* @param len_array : len_array of sysIDlist(=nb of subsystem)
* @param ErrorMessage : error message in case of failure
* @return 0 if the disconnection is successful
*/
int LoadSubsystemTable(char sysnameList[50][10],int* sysIDlist, int &len_array,char* ErrorMessage)
	{
		char selectdevtype[1000];
		char appliName[100]="LoadSubsystemTable";
		char system_name[12];
		int len_sys=12;
		int sysID=0;
		int i=0;
		int status=0;
		int rescode=0;
		OCIStmt *stmthp;
		OCIDefine* def[2];
		int prefetch_rows=100;
		int pos1=0;
		int j=0;
		char sysnameList_temp[100][10];
		int * syslist=(int*)malloc(prefetch_rows*sizeof(int));
		if(syslist==NULL)
		{
//			status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
			GetErrorMess(appliName,"Malloc unsuccessful",ErrorMessage,1);
			return -1;
		}

		try{
			HandleAlloc(ociEnv,(dvoid**)&stmthp,OCI_HTYPE_STMT,&status);
			sprintf(selectdevtype,"select system_name||'?',systemid from %s ",SUBSYSTEM_TABLE);
			StmtPrepare(stmthp,ociError, selectdevtype,&status);
			AttrSet(stmthp, OCI_HTYPE_STMT,&prefetch_rows,0, OCI_ATTR_PREFETCH_ROWS,ociError, &status);
			StmtExecute(ociHdbc, stmthp, ociError, 0, &status);
			DefineByPos(stmthp,&def[0],ociError,1, system_name,len_sys,SQLT_STR,&status);
			DefineByPos(stmthp,&def[1],ociError,2, &sysID,sizeof(sysID),SQLT_INT,&status);
												
		}catch(Error err){
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,err.msg,ErrorMessage,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}

		while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
		{
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);
			if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				RemoveSeparator(system_name, "?");
				strcpy(sysnameList_temp[i],system_name);
				syslist[i]=sysID;
				i++;
			}
		}
		
		if(i>0)
		{
			if(len_array>i)
			{
				for(j=0;j<i;j++)
				{
					sysIDlist[j]=syslist[j];
					strcpy(sysnameList[j],sysnameList_temp[j]);
				}
				len_array=i;
			}
			else
			{
				free(syslist);
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
				return rescode;
			}
		}
		else
		{
			if(rescode==0)
			{
				free(syslist);
				rescode=-1; // in this case it's an error
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"SUBSYSTEM_TABLE_EMPTY",ErrorMessage,1);
				return rescode;
			}
		}
		free(syslist);
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(rescode==0)
		{
			OCIReportError(ociError,appliName,ErrorMessage,0); 
		}
		else
		{
			OCIReportError(ociError,appliName,ErrorMessage,1); 
		}
		return rescode;

	}


EXTERN_CONFDB
int DBConnexion(char* server,char* usr,char* pwd,char* ErrorMessage)
{
	int status;
	char appliName[100]="LoadSubsystemTable";
	int rescode=0;
	_nb_of_subsystem=50;
	status=Connect(server,usr,pwd,ociEnv,mysrvhp,myusrhp, ociHdbc, ociError,ErrorMessage);	
	if(status!=0)
	{
		ociError=0; /* the error handle */ 
		ociEnv=0; /* the environment handle */ 
		ociHdbc=0; /* the context handle */
		mysrvhp=0; /* the server handle */
		myusrhp=0; /* user session handle */
	}
	else
	{
		status=LoadSubsystemTable(_list_of_subsystemname,_list_of_subsystemID,_nb_of_subsystem,ErrorMessage);
		if(status!=0)
		{
			ociError=0; /* the error handle */ 
			ociEnv=0; /* the environment handle */ 
			ociHdbc=0; /* the context handle */
			mysrvhp=0; /* the server handle */
			myusrhp=0; /* user session handle */
			GetErrorMess(appliName,"SUBSYSTEM_TABLE_EMPTY",ErrorMessage,1);
			return -1;
		}
		else
		{
			std::cout<<"Successful Loading "<<std::endl;
		}
	}
	return status;
}
/**
* Disconnect from the database  returning an integer value.
* @param ErrorMessage : error message in case of failure
* @return 0 if the disconnection is successful
*/

EXTERN_CONFDB
	int DBDeconnexion(char* ErrorMessage)
{
	int status;
	status=Disconnect(ociEnv,ociError,mysrvhp,myusrhp, ociHdbc,5,ErrorMessage);
	ociError=0; /* the error handle */ 
	ociEnv=0; /* the environment handle */ 
	ociHdbc=0; /* the context handle */
	mysrvhp=0; /* the server handle */
	myusrhp=0; /* user session handle */
	return status;
}


#if defined(__cplusplus) || defined(c_plusplus)
}
#endif