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
	// delete the row of the given lkid permanently
	/***********************************************************************/
	/**
	* delete a link returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param linkid : linkid of the link.
	* @param macro_link : 1 if it's a macro link (i.e between boards), 0 if it's a micro link.
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/
	EXTERN_CONFDB
		int DeleteLinkRow(int linkid,int macro_link,char* ErrMess)
	{
		char appliName[100]="DeleteLinkRow";
		int numrows=1;
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[5]; 
		int ptoid=0;
		int pfromid=0;
		int i=0;
		int rescode=0;
		int resquery=-1;
		sword status;
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
			GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
			free(errmessg1);
			return -1;
		}

		int res_query=getTerminalName(host,len_host,errmessg1);
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
			return -1;
		}
		//need to proceed with messages

		status =OCIHandleAlloc (ociEnv, (void**)&hstmt, OCI_HTYPE_STMT , 0, 0);
		if(status!=OCI_SUCCESS)
		{
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrMess,1); 
			return -1;
		}
		else
		{
			if(macro_link==1)
				sprintf(sqlstmt,"begin delete from %s where linkid=:lkid returning portidfrom,portidto into :pfrom,:pto; if %s=1 then :rescode:=%s(:pfrom,:pto); else :numrows:=0; end if; end;",MACRO_CONNECTIVITY_TABLE,SQLROWCOUNT,_DeleteLinksInPaths);
			else
				sprintf(sqlstmt,"begin delete from %s where linkid=:lkid returning cpntidfrom,cpntidto into :pfrom,:pto; if %s=1 then :rescode:=0; else :numrows:=0; end if; end;",MICRO_CONNECTIVITY_TABLE,SQLROWCOUNT);

			status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
		{	

			status =OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":lkid", -1,(dvoid*)&linkid, sizeof(int),  SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
		}		
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status = OCIBindByName (hstmt, &bndp[1], ociError,(text*) ":pfrom", -1,(dvoid*)&pfromid,sizeof(int), SQLT_INT, 0, 0, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status = OCIBindByName (hstmt, &bndp[2], ociError, (text*) ":pto", -1,(dvoid*)&ptoid,sizeof(int), SQLT_INT, 0, 0, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else	
			status = OCIBindByName(hstmt, &bndp[3], ociError, (text*) ":rescode", -1,(dvoid*)&resquery,sizeof(int), SQLT_INT, (dvoid*) 0, 0, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else	
			status = OCIBindByName(hstmt, &bndp[4], ociError, (text*) ":numrows", -1,(dvoid*)&numrows,sizeof(int), SQLT_INT, (dvoid*) 0, 0, 0, 0, 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status= OCIStmtExecute(ociHdbc, hstmt, ociError, 1, 0, 0, 0, OCI_DEFAULT );

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
			//even if it's not good for performance, we commit the transaction one by one... for consistency reasons.
			if(numrows==0)
			{
				status = OCITransCommit(ociHdbc, ociError, 0);
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				GetErrorMess(appliName,  "LINKID DOESN'T EXIST",ErrMess,1);
				return -1;
			}
			else
			{
				if(resquery==0)
					status = OCITransCommit(ociHdbc, ociError, 0);
				else
				{
					status = OCITransRollback(ociHdbc, ociError, 0);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					GetErrorMess(appliName,  "ERROR in deleting paths",ErrMess,1);
					return -1;
				}

			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCITransCommit unsuccessful");
			}	

		}
		status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);

		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 
		else
			OCIReportError(ociError,appliName, ErrMess,0); 
		return rescode;
	}

	/************************************************************************/
	// delete a port row from the functional port table and hw port table if the port has a mac address
	/***********************************************************************/
	/**
	* delete a link returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param portid : portid of the prot
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/
	EXTERN_CONFDB
		int DeletePortRow(int portid,char* ErrMess)
	{
		char appliName[100]="DeletePortRow";
		int numrows=1;
		int numrows_bis=1;
		char sqlstmt[10000];
		OCIStmt *hstmt;
		OCIBind  *bndp[11]; 
		int ptoid=0;
		int pfromid=0;
		int iplen=20;
		int i=0;
		int rescode=0;
		int resquery=-1;
		sword status;
		int len_host=LOGIN_LEN;
		char login[LOGIN_LEN];
		char host[LOGIN_LEN];
		int devid=0;
		int pway=0;
		int len_string=101;
		char ptype[100]="blfsl";
		char pnb[100]="jfsk";
		char ipaddress[20]="bbbbbbb";
		char* errmessg1=(char*)malloc(ERROR_BUFFER*sizeof(char));

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

		int res_query=getTerminalName(host,len_host,errmessg1);
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
			return -1;
		}
		//need to proceed with messages

		status =OCIHandleAlloc (ociEnv, (void**)&hstmt, OCI_HTYPE_STMT , 0, 0);
		if(status!=OCI_SUCCESS)
		{
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrMess,1); 
			return -1;
		}
		else
		{

			//sprintf(sqlstmt,"begin delete from %s where portidfrom=:pid or portidto=:pid returning portidfrom,portidto into :pfrom,:pto; if %s=1 then :rescode:=%s(:pfrom,:pto); else :numrows:=0; end if; delete from %s where portid=:pid returning nvl(port_type,'none'),port_nbr,port_way,deviceid,nvl(ipaddress,'none') into :ptype,:pnb,:pway,:devid,:ipadd ;if %s=1 then delete from %s where port_nbr=:pnb and port_way=:pway and nvl(port_type,'none')=:ptype and serialnb=(select serialnb from %s where deviceid=:devid); if :ipadd!='none' then  delete from %s g where g.ipaddress=:ipadd and g.ipaddress not in (select ipaddress from %s where ipaddress=:ipadd); delete from %s t where t.ipaddress=:ipadd and t.ipaddress not in (select e.ipaddress from %s e); end if;else :numrows_bis:=0; end if; end;",MACRO_CONNECTIVITY_TABLE,SQLROWCOUNT,_DeleteLinksInPaths,PORT_TABLE,SQLROWCOUNT,HWPORT_TABLE,LOGICAL_DEVICE_TABLE,IPALIAS_TABLE, PORT_TABLE,IPINFO_TABLE,PORT_TABLE);
			sprintf(sqlstmt,"begin delete from %s where portidfrom=:pid or portidto=:pid returning portidfrom,portidto into :pfrom,:pto; if %s=1 then :rescode:=%s(:pfrom,:pto); else :numrows:=0; end if; delete from %s where portidfrom=:pid or portidto=:pid; delete from %s where portid=:pid returning nvl(port_type,'none'),port_nbr,port_way,deviceid,nvl(ipaddress,'none') into :ptype,:pnb,:pway,:devid,:ipadd ;if %s=1 then delete from %s where port_nbr=:pnb and port_way=:pway and nvl(port_type,'none')=:ptype and serialnb=(select serialnb from %s where deviceid=:devid); if :ipadd!='none' then  delete from %s g where g.ipaddress=:ipadd and g.ipaddress not in (select ipaddress from %s where ipaddress=:ipadd); delete from %s t where t.ipaddress=:ipadd and t.ipaddress not in (select e.ipaddress from %s e); end if;else :numrows_bis:=0; end if; end;",MACRO_CONNECTIVITY_TABLE,SQLROWCOUNT,_DeleteLinksInPaths,MICRO_CONNECTIVITY_TABLE,PORT_TABLE,SQLROWCOUNT,HWPORT_TABLE,LOGICAL_DEVICE_TABLE,IPALIAS_TABLE, PORT_TABLE,IPINFO_TABLE,PORT_TABLE);

      status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
		{	

			status =OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":pid", -1,(dvoid*)&portid, sizeof(int),  SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
		}		
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status = OCIBindByName (hstmt, &bndp[1], ociError,(text*) ":pfrom", -1,(dvoid*)&pfromid,sizeof(int), SQLT_INT, 0, 0, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status = OCIBindByName (hstmt, &bndp[2], ociError, (text*) ":pto", -1,(dvoid*)&ptoid,sizeof(int), SQLT_INT, 0, 0, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else	
			status = OCIBindByName(hstmt, &bndp[3], ociError, (text*) ":rescode", -1,(dvoid*)&resquery,sizeof(int), SQLT_INT, (dvoid*) 0, 0, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else	
			status = OCIBindByName(hstmt, &bndp[4], ociError, (text*) ":numrows", -1,(dvoid*)&numrows,sizeof(int), SQLT_INT, (dvoid*) 0, 0, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else	
			status = OCIBindByName(hstmt, &bndp[5], ociError, (text*) ":devid", -1,(dvoid*)&devid,sizeof(int), SQLT_INT, (dvoid*) 0, 0, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else	
			status = OCIBindByName(hstmt, &bndp[6], ociError, (text*) ":pway", -1,(dvoid*)&pway,sizeof(int), SQLT_INT, (dvoid*) 0, 0, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else	
			status = OCIBindByName(hstmt, &bndp[7], ociError, (text*) ":ptype", -1,(dvoid*)ptype,len_string, SQLT_STR, (dvoid*) 0, 0, 0, 0, 0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else	
			status = OCIBindByName(hstmt, &bndp[8], ociError, (text*) ":pnb", -1,(dvoid*)pnb,len_string, SQLT_STR, (dvoid*) 0, 0, 0, 0, 0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else	
			status = OCIBindByName(hstmt, &bndp[9], ociError, (text*) ":numrows_bis", -1,(dvoid*)&numrows_bis,sizeof(int), SQLT_INT, (dvoid*) 0, 0, 0, 0, 0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else	
			status = OCIBindByName(hstmt, &bndp[10], ociError, (text*) ":ipadd", -1,(dvoid*)ipaddress,iplen, SQLT_STR, (dvoid*) 0, 0, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status= OCIStmtExecute(ociHdbc, hstmt, ociError, 1, 0, 0, 0, OCI_DEFAULT );

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
			//even if it's not good for performance, we commit the transaction one by one... for consistency reasons.
			if(numrows_bis==0)
			{
				status = OCITransCommit(ociHdbc, ociError, 0);
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				GetErrorMess(appliName,  "PORTID DOESN'T EXIST",ErrMess,1);
				return -1;
			}
			else
			{
				if(numrows==0)
				{
					status = OCITransCommit(ociHdbc, ociError, 0);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					sprintf(ErrMess,"NO_ERROR in %s but no link was deleted",appliName);
					return 0;
				}
				else
				{
					if(resquery==0)
						status = OCITransCommit(ociHdbc, ociError, 0);
					else
					{
						status = OCITransRollback(ociHdbc, ociError, 0);
						status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
						GetErrorMess(appliName,  "ERROR in deleting port: couldn't delete paths",ErrMess,1);
						return -1;
					}
				}

			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCITransCommit unsuccessful");
			}	

		}
		status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);

		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 
		else
			OCIReportError(ociError,appliName, ErrMess,0); 
		return rescode;
	}

	/************************************************************************/
	// delete the row of the given ipalias permanently
	/***********************************************************************/
	/**
	* delete an ipalias returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param ipalias : ipalias to delete .
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/
	EXTERN_CONFDB
		int DeleteIPAlias(char* ipalias,char* ErrMess)
	{
		char appliName[100]="DeleteIPAlias";
		int numrows=1;
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[2]; 
		int ptoid=0;
		int pfromid=0;
		int i=0;
		int rescode=0;
		int resquery=-1;
		sword status;
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
			GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
			free(errmessg1);
			return -1;
		}

		int res_query=getTerminalName(host,len_host,errmessg1);
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
			return -1;
		}
		//need to proceed with messages

		status =OCIHandleAlloc (ociEnv, (void**)&hstmt, OCI_HTYPE_STMT , 0, 0);
		if(status!=OCI_SUCCESS)
		{
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrMess,1); 
			return -1;
		}
		else
		{

			sprintf(sqlstmt,"begin delete from %s where ipalias=:ipalias;  :numrows:=%s;  end;",IPALIAS_TABLE,SQLROWCOUNT);

			status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
		{	

			status =OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":ipalias", -1,(dvoid*)ipalias, strlen(ipalias)+1,  SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
		}		


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else	
			status = OCIBindByName(hstmt, &bndp[1], ociError, (text*) ":numrows", -1,(dvoid*)&numrows,sizeof(int), SQLT_INT, (dvoid*) 0, 0, 0, 0, 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status= OCIStmtExecute(ociHdbc, hstmt, ociError, 1, 0, 0, 0, OCI_DEFAULT );

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
			//even if it's not good for performance, we commit the transaction one by one... for consistency reasons.
			if(numrows==0)
			{
				status = OCITransCommit(ociHdbc, ociError, 0);
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				GetErrorMess(appliName,  "IPALIAS DOESN'T EXIST",ErrMess,1);
				return -1;
			}
			else
			{

				status = OCITransCommit(ociHdbc, ociError, 0);


			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCITransCommit unsuccessful");
			}	

		}
		status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);

		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 
		else
			OCIReportError(ociError,appliName, ErrMess,0); 
		return rescode;
	}

	/************************************************************************/
	// delete the row of the given functional device permanently : the functional device should not contain any microscopic cpnt!!!!
	/***********************************************************************/
	/**
	* delete a functional device returning an integer value, to be used in case of error in the device names. Ports of the device must be deleted before using this function
	* will delete also the hw device associated to it and the entry in the history table (should appear only once otheriwse we don't delete the device..)
	* The user should manage the memory : there is no memory allocation.
	* @param deviceid : deviceid to delete .
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/
	EXTERN_CONFDB
		int DeleteFunctionalDevice(int deviceid,char* ErrMess)
	{
		char appliName[100]="DeleteFunctionalDevice";
		int numrows=1;
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[4]; 
		int ptoid=0;
		int pfromid=0;
		int i=0;
		int rescode=0;
		int numrows1=0;
		int resquery=-1;
		sword status;
		int len_host=LOGIN_LEN;
		char login[LOGIN_LEN];
		char host[LOGIN_LEN];
		char serialnb[LOGIN_LEN];

		char* errmessg1=(char*)malloc(ERROR_BUFFER*sizeof(char));

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

		int res_query=getTerminalName(host,len_host,errmessg1);
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
			return -1;
		}
		//need to proceed with messages

		status =OCIHandleAlloc (ociEnv, (void**)&hstmt, OCI_HTYPE_STMT , 0, 0);
		if(status!=OCI_SUCCESS)
		{
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrMess,1); 
			return -1;
		}
		else
		{

			sprintf(sqlstmt,"begin delete from %s where deviceid=:devid and status_change=(select min(status_change) from %s where deviceid=:devid);delete from %s where deviceid=:devid returning serialnb into :snb; :numrows_first:=%s; delete from %s where serialnb=:snb;  :numrows:=%s;  end;",HISTORY_DEVICE_TABLE,HISTORY_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,SQLROWCOUNT,HW_DEVICE_TABLE,SQLROWCOUNT);

			status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
		{	

			status =OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":snb", -1,(dvoid*)serialnb, LOGIN_LEN+1,  SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
		}		


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else	
			status = OCIBindByName(hstmt, &bndp[1], ociError, (text*) ":numrows", -1,(dvoid*)&numrows,sizeof(int), SQLT_INT, (dvoid*) 0, 0, 0, 0, 0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else	
			status = OCIBindByName(hstmt, &bndp[2], ociError, (text*) ":devid", -1,(dvoid*)&deviceid,sizeof(int), SQLT_INT, (dvoid*) 0, 0, 0, 0, 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else	
			status = OCIBindByName(hstmt, &bndp[3], ociError, (text*) ":numrows_first", -1,(dvoid*)&numrows1,sizeof(int), SQLT_INT, (dvoid*) 0, 0, 0, 0, 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status= OCIStmtExecute(ociHdbc, hstmt, ociError, 1, 0, 0, 0, OCI_DEFAULT );

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
			//even if it's not good for performance, we commit the transaction one by one... for consistency reasons.
			if(numrows1==0)
			{
				status = OCITransCommit(ociHdbc, ociError, 0);
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				GetErrorMess(appliName,  "DEVICEID DOESN'T EXIST",ErrMess,1);
				return -1;
			}
			else
			{
				if(numrows==0)
				{

					status = OCITransCommit(ociHdbc, ociError, 0);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					sprintf(ErrMess,"NO_ERROR in %s but no hw device was deleted",appliName);
					return 0;

				}
				else
					status = OCITransCommit(ociHdbc, ociError, 0);


			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCITransCommit unsuccessful");
			}	

		}
		status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);

		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 
		else
			OCIReportError(ociError,appliName, ErrMess,0); 
		return rescode;
	}

	/***********************************************************************/
	/**
	* delete a functional device type returning an integer value, to be used in case of error in the name of the device type. Devices of this type  must be deleted before using this function

	* The user should manage the memory : there is no memory allocation.
	* @param devicetype: devicetype to delete .
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/
	EXTERN_CONFDB
		int DeleteFunctionalDeviceType(char* devicetype,char* ErrMess)
	{
		char appliName[100]="DeleteFunctionalDeviceType";
		int numrows=1;
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[5]; 
		int ptoid=0;
		int pfromid=0;
		int i=0;
		int rescode=0;

		int resquery=-1;
		sword status;
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
			GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
			free(errmessg1);
			return -1;
		}

		int res_query=getTerminalName(host,len_host,errmessg1);
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
			return -1;
		}
		//need to proceed with messages

		status =OCIHandleAlloc (ociEnv, (void**)&hstmt, OCI_HTYPE_STMT , 0, 0);
		if(status!=OCI_SUCCESS)
		{
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrMess,1); 
			return -1;
		}
		else
		{

			sprintf(sqlstmt,"begin delete from %s where devicetype=:devtype;  :numrows:=%s;  end;",DEVICETYPE_TABLE,SQLROWCOUNT);

			status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
		{	

			status =OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":devtype", -1,(dvoid*)devicetype, strlen(devicetype)+1,  SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
		}		


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else	
			status = OCIBindByName(hstmt, &bndp[1], ociError, (text*) ":numrows", -1,(dvoid*)&numrows,sizeof(int), SQLT_INT, (dvoid*) 0, 0, 0, 0, 0, OCI_DEFAULT);



		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status= OCIStmtExecute(ociHdbc, hstmt, ociError, 1, 0, 0, 0, OCI_DEFAULT );

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
			//even if it's not good for performance, we commit the transaction one by one... for consistency reasons.
			if(numrows==0)
			{
				status = OCITransCommit(ociHdbc, ociError, 0);
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				GetErrorMess(appliName,  "DEVICETYPE DOESN'T EXIST",ErrMess,1);
				return -1;
			}
			else
			{

				status = OCITransCommit(ociHdbc, ociError, 0);


			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCITransCommit unsuccessful");
			}	

		}
		status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);

		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 
		else
			OCIReportError(ociError,appliName, ErrMess,0); 
		return rescode;
	}

	/***********************************************************************/
	/**
	* delete a hw device (its status should be different from IN_USE)returning an integer value, to be used in case of error in the device. 
	* the hw device should not contain any microscopic components
	* The user should manage the memory : there is no memory allocation.
	* @param serialnb: devicetype to delete .
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/
	EXTERN_CONFDB
		int DeleteHWDevice(char* serialnb,char* ErrMess)
	{
		char appliName[100]="DeleteHWDevice";
		int numrows=1;
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[5]; 
		int ptoid=0;
		int pfromid=0;
		int i=0;
		int rescode=0;

		int resquery=-1;
		sword status;
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
			GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
			free(errmessg1);
			return -1;
		}

		int res_query=getTerminalName(host,len_host,errmessg1);
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
			return -1;
		}
		//need to proceed with messages

		status =OCIHandleAlloc (ociEnv, (void**)&hstmt, OCI_HTYPE_STMT , 0, 0);
		if(status!=OCI_SUCCESS)
		{
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrMess,1); 
			return -1;
		}
		else
		{

			sprintf(sqlstmt,"begin delete from %s where serialnb=:snb and status_change=(select min(status_change) from %s where serialnb=:snb); delete from %s where serialnb=:snb;  :numrows:=%s;  end;",HISTORY_DEVICE_TABLE,HISTORY_DEVICE_TABLE,HW_DEVICE_TABLE,SQLROWCOUNT);

			status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
		{	

			status =OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":snb", -1,(dvoid*)serialnb, strlen(serialnb)+1,  SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
		}		


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else	
			status = OCIBindByName(hstmt, &bndp[1], ociError, (text*) ":numrows", -1,(dvoid*)&numrows,sizeof(int), SQLT_INT, (dvoid*) 0, 0, 0, 0, 0, OCI_DEFAULT);



		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status= OCIStmtExecute(ociHdbc, hstmt, ociError, 1, 0, 0, 0, OCI_DEFAULT );

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
			//even if it's not good for performance, we commit the transaction one by one... for consistency reasons.
			if(numrows==0)
			{
				status = OCITransCommit(ociHdbc, ociError, 0);
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				GetErrorMess(appliName,  "HW device DOESN'T EXIST",ErrMess,1);
				return -1;
			}
			else
			{

				status = OCITransCommit(ociHdbc, ociError, 0);


			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCITransCommit unsuccessful");
			}	

		}
		status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);

		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 
		else
			OCIReportError(ociError,appliName, ErrMess,0); 
		return rescode;
	}

	/************************************************************************/
	// delete the row of the given system name permanently: make sure that no devices are part of this subsystem
	/***********************************************************************/
	/**
	* delete a system name returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param system name : name of the system to delete .
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/
	EXTERN_CONFDB
		int DeleteSystemName(char* sysname,char* ErrMess)
	{
		char appliName[100]="DeleteSystemName";
		int numrows=1;
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[2]; 
		int ptoid=0;
		int pfromid=0;
		int i=0;
		int rescode=0;
		int resquery=-1;
		sword status;


		//need to proceed with messages

		status =OCIHandleAlloc (ociEnv, (void**)&hstmt, OCI_HTYPE_STMT , 0, 0);
		if(status!=OCI_SUCCESS)
		{
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrMess,1); 
			return -1;
		}
		else
		{

			sprintf(sqlstmt,"begin delete from %s e where e.system_name=:sysname and e.systemid not in (select l.systemid from %s t,%s l where rownum=1 and mod(t.system_name,l.systemid)=0 and t.system_name>1 and l.system_name=:sysname);  :numrows:=%s;  end;",SUBSYSTEM_TABLE,LOGICAL_DEVICE_TABLE,SUBSYSTEM_TABLE,SQLROWCOUNT);

			status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
		{	

			status =OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":sysname", -1,(dvoid*)sysname, strlen(sysname)+1,  SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
		}		


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else	
			status = OCIBindByName(hstmt, &bndp[1], ociError, (text*) ":numrows", -1,(dvoid*)&numrows,sizeof(int), SQLT_INT, (dvoid*) 0, 0, 0, 0, 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status= OCIStmtExecute(ociHdbc, hstmt, ociError, 1, 0, 0, 0, OCI_DEFAULT );

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
			//even if it's not good for performance, we commit the transaction one by one... for consistency reasons.
			if(numrows==0)
			{
				status = OCITransCommit(ociHdbc, ociError, 0);
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				GetErrorMess(appliName,  "SYSTEM_NAME DOESN'T EXIST OR SYSTEM_NAME STILL IN USE BY DEVICES",ErrMess,1);
				return -1;
			}
			else
			{

				status = OCITransCommit(ociHdbc, ociError, 0);


			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCITransCommit unsuccessful");
			}	

		}
		status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);

		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 
		else
			OCIReportError(ociError,appliName, ErrMess,0); 
		return rescode;
	}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif
