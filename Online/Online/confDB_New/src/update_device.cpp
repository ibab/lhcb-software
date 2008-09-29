/********************************************************************************/
//	Author: L.Abadie
// version:v3.5
// changes: handling error such that can be extended to C++: (create a class SQLException) (strlen ck + update stmt)
// file which performs bulk collect update for the device table
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
* Update a device  returning an integer value.
* The user should manage the memory : there is no memory allocation.
* @param devicename : value of the devicename to update
* @param node : new value of node 1 if it's a host node (usually starts or ends a path such as L1FEs) , 0 otherwise.Put -1 if thereis no change
* @param promiscuous_mode : new value of prom.mode 1 if put in this mode (default mode), 0 otherwise. Put -1 not to change the value
* @param location : new value of the location of the device if no changes you put none (lower case)
* @param function_list : new value of the functions of the device. If several, separate them with a coma. Put "none", if it doesn't have any.
* @param first_time1 : 1 if it's your first MultipleDevice update (save it into the database), 0 otherwise
* @param last_rows1 : 1 if it's your MultipleDevice last update (save it into the database), 0 otherwise
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
*/
EXTERN_CONFDB
	int UpdateMultipleAttributesDevices(char* devicename,int node,int promiscuous_mode,char* location,char* function_list,int first_time,int last_rows,char* ErrMess)
	{
		char appliName[100]="UpdateMultipleAttributesDevices";
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[6]; 
		int rescode=0;
		int res_query=0;
		sword status=0;
		int i=0;

		char* dname=NULL;		
		char* dlocation=NULL;
		char* dfunctiondev=NULL;
		int* dnode=NULL;
		int* Promiscuous_mode=NULL;
		
		int* name_nullList=NULL;
		int* location_nullList=NULL;
		int* function_nullList=NULL;
	
		static int FIRST_TIME=0;
		char res_query1[100]="";
		int force_insert=0;
		int* numrows_inserted=NULL;
		int numrows=0;
		
		static int NbElement=0;
		UpdtDevice** deviceList;
		int max_namelen=0;
		int max_locationlen=0;
		int max_functionlen=0;

		if (first_time==1)
		{
			FIRST_TIME=1;
			NbElement=1;
			deviceList=(UpdtDevice**)malloc( NbElement*sizeof(UpdtDevice));			
		}
		else 
		{
			if (FIRST_TIME==0&&force_insert==0)
			{
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
				return -1;
			}
			NbElement++;
			deviceList=(UpdtDevice**)realloc(deviceList, NbElement*sizeof(UpdtDevice));
		}
		deviceList[NbElement-1]=new UpdtDevice(devicename,node,promiscuous_mode,location,function_list);

		if(NbElement==MAXROWS && last_rows!=1)
		{
			force_insert=1;
			last_rows=1;
		}

		int need_to_update=0;
		
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
				if(dname!=NULL)	
					free(dname);
				if(dlocation!=NULL)
					free(dlocation);
				if(dfunctiondev!=NULL)
					free(dfunctiondev);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(name_nullList!=NULL)
					free(name_nullList);
				if(location_nullList!=NULL)
					free(location_nullList);
				if(function_nullList!=NULL)
					free(function_nullList);
				if(dnode!=NULL)
					free(dnode);
				if(Promiscuous_mode!=NULL)
					free(Promiscuous_mode);
				
				for(i=0;i<NbElement;i++)
					delete deviceList[i];
				if(deviceList!=NULL)
					free(deviceList);
				
				free(errmessg1);
				free(errmessg2);
				return -1;
			}
			for(i=0;i<NbElement;i++){
				if (deviceList[i]->namelen > max_namelen)
					max_namelen=deviceList[i]->namelen;
				if (deviceList[i]->locationlen > max_locationlen)
					max_locationlen=deviceList[i]->locationlen;
				if (deviceList[i]->functionlen > max_functionlen)
					max_functionlen=deviceList[i]->functionlen;
			}
			dname=(char*)malloc( NbElement*max_namelen*sizeof(char));
			dlocation=(char*)malloc(NbElement*max_locationlen*sizeof(char));
			dfunctiondev=(char*)malloc(NbElement*max_functionlen*sizeof(char));
			dnode=(int*)malloc(sizeof(int)*NbElement);
			Promiscuous_mode=(int*)malloc(sizeof(int)*NbElement);
			
			name_nullList=(int*)malloc(NbElement*sizeof(int));
			location_nullList=(int*)malloc(NbElement*sizeof(int));
			function_nullList=(int*)malloc(NbElement*sizeof(int));

			if(name_nullList!=NULL && location_nullList!=NULL && function_nullList!=NULL && dname!=NULL && dlocation!=NULL && dfunctiondev!=NULL && dnode!=NULL && Promiscuous_mode!=NULL)
			for(i=0;i<NbElement;i++)
			{
				memcpy(dname+i*max_namelen,deviceList[i]->name,deviceList[i]->namelen);
				name_nullList[i]=deviceList[i]->name_null;
				memcpy(dlocation+i*max_locationlen,deviceList[i]->dlocation,deviceList[i]->locationlen);
				location_nullList[i]=deviceList[i]->location_null;
				memcpy(dfunctiondev+i*max_functionlen,deviceList[i]->function,deviceList[i]->functionlen);
				function_nullList[i]=deviceList[i]->function_null;
				dnode[i]=deviceList[i]->dnode;
				Promiscuous_mode[i]=deviceList[i]->prom_mode;
			}
			numrows_inserted=(int*)malloc(sizeof(int)*NbElement);

			if(dlocation==NULL || dname==NULL || dfunctiondev==NULL ||numrows_inserted==NULL 
				||name_nullList==NULL || location_nullList==NULL || function_nullList==NULL || dnode==NULL || Promiscuous_mode==NULL)
			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				
				FIRST_TIME=0;
				
				if(dname!=NULL)
					free(dname);
				if(dlocation!=NULL)
					free(dlocation);
				if(dfunctiondev!=NULL)
					free(dfunctiondev);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(name_nullList!=NULL)
					free(name_nullList);
				if(location_nullList!=NULL)
					free(location_nullList);
				if(function_nullList!=NULL)
					free(function_nullList);
				if(dnode!=NULL)
					free(dnode);
				if(Promiscuous_mode!=NULL)
					free(Promiscuous_mode);
				
				for(i=0;i<NbElement;i++)
					delete deviceList[i];
				if(deviceList!=NULL)
                    free(deviceList);
				
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
				sprintf(sqlstmt,"BEGIN update %s set node=decode(:node,1,1,0,0,node),promiscuous_mode=decode(:pmode,1,1,0,0,promiscuous_mode),location=decode(nvl(:location,'none'),'none',location,:location),functionID=decode(:fctlist,'none',0,%s(:fctlist)),last_update=sysdate,user_update='%s' where devicename=:dname; :numrows:=%s; END;",LOGICAL_DEVICE_TABLE,_ComposeFctID,login,SQLROWCOUNT);
				StmtPrepare(hstmt,ociError, sqlstmt, &status);

				BindByName(hstmt,&bndp[0],ociError,":dname",dname, max_namelen,SQLT_STR,&name_nullList[0],&status);
				BindByName(hstmt,&bndp[1],ociError,":node",&dnode[0],sizeof(&deviceList[0]->dnode),SQLT_INT,0,&status);				
				BindByName(hstmt,&bndp[2],ociError,":pmode",&Promiscuous_mode[0],sizeof(&deviceList[0]->dnode),SQLT_INT,0,&status);
				BindByName(hstmt,&bndp[3],ociError,":location",dlocation,max_locationlen,SQLT_STR,&location_nullList[0],&status);
				BindByName(hstmt,&bndp[4],ociError,":fctlist",dfunctiondev,max_functionlen,SQLT_STR,&function_nullList[0],&status);
				BindByName(hstmt,&bndp[5],ociError,":numrows",&numrows_inserted[0],sizeof(int),SQLT_INT,0,&status);
				
				BindArrayOfStruct(bndp[0],ociError,max_namelen,sizeof(int),&status);
				BindArrayOfStruct(bndp[1],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[2],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[3],ociError,max_locationlen,sizeof(int),&status);
				BindArrayOfStruct(bndp[4],ociError,max_functionlen,sizeof(int),&status);
				BindArrayOfStruct(bndp[5],ociError,sizeof(int),0,&status);

				StmtExecute(ociHdbc, hstmt, ociError,NbElement, &status);
				
			}catch(Error err){
				sprintf(appliName,"UpdateMultipleAttributesDevices");	///////
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
				
                if(dname!=NULL)
					free(dname);
				if(dlocation!=NULL)
					free(dlocation);
				if(dfunctiondev!=NULL)
					free(dfunctiondev);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(name_nullList!=NULL)
					free(name_nullList);
				if(location_nullList!=NULL)
					free(location_nullList);
				if(function_nullList!=NULL)
					free(function_nullList);
				if(dnode!=NULL)
					free(dnode);
				if(Promiscuous_mode!=NULL)
					free(Promiscuous_mode);
				
				for(i=0;i<NbElement;i++)
					delete deviceList[i];
				if(force_insert==1)
				{
                    FIRST_TIME=1;
					force_insert=0;
					deviceList=(UpdtDevice**)realloc(deviceList, 0*sizeof(UpdtDevice));
				}
				else if(deviceList!=NULL)
                    free(deviceList);

				NbElement=0;

				GetErrorMess(appliName, "COULDNOT_UPDATE_ALL_ROWS",ErrMess,1);
				return -1;
			}
			else
			{
				status = OCITransCommit(ociHdbc, ociError, 0);
				while(i<NbElement)
				{
					if(dnode[i]==0 || dnode[i]==1)
					{
						need_to_update=1;
						i=NbElement+5;
					}
					else
						i++;
				}
				if(need_to_update==1)
				{
					sprintf(sqlstmt,"BEGIN update %s d set d.link_weight=(select decode(f.node,1,t.node+f.node+1,t.node+f.node) from %s t,%s f, %s l,%s g where d.portidfrom=l.portid and l.deviceid=t.deviceid and d.portidto=g.portid and g.deviceid=f.deviceid),d.last_update=sysdate,d.user_update='%s'; :rescode:=%s(1); :numrows:=%s; END;",MACRO_CONNECTIVITY_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,PORT_TABLE,PORT_TABLE,login,_UpdateDynamicTables,SQLROWCOUNT);
					try{
						StmtPrepare(hstmt,ociError, sqlstmt, &status);
						BindByName(hstmt,&bndp[0],ociError,":numrows",&numrows,sizeof(int),SQLT_INT,0,&status);
						BindByName(hstmt,&bndp[1],ociError,":rescode",res_query1,101,SQLT_STR,0,&status);
						StmtExecute(ociHdbc, hstmt, ociError,1, &status);
						TransCommit(ociHdbc, ociError, 0, &status );
                	}catch(Error err){
						rescode=ShowErrors (status, err.ociError, err.log);				
					}
					if(status!=OCI_SUCCESS)
					{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCITransCommit unsuccessful");
					}
				}
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
				delete deviceList[i];
			if(force_insert==1)
			{
				FIRST_TIME=1;
				force_insert=0;
				deviceList=(UpdtDevice**)realloc(deviceList, 0*sizeof(UpdtDevice));
			}
			else if(deviceList!=NULL)
                free(deviceList);
			NbElement=0;
		}
		else
		{
			if(res_query!=0)
			{
				FIRST_TIME=0;
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
				for(i=0;i<NbElement;i++)
					delete deviceList[i];
				if(deviceList!=NULL)
                    free(deviceList);
			}
			if(ociEnv==0)
			{
				FIRST_TIME=0;
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
				for(i=0;i<NbElement;i++)
					delete deviceList[i];
				if(deviceList!=NULL)
                    free(deviceList);
			}
			status+=res_query;
			if(ociEnv!=0 && FIRST_TIME==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}
		}
		if(dname!=NULL)
			free(dname);
		if(dlocation!=NULL)
			free(dlocation);
		if(dfunctiondev!=NULL)
			free(dfunctiondev);
		if(numrows_inserted!=NULL)
			free(numrows_inserted);
		if(name_nullList!=NULL)
			free(name_nullList);
		if(location_nullList!=NULL)
			free(location_nullList);
		if(function_nullList!=NULL)
			free(function_nullList);
		if(dnode!=NULL)
			free(dnode);
		if(Promiscuous_mode!=NULL)
			free(Promiscuous_mode);
		
		return (status+rescode);
	}

/******************************************************/
//nodeused either 1 or 0 
//0 means it won't be take into acount in routing tables and so on
/**
* Update the nodeused parameter of a device  returning an integer value.
* The user should manage the memory : there is no memory allocation.
* @param devicename : name of the device
* @param nodeused : 1 if it's used , 0 otherwise.If not equal to 1 or 0 we don't change the field
* @param first_time1 : 1 if it's your first MultipleDeviceNodeUsed update (save it into the database), 0 otherwise
* @param last_rows1 : 1 if it's your MultipleDeviceNodeUsed last update (save it into the database), 0 otherwise
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
*/
/*********************************************************************/
EXTERN_CONFDB 
	int UpdateMultipleDeviceNodeUsed(char* devicename,int nodeused,int first_time,int last_rows,char* ErrMess)
	{
		char appliName[100]="UpdateMultipleDeviceNodeUsed";
		char sqlstmt[2000];
		OCIStmt *hstmt;
		OCIBind  *bndp[5]; 
		int rescode=0;
		int res_query=0;
		sword status=0;
		int i=0;

		char* dname=NULL;		
		int* nused=NULL;		
		int* name_nullList=NULL;
	
		static int FIRST_TIME=0;

		int force_insert=0;
		int* numrows_inserted=NULL;
		int* numrows_inserted1=NULL;/////////////
		int numrows=0;
	
		static int NbElement=0;
		UpdtDevice** deviceList;
		int max_namelen=0;
		
		if (first_time==1)
		{
			FIRST_TIME=1;
			NbElement=1;
			deviceList=(UpdtDevice**)malloc( NbElement*sizeof(UpdtDevice));
		}
		else 
		{
			if (FIRST_TIME==0&&force_insert==0)
			{
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
				return -1;
			}
			NbElement++;
			deviceList=(UpdtDevice**)realloc(deviceList, NbElement*sizeof(UpdtDevice));
		}

		deviceList[NbElement-1]=new UpdtDevice(devicename,nodeused);

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

			int sysID=GetSubsystemID("DAQ,TFC"); //only devices part of these subsystems affect routing and or destina tables

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
				if(dname!=NULL)	
					free(dname);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(numrows_inserted1!=NULL)
					free(numrows_inserted1);
				if(name_nullList!=NULL)
					free(name_nullList);
				if(nused!=NULL)
					free(nused);
				
				for(i=0;i<NbElement;i++)
					delete deviceList[i];
				if(deviceList!=NULL)
					free(deviceList);
				
				free(errmessg1);
				free(errmessg2);
				return -1;
			}
			for(i=0;i<NbElement;i++){
				if (deviceList[i]->namelen > max_namelen)
					max_namelen=deviceList[i]->namelen;
			}
			dname=(char*)malloc( NbElement*max_namelen*sizeof(char));
			nused=(int*)malloc(sizeof(int)*NbElement);
			name_nullList=(int*)malloc(NbElement*sizeof(int));

			if(name_nullList!=NULL && dname!=NULL && nused!=NULL)
			for(i=0;i<NbElement;i++)
			{
				memcpy(dname+i*max_namelen,deviceList[i]->name,deviceList[i]->namelen);
				name_nullList[i]=deviceList[i]->name_null;
				nused[i]=deviceList[i]->nused;
			}
			numrows_inserted=(int*)malloc(sizeof(int)*NbElement);
			numrows_inserted1=(int*)malloc(sizeof(int)*NbElement);

			if( dname==NULL || numrows_inserted==NULL || numrows_inserted1==NULL ||name_nullList==NULL || nused==NULL)

			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				
				FIRST_TIME=0;
				
				if(dname!=NULL)
					free(dname);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(numrows_inserted1!=NULL)
					free(numrows_inserted1);
				if(name_nullList!=NULL)
					free(name_nullList);
				if(nused!=NULL)
					free(nused);
				
				for(i=0;i<NbElement;i++)
					delete deviceList[i];
				if(deviceList!=NULL)
                    free(deviceList);
				
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
				numrows_inserted1[NbElement-1]=1;
			}	

			try{
                HandleAlloc(ociEnv,(dvoid**)&hstmt,OCI_HTYPE_STMT,&status);
				sprintf(sqlstmt,"BEGIN update %s set nodeused=decode(:nused,1,1,0,0,nodeused),last_update=sysdate,user_update='%s' where devicename=:dname ; if :numrows1=1 then update %s e set e.last_update=sysdate,e.user_update='%s',e.lkused=(select t.nodeused*f.nodeused from %s t,%s f, %s l,%s m where t.deviceid=l.deviceid and l.portid=e.portidfrom and f.deviceid=m.deviceid and m.portid=e.portidto); :rescode:=%s(%d); end if; :numrows:=%s; END;",LOGICAL_DEVICE_TABLE,login,MACRO_CONNECTIVITY_TABLE,login,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,PORT_TABLE,PORT_TABLE,_UpdatePathUsed,sysID,SQLROWCOUNT);
				StmtPrepare(hstmt,ociError, sqlstmt, &status);

				BindByName(hstmt,&bndp[0],ociError,":dname",dname, max_namelen,SQLT_STR,&name_nullList[0],&status);
				BindByName(hstmt,&bndp[1],ociError,":nused",&nused[0],sizeof(&deviceList[0]->nused),SQLT_INT,0,&status);				
				BindByName(hstmt,&bndp[2],ociError,":numrows",&numrows_inserted[0],sizeof(int),SQLT_INT,0,&status);
				BindByName(hstmt,&bndp[3],ociError,":numrows1",&numrows_inserted1[0],sizeof(int),SQLT_INT,0,&status);
				BindByName(hstmt,&bndp[4],ociError,":rescode",&res_query,sizeof(int),SQLT_INT,0,&status);
				
				BindArrayOfStruct(bndp[0],ociError,max_namelen,sizeof(int),&status);
				BindArrayOfStruct(bndp[1],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[2],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[3],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[4],ociError,sizeof(int),0,&status);

				StmtExecute(ociHdbc, hstmt, ociError,NbElement, &status);
			}catch(Error err){
				sprintf(appliName,"UpdateMultipleDeviceNodeUsed");	///////
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
			if(numrows=0)
			{
				status = OCITransCommit(ociHdbc, ociError, 0);
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				FIRST_TIME=0;
				
                if(dname!=NULL)
					free(dname);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(numrows_inserted1!=NULL)
					free(numrows_inserted1);
				if(name_nullList!=NULL)
					free(name_nullList);
				if(nused!=NULL)
					free(nused);
				
				for(i=0;i<NbElement;i++)
					delete deviceList[i];
				if(force_insert==1)
				{
                    FIRST_TIME=1;
					force_insert=0;
					deviceList=(UpdtDevice**)realloc(deviceList, 0*sizeof(UpdtDevice));
				}
				else if(deviceList!=NULL)
                    free(deviceList);

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
				delete deviceList[i];
			if(force_insert==1)
			{
				FIRST_TIME=1;
				force_insert=0;
				deviceList=(UpdtDevice**)realloc(deviceList, 0*sizeof(UpdtDevice));
			}
			else if(deviceList!=NULL)
                free(deviceList);
			NbElement=0;
		}
		else
		{
			if(res_query!=0)
			{
				FIRST_TIME=0;
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
				for(i=0;i<NbElement;i++)
					delete deviceList[i];
				if(deviceList!=NULL)
                    free(deviceList);
			}
			if(ociEnv==0)
			{
				FIRST_TIME=0;
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
				for(i=0;i<NbElement;i++)
					delete deviceList[i];
				if(deviceList!=NULL)
                    free(deviceList);
			}
			status+=res_query;
			if(ociEnv!=0 && FIRST_TIME==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}
		}
		if(dname!=NULL)
			free(dname);
		if(numrows_inserted!=NULL)
			free(numrows_inserted);
		if(numrows_inserted1!=NULL)
			free(numrows_inserted1);
		if(name_nullList!=NULL)
			free(name_nullList);
		if(nused!=NULL)
			free(nused);
				
		return (status+rescode);
	}

/****************************************************************************************/
//nodeused either 1 or 0
//0 means it won't be take into acount in routing tables and so on
/**
* Update the nodeused field for a devicetype or a set of devicetype whose name begins with  returning an integer value.
* The user should manage the memory : there is no memory allocation.
* @param devicetype : name of the devicetype or (the prefix : all devices which begin with devicetype)
* @param nodeused : 1 if it's used , 0 otherwise.If not equal to 1 or 0 we don't change the field.
* @param devtype_given : 1 if you give a devicetype, 0 otherwise
* @param last_rows : 1 if you want to commit, 0 otherwise (in that case we just insert in the DB)
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
*/
/************************************************************************/
EXTERN_CONFDB 
	int UpdateMultipleDeviceNodeUsedByType(char* devicetype,int nodeused,int devtype_given,int last_rows,char* ErrMess)
	{
		char appliName[100]="UpdateMultipleDeviceNodeUsedByType";
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[3]; 
		OCIBind  *bndp1 = (OCIBind *) 0;
		int rescode=0;
		int res_query=0;
		sword status=0;
		
		int sysID=GetSubsystemID("DAQ,TFC");

		int type_null=-1;
		if(devicetype!=NULL)
			type_null=0;

		int numrows_inserted=0;
		
		if(ociEnv!=0)
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

				free(errmessg1);
				free(errmessg2);
				return -1;
			}
		
			try{
				HandleAlloc(ociEnv,(dvoid**)&hstmt,OCI_HTYPE_STMT,&status);
				if(devtype_given==1)
				{
					sprintf(sqlstmt,"BEGIN update %s set nodeused=decode(:nused,1,1,0,0,nodeused),last_update=sysdate,user_update='%s' where devicetypeid=(select t.devicetypeid from %s t where t.devicetype=:dname) returning system_name into :sysID; update %s e set e.last_update=sysdate,e.user_update='%s',e.lkused=(select t.nodeused*f.nodeused from %s t,%s f, %s l,%s m where t.deviceid=l.deviceid and l.portid=e.portidfrom and f.deviceid=m.deviceid and m.portid=e.portidto); :rescode:=%s(:sysID);  :numrows:=%s; END;",LOGICAL_DEVICE_TABLE,login,DEVICETYPE_TABLE,MACRO_CONNECTIVITY_TABLE,login,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,PORT_TABLE,PORT_TABLE,_UpdatePathUsed,SQLROWCOUNT);
					StmtPrepare(hstmt,ociError, sqlstmt, &status);
					BindByName(hstmt,&bndp1,ociError,":dname",devicetype,strlen(devicetype)+1,SQLT_STR,&type_null,&status);
				}
				else
				{
					sprintf(sqlstmt,"BEGIN update %s set nodeused=decode(:nused,1,1,0,0,nodeused),last_update=sysdate,user_update='%s' where devicename like '%s%%'; update %s e set e.last_update=sysdate,e.user_update='%s',e.lkused=(select t.nodeused*f.nodeused from %s t,%s f, %s l,%s m where t.deviceid=l.deviceid and l.portid=e.portidfrom and f.deviceid=m.deviceid and m.portid=e.portidto); :rescode:=%s(%d);  :numrows:=%s; END;",LOGICAL_DEVICE_TABLE,login,devicetype,MACRO_CONNECTIVITY_TABLE,login,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,PORT_TABLE,PORT_TABLE,_UpdatePathUsed,sysID,SQLROWCOUNT);
					StmtPrepare(hstmt,ociError, sqlstmt, &status);
				}
				BindByName(hstmt,&bndp[0],ociError,":nused",&nodeused,sizeof(nodeused),SQLT_INT,0,&status);
				BindByName(hstmt,&bndp[1],ociError,":numrows",&numrows_inserted,sizeof(int),SQLT_INT,0,&status);
				BindByName(hstmt,&bndp[2],ociError,":rescode",&res_query,sizeof(int),SQLT_INT,0,&status);
				StmtExecute(ociHdbc, hstmt, ociError,1, &status);		
	
			}catch(Error err){
				sprintf(appliName,"UpdateMultipleDeviceNodeUsedByType");	///////
				rescode=ShowErrors (status, err.ociError, err.log);
			
				if(ociError!=0)
					OCIReportError(ociError,appliName,ErrMess,1); 
				else
					GetErrorMess(appliName,err.msg,ErrMess,1);
			
				if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
				return -1;
			}

			if(numrows_inserted==0)
			{
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				GetErrorMess(appliName, "COULDNOT_UPDATE_ALL_ROWS",ErrMess,1);
				return -1;
			}
			else
			{
				if(last_rows==1)
					status = OCITransCommit(ociHdbc, ociError, 0);
			}

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
		}
		else
		{

			if(ociEnv==0)
			{
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
			}
			status+=res_query;
		}
		
		return (status+rescode);
	}
/***************************************************************************************/
//active either 1 or 0 it means it's used by sb else:
/**
* Update the active field for a devicetype or a set of devicetype whose name begins with  returning an integer value.
* The user should manage the memory : there is no memory allocation.
* @param devicetype : name of the devicetype or (the prefix : all devices which begin with devicetype)
* @param active : 1 if it's busy , 0 otherwise (means free).If not equal to 1 or 0 we don't change the field.
* @param devtype_given : 1 if you give a devicetype, 0 otherwise
* @param last_rows : 1 if you want to commit, 0 otherwise (in that case we just insert in the DB)
* @param ErrMess : error message in case of failure
* @return 0 if it is successful   */
EXTERN_CONFDB 
	int UpdateMultipleDeviceActiveByType(char* devicetype,int active,int devtype_given,int last_rows,char* ErrMess)
	{
		char appliName[100]="updateMultipleDeviceActiveByType";
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[2]; 
		OCIBind  *bndp3 = (OCIBind *) 0;
		int rescode=0;
		int res_query=0;
		sword status=0;	
		int numrows_inserted=0;
		
		int dname1=-1;
		if(devicetype!=NULL)
			dname1=0;

		if(ociEnv!=0)
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
				
				free(errmessg1);
				free(errmessg2);
				return -1;
			}
			
			try{
				HandleAlloc(ociEnv,(dvoid**)&hstmt,OCI_HTYPE_STMT,&status);
				if(devtype_given==1)
				{
					sprintf(sqlstmt,"BEGIN update %s set active=decode(:nused,1,1,0,0,active),last_update=sysdate,user_update='%s' where devicetypeid=(select devicetypeid from %s where devicetype=:dname); :numrows:=%s; END;",LOGICAL_DEVICE_TABLE,login,DEVICETYPE_TABLE,SQLROWCOUNT);
					StmtPrepare(hstmt,ociError, sqlstmt, &status);
					BindByName(hstmt,&bndp3,ociError,":dname",devicetype,strlen(devicetype)+1,SQLT_STR,&dname1,&status);
				}
				else
				{
					sprintf(sqlstmt,"BEGIN update %s set active=decode(:nused,1,1,0,0,active),last_update=sysdate,user_update='%s' where devicename like '%s%%'; :numrows:=%s; END;",LOGICAL_DEVICE_TABLE,login,devicetype,SQLROWCOUNT);
					StmtPrepare(hstmt,ociError, sqlstmt, &status);
				}
				BindByName(hstmt,&bndp[0],ociError,":nused",&active,sizeof(active),SQLT_INT,0,&status);
				BindByName(hstmt,&bndp[1],ociError,":numrows",&numrows_inserted,sizeof(int),SQLT_INT,0,&status);
				StmtExecute(ociHdbc, hstmt, ociError,1, &status);
	
			}catch(Error err){
				sprintf(appliName,"UpdateMultipleDeviceActiveByType");	///////
				rescode=ShowErrors (status, err.ociError, err.log);
			
				if(ociError!=0)
					OCIReportError(ociError,appliName,ErrMess,1); 
				else
					GetErrorMess(appliName,err.msg,ErrMess,1);
			
				if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
				return -1;
			}

			if(numrows_inserted==0)
			{
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				GetErrorMess(appliName, "COULDNOT_UPDATE_ALL_ROWS",ErrMess,1);
				return -1;
			}
			else
			{
				if(last_rows==1)
					status = OCITransCommit(ociHdbc, ociError, 0);
			}

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
		}
		else
		{

			if(ociEnv==0)
			{
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
			}
			status+=res_query;
		}
		
		return (status+rescode);
	}

/*************************************************************************/
//active either 1 or 0 
//0 means it's free
/**
* Update the active field of a  device  returning an integer value.
* The user should manage the memory : there is no memory allocation.
* @param devicename : name of the device
* @param active :  1 if this node is busy (booked) , 0 otherwise. if not equals to 0 or 1, we don't change the value
* @param first_time1 : 1 if it's your first MultipleDeviceActive update (save it into the database), 0 otherwise
* @param last_rows1 : 1 if it's your MultipleDeviceActive last update (save it into the database), 0 otherwise
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
*/
EXTERN_CONFDB 
	int UpdateMultipleDeviceActive(char* devicename,int active,int first_time,int last_rows,char* ErrMess)
	{
		char appliName[100]="UpdateMultipleDeviceActive";
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[3]; 
		int rescode=0;
		int res_query=0;
		sword status=0;
		int i=0;

		char* dname=NULL;		
		int* act=NULL;	
		int* name_nullList=NULL;
	
		static int FIRST_TIME=0;

		int force_insert=0;
		int* numrows_inserted=NULL;
		int numrows=0;
		
		static int NbElement=0;
		UpdtDevice** deviceList;
		int max_namelen=0;

		if (first_time==1)
		{
			FIRST_TIME=1;
			NbElement=1;
			deviceList=(UpdtDevice**)malloc( NbElement*sizeof(UpdtDevice));			
		}
		else 
		{
			if (FIRST_TIME==0&&force_insert==0)
			{
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
				return -1;
			}
			NbElement++;
			deviceList=(UpdtDevice**)realloc(deviceList, NbElement*sizeof(UpdtDevice));
		}

		deviceList[NbElement-1]=new UpdtDevice(devicename,NULL,active);

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
				if(dname!=NULL)	
					free(dname);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(name_nullList!=NULL)
					free(name_nullList);
				if(act!=NULL)
					free(act);
				
				for(i=0;i<NbElement;i++)
					delete deviceList[i];
				if(deviceList!=NULL)
					free(deviceList);
				
				free(errmessg1);
				free(errmessg2);
				return -1;
			}

			for(i=0;i<NbElement;i++){
				if (deviceList[i]->namelen > max_namelen)
					max_namelen=deviceList[i]->namelen;
			}
			dname=(char*)malloc( NbElement*max_namelen*sizeof(char));
			act=(int*)malloc(sizeof(int)*NbElement);
			name_nullList=(int*)malloc(NbElement*sizeof(int));
			
			if(name_nullList!=NULL && dname!=NULL && act!=NULL)
			for(i=0;i<NbElement;i++)
			{
				memcpy(dname+i*max_namelen,deviceList[i]->name,deviceList[i]->namelen);
				name_nullList[i]=deviceList[i]->name_null;
				act[i]=deviceList[i]->act;
			}
	
			numrows_inserted=(int*)malloc(sizeof(int)*NbElement);

			if(dname==NULL ||numrows_inserted==NULL ||name_nullList==NULL || act==NULL)
			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				
				FIRST_TIME=0;
				
				if(dname!=NULL)
					free(dname);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(name_nullList!=NULL)
					free(name_nullList);
				if(act!=NULL)
					free(act);
				
				for(i=0;i<NbElement;i++)
					delete deviceList[i];
				if(deviceList!=NULL)
                    free(deviceList);
				
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
				sprintf(sqlstmt,"BEGIN update %s set active=decode(:nused,1,1,0,0,active),last_update=sysdate,user_update='%s' where devicename=:dname; :numrows:=%s; END;",LOGICAL_DEVICE_TABLE,login,SQLROWCOUNT);
				StmtPrepare(hstmt,ociError, sqlstmt, &status);

				BindByName(hstmt,&bndp[0],ociError,":dname",dname, max_namelen,SQLT_STR,&name_nullList[0],&status);
				BindByName(hstmt,&bndp[1],ociError,":nused",&act[0],sizeof(int),SQLT_INT,0,&status);
				BindByName(hstmt,&bndp[2],ociError,":numrows",&numrows_inserted[0],sizeof(int),SQLT_INT,0,&status);

				BindArrayOfStruct(bndp[0],ociError,max_namelen,sizeof(int),&status);
				BindArrayOfStruct(bndp[1],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[2],ociError,sizeof(int),0,&status);
			
				StmtExecute(ociHdbc, hstmt, ociError,NbElement, &status);
				
			}catch(Error err){
				sprintf(appliName,"InsertMultipleFunctionalDevices");	///////
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
				
                if(dname!=NULL)
					free(dname);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(name_nullList!=NULL)
					free(name_nullList);
				if(act!=NULL)
					free(act);
				
				for(i=0;i<NbElement;i++)
					delete deviceList[i];
				if(force_insert==1)
				{
                    FIRST_TIME=1;
					force_insert=0;
					deviceList=(UpdtDevice**)realloc(deviceList, 0*sizeof(UpdtDevice));
				}
				else if(deviceList!=NULL)
                    free(deviceList);

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
				delete deviceList[i];
			if(force_insert==1)
			{
				FIRST_TIME=1;
				force_insert=0;
				deviceList=(UpdtDevice**)realloc(deviceList, 0*sizeof(UpdtDevice));
			}
			else if(deviceList!=NULL)
                free(deviceList);
			NbElement=0;
		}
		else
		{
			if(res_query!=0)
			{
				FIRST_TIME=0;
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
				for(i=0;i<NbElement;i++)
					delete deviceList[i];
				if(deviceList!=NULL)
                    free(deviceList);
			}
			
			if(ociEnv==0)
			{
				FIRST_TIME=0;
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
				for(i=0;i<NbElement;i++)
					delete deviceList[i];
				if(deviceList!=NULL)
                    free(deviceList);
			}
			status+=res_query;
			if(ociEnv!=0 && FIRST_TIME==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}
		}
		if(dname!=NULL)
			free(dname);
		if(numrows_inserted!=NULL)
			free(numrows_inserted);
		if(name_nullList!=NULL)
			free(name_nullList);
		if(act!=NULL)
			free(act);
						
		return (status+rescode);
	}

/****************************************************/
//there is one board with a serial nb XXX and has log_dev_1 as a functional name (its function)
//there is one board with a serial nb YYY and has log_dev_2 as a functional name (its function)
//Swapping two functional devices (log_dev_1 and log_dev_2) means:
//the board with the serial nb XXX has log_dev_2 as a functional name
//the board with the serial nb YYY has log_dev_1 as a functional name
//Connectivity is bound to the functional_device, so after the swapping the board XXX will be connected as the board YYY was connected and vice-versa
//For the DAQ, we also swap the mac addresses, but not the IP @ because they are bound to the functional device.
//we also report the swapping into the history table
/**
* Swap two devices  returning an integer value.
* The user should manage the memory : there is no memory allocation.
* @param functional_devicename1 : functional name of the device 1 
* @param functional_devicename2 :  functional name of the device 2
* @param comments : put "none" if you want to put the default comment which is "Swapping functional_devicename1 and functional_devicename1", otherwise, it will append your comment to the default.
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
*/
/*****************************************************/
EXTERN_CONFDB 
	int SwapTwoDevices(char* functional_devicename1,char* functional_devicename2,char* comments,char* ErrMess)
	{
		char appliName[100]="SwapTwoDevices";
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[4]; 
		int rescode=0;
		char res_query[200]="";
		int len3=200;
		sword status=0;
		int numrows=0;
		
		int len1=0;
		int len2=0;
		
		if(ociEnv!=0)
		{
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
			int len_host=LOGIN_LEN;
			char login[LOGIN_LEN];
			char host[LOGIN_LEN];
			rescode=getTerminalName(host,len_host,errmessg1);
			len_host=LOGIN_LEN;
			rescode=getLoginUser(login,len_host,errmessg2);
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
			if (functional_devicename1!=NULL)
				len1=strlen(functional_devicename1);
			else
			{
				GetErrorMess(appliName, "functional_devicename1 can't be null",ErrMess,1);
				return -1;
			}
			if (functional_devicename2!=NULL)
				len2=strlen(functional_devicename2);
			else
			{
				GetErrorMess(appliName, "functional_devicename2 can't be null",ErrMess,1);
				return -1;
			}

			try{
				HandleAlloc(ociEnv,(dvoid**)&hstmt,OCI_HTYPE_STMT,&status);
				sprintf(sqlstmt,"BEGIN :rescode:=%s(:logName1 ,logName2, '%s', :usercomments); :numrows:=%s; END;", _SwapTwoDevices,login,SQLROWCOUNT);
				StmtPrepare(hstmt,ociError, sqlstmt, &status);
				BindByName(hstmt,&bndp[0],ociError,":logName1",functional_devicename1,len1+1,SQLT_STR,0,&status);
				BindByName(hstmt,&bndp[1],ociError,":logName2",functional_devicename2,len2+1,SQLT_STR,0,&status);
				BindByName(hstmt,&bndp[2],ociError,":rescode",res_query,len3+1,SQLT_STR,0,&status);
				BindByName(hstmt,&bndp[3],ociError,":numrows",&numrows,sizeof(int),SQLT_INT,0,&status);
				
				StmtExecute(ociHdbc, hstmt, ociError,1, &status);		
	
			}catch(Error err){
				sprintf(appliName,"Swap_TwoDevices");	///////
				rescode=ShowErrors (status, err.ociError, err.log);
			
				if(ociError!=0)
					OCIReportError(ociError,appliName,ErrMess,1); 
				else
					GetErrorMess(appliName,err.msg,ErrMess,1);
			
				if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
				return -1;
			}

			if(strncmp(res_query,"SUCCESSFUL_SWAPPING",19)!=0)
			{
				status = OCITransRollback(ociHdbc, ociError, 0);
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				GetErrorMess(appliName, res_query,ErrMess,1);
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
			if(status!=0)
				if(ociError!=0)
					OCIReportError(ociError,appliName, ErrMess,1); 
				else
					GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0);
		}
		else
		{
			GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1);
			return -1;
		}
        return (status+rescode);
	}


/**
* Update the devicename field of a device (typically when there is a mistyping) returning an integer value.
* The user should manage the memory : there is no memory allocation.
* @param devicename_old : name of the device you want to update
* @param devicename : new value of the devicename. can't be NULL!
* @param first_time1 : 1 if it's your first MultipleDevice update (save it into the database), 0 otherwise
* @param last_rows1 : 1 if it's your MultipleDevice last update (save it into the database), 0 otherwise
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
*/
/******************************************************************/
EXTERN_CONFDB
	int UpdateMultipleDevNamesDevices(char* devicename_old,char* devicename,int first_time,int last_rows,char* ErrMess)
	{
		char appliName[100]="UpdateMultipleDevNamesDevices";
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[3]; 
		int rescode=0;
		int res_query=0;
		sword status=0;
		int i=0;

		char* dname=NULL;		
		char* dname_old=NULL;
		int* name_nullList=NULL;
		int* nameold_nullList=NULL;
		
		static int FIRST_TIME=0;

		int force_insert=0;
		int* numrows_inserted=NULL;
		int numrows=0;

		static int NbElement=0;
		UpdtDevice** deviceList;
		int max_namelen=0;
		int max_nameoldlen=0;

		if (first_time==1)
		{
			FIRST_TIME=1;
			NbElement=1;
			deviceList=(UpdtDevice**)malloc( NbElement*sizeof(UpdtDevice));			
		}
		else 
		{
			if (FIRST_TIME==0&&force_insert==0)
			{
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
				return -1;
			}
			NbElement++;
			deviceList=(UpdtDevice**)realloc(deviceList, NbElement*sizeof(UpdtDevice));
		}
		deviceList[NbElement-1]=new UpdtDevice(devicename_old,devicename);
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
				if(dname!=NULL)	
					free(dname);
				if(dname_old!=NULL)
					free(dname_old);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(name_nullList!=NULL)
					free(name_nullList);
				if(nameold_nullList!=NULL)
					free(nameold_nullList);
				
				for(i=0;i<NbElement;i++)
					delete deviceList[i];
				if(deviceList!=NULL)
					free(deviceList);
				
				free(errmessg1);
				free(errmessg2);
				return -1;
			}
			for(i=0;i<NbElement;i++){
				if (deviceList[i]->namelen > max_namelen)
					max_namelen=deviceList[i]->namelen;
				if (deviceList[i]->nameoldlen > max_nameoldlen)
					max_nameoldlen=deviceList[i]->nameoldlen;
			}
			dname=(char*)malloc( NbElement*max_namelen*sizeof(char));
			dname_old=(char*)malloc(NbElement*max_nameoldlen*sizeof(char));
			name_nullList=(int*)malloc(NbElement*sizeof(int));
			nameold_nullList=(int*)malloc(NbElement*sizeof(int));
			
			if(name_nullList!=NULL && nameold_nullList!=NULL && dname!=NULL && dname_old!=NULL)
			for(i=0;i<NbElement;i++)
			{
				memcpy(dname+i*max_namelen,deviceList[i]->name,deviceList[i]->namelen);
				name_nullList[i]=deviceList[i]->name_null;
				memcpy(dname_old+i*max_nameoldlen,deviceList[i]->nameold,deviceList[i]->nameoldlen);
				nameold_nullList[i]=deviceList[i]->nameold_null;
			}

			numrows_inserted=(int*)malloc(sizeof(int)*NbElement);

			if(dname==NULL || dname_old==NULL || numrows_inserted==NULL ||name_nullList==NULL || nameold_nullList==NULL)
			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				
				FIRST_TIME=0;
				
				if(dname!=NULL)
					free(dname);
				if(dname_old!=NULL)
					free(dname_old);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(name_nullList!=NULL)
					free(name_nullList);
				if(nameold_nullList!=NULL)
					free(nameold_nullList);
				
				for(i=0;i<NbElement;i++)
					delete deviceList[i];
				if(deviceList!=NULL)
                    free(deviceList);
				
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
				sprintf(sqlstmt,"BEGIN update %s set devicename=decode(nvl(:dname,'none'),'none',devicename,:dname),last_update=sysdate,user_update='%s' where devicename=:dname_old; :numrows:=%s; END;",LOGICAL_DEVICE_TABLE,login,SQLROWCOUNT);
				StmtPrepare(hstmt,ociError, sqlstmt, &status);

				BindByName(hstmt,&bndp[0],ociError,":dname",dname, max_namelen,SQLT_STR,&name_nullList[0],&status);
				BindByName(hstmt,&bndp[1],ociError,":dname_old",dname_old, max_nameoldlen,SQLT_STR,&nameold_nullList[0],&status);
				BindByName(hstmt,&bndp[2],ociError,":numrows",&numrows_inserted[0],sizeof(int),SQLT_INT,0,&status);

				BindArrayOfStruct(bndp[0],ociError,max_namelen,sizeof(int),&status);
				BindArrayOfStruct(bndp[1],ociError,max_nameoldlen,sizeof(int),&status);
				BindArrayOfStruct(bndp[2],ociError,sizeof(int),0,&status);
				
				StmtExecute(ociHdbc, hstmt, ociError,NbElement, &status);
			}catch(Error err){
				sprintf(appliName,"UpdateMultipleDevNamesDevices");	///////
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
				
                if(dname!=NULL)
					free(dname);
				if(dname_old!=NULL)
					free(dname_old);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(name_nullList!=NULL)
					free(name_nullList);
				if(nameold_nullList!=NULL)
					free(nameold_nullList);
								
				for(i=0;i<NbElement;i++)
					delete deviceList[i];
				if(force_insert==1)
				{
                    FIRST_TIME=1;
					force_insert=0;
					deviceList=(UpdtDevice**)realloc(deviceList, 0*sizeof(UpdtDevice));
				}
				else if(deviceList!=NULL)
                    free(deviceList);

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
				delete deviceList[i];
			if(force_insert==1)
			{
				FIRST_TIME=1;
				force_insert=0;
				deviceList=(UpdtDevice**)realloc(deviceList, 0*sizeof(UpdtDevice));
			}
			else if(deviceList!=NULL)
                free(deviceList);
			NbElement=0;
		}
		else
		{
			if(res_query!=0)
			{
				FIRST_TIME=0;
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
				for(i=0;i<NbElement;i++)
					delete deviceList[i];
				if(deviceList!=NULL)
                    free(deviceList);
			}
			
			if(ociEnv==0)
			{
				FIRST_TIME=0;
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
				for(i=0;i<NbElement;i++)
					delete deviceList[i];
				if(deviceList!=NULL)
                    free(deviceList);
			}
			status+=res_query;
			if(ociEnv!=0 && FIRST_TIME==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}
		}
		if(dname!=NULL)
			free(dname);
		if(dname_old!=NULL)
			free(dname_old);
		if(numrows_inserted!=NULL)
			free(numrows_inserted);
		if(name_nullList!=NULL)
			free(name_nullList);
		if(nameold_nullList!=NULL)
			free(nameold_nullList);

		return (status+rescode);
	}


/**
* Update the devicetype field of a device :make sure that the devicetype exists 1  returning an integer value.
* The user should manage the memory : there is no memory allocation.
* @param devicename : name of the device you want to update
* @param devicetype : new value of the devicetype, 
* @param first_time1 : 1 if it's your first MultipleDevice update (save it into the database), 0 otherwise
* @param last_rows1 : 1 if it's your MultipleDevice last update (save it into the database), 0 otherwise
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
******************************************************************/
EXTERN_CONFDB
	int UpdateMultipleDTypeDevices(char* devicename,char* devicetype,int first_time,int last_rows,char* ErrMess)
	{
		char appliName[100]="UpdateMultipleDTypeDevices";
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[3]; 
		int rescode=0;
		int res_query=0;
		sword status=0;
		int i=0;

		char* dname=NULL;		
		char* dtype=NULL;
		int* type_nullList=NULL;
		int* name_nullList=NULL;		
		static int FIRST_TIME=0;

		int force_insert=0;
		int* numrows_inserted=NULL;
		int numrows=0;
		
		static int NbElement=0;
		UpdtDevice** deviceList;
		int max_namelen=0;
		int max_typelen=0;
		
		if (first_time==1)
		{
			FIRST_TIME=1;
			NbElement=1;
			deviceList=(UpdtDevice**)malloc( NbElement*sizeof(UpdtDevice));			
		}
		else 
		{
			if (FIRST_TIME==0&&force_insert==0)
			{
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
				return -1;
			}
			NbElement++;
			deviceList=(UpdtDevice**)realloc(deviceList, NbElement*sizeof(UpdtDevice));
		}

		deviceList[NbElement-1]=new UpdtDevice(devicetype,devicename);

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
				if(dname!=NULL)	
					free(dname);
				if(dtype!=NULL)
					free(dtype);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(name_nullList!=NULL)
					free(name_nullList);
				if(type_nullList!=NULL)
					free(type_nullList);
				
				for(i=0;i<NbElement;i++)
					delete deviceList[i];
				if(deviceList!=NULL)
					free(deviceList);
				
				free(errmessg1);
				free(errmessg2);
				return -1;
			}
			for(i=0;i<NbElement;i++){
				if (deviceList[i]->namelen > max_namelen)
					max_namelen=deviceList[i]->namelen;
				if (deviceList[i]->nameoldlen > max_typelen)
					max_typelen=deviceList[i]->nameoldlen;
			}
			dname=(char*)malloc( NbElement*max_namelen*sizeof(char));
			dtype=(char*)malloc(NbElement*max_typelen*sizeof(char));
			name_nullList=(int*)malloc(NbElement*sizeof(int));
			type_nullList=(int*)malloc(NbElement*sizeof(int));

			if(name_nullList!=NULL && type_nullList!=NULL && dname!=NULL && dtype!=NULL)
			for(i=0;i<NbElement;i++)
			{
				memcpy(dname+i*max_namelen,deviceList[i]->name,deviceList[i]->namelen);
				name_nullList[i]=deviceList[i]->name_null;
				memcpy(dtype+i*max_typelen,deviceList[i]->nameold,deviceList[i]->nameoldlen);
				type_nullList[i]=deviceList[i]->nameold_null;
			}
	
			numrows_inserted=(int*)malloc(sizeof(int)*NbElement);

			if(dtype==NULL || dname==NULL || numrows_inserted==NULL ||name_nullList==NULL || type_nullList==NULL)
			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				
				FIRST_TIME=0;
				
				if(dname!=NULL)
					free(dname);
				if(dtype!=NULL)
					free(dtype);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(name_nullList!=NULL)
					free(name_nullList);
				if(type_nullList!=NULL)
					free(type_nullList);
				
				for(i=0;i<NbElement;i++)
					delete deviceList[i];
				if(deviceList!=NULL)
                    free(deviceList);
				
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
				sprintf(sqlstmt,"BEGIN update %s set devicetypeID=nvl((select t.devicetypeID from %s t where t.devicetype=:dtype),-1), last_update=sysdate,user_update='%s' where devicename=:dname; :numrows:=%s; END;",LOGICAL_DEVICE_TABLE,DEVICETYPE_TABLE,login,SQLROWCOUNT);
				StmtPrepare(hstmt,ociError, sqlstmt, &status);

				BindByName(hstmt,&bndp[0],ociError,":dname",dname, max_namelen,SQLT_STR,&name_nullList[0],&status);
				BindByName(hstmt,&bndp[1],ociError,":dtype",dtype,max_typelen,SQLT_STR,&type_nullList[0],&status);
				BindByName(hstmt,&bndp[2],ociError,":numrows",&numrows_inserted[0],sizeof(int),SQLT_INT,0,&status);

				BindArrayOfStruct(bndp[0],ociError,max_namelen,sizeof(int),&status);
				BindArrayOfStruct(bndp[1],ociError,max_typelen,sizeof(int),&status);
				BindArrayOfStruct(bndp[2],ociError,sizeof(int),0,&status);
				
				StmtExecute(ociHdbc, hstmt, ociError,NbElement, &status);
			}catch(Error err){
				sprintf(appliName,"UpdateMultipleDTypeDevices");	///////
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
				
                if(dname!=NULL)
					free(dname);
				if(dtype!=NULL)
					free(dtype);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(name_nullList!=NULL)
					free(name_nullList);
				if(type_nullList!=NULL)
					free(type_nullList);
				
				for(i=0;i<NbElement;i++)
					delete deviceList[i];
				if(force_insert==1)
				{
                    FIRST_TIME=1;
					force_insert=0;
					deviceList=(UpdtDevice**)realloc(deviceList, 0*sizeof(UpdtDevice));
				}
				else if(deviceList!=NULL)
                    free(deviceList);

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
				delete deviceList[i];
			if(force_insert==1)
			{
				FIRST_TIME=1;
				force_insert=0;
				deviceList=(UpdtDevice**)realloc(deviceList, 0*sizeof(UpdtDevice));
			}
			else if(deviceList!=NULL)
                free(deviceList);
			NbElement=0;
		}
		else
		{
			if(res_query!=0)
			{
				FIRST_TIME=0;
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
				for(i=0;i<NbElement;i++)
					delete deviceList[i];
				if(deviceList!=NULL)
                    free(deviceList);
			}			
			if(ociEnv==0)
			{
				FIRST_TIME=0;
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
				for(i=0;i<NbElement;i++)
					delete deviceList[i];
				if(deviceList!=NULL)
                    free(deviceList);
			}
			status+=res_query;
			if(ociEnv!=0 && FIRST_TIME==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}
		}
		if(dname!=NULL)
			free(dname);
		if(dtype!=NULL)
			free(dtype);
		if(numrows_inserted!=NULL)
			free(numrows_inserted);
		if(name_nullList!=NULL)
			free(name_nullList);
		if(type_nullList!=NULL)
			free(type_nullList);

		return (status+rescode);
	}
/******************************************************/
//nodeused either 1 or 0 
//0 means it won't be take into acount in routing tables and so on
/**
* Update the nodeused parameter of a device  returning an integer value.
* The user should manage the memory : there is no memory allocation.
* @param devicename : name of the device
* @param new_systemList : new list of system, UPPER CASE, separated with a coma
* @param first_time1 : 1 if it's your first MultipleDeviceNodeUsed update (save it into the database), 0 otherwise
* @param last_rows1 : 1 if it's your MultipleDeviceNodeUsed last update (save it into the database), 0 otherwise
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
*/
/*********************************************************************/
EXTERN_CONFDB 
	int UpdateMultipleDeviceSystemList(char* devicename,char* new_systemList,int first_time,int last_rows,char* ErrMess)
	{
		char appliName[100]="UpdateMultipleDeviceSystemList";
		char sqlstmt[2000];
		OCIStmt *hstmt;
		OCIBind  *bndp[5]; 
		int rescode=0;
		int res_query=0;
		char res_query1[100]="";
		sword status=0;
		int i=0;
		
		char* dname=NULL;		
		int* sysIDList=NULL;
		int* name_nullList=NULL;	
		static int FIRST_TIME=0;

		int force_insert=0;
		int* numrows_inserted=NULL;
		int* numrows_inserted1=NULL;
		int numrows=0;

		static int NbElement=0;
		UpdtDevice** deviceList;
		int max_namelen=0;
		
		if (first_time==1)
		{
			FIRST_TIME=1;
			NbElement=1;
			deviceList=(UpdtDevice**)malloc( NbElement*sizeof(UpdtDevice));			
		}
		else 
		{
			if (FIRST_TIME==0&&force_insert==0)
			{
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
				return -1;
			}
			NbElement++;
			deviceList=(UpdtDevice**)realloc(deviceList, NbElement*sizeof(UpdtDevice));
		}
		int sysID=GetSubsystemID(new_systemList);
		if(sysID==-1)
		{
			GetErrorMess(appliName, "systemnameList is null",ErrMess,1);
			return -1;
		}
		deviceList[NbElement-1]=new UpdtDevice(devicename,NULL);
		deviceList[NbElement-1]->sysID=sysID; 
						
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
				if(dname!=NULL)	
					free(dname);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(name_nullList!=NULL)
					free(name_nullList);
				if(sysIDList!=NULL)
					free(sysIDList);

				for(i=0;i<NbElement;i++)
					delete deviceList[i];
				if(deviceList!=NULL)
					free(deviceList);
				
				free(errmessg1);
				free(errmessg2);
				return -1;
			}
			for(i=0;i<NbElement;i++){
				if (deviceList[i]->namelen > max_namelen)
					max_namelen=deviceList[i]->namelen;				
			}

			dname=(char*)malloc( NbElement*max_namelen*sizeof(char));
			sysIDList=(int*)malloc(sizeof(int)*NbElement);			
			name_nullList=(int*)malloc(NbElement*sizeof(int));

			if(name_nullList!=NULL && dname!=NULL && sysIDList!=NULL)
			for(i=0;i<NbElement;i++)
			{
				memcpy(dname+i*max_namelen,deviceList[i]->name,deviceList[i]->namelen);
				name_nullList[i]=deviceList[i]->name_null;
				sysIDList[i]=deviceList[i]->sysID;
			}
			numrows_inserted=(int*)malloc(sizeof(int)*NbElement);
			numrows_inserted1=(int*)malloc(sizeof(int)*NbElement);

			if(dname==NULL || numrows_inserted==NULL || numrows_inserted1==NULL ||name_nullList==NULL || sysIDList==NULL)
			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				
				FIRST_TIME=0;
				
				if(dname!=NULL)
					free(dname);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(name_nullList!=NULL)
					free(name_nullList);
				if(sysIDList!=NULL)
					free(sysIDList);
				
				for(i=0;i<NbElement;i++)
					delete deviceList[i];
				if(deviceList!=NULL)
                    free(deviceList);
				
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
				sprintf(sqlstmt,"BEGIN update %s set system_name=:sysid,last_update=sysdate,user_update='%s' where devicename=:dname; if :numrows1=1 then update %s g set g.last_update=sysdate,g.user_update='%s',g.system_name=(select case when ceil(power(10,sum(log(10,t.systemID))))-power(10,sum(log(10,t.systemID)))>0.5  then floor(power(10,sum(log(10,t.systemid)))) else ceil(power(10,sum(log(10,t.systemid)))) end from %s t,%s o,%s d,%s p,%s h where t.systemid not in (select v.systemid from %s v,%s q where mod(q.systemid,v.systemid)=0 and q.systemid!=v.systemid) and mod(o.system_name,t.systemid)=0 and mod(d.system_name,t.systemid)=0 AND g.portidfrom=p.portid and p.deviceid=o.deviceid and g.portidto=h.portid and h.deviceid=d.deviceid ) where g.system_name is not null; :rescode:=%s(%d);end if; :numrows:=%s; END;",LOGICAL_DEVICE_TABLE,login,MACRO_CONNECTIVITY_TABLE,login,SUBSYSTEM_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,PORT_TABLE,PORT_TABLE,SUBSYSTEM_TABLE,SUBSYSTEM_TABLE,_UpdateDynamicTables,sysID,SQLROWCOUNT);
				StmtPrepare(hstmt,ociError, sqlstmt, &status);

				BindByName(hstmt,&bndp[0],ociError,":dname",dname, max_namelen,SQLT_STR,&name_nullList[0],&status);
				BindByName(hstmt,&bndp[1],ociError,":sysid",&sysIDList[0],sizeof(int),SQLT_INT,0,&status);
				BindByName(hstmt,&bndp[2],ociError,":numrows",&numrows_inserted[0],sizeof(int),SQLT_INT,0,&status);
				BindByName(hstmt,&bndp[3],ociError,":numrows1",&numrows_inserted1[0],sizeof(int),SQLT_INT,0,&status);
				BindByName(hstmt,&bndp[4],ociError,":rescode",res_query1,101,SQLT_STR,0,&status);

				BindArrayOfStruct(bndp[0],ociError,max_namelen,sizeof(int),&status);
				BindArrayOfStruct(bndp[1],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[2],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[3],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[4],ociError,sizeof(int),0,&status);

				StmtExecute(ociHdbc, hstmt, ociError,NbElement, &status);
			}catch(Error err){
				sprintf(appliName,"UpdateMultipleDeviceSystemList");	///////
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
			
			if(res_query!=0)
			{
				status = OCITransCommit(ociHdbc, ociError, 0);
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				FIRST_TIME=0;
				
                if(dname!=NULL)
					free(dname);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(numrows_inserted1!=NULL)
					free(numrows_inserted1);
				if(name_nullList!=NULL)
					free(name_nullList);
				if(sysIDList!=NULL)
					free(sysIDList);
				
				for(i=0;i<NbElement;i++)
					delete deviceList[i];
				if(force_insert==1)
				{
                    FIRST_TIME=1;
					force_insert=0;
					deviceList=(UpdtDevice**)realloc(deviceList, 0*sizeof(UpdtDevice));
				}
				else if(deviceList!=NULL)
                    free(deviceList);

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
				delete deviceList[i];
			if(force_insert==1)
			{
				FIRST_TIME=1;
				force_insert=0;
				deviceList=(UpdtDevice**)realloc(deviceList, 0*sizeof(UpdtDevice));
			}
			else if(deviceList!=NULL)
                free(deviceList);
			NbElement=0;
		}
		else
		{
			if(res_query!=0)
			{
				FIRST_TIME=0;
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
				for(i=0;i<NbElement;i++)
					delete deviceList[i];
				if(deviceList!=NULL)
                    free(deviceList);
			}
			
			if(ociEnv==0)
			{
				FIRST_TIME=0;
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
				for(i=0;i<NbElement;i++)
					delete deviceList[i];
				if(deviceList!=NULL)
                    free(deviceList);
			}
			status+=res_query;
			if(ociEnv!=0 && FIRST_TIME==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}
		}		
		if(dname!=NULL)
			free(dname);
		if(numrows_inserted!=NULL)
			free(numrows_inserted);
		if(numrows_inserted1!=NULL)
			free(numrows_inserted1);		
		if(name_nullList!=NULL)
			free(name_nullList);
		if(sysIDList!=NULL)
			free(sysIDList);
		
		return (status+rescode);
	}

/******************************************************/
/* Update the name of the function, typically when there was a mistyping
* The user should manage the memory : there is no memory allocation.
* @param  old_function_name : name of the old function
* @param new_function_name : name of the new function
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
*********************************************************************/
EXTERN_CONFDB 
	int UpdateDeviceFunctionName(char* old_function_name,char* new_function_name,char* ErrMess)
	{
		char appliName[100]="UpdateDeviceFunctionName";
		char sqlstmt[2000];
		OCIStmt *hstmt;
		OCIBind  *bndp[3]; 
		int rescode=0;
		int res_query=0;
		sword status=0;
		int numrows_inserted;

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
			sprintf(sqlstmt,"BEGIN update %s set function_name=:new_fct,last_update=sysdate,user_update='%s' where function_name=:old_fct; :numrows:=%s; END;",DEVICE_FUNCTION_TABLE,login,SQLROWCOUNT);
			StmtPrepare(hstmt,ociError, sqlstmt, &status);
			BindByName(hstmt,&bndp[0],ociError,":new_fct",new_function_name,strlen(new_function_name)+1,SQLT_STR,0,&status);
			BindByName(hstmt,&bndp[1],ociError,":old_fct",old_function_name,strlen(old_function_name)+1,SQLT_STR,0,&status);
			BindByName(hstmt,&bndp[2],ociError,":numrows",&numrows_inserted,sizeof(int),SQLT_INT,0,&status);
			StmtExecute(ociHdbc, hstmt, ociError,1, &status);
			
		}catch(Error err){
			sprintf(appliName,"UpdateDeviceFunctionName");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,err.msg,ErrMess,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}

		if (numrows_inserted=0)
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

/******************************************************/
/*Update the serialnb , typically when there was a mistyping
* The user should manage the memory : there is no memory allocation.
* @param  old_serialnb : old serialnb
* @param new_serialnb : new serialnb
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
*********************************************************************/
EXTERN_CONFDB
	int UpdateHWSerialNB(char* old_serialnb,char* new_serialnb,char* ErrMess)
	{	
		char appliName[100]="UpdateHWSerialNB";
		char sqlstmt[2000];
		OCIStmt *hstmt;
		OCIBind  *bndp[3]; 
		int rescode=0;
		int res_query=0;
		int deviceid=-1;
		sword status=0;
		int numrows_inserted=0;

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
			sprintf(sqlstmt,"BEGIN update %s set serialnb=NULL where serialnb=:old_snb returning deviceid into :devid;   if %s=1 then :numrows:=%s(:old_snb,:new_snb,:devid); else :numrows:=%s(:old_snb,:new_snb,:devid); end if; END;",LOGICAL_DEVICE_TABLE,SQLROWCOUNT,_UpdateHistory,_UpdateHistory);
			StmtPrepare(hstmt,ociError, sqlstmt, &status);
			BindByName(hstmt,&bndp[0],ociError,":old_snb",old_serialnb,strlen(old_serialnb)+1,SQLT_STR,0,&status);
			BindByName(hstmt,&bndp[1],ociError,":new_snb",new_serialnb,strlen(new_serialnb)+1,SQLT_STR,0,&status);
			BindByName(hstmt,&bndp[2],ociError,":devid",&deviceid,sizeof(int),SQLT_INT,0,&status);
			BindByName(hstmt,&bndp[3],ociError,":numrows",&numrows_inserted,sizeof(int),SQLT_INT,0,&status);
			StmtExecute(ociHdbc, hstmt, ociError,1, &status);		
			
		}catch(Error err){
			sprintf(appliName,"UpdateHWSerialNB");	///////
			rescode=ShowErrors (status, err.ociError, err.log);
			
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,err.msg,ErrMess,1);
			
			if(strstr(err.msg,"NOT CONNECTED TO ANY DB")!=NULL)
			return -1;
		}
		
		if (numrows_inserted==0)
			status = OCITransCommit(ociHdbc, ociError, 0);

		else
		{
			status = OCITransRollback(ociHdbc, ociError, 0);
			status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
			GetErrorMess(appliName, "COULDNOT_UPDATE_ROW",ErrMess,1);
			return -1;
		}
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