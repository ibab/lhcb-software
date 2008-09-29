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



	//char  _list_of_subsystemname[50][10];
	//int _list_of_subsystemID[50];
	//int _nb_of_subsystem=50;
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
		sword errcode=0;

		if(count_free>4)
		{
			status=OCISessionEnd (mysvchp,err,myusrhp1,OCI_DEFAULT);
			//std::cout<<"OCISessionEnd and status="<<status<<std::endl;
		}
		if(count_free>3)
		{
			status=OCIServerDetach (mysrvhp1,err,OCI_DEFAULT);
			//std::cout<<"OCIServerDetach and status="<<status<<std::endl;
		}                      



		if(count_free>4)
		{
			status=OCIHandleFree(myusrhp1,OCI_HTYPE_SESSION);
			//std::cout<<"OCIHandleFree 1 and status="<<status<<std::endl;

		}
		if(count_free>3)
		{
			status+=OCIHandleFree(mysvchp,OCI_HTYPE_SVCCTX);
			//std::cout<<"OCIHandleFree 2 and status="<<status<<std::endl;

		}
		if(count_free>2)
		{
			status+=OCIHandleFree (err, OCI_HTYPE_ERROR);
			//std::cout<<"OCIHandleFree 3 and status="<<status<<std::endl;

		}
		if(count_free>1)
		{
			status+=OCIHandleFree(mysrvhp1,OCI_HTYPE_SERVER);
			//std::cout<<"OCIHandleFree 4 and status="<<status<<std::endl;

		}
		if(count_free>0)
		{
			status+=OCIHandleFree (env, OCI_HTYPE_ENV);
			//std::cout<<"OCIHandleFree 5 and status="<<status<<std::endl;

			//status+=OCITerminate ( OCI_DEFAULT );
			//std::cout<<"OCITerminate 2 and status="<<status<<std::endl;

		}
		if(status== OCI_SUCCESS)
		{

			//OCIReportError(err,appliName,ErrorMessage,0);
			GetErrorMess(appliName,"DISCONNECTION SUCCESSFUL",ErrorMessage,0);
			std::cout << "OCI Connection successfully terminated...." << count_free << std::endl;
		}
		else
		{	
			//OCIReportError(err,appliName,ErrorMessage,1);
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
		sword errcode=0;
		int count_free=0;
		int rescode=0;
		int res_win=0;
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
		status=OCIEnvCreate(&myenvhp, OCI_DEFAULT, (dvoid *)0,0, 0, 0, (size_t) 0, (dvoid **)0);
		if(status!= OCI_SUCCESS)
		{
			char log[1000];
			ShowErrors ( status, myerrhp, log) ;
			count_free++; //1
			status=Disconnect(myenvhp,myerrhp,mysrvhp1,myusrhp1, mysvchp,count_free,errmessg);
			if(status==OCI_SUCCESS)
				status=-1;
			rescode= -1;
		}
		else
		{
			/* allocate a server handle */
			count_free++;
			status=OCIHandleAlloc ((dvoid *)myenvhp, (dvoid **)&mysrvhp1,OCI_HTYPE_SERVER, 0, (dvoid **) 0);
		}

		if(status!= OCI_SUCCESS )
		{ 
			if(rescode==0)
			{
				count_free++;  //2
				status=Disconnect(myenvhp,myerrhp,mysrvhp1,myusrhp1, mysvchp,count_free,errmessg);
				if(status==OCI_SUCCESS)
					status=-1;
				rescode= -1;
			}
		}
		else
		{
			/* allocate an error handle */
			count_free++;
			status=OCIHandleAlloc ((dvoid *)myenvhp, (dvoid **)&myerrhp,OCI_HTYPE_ERROR, 0, (dvoid **) 0);
		}

		if(status!= OCI_SUCCESS)
		{ 
			if(rescode==0)
			{
				count_free++;  //3
				status=Disconnect(myenvhp,myerrhp,mysrvhp1,myusrhp1, mysvchp,count_free,errmessg);
				if(status==OCI_SUCCESS)
					status=-1;
				rescode= -1;
			}

		}
		else
		{
			/* create a server context */
			count_free++;
			status=OCIServerAttach (mysrvhp1, myerrhp, (text *)server,strlen(server), OCI_DEFAULT);
		}

		if(status!= OCI_SUCCESS)
		{ 

			if(rescode==0)
			{
				status=Disconnect(myenvhp,myerrhp,mysrvhp1,myusrhp1, mysvchp,count_free,errmessg);
				if(status==OCI_SUCCESS)
					status=-1;
				rescode= -1;
			}
		}
		else
		{
			/* allocate a service handle */
			status=OCIHandleAlloc ((dvoid *)myenvhp, (dvoid **)&mysvchp,OCI_HTYPE_SVCCTX, 0, (dvoid **) 0);
		}

		if(status!= OCI_SUCCESS)
		{ 
			if(rescode==0)
			{
				count_free++;  //4
				status=Disconnect(myenvhp,myerrhp,mysrvhp1,myusrhp1, mysvchp,count_free,errmessg);
				if(status==OCI_SUCCESS)
					status=-1;
				rescode= -1;

			}

		}
		else
		{
			count_free++;
			/* set the server attribute in the service context handle*/
			status=OCIAttrSet ((dvoid *)mysvchp, OCI_HTYPE_SVCCTX,(dvoid *)mysrvhp1, (ub4) 0, OCI_ATTR_SERVER, myerrhp);
		}

		if(status!= OCI_SUCCESS)
		{ 
			if(rescode==0)
			{
				status=Disconnect(myenvhp,myerrhp,mysrvhp1,myusrhp1, mysvchp,count_free,errmessg);
				if(status==OCI_SUCCESS)
					status=-1;
				rescode= -1;
			}
		}
		else
		{
			/* allocate a user session handle */
			status=OCIHandleAlloc ((dvoid *)myenvhp, (dvoid **)&myusrhp1,OCI_HTYPE_SESSION, 0, (dvoid **) 0);
		}

		if(status!= OCI_SUCCESS)
		{ 
			if(rescode==0)
			{
				count_free++;//5
				status=Disconnect(myenvhp,myerrhp,mysrvhp1,myusrhp1, mysvchp,count_free,errmessg);

				if(status==OCI_SUCCESS)
					status=-1;
				rescode= -1;
			}

		}
		else
		{
			/* set user name attribute in user session handle */
			status=OCIAttrSet ((dvoid *)myusrhp1, OCI_HTYPE_SESSION,(dvoid *)usr, (ub4)strlen(usr),OCI_ATTR_USERNAME, myerrhp);
			count_free++;
		}


		if(status!= OCI_SUCCESS)
		{ 
			if(rescode==0)
			{
				status=Disconnect(myenvhp,myerrhp,mysrvhp1,myusrhp1, mysvchp,count_free,errmessg);
				if(status==OCI_SUCCESS)
					status=-1;
				rescode= -1;
			}
		}
		else
		{
			/* set password attribute in user session handle */
			status=OCIAttrSet ((dvoid *)myusrhp1, OCI_HTYPE_SESSION,(dvoid *)pwd, (ub4)strlen(pwd),OCI_ATTR_PASSWORD, myerrhp);
		}

		if(status!= OCI_SUCCESS)
		{ 
			if(rescode==0)
			{
				status=Disconnect(myenvhp,myerrhp,mysrvhp1,myusrhp1, mysvchp,count_free,errmessg);
				if(status==OCI_SUCCESS)
					status=-1;
				rescode= -1;
			}
		}
		else
			status=OCISessionBegin(mysvchp, myerrhp, myusrhp1,OCI_CRED_RDBMS, OCI_DEFAULT);

		//if(status!= OCI_SUCCESS)
		if(status!= OCI_SUCCESS && status!= OCI_SUCCESS_WITH_INFO)
		{
			char log[1000];
			ShowErrors ( status, myerrhp, log) ;
			if(rescode==0)
			//if((status!= OCI_SUCCESS_WITH_INFO)&&(rescode==0))///////////
			{
				status=Disconnect(myenvhp,myerrhp,mysrvhp1,myusrhp1, mysvchp,count_free,errmessg);
				if(status==OCI_SUCCESS)
					status=-1;
				rescode= -1;
			}
		}
		
		else
		{
			if(status==OCI_SUCCESS_WITH_INFO){
				//char log[100];
				ShowErrors ( status, myerrhp, "") ;
			}

			/* set the user session attribute in the service context handle*/
			status=OCIAttrSet ( (dvoid *)mysvchp, OCI_HTYPE_SVCCTX,(dvoid *)myusrhp1, (ub4) 0, OCI_ATTR_SESSION, myerrhp);
		}

		if(status!= OCI_SUCCESS)// && status!= OCI_SUCCESS_WITH_INFO)/////////////
		{ 
			if(rescode==0)
			{
				status=Disconnect(myenvhp,myerrhp,mysrvhp1,myusrhp1, mysvchp,count_free,errmessg);
				if(status==OCI_SUCCESS)
					status=-1;
				rescode= -1;
			}
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
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		if(status!=OCI_SUCCESS)
		{	
			rescode=ShowErrors (status, ociError, "OCIStmtHAndleAlloc unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrorMessage,1);
			return -1;
		}
		char sysnameList_temp[100][10];
		int * syslist=(int*)malloc(prefetch_rows*sizeof(int));
		if(syslist==NULL)
		{
			status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
			GetErrorMess(appliName,"Malloc unsuccessful",ErrorMessage,1);
			return -1;
		}
		sprintf(selectdevtype,"select system_name||'?',systemid from %s ",SUBSYSTEM_TABLE);
		status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevtype,(ub4) strlen(selectdevtype),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=	OCIAttrSet (stmthp,OCI_HTYPE_STMT,&prefetch_rows,0,OCI_ATTR_PREFETCH_ROWS,ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIBindByPos unsuccessful");
			}
		}
		else
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[0], ociError, 1,(ub1*) system_name,len_sys, SQLT_STR, 0, 0, 0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[1], ociError, 2,(ub1*)&sysID,sizeof(int), SQLT_INT, 0, 0, 0, OCI_DEFAULT);
		while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
		{
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);
			if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				pos1=strcspn(system_name,"?");
				system_name[pos1]='\0';
				strcpy(sysnameList_temp[i],system_name);
				syslist[i]=sysID;
				i++;
			}
		}
		//std::cout<<"value of system_name"<<system_name<<" and value of sysID="<<sysID<<std::endl;
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
				//std::cout<<"Connection aborted : couldn't load subsystem table"<<std::endl;
				GetErrorMess(appliName,"SUBSYSTEM_TABLE_EMPTY",ErrorMessage,1);
				return -1;
			}
			else
			{
				std::cout<<"Successful Loading "<<std::endl;
				//std::cout<<"value of =_nb_of_subsystem"<<_nb_of_subsystem<<std::endl;
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
/*
class CICDBGuard
{
public:
static CICDBGuard g_Guard;
virtual ~CICDBGuard();
};

//extern "C" OCITerminate();

CICDBGuard CICDBGuard::g_Guard;

CICDBGuard::~CICDBGuard()
{
std::cout<<"Guard is terminating..."<<std::endl;
OCITerminate(OCI_DEFAULT);
};
*/