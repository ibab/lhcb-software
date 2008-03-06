/********************************************************************************/
// Author: L.Abadie
// version:v3.5
// file which performs against board component inventory (update + select)
// NB: for a board cpnt the status TEST doesn't make sense as it requires more stuff to perform a test, usually it will be tested in a lab (so EXT_TEST)
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
	* give the status of a given functional board cpnt name returning an integer value (0 if it's successful).
	* The user should manage the memory : there is no memory allocation.
	* @param cpntname : name of the functional board cpnt
	* @param cpnt_status : status of the functional device  : IN_USE,NONE (means that there are no hw)
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/

	/******************************************************************/
	EXTERN_CONFDB
		int GetFunctionalBoardCpntStatus(char* cpntname,char* cpnt_status ,char* ErrMess)
	{
		char appliName[100]="GetFunctionalBoardCpntStatus";
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
		if(cpntname==NULL)
		{
			GetErrorMess(appliName," cpnt name MUST be given",ErrMess,1);
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
			sprintf(selectdev,"select decode(nvl(g.cpnt_status,-1),-1,%d,g.cpnt_status) from %s t, %s g where t.cpntname=:dname and nvl(t.snbid,-1)=g.snbid ",NOT_AVAILABLE,LG_CPNTS_TABLE,HW_CPNTS_TABLE);

			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdev,(ub4) strlen(selectdev),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else	
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":dname",-1,(dvoid*) cpntname,strlen(cpntname)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

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

			rescode=GetDeviceStatus(devstatus, cpnt_status);
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
	* give the status of the given hw board component (serialnb) returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param serialnb : serialnb of the hw cpnt. Put "none" if you provide the snbid. (case where your hw board cpnt doesn't have a serialnb) 
	* @param snbid : snbid of the hw cpnt. Put -1 if you have provided a serialnb. Useful if your hw doesn't have a serialnb.
	* @param cpnt_status : status of the hw device  : IN_USE,IN_REPAIR (not replaced yet or not repaired yet),DESTROYED (not replaced yet),TEST (means being test using CERN infrastructure ),EXT_TEST
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/

	/******************************************************************/
	EXTERN_CONFDB
		int GetHWBoardCpntStatus(char* serialnb,int snbid,char* cpnt_status ,char* ErrMess)
	{
		char appliName[100]="GetHWBoardCpntStatus";
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
		if(serialnb==NULL && snbid<1)
		{
			GetErrorMess(appliName," serialnb or snbid MUST be given",ErrMess,1);
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
			if(strncmp(serialnb,"none",4)!=0)
				sprintf(selectdev,"select cpnt_status from %s  where serialnb=:snb ",HW_CPNTS_TABLE);
			else
				sprintf(selectdev,"select cpnt_status from %s  where snbid=:snb ",HW_CPNTS_TABLE);

			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdev,(ub4) strlen(selectdev),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
		{
			if(strncmp(serialnb,"none",4)!=0)
				status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":snb",-1,(dvoid*) serialnb,strlen(serialnb)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
			else
				status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":snb",-1,(dvoid*) &snbid,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		}
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

			rescode=GetDeviceStatus(devstatus, cpnt_status);
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
	* replace a board cpnt (should be replacable on its own ) which  occupied the functional board cpnt ftc. (initial state of the board cpnt:IN_USE) returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* IF THE BOARD CPNT STATUS CHANGE IMPLIES ALSO THE STATUS CHANGE OF THE MOTHERBOARD, USE ReplaceLogicalDevice function, NOT THIS ONE.
	* @param cpntname : name of the functional board cpnt 
	* @param new_cpnt_status : new status of the board which was occupying this fct : IN_REPAIR,DESTROYED,SPARE, EXT_TEST
	* @param new_location : new location of the board cpnt
	* @param user_comments : comments to explain why this replacement. Put "none", if you the default one :"status change in functional device with name %s " or 
	* @param status_datechange : date of the status change of the board formatted as YY/MM/DD/HH24/MI/SS
	* @param serialnb_replaced : serialnb which replaces this cpnt board (must be already declared as SPARE, otherwise won't work). if none.
	* @param replace_date : date of the replacement formatted as YY/MM/DD/HH24/MI/SS
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/

	/******************************************************************/
	EXTERN_CONFDB
		int ReplaceFunctionalBoardCpnt(char* cpntname,char* new_cpnt_status,char* new_location,char* user_comments,char* status_datechange,char* serialnb_replaced,char* replace_date,char* ErrMess)
	{
		int status_case=0;
		int rescode=0;
		char appliName[100]="ReplaceFunctionalBoardCpnt";
		char seq_name[100]="LHCB_CPNT_HISTORY_COMPTID_SEQ.nextval";
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
				sprintf(usercomments,"status change in functional device with name %s ",cpntname);
			else
				sprintf(usercomments,"replacement of the functional device with name %s",cpntname);
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
		if(strcmp(new_cpnt_status,"DESTROYED")==0)
			status_case=DESTROYED;
		if(strcmp(new_cpnt_status,"IN_REPAIR")==0)
			status_case=IN_REPAIR;
		if(strcmp(new_cpnt_status,"SPARE")==0)
			status_case=SPARE;
		if(strcmp(new_cpnt_status,"EXT_TEST")==0)
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
			sprintf(selectdev,"BEGIN update %s set cpnt_status=:dstatus,location=:dlocation where cpnt_status=%d and snbid=(select snbid from %s where cpntname=:dname) and replacable=1; if %s=1 then insert ALL into %s(HISTORYCPNTID,snbid,cpnt_status,user_comment,location,status_change,created,author,terminal_name) values (%s,snb1,'%s',:usercomment,:dlocation,to_date(:statchange,'%s'),sysdate,'%s','%s') select snbid as snb1  from %s where cpntname=:dname; update %s set last_update=sysdate,user_update='%s',snbid=NULL where cpntname=:dname returning cpntid into :devid;insert ALL into %s(HISTORYCPNTID,cpntid,cpnt_status,user_comment,location,status_change,created,author,terminal_name) values (%s,devid,'NONE',:usercomment,location,to_date(:statchange,'%s'),sysdate,'%s','%s') select cpntid as devid,devicename as location from %s,%s where cpntname=:dname and motherboardid=deviceid;:numrows:=%s; :rescode:=0; else :rescode:=-10; :numrows:=0; end if;END;",HW_CPNTS_TABLE,IN_USE,LG_CPNTS_TABLE,SQLROWCOUNT,HISTORY_CPNT_TABLE,seq_name,new_cpnt_status,_date_format,login,host,LG_CPNTS_TABLE,LG_CPNTS_TABLE,login,HISTORY_CPNT_TABLE,seq_name,_date_format,login,host,LG_CPNTS_TABLE,LOGICAL_DEVICE_TABLE,SQLROWCOUNT,login,host);
		else
		{
			sprintf(selectdev,"BEGIN update %s set cpnt_status=:dstatus,location=:dlocation where cpnt_status=%d and snbid=(select snbid from %s where cpntname=:dname) and replacable=1; if %s=1 then insert ALL into %s(HISTORYCPNTID,snbid,  cpnt_status, user_comment, location, status_change, created,author,terminal_name) values (%s,snb1,'%s',:usercomment,:dlocation,to_date(:statchange,'%s'), sysdate,'%s','%s') select snbid as snb1 from %s where cpntname=:dname;if to_date(:statchange,'%s')<to_date(:statchange2,'%s') then insert ALL into %s(HISTORYCPNTID, cpntid,  cpnt_status, user_comment, location, status_change,created,author,terminal_name) values (%s,devid1,'NONE',:usercomment,motherboard,to_date(:statchange,'%s'), sysdate,'%s','%s') select cpntid as devid1,devicename as motherboard from %s,%s  where cpntname=:dname and motherboardid=deviceid; end if; if to_date(:statchange2,'%s')<to_date(:statchange,'%s') then :rescode:=-15; :numrows:=0; else update %s set snbid=(select snbid from %s where serialnb=:snb),last_update=sysdate,user_update='%s' where cpntname=:dname and %d=(select cpnt_status from %s where serialnb=:snb and cpnt_status=%d) returning cpntid into :devid; if %s=1 then insert all into %s(historycpntid,snbid,cpntid,cpnt_status,user_comment,location,status_change,created,author,terminal_name) values (%s,snb,devid1,'IN_USE',:usercomment,locat,to_date(:statchange2,'%s'),sysdate,'%s','%s') select snbid as snb,cpntid as devid1,devicename as locat from %s,%s where cpntname=:dname and motherboardid=deviceid;update %s set cpnt_status=%d where serialnb=:snb; :numrows:=%s;  else :rescode:=-5; :numrows:=0; end if;  end if; else :rescode:=-10; :numrows:=0;end if;END;",HW_CPNTS_TABLE,IN_USE,LG_CPNTS_TABLE,SQLROWCOUNT,HISTORY_CPNT_TABLE,seq_name,new_cpnt_status,_date_format,login,host,LG_CPNTS_TABLE,_date_format,_date_format,HISTORY_CPNT_TABLE,seq_name,_date_format,login,host,LG_CPNTS_TABLE,LOGICAL_DEVICE_TABLE,_date_format,_date_format,LG_CPNTS_TABLE,HW_CPNTS_TABLE,login,SPARE,HW_CPNTS_TABLE,SPARE,SQLROWCOUNT,HISTORY_CPNT_TABLE,seq_name,_date_format,login,host,LG_CPNTS_TABLE,LOGICAL_DEVICE_TABLE,HW_CPNTS_TABLE,IN_USE,SQLROWCOUNT,login,host);			
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
			if(cpntname!=NULL)
				status=OCIBindByName(stmthp, &bndp[2], ociError,(text*)":dname",-1,(dvoid*) cpntname,strlen(cpntname)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
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
					GetErrorMess(appliName,"Error The given log. cpnt name is not IN_USE or not replacable",ErrMess,1);
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
					// need to update the micro link state if there is no replacement --> linkused=0 
					sprintf(selectdev,"begin update %s e set e.last_update=sysdate,e.user_update='%s',e.lkused=(select decode(t.cpnt_status*f.cpnt_status,1,1,0) from %s t,%s f, %s l,%s m where t.snbid=l.snbid and l.cpntid=e.cpntidfrom and e.cpntidto=m.cpntid and m.snbid=f.snbid) where (e.cpntidfrom in (select cpntid from %s  where cpntid=%d)) or (e.cpntidto in (select cpntid from %s where cpntid=%d));end;",MICRO_CONNECTIVITY_TABLE, login,HW_CPNTS_TABLE,HW_CPNTS_TABLE,LG_CPNTS_TABLE,LG_CPNTS_TABLE,LG_CPNTS_TABLE,deviceid,LG_CPNTS_TABLE,deviceid,SQLROWCOUNT);
					status=OCIStmtPrepare(stmthp, ociError, (text*)selectdev, (ub4)strlen((char *)selectdev), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);

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
						GetErrorMess(appliName,"Error couldn't update the micro links and paths status",ErrMess,1);
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



	//next step is to define the fct any status to IN_USE: need to check that the log. device is not already In_use
	/************************************************************************/
	// return the row of the given type in devtype_result
	/***********************************************************************/
	/**
	* update a hw cpnt status whose initial status is not IN_USE to any status IN_USE,SPARE,DESTROYED,EXT_TEST.returning an integer value.
	* The user should manage the memory : there is no memory allocation. tO USE IF IT DOESN'T AFFECT HTE STATUS OF THE MOTHERBOARD.
	* @param serialnb  : serialnb of your hw
	* @param new_cpnt_status : new status of the board which was occupying this fct : IN_USE,IN_REPAIR,DESTROYED,SPARE, EXT_TEST
	* @param new_location : new location of the board if new_device_status!='IN_USE' . If new_device_status=IN_USE. Put "none"
	* @param user_comments : comments to explain why this replacement. Put "none", if you the default one :"status change in functional device with name %s " or 
	* @param status_datechange : date of the status change of the board formatted as YY/MM/DD/HH24/MI/SS
	* @param functional_cpntname : functional_name which will be occupied by the given serialnb. This is only available for new_device_status=IN_USE status and there should be no serial nb which already occupies this functional devicename otherwise use Replace fct.
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/

	/******************************************************************/
	EXTERN_CONFDB
		int UpdateHWBoardCpntStatus(char* serialnb,char* new_cpnt_status,char* new_location,char* user_comments,char* status_datechange,char* functional_cpntname,char* ErrMess)
	{
		int status_case=0;
		int rescode=0;
		char appliName[100]="UpdateHWBoardCpntStatus";
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
			if(functional_cpntname==NULL||strncmp(functional_cpntname,"none",4)==0)
				sprintf(usercomments,"status change in hw device with name %s ",serialnb);
			else
				sprintf(usercomments,"replacement of the hw  device with name %s",serialnb);
		}
		if(new_location==NULL || strncmp(new_location,"none",4)==0)
		{
			if(strncmp(new_cpnt_status,"IN_USE",6)==0)
				strcpy(newlocation,"blabla");
			else
				strcpy(newlocation,new_location);
		}
		else
			strcpy(newlocation,new_location);


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
		if(strcmp(new_cpnt_status,"DESTROYED")==0)
			status_case=DESTROYED;
		if(strcmp(new_cpnt_status,"IN_REPAIR")==0)
			status_case=IN_REPAIR;
		if(strcmp(new_cpnt_status,"SPARE")==0)
			status_case=SPARE;
		if(strcmp(new_cpnt_status,"EXT_TEST")==0)
			status_case=EXT_TEST;
		if(strcmp(new_cpnt_status,"IN_USE")==0)
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
		if(status_case==IN_USE && (functional_cpntname==NULL ||strncmp(functional_cpntname,"none",4)==0))
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
		if(status_case!=IN_USE)			
			sprintf(selectdev,"BEGIN update %s set cpnt_status=:dstatus,location=:dlocation where replacable=1 and cpnt_status!=%d and cpnt_status!=%d and serialnb=:snb; if %s=1 then insert all into %s(historycpntid, snbid,  cpnt_status, user_comment, location, status_change, created,author,terminal_name) values (%s,snbid,'%s',:usercomment,:dlocation,to_date(:statchange,'%s'), sysdate,'%s','%s') select snbid from %s where serialnb=:snb; :numrows:=%s; :rescode:=0; else :rescode:=-10; :numrows:=0;end if;END;",HW_CPNTS_TABLE,IN_USE,DESTROYED,SQLROWCOUNT,HISTORY_CPNT_TABLE,seq_name,new_cpnt_status,_date_format,login,host,HW_CPNTS_TABLE,SQLROWCOUNT);			
		else
			sprintf(selectdev,"BEGIN update %s set cpnt_status=:dstatus,location=(select serialnb from %s,%s where cpntname=:dname and motherboardid=deviceid) where replacable=1 and cpnt_status!=%d and cpnt_status!=%d and serialnb=:snb; if %s=1 then update %s set snbid=(select snbid from %s where serialnb=:snb),  last_update=sysdate,user_update='%s' where cpntname=:dname and snbid is null returning cpntid,snbid,motherboardid into :devid,:sysname,:dlocation; if %s=1 then insert all into %s(historycpntid, cpntid,snbid, cpnt_status, user_comment, location, status_change,created,author,terminal_name) values (%s,:devid,:sysname,'%s',:usercomment,location,to_date(:statchange,'%s'),sysdate,'%s','%s') select devicename as location from %s where deviceid=:dlocation;:numrows:=%s; :rescode:=0; else :rescode:=-5; :numrows:=0;end if; else :rescode:=-10; :numrows:=0; end if; END;",HW_CPNTS_TABLE,LG_CPNTS_TABLE,LOGICAL_DEVICE_TABLE,IN_USE,DESTROYED,SQLROWCOUNT,LG_CPNTS_TABLE,HW_CPNTS_TABLE,login,SQLROWCOUNT,HISTORY_CPNT_TABLE,seq_name,new_cpnt_status,_date_format,login,host,LOGICAL_DEVICE_TABLE,SQLROWCOUNT);	
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
				if(functional_cpntname!=NULL)
					status=OCIBindByName(stmthp, &bndp[6], ociError,(text*)":dname",-1,(dvoid*) functional_cpntname,strlen(functional_cpntname)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
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
					sprintf(selectdev,"begin update %s e set e.last_update=sysdate,e.user_update='%s',e.lkused=(select decode(t.cpnt_status*f.cpnt_status,1,1,0) from %s t,%s f, %s l,%s m where t.snbid=l.snbid and l.cpntid=e.cpntidfrom and e.cpntidto=m.cpntid and m.snbid=f.snbid) where (e.cpntidfrom in (select cpntid from %s  where cpntid=%d)) or (e.cpntidto in (select cpntid from %s where cpntid=%d));end;",MICRO_CONNECTIVITY_TABLE, login,HW_CPNTS_TABLE,HW_CPNTS_TABLE,LG_CPNTS_TABLE,LG_CPNTS_TABLE,LG_CPNTS_TABLE,deviceid,LG_CPNTS_TABLE,deviceid,SQLROWCOUNT);
					status=OCIStmtPrepare(stmthp, ociError, (text*)selectdev, (ub4)strlen((char *)selectdev), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);

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
	* return last functional board cpnt name occupied by the hw board cpnt (serialnb) returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param serialnb : serial nb hw board cpnt. Put 'none' if you provide a snbid because uour hw cpnt doesn't have a serialnb
	* @param snbid : snbid of hw board cpnt (provided by the DB). Put -1 if you have provided a serialnb 
	* @param functional_cpntname : last functional board cpnt name occupied by the given hw board cpnt name. We put "none" if there is not.Case where the board cpnt has always been a spare
	* @param len_functionaldname : length of the functional board cpnt name 
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/
	/******************************************************************/
	EXTERN_CONFDB
		int GetHWLastFunctionalBoardCpntName(char* serialnb,int snbid,char* functional_cpntname,int &len_functionaldname ,char* ErrMess)
	{
		char appliName[100]="GetHWLastFunctionalBoardCpntName";
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
			GetErrorMess(appliName," serialnb MUST be given",ErrMess,1);
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
			if(snbid==-1)
				sprintf(selectdev,"select nvl(t.cpntname,'none')||'?' from %s t,%s p,%s f where t.cpntid=p.cpntid and p.snbid=f.snbid and f.serialnb=:snb and p.cpnt_status='IN_USE' and p.status_change=(select max(status_change) from %s  where snbid=p.snbid  and cpnt_status='IN_USE')",LG_CPNTS_TABLE,HISTORY_CPNT_TABLE,HW_CPNTS_TABLE,HISTORY_CPNT_TABLE);
			else
				sprintf(selectdev,"select nvl(t.cpntname,'none')||'?' from %s t,%s p where t.cpntid=p.cpntid and p.snbid=:snb and p.cpnt_status='IN_USE' and p.status_change=(select max(status_change) from %s  where snbid=p.snbid  and cpnt_status='IN_USE')",LG_CPNTS_TABLE,HISTORY_CPNT_TABLE,HISTORY_CPNT_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdev,(ub4) strlen(selectdev),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else	
		{
			if(snbid==-1)
				status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":snb",-1,(dvoid*) serialnb,strlen(serialnb)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
			else
				status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":snb",-1,(dvoid*) &snbid,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
		}
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
				strcpy(functional_cpntname,devicename);

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
				if(devicename!=NULL)
					free(devicename);
				rescode=ShowErrors (status, ociError, "Fetch unsuccessful");
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrMess,1);
				strcpy(functional_cpntname,"none");
				return 0;
			}
		}
		if(devicename!=NULL)
			free(devicename);
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
	// return the status of a hw board cpnt (serialnb)
	/***********************************************************************/
	/**
	* return last hw board cpnt name occupied by the given functional cpnt name ( returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param functional_cpntname :  functional board cpntname
	* @param serialnb : last serial nb which was occupying the given functional board cpntname. Concatenation of serialnb|snbid (case where the serialnb is NULL, it is replaced by 'none')
	* @param len_serialnb : length of the functional device name 
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/
	/******************************************************************/
	EXTERN_CONFDB
		int GetFunctionalBoardCpntNameLastHW(char* functional_cpntname,char* serialnb,int &len_serialnb ,char* ErrMess)
	{
		char appliName[100]="GetFunctionalBoardCpntNameLastHW";
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
		if(functional_cpntname==NULL)
		{
			GetErrorMess(appliName," functional_cpntname MUST be given",ErrMess,1);
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
			sprintf(selectdev,"select nvl(f.serialnb,'none')||'|'||nvl(f.snbid,-1)||'?' from %s t,%s p ,%s f where t.cpntid=p.cpntid and t.cpntname=:dname and nvl(p.snbid,-1)=f.snbid and p.cpnt_status='IN_USE' and p.status_change=(select max(status_change) from %s where cpntid=p.cpntid and p.cpnt_status='IN_USE')",LG_CPNTS_TABLE,HISTORY_CPNT_TABLE,HW_CPNTS_TABLE,HISTORY_CPNT_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdev,(ub4) strlen(selectdev),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else	
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":dname",-1,(dvoid*) functional_cpntname,strlen(functional_cpntname)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

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
				if(devicename!=NULL)
					free(devicename);
				rescode=ShowErrors (status, ociError, "Fetch unsuccessful");
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrMess,1);
				strcpy(serialnb,"none");
				return 0;
			}
		}
		if(devicename!=NULL)
			free(devicename);
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
	/**
	* return the history of a functional board cpnt (functional_cpntname) returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param functional_cpntname :  functional cpntname
	* @param functionalcpnt_history : result as follows lines are separated with '\\0',one line consists of "cpntname|serialnb|snbid|status|date of change|location" 
	* @param len_history : length of the logcaldevice_history 
	* @param min_date : all changes which appear after this date (or equal to this date). Put "none", if nothing
	* @param max_date : all changes which appear before this date (or equal to this date). Put "none", if nothing.
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/
	/******************************************************************/
	EXTERN_CONFDB
		int GetHistoryOfFunctionalBoardCpntName(char* functional_cpntname,char* functionalcpnt_history,int &len_history , char* min_date, char* max_date,char* ErrMess)
	{
		char appliName[100]="GetHistoryOfFunctionalBoardCpntName";
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
		if(functional_cpntname==NULL)
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
				sprintf(selectdev,"select t.cpntname||'|'||nvl(f.serialnb,'none')||'|'||f.snbid||'|'||p.cpnt_status||'|'||to_char(p.status_change,'%s')||'|'||nvl(p.location,'none')||'?' from %s t,%s p,%s f where t.cpntid=p.cpntid and t.cpntname=:dname and f.snbid=nvl(p.snbid,-1) order by p.status_change",_date_format,LG_CPNTS_TABLE,HISTORY_CPNT_TABLE,HW_CPNTS_TABLE);
				i=7;
			}
			else
			{
				if(strncmp(min_date,"none",4)!=0 &&strncmp(max_date,"none",4)!=0)
				{
					i=6;
					sprintf(selectdev,"select t.cpntname||'|'||nvl(f.serialnb,'none')||'|'||f.snbid||'|'||p.cpnt_status||'|'||to_char(p.status_change,'%s')||'|'||nvl(p.location,'none')||'?' from %s t,%s p,%s f where t.cpntid=p.cpntid  and t.cpntname=:dname and f.snbid=nvl(p.snbid,-1) and to_char(p.status_change,'%s') between :mindate and :maxdate order by p.status_change",_date_format,LG_CPNTS_TABLE,HISTORY_CPNT_TABLE,HW_CPNTS_TABLE,_date_format);
				}
				else
				{
					if(strncmp(max_date,"none",4)==0)
					{
						sprintf(selectdev,"select t.cpntname||'|'||nvl(f.serialnb,'none')||'|'||f.snbid||'|'||p.cpnt_status||'|'||to_char(p.status_change,'%s')||'|'||nvl(p.location,'none')||'?' from %s t,%s p,%s f where t.cpntid=p.cpntid  and t.cpntname=:dname and f.snbid=nvl(p.snbid,-1) and  to_char(p.status_change,'%s')>=:mindate  order by p.status_change",_date_format,LG_CPNTS_TABLE,HISTORY_CPNT_TABLE,HW_CPNTS_TABLE,_date_format);
						i=3;
					}
					else
					{
						sprintf(selectdev,"select t.cpntname||'|'||nvl(f.serialnb,'none')||'|'||f.snbid||'|'||p.cpnt_status||'|'||to_char(p.status_change,'%s')||'|'||nvl(p.location,'none')||'?' from %s t,%s p,%s f where t.cpntid=p.cpntid  and t.cpntname=:dname and f.snbid=nvl(p.snbid,-1) and  to_char(p.status_change,'%s')<=:maxdate order by p.status_change",_date_format,LG_CPNTS_TABLE,HISTORY_CPNT_TABLE,HW_CPNTS_TABLE,_date_format);
						i=2;
					}
				}

			}
			//std::cout<<"value of selectdev "<<selectdev<<std::endl;	
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdev,(ub4) strlen(selectdev),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else	
			status=OCIBindByName(stmthp, &bnd2p, ociError,(text*)":dname",-1,(dvoid*) functional_cpntname,strlen(functional_cpntname)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

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

						memcpy(functionalcpnt_history,devicenamehist_list,pos2);
						len_history=pos2;
					}
				}
				else
				{
					rescode=ShowErrors (status, ociError, "OCIFETCH unsuccessful");
					rescode=0;
					strcpy(functionalcpnt_history,"NO_ROWS_SELECTED");
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
	* return the history of a hw board cpnt (serialnb) returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param serialnb :  serialnb of the hw. Put "none" if there is not.
	* @param snbid : snbid of the hw. Put -1 if you provide a serialnb
	* @param functionalcpnt_history : result as follows lines are separated with '\\0',one line consists of "cpntname|serialnb|snbid|status|date of change|location" 
	* @param len_history : length of the logcaldevice_history 
	* @param min_date : all changes which appear after this date (or equal to this date). Put "none", if nothing
	* @param max_date : all changes which appear before this date (or equal to this date). Put "none", if nothing.
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/

	/******************************************************************/
	EXTERN_CONFDB
		int GetHistoryOfHWBoardCpnt(char* serialnb,int snbid,char* functionalcpnt_history,int &len_history , char* min_date, char* max_date,char* ErrMess)
	{
		char appliName[100]="GetHistoryOfHWBoardCpnt";
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
			if(snbid==-1)
			{
				if(strncmp(min_date,"none",4)==0 && strncmp(max_date,"none",4)==0)
				{
					sprintf(selectdev,"select decode(t.cpntname,'motherboard','none',t.cpntname)||'|'||nvl(f.serialnb,'none')||'|'||f.snbid||'|'||p.cpnt_status||'|'||to_char(p.status_change,'%s')||'|'||nvl(p.location,'none')||'?' from %s t,%s p,%s f where t.cpntid=nvl(p.cpntid,-1) and f.serialnb=:dname and f.snbid=p.snbid order by p.status_change",_date_format,LG_CPNTS_TABLE,HISTORY_CPNT_TABLE,HW_CPNTS_TABLE);
					i=7;
				}
				else
				{
					if(strncmp(min_date,"none",4)!=0 &&strncmp(max_date,"none",4)!=0)
					{
						i=6;
						sprintf(selectdev,"select decode(t.cpntname,'motherboard','none',t.cpntname)||'|'||nvl(f.serialnb,'none')||'|'||f.snbid||'|'||p.cpnt_status||'|'||to_char(p.status_change,'%s')||'|'||nvl(p.location,'none')||'?' from %s t,%s p.%s f where t.cpntid)=nvl(p.cpntid,-1) and f.serialnb=:dname and f.snbid=p.snbid and to_char(p.status_change,'%s') between :mindate and :maxdate order by p.status_change",_date_format,LG_CPNTS_TABLE,HISTORY_CPNT_TABLE,HW_CPNTS_TABLE,_date_format);
					}
					else
					{
						if(strncmp(max_date,"none",4)==0)
						{
							sprintf(selectdev,"select decode(t.cpntname,'motherboard','none',t.cpntname)||'|'||nvl(f.serialnb,'none')||'|'||f.snbid||'|'||p.cpnt_status||'|'||to_char(p.status_change,'%s')||'|'||nvl(p.location,'none')||'?' from %s t,%s p,%s f where t.cpntid=nvl(p.cpntid,-1) and f.serialnb=:dname and f.snbid=p.snbid and to_char(p.status_change,'%s')>=:mindate order by p.status_change",_date_format,LG_CPNTS_TABLE,HISTORY_CPNT_TABLE,HW_CPNTS_TABLE,_date_format);
							i=3;
						}
						else
						{
							sprintf(selectdev,"select decode(t.cpntname,'motherboard','none',t.cpntname)||'|'||nvl(f.serialnb,'none')||'|'||f.snbid||'|'||p.cpnt_status||'|'||to_char(p.status_change,'%s')||'|'||nvl(p.location,'none')||'?' from %s t,%s p,%s f where t.cpntid=nvl(p.cpntid,-1) and f.serialnb=:dname and f.snbid=p.snbid and  to_char(p.status_change,'%s')<=:maxdate order by p.status_change",_date_format,LG_CPNTS_TABLE,HISTORY_CPNT_TABLE,HW_CPNTS_TABLE,_date_format);
							i=2;
						}
					}

				}
			}
			else
			{
				if(strncmp(min_date,"none",4)==0 && strncmp(max_date,"none",4)==0)
				{
					sprintf(selectdev,"select decode(t.cpntname,'motherboard','none',t.cpntname)||'|'||nvl(f.serialnb,'none')||'|'||f.snbid||'|'||p.cpnt_status||'|'||to_char(p.status_change,'%s')||'|'||nvl(p.location,'none')||'?' from %s t,%s p,%s f where t.cpntid=nvl(p.cpntid,-1) and f.snbid=:dname and f.snbid=p.snbid order by p.status_change",_date_format,LG_CPNTS_TABLE,HISTORY_CPNT_TABLE,HW_CPNTS_TABLE);
					i=7;
				}
				else
				{
					if(strncmp(min_date,"none",4)!=0 &&strncmp(max_date,"none",4)!=0)
					{
						i=6;
						sprintf(selectdev,"select decode(t.cpntname,'motherboard','none',t.cpntname)||'|'||nvl(f.serialnb,'none')||'|'||f.snbid||'|'||p.cpnt_status||'|'||to_char(p.status_change,'%s')||'|'||nvl(p.location,'none')||'?' from %s t,%s p.%s f where t.cpntid)=nvl(p.cpntid,-1) and f.snbid=:dname and f.snbid=p.snbid and to_char(p.status_change,'%s') between :mindate and :maxdate order by p.status_change",_date_format,LG_CPNTS_TABLE,HISTORY_CPNT_TABLE,HW_CPNTS_TABLE,_date_format);
					}
					else
					{
						if(strncmp(max_date,"none",4)==0)
						{
							sprintf(selectdev,"select decode(t.cpntname,'motherboard','none',t.cpntname)||'|'||nvl(f.serialnb,'none')||'|'||f.snbid||'|'||p.cpnt_status||'|'||to_char(p.status_change,'%s')||'|'||nvl(p.location,'none')||'?' from %s t,%s p,%s f where t.cpntid=nvl(p.cpntid,-1) and f.snbid=:dname and f.snbid=p.snbid and to_char(p.status_change,'%s') >=:mindate order by p.status_change",_date_format,LG_CPNTS_TABLE,HISTORY_CPNT_TABLE,HW_CPNTS_TABLE,_date_format);
							i=3;
						}
						else
						{
							sprintf(selectdev,"select decode(t.cpntname,'motherboard','none',t.cpntname)||'|'||nvl(f.serialnb,'none')||'|'||f.snbid||'|'||p.cpnt_status||'|'||to_char(p.status_change,'%s')||'|'||nvl(p.location,'none')||'?' from %s t,%s p,%s f where t.cpntid=nvl(p.cpntid,-1) and f.snbid=:dname and f.snbid=p.snbid and  to_char(p.status_change,'%s')<=:maxdate order by p.status_change",_date_format,LG_CPNTS_TABLE,HISTORY_CPNT_TABLE,HW_CPNTS_TABLE,_date_format);
							i=2;
						}
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
		{
			if(snbid==-1)
				status=OCIBindByName(stmthp, &bnd2p, ociError,(text*)":dname",-1,(dvoid*) serialnb,strlen(serialnb)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
			else
				status=OCIBindByName(stmthp, &bnd2p, ociError,(text*)":dname",-1,(dvoid*) &snbid,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
		}
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

						memcpy(functionalcpnt_history,devicenamehist_list,pos2);
						len_history=pos2;
					}
				}
				else
				{
					rescode=ShowErrors (status, ociError, "OCIFETCH unsuccessful");
					rescode=0;
					strcpy(functionalcpnt_history,"NO_ROWS_SELECTED");
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
	* give the list of hw board cpnt which are in status= (serialnb) returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param cpnt_status :  status of the hw board cpnt 
	* @param functionalcpnt_status : result as follows list of the serialnb|snbid are separated with '\\0', 
	* @param len_status : length of the functionalcpnt_status 
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/

	/******************************************************************/
	EXTERN_CONFDB
		int GetHWBoardCpntByStatus(char* cpnt_status,char* functionalcpnt_status,int &len_status , char* ErrMess)
	{
		char appliName[100]="GetHWBoardCpntByStatus";
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
		int status_case=0;
		if(strcmp(cpnt_status,"DESTROYED")==0)
			status_case=DESTROYED;
		if(strcmp(cpnt_status,"IN_REPAIR")==0)
			status_case=IN_REPAIR;
		if(strcmp(cpnt_status,"SPARE")==0)
			status_case=SPARE;
		if(strcmp(cpnt_status,"EXT_TEST")==0)
			status_case=EXT_TEST;
		if(strcmp(cpnt_status,"TEST")==0)
			status_case=TEST;
		if(strcmp(cpnt_status,"IN_USE")==0)
			status_case=IN_USE;
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
			sprintf(selectdev,"select nvl(serialnb,'none')||'|'||snbid||'?' from %s  where cpnt_status=:dstatus  ",HW_CPNTS_TABLE);

			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdev,(ub4) strlen(selectdev),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else	
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":dstatus",-1,(dvoid*) &status_case,sizeof(int), SQLT_INT,(dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);


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
					//devicenamehist_list[pos2-1]='\0';
					if((pos2)>len_status)
					{
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

						memcpy(functionalcpnt_status,devicenamehist_list,pos2);
						len_status=pos2;
					}
				}
				else
				{
					rescode=0;
					strcpy(functionalcpnt_status,"NO_ROWS_SELECTED");
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
