/********************************************************************************/
//	Author: L.Abadie
// version:v3.5
// changes: handling error such that can be extended to C++: (create a class SQLException) (strlen ck + update stmt)
// file which performs bulk collect insert for the macIP table
// N.B : Memory management (allocation+release) should be handled by the user application thus, for all the fct provided by this lib
/********************************************************************************/


#include "list_structures.h"
#include "system_info.h"
#include "CDB.h"

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif


#if defined( _WIN32) ||defined(WIN32)
#define EXTERN_CONFDB  _declspec(dllexport)
#else
#define EXTERN_CONFDB extern
#endif


	extern   OCIError* ociError ;
	extern   OCIEnv* ociEnv ;
	extern   OCISvcCtx* ociHdbc ;
	extern   OCIServer *mysrvhp;
	extern   OCISession *myusrhp; 

/**
* Update MAC entries,  returning an integer value.
* The user should manage the memory : there is no memory allocation.
* @param ip_add : key entry : you want to update the info about this IPaddress
* @param subnet_mask : new value : put  "none" if no changes
* @param ipname : new value : put  "none" if no changes
* @param first_time1 : 1 if it's your first MacIP insert (save it into the database), 0 otherwise
* @param last_rows1 : 1 if it's your last MacIP insert (save it into the database), 0 otherwise
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
*/ 

EXTERN_CONFDB
	int UpdateMultipleAttributeMacIPs(char* ip_add,char* subnet_mask,char* ipname,int first_time,int last_rows,char* ErrMess)
	{
		char appliName[100]="UpdateMultipleAttributeMacIPs";
		char sqlstmt[2000];
		OCIStmt *hstmt;
		OCIBind  *bndp[4]; 
		int rescode=0;
		int res_query=0;
		sword status=0;
		int i=0;

		char* ipadd=NULL;
		char* iname=NULL;
		char* submask=NULL;
				
		int* ipadd_nullList=NULL;
		int* iname_nullList=NULL;
		int* submask_nullList=NULL;
				
		static int FIRST_TIME=0;
		int force_insert=0;
		int* numrows_inserted=NULL;
		int numrows=0;

		static int NbElement;
		macIP** macIPList;
		int max_ipaddlen=0;
		int max_inamelen=0;
		int max_submasklen=0;
				
		if (first_time==1)
		{
			FIRST_TIME=1;
			NbElement=1;
			macIPList=(macIP**)malloc( NbElement*sizeof(macIP));			
		}
		else 
		{
			if (FIRST_TIME==0&&force_insert==0)
			{
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
				return -1;
			}
			NbElement++;
			macIPList=(macIP**)realloc(macIPList, NbElement*sizeof(macIP));
		}

		macIPList[NbElement-1]=new macIP(ip_add,subnet_mask,ipname);
		
		if(NbElement==MAXROWS && last_rows!=1)
		{
			force_insert=1;
			last_rows=1;
		}

		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME==1)
		{
			int len_host=LOGIN_LEN;
			char login[LOGIN_LEN];
			char host[LOGIN_LEN];
			char* errmessg1=(char*)malloc(ERROR_BUFFER*sizeof(char));

			if(errmessg1==NULL)
			{
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				return -1;
			}
			char* errmessg2=(char*)malloc(ERROR_BUFFER*sizeof(char));
			if(errmessg2==NULL)
			{
				free(errmessg1);
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				return -1;
			}
			res_query=getTerminalName(host,len_host,errmessg1);
			len_host=LOGIN_LEN;
			res_query=getLoginUser(login,len_host,errmessg2);
			if(strncmp("NO_ERROR",errmessg1,8)==0 && strncmp("NO_ERROR",errmessg2,8)==0 )
			{
				free(errmessg1);
				free(errmessg2);
				rescode=0;
			}
			else
			{
				status=-1;
				rescode=-1;
				if(strncmp("NO_ERROR",errmessg1,8)!=0)
				{
					strcpy(ErrMess,errmessg1);
					rescode=-2;
				}
				if(strncmp("NO_ERROR",errmessg2,8)!=0)
				{
					if(rescode==-2)
						sprintf(ErrMess,"%s , %s",errmessg1,errmessg2);
					else
						strcpy(ErrMess,errmessg2);
				}
				FIRST_TIME=0;
		
				if(ipadd!=NULL)
					free(ipadd);
				if(iname!=NULL)
					free(iname);
				if(submask!=NULL)
					free(submask);
				if(ipadd_nullList!=NULL)
					free(ipadd_nullList);
				if(iname_nullList!=NULL)
					free(iname_nullList);
				if(submask_nullList!=NULL)
					free(submask_nullList);
				
				for(i=0;i<NbElement;i++)
					delete macIPList[i];
				if(macIPList!=NULL)
					free(macIPList);
				
				free(errmessg1);
				free(errmessg2);
				return -1;
			}
			for(i=0;i<NbElement;i++){
				if (macIPList[i]->ipaddlen > max_ipaddlen)
					max_ipaddlen=macIPList[i]->ipaddlen;
				if (macIPList[i]->inamelen > max_inamelen)
					max_inamelen=macIPList[i]->inamelen;
				if (macIPList[i]->submasklen > max_submasklen)
					max_submasklen=macIPList[i]->submasklen;				
			}

			ipadd=(char*)malloc(NbElement*max_ipaddlen*sizeof(char));
			iname=(char*)malloc(NbElement*max_inamelen*sizeof(char));
			submask=(char*)malloc(NbElement*max_submasklen*sizeof(char));

			ipadd_nullList=(int*)malloc(NbElement*sizeof(int));
			iname_nullList=(int*)malloc(NbElement*sizeof(int));
			submask_nullList=(int*)malloc(NbElement*sizeof(int));
			
			if(ipadd!=NULL && ipadd_nullList!=NULL && iname!=NULL && iname_nullList!=NULL && submask!=NULL && submask_nullList!=NULL)
			for(i=0;i<NbElement;i++)
			{
				memcpy(ipadd+i*max_ipaddlen,macIPList[i]->ipadd,macIPList[i]->ipaddlen);
				ipadd_nullList[i]=macIPList[i]->ipadd_null;
				memcpy(iname+i*max_inamelen,macIPList[i]->iname,macIPList[i]->inamelen);
				iname_nullList[i]=macIPList[i]->iname_null;
				memcpy(submask+i*max_submasklen,macIPList[i]->submask,macIPList[i]->submasklen);
				submask_nullList[i]=macIPList[i]->submask_null;				
			}
			numrows_inserted=(int*)malloc(sizeof(int)*NbElement);

			if(ipadd==NULL || ipadd_nullList==NULL || iname==NULL || iname_nullList==NULL || submask==NULL || submask_nullList==NULL || numrows_inserted==NULL)
			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				
				FIRST_TIME=0;
				if(ipadd!=NULL)
					free(ipadd);
				if(iname!=NULL)
					free(iname);
				if(submask!=NULL)
					free(submask);
				if(ipadd_nullList!=NULL)
					free(ipadd_nullList);
				if(iname_nullList!=NULL)
					free(iname_nullList);
				if(submask_nullList!=NULL)
					free(submask_nullList);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				
				for(i=0;i<NbElement;i++)
					delete macIPList[i];
				if(macIPList!=NULL)
                    free(macIPList);
				
				NbElement=0;				
				return -1;
			}
			else
			{
				for(i=0;i<NbElement;i++)
					numrows_inserted[i]=0;
			}			

			try{
                HandleAlloc(ociEnv,(dvoid**)&hstmt,OCI_HTYPE_STMT,&status);
				sprintf(sqlstmt,"BEGIN  update %s set subnet_info=decode(nvl(:submask,'none'),'none',subnet_info,:submask),ipname=decode(nvl(:ipname,'none'),'none',ipname,:ipname),LAST_UPDATE=sysdate,author='%s' where ipaddress=:ipadd; :numrows:=%s; end; ",IPINFO_TABLE,login,SQLROWCOUNT);
				StmtPrepare(hstmt,ociError, sqlstmt, &status);

				BindByName(hstmt,&bndp[0],ociError,":ipadd",ipadd,max_ipaddlen,SQLT_STR,&ipadd_nullList[0],&status);
				BindByName(hstmt,&bndp[1],ociError,":submask",submask,max_submasklen,SQLT_STR,&submask_nullList[0],&status);
				BindByName(hstmt,&bndp[2],ociError,":ipname",iname,max_inamelen,SQLT_STR,&iname_nullList[0],&status);
				BindByName(hstmt,&bndp[3],ociError,":numrows",&numrows_inserted[0],sizeof(int),SQLT_INT,0,&status);

				BindArrayOfStruct(bndp[0],ociError,max_ipaddlen,sizeof(int),&status);
				BindArrayOfStruct(bndp[1],ociError,max_submasklen,sizeof(int),&status);				
				BindArrayOfStruct(bndp[2],ociError,max_inamelen,sizeof(int),&status);
				BindArrayOfStruct(bndp[3],ociError,sizeof(int),0,&status);
				
				StmtExecute(ociHdbc, hstmt, ociError,NbElement, &status);
			}catch(Error err){
				sprintf(appliName,"UpdateMultipleAttributeMacIPs");	///////
				rescode=ShowErrors (status, err.ociError, err.log);
				
				if(ociError!=0)
					OCIReportError(ociError,appliName,ErrMess,1); 
				else
					GetErrorMess(appliName,err.msg,ErrMess,1);
			}
			
			numrows=-1;
			for(i=0;i< NbElement;i++)
			{
				if(numrows_inserted[i]==0)
				{
					numrows=0;
					i= NbElement+5;
				}
				else
					numrows=numrows_inserted[i];
			}
			if(numrows==0)
			{
				status = OCITransCommit(ociHdbc, ociError, 0);
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				FIRST_TIME=0;
				
				if(ipadd!=NULL)
					free(ipadd);
				if(iname!=NULL)
					free(iname);
				if(submask!=NULL)
					free(submask);
				if(ipadd_nullList!=NULL)
					free(ipadd_nullList);
				if(iname_nullList!=NULL)
					free(iname_nullList);
				if(submask_nullList!=NULL)
					free(submask_nullList);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
							
				for(i=0;i<NbElement;i++)
					delete macIPList[i];
				if(force_insert==1)
				{
                    FIRST_TIME=1;
					force_insert=0;
					macIPList=(macIP**)realloc(macIPList, 0*sizeof(macIP));
				}
				else if(macIPList!=NULL)
                    free(macIPList);

				NbElement=0;

				GetErrorMess(appliName, "COULDNOT_UPDATE_ALL_ROWS",ErrMess,1);
				return -1;
			}
			else
				status = OCITransCommit(ociHdbc, ociError, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCITransCommit unsuccessful");
			}			
	
			FIRST_TIME=0;
			status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);

			if(rescode!=0)
				if(ociError!=0)
					OCIReportError(ociError,appliName, ErrMess,1); 
				else
					GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0); 
			
			for(i=0;i<NbElement;i++)
				delete macIPList[i];

			if(force_insert==1)
			{
				FIRST_TIME=1;
				force_insert=0;
				macIPList=(macIP**)realloc(macIPList, 0*sizeof(macIP));
			}
			else if(macIPList!=NULL)
                free(macIPList);
			NbElement=0;
	
		}
		else
		{
			if(res_query!=0)
			{
				FIRST_TIME=0;
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
				for(i=0;i<NbElement;i++)
					delete macIPList[i];
				if(macIPList!=NULL)
                    free(macIPList);
			}
			if(ociEnv==0)
			{
				FIRST_TIME=0;
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
				for(i=0;i<NbElement;i++)
					delete macIPList[i];
				if(macIPList!=NULL)
                    free(macIPList);
			}
			status+=res_query;
			if(ociEnv!=0 && FIRST_TIME==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}	
		}
		if(ipadd!=NULL)
			free(ipadd);
		if(iname!=NULL)
			free(iname);
		if(submask!=NULL)
			free(submask);
		if(ipadd_nullList!=NULL)
			free(ipadd_nullList);
		if(iname_nullList!=NULL)
			free(iname_nullList);
		if(submask_nullList!=NULL)
			free(submask_nullList);
		if(numrows_inserted!=NULL)
			free(numrows_inserted);

		return (rescode+status);
	}

/**
* Update MAC entries,  returning an integer value.
* The user should manage the memory : there is no memory allocation.
* @param oldip_add :  ip address entry you want to modify. 
* @param ip_add : new value of ip address entry you want to modify. 
* @param first_time1 : 1 if it's your first MacIP insert (save it into the database), 0 otherwise
* @param last_rows1 : 1 if it's your last MacIP insert (save it into the database), 0 otherwise
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
*/ 
EXTERN_CONFDB
	int UpdateMultipleIPAddresses(char* ip_add,char* oldip_add,int first_time,int last_rows,char* ErrMess)
	{
		char appliName[100]="UpdateMultipleIPAddresses";
		char sqlstmt[2000];
		OCIStmt *hstmt;
		OCIBind  *bndp[4]; 
		int rescode=0;
		int res_query=0;
		sword status=0;
		int i=0;

		char* ipadd=NULL;
		char* ipaddold=NULL;
						
		int* ipadd_nullList=NULL;
		int* ipaddold_nullList=NULL;
		
		static int FIRST_TIME=0;
		int force_insert=0;
		int* numrows_inserted=NULL;
		int* numrows_inserted1=NULL;
		int numrows=0;

		static int NbElement;
		UpdtmacIP** macIPList;
		int max_ipaddlen=0;
		int max_ipaddoldlen=0;
						
		if (first_time==1)
		{
			FIRST_TIME=1;
			NbElement=1;
			macIPList=(UpdtmacIP**)malloc( NbElement*sizeof(UpdtmacIP));			
		}
		else 
		{
			if (FIRST_TIME==0&&force_insert==0)
			{
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
				return -1;
			}
			NbElement++;
			macIPList=(UpdtmacIP**)realloc(macIPList, NbElement*sizeof(UpdtmacIP));
		}

		macIPList[NbElement-1]=new UpdtmacIP(ip_add,oldip_add);
		
		if(NbElement==MAXROWS && last_rows!=1)
		{
			force_insert=1;
			last_rows=1;
		}

		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME==1)
		{
			int len_host=LOGIN_LEN;
			char login[LOGIN_LEN];
			char host[LOGIN_LEN];
			char* errmessg1=(char*)malloc(ERROR_BUFFER*sizeof(char));

			if(errmessg1==NULL)
			{
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				return -1;
			}
			char* errmessg2=(char*)malloc(ERROR_BUFFER*sizeof(char));
			if(errmessg2==NULL)
			{
				free(errmessg1);
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				return -1;
			}
			res_query=getTerminalName(host,len_host,errmessg1);
			len_host=LOGIN_LEN;
			res_query=getLoginUser(login,len_host,errmessg2);
			if(strncmp("NO_ERROR",errmessg1,8)==0 && strncmp("NO_ERROR",errmessg2,8)==0 )
			{
				free(errmessg1);
				free(errmessg2);
				rescode=0;
			}
			else
			{
				status=-1;
				rescode=-1;
				if(strncmp("NO_ERROR",errmessg1,8)!=0)
				{
					strcpy(ErrMess,errmessg1);
					rescode=-2;
				}
				if(strncmp("NO_ERROR",errmessg2,8)!=0)
				{
					if(rescode==-2)
						sprintf(ErrMess,"%s , %s",errmessg1,errmessg2);
					else
						strcpy(ErrMess,errmessg2);
				}
				FIRST_TIME=0;
		
				if(ipadd!=NULL)
					free(ipadd);
				if(ipaddold!=NULL)
					free(ipaddold);
				if(ipadd_nullList!=NULL)
					free(ipadd_nullList);
				if(ipaddold_nullList!=NULL)
					free(ipaddold_nullList);
				
				for(i=0;i<NbElement;i++)
					delete macIPList[i];
				if(macIPList!=NULL)
					free(macIPList);
				
				free(errmessg1);
				free(errmessg2);
				return -1;
			}
			for(i=0;i<NbElement;i++){
				if (macIPList[i]->ipaddlen > max_ipaddlen)
					max_ipaddlen=macIPList[i]->ipaddlen;
				if (macIPList[i]->ipaddoldlen > max_ipaddoldlen)
					max_ipaddoldlen=macIPList[i]->ipaddoldlen;
			}

			ipadd=(char*)malloc(NbElement*max_ipaddlen*sizeof(char));
			ipaddold=(char*)malloc(NbElement*max_ipaddoldlen*sizeof(char));
			
			ipadd_nullList=(int*)malloc(NbElement*sizeof(int));
			ipaddold_nullList=(int*)malloc(NbElement*sizeof(int));
			
			if(ipadd!=NULL && ipadd_nullList!=NULL && ipaddold!=NULL && ipaddold_nullList!=NULL)
			for(i=0;i<NbElement;i++)
			{
				memcpy(ipadd+i*max_ipaddlen,macIPList[i]->ipadd,macIPList[i]->ipaddlen);
				ipadd_nullList[i]=macIPList[i]->ipadd_null;
				memcpy(ipaddold+i*max_ipaddoldlen,macIPList[i]->ipaddold,macIPList[i]->ipaddoldlen);
				ipaddold_nullList[i]=macIPList[i]->ipaddold_null;				
			}
			numrows_inserted=(int*)malloc(sizeof(int)*NbElement);
			numrows_inserted1=(int*)malloc(sizeof(int)*NbElement);

			if(ipadd==NULL || ipadd_nullList==NULL || ipaddold==NULL || ipaddold_nullList==NULL || numrows_inserted==NULL || numrows_inserted==NULL)
			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				
				FIRST_TIME=0;
				if(ipadd!=NULL)
					free(ipadd);
				if(ipaddold!=NULL)
					free(ipaddold);
				if(ipadd_nullList!=NULL)
					free(ipadd_nullList);
				if(ipaddold_nullList!=NULL)
					free(ipaddold_nullList);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(numrows_inserted1!=NULL)
					free(numrows_inserted1);
				
				for(i=0;i<NbElement;i++)
					delete macIPList[i];
				if(macIPList!=NULL)
                    free(macIPList);
				
				NbElement=0;				
				return -1;
			}
			else
			{
				for(i=0;i<NbElement;i++)
				{
					numrows_inserted[i]=0;
					numrows_inserted1[i]=0;
				}
			}			

			try{
                HandleAlloc(ociEnv,(dvoid**)&hstmt,OCI_HTYPE_STMT,&status);
				sprintf(sqlstmt,"BEGIN :numrows1:=%s(:old_ip,:ipadd,'%s');  :numrows:=%s; end; ",_updateIPaddress,login,SQLROWCOUNT);
				StmtPrepare(hstmt,ociError, sqlstmt, &status);
				
				BindByName(hstmt,&bndp[0],ociError,":ipadd",ipadd,max_ipaddlen,SQLT_STR,&ipadd_nullList[0],&status);
				BindByName(hstmt,&bndp[1],ociError,":old_ip",ipaddold,max_ipaddoldlen,SQLT_STR,&ipaddold_nullList[0],&status);
				BindByName(hstmt,&bndp[2],ociError,":numrows",&numrows_inserted[0],sizeof(int),SQLT_INT,0,&status);
				BindByName(hstmt,&bndp[3],ociError,":numrows1",&numrows_inserted1[0],sizeof(int),SQLT_INT,0,&status);

				BindArrayOfStruct(bndp[0],ociError,max_ipaddlen,sizeof(int),&status);
				BindArrayOfStruct(bndp[1],ociError,max_ipaddoldlen,sizeof(int),&status);
				BindArrayOfStruct(bndp[2],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[3],ociError,sizeof(int),0,&status);
				
				StmtExecute(ociHdbc, hstmt, ociError,NbElement, &status);
			}catch(Error err){
				sprintf(appliName,"UpdateMultipleIPAddresses");	///////
				rescode=ShowErrors (status, err.ociError, err.log);
				
				if(ociError!=0)
					OCIReportError(ociError,appliName,ErrMess,1); 
				else
					GetErrorMess(appliName,err.msg,ErrMess,1);
			}
			
			numrows=-1;
			for(i=0;i< NbElement;i++)
			{
				if(numrows_inserted1[i]==0)
				{
					numrows=0;
					i= NbElement+5;
				}
				else
					numrows=numrows_inserted1[i];
			}
			if(numrows==0)
			{
				status = OCITransCommit(ociHdbc, ociError, 0);
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				FIRST_TIME=0;
				
				if(ipadd!=NULL)
					free(ipadd);
				if(ipaddold!=NULL)
					free(ipaddold);
				if(ipadd_nullList!=NULL)
					free(ipadd_nullList);
				if(ipaddold_nullList!=NULL)
					free(ipaddold_nullList);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(numrows_inserted1!=NULL)
					free(numrows_inserted1);
							
				for(i=0;i<NbElement;i++)
					delete macIPList[i];
				if(force_insert==1)
				{
                    FIRST_TIME=1;
					force_insert=0;
					macIPList=(UpdtmacIP**)realloc(macIPList, 0*sizeof(UpdtmacIP));
				}
				else if(macIPList!=NULL)
                    free(macIPList);

				NbElement=0;

				GetErrorMess(appliName, "COULDNOT_UPDATE_ALL_ROWS",ErrMess,1);
				return -1;
			}
			else
				status = OCITransCommit(ociHdbc, ociError, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCITransCommit unsuccessful");
			}			

			FIRST_TIME=0;
			status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);

			if(rescode!=0)
				if(ociError!=0)
					OCIReportError(ociError,appliName, ErrMess,1); 
				else
					GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0); 
			
			for(i=0;i<NbElement;i++)
				delete macIPList[i];

			if(force_insert==1)
			{
				FIRST_TIME=1;
				force_insert=0;
				macIPList=(UpdtmacIP**)realloc(macIPList, 0*sizeof(UpdtmacIP));
			}
			else if(macIPList!=NULL)
                free(macIPList);
			NbElement=0;
		}
		else
		{
			if(res_query!=0)
			{
				FIRST_TIME=0;
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
				for(i=0;i<NbElement;i++)
					delete macIPList[i];
				if(macIPList!=NULL)
                    free(macIPList);
			}
			if(ociEnv==0)
			{
				FIRST_TIME=0;
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
				for(i=0;i<NbElement;i++)
					delete macIPList[i];
				if(macIPList!=NULL)
                    free(macIPList);
			}
			status+=res_query;
			if(ociEnv!=0 && FIRST_TIME==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}
		}
		if(ipadd!=NULL)
			free(ipadd);
		if(ipaddold!=NULL)
			free(ipaddold);
		if(ipadd_nullList!=NULL)
			free(ipadd_nullList);
		if(ipaddold_nullList!=NULL)
			free(ipaddold_nullList);
		if(numrows_inserted!=NULL)
			free(numrows_inserted);
		if(numrows_inserted1!=NULL)
			free(numrows_inserted1);

		return (rescode+status);
	}
/**
* Update IP alias entries,  returning an integer value.
* The user should manage the memory : there is no memory allocation.
* @param old_ipalias :  ip address entry you want to modify. 
* @param new_ipalias : new value of ip address entry you want to modify. 
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
*************************************************************************/
EXTERN_CONFDB 
	int UpdateIPalias(char* old_ipalias,char* new_ipalias,char* ErrMess)
	{
		char appliName[100]="UpdateIPalias";
		char sqlstmt[2000];
		OCIStmt *hstmt;
		OCIBind  *bndp[3]; 
		int rescode=0;
		int res_query=0;
		sword status=0;
		int numrows_inserted=0;
		int numrows=0;
	
		int len_host=LOGIN_LEN;
		char login[LOGIN_LEN];
		char host[LOGIN_LEN];
		char* errmessg1=(char*)malloc(ERROR_BUFFER*sizeof(char));
		if(errmessg1==NULL)
		{
			GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
			return -1;
		}
		char* errmessg2=(char*)malloc(ERROR_BUFFER*sizeof(char));
		if(errmessg2==NULL)
		{
			free(errmessg1);
			GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
			return -1;
		}
		res_query=getTerminalName(host,len_host,errmessg1);
		len_host=LOGIN_LEN;
		res_query=getLoginUser(login,len_host,errmessg2);

		if(strncmp("NO_ERROR",errmessg1,8)==0 && strncmp("NO_ERROR",errmessg2,8)==0 )
		{
			free(errmessg1);
			free(errmessg2);
			rescode=0;
		}
		else
		{
			status=-1;
			rescode=-1;
			if(strncmp("NO_ERROR",errmessg1,8)!=0)
			{
				strcpy(ErrMess,errmessg1);
				rescode=-2;
			}
			if(strncmp("NO_ERROR",errmessg2,8)!=0)
			{
				if(rescode==-2)
					sprintf(ErrMess,"%s , %s",errmessg1,errmessg2);
				else
					strcpy(ErrMess,errmessg2);
			}
			rescode=-1;

			free(errmessg1);
			free(errmessg2);
			return rescode;
		}
		
		try{
			HandleAlloc(ociEnv,(dvoid**)&hstmt,OCI_HTYPE_STMT,&status);
			//sprintf(sqlstmt,"BEGIN update %s set ipalias=:new_ipalias, last_update=sysdate where ipalias=:old_ipalias; :numrows:=%s; END;",IPALIAS_TABLE,SQLROWCOUNT);
			sprintf(sqlstmt,"BEGIN update %s set ipaddress=:new_ipalias, last_update=sysdate where ipaddress=:old_ipalias; :numrows:=%s; END;",IPALIAS_TABLE,SQLROWCOUNT);
			StmtPrepare(hstmt,ociError, sqlstmt, &status);
			BindByName(hstmt,&bndp[0],ociError,":old_ipalias",old_ipalias,strlen(old_ipalias)+1,SQLT_STR,0,&status);
			BindByName(hstmt,&bndp[1],ociError,":new_ipalias",new_ipalias,strlen(new_ipalias)+1,SQLT_STR,0,&status);
			BindByName(hstmt,&bndp[2],ociError,":numrows",&numrows_inserted,sizeof(int),SQLT_INT,0,&status);			
			StmtExecute(ociHdbc, hstmt, ociError,1, &status);
			
		}catch(Error err){
			sprintf(appliName,"UpdateIPalias");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,err.msg,ErrMess,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}

		if (numrows_inserted==0)
		{
			status = OCITransCommit(ociHdbc, ociError, 0);
			status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
			GetErrorMess(appliName, "COULDNOT_UPDATE_ALL_ROWS",ErrMess,1);
			return -1;
		}
		else
			status = OCITransCommit(ociHdbc, ociError, 0);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCITransCommit unsuccessful");
		}		

		status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);

		if(rescode!=0)
		{
			if(ociError!=0)
				OCIReportError(ociError,appliName, ErrMess,1); 
			else
				GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1); 
		}
		else
			OCIReportError(ociError,appliName, ErrMess,0); 

		return (status+rescode);
	}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

