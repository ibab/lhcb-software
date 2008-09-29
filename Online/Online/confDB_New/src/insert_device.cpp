/********************************************************************************/
//	Author: L.Abadie
// version:v3.5
// changes: handling error such that can be extended to C++: (create a class SQLException) (strlen ck + update stmt)
// file which performs bulk collect insert for the device table
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
* Insert a device into the functional_device table and the hw table :it means that the hw device is currently IN_USE and it's having the given functional fct returning an integer value.
* The user should manage the memory : there is no memory allocation.
* @param systemnameList : list of the subsystems (UPPER CASE) by which this device type is used: a VELO_TELL1 board  is used by the VELO, TFC and DAQ. so you type VELO,TFC,DAQ. you separate the different subsystems with a coma.
* @param devicename : functional name of the device
* @param devicetype : name of the devicetype
* @param node : 1 if it's a host node (usually starts or ends a path such as L1FEs) , 0 otherwise.
* @param promiscuous_mode : 1 if put in this mode (default mode), 0 otherwise
* @param serial_nb :  serial_nb of the device
* @param hwtype :hardware type : this one should be the same as you declared for your spare: in the case of a VELO_TELL1_board, here you can put Tell1_board as they are common to all the subsystems
* @param responsible :guy responsible for this hw
* @param comments : some comments about the hw (max.1000 characters)
* @param location :  location of the device : crate\\rack\\slot (as defined in the equipment DB) or on the detector
* @param function_list : list of the functions of your devices (essentially for the DAQ, for instance DHCP_SERVER, DNS_SERVER...). Functions must be declared using insertSimpleDeviceFunction...If several, separate them with a coma. For ex. DHCP,DNS, NFS. if none, put "none"
* @param first_time : 1 if it's your first MultipleDevice insert (save it into the database), 0 otherwise
* @param last_rows1 : 1 if it's your MultipleDevice last insert (save it into the database), 0 otherwise  Maximum in one go: 10000 rows.( we force the insert)
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
*/

//extern "C" __declspec(dllexport)
EXTERN_CONFDB
	int InsertMultipleFunctionalDevices(char* systemnameList,char* devicename,char* devicetype,int node,int promiscuous_mode,char* serial_nb,char* hwtype,char* responsible,char* comments,char* location,char* function_list,int first_time,int last_rows,char* ErrMess)
	{
		char appliName[100]="InsertMultipleFunctionalDevices";
		char sqlstmt[2000];
		OCIStmt *hstmt;
		OCIBind  *bndp[12]; 
		int rescode=0;
		int res_query=0;
		sword status=0;
		int i=0;

		char* dname=NULL;		
		char* dlocation=NULL;
		char* dserialnb=NULL;
		char* dtype=NULL;
		char* dresponsible=NULL;
		char* dcomments=NULL;
		char* dhwtype=NULL;
		char* dfunctiondev=NULL;
		int* Node=NULL;
		int* Promiscuous_mode=NULL;
		int* sysIDList=NULL;

		int* hwtype_nullList=NULL;
		int* comments_nullList=NULL;
		int* responsible_nullList=NULL;
		int* type_nullList=NULL;
		int* serialnb_nullList=NULL;
		int* name_nullList=NULL;
		int* location_nullList=NULL;
		int* function_nullList=NULL;
	
		static int FIRST_TIME=0;

		int force_insert=0;
		char seqname[100]="lhcb_history_deviceseq.nextval";
		char seq_name[100]="lhcb_lg_deviceseq.nextval";
		int* numrows_inserted=NULL;
		int numrows=0;
		
		static int NbElement=0;
		Device** deviceList;
		int max_namelen=0;
		int max_serialnblen=0;
		int max_locationlen=0;
		int max_typelen=0;
		int max_hwtypelen=0;
		int max_commentslen=0;
		int max_responsiblelen=0;
		int max_functionlen=0;

		if (first_time==1)
		{
			FIRST_TIME=1;
			NbElement=1;
			deviceList=(Device**)malloc( NbElement*sizeof(Device));			
		}
		else 
		{
			if (FIRST_TIME==0&&force_insert==0)
			{
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
				return -1;
			}
			NbElement++;
			deviceList=(Device**)realloc(deviceList, NbElement*sizeof(Device));
		}

		deviceList[NbElement-1]=new Device(systemnameList,devicename,devicetype,node,promiscuous_mode,serial_nb,hwtype,responsible,comments,location,function_list);

		int sysID=GetSubsystemID(systemnameList);
		if(sysID==-1)
		{
			GetErrorMess(appliName, "systemnameList is null",ErrMess,1);
			return -1;
		}
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
				if(dtype!=NULL)
					free(dtype);
				if(dlocation!=NULL)
					free(dlocation);
				if(dserialnb!=NULL)
					free(dserialnb);
				if(dresponsible!=NULL)
					free(dresponsible);
				if(dcomments!=NULL)
					free(dcomments);
				if(dhwtype!=NULL)
					free(dhwtype);
				if(dfunctiondev!=NULL)
					free(dfunctiondev);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(name_nullList!=NULL)
					free(name_nullList);
				if(serialnb_nullList!=NULL)
					free(serialnb_nullList);
				if(location_nullList!=NULL)
					free(location_nullList);
				if(type_nullList!=NULL)
					free(type_nullList);
				if(hwtype_nullList!=NULL)
					free(hwtype_nullList);
				if(comments_nullList!=NULL)
					free(comments_nullList);
				if(responsible_nullList!=NULL)
					free(responsible_nullList);
				if(function_nullList!=NULL)
					free(function_nullList);
				if(Node!=NULL)
					free(Node);
				if(Promiscuous_mode!=NULL)
					free(Promiscuous_mode);
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
				if (deviceList[i]->serialnblen > max_serialnblen)
					max_serialnblen=deviceList[i]->serialnblen;
				if (deviceList[i]->locationlen > max_locationlen)
					max_locationlen=deviceList[i]->locationlen;
				if (deviceList[i]->typelen > max_typelen)
					max_typelen=deviceList[i]->typelen;
				if (deviceList[i]->hwtypelen > max_hwtypelen)
					max_hwtypelen=deviceList[i]->hwtypelen;
				if (deviceList[i]->commentslen > max_commentslen)
					max_commentslen=deviceList[i]->commentslen;
				if (deviceList[i]->responsiblelen > max_responsiblelen)
					max_responsiblelen=deviceList[i]->responsiblelen;
				if (deviceList[i]->functionlen > max_functionlen)
					max_functionlen=deviceList[i]->functionlen;
			}

			dname=(char*)malloc( NbElement*max_namelen*sizeof(char));
			dserialnb=(char*)malloc(NbElement*max_serialnblen*sizeof(char));
			dlocation=(char*)malloc(NbElement*max_locationlen*sizeof(char));
			dtype=(char*)malloc(NbElement*max_typelen*sizeof(char));
			dhwtype=(char*)malloc(NbElement*max_hwtypelen*sizeof(char));
			dcomments=(char*)malloc(NbElement*max_commentslen*sizeof(char));
			dresponsible=(char*)malloc(NbElement*max_responsiblelen*sizeof(char));
			dfunctiondev=(char*)malloc(NbElement*max_functionlen*sizeof(char));
			Node=(int*)malloc(sizeof(int)*NbElement);
			Promiscuous_mode=(int*)malloc(sizeof(int)*NbElement);
			sysIDList=(int*)malloc(sizeof(int)*NbElement);
			
			name_nullList=(int*)malloc(NbElement*sizeof(int));
			serialnb_nullList=(int*)malloc(NbElement*sizeof(int));
			location_nullList=(int*)malloc(NbElement*sizeof(int));
			type_nullList=(int*)malloc(NbElement*sizeof(int));
			hwtype_nullList=(int*)malloc(NbElement*sizeof(int));
			comments_nullList=(int*)malloc(NbElement*sizeof(int));
			responsible_nullList=(int*)malloc(NbElement*sizeof(int));
			function_nullList=(int*)malloc(NbElement*sizeof(int));

			if(name_nullList!=NULL && serialnb_nullList!=NULL && location_nullList!=NULL && type_nullList!=NULL && hwtype_nullList!=NULL && comments_nullList!=NULL && responsible_nullList!=NULL && function_nullList!=NULL 
				&& dname!=NULL && dserialnb!=NULL && dlocation!=NULL && dtype!=NULL && dhwtype!=NULL && dcomments!=NULL && dresponsible!=NULL && dfunctiondev!=NULL && Node!=NULL && Promiscuous_mode!=NULL && sysIDList!=NULL)
			for(i=0;i<NbElement;i++)
			{
				memcpy(dname+i*max_namelen,deviceList[i]->name,deviceList[i]->namelen);
				name_nullList[i]=deviceList[i]->name_null;
				memcpy(dserialnb+i*max_serialnblen,deviceList[i]->serialnb,deviceList[i]->serialnblen);
				serialnb_nullList[i]=deviceList[i]->serialnb_null;
				memcpy(dlocation+i*max_locationlen,deviceList[i]->location,deviceList[i]->locationlen);
				location_nullList[i]=deviceList[i]->location_null;
				memcpy(dtype+i*max_typelen,deviceList[i]->type,deviceList[i]->typelen);
				type_nullList[i]=deviceList[i]->type_null;
				memcpy(dhwtype+i*max_hwtypelen,deviceList[i]->hwtype,deviceList[i]->hwtypelen);					
				hwtype_nullList[i]=deviceList[i]->hwtype_null;
				memcpy(dcomments+i*max_commentslen,deviceList[i]->comments,deviceList[i]->commentslen);					
				comments_nullList[i]=deviceList[i]->comments_null;
				memcpy(dresponsible+i*max_responsiblelen,deviceList[i]->responsible,deviceList[i]->responsiblelen);					
				responsible_nullList[i]=deviceList[i]->responsible_null;
				memcpy(dfunctiondev+i*max_functionlen,deviceList[i]->function,deviceList[i]->functionlen);
				function_nullList[i]=deviceList[i]->function_null;
				Node[i]=deviceList[i]->node;
				Promiscuous_mode[i]=deviceList[i]->prom_mode;
				sysIDList[i]=deviceList[i]->sysID;
			}
	
			numrows_inserted=(int*)malloc(sizeof(int)*NbElement);

			if( (dtype==NULL) || dlocation==NULL || (dname==NULL) || dfunctiondev==NULL || (dserialnb==NULL) || (dresponsible==NULL)||dcomments==NULL||dhwtype==NULL||numrows_inserted==NULL 
				||name_nullList==NULL || serialnb_nullList==NULL || location_nullList==NULL || type_nullList==NULL || hwtype_nullList==NULL || comments_nullList==NULL || responsible_nullList==NULL || function_nullList==NULL || Node==NULL || Promiscuous_mode==NULL || sysIDList==NULL)

			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				
				FIRST_TIME=0;
				
				if(dname!=NULL)
					free(dname);
				if(dtype!=NULL)
					free(dtype);
				if(dlocation!=NULL)
					free(dlocation);
				if(dserialnb!=NULL)
					free(dserialnb);
				if(dfunctiondev!=NULL)
					free(dfunctiondev);
				if(dresponsible!=NULL)
					free(dresponsible);
				if(dcomments!=NULL)
					free(dcomments);
				if(dhwtype!=NULL)
					free(dhwtype);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(name_nullList!=NULL)
					free(name_nullList);
				if(serialnb_nullList!=NULL)
					free(serialnb_nullList);
				if(location_nullList!=NULL)
					free(location_nullList);
				if(type_nullList!=NULL)
					free(type_nullList);
				if(hwtype_nullList!=NULL)
					free(hwtype_nullList);
				if(comments_nullList!=NULL)
					free(comments_nullList);
				if(responsible_nullList!=NULL)
					free(responsible_nullList);
				if(function_nullList!=NULL)
					free(function_nullList);
				if(Node!=NULL)
					free(Node);
				if(Promiscuous_mode!=NULL)
					free(Promiscuous_mode);
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
					numrows_inserted[i]=0;
			}			
			
			try{
                HandleAlloc(ociEnv,(dvoid**)&hstmt,OCI_HTYPE_STMT,&status);
				sprintf(sqlstmt,"BEGIN insert all into %s(serialnb,hwtype,responsible,user_comments,device_status) values(:serialnb,:hwtype,:responsible,:comments,%d) select 1 from %s where devicetype=:dtype; insert all into %s(devicename,devicetypeID,node,deviceid,promiscuous_mode,system_name,serialnb,location,author,user_update,terminal_name,nodeused,functionID) values (:dname,devicetypeID,:dnode,%s,:dpromiscuous_mode,:sysID,:serialnb,:location,'%s','%s','%s',1,fctID) select devicetypeID,decode(:fct_list,'none',0,%s(:fct_list)) as fctID  from %s  where devicetype=:dtype; insert all into %s(historydeviceID,serialnb,deviceid,device_status,user_comment,location,status_change,system_name,created,author,terminal_name) values(%s,:serialnb,deviceid,'IN_USE',:comments,:location,statuschange,:sysID,sysdate,'%s','%s') select deviceid,created as statuschange from %s where devicename=:dname; :numrows:=%s; END;",HW_DEVICE_TABLE,IN_USE,DEVICETYPE_TABLE,LOGICAL_DEVICE_TABLE,seq_name,login,login,host,_ComposeFctID,DEVICETYPE_TABLE,HISTORY_DEVICE_TABLE,seqname,login,host,LOGICAL_DEVICE_TABLE,SQLROWCOUNT);
				StmtPrepare(hstmt,ociError, sqlstmt, &status);

				BindByName(hstmt,&bndp[0],ociError,":dname",dname, max_namelen,SQLT_STR,&name_nullList[0],&status);
				BindByName(hstmt,&bndp[1],ociError,":dtype",dtype,max_typelen,SQLT_STR,&type_nullList[0],&status);
				BindByName(hstmt,&bndp[2],ociError,":dnode",&Node[0],sizeof(&deviceList[0]->node),SQLT_INT,0,&status);				
				BindByName(hstmt,&bndp[3],ociError,":dpromiscuous_mode",&Promiscuous_mode[0],sizeof(&deviceList[0]->node),SQLT_INT,0,&status);
				BindByName(hstmt,&bndp[4],ociError,":sysID",&sysIDList[0],sizeof(&deviceList[0]->sysID),SQLT_INT,0,&status);
				BindByName(hstmt,&bndp[5],ociError,":serialnb",dserialnb,max_serialnblen,SQLT_STR,&serialnb_nullList[0],&status);
				BindByName(hstmt,&bndp[6],ociError,":location",dlocation,max_locationlen,SQLT_STR,&location_nullList[0],&status);
				BindByName(hstmt,&bndp[7],ociError,":hwtype",dhwtype,max_hwtypelen,SQLT_STR,&hwtype_nullList[0],&status);
				BindByName(hstmt,&bndp[8],ociError,":responsible",dresponsible,max_responsiblelen,SQLT_STR,&responsible_nullList[0],&status);
				BindByName(hstmt,&bndp[9],ociError,":comments",dcomments,max_commentslen,SQLT_STR,&comments_nullList[0],&status);
				BindByName(hstmt,&bndp[10],ociError,":fct_list",dfunctiondev,max_functionlen,SQLT_STR,&function_nullList[0],&status);
				BindByName(hstmt,&bndp[11],ociError,":numrows",&numrows_inserted[0],sizeof(int),SQLT_INT,0,&status);
	
				BindArrayOfStruct(bndp[0],ociError,max_namelen,sizeof(int),&status);
				BindArrayOfStruct(bndp[1],ociError,max_typelen,sizeof(int),&status);
				BindArrayOfStruct(bndp[2],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[3],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[4],ociError,sizeof(int),0,&status);
				BindArrayOfStruct(bndp[5],ociError,max_serialnblen,sizeof(int),&status);
				BindArrayOfStruct(bndp[6],ociError,max_locationlen,sizeof(int),&status);
				BindArrayOfStruct(bndp[7],ociError,max_hwtypelen,sizeof(int),&status);
				BindArrayOfStruct(bndp[8],ociError,max_responsiblelen,sizeof(int),&status);
				BindArrayOfStruct(bndp[9],ociError,max_commentslen,sizeof(int),&status);
				BindArrayOfStruct(bndp[10],ociError,max_functionlen,sizeof(int),&status);
				BindArrayOfStruct(bndp[11],ociError,sizeof(int),0,&status);

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
				if(dtype!=NULL)
					free(dtype);
				if(dlocation!=NULL)
					free(dlocation);
				if(dserialnb!=NULL)
					free(dserialnb);
				if(dresponsible!=NULL)
					free(dresponsible);
				if(dcomments!=NULL)
					free(dcomments);
				if(dhwtype!=NULL)
					free(dhwtype);
				if(dfunctiondev!=NULL)
					free(dfunctiondev);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(name_nullList!=NULL)
					free(name_nullList);
				if(serialnb_nullList!=NULL)
					free(serialnb_nullList);
				if(location_nullList!=NULL)
					free(location_nullList);
				if(type_nullList!=NULL)
					free(type_nullList);
				if(hwtype_nullList!=NULL)
					free(hwtype_nullList);
				if(comments_nullList!=NULL)
					free(comments_nullList);
				if(responsible_nullList!=NULL)
					free(responsible_nullList);
				if(function_nullList!=NULL)
					free(function_nullList);
				if(Node!=NULL)
					free(Node);
				if(Promiscuous_mode!=NULL)
					free(Promiscuous_mode);
				if(sysIDList!=NULL)
					free(sysIDList);
				
				for(i=0;i<NbElement;i++)
					delete deviceList[i];
				if(force_insert==1)
				{
                    FIRST_TIME=1;
					force_insert=0;
					deviceList=(Device**)realloc(deviceList, 0*sizeof(Device));
				}
				else if(deviceList!=NULL)
                    free(deviceList);

				NbElement=0;

				GetErrorMess(appliName, "COULDNOT_INSERT_ALL_ROWS",ErrMess,1);
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
				deviceList=(Device**)realloc(deviceList, 0*sizeof(Device));
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
		if(dlocation!=NULL)
			free(dlocation);
		if(dserialnb!=NULL)
			free(dserialnb);
		if(dresponsible!=NULL)
			free(dresponsible);
		if(dcomments!=NULL)
			free(dcomments);
		if(dhwtype!=NULL)
			free(dhwtype);
		if(dfunctiondev!=NULL)
			free(dfunctiondev);
		if(numrows_inserted!=NULL)
			free(numrows_inserted);
		if(name_nullList!=NULL)
			free(name_nullList);
		if(serialnb_nullList!=NULL)
			free(serialnb_nullList);
		if(location_nullList!=NULL)
			free(location_nullList);
		if(type_nullList!=NULL)
			free(type_nullList);
		if(hwtype_nullList!=NULL)
			free(hwtype_nullList);
		if(comments_nullList!=NULL)
			free(comments_nullList);
		if(responsible_nullList!=NULL)
			free(responsible_nullList);
		if(function_nullList!=NULL)
			free(function_nullList);
		if(Node!=NULL)
			free(Node);
		if(Promiscuous_mode!=NULL)
			free(Promiscuous_mode);
		if(sysIDList!=NULL)
			free(sysIDList);
		
		return (status+rescode);
	}

/**
* Insert a spare device into the  the hw table :it means that the hw device has the status SPARE and it's having the given functional fct returning an integer value.
* The user should manage the memory : there is no memory allocation.
* @param serial_nb :  serial_nb of the device
* @param hwname : you can put "spare_nb";
* @param hwtype : hardware type (market specif) prefixed by the subsystem like "VELO_", if the spare is specific to it.hw type is sth which specifies whta type of spare it is so that we can group spare. Ex. Tell1 board is a hw type. 
* @param responsible :guy responsible for this hw
* @param comments : some comments about the hw (max.1000 characters)
* @param location :  location of the spare :which building shelf?
* @param first_time1 : 1 if it's your first MultipleDevice insert (save it into the database), 0 otherwise
* @param last_rows1 : 1 if it's your MultipleDevice last insert (save it into the database), 0 otherwise  Maximum in one go: 10000 rows.( we force the insert)
* @param ErrMess : error message in case of failure
* @return 0 if it is successful
*/

EXTERN_CONFDB
	int InsertMultipleSpareDevices(char* hwname,char* hwtype,char* serial_nb,char* responsible,char* comments,char* location,int first_time,int last_rows,char* ErrMess)
	{
		char appliName[100]="InsertMultipleSpareDevices";
		char sqlstmt[2000];
		OCIStmt *hstmt;
		OCIBind  *bndp[7]; 
		int rescode=0;
		int res_query=0;
		sword status=0;
		int i=0;
		
		char* dhwname=NULL;		
		char* dlocation=NULL;
		char* dserialnb=NULL;
		char* dresponsible=NULL;
		char* dcomments=NULL;
		char* dhwtype=NULL;

		int* hwname_nullList=NULL;
		int* location_nullList=NULL;
		int* serialnb_nullList=NULL;
		int* responsible_nullList=NULL;
		int* comments_nullList=NULL;
		int* hwtype_nullList=NULL;

		static int FIRST_TIME;

		int force_insert=0;
		char seqname[100]="lhcb_history_deviceseq.nextval";
		int* numrows_inserted=NULL;
		int numrows=0;
		
		static int NbElement;
		Device** deviceList;
		int max_hwnamelen=0;
		int max_locationlen=0;
		int max_serialnblen=0;
		int max_responsiblelen=0;
		int max_commentslen=0;
		int max_hwtypelen=0;
		
		if (first_time==1)
		{
			FIRST_TIME=1;
			NbElement=1;
			deviceList=(Device**)malloc( NbElement*sizeof(Device));			
		}
		else 
		{
			if (FIRST_TIME==0&&force_insert==0)
			{
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
				return -1;
			}
			NbElement++;
			deviceList=(Device**)realloc(deviceList, NbElement*sizeof(Device));
		}
		
		deviceList[NbElement-1]=new Device(NULL,hwname,NULL,NULL,NULL,serial_nb,hwtype,responsible,comments,location,NULL);

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
				if(dhwname!=NULL)
					free(dhwname);
				if(dlocation!=NULL)
					free(dlocation);
				if(dserialnb!=NULL)
					free(dserialnb);
				if(dresponsible!=NULL)
					free(dresponsible);
				if(dcomments!=NULL)
					free(dcomments);
				if(dhwtype!=NULL)
					free(dhwtype);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(hwname_nullList!=NULL)
					free(hwname_nullList);
				if(serialnb_nullList!=NULL)
					free(serialnb_nullList);
				if(location_nullList!=NULL)
					free(location_nullList);
				if(hwtype_nullList!=NULL)
					free(hwtype_nullList);
				if(comments_nullList!=NULL)
					free(comments_nullList);
				if(responsible_nullList!=NULL)
					free(responsible_nullList);
				
				for(i=0;i<NbElement;i++)
					delete deviceList[i];
				if(deviceList!=NULL)
					free(deviceList);

				free(errmessg1);
				free(errmessg2);
				return -1;
			}
		
			for(i=0;i<NbElement;i++){
				if (deviceList[i]->namelen > max_hwnamelen)
					max_hwnamelen=deviceList[i]->namelen;
				if (deviceList[i]->serialnblen > max_serialnblen)
					max_serialnblen=deviceList[i]->serialnblen;
				if (deviceList[i]->locationlen > max_locationlen)
					max_locationlen=deviceList[i]->locationlen;
				if (deviceList[i]->hwtypelen > max_hwtypelen)
					max_hwtypelen=deviceList[i]->hwtypelen;
				if (deviceList[i]->commentslen > max_commentslen)
					max_commentslen=deviceList[i]->commentslen;
				if (deviceList[i]->responsiblelen > max_responsiblelen)
					max_responsiblelen=deviceList[i]->responsiblelen;
			}

			dhwname=(char*)malloc( NbElement*max_hwnamelen*sizeof(char));
			dserialnb=(char*)malloc(NbElement*max_serialnblen*sizeof(char));
			dlocation=(char*)malloc(NbElement*max_locationlen*sizeof(char));
			dhwtype=(char*)malloc(NbElement*max_hwtypelen*sizeof(char));
			dcomments=(char*)malloc(NbElement*max_commentslen*sizeof(char));
			dresponsible=(char*)malloc(NbElement*max_responsiblelen*sizeof(char));
			
			hwname_nullList=(int*)malloc(NbElement*sizeof(int));
			serialnb_nullList=(int*)malloc(NbElement*sizeof(int));
			location_nullList=(int*)malloc(NbElement*sizeof(int));
			hwtype_nullList=(int*)malloc(NbElement*sizeof(int));
			comments_nullList=(int*)malloc(NbElement*sizeof(int));
			responsible_nullList=(int*)malloc(NbElement*sizeof(int));

			if(hwname_nullList!=NULL && serialnb_nullList!=NULL && location_nullList!=NULL && hwtype_nullList!=NULL && comments_nullList!=NULL && responsible_nullList!=NULL 
				&& dhwname!=NULL && dserialnb!=NULL && dlocation!=NULL && dhwtype!=NULL && dcomments!=NULL && dresponsible!=NULL)
			for(i=0;i<NbElement;i++)
			{
				memcpy(dhwname+i*max_hwnamelen,deviceList[i]->name,deviceList[i]->namelen);
				hwname_nullList[i]=deviceList[i]->name_null;
				memcpy(dserialnb+i*max_serialnblen,deviceList[i]->serialnb,deviceList[i]->serialnblen);
				serialnb_nullList[i]=deviceList[i]->serialnb_null;
				memcpy(dlocation+i*max_locationlen,deviceList[i]->location,deviceList[i]->locationlen);
				location_nullList[i]=deviceList[i]->location_null;
				memcpy(dhwtype+i*max_hwtypelen,deviceList[i]->hwtype,deviceList[i]->hwtypelen);					
				hwtype_nullList[i]=deviceList[i]->hwtype_null;
				memcpy(dcomments+i*max_commentslen,deviceList[i]->comments,deviceList[i]->commentslen);					
				comments_nullList[i]=deviceList[i]->comments_null;
				memcpy(dresponsible+i*max_responsiblelen,deviceList[i]->responsible,deviceList[i]->responsiblelen);					
				responsible_nullList[i]=deviceList[i]->responsible_null;
			}
	
			numrows_inserted=(int*)malloc(sizeof(int)*NbElement);

			if( (dhwname==NULL) || (dserialnb==NULL) || dlocation==NULL || (dresponsible==NULL)||dcomments==NULL||dhwtype==NULL||numrows_inserted==NULL 
				||hwname_nullList==NULL || serialnb_nullList==NULL || location_nullList==NULL || hwtype_nullList==NULL || comments_nullList==NULL || responsible_nullList==NULL)
			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				
				FIRST_TIME=0;
				
				if(dhwname!=NULL)
					free(dhwname);
				if(dlocation!=NULL)
					free(dlocation);
				if(dserialnb!=NULL)
					free(dserialnb);
				if(dresponsible!=NULL)
					free(dresponsible);
				if(dcomments!=NULL)
					free(dcomments);
				if(dhwtype!=NULL)
					free(dhwtype);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(hwname_nullList!=NULL)
					free(hwname_nullList);
				if(serialnb_nullList!=NULL)
					free(serialnb_nullList);
				if(location_nullList!=NULL)
					free(location_nullList);
				if(hwtype_nullList!=NULL)
					free(hwtype_nullList);
				if(comments_nullList!=NULL)
					free(comments_nullList);
				if(responsible_nullList!=NULL)
					free(responsible_nullList);
				
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
				sprintf(sqlstmt,"BEGIN insert into %s(serialnb,hwtype,hwname,responsible,user_comments,location,device_status) values(:serialnb,:hwtype,:hwname,:responsible,:comments,:location,%d);insert into %s(historydeviceID,serialnb,device_status,user_comment,location,status_change,created,author,terminal_name) values(%s,:serialnb,'SPARE',:comments,:location,sysdate,sysdate,'%s','%s'); :numrows:=%s; END;",HW_DEVICE_TABLE,SPARE,HISTORY_DEVICE_TABLE,seqname,login,host,SQLROWCOUNT);
				StmtPrepare(hstmt,ociError, sqlstmt, &status);
				BindByName(hstmt,&bndp[0],ociError,":hwname",dhwname, max_hwnamelen,SQLT_STR,&hwname_nullList[0],&status);
				BindByName(hstmt,&bndp[1],ociError,":serialnb",dserialnb,max_serialnblen,SQLT_STR,&serialnb_nullList[0],&status);
				BindByName(hstmt,&bndp[2],ociError,":location",dlocation,max_locationlen,SQLT_STR,&location_nullList[0],&status);
				BindByName(hstmt,&bndp[3],ociError,":hwtype",dhwtype,max_hwtypelen,SQLT_STR,&hwtype_nullList[0],&status);
				BindByName(hstmt,&bndp[4],ociError,":responsible",dresponsible,max_responsiblelen,SQLT_STR,&responsible_nullList[0],&status);
				BindByName(hstmt,&bndp[5],ociError,":comments",dcomments,max_commentslen,SQLT_STR,&comments_nullList[0],&status);
				BindByName(hstmt,&bndp[6],ociError,":numrows",&numrows_inserted[0],sizeof(int),SQLT_INT,0,&status);

				BindArrayOfStruct(bndp[0],ociError,max_hwnamelen,sizeof(int),&status);
				BindArrayOfStruct(bndp[1],ociError,max_serialnblen,sizeof(int),&status);
				BindArrayOfStruct(bndp[2],ociError,max_locationlen,sizeof(int),&status);
				BindArrayOfStruct(bndp[3],ociError,max_hwtypelen,sizeof(int),&status);
				BindArrayOfStruct(bndp[4],ociError,max_responsiblelen,sizeof(int),&status);
				BindArrayOfStruct(bndp[5],ociError,max_commentslen,sizeof(int),&status);
				BindArrayOfStruct(bndp[6],ociError,sizeof(int),0,&status);
				StmtExecute(ociHdbc, hstmt, ociError,NbElement, &status);

			}catch(Error err){
				sprintf(appliName,"InsertMultipleSpareDevices");	///////
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

				if(dhwname!=NULL)
					free(dhwname);
				if(dlocation!=NULL)
					free(dlocation);
				if(dserialnb!=NULL)
					free(dserialnb);
				if(dresponsible!=NULL)
					free(dresponsible);
				if(dcomments!=NULL)
					free(dcomments);
				if(dhwtype!=NULL)
					free(dhwtype);
				if(numrows_inserted!=NULL)
					free(numrows_inserted);
				if(hwname_nullList!=NULL)
					free(hwname_nullList);
				if(serialnb_nullList!=NULL)
					free(serialnb_nullList);
				if(location_nullList!=NULL)
					free(location_nullList);
				if(hwtype_nullList!=NULL)
					free(hwtype_nullList);
				if(comments_nullList!=NULL)
					free(comments_nullList);
				if(responsible_nullList!=NULL)
					free(responsible_nullList);
			
				for(i=0;i<NbElement;i++)
					delete deviceList[i];
				if(force_insert==1)
				{
					FIRST_TIME=1;
					force_insert=0;
					deviceList=(Device**)realloc(deviceList, 0*sizeof(Device));
				}
				else if(deviceList!=NULL)
				    free(deviceList);

				NbElement=0;
				GetErrorMess(appliName, "COULDNOT_INSERT_ALL_ROWS",ErrMess,1);
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
				deviceList=(Device**)realloc(deviceList, 0*sizeof(Device));
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
	
		if(dhwname!=NULL)
			free(dhwname);
		if(dlocation!=NULL)
			free(dlocation);
		if(dserialnb!=NULL)
			free(dserialnb);
		if(dresponsible!=NULL)
			free(dresponsible);
		if(dcomments!=NULL)
			free(dcomments);
		if(dhwtype!=NULL)
			free(dhwtype);
		if(numrows_inserted!=NULL)
			free(numrows_inserted);
		if(hwname_nullList!=NULL)
			free(hwname_nullList);
		if(serialnb_nullList!=NULL)
			free(serialnb_nullList);
		if(location_nullList!=NULL)
			free(location_nullList);
		if(hwtype_nullList!=NULL)
			free(hwtype_nullList);
		if(comments_nullList!=NULL)
			free(comments_nullList);
		if(responsible_nullList!=NULL)
			free(responsible_nullList);
		
		return (status+rescode);
	}
	

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif