/********************************************************************************/
// Author: L.Abadie
// version:v3.5
// changes: handling error such that can be extended to C++: (create a class SQLException) (strlen ck + update stmt)
// file which performs select statements
// N.B : Memory management (allocation+release) should be handled by the user application thus, for all the fct provided by this lib
/********************************************************************************/
#include "list_structures.h"
#include "system_info.h"
#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif


#if defined( _WIN32) || defined(WIN32)
#define EXTERN_CONFDB  _declspec(dllexport)
#else
#include <stdlib.h> 
#define EXTERN_CONFDB 
#endif

	extern   OCIError* ociError ;
	extern   OCIEnv* ociEnv ;
	extern   OCISvcCtx* ociHdbc ;
	extern   OCIServer *mysrvhp;
	extern   OCISession *myusrhp; 

	/************************************************************************/
	// return the status of a functional device
	/***********************************************************************/
	/**
	* return the functional status of a device returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param devicename : name of the functional device
	* @param device_status : status of the functional device  : IN_USE, NONE (means that there are no hw )
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/

	/******************************************************************/
	EXTERN_CONFDB
		int GetFunctionalDeviceStatus(char* devicename,char* device_status ,char* ErrMess)
	{
		char appliName[100]="GetFunctionalDeviceStatus";
		int i = 0;
		int j=0;
		int pos1=0;
		int pos2=0;
		int pos3=0;
		int pos4=0;
		int free_mem=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		sword status;
		int rescode=0;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def[1];



		int devstatus=0;

		int len=0;
		char selectdev[1000];
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		if(devicename==NULL)
		{
			GetErrorMess(appliName," device name MUST be given",ErrMess,1);
			return -1;
		}
		if(status!=OCI_SUCCESS)
		{
			rescode=ShowErrors (status, ociError, "OCIStmtHAndleStmt unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrMess,1);
			return -1;
		}
		else
		{
			sprintf(selectdev,"select decode(nvl(t.serialnb,'none'),'none',%d,g.device_status) from %s t, %s g where t.devicename=:dname and (t.serialnb=g.serialnb or t.serialnb is null)",NOT_AVAILABLE,LOGICAL_DEVICE_TABLE,HW_DEVICE_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdev,(ub4) strlen(selectdev),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else	
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":dname",-1,(dvoid*) devicename,strlen(devicename)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByPos unsuccessful");
		}
		else	
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "Execute unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[0], ociError, 1,&devstatus,sizeof(devstatus), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos1 unsuccessful");

			}
		}
		else
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);

		if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
		{	

			rescode=GetDeviceStatus(devstatus, device_status);
		} 
		else 
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "Fetch unsuccessful");
				devstatus=0; //means no data found
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrMess,1);
				return devstatus;
			}
		}
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(rescode==0)
		{
			OCIReportError(ociError,appliName,ErrMess,0); 
		}
		else
		{
			OCIReportError(ociError,appliName,ErrMess,1); 
		}
		rescode+=status;
		return rescode;
	}

	/************************************************************************/
	// return the status of a hw device (serialnb)
	/***********************************************************************/

	/**
	* return the status of a hw device (serialnb) returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param serialnb : serialnb of the hw device
	* @param device_status : status of the hw device  : IN_USE,IN_REPAIR (not replaced yet or not repaired yet),DESTROYED (not replaced yet),TEST (means being test ),EXT_TEST
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/

	/******************************************************************/
	EXTERN_CONFDB
		int GetHWDeviceStatus(char* serialnb,char* device_status ,char* ErrMess)
	{
		char appliName[100]="GetHWDeviceStatus";
		int i = 0;
		int j=0;
		int pos1=0;
		int pos2=0;
		int pos3=0;
		int pos4=0;
		int free_mem=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		sword status;
		int rescode=0;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def[1];



		int devstatus=0;

		int len=0;
		char selectdev[1000];
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		if(serialnb==NULL)
		{
			GetErrorMess(appliName," device name MUST be given",ErrMess,1);
			return -1;
		}
		if(status!=OCI_SUCCESS)
		{
			rescode=ShowErrors (status, ociError, "OCIStmtHAndleStmt unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrMess,1);
			return -1;
		}
		else
		{
			sprintf(selectdev,"select device_status from %s  where serialnb=:snb ",HW_DEVICE_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdev,(ub4) strlen(selectdev),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else	
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":snb",-1,(dvoid*) serialnb,strlen(serialnb)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByPos unsuccessful");
		}
		else	
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "Execute unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[0], ociError, 1,&devstatus,sizeof(devstatus), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos1 unsuccessful");

			}
		}
		else
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);

		if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
		{	

			rescode=GetDeviceStatus(devstatus, device_status);
		} 
		else 
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "Fetch unsuccessful");
				devstatus=0; //means no data found
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrMess,1);
				return devstatus;
			}
		}
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(rescode==0)
		{
			OCIReportError(ociError,appliName,ErrMess,0); 
		}
		else
		{
			OCIReportError(ociError,appliName,ErrMess,1); 
		}
		rescode+=status;
		return rescode;
	}
	/************************************************************************/
	// return the row of the given type in devtype_result
	/***********************************************************************/

	/**
	* replace the board which  occupied the functional device. (initial state of the board:IN_USE) returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param devicename : name of the functional device
	* @param new_device_status : new status of the board which was occupying this fct : IN_REPAIR,DESTROYED,SPARE, EXT_TEST
	* @param new_location : new location of the board
	* @param user_comments : comments to explain why this replacement. Put "none", if you the default one :"status change in functional device with name %s " or 
	* @param status_datechange : date of the status change of the board formatted as functional
	* @param serialnb_replaced : serialnb which replaces this devices (must be already declared as SPARE, otherwise won't work). if none, put "none".
	* @param replace_date : date of the replacement formatted as functional
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/

	/******************************************************************/
	EXTERN_CONFDB
		int ReplaceFunctionalDevice(char* devicename,char* new_device_status,char* new_location,char* user_comments,char* status_datechange,char* serialnb_replaced,char* replace_date,char* ErrMess)
	{
		int status_case=0;
		int rescode=0;
		char appliName[100]="ReplaceFunctionalDevice";
		char seqname[100]="lhcb_history_deviceseq.nextval";
		OCIBind  *bndp[9]; 
		int locationlen=0;
		int devicelen=0;
		int numrows=0;
		OCIBind  *bndp3=0; 
		OCIBind  *bndp4=0; 
		int sysID=0;
		OCIStmt *stmthp;
		int res_code=0;
		int res_query;
		int deviceid;
		int len_host=LOGIN_LEN;
		char login[LOGIN_LEN];
		char host[LOGIN_LEN];
		//char seq_name[100]="lhcb_lg_deviceseq.nextval";
		char selectdev[2500];
		char* errmessg1=(char*)malloc(ERROR_BUFFER*sizeof(char));
		sword status;
		char usercomments[200]="bla";
		if(user_comments==NULL || strncmp(user_comments,"none",4)==0)
		{
			if(serialnb_replaced==NULL||strncmp(serialnb_replaced,"none",4)==0)
				sprintf(usercomments,"status change in functional device with name %s ",devicename);
			else
				sprintf(usercomments,"replacement of the functional device with name %s",devicename);
		}


		//if(serialnb_replaced==NULL||strncmp(serialnb_replaced,"none",4)==0)

		if(errmessg1==NULL)
		{
			GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
			return -1;
		}
		char* errmessg2=(char*)malloc(ERROR_BUFFER*sizeof(char));
		if(errmessg2==NULL)
		{
			GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
			free(errmessg1);
			return -1;
		}
		if(strcmp(new_device_status,"DESTROYED")==0)
			status_case=DESTROYED;
		if(strcmp(new_device_status,"IN_REPAIR")==0)
			status_case=IN_REPAIR;
		if(strcmp(new_device_status,"SPARE")==0)
			status_case=SPARE;
		if(strcmp(new_device_status,"EXT_TEST")==0)
			status_case=EXT_TEST;


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
		//need to proceed with messages
		if(status_case==0)
		{
			GetErrorMess(appliName,"INVALID STATUS",ErrMess,1);
			rescode=-1;
			return rescode;
		}
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);

		if(status!=OCI_SUCCESS)
		{
			rescode=ShowErrors (status, ociError, "OCIStmtHAndleStmt unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrMess,1);

			return -1;
		}
		if(serialnb_replaced==NULL||strncmp(serialnb_replaced,"none",4)==0)
			sprintf(selectdev,"BEGIN update %s set device_status=:dstatus,location=:dlocation where device_status=%d and serialnb=(select serialnb from %s where devicename=:dname); if %s=1 then insert ALL into %s(historydeviceid,serialnb,device_status,user_comment,location,status_change,system_name,created,author,terminal_name) values (%s,snb1,'%s',:usercomment,:dlocation,to_date(:statchange,'%s'),sysid,sysdate,'%s','%s') select serialnb as snb1,system_name as sysid from %s where devicename=:dname; update %s set nodeused=0,last_update=sysdate,user_update='%s',serialnb=NULL where devicename=:dname returning deviceid, system_name into :devid,:sysID;insert ALL into %s(historydeviceid,deviceid,device_status,user_comment,location,status_change,system_name,created,author,terminal_name) values (%s,devid,'NOT_AVAILABLE',:usercomment,location,to_date(:statchange,'%s'),sysid,sysdate,'%s','%s') select deviceid as devid,location,system_name as sysid from %s where devicename=:dname;:numrows:=%s; :rescode:=%s(:devid,:dlocation,:dstatus,:statchange,'%s', :usercomment,'%s');else :rescode:=-10; :numrows:=0; end if;END;",HW_DEVICE_TABLE,IN_USE,LOGICAL_DEVICE_TABLE,SQLROWCOUNT,HISTORY_DEVICE_TABLE,seqname,new_device_status,_date_format,login,host,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,login,HISTORY_DEVICE_TABLE,seqname,_date_format,login,host,LOGICAL_DEVICE_TABLE,SQLROWCOUNT,_UpdateBoardCpnt,login,host);
		else
		{
			sprintf(selectdev,"BEGIN update %s set device_status=:dstatus,location=:dlocation where device_status=%d and serialnb=(select serialnb from %s where devicename=:dname); if %s=1 then insert ALL into %s(historydeviceid, serialnb,  device_status, user_comment, location, status_change, system_name,created,author,terminal_name) values (%s,snb1,'%s',:usercomment,:dlocation,to_date(:statchange,'%s'), sysid,sysdate,'%s','%s') select serialnb as snb1,system_name as sysid from %s where devicename=:dname;if to_date(:statchange,'%s')<to_date(:statchange2,'%s') then insert ALL into %s(historydeviceid, deviceid,  device_status, user_comment, location, status_change, system_name,created,author,terminal_name) values (%s,devid1,'NONE',:usercomment,location,to_date(:statchange,'%s'), sysid,sysdate,'%s','%s') select deviceid as devid1,system_name as sysid,location from %s where devicename=:dname; end if; if to_date(:statchange2,'%s')<to_date(:statchange,'%s') then :rescode:=-15; :numrows:=0; else update %s set serialnb=:snb,last_update=sysdate,user_update='%s' where devicename=:dname and %d=(select device_status from %s where serialnb=:snb and device_status=%d) returning deviceid into :devid; if %s=1 then insert all into %s(historydeviceid,serialnb,deviceid,device_status,user_comment,location,status_change,system_name,created,author,terminal_name) values (%s,snb,devid1,'IN_USE',:usercomment,locat,to_date(:statchange2,'%s'),sysid,sysdate,'%s','%s') select serialnb as snb,deviceid as devid1,location as locat,system_name as sysid from %s where devicename=:dname;update %s set device_status=%d where serialnb=:snb; :numrows:=%s; :rescode:=%s(:devid,:dlocation,:dstatus,:statchange,'%s',:usercomment,'%s',:snb,:statchange2);  else :rescode:=-5; :numrows:=0; end if;  end if; else :rescode:=-10; :numrows:=0;end if;END;",HW_DEVICE_TABLE,IN_USE,LOGICAL_DEVICE_TABLE,SQLROWCOUNT,HISTORY_DEVICE_TABLE,seqname,new_device_status,_date_format,login,host,LOGICAL_DEVICE_TABLE,_date_format,_date_format,HISTORY_DEVICE_TABLE,seqname,_date_format,login,host,LOGICAL_DEVICE_TABLE,_date_format,_date_format,LOGICAL_DEVICE_TABLE,login,SPARE,HW_DEVICE_TABLE,SPARE,SQLROWCOUNT,HISTORY_DEVICE_TABLE,seqname,_date_format,login,host,LOGICAL_DEVICE_TABLE,HW_DEVICE_TABLE,IN_USE,SQLROWCOUNT,_UpdateBoardCpnt,login,host);			
		}	

		//std::cout<<"selectdev="<<selectdev<<std::endl;
		status=OCIStmtPrepare(stmthp, ociError, (text*)selectdev, (ub4)strlen((char *)selectdev), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else	
			status=OCIBindByName(stmthp, &bndp[0], ociError,(text*)":dstatus",-1,(dvoid*) &status_case,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBind 1 unsuccessful");
		}
		else
		{
			if(new_location!=NULL)
				status=OCIBindByName(stmthp, &bndp[1], ociError,(text*)":dlocation",-1,(dvoid*) new_location,strlen(new_location)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
			else
				status=OCIBindByName(stmthp, &bndp[1], ociError,(text*)":dlocation",-1,(dvoid*) new_location,1, SQLT_STR, (dvoid *) -1,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBind 2 unsuccessful");
		}
		else
		{
			if(devicename!=NULL)
				status=OCIBindByName(stmthp, &bndp[2], ociError,(text*)":dname",-1,(dvoid*) devicename,strlen(devicename)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
			else
			{
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"DEVICE NAME MUST BE GIVEN",ErrMess,1);
				return -1;
			}
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBind 3 unsuccessful");
		}
		else
		{
			if(strncmp(usercomments,"bla",3)==0)
				status=OCIBindByName(stmthp, &bndp[3], ociError,(text*)":usercomment",-1,(dvoid*) user_comments,strlen(user_comments)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
			else
				status=OCIBindByName(stmthp, &bndp[3], ociError,(text*)":usercomment",-1,(dvoid*) usercomments,strlen(usercomments)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBind 4 unsuccessful");
		}
		else
		{
			//std::cout<<"date ="<<status_datechange<<std::endl;
			if(status_datechange!=NULL) 
				status=OCIBindByName(stmthp, &bndp[4], ociError,(text*)":statchange",-1,(dvoid*) status_datechange,strlen(status_datechange)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
			else
			{
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"DATE OF CHANGE STATUS MUST BE GIVEN",ErrMess,1);
				return -1;
			}
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBind 5 unsuccessful");
		}
		else	
			status=OCIBindByName(stmthp, &bndp[5], ociError,(text*)":numrows",-1,(dvoid*) &numrows,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBind 5 unsuccessful");
		}
		else	
			status=OCIBindByName(stmthp, &bndp[6], ociError,(text*)":devid",-1,(dvoid*) &deviceid,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBind 5 unsuccessful");
		}
		else	
			status=OCIBindByName(stmthp, &bndp3, ociError,(text*)":rescode",-1,(dvoid*) &res_code,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		//std::cout<<"after binding 6"<<serialnb_replaced<<std::endl;
		if(serialnb_replaced!=NULL && strncmp(serialnb_replaced,"none",4)!=0)
		{
			//std::cout<<"date of repalce ="<<replace_date<<std::endl;
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIBind 6 unsuccessful");
			}
			else
			{
				if(replace_date!=NULL)
					status=OCIBindByName(stmthp, &bndp[7], ociError,(text*)":statchange2",-1,(dvoid*) replace_date,strlen(replace_date)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
				else
				{
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName,"DATE OF REPLACEMENT MUST BE GIVEN",ErrMess,1);
					return -1;
				}
			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIBind 7 unsuccessful");
			}
			else
			{
				status=OCIBindByName(stmthp, &bndp[8], ociError,(text*)":snb",-1,(dvoid*) serialnb_replaced,strlen(serialnb_replaced)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

			}
		}
		else
		{
			status=OCIBindByName(stmthp, &bndp4, ociError,(text*)":sysID",-1,(dvoid*) &sysID,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBind 8 unsuccessful");
		}
		else
			status= OCIStmtExecute(ociHdbc, stmthp, ociError,  1, 0, 0, 0, OCI_DEFAULT );
		//std::cout<<"res_code="<<res_code<<" numrows "<<numrows<<std::endl;
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIExecute unsuccessful");
		}
		else
		{
			if (res_code!=0)
			{
				status = OCITransRollback(ociHdbc, ociError, 0);
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				if(res_code==-10)
					GetErrorMess(appliName,"Error The given log. device name is not IN_USE",ErrMess,1);
				else
				{
					if (res_code==-5)
					{
						GetErrorMess(appliName,"Error The given serial nb is not a SPARE",ErrMess,1);
						//std::cout<<"value of rescode="<<numrows<<std::endl;
					}
					else
					{
						if(res_code==-15)
							GetErrorMess(appliName,"Error date of replacement must be after date of status change",ErrMess,1);
						else
							GetErrorMess(appliName,"Error in the transaction: no changes have been done",ErrMess,1);
					}


				}
				return -1;
			}
			else
			{
				status = OCITransCommit(ociHdbc, ociError, 0);
				if(serialnb_replaced==NULL||strncmp(serialnb_replaced,"none",4)==0)
				{
					// need to update the link state if there is no replacement and also pathes--> linkused=0 and pathused=0
					sprintf(selectdev,"begin update %s e set e.last_update=sysdate,e.user_update='%s',e.lkused=(select t.nodeused*f.nodeused from %s t,%s f, %s l,%s m where t.deviceid=l.deviceid and l.portid=e.portidfrom and f.deviceid=m.deviceid and m.portid=e.portidto) where (e.portidto in (select p.portid from %s p where p.deviceid=%d)) or (e.portidfrom in (select p.portid from %s p where p.deviceid=%d)); if %s> 0 then :rescode:=%s(%d); end if; end; ",MACRO_CONNECTIVITY_TABLE,login,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,PORT_TABLE,PORT_TABLE,PORT_TABLE,deviceid,PORT_TABLE,deviceid,SQLROWCOUNT,_UpdatePathUsed,sysID);
					status=OCIStmtPrepare(stmthp, ociError, (text*)selectdev, (ub4)strlen((char *)selectdev), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
					if(status!=OCI_SUCCESS)
					{
						if(rescode==0)
							rescode=ShowErrors (status, ociError, "OCIPrepare 2 unsuccessful");
					}
					else	
						status=OCIBindByName(stmthp, &bndp3, ociError,(text*)":rescode",-1,(dvoid*) &res_code,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

					if(status!=OCI_SUCCESS)
					{
						if(rescode==0)
							rescode=ShowErrors (status, ociError, "OCIStmtBind 2 unsuccessful");
					}
					else
						status= OCIStmtExecute(ociHdbc, stmthp, ociError,  1, 0, 0, 0, OCI_DEFAULT );

					if(status!=OCI_SUCCESS)
					{
						if(rescode==0)	
							rescode=ShowErrors (status, ociError, "OCIExecute2 unsuccessful");
					}
					if (res_code!=0 )
					{
						GetErrorMess(appliName,"Error couldn't update links and paths status",ErrMess,1);
						status = OCITransCommit(ociHdbc, ociError, 0);
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						return -1;
					}
					else
						status = OCITransCommit(ociHdbc, ociError, 0);
				}
			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCITransCommit unsuccessful");
			}
		}
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 
		else
			OCIReportError(ociError,appliName, ErrMess,0); 

		return (status+rescode);

	}

	/************************************************************************/
	// return the row of the given type in devtype_result
	/***********************************************************************/

	/**
	* replace the board which  occupied the functional device. (initial state of the board:IN_USE) returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param devicename : name of the functional device
	* @param user_comments : comments to explain why this replacement
	* @param status_change : date of the status change of the board formatted as YY/MM/DD/HH24/MM/SS
	* @param replace_date : date of the replacement  formatted as YY/MM/DD/HH24/MM/SS
	* @param testboard_name : name of the test board used for testing
	* @param serialnb_replaced : serialnb which replaces this devices (must be already declared as SPARE). if none, put "none".
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/

	/******************************************************************/
	EXTERN_CONFDB
		int SetToTestUseStatus(char* devicename,char* user_comments,char* status_change,char* serialnb_replaced,char* testboard_name,char* replace_date,char* ErrMess)
	{
		int snb1_len=1001;
		int snb2_len=snb1_len;
		int loc1_len=snb1_len;
		int loc2_len=snb1_len;
		char appliName[100]="SetToTestUseStatus";
		int rescode=0;
		OCIBind  *bndp3=0; 
		char * serialnb1=(char*)malloc(snb1_len*sizeof(char));
		if(serialnb1==NULL)
		{
			GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
			return -1;
		}
		char * serialnb2=(char*)malloc(snb2_len*sizeof(char));
		if(serialnb2==NULL)
		{
			GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
			free(serialnb1);
			return -1;
		}
		char * location2=(char*)malloc(loc2_len*sizeof(char));
		if(location2==NULL)
		{
			GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
			free(serialnb1);
			free(serialnb2);
			return -1;
		}
		char* location1=(char*)malloc(loc1_len*sizeof(char));
		if(location1==NULL)
		{
			GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
			free(serialnb1);
			free(serialnb2);
			free(location2);
			return -1;
		}
		strcpy(serialnb1,"blabla");
		strcpy(serialnb2,"blabla");
		strcpy(location2,"blabla");
		strcpy(location1,"blabla");

		int devid1=0;
		int devid2=0;
		int sysID=0;
		char seqname[100]="lhcb_history_deviceseq.nextval";
		OCIBind  *bndp[14]; 
		int numrows=0;
		OCIStmt *stmthp;
		int len_host=LOGIN_LEN;
		char login[LOGIN_LEN];
		char host[LOGIN_LEN];
		int res_query=0;

		char selectdev[3000];
		sword status;
		char usercomments[200]="bla";
		if(user_comments==NULL || strncmp(user_comments,"none",4)==0)
		{
			if(serialnb_replaced==NULL||strncmp(serialnb_replaced,"none",4)==0)
				sprintf(usercomments,"status change in functional device with name %s ",devicename);
			else
				sprintf(usercomments,"replacement of the functional device with name %s",devicename);


		}
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);

		if(status!=OCI_SUCCESS)
		{
			rescode=ShowErrors (status, ociError, "OCIStmtHAndleStmt unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrMess,1);
			free(serialnb1);
			free(serialnb2);
			free(location2);
			free(location1);
			return -1;
		}

		//if(serialnb_replaced==NULL||strncmp(serialnb_replaced,"none",4)==0)
		char* errmessg1=(char*)malloc(ERROR_BUFFER*sizeof(char));
		if(errmessg1==NULL)
		{
			GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
			free(serialnb1);
			free(serialnb2);
			free(location2);
			free(location1);
			return -1;
		}
		char* errmessg2=(char*)malloc(ERROR_BUFFER*sizeof(char));
		if(errmessg2==NULL)
		{
			GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
			free(errmessg1);
			free(serialnb1);
			free(serialnb2);
			free(location2);
			free(location1);
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

		if(testboard_name==NULL || strncmp(testboard_name,"none",4)==0)
		{
			GetErrorMess(appliName,"TEST BOARD NAME  MUST BE GIVEN",ErrMess,1);
			free(serialnb1);
			free(serialnb2);
			free(location2);
			free(location1);
			return -1;
		}
		if(devicename==NULL || strncmp(devicename,"none",4)==0)
		{
			GetErrorMess(appliName,"DEVICE NAME  MUST BE GIVEN",ErrMess,1);
			free(serialnb1);
			free(serialnb2);
			free(location2);
			free(location1);
			return -1;
		}
		if(status_change==NULL || strncmp(status_change,"none",4)==0)
		{
			GetErrorMess(appliName,"date of status_change  MUST BE GIVEN",ErrMess,1);
			free(serialnb1);
			free(serialnb2);
			free(location2);
			free(location1);
			return -1;
		}
		//std::cout<<"value of selectdev before"<<std::endl;	
		if(serialnb_replaced==NULL||strncmp(serialnb_replaced,"none",4)==0)
			sprintf(selectdev,"BEGIN update %s set device_status=%d where device_status=%d and serialnb=(select serialnb from %s where devicename=:dname) returning serialnb into :snb; if %s=1 then update %s set serialnb=:snb,last_update=sysdate,user_update='%s' where devicename=:bname and serialnb is null returning deviceid,location into :devid,:location; if %s=1 then insert into %s(historydeviceid,serialnb,deviceid,device_status,user_comment,location,status_change,created,author,terminal_name) values (%s,:snb,:devid,'TEST',:usercomment,:location,to_date(:statchange,'%s'),sysdate,'%s','%s'); update %s set nodeused=0,serialnb=NULL,last_update=sysdate,user_update='%s' where devicename=:dname returning deviceid,system_name,location into :dID,:sysID,:loc; insert into %s(historydeviceid,serialnb,deviceid,device_status,user_comment,location,status_change,system_name,created,author,terminal_name) values (%s,NULL,:dID,'NONE',:usercomment,:loc,to_date(:statchange,'%s'),:sysID,sysdate,'%s','%s'); :numrows:=%s; :rescode:=%s(:dID,:devid,:statchange,'%s', :usercomment,'%s'); else :rescode:=-5; :numrows:=0; end if; else :rescode:=-10; :numrows:=0; end if;END;",HW_DEVICE_TABLE,TEST,IN_USE,LOGICAL_DEVICE_TABLE,SQLROWCOUNT,LOGICAL_DEVICE_TABLE,login,SQLROWCOUNT,HISTORY_DEVICE_TABLE,seqname,_date_format,login,host,LOGICAL_DEVICE_TABLE,login,HISTORY_DEVICE_TABLE,seqname,_date_format,login,host,SQLROWCOUNT,_TestUseBoardCpnt,login,host);
		else		
			sprintf(selectdev,"BEGIN update %s set device_status=%d where device_status=%d and serialnb=(select serialnb from %s where devicename=:dname) returning serialnb into :snb;if %s=1 then update %s set serialnb=:snb,last_update=sysdate,user_update='%s' where devicename=:bname and serialnb is null returning deviceid,location into :devid,:location; if %s=1 then insert  into %s(historydeviceid,serialnb,deviceid,device_status,user_comment,location,status_change,created,author,terminal_name) values (%s,:snb,:devid,'TEST',:usercomment,:location,to_date(:statchange,'%s'),sysdate,'%s','%s'); if to_date(:statchange,'%s')<to_date(:statchange2,'%s') then insert all into %s(historydeviceid,deviceid,device_status,user_comment,location,status_change,system_name,created,author,terminal_name) values (%s,deviceid,'NONE',:usercomment,location,to_date(:statchange,'%s'),system_name,sysdate,'%s','%s') select deviceid,location,system_name from %s where devicename=:dname; end if; if to_date(:statchange2,'%s')<to_date(:statchange,'%s') then :numrows:=0; :rescode:=-15; else update %s set serialnb=:snb2,last_update=sysdate,user_update='%s' where devicename=:dname and %d=(select device_status from %s where device_status=%d and serialnb=:snb2) returning deviceid,system_name,location into :dID,:sysID,:loc;if %s=1 then update %s set device_status=%d where serialnb=:snb2; insert into %s(historydeviceid,serialnb,deviceid,device_status,user_comment,location,status_change,system_name,created,author,terminal_name) values (%s,:snb2,:dID,'IN_USE',:usercomment,:loc,to_date(:statchange2,'%s'),:sysID,sysdate,'%s','%s'); :numrows:=%s; :rescode:=%s(:dID,:devid,:statchange,'%s', :usercomment,'%s',:snb,:statchange2);  else :rescode:=-3; :numrows:=0;  end if; end if; else :rescode:=-5; :numrows:=0; end if; else :rescode:=-10; :numrows:=0; end if;END;",HW_DEVICE_TABLE,TEST,IN_USE,LOGICAL_DEVICE_TABLE,SQLROWCOUNT,LOGICAL_DEVICE_TABLE,login,SQLROWCOUNT,HISTORY_DEVICE_TABLE,seqname,_date_format,login,host,_date_format,_date_format,HISTORY_DEVICE_TABLE,seqname,_date_format,login,host,LOGICAL_DEVICE_TABLE,_date_format,_date_format,LOGICAL_DEVICE_TABLE,login,SPARE,HW_DEVICE_TABLE,SPARE,SQLROWCOUNT,HW_DEVICE_TABLE,IN_USE,HISTORY_DEVICE_TABLE,seqname,_date_format,login,host,SQLROWCOUNT,_TestUseBoardCpnt,login,host);

		//std::cout<<"value of selectdev "<<selectdev <<" and strlen= "<<strlen(selectdev)<<std::endl;
		status=OCIStmtPrepare(stmthp, ociError, (text*)selectdev, (ub4)strlen((char *)selectdev), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else	
			status=OCIBindByName(stmthp, &bndp[0], ociError,(text*)":snb",-1,(dvoid*) serialnb1,snb1_len+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBind1 unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bndp[1], ociError,(text*)":bname",-1,(dvoid*) testboard_name,strlen(testboard_name)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBind2 unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bndp[2], ociError,(text*)":dname",-1,(dvoid*) devicename,strlen(devicename)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBind3 unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bndp[3], ociError,(text*)":devid",-1,(dvoid*) &devid1,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBind4 unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bndp[4], ociError,(text*)":dID",-1,(dvoid*) &devid2,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBind5 unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bndp[5], ociError,(text*)":location",-1,(dvoid*) location1,loc1_len, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBind6 unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bndp[6], ociError,(text*)":sysID",-1,(dvoid*) &sysID,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBind7 unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bndp[7], ociError,(text*)":loc",-1,(dvoid*) location2,loc2_len, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBind8 unsuccessful");
		}
		else
		{
			if(strncmp(usercomments,"bla",3)==0)
				status=OCIBindByName(stmthp, &bndp[8], ociError,(text*)":usercomment",-1,(dvoid*) user_comments,strlen(user_comments)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
			else
				status=OCIBindByName(stmthp, &bndp[8], ociError,(text*)":usercomment",-1,(dvoid*) usercomments,strlen(usercomments)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBind9 unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bndp[9], ociError,(text*)":statchange",-1,(dvoid*) status_change,strlen(status_change)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBind10 unsuccessful");
		}
		else	
			status=OCIBindByName(stmthp, &bndp[10], ociError,(text*)":numrows",-1,(dvoid*) &numrows,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBind11 unsuccessful");
		}
		else	
			status=OCIBindByName(stmthp, &bndp3, ociError,(text*)":rescode",-1,(dvoid*) &res_query,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(serialnb_replaced!=NULL && strncmp(serialnb_replaced,"none",4)!=0)
		{
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIBind12 unsuccessful");
			}
			else
			{
				if(replace_date!=NULL)
					status=OCIBindByName(stmthp, &bndp[11], ociError,(text*)":statchange2",-1,(dvoid*) replace_date,strlen(replace_date)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
				else
				{
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName,"DATE OF REPLACEMENT MUST BE GIVEN",ErrMess,1);
					free(serialnb1);
					free(serialnb2);
					free(location2);
					free(location1);
					return -1;
				}
			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIBind13 unsuccessful");
			}
			else
				status=OCIBindByName(stmthp, &bndp[12], ociError,(text*)":snb2",-1,(dvoid*) serialnb_replaced,strlen(serialnb_replaced)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);		
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBind unsuccessful");
		}
		else
			status= OCIStmtExecute(ociHdbc, stmthp, ociError,  1, 0, 0, 0, OCI_DEFAULT );
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIExecute unsuccessful");
		}
		else
		{
			if(numrows==0)
			{
				status = OCITransRollback(ociHdbc, ociError, 0);
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				if (res_query==-10)
					GetErrorMess(appliName,"Error Log. device name is not IN_USE status",ErrMess,1);
				else
				{
					if(res_query==-5)
						GetErrorMess(appliName,"Error Board name is already IN_USE status",ErrMess,1);
					else
					{
						if(res_query==-3)
							GetErrorMess(appliName,"Error serial nb is not a spare",ErrMess,1);
						else
						{
							if(res_query==-15)
								GetErrorMess(appliName,"Error date of status change must before date of replace",ErrMess,1);
							else
								GetErrorMess(appliName,"Error in the transaction: no changes have been done",ErrMess,1);
						}
					}


				}
				free(serialnb1);
				free(serialnb2);
				free(location2);
				free(location1);
				return -1;
			}
			else
			{
				status = OCITransCommit(ociHdbc, ociError, 0);
				if(serialnb_replaced==NULL||strncmp(serialnb_replaced,"none",4)==0)
				{
					// need to update the link state if there is no replacement and also pathes--> linkused=0 and pathused=0
					sprintf(selectdev,"begin update %s e set e.last_update=sysdate,e.user_update='%s',e.lkused=(select t.nodeused*f.nodeused from %s t,%s f, %s l,%s m where t.deviceid=l.deviceid and l.portid=e.portidfrom and f.deviceid=m.deviceid and m.portid=e.portidto) where (e.portidto in (select p.portid from %s p where p.deviceid=%d)) or (e.portidfrom in (select p.portid from %s p  where p.deviceid=%d)); if %s>0 then :rescode:=%s(%d); end if; end; ",MACRO_CONNECTIVITY_TABLE,login,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,PORT_TABLE,PORT_TABLE,PORT_TABLE,devid2,PORT_TABLE,devid2,SQLROWCOUNT,_UpdatePathUsed,sysID);
					status=OCIStmtPrepare(stmthp, ociError, (text*)selectdev, (ub4)strlen((char *)selectdev), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
					//std::cout<<"value of selectdev "<<selectdev <<" and strlen= "<<strlen(selectdev)<<std::endl;
					if(status!=OCI_SUCCESS)
					{
						if(rescode==0)
							rescode=ShowErrors (status, ociError, "OCIPrepare 2 unsuccessful");
					}
					else	
						status=OCIBindByName(stmthp, &bndp3, ociError,(text*)":rescode",-1,(dvoid*) &res_query,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

					if(status!=OCI_SUCCESS)
					{
						if(rescode==0)
							rescode=ShowErrors (status, ociError, "OCIStmtBind 2 unsuccessful");
					}
					else
						status= OCIStmtExecute(ociHdbc, stmthp, ociError,  1, 0, 0, 0, OCI_DEFAULT );

					if(status!=OCI_SUCCESS)
					{
						if(rescode==0)	
							rescode=ShowErrors (status, ociError, "OCIExecute2 unsuccessful");
					}
					if (res_query!=0 )
					{
						GetErrorMess(appliName,"Error couldn't update links and paths status",ErrMess,1);
						status = OCITransCommit(ociHdbc, ociError, 0);
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						return -1;
					}
					else
						status = OCITransCommit(ociHdbc, ociError, 0);
				}
			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCITransCommit unsuccessful");
			}
		}
		free(serialnb1);
		free(serialnb2);
		free(location2);
		free(location1);
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 
		else
			OCIReportError(ociError,appliName, ErrMess,0); 
		return (rescode+status);
	}

	//next step is to define the fct any status to IN_USE: need to check that the log. device is not already In_use
	/************************************************************************/
	// return the row of the given type in devtype_result
	/***********************************************************************/

	/**
	* update a hw device status whose initial status is not IN_USE to any status IN_USE,SPARE,DESTROYED,EXT_TEST.returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* update to TEST and EXT_TEST status makes sense only when the initial status of the device is IN_USE...So not in this context
	* @param serialnb  : serialnb of your hw
	* @param new_device_status : new status of the board which was occupying this fct : IN_USE,IN_REPAIR,DESTROYED,SPARE, 
	* @param new_location : new location of the board if new_device_status!='IN_USE' . If new_device_status=IN_USE. Put "none"
	* @param user_comments : comments to explain why this replacement. Put "none", if you the default one :"status change in functional device with name %s " or 
	* @param status_datechange : date of the status change of the board formatted as functional
	* @param functional_devicename : functional_name which will be occupied by the given serialnb. This is only available for new_device_status=IN_USE status and there should be no serial nb which already occupies this functional devicename otherwise use Replace fct.
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/

	/******************************************************************/
	EXTERN_CONFDB
		int UpdateHWDeviceStatus(char* serialnb,char* new_device_status,char* new_location,char* user_comments,char* status_datechange,char* functional_devicename,char* ErrMess)
	{
		int status_case=0;
		int rescode=0;
		char appliName[100]="UpdateHWDeviceStatus";
		char seqname[100]="lhcb_history_deviceseq.nextval";
		char seq_name[100]="LHCB_CPNT_HISTORY_COMPTID_SEQ.nextval";
		OCIBind  *bndp[8]; 
		int locationlen=0;
		int deviceid=0;
		int numrows=0;
		OCIBind  *bndp3=0; 
		OCIBind  *bndp4=0; 
		int sysID=0;
		OCIStmt *stmthp;
		int res_code=0;
		int res_query;
		int len_host=LOGIN_LEN;
		char login[LOGIN_LEN];
		char host[LOGIN_LEN];
		char newlocation[100];
		//char seq_name[100]="lhcb_lg_deviceseq.nextval";
		char selectdev[2500];
		char* errmessg1=(char*)malloc(ERROR_BUFFER*sizeof(char));
		sword status;
		char usercomments[200]="bla";
		if(user_comments==NULL || strncmp(user_comments,"none",4)==0)
		{
			if(functional_devicename==NULL||strncmp(functional_devicename,"none",4)==0)
				sprintf(usercomments,"status change in hw device with name %s ",serialnb);
			else
				sprintf(usercomments,"replacement of the hw  device with name %s",serialnb);
		}
		if(new_location==NULL || strncmp(new_location,"none",4)==0)
		{
			if(strcmp(new_device_status,"IN_USE")==0)
				strcpy(newlocation,"blabla");
			else
				strcpy(newlocation,new_location);;
		}



		if(errmessg1==NULL)
		{
			GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
			return -1;
		}
		char* errmessg2=(char*)malloc(ERROR_BUFFER*sizeof(char));
		if(errmessg2==NULL)
		{
			GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
			free(errmessg1);
			return -1;
		}
		if(strcmp(new_device_status,"DESTROYED")==0)
			status_case=DESTROYED;
		if(strcmp(new_device_status,"IN_REPAIR")==0)
			status_case=IN_REPAIR;
		if(strcmp(new_device_status,"SPARE")==0)
			status_case=SPARE;
		if(strcmp(new_device_status,"IN_USE")==0)
			status_case=IN_USE;


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
		//need to proceed with messages
		if(status_case==0)
		{
			GetErrorMess(appliName,"INVALID STATUS",ErrMess,1);
			rescode=-1;
			return rescode;
		}
		if(status_case==IN_USE && (functional_devicename==NULL ||strncmp(functional_devicename,"none",4)==0))
		{
			GetErrorMess(appliName,"You should provide a functional devicename",ErrMess,1);
			rescode=-1;
			return rescode;
		}
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);

		if(status!=OCI_SUCCESS)
		{
			rescode=ShowErrors (status, ociError, "OCIStmtHAndleStmt unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrMess,1);

			return -1;
		}
		switch (status_case)   
		{
		case IN_USE:
			sprintf(selectdev,"BEGIN update %s set device_status=:dstatus where device_status!=%d and device_status!=%d and serialnb=:snb; if %s=1 then update %s set serialnb=:snb, nodeused=1, last_update=sysdate,user_update='%s' where devicename=:dname and serialnb is null returning deviceid,system_name,location into :devid,:sysname,:dlocation; if %s=1 then insert into %s(historydeviceid, deviceid,serialnb,  device_status, user_comment, location, status_change, system_name,created,author,terminal_name) values (%s,:devid,:snb,'%s',:usercomment,:dlocation,to_date(:statchange,'%s'),:sysname, sysdate,'%s','%s');update %s set  CPNT_STATUS=:dstatus where location=:snb; if %s>0 then update %s t set t.snbid=(select p.snbid from %s p where p.hwname=t.cpntname) where t.motherboardid=:devid;insert all into %s (historycpntid,snbid,cpntid,cpnt_status,user_comment,location,status_change,created,author,terminal_name) values(%s,snbid,cpntid,'%s',:usercomment,:snb,to_date(:statchange,'%s'),sysdate,'%s','%s') select snbid,cpntid from %s where motherboardid=:devid; end if; :numrows:=%s; :rescode:=0; else :rescode:=-5; :numrows:=0;end if; else :rescode:=-10; :numrows:=0; end if; END;",HW_DEVICE_TABLE,IN_USE,DESTROYED, SQLROWCOUNT,LOGICAL_DEVICE_TABLE,login,SQLROWCOUNT,HISTORY_DEVICE_TABLE,seqname,new_device_status,_date_format,login,host,HW_CPNTS_TABLE,SQLROWCOUNT,LG_CPNTS_TABLE,HW_CPNTS_TABLE, HISTORY_CPNT_TABLE,seq_name,new_device_status,_date_format,login,host,LG_CPNTS_TABLE,SQLROWCOUNT);
		case DESTROYED:
			sprintf(selectdev,"BEGIN update %s set device_status=:dstatus,location=:dlocation where device_status!=%d and device_status!=%d and device_status!=%d and serialnb=:snb; if %s=1 then insert into %s(historydeviceid, serialnb,  device_status, user_comment, location, status_change, created,author,terminal_name) values (%s,:snb,'%s',:usercomment,:dlocation,to_date(:statchange,'%s'), sysdate,'%s','%s'); update %s set  CPNT_STATUS=:dstatus where location=:snb;if %s>0 then insert all into %s (historycpntid,snbid,cpnt_status,user_comment,location,status_change,created,author,terminal_name) values(%s,snbid,'%s',:usercomment,:snb,to_date(:statchange,'%s'),sysdate,'%s','%s') select snbid from %s where location=:snb; end if;:numrows:=%s; :rescode:=0; else :rescode:=-10; :numrows:=0;end if;END;",HW_DEVICE_TABLE,IN_USE,DESTROYED,SPARE,SQLROWCOUNT,HISTORY_DEVICE_TABLE,seqname,new_device_status,_date_format,login,host,HW_CPNTS_TABLE,SQLROWCOUNT,HISTORY_CPNT_TABLE,seq_name,new_device_status,_date_format,login,host,HW_CPNTS_TABLE,SQLROWCOUNT);			
		case IN_REPAIR:
			sprintf(selectdev,"BEGIN update %s set device_status=:dstatus,location=:dlocation where device_status!=%d and device_status!=%d and device_status!=%d and device_status!=%d and serialnb=:snb; if %s=1 then insert into %s(historydeviceid, serialnb,  device_status, user_comment, location, status_change, created,author,terminal_name) values (%s,:snb,'%s',:usercomment,:dlocation,to_date(:statchange,'%s'), sysdate,'%s','%s'); update %s set  CPNT_STATUS=:dstatus where location=:snb;if %s>0 then insert all into %s (historycpntid,snbid,cpnt_status,user_comment,location,status_change,created,author,terminal_name) values(%s,snbid,'%s',:usercomment,:snb,to_date(:statchange,'%s'),sysdate,'%s','%s') select snbid from %s where location=:snb; end if;:numrows:=%s; :rescode:=0; else :rescode:=-10; :numrows:=0;end if;END;",HW_DEVICE_TABLE,IN_REPAIR,IN_USE,DESTROYED,SPARE,SQLROWCOUNT,HISTORY_DEVICE_TABLE,seqname,new_device_status,_date_format,login,host,HW_CPNTS_TABLE,SQLROWCOUNT,HISTORY_CPNT_TABLE,seq_name,new_device_status,_date_format,login,host,HW_CPNTS_TABLE,SQLROWCOUNT);			
		case SPARE:
			sprintf(selectdev,"BEGIN update %s set device_status=:dstatus,location=:dlocation where  device_status!=%d and device_status!=%d and device_status!=%d and serialnb=:snb; if %s=1 then insert into %s(historydeviceid, serialnb,  device_status, user_comment, location, status_change, created,author,terminal_name) values (%s,:snb,'%s',:usercomment,:dlocation,to_date(:statchange,'%s'), sysdate,'%s','%s'); update %s set  CPNT_STATUS=:dstatus where location=:snb;if %s>0 then insert all into %s (historycpntid,snbid,cpnt_status,user_comment,location,status_change,created,author,terminal_name) values(%s,snbid,'%s',:usercomment,:snb,to_date(:statchange,'%s'),sysdate,'%s','%s') select snbid from %s where location=:snb; end if;:numrows:=%s; :rescode:=0; else :rescode:=-10; :numrows:=0;end if;END;",HW_DEVICE_TABLE,IN_USE,DESTROYED,SPARE,SQLROWCOUNT,HISTORY_DEVICE_TABLE,seqname,new_device_status,_date_format,login,host,HW_CPNTS_TABLE,SQLROWCOUNT,HISTORY_CPNT_TABLE,seq_name,new_device_status,_date_format,login,host,HW_CPNTS_TABLE,SQLROWCOUNT);			

		}

		if(status_case!=IN_USE)			
			sprintf(selectdev,"BEGIN update %s set device_status=:dstatus,location=:dlocation where device_status!=%d and device_status!=%d and serialnb=:snb; if %s=1 then insert into %s(historydeviceid, serialnb,  device_status, user_comment, location, status_change, created,author,terminal_name) values (%s,:snb,'%s',:usercomment,:dlocation,to_date(:statchange,'%s'), sysdate,'%s','%s'); update %s set  CPNT_STATUS=:dstatus where location=:snb;if %s>0 then insert all into %s (historycpntid,snbid,cpnt_status,user_comment,location,status_change,created,author,terminal_name) values(%s,snbid,'%s',:usercomment,:snb,to_date(:statchange,'%s'),sysdate,'%s','%s') select snbid from %s where location=:snb; end if;:numrows:=%s; :rescode:=0; else :rescode:=-10; :numrows:=0;end if;END;",HW_DEVICE_TABLE,IN_USE,DESTROYED,SQLROWCOUNT,HISTORY_DEVICE_TABLE,seqname,new_device_status,_date_format,login,host,HW_CPNTS_TABLE,SQLROWCOUNT,HISTORY_CPNT_TABLE,seq_name,new_device_status,_date_format,login,host,HW_CPNTS_TABLE,SQLROWCOUNT);			
		else
			sprintf(selectdev,"BEGIN update %s set device_status=:dstatus where device_status!=%d and device_status!=%d and serialnb=:snb; if %s=1 then update %s set serialnb=:snb, nodeused=1, last_update=sysdate,user_update='%s' where devicename=:dname and serialnb is null returning deviceid,system_name,location into :devid,:sysname,:dlocation; if %s=1 then insert into %s(historydeviceid, deviceid,serialnb,  device_status, user_comment, location, status_change, system_name,created,author,terminal_name) values (%s,:devid,:snb,'%s',:usercomment,:dlocation,to_date(:statchange,'%s'),:sysname, sysdate,'%s','%s');update %s set  CPNT_STATUS=:dstatus where location=:snb; if %s>0 then update %s t set t.snbid=(select p.snbid from %s p where p.hwname=t.cpntname) where t.motherboardid=:devid;insert all into %s (historycpntid,snbid,cpntid,cpnt_status,user_comment,location,status_change,created,author,terminal_name) values(%s,snbid,cpntid,'%s',:usercomment,:snb,to_date(:statchange,'%s'),sysdate,'%s','%s') select snbid,cpntid from %s where motherboardid=:devid; end if; :numrows:=%s; :rescode:=0; else :rescode:=-5; :numrows:=0;end if; else :rescode:=-10; :numrows:=0; end if; END;",HW_DEVICE_TABLE,IN_USE,DESTROYED, SQLROWCOUNT,LOGICAL_DEVICE_TABLE,login,SQLROWCOUNT,HISTORY_DEVICE_TABLE,seqname,new_device_status,_date_format,login,host,HW_CPNTS_TABLE,SQLROWCOUNT,LG_CPNTS_TABLE,HW_CPNTS_TABLE, HISTORY_CPNT_TABLE,seq_name,new_device_status,_date_format,login,host,LG_CPNTS_TABLE,SQLROWCOUNT);

		//std::cout<<"selectdev="<<selectdev<<std::endl;
		status=OCIStmtPrepare(stmthp, ociError, (text*)selectdev, (ub4)strlen((char *)selectdev), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else	
			status=OCIBindByName(stmthp, &bndp[0], ociError,(text*)":dstatus",-1,(dvoid*) &status_case,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBind 3 unsuccessful");
		}
		else
		{
			if(strncmp(usercomments,"bla",3)==0)
				status=OCIBindByName(stmthp, &bndp[1], ociError,(text*)":usercomment",-1,(dvoid*) user_comments,strlen(user_comments)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
			else
				status=OCIBindByName(stmthp, &bndp[1], ociError,(text*)":usercomment",-1,(dvoid*) usercomments,strlen(usercomments)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBind 4 unsuccessful");
		}
		else
		{
			//std::cout<<"date ="<<status_datechange<<std::endl;
			if(status_datechange!=NULL) 
				status=OCIBindByName(stmthp, &bndp[2], ociError,(text*)":statchange",-1,(dvoid*) status_datechange,strlen(status_datechange)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
			else
			{
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"DATE OF CHANGE STATUS MUST BE GIVEN",ErrMess,1);
				return -1;
			}
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBind 5 unsuccessful");
		}
		else	
			status=OCIBindByName(stmthp, &bndp[3], ociError,(text*)":numrows",-1,(dvoid*) &numrows,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBind 5 unsuccessful");
		}
		else	
			status=OCIBindByName(stmthp, &bndp3, ociError,(text*)":rescode",-1,(dvoid*) &res_code,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBind 7 unsuccessful");
		}
		else
		{
			status=OCIBindByName(stmthp, &bndp[4], ociError,(text*)":snb",-1,(dvoid*) serialnb,strlen(serialnb)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBind 1 unsuccessful");
		}
		else
		{

			status=OCIBindByName(stmthp, &bndp[5], ociError,(text*)":dlocation",-1,(dvoid*) newlocation,101, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
		}
		if(status_case==IN_USE)
		{

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIBind 2 unsuccessful");
			}
			else
			{
				if(functional_devicename!=NULL)
					status=OCIBindByName(stmthp, &bndp[6], ociError,(text*)":dname",-1,(dvoid*) functional_devicename,strlen(functional_devicename)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
				else
				{
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName,"DEVICE NAME MUST BE GIVEN",ErrMess,1);
					return -1;
				}
			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBind 8 unsuccessful");
			}
			else
				status=OCIBindByName(stmthp, &bndp4, ociError,(text*)":sysname",-1,(dvoid*) &sysID,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBind 8 unsuccessful");
			}
			else
				status=OCIBindByName(stmthp, &bndp[7], ociError,(text*)":devid",-1,(dvoid*) &deviceid,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		}




		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBind 8 unsuccessful");
		}
		else
			status= OCIStmtExecute(ociHdbc, stmthp, ociError,  1, 0, 0, 0, OCI_DEFAULT );
		//std::cout<<"res_code="<<res_code<<" numrows "<<numrows<<std::endl;
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIExecute unsuccessful");
		}
		else
		{
			if (res_code!=0)
			{
				status = OCITransRollback(ociHdbc, ociError, 0);
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				if(res_code==-10)
					GetErrorMess(appliName,"Error The given serialnb  doens't exist or it is in IN_USE status ",ErrMess,1);
				else
				{
					if (res_code==-5)
					{
						GetErrorMess(appliName,"Error The given functional device is already in USE",ErrMess,1);
						//std::cout<<"value of rescode="<<numrows<<std::endl;
					}
					else
					{
						GetErrorMess(appliName,"Error in the transaction: no changes have been done",ErrMess,1);
					}


				}
				return -1;
			}
			else
			{
				status = OCITransCommit(ociHdbc, ociError, 0);
				if(status_case==IN_USE)	
				{
					// need to update the link state if there is no replacement and also pathes--> linkused=0 and pathused=0
					sprintf(selectdev,"begin update %s e set e.last_update=sysdate,e.user_update='%s',e.lkused=(select t.nodeused*f.nodeused from %s t,%s f, %s l,%s m where t.deviceid=l.deviceid and l.portid=e.portidfrom and f.deviceid=m.deviceid and m.portid=e.portidto) where (e.portidto in (select p.portid from %s p where p.deviceid=%d)) or (e.portidfrom in (select p.portid from %s p where p.deviceid=%d))  ; if %s>0 then :rescode:=%s(%d); end if; end; ",MACRO_CONNECTIVITY_TABLE,login,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,PORT_TABLE,PORT_TABLE,PORT_TABLE,deviceid,PORT_TABLE,deviceid,SQLROWCOUNT,_UpdatePathUsed,sysID);
					status=OCIStmtPrepare(stmthp, ociError, (text*)selectdev, (ub4)strlen((char *)selectdev), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
					if(status!=OCI_SUCCESS)
					{
						if(rescode==0)
							rescode=ShowErrors (status, ociError, "OCIPrepare 2 unsuccessful");
					}
					else	
						status=OCIBindByName(stmthp, &bndp3, ociError,(text*)":rescode",-1,(dvoid*) &res_code,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

					if(status!=OCI_SUCCESS)
					{
						if(rescode==0)
							rescode=ShowErrors (status, ociError, "OCIStmtBind 2 unsuccessful");
					}
					else
						status= OCIStmtExecute(ociHdbc, stmthp, ociError,  1, 0, 0, 0, OCI_DEFAULT );

					if(status!=OCI_SUCCESS)
					{
						if(rescode==0)	
							rescode=ShowErrors (status, ociError, "OCIExecute2 unsuccessful");
					}
					if (res_code!=0 )
					{
						GetErrorMess(appliName,"Error couldn't update links and paths status",ErrMess,1);
						status = OCITransCommit(ociHdbc, ociError, 0);
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						return -1;
					}
					else
						status = OCITransCommit(ociHdbc, ociError, 0);
				}
			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCITransCommit unsuccessful");
			}
		}
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 
		else
			OCIReportError(ociError,appliName, ErrMess,0); 

		return (status+rescode);

	}

	/************************************************************************/
	// return the last functional device occupied by the hw device (serialnb)
	/***********************************************************************/

	/**
	* return last functional device occupied by the hw device (serialnb) returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param serialnb : serial nb hw device
	* @param functional_devicename : last functional devicename occupied by the given hw name. If there is no board (for instance hw was always a spare) we put ErrMess=NO_ROWS_SELECTED and res=0
	* @param len_functionaldname : length of the functional device name 
	* @param ErrMess : error message in case of failure or no return found
	* @return 0 if it is successful
	*/

	/******************************************************************/
	EXTERN_CONFDB
		int GetHWLastFunctionalDevice(char* serialnb,char* functional_devicename,int &len_functionaldname ,char* ErrMess)
	{
		char appliName[100]="GetHWLastFunctionalDevice";
		int i = 0;
		int j=0;

		int devlen=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		sword status;
		int rescode=0;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def[1];
		char* devicename=NULL;
		int dname_null=0;
		char logmessage[100];
		OCIParam *parmdp;

		int len=0;
		char selectdev[1000];
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		if(serialnb==NULL)
		{
			GetErrorMess(appliName," device name MUST be given",ErrMess,1);
			return -1;
		}
		if(status!=OCI_SUCCESS)
		{
			rescode=ShowErrors (status, ociError, "OCIStmtHAndleStmt unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrMess,1);
			return -1;
		}
		else
		{
			sprintf(selectdev,"select t.devicename||'?' from %s t,%s p where t.deviceid=p.deviceid and p.serialnb=:snb and p.device_status='IN_USE' and p.status_change=(select max(status_change) from %s where serialnb=:snb and device_status='IN_USE')",LOGICAL_DEVICE_TABLE,HISTORY_DEVICE_TABLE,HISTORY_DEVICE_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdev,(ub4) strlen(selectdev),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else	
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":snb",-1,(dvoid*) serialnb,strlen(serialnb)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByPos unsuccessful");
		}
		else	
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIExecute unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 1);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet1  unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &devlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "Execute unsuccessful");
			}
		}
		else
		{
			devicename=(char*)malloc((devlen+1)*sizeof(char));
			if(devicename!=NULL)
				status=OCIDefineByPos (stmthp, &def[0], ociError, 1,devicename,devlen+1, SQLT_STR, 0, 0, 0, OCI_DEFAULT);
			else
			{
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"MALLOC UNSUCCESSFUL",ErrMess,1);
				return -1;
			}
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos1 unsuccessful");

			}
		}
		else
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);

		if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
		{	
			Format_output(dname_null,devicename, logmessage,'?');
			//std::cout0 <<"logmessage dname "<<logmessage <<"oracle "<<dname_null <<std::endl;
			i=strcspn(devicename,"?");
			if(len_functionaldname>i)
			{
				devicename[i]='\0';
				strcpy(functional_devicename,devicename);

			}
			else
			{
				rescode=ShowErrors (status, ociError, "Fetch unsuccessful");
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrMess,1);
				free(devicename);
				len_functionaldname=i;
				return -1;
			}
		} 
		else 
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "Fetch unsuccessful");
				rescode=0;
				GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrMess,1);
				strcpy(functional_devicename,"none");

			}
		}
		if(devicename!=NULL)
			free(devicename);
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(strstr(ErrMess,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrMess,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrMess,1); 
			}
		}
		rescode+=status;
		return rescode;
	}


	/************************************************************************/
	// return the status of a hw device (serialnb)
	/***********************************************************************/
	/**
	* return the status of a hw device (serialnb) returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param functional_devicename :  functional devicename
	* @param serialnb : last serial nb which was occupying the given log. devicename
	* @param len_serialnb : length of the functional device name 
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/
	/******************************************************************/
	EXTERN_CONFDB
		int GetFunctionalDeviceLastHW(char* functional_devicename,char* serialnb,int &len_serialnb ,char* ErrMess)
	{
		char appliName[100]="GetFunctionalDeviceLastHW";
		int i = 0;
		int j=0;

		int devlen=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		sword status;
		int rescode=0;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def[1];
		char* devicename=NULL;
		int dname_null=0;
		char logmessage[100];
		OCIParam *parmdp;

		int len=0;
		char selectdev[1000];
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		if(serialnb==NULL)
		{
			GetErrorMess(appliName," device name MUST be given",ErrMess,1);
			return -1;
		}
		if(status!=OCI_SUCCESS)
		{
			rescode=ShowErrors (status, ociError, "OCIStmtHAndleStmt unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrMess,1);
			return -1;
		}
		else
		{
			sprintf(selectdev,"select p.serialnb||'?' from %s t,%s p where t.deviceid=p.deviceid and t.devicename=:dname and device_status='IN_USE' and p.status_change=(select max(status_change) from %s where deviceid=p.deviceid and device_status='IN_USE')",LOGICAL_DEVICE_TABLE,HISTORY_DEVICE_TABLE,HISTORY_DEVICE_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdev,(ub4) strlen(selectdev),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else	
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":dname",-1,(dvoid*) functional_devicename,strlen(functional_devicename)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByPos unsuccessful");
		}
		else	
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIExecute unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 1);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet1  unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &devlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "Execute unsuccessful");
			}
		}
		else
		{
			devicename=(char*)malloc((devlen+1)*sizeof(char));
			if(devicename!=NULL)
				status=OCIDefineByPos (stmthp, &def[0], ociError, 1,devicename,devlen+1, SQLT_STR, 0, 0, 0, OCI_DEFAULT);
			else
			{
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"MALLOC UNSUCCESSFUL",ErrMess,1);
				return -1;
			}
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos1 unsuccessful");

			}
		}
		else
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);

		if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
		{	
			Format_output(dname_null,devicename, logmessage,'?');
			//std::cout0 <<"logmessage dname "<<logmessage <<"oracle "<<dname_null <<std::endl;
			i=strcspn(devicename,"?");
			if(len_serialnb>i)
			{
				devicename[i]='\0';
				strcpy(serialnb,devicename);

			}
			else
			{
				rescode=ShowErrors (status, ociError, "Fetch unsuccessful");
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrMess,1);
				free(devicename);
				len_serialnb=i;
				return -1;
			}
		} 
		else 
		{
			if(rescode==0)
			{
				rescode=0;
				GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrMess,1);
				strcpy(serialnb,"none");

			}
		}
		if(devicename!=NULL)
			free(devicename);
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(strstr(ErrMess,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrMess,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrMess,1); 
			}
		}
		rescode+=status;
		return rescode;
	}
	/**
	* return the history of a functional device (devicename) returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param functional_devicename :  functional devicename
	* @param functionaldevice_history : result as follows lines are separated with '\\0',one line consists of "devicename|serialnb|status|date of change|location" 
	* @param len_history : length of the functionaldevice_history 
	* @param min_date : all changes which appear after this date (or equal to this date). Put "none", if nothing
	* @param max_date : all changes which appear before this date (or equal to this date). Put "none", if nothing.
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/

	/******************************************************************/
	EXTERN_CONFDB
		int GetHistoryOfFunctionalDevice(char* functional_devicename,char* functionaldevice_history,int &len_history , char* min_date, char* max_date,char* ErrMess)
	{
		char appliName[100]="GetHistoryOfFunctionalDevice";
		int i = 0;
		int j=0;

		int devlen=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		sword status;
		int pos2=0;
		int pos3=0;
		int pos1=0;
		int rescode=0;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIBind  *bnd2p = (OCIBind *) 0; 
		OCIBind  *bnd3p = (OCIBind *) 0; 
		int prefetch_rows=8000;
		OCIDefine* def[1];
		char* devicenamehist_row=NULL;
		char* devicenamehist_list=NULL;
		int dname_null=0;
		char logmessage[100];
		OCIParam *parmdp;
		int devhist_len=0;
		int len=0;
		char selectdev[1000];
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		if(functional_devicename==NULL)
		{
			GetErrorMess(appliName," device name MUST be given",ErrMess,1);
			return -1;
		}
		if(status!=OCI_SUCCESS)
		{
			rescode=ShowErrors (status, ociError, "OCIStmtHAndleStmt unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrMess,1);
			return -1;
		}
		else
		{
			if(strncmp(min_date,"none",4)==0 && strncmp(max_date,"none",4)==0)
			{
				sprintf(selectdev,"select t.devicename||'|'||nvl(p.serialnb,'none')||'|'||p.device_status||'|'||to_char(p.status_change,'%s')||'|'||nvl(p.location,'none')||'?' from %s t,%s p where t.deviceid=p.deviceid and t.devicename=:dname  order by p.status_change",_date_format,LOGICAL_DEVICE_TABLE,HISTORY_DEVICE_TABLE);
				i=7;
			}
			else
			{
				if(strncmp(min_date,"none",4)!=0 && strncmp(max_date,"none",4)!=0)
				{
					i=6;
					sprintf(selectdev,"select t.devicename||'|'||nvl(p.serialnb,'none')||'|'||p.device_status||'|'||to_char(p.status_change,'%s')||'|'||nvl(p.location,'none')||'?' from %s t,%s p where t.deviceid=p.deviceid and t.devicename=:dname and to_char(p.status_change,'%s') between :mindate and :maxdate order by p.status_change",_date_format,LOGICAL_DEVICE_TABLE,HISTORY_DEVICE_TABLE,_date_format);
				}
				else
				{
					if(strncmp(max_date,"none",4)==0)
					{
						sprintf(selectdev,"select t.devicename||'|'||nvl(p.serialnb,'none')||'|'||p.device_status||'|'||to_char(p.status_change,'%s')||'|'||nvl(p.location,'none')||'?' from %s t,%s p where t.deviceid=p.deviceid and t.devicename=:dname and  to_char(p.status_change,'%s') >= :mindate order by p.status_change",_date_format,LOGICAL_DEVICE_TABLE,HISTORY_DEVICE_TABLE,_date_format);
						i=3;
					}
					else
					{
						sprintf(selectdev,"select t.devicename||'|:'||nvl(p.serialnb,'none')||'|'||p.device_status||'|'||to_char(p.status_change,'%s')||'|'||nvl(p.location,'none')||'?' from %s t,%s p where t.deviceid=p.deviceid and t.devicename=:dname and  to_char(p.status_change,'%s')<=:maxdate order by p.status_change",_date_format,LOGICAL_DEVICE_TABLE,HISTORY_DEVICE_TABLE,_date_format);
						i=2;
					}
				}

			}

			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdev,(ub4) strlen(selectdev),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else	
			status=OCIBindByName(stmthp, &bnd2p, ociError,(text*)":dname",-1,(dvoid*) functional_devicename,strlen(functional_devicename)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if((i%3)==0)
		{
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else	
				status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":mindate",-1,(dvoid*) min_date,strlen(min_date)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		}
		if((i%2)==0)
		{
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else	
				status=OCIBindByName(stmthp, &bnd3p, ociError,(text*)":maxdate",-1,(dvoid*) max_date,strlen(max_date)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=	OCIAttrSet (stmthp,OCI_HTYPE_STMT,&prefetch_rows,0,OCI_ATTR_PREFETCH_ROWS,ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByPos unsuccessful");
		}
		else	
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIExecute unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 1);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet1  unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &devlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "Execute unsuccessful");
			}
		}
		else
		{
			devicenamehist_row=(char*)malloc((devlen+1)*sizeof(char));
			if(devicenamehist_row!=NULL)
				status=OCIDefineByPos (stmthp, &def[0], ociError, 1,devicenamehist_row,devlen+1, SQLT_STR, 0, 0, 0, OCI_DEFAULT);
			else
			{
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"MALLOC UNSUCCESSFUL",ErrMess,1);
				return -1;
			}
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "Execute unsuccessful");
			}
		}
		else
		{

			i=0;
			pos2=0;
			pos3=0;
			pos1=0;
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2 (stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);
				if(status==OCI_SUCCESS)
				{
					Format_output(dname_null, devicenamehist_row, logmessage, '?');
					pos1=strcspn(devicenamehist_row,"?");
					devicenamehist_row[pos1]='\0';
					pos3=pos2;
					pos2+=pos1+1;
					devicenamehist_list=(char*) realloc(devicenamehist_list,pos2*sizeof(char));
					if(devicenamehist_list==NULL)
					{
						status=-10;
						rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
						rescode=-1;
						free(devicenamehist_row);
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrMess,1);
						return rescode;	
					}
					else
					{
						memcpy(devicenamehist_list+pos3,devicenamehist_row,pos1+1);
						i++;
					}
				}
			}
			if(rescode==0)
			{	
				if(i>0)
				{
					if(pos2>len_history)
					{

						len_history=pos2;
						rescode= -1;	
						free(devicenamehist_row);
						free(devicenamehist_list);
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrMess,1);
						return rescode;	
					}
					else
					{


						memcpy(functionaldevice_history,devicenamehist_list,pos2);
						len_history=pos2;
					}
				}
				else
				{

					strcpy(functionaldevice_history,"NO_ROWS_SELECTED");
					GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrMess,1);
					rescode=0;

				}

			}


		} 

		if(devicenamehist_row!=NULL)
			free(devicenamehist_row);
		if(devicenamehist_list!=NULL)
			free(devicenamehist_list);
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(strstr(ErrMess,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrMess,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrMess,1); 
			}
		}
		rescode+=status;
		return rescode;
	}

	/**
	* return the history of a hw device (serialnb) returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param serialnb :  functional devicename
	* @param functionaldevice_history : result as follows lines are separated with '\\0',one line consists of "devicename|serialnb|status|date of change|location" 
	* @param len_history : length of the logcaldevice_history 
	* @param min_date : all changes which appear after this date (or equal to this date). Put "none", if nothing
	* @param max_date : all changes which appear before this date (or equal to this date). Put "none", if nothing.
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/
	/******************************************************************/
	EXTERN_CONFDB
		int GetHistoryOfHWDevice(char* serialnb,char* functionaldevice_history,int &len_history , char* min_date, char* max_date,char* ErrMess)
	{
		char appliName[100]="GetHistoryOfHWDevice";
		int i = 0;
		int j=0;

		int devlen=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		sword status;
		int pos2=0;
		int pos3=0;
		int pos1=0;
		int rescode=0;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIBind  *bnd2p = (OCIBind *) 0; 
		OCIBind  *bnd3p = (OCIBind *) 0; 
		int prefetch_rows=8000;
		OCIDefine* def[1];
		char* devicenamehist_row=NULL;
		char* devicenamehist_list=NULL;
		int dname_null=0;
		char logmessage[100];
		OCIParam *parmdp;
		int devhist_len=0;
		int len=0;
		char selectdev[1000];
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		if(serialnb==NULL)
		{
			GetErrorMess(appliName," device name MUST be given",ErrMess,1);
			return -1;
		}

		if(status!=OCI_SUCCESS)
		{
			rescode=ShowErrors (status, ociError, "OCIStmtHAndleStmt unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrMess,1);
			return -1;
		}
		else
		{
			if(strncmp(min_date,"none",4)==0 && strncmp(max_date,"none",4)==0)
			{
				sprintf(selectdev,"select t.devicename||'|'||p.serialnb||'|'||p.device_status||'|'||to_char(p.status_change,'%s')||'|'||nvl(p.location,'none')||'?' from %s t,%s p where  p.serialnb=:dname  and nvl(p.deviceid,-1)=t.deviceid order by p.status_change",_date_format,LOGICAL_DEVICE_TABLE,HISTORY_DEVICE_TABLE);
				i=7;
			}
			else
			{
				if(strncmp(min_date,"none",4)!=0 &&strncmp(max_date,"none",4)!=0)
				{
					i=6;
					sprintf(selectdev,"select t.devicename||'|'||nvl(p.serialnb,'none')||'|'||p.device_status||'|'||to_char(p.status_change,'%s')||'|'||nvl(p.location,'none')||'?' from %s t,%s p where t.deviceid=nvl(p.deviceid,-1) and p.serialnb=:dname and to_char(p.status_change,'%s') between :mindate and :maxdate order by p.status_change",_date_format,LOGICAL_DEVICE_TABLE,HISTORY_DEVICE_TABLE,_date_format);
				}
				else
				{
					if(strncmp(max_date,"none",4)==0)
					{
						sprintf(selectdev,"select t.devicename||'|'||nvl(p.serialnb,'none')||'|'||p.device_status||'|'||to_char(p.status_change,'%s')||'|'||nvl(p.location,'none')||'?' from %s t,%s p where t.deviceid=nvl(p.deviceid,-1) and p.serialnb=:dname and to_char(p.status_change,'%s')>=:mindate order by p.status_change",_date_format,LOGICAL_DEVICE_TABLE,HISTORY_DEVICE_TABLE,_date_format);
						i=3;
					}
					else
					{
						sprintf(selectdev,"select t.devicename||'|'||nvl(p.serialnb,'none')||'|'||p.device_status||'|'||to_char(p.status_change,'%s')||'|'||nvl(p.location,'none')||'?' from %s t,%s p where t.deviceid=nvl(p.deviceid,-1) and p.serialnb=:dname and  to_char(p.status_change,'%s')<=:maxdate order by p.status_change",_date_format,LOGICAL_DEVICE_TABLE,HISTORY_DEVICE_TABLE,_date_format);
						i=2;
					}
				}

			}
			//std::cout<<"value of sqlquery "<<selectdev<<std::endl;	
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdev,(ub4) strlen(selectdev),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else	
			status=OCIBindByName(stmthp, &bnd2p, ociError,(text*)":dname",-1,(dvoid*) serialnb,strlen(serialnb)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if((i%3)==0)
		{
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else	
				status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":mindate",-1,(dvoid*) min_date,strlen(min_date)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		}
		if((i%2)==0)
		{
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else	
				status=OCIBindByName(stmthp, &bnd3p, ociError,(text*)":maxdate",-1,(dvoid*) max_date,strlen(max_date)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=	OCIAttrSet (stmthp,OCI_HTYPE_STMT,&prefetch_rows,0,OCI_ATTR_PREFETCH_ROWS,ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByPos unsuccessful");
		}
		else	
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);
		//std::cout<<"after execute "<<std::endl;	
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIExecute unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 1);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet1  unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &devlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);



		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "Execute unsuccessful");
			}
		}
		else
		{
			devicenamehist_row=(char*)malloc((devlen+1)*sizeof(char));
			if(devicenamehist_row!=NULL)
				status=OCIDefineByPos (stmthp, &def[0], ociError, 1,devicenamehist_row,devlen+1, SQLT_STR, 0, 0, 0, OCI_DEFAULT);
			else
			{
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"MALLOC UNSUCCESSFUL",ErrMess,1);
				return -1;
			}
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "Execute unsuccessful");
			}
		}
		else
		{

			i=0;
			pos2=0;
			pos3=0;
			pos1=0;
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2 (stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);
				if(status==OCI_SUCCESS)
				{
					Format_output(dname_null, devicenamehist_row, logmessage, '?');
					pos1=strcspn(devicenamehist_row,"?");
					devicenamehist_row[pos1]='\0';
					pos3=pos2;
					pos2+=pos1+1;
					devicenamehist_list=(char*) realloc(devicenamehist_list,pos2*sizeof(char));
					if(devicenamehist_list==NULL)
					{
						status=-10;
						rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
						rescode=-1;
						free(devicenamehist_row);
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrMess,1);
						return rescode;	
					}
					else
					{
						memcpy(devicenamehist_list+pos3,devicenamehist_row,pos1+1);
						i++;
						//std::cout<<"devicenamehist_list "<<(devicenamehist_list+pos3)<<" and  i="<<std::endl;	
					}
				}
			}
			//std::cout<<"after the loop "<<i<<std::endl;	

			if(rescode==0)
			{	

				if(i>0)
				{
					if((pos2)>len_history)
					{

						len_history=pos2;
						rescode= -1;	
						free(devicenamehist_row);
						free(devicenamehist_list);
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrMess,1);
						return rescode;	
					}
					else
					{


						memcpy(functionaldevice_history,devicenamehist_list,pos2);
						len_history=pos2;
					}
				}
				else
				{



					strcpy(functionaldevice_history,"NO_ROWS_SELECTED");
					GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrMess,1);
					rescode=0;

				}

			}


		} 
		//std::cout<<"just before free "<<i<<std::endl;
		if(devicenamehist_row!=NULL)
			free(devicenamehist_row);
		if(devicenamehist_list!=NULL)
			free(devicenamehist_list);
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(strstr(ErrMess,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrMess,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrMess,1); 
			}
		}
		rescode+=status;
		//std::cout<<"end of fct "<<std::endl;	

		return rescode;
	}

	/**
	* return the list of functional devices which belongs to the given subsystem with a given status.
	* The user should manage the memory : there is no memory allocation.
	* @param system_name :  system_name. Put "all" ,lower case if you want all of them. 
	* @param device_status : status selected (IN_USE or NONE)
	* @param len_devlist : length of the device_list
	* @param device_list : list of functional devices 
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/

	/******************************************************************/
	EXTERN_CONFDB
		int GetFunctionalDeviceByStatus(char* system_name, char* device_status, int &len_devlist , char* device_list,char* ErrMess)
	{
		char appliName[100]="GetFunctionalDeviceByStatus";
		int i = 0;
		int j=0;

		int devlen=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		sword status;
		int pos2=0;
		int pos3=0;
		int pos1=0;
		int rescode=0;
		OCIStmt *stmthp;
		OCIBind  *bnd2p = (OCIBind *) 0; 

		int prefetch_rows=8000;
		OCIDefine* def[1];
		char* devicenamehist_row=NULL;
		char* devicenamehist_list=NULL;
		int dname_null=0;
		char logmessage[100];
		OCIParam *parmdp;
		int devhist_len=0;
		int len=0;
		char selectdev[1000];
		int sysID=0;
		if(strncmp(system_name,"all",3)==0)
			sysID=1;
		else
			sysID=GetSubsystemID(system_name);
		int status_case=0;
		if(strcmp(device_status,"NONE")==0)
			status_case=NOT_AVAILABLE;
		if(strcmp(device_status,"IN_USE")==0)
			status_case=IN_USE;
		if(status_case==0)
		{
			GetErrorMess(appliName," INVALID STATUS",ErrMess,1);
			return -1;
		}
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		if(system_name==NULL)
		{
			GetErrorMess(appliName," system name MUSTNOT BE NULL",ErrMess,1);
			return -1;
		}
		if(status!=OCI_SUCCESS)
		{
			rescode=ShowErrors (status, ociError, "OCIStmtHAndleStmt unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrMess,1);
			return -1;
		}
		else
		{
			if(status_case==NOT_AVAILABLE)
				sprintf(selectdev,"select devicename||'?' from %s  where serialnb is null and mod(system_name,:sysid)=0 and  deviceid!=-1 and system_name is not null ",LOGICAL_DEVICE_TABLE);
			else
				sprintf(selectdev,"select t.devicename||'?' from %s t,%s d  where t.serialnb=d.serialnb and d.device_status=%d and mod(t.system_name,:sysid)=0 and  t.deviceid!=-1 and t.system_name is not null ",LOGICAL_DEVICE_TABLE,HW_DEVICE_TABLE,IN_USE);

			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdev,(ub4) strlen(selectdev),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else	
			status=OCIBindByName(stmthp, &bnd2p, ociError,(text*)":sysid",-1,(dvoid*) &sysID,sizeof(int), SQLT_INT ,(dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);



		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=	OCIAttrSet (stmthp,OCI_HTYPE_STMT,&prefetch_rows,0,OCI_ATTR_PREFETCH_ROWS,ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByPos unsuccessful");
		}
		else	
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIExecute unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 1);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet1  unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &devlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "Execute unsuccessful");
			}
		}
		else
		{
			devicenamehist_row=(char*)malloc((devlen+1)*sizeof(char));
			if(devicenamehist_row!=NULL)
				status=OCIDefineByPos (stmthp, &def[0], ociError, 1,devicenamehist_row,devlen+1, SQLT_STR, 0, 0, 0, OCI_DEFAULT);
			else
			{
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"MALLOC UNSUCCESSFUL",ErrMess,1);
				return -1;
			}
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "Execute unsuccessful");
			}
		}
		else
		{

			i=0;
			pos2=0;
			pos3=0;
			pos1=0;
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2 (stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);
				if(status==OCI_SUCCESS)
				{
					Format_output(dname_null, devicenamehist_row, logmessage, '?');
					pos1=strcspn(devicenamehist_row,"?");
					devicenamehist_row[pos1]='\0';
					pos3=pos2;
					pos2+=pos1+1;
					devicenamehist_list=(char*) realloc(devicenamehist_list,pos2*sizeof(char));
					if(devicenamehist_list==NULL)
					{
						status=-10;
						rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
						rescode=-1;
						free(devicenamehist_row);
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrMess,1);
						return rescode;	
					}
					else
					{
						memcpy(devicenamehist_list+pos3,devicenamehist_row,pos1+1);
						i++;
					}
				}
			}
			if(rescode==0)
			{	
				if(i>0)
				{
					if((pos2)>len_devlist)
					{
						//devicenamehist_list[pos2-1]='\0';
						len_devlist=pos2;
						rescode= -1;	
						free(devicenamehist_row);
						free(devicenamehist_list);
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrMess,1);
						return rescode;	
					}
					else
					{

						//devicenamehist_list[pos2-1]='\0';
						memcpy(device_list,devicenamehist_list,pos2);
						len_devlist=pos2;
					}
				}
				else
				{
					rescode=0;
					strcpy(device_list,"NO_ROWS_SELECTED");
					GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrMess,1);
				}
			}




		} 

		if(devicenamehist_row!=NULL)
			free(devicenamehist_row);
		if(devicenamehist_list!=NULL)
			free(devicenamehist_list);
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(strstr(ErrMess,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrMess,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrMess,1); 
			}
		}
		rescode+=status;
		return rescode;
	}
	/**
	* return the list of hw devices which are in the given status
	* The user should manage the memory : there is no memory allocation.
	* @param system_name :  system_name. Put "all", lower case if you want all of them. Use donly if the given status='IN_USE"
	* @param functionaldevice_status: result as follows list of the serialnbs are separated with '\\0', 
	* @param len_status : length of the logcaldevice_history 
	* @param device_status : status you want to group by your serialnb (SPARE,EXT_TEST,TEST,DESTROYED,IN_REPAIR, IN_USE)
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/

	/******************************************************************/
	EXTERN_CONFDB
		int GetHWDeviceByStatus(char* system_name, char* device_status,int &len_status ,char* functionaldevice_status,char* ErrMess)
	{
		char appliName[100]="GetHWDeviceByStatus";
		int i = 0;
		int j=0;

		int devlen=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		sword status;
		int pos2=0;
		int pos3=0;
		int pos1=0;
		int rescode=0;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIBind  *bnd2p = (OCIBind *) 0; 

		int prefetch_rows=8000;
		OCIDefine* def[1];
		char* devicenamehist_row=NULL;
		char* devicenamehist_list=NULL;
		int dname_null=0;
		char logmessage[100];
		OCIParam *parmdp;
		int devhist_len=0;
		int len=0;
		int sysID=1;
		char selectdev[1000];
		int status_case=0;
		if(strcmp(device_status,"DESTROYED")==0)
			status_case=DESTROYED;
		if(strcmp(device_status,"IN_REPAIR")==0)
			status_case=IN_REPAIR;
		if(strcmp(device_status,"SPARE")==0)
			status_case=SPARE;
		if(strcmp(device_status,"EXT_TEST")==0)
			status_case=EXT_TEST;
		if(strcmp(device_status,"TEST")==0)
			status_case=TEST;
		if(strcmp(device_status,"IN_USE")==0)
			status_case=IN_USE;
		if(strcmp(system_name,"all")==0)
			sysID=1;
		else
			sysID=GetSubsystemID(system_name);
		if(status_case==0)
		{
			GetErrorMess(appliName,"INVALID STATUS",ErrMess,1);
			rescode=-1;
			return rescode;
		}
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);

		if(status!=OCI_SUCCESS)
		{
			rescode=ShowErrors (status, ociError, "OCIStmtHAndleStmt unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrMess,1);
			return -1;
		}
		else
		{
			if(status_case==IN_USE)
				sprintf(selectdev,"select t.serialnb||'?' from %s t,%s h where t.device_status=:dstatus and t.serialnb=h.serialnb and mod(h.system_name,:sysid)=0  ",HW_DEVICE_TABLE,LOGICAL_DEVICE_TABLE);
			else
				sprintf(selectdev,"select serialnb||'?' from %s  where device_status=:dstatus  ",HW_DEVICE_TABLE);

			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdev,(ub4) strlen(selectdev),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else	
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":dstatus",-1,(dvoid*) &status_case,sizeof(int), SQLT_INT ,(dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
		{
			if(status_case==IN_USE)
				status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":sysid",-1,(dvoid*) &sysID,sizeof(int), SQLT_INT ,(dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
		}


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=	OCIAttrSet (stmthp,OCI_HTYPE_STMT,&prefetch_rows,0,OCI_ATTR_PREFETCH_ROWS,ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByPos unsuccessful");
		}
		else	
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIExecute unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 1);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet1  unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &devlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "Execute unsuccessful");
			}
		}
		else
		{
			devicenamehist_row=(char*)malloc((devlen+1)*sizeof(char));
			if(devicenamehist_row!=NULL)
				status=OCIDefineByPos (stmthp, &def[0], ociError, 1,devicenamehist_row,devlen+1, SQLT_STR, 0, 0, 0, OCI_DEFAULT);
			else
			{
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"MALLOC UNSUCCESSFUL",ErrMess,1);
				return -1;
			}
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "Execute unsuccessful");
			}
		}
		else
		{

			i=0;
			pos2=0;
			pos3=0;
			pos1=0;
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2 (stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);
				if(status==OCI_SUCCESS)
				{
					Format_output(dname_null, devicenamehist_row, logmessage, '?');
					pos1=strcspn(devicenamehist_row,"?");
					devicenamehist_row[pos1]='\0';
					pos3=pos2;
					pos2+=pos1+1;
					devicenamehist_list=(char*) realloc(devicenamehist_list,pos2*sizeof(char));
					if(devicenamehist_list==NULL)
					{
						status=-10;
						rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
						rescode=-1;
						free(devicenamehist_row);
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrMess,1);
						return rescode;	
					}
					else
					{
						memcpy(devicenamehist_list+pos3,devicenamehist_row,pos1+1);
						i++;
					}
				}
			}
			if(rescode==0)
			{	
				if(i>0)
				{
					if((pos2)>len_status)
					{
						//devicenamehist_list[pos2-1]='\0';
						len_status=pos2;
						rescode= -1;	
						free(devicenamehist_row);
						free(devicenamehist_list);
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrMess,1);
						return rescode;	
					}
					else
					{

						//devicenamehist_list[pos2-1]='\0';
						memcpy(functionaldevice_status,devicenamehist_list,pos2);
						len_status=pos2;
					}
				}
				else
				{
					rescode=0;
					strcpy(functionaldevice_status,"NO_ROWS_SELECTED");
					GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrMess,1);
				}
			}




		} 

		if(devicenamehist_row!=NULL)
			free(devicenamehist_row);
		if(devicenamehist_list!=NULL)
			free(devicenamehist_list);
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(strstr(ErrMess,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrMess,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrMess,1); 
			}
		}
		rescode+=status;
		return rescode;
	}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif
