/********************************************************************************/
//	Author: L.Abadie
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


	/*****************************************************************************************/
	/**
	* Get the state of  subsystemnames (free or active...)   an integer value known from the TFC system.
	* The user should manage the memory : there is no memory allocation.
	* @param len_array1 : length of the ListSubsystemUsed.
	* @param ListSubsystemUsed : list of subsystem names you want to know the state (free or not), names between '' and separated with a coma 'TFC','DAQ'. Put "ALL", if you want all of them.
	* @param len_array : length of the BusySubsystemsList. If the buffer is too small, we put the size needed here.
	* @param ListSubsystemState :  int* buffer where to put the result (0 means free, 1 means booked)
	* @param ListSubsystemUsed1 : list of subsystem
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
	EXTERN_CONFDB
		int GetBusySubsystems(int &len_array1,char* ListSubsystemUsed,char* ListSubsystemUsed1,int &len_array, int* ListSubsystemState, char* ErrorMessage)
	{
		char appliName[100]="GetBusySubsystems";
		int i = 0;
		int j=0;
		int rescode=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		int pos1=0;
		int pos3=0;
		int actual_len=0;
		int pos2=0;
		int len_devname=0;
		char* devName=NULL;
		char* devIDs_temp=NULL;
		sword status;
		int nline=0;
		int nb_rdtrip=1;
		int prefetch_rows=100;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def[2];
		OCIParam *parmdp;

		char selectdevID[1000];
		int lenarray=1000;
		int state_res=0;
		//std::cout<<" entering fct "<<ListSubsystemUsed<<std::endl;
		if(strcspn(ListSubsystemUsed,";")<strlen(ListSubsystemUsed))
		{
			GetErrorMess(appliName,"INVALID SYSTEMLIST",ErrorMessage,1);
			return -1;
		}
		//char* SubList=(char*)malloc(lenarray*sizeof(char));
		//if(SubList!=NULL)
		//{
		//	status=GetSubsystemList(SubList,lenarray,ListSubsystemUsed,len_array1);

		//}
		//else
		//{
		//	GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
		//	return -1;	
		//}
		//std::cout<<"sublist values" <<SubList<<std::endl;
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
		else
		{
			if(strncmp("ALL",ListSubsystemUsed,3)==0)
				sprintf(selectdevID,"select t.system_name||'|',max(e.active) from %s t,%s e where  mod(e.system_name,t.systemid)=0  group by t.system_name ",SUBSYSTEM_TABLE,LOGICAL_DEVICE_TABLE,ListSubsystemUsed);
			else
				sprintf(selectdevID,"select t.system_name||'|',max(e.active) from %s t,%s e where  mod(e.system_name,t.systemid)=0 and t.system_name in (%s) group by t.system_name ",SUBSYSTEM_TABLE,LOGICAL_DEVICE_TABLE,ListSubsystemUsed);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevID,(ub4) strlen(selectdevID),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			//std::cout<<"selectdevID="<<selectdevID<<std::endl;
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
				rescode=ShowErrors (status, ociError, "OCIAttrSet unsuccessful");
		}
		else
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
			/* The next two statements describe the select-list item, dept, and return its length to allocate the memory*/
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 1);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet1 unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &len_devname, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else
		{
			devName=(char*)malloc((len_devname+1)*sizeof(char));
			if(devName!=NULL)
				status=OCIDefineByPos (stmthp, &def[0], ociError, 1,(ub1*) devName,len_devname+1, SQLT_STR, 0, 0, 0, OCI_DEFAULT);
			else
			{

				rescode=ShowErrors (status, ociError, "pointer allocation unsuccessful");
				rescode=-1;

				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;	
			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)
					rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
			}
			else
				status=OCIDefineByPos (stmthp, &def[1], ociError, 2,(ub1*) &state_res,sizeof(int), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
		{

			pos2=0;
			pos3=0;
			i=0;
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);
				if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
				{
					pos1=strcspn(devName,"|");
					devName[pos1]='\0';
					pos3=pos2;
					pos2+=pos1+1;

					//std::cout<<"devname= "<<devName<<" and pos1="<<pos1<<" and pos2="<<pos2<<std::endl;
					//actual_len+=pos1+1;
					devIDs_temp=(char*) realloc(devIDs_temp,pos2*sizeof(char));
					ListSubsystemState[i]=state_res;
					if(devIDs_temp==NULL)
					{
						status=-10;
						rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
						rescode=-1;
						free(devName);

						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
						return rescode;
					}
					else
					{
						memcpy(devIDs_temp+pos3,devName,pos1+1);
						i++;
					}
				}
			}

			if(rescode==0)
			{	

				if(pos2>len_array1)
				{
					//devIDs_temp[pos2-2]='\0';
					len_array1=pos2;
					devIDs_temp=(char*) realloc(devIDs_temp,0*sizeof(char));
					rescode=-1;
					free(devName);

					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrorMessage,1);
					return rescode;
				}
				else
				{
					if(i>0)
					{

						memcpy(ListSubsystemUsed1,devIDs_temp,pos2);
						len_array1=pos2;

					}
					else
					{
						rescode=ShowErrors (status, ociError, "OCIFETCH unsuccessful");
						rescode=0;
						strcpy(ListSubsystemUsed1,"NO_ROWS_SELECTED");
						GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrorMessage,1);
					}
				}
				free(devIDs_temp);
				free(devName);

			}
		}

		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}

		return rescode;
	}

	/*****************************************************************************************/
	/**
	* Get the state of  subsystemnames (free or active...)   an integer value known from the TFC system.
	* The user should manage the memory : there is no memory allocation.
	* @param len_array1 : length of the ListSubsystemUsed.
	* @param ListSubsystemUsed : list of subsystem names you want to know the state (free or not), names between '' and separated with a coma 'TFC','DAQ'. Put "ALL", if you want all of them.
	* @param len_array : length of the BusySubsystemsList. If the buffer is too small, we put the size needed here.
	* @param ListSubsystemState :  int* buffer where to put the result (0 means free, 1 means booked)
	* @param ListSubsystemUsed1 : list of subsystem
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/
	EXTERN_CONFDB
		int GetDevicesPerSystem(char* system_name,int method_used,int &len_array, int* ListSubsystemState, char* ErrorMessage)
	{
		char appliName[100]="GetDevicesPerSystem";
		int i = 0;
		int j=0;
		int rescode=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		int pos1=0;
		int pos3=0;
		int actual_len=0;
		int pos2=0;
		int len_devname=0;
		char* devName=NULL;
		char* devIDs_temp=NULL;
		sword status;
		int nline=0;
		int nb_rdtrip=1;
		int prefetch_rows=100;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def[5];
		OCIParam *parmdp;

		char selectdevID[3000];
		int lenarray=1000;
		int state_res=0;

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
		else
		{
			//if(method_used==0)
			//	sprintf(selectdevID,"select t.devicename||'?' from %s e,%s t where  mod(t.system_name,e.systemid)=0  and e.system_name=:sysname ",SUBSYSTEM_TABLE,LOGICAL_DEVICE_TABLE);
			//else
			//	sprintf(selectdevID,"select t.devicename from %s e,%s t,lhcb_device_system l   where  t.deviceid=l.deviceid and l.systemid=e.systemid and e.system_name=:sysname ",SUBSYSTEM_TABLE,LOGICAL_DEVICE_TABLE);
			if(method_used==0)
				sprintf(selectdevID,"select t.linkid,g.deviceid,f.deviceid,g.port_nbr||'|'||nvl(g.port_type,'none')||'?',f.port_nbr||'|'||nvl(f.port_type,'none')||'?',t.link_type,t.bidirectional_link_used,t.lkused,t.link_weight,m.devicename||'?',n.devicename||'?',nvl(t.link_info,'none')||'?' from %s t,%s g,%s f, %s l,%s m,%s n where t.lkused=1 and g.deviceid=m.deviceid and f.deviceid=n.deviceid and t.portidfrom=g.portid and t.portidto=f.portid and mod(l.system_name,t.system_name)*mod(t.system_name,l.system_name)=0 and (l.devicename=:dfrom  ) union select -t.linkid,f.deviceid,g.deviceid,f.port_nbr||'|'||nvl(f.port_type,'none')||'?',g.port_nbr||'|'||nvl(g.port_type,'none')||'?',t.link_type,t.bidirectional_link_used,t.lkused,decode(t.link_weight,1,2,2,1,0),n.devicename||'?',m.devicename||'?',nvl(t.link_info,'none')||'?' from %s t,%s g,%s f,%s m,%s n,%s l where t.lkused=1 and g.deviceid=m.deviceid and f.deviceid=n.deviceid and t.portidfrom=g.portid and t.portidto=f.portid and mod(l.system_name,t.system_name)*mod(t.system_name,l.system_name)=0 and (l.devicename=:dfrom  ) and t.bidirectional_link_used=1",MACRO_CONNECTIVITY_TABLE,PORT_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,MACRO_CONNECTIVITY_TABLE,PORT_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE);
			else
				sprintf(selectdevID,"select t.linkid,g.deviceid,f.deviceid,g.port_nbr||'|'||nvl(g.port_type,'none')||'?',f.port_nbr||'|'||nvl(f.port_type,'none')||'?',t.link_type,t.bidirectional_link_used,t.lkused,t.link_weight,m.devicename||'?',n.devicename||'?',nvl(t.link_info,'none')||'?' from %s t,%s g,%s f, %s l,%s m,%s n,lhcb_device_system k,lhcb_device_system c,lhcb_device_system b where t.lkused=1 and g.deviceid=m.deviceid and f.deviceid=n.deviceid and t.portidfrom=g.portid and t.portidto=f.portid and l.deviceid=k.deviceid and k.systemid=c.systemid and c.deviceid=m.deviceid and l.deviceid=b.deviceid and b.systemid=n.deviceid and (l.devicename=:dfrom ) union select -t.linkid,f.deviceid,g.deviceid,f.port_nbr||'|'||nvl(f.port_type,'none')||'?',g.port_nbr||'|'||nvl(g.port_type,'none')||'?',t.link_type,t.bidirectional_link_used,t.lkused,decode(t.link_weight,1,2,2,1,0),n.devicename||'?',m.devicename||'?',nvl(t.link_info,'none')||'?' from %s t,%s g,%s f,%s m,%s n,%s l,lhcb_device_system k,lhcb_device_system c,lhcb_device_system b where t.lkused=1 and g.deviceid=m.deviceid and f.deviceid=n.deviceid and t.portidfrom=g.portid and t.portidto=f.portid and l.deviceid=k.deviceid and k.systemid=c.systemid and c.deviceid=m.deviceid and k.systemid=b.systemid and b.deviceid=n.deviceid and l.devicename=:dfrom  and t.bidirectional_link_used=1",MACRO_CONNECTIVITY_TABLE,PORT_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,MACRO_CONNECTIVITY_TABLE,PORT_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE);


			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevID,(ub4) strlen(selectdevID),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			//std::cout<<"selectdevID="<<selectdevID<<std::endl;
		}
		if(status!=OCI_SUCCESS )
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
		}
		else
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":dfrom",-1,(dvoid*) system_name,strlen(system_name)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

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
				rescode=ShowErrors (status, ociError, "OCIAttrSet unsuccessful");
		}
		else
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
			/* The next two statements describe the select-list item, dept, and return its length to allocate the memory*/
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 1);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet1 unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &len_devname, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else
		{
			devName=(char*)malloc((len_devname+1)*sizeof(char));
			if(devName!=NULL)
				status=OCIDefineByPos (stmthp, &def[3], ociError, 4,(ub1*) devName,len_devname+1, SQLT_STR, 0, 0, 0, OCI_DEFAULT);
			else
			{

				rescode=ShowErrors (status, ociError, "pointer allocation unsuccessful");
				rescode=-1;

				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;	
			}

		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
		{

			pos2=0;
			pos3=0;
			i=0;
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);
				if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
				{
					pos1=strcspn(devName,"?");
					devName[pos1]='\0';
					pos3=pos2;
					pos2+=pos1+1;

					//std::cout<<"devname= "<<devName<<" and pos1="<<pos1<<" and pos2="<<pos2<<std::endl;
					//actual_len+=pos1+1;
					devIDs_temp=(char*) realloc(devIDs_temp,pos2*sizeof(char));

					if(devIDs_temp==NULL)
					{
						status=-10;
						rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
						rescode=-1;
						free(devName);

						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
						return rescode;
					}
					else
					{
						memcpy(devIDs_temp+pos3,devName,pos1+1);
						i++;
					}
				}
			}


		}
		std::cout<<"i= "<<i<<std::endl;
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}

		return rescode;
	}

	/**
	* Update a subsystem name entry in case it was mystaped,  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param old_systemname :  ip address entry you want to modify. 
	* @param new_systemname : new value of ip address entry you want to modify. 
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/ 
	/*************************************************************************/
	EXTERN_CONFDB int UpdateSubsystemName(char* old_sysname,char* new_sysname,char* ErrMess)
	{

		//extern "C" __declspec(dllexport)
		char appliName[100]="UpdateSubsystemName";
		char sqlstmt[2000];
		OCIStmt *hstmt;
		OCIBind  *bndp[2]; 
		int rescode=0;
		int free_mem=0;
		char res_query1[100]="blabla";
		int res_query=0;
		int deviceid=0;
		sword status=0;
		int i=0;
		int numrows_inserted=0;
		int numrows=0;
		OCIBind  *bndp3 = (OCIBind *) 0;






		status =OCIHandleAlloc (ociEnv, (void**)&hstmt, OCI_HTYPE_STMT , 0, 0);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIHandleAlloc unsuccessful");
		}
		else
		{

			//sprintf(sqlstmt,"BEGIN update %s set nodeused=decode(:nused,1,1,0,0,nodeused),last_update=sysdate,user_update='%s' where devicename=:dname; if :numrows1=1 then update %s e set e.last_update=sysdate,e.user_update='%s',e.lkused=(select t.nodeused*f.nodeused from %s t,%s f, %s l,%s m where t.deviceid=l.deviceid and l.portid=e.portidfrom and f.deviceid=m.deviceid and m.portid=e.portidto); end if; :numrows:=%s; END;",LOGICAL_DEVICE_TABLE,login,MACRO_CONNECTIVITY_TABLE,login,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,PORT_TABLE,PORT_TABLE,SQLROWCOUNT);
			sprintf(sqlstmt,"BEGIN update %s set system_name=:new_sysname where system_name=:old_sysname; :numrows:=%s; END;",SUBSYSTEM_TABLE,SQLROWCOUNT);
			status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			//std::cout<<"sql stmt "<<sqlstmt<<std::endl;

		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
		{

			status =OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":old_sysname", -1,(dvoid*)old_sysname,strlen(old_sysname)+1,  SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status =OCIBindByName(hstmt, &bndp[1], ociError,(text*) ":new_sysname", -1,(dvoid*)new_sysname,strlen(new_sysname)+1,  SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status =OCIBindByName(hstmt, &bndp3, ociError,(text*) ":numrows", -1,(dvoid*)&numrows_inserted, sizeof(int),SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status= OCIStmtExecute(ociHdbc, hstmt, ociError, 1, 0, 0, 0, OCI_DEFAULT );
		//std::cout<<"res_query "<<res_query<<std::endl;
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
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


		//std::cout<<"end of fct "<<std::endl;
		return (status+rescode);
	}

	/*****************************************************************************************/
#if defined(__cplusplus) || defined(c_plusplus)
}
#endif
