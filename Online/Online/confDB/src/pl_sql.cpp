/********************************************************************************/
//	Author: L.Abadie
// version:v3.5
// changes: handling error such that can be extended to C++: (create a class SQLException) (strlen ck + update stmt)
// file which uses PL/SQL procedures/fct : use for getting the connectivity between devices not directly connected
// N.B : Memory management (allocation+release) should be handled by the user application thus, for all the fct provided by this lib
/********************************************************************************/
#include "list_structures.h"
#include "system_info.h"
#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#if defined( _WIN32) || defined(WIN32)

#define EXTERN_CONFDB _declspec(dllexport)

#else
#define EXTERN_CONFDB 
#endif

	extern   OCIError* ociError ;
	extern   OCIEnv* ociEnv ;
	extern   OCISvcCtx* ociHdbc ;
	extern   OCIServer *mysrvhp;
	extern   OCISession *myusrhp; 



	/**************************************************************************************/
	/**
	* create the tables, indexes and constraints in your DB schema,  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* you need grant execute if you have your own db account
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/
	EXTERN_CONFDB
		int CreateTableSchema(char* ErrMess)
	{
		char appliName[100]="CreateTableSchema";
		int rescode=0;
		int res_code=1;
		sword status;
		OCIStmt *stmthp;
		OCIBind  *bnd1p; 

		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		if(status!=OCI_SUCCESS)
		{	
			rescode=ShowErrors (status, ociError, "OCIHandleAlloc unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrMess,1); 
			return -1;
		}
		else
		{
			char selectdevtype[1000];
			sprintf(selectdevtype,"BEGIN :rescode:=%s(1); END;",_createschema);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevtype,(ub4) strlen(selectdevtype),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":rescode",-1,(dvoid*)&res_code,sizeof(res_code), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIStmtExecute(ociHdbc, stmthp, ociError,1, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 
		else
		{
			if(res_code==0)
				OCIReportError(ociError,appliName, ErrMess,0); 
			else
			{
				GetErrorMess(appliName,"Couldn't create the tables",ErrMess,1); 


			}
		}

		return (status+res_code);
	}
	/**************************************************************************************/
	/**
	* drop the tables, indexes and constraints in your DB schema,  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* you need grant execute if you have your own db account
	* once the schema is dropped you will loose all your data: to use with precaution
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/
	EXTERN_CONFDB
		int DropTableSchema(char* ErrMess)
	{
		char appliName[100]="DropTableSchema";
		int rescode=0;
		int res_code=1;
		sword status;
		OCIStmt *stmthp;
		OCIBind  *bnd1p; 
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		if(status!=OCI_SUCCESS)
		{	

			rescode=ShowErrors (status, ociError, "OCIHandleAlloc unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrMess,1); 
			return -1;
		}
		else
		{
			char selectdevtype[1000];
			sprintf(selectdevtype,"BEGIN :rescode:=%s(1); END;",_dropschema);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevtype,(ub4) strlen(selectdevtype),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":rescode",-1,(dvoid*)&res_code,sizeof(res_code), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);



		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIStmtExecute(ociHdbc, stmthp, ociError,1, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 
		else
		{
			if(res_code==0)
				OCIReportError(ociError,appliName, ErrMess,0); 
			else
			{
				GetErrorMess(appliName,"Couldn't create the tables",ErrMess,1); 

			}
		}

		return (status+res_code);
	}
	/**************************************************************************************/
	/**
	* create the forwarding table (IP,MAC table ) of the given device in your DB schema,  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param devicename :devicename 
	* @param round_trip_max : maximum path length put 0 if you want the default value (10)
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/
	EXTERN_CONFDB
		int CreateRoutingTable(char* devicename, int round_trip_max,char* ErrMess)
	{
		char appliName[100]="CreateRoutingTable";
		int rescode=0;
		int res_code=1;
		int roundtrip;
		sword status;
		OCIStmt *stmthp;
		OCIBind  *bnd1p[3]; 
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		if(status!=OCI_SUCCESS)
		{	
			rescode=ShowErrors (status, ociError, "OCIHandleAlloc unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrMess,1); 
			return -1;
		}
		else
		{
			char selectdevtype[1000];
			if(round_trip_max==0)
				roundtrip=10;
			else
				roundtrip=round_trip_max;

			sprintf(selectdevtype,"BEGIN :rescode:=%s(:devname,1,:roundtrip,1); END;",_createRT);
			//std:://cout <<"selecttype "<<selectdevtype<<" sysname " <<systemname<< " devicename :"<<devicename<< std::endl;
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevtype,(ub4) strlen(selectdevtype),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[0], ociError,(text*)":rescode",-1,(dvoid*)&res_code,sizeof(res_code), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[1], ociError,(text*)":devname",-1,(dvoid*)devicename,strlen(devicename)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[2], ociError,(text*)":roundtrip",-1,(dvoid*)&roundtrip,sizeof(roundtrip), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIStmtExecute(ociHdbc, stmthp, ociError,1, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 
		else
		{
			if(res_code==0)
				OCIReportError(ociError,appliName, ErrMess,0); 
			else
			{
				GetErrorMess(appliName,"Couldn't create the routing table",ErrMess,0); 
			}
		}
		return (status+res_code);
	}
	/*****************************************************************************************************/
	/**************************************************************************************/
	/**
	* create the destination table (IP,MAC table ) of the given device in your DB schema,  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param devicename :devicename 
	* @param round_trip_max : maximum path length put 0 if you want the default value (10)
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/
	EXTERN_CONFDB
		int CreateDestinationTable(char* devicename, int round_trip_max,char* ErrMess)
	{
		char appliName[100]="CreateDestinationTable";
		int rescode=0;
		int res_code=1;
		sword status;
		OCIStmt *stmthp;
		OCIBind  *bnd1p[3]; 
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		if(status!=OCI_SUCCESS)
		{	
			rescode=ShowErrors (status, ociError, "OCIHandleAlloc unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrMess,1); 
			return -1;
		}
		else
		{
			char selectdevtype[1000];
			sprintf(selectdevtype,"BEGIN :rescode:=%s(:devname,1,:roundtrip,1); END;",_createTD);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevtype,(ub4) strlen(selectdevtype),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[0], ociError,(text*)":rescode",-1,(dvoid*)&res_code,sizeof(res_code), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[1], ociError,(text*)":devname",-1,(dvoid*)devicename,strlen(devicename)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[2], ociError,(text*)":roundtrip",-1,(dvoid*)&round_trip_max,sizeof(round_trip_max), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIStmtExecute(ociHdbc, stmthp, ociError,1, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 
		else
		{
			if(res_code==0)
				OCIReportError(ociError,appliName, ErrMess,0); 
			else
			{
				GetErrorMess(appliName,"Couldn't create the destination table",ErrMess,0); 
			}
		}
		return (status+res_code);
	}


	/******************************************************************************************************/
	/**
	* Get the port nb out between dev_from and the subsystem (mainly used for partioning),  returning an integer value.
	* dev_from is either a TFC device device.
	* The user should manage the memory : there is no memory allocation.
	* you need grant execute if you have your own db account
	* @param dev_from : name of the device 1 (ex. THOR_00)
	* @param subsystem_name : name of a the subsystem name follows the convention guides
	* @param len_array : length of the pfrom_list  you have allocated. If the int * buffer is too small, we put the size needed here.
	* @param pfrom_list : list of the port from numbers: concatenation of port_nbr|port_way|port_type. each line is separated with |
	* @param ErrMess : error message in case of failure (even NO_ROWS_SELECTED for no result)
	* @return 0 if it is successful
	*/

	EXTERN_CONFDB
		int GetPortPerSubsystem(char* dev_from, char* subsystem_name, int& len_array, char* pfrom_list, char* ErrMess)
	{
		char appliName[100]="GetPortPerSubsystem";
		int i=0;
		int j=0;
		int res_code=0;
		int pos2=0;
		int len1=0;
		int rescode=0;
		int free_mem=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		int prefetch_rows=1000;
		sword status;
		OCIStmt *stmthp;
		OCIBind  *bnd1p; 
		ub4  descriptionlen=0;
		ub4 devtypelen=0;
		int len11=0;
		OCIDefine* def[1];
		int devid=0;
		int pathid=0;
		int pto=0;
		int pfrom=0;
		int nfromlen=0;
		int ntolen=0;
		int stop=0;
		int pos1=0;
		int sysID=-1;
		OCIParam *parmdp;

		//strcpy(ErrMess,"dummy test");
		if(strcspn(subsystem_name,",")>=strlen(subsystem_name))
		{
			sysID=GetSubsystemID(subsystem_name);
		}
		else
		{
			GetErrorMess(appliName, "One subsystem name should be given",ErrMess,1);
			return -1;
		}
		//std:://cout<<"sysID= "<<sysID<<" and value of dev_from="<<dev_from<<std::endl;	
		char* port_list_temp=NULL;
		if(sysID==-1)
		{
			status=-10;
			GetErrorMess(appliName, "Invalid subsystem name",ErrMess,1);
			return -1;
		}

		//char* pfrom_list_temp=NULL;
		char * portlist=NULL;


		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		//strcpy(portlist,"tetstggggggggg");

		if(status!=OCI_SUCCESS)
		{	
			rescode=ShowErrors (status, ociError, "OCIHandleAlloc unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrMess,1); 
			free(portlist);
			return -1;
		}
		else
		{
			char selectdevtype[1000];
			sprintf(selectdevtype,"select distinct t.port_nbr||'|'||nvl(t.port_type,'none')||'|'||t.port_way||'?' from %s t,%s e,%s r,%s d where t.portid=e.pfromid0 and e.nodeid_end1=r.deviceid and  mod(r.system_name,%d)=0 and e.nodeid_start0=d.deviceid and d.devicename=:devfrom ",PORT_TABLE,DESTINATIONTABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,sysID);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevtype,(ub4) strlen(selectdevtype),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			//std:://cout<<"selectdevtype "<<selectdevtype<<" and value of sysID="<<sysID<<std::endl;	
		}


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":devfrom",-1,(dvoid*)dev_from,strlen(dev_from)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		//if(status!=OCI_SUCCESS)
		//{
		//	if(rescode==0)	
		//	rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		//}
		//else
		//	status=OCIBindByName(stmthp, &bnd1p[1], ociError,(text*)":sysID",-1,(dvoid*)&sysID,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIAttrSet (stmthp,OCI_HTYPE_STMT,&prefetch_rows,0,OCI_ATTR_PREFETCH_ROWS,ociError);

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
				rescode=ShowErrors (status, ociError, "OCIStmtExecute  unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 1);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet  unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &len11, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		//std:://cout<<"dev_from= "<<dev_from<<std::endl;	
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIExecute unsuccessful");
		}
		else
		{
			portlist=(char*)malloc(len11*sizeof(char));
			if(portlist==NULL)
			{
				status=-10;
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				return -1;
			}
			else
				status=OCIDefineByPos (stmthp, &def[0], ociError,1,portlist,len11, SQLT_STR, 0, 0, 0, OCI_DEFAULT);
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
		{
			i=0;

			pos1=0;
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);

				if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
				{

					pos2=strcspn(portlist,"?");
					portlist[pos2]='\0';
					port_list_temp=(char*)realloc(port_list_temp,(pos2+pos1+1)*sizeof(char*));
					if(port_list_temp==NULL || portlist==NULL )
					{
						if(portlist!=NULL)
							free(portlist);
						if(port_list_temp!=NULL)
							free(port_list_temp);
						status=OCITransCommit(ociHdbc, ociError, 0);
						status+=OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
						return -1;
					}
					memcpy(port_list_temp+pos1,portlist,pos2+1);
					i++;
					pos1+=pos2+1;
					//std:://cout<<"after fecthc value of portlist="<<port_list_temp<<std::endl;
				}
			}
			//std:://cout<<"value of portlist="<<portlist<<" and value of pos1="<<pos1<<std::endl;
			if(i>0)
			{
				status=0;
				if(len_array>(pos1))
				{
					memcpy(pfrom_list,port_list_temp,pos1);
					len_array=pos1;
					OCIReportError(ociError,appliName,ErrMess,0); 
				}
				else
				{
					strcpy(pfrom_list,"BUFFER_TOO_SMALL");
					GetErrorMess(appliName, "BUFEER_TOO_SMALL",ErrMess,1);
					rescode=-1;
					len_array=pos1+pos2+1;
				}
			}
			else
			{
				GetErrorMess(appliName, "NO_ROWS_SELECTED",ErrMess,1);
				rescode=0;
			}

			if(port_list_temp!=NULL)
				free(port_list_temp);
		}
		if(portlist!=NULL)
			free(portlist);
		status=OCITransCommit(ociHdbc, ociError, 0);
		status+=OCIHandleFree (stmthp,OCI_HTYPE_STMT);

		//std:://cout<<"protlist= "<<pfrom_list<<std::endl;	
		if(rescode!=0 )
			OCIReportError(ociError,appliName, ErrMess,1); 
		return (status+rescode);

	}








	/**********************************************************************************************************/
	/**
	* Get the path details  between dev_from and dev_to, returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* you need grant execute if you have your own db account
	* @param cpntname_from : name of the board cpnt which wil start the micro path
	* @param  mboard_portid : portid of the motherboard which wil end the micro path. Put 0 if you want for all of them.
	* @param nfrom_list : list of the cpnts which start the link (separated by '\\0') concatenation of cpntname|port_nbr
	* @param len_array : length of the pfrom_list (which is equal to pto_list,pathid_list and link_pos_list) you have allocated. If the int * buffer is too small, we put the size needed here.
	* @param nfrom_arraylen : length of the nfrom_list  you have allocated. If the char * buffer is too small, we put the size needed here.
	* @param nto_arraylen : length of the nto_list  you have allocated. If the char * buffer is too small, we put the size needed here.
	* @param nto_list : list of the cpnts which end the link concatenation of cpntname|port_nbr and for the last link we have cpntname|port_nbr|port_type|port_way 
	* @param pathid_list : list of the pathid
	* @param last_nfrominfo : details  which start the last link cpntname|port_nbr|port_type|port_way 
	* @param last_ntoinfo : details  which ends the last link cpntname|port_nbr|port_type|port_way 
	* @param len_lastnfrominfo :length of last_nfrominfo you have allocated, if too small we put the size
	* @param len_lastntoinfo :length of last_ntoinfo you have allocated, if too small we put the size
	* @param link_pos_list : position of the link in the path (start with 1)
	* NB: the i-th entry of the nfrom_list,pfrom_list,nto_list,pto_list,pathid_list and link_pos_list corresponds to one link of the path
	* @param ErrMess : error message in case of failure (even NO_ROWS_SELECTED for no result)
	* @return 0 if it is successful
	*/

	EXTERN_CONFDB
		int GetMicroConnectBetweenBoardCpntAndMotherBoard(char* cpntname_from, int mboard_portid, int* pathid_list, int* link_pos_list,int &len_array,char* nfrom_list,int &nfrom_arraylen,char* nto_list,int &nto_arraylen,char* last_nfrominfo,unsigned int &len_lastnfrominfo,char* last_ntoinfo,unsigned int &len_lastntoinfo,char* ErrMess)
	{
		char appliName[100]="GetMicroConnectBetweenBoardCpntAndMotherBoard";
		int i=0;
		int j=0;
		int res_code=0;
		int pos2=0;
		int len1=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		sword status;
		int rescode=0;
		int rescode_pl=-1;
		int rescode_pl2=-1;
		int resquery=-1;
		int cpntid=-1;
		int portid=mboard_portid;
		int free_stmt=0;
		OCIStmt *stmthp;
		OCIBind  *bnd1p[7]; 
		OCIDefine* def[6];
		ub4  descriptionlen=0;
		ub4 devtypelen=0;
		char * nfrom_temp = NULL;
		char *nto_temp=NULL;
		int prefetch_rows=10000;
		int len11=101;

		int nfrom_list_len=0;
		int nto_list_len=0;
		int nfrom_list_len_prev=0;
		int nto_list_len_prev=0;

		OCIParam *parmdp;

		int pathid=0;

		int round_trip=1;
		int nfromlen=0;
		int ntolen=0;
		int link_pos=0;
		char* nfrom_list_temp=NULL;

		char* nto_list_temp=NULL;
		int mboard=0;
		int* pathid_list_temp=NULL; 
		int* link_pos_temp=NULL;
		char selectdevtype[1000];
		//clock_t start,finish;
		//double time;

		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);

		if(status!=OCI_SUCCESS || ociEnv==0)
		{	
			rescode=ShowErrors (status, ociError, "OCIHandleAlloc unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrMess,1); 

			return -1;
		}
		else
		{

			//start = clock();

			//std:://cout <<"value of systemname "<<systemname<<"  and value of dev_from "<<dev_from <<" and dev_to "<< dev_to <<std::endl;
			sprintf(selectdevtype,"BEGIN %s(:mboard,1,:rescode,:cpntid,:cpntname); if :rescode=0 then :resquery:=%s(:cpntid); else :resquery:=-1; end if; if :resquery=0 then :rcode:=%s(:cpntid,:portid); else :rcode:=-1; end if;END;",_CreateMicroLgLinks,_CreateMicroPaths,_InsertMicroPathUsingID);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevtype,(ub4) strlen(selectdevtype),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[0], ociError,(text*)":mboard",-1,(dvoid*)&mboard,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[1], ociError,(text*)":cpntname",-1,(dvoid*)cpntname_from,strlen(cpntname_from)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[2], ociError,(text*)":rescode",-1,(dvoid*)&rescode_pl,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[3], ociError,(text*)":cpntid",-1,(dvoid*)&cpntid,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[4], ociError,(text*)":resquery",-1,(dvoid*)&resquery,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[5], ociError,(text*)":rcode",-1,(dvoid*)&rescode_pl2,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[6], ociError,(text*)":portid",-1,(dvoid*)&portid,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 1, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

		//finish = clock();
		//time = (double(finish)-double(start))/CLOCKS_PER_SEC;  
		//std:://cout <<"value of tabame "<<tabname <<" time elapsed "<<time<<std::endl;

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
			if(rescode_pl2!=0)
			{
				if(resquery!=0) //means that the PL/SQL fct unsuccessfully executed
				{
					if(rescode_pl!=0)	
					{
						//rescode=ShowErrors (OCI_SUCCESS_WITH_INFO, ociError, "OCIPL/SQL fct unsuccessful");
						//std:://cout <<"length tabname "<<strlen(tabname)<<std::endl;
						if(rescode==-1)
							GetErrorMess(appliName, "Cpnt Name not found",ErrMess,1);
						else
							GetErrorMess(appliName, "Pb when creating the micro lg links",ErrMess,1);

						if(OCITransCommit(ociHdbc, ociError, 0)!=OCI_SUCCESS)
						{
							if(rescode==0)	
								rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
						}
						status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);

						return -1;
					}
					else
					{
						GetErrorMess(appliName, "Pb when finding the micro paths",ErrMess,1);
						if(OCITransCommit(ociHdbc, ociError, 0)!=OCI_SUCCESS)
						{
							if(rescode==0)	
								rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
						}
						status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						return -1;
					}
				}
				else
				{
					GetErrorMess(appliName, "Error  when trying to find the paths",ErrMess,1);
					if(OCITransCommit(ociHdbc, ociError, 0)!=OCI_SUCCESS)
					{
						if(rescode==0)	
							rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
					}
					status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					return -1;
				}
			}
			else
			{
				sprintf(selectdevtype,"select d.pathid,d.link_pos,d.nfrom||'?',d.nto||'?' from %s d order by pathid,link_pos ",MICROPATH_TEMP);
				status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevtype,(ub4) strlen(selectdevtype),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
				//std:://cout <<"value of selectdevtype "<<selectdevtype <<std::endl;

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
				}
				else
					status=OCIAttrSet (stmthp,OCI_HTYPE_STMT,&prefetch_rows,0,OCI_ATTR_PREFETCH_ROWS,ociError);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIAttrSet  unsuccessful");
				}
				else
					status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);
				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIStmtExecute  unsuccessful");
				}
				else
					status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 3);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIParamGet  unsuccessful");
				}
				else
					status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &nfromlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIStmtExecute  unsuccessful");
				}
				else
					status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 4);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIParamGet  unsuccessful");
				}
				else
					status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &ntolen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

				status=OCIDefineByPos (stmthp, &def[0], ociError,1,&pathid,sizeof(pathid), SQLT_INT, 0, 0, 0, OCI_DEFAULT);
				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
				else
					status=OCIDefineByPos (stmthp, &def[1], ociError,2,&link_pos,sizeof(link_pos), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
				else
				{
					nfromlen=800;
					nfrom_temp=(char *) realloc(nfrom_temp,(nfromlen+1)*sizeof(char));
					ntolen=800;
					nto_temp= (char *) realloc(nto_temp,(ntolen+1)*sizeof(char));

					if(nfrom_temp==NULL || nto_temp==NULL )
					{	
						if(nfrom_temp!=NULL)
							free(nfrom_temp);
						if(nto_temp!=NULL)
							free(nto_temp);

						GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
						status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);

						return -1;
					}
					else
						status =OCIDefineByPos(stmthp, &def[2], ociError,3, (ub1 *) (nfrom_temp), nfromlen+1,SQLT_STR, (dvoid *) 0,(ub2 *) 0,0, OCI_DEFAULT);	
				}


				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
				else
					status =OCIDefineByPos(stmthp, &def[3], ociError,4, (ub1 *) (nto_temp), ntolen + 1,SQLT_STR, (dvoid *) 0,(ub2 *) 0,0, OCI_DEFAULT);


				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
				else
				{
					i=0;
					pathid_list_temp=(int*)realloc(pathid_list_temp,prefetch_rows*sizeof(int));
					link_pos_temp=(int*)realloc(link_pos_temp,prefetch_rows*sizeof(int));

					if( pathid_list_temp==NULL || link_pos_temp==NULL)
					{
						if(nfrom_temp!=NULL)
							free(nfrom_temp);
						if(nto_temp!=NULL)
							free(nto_temp);

						if(pathid_list_temp!=NULL)
							free(nfrom_temp);
						if(link_pos_temp!=NULL)
							free(nto_temp);

						status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);

						GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
						return -1;
					}
					while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
					{
						status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);
						if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
						{

							pathid_list_temp[i]=pathid;
							link_pos_temp[i]=link_pos;
							ntolen=strcspn(nto_temp,"?");
							nto_list_len+=ntolen+1;
							nto_temp[ntolen]='\0';

							nfromlen=strcspn(nfrom_temp,"?");
							nfrom_temp[nfromlen]='\0';
							nfrom_list_len+=nfromlen+1;

							nfrom_list_temp=(char *) realloc(nfrom_list_temp,nfrom_list_len*sizeof(char));
							nto_list_temp=(char *) realloc(nto_list_temp,nto_list_len*sizeof(char));
							if(nfrom_list_temp==NULL || nto_list_temp==NULL )
							{
								if(nfrom_temp!=NULL)
									free(nfrom_temp);
								if(nto_temp!=NULL)
									free(nto_temp);

								if(nfrom_list_temp!=NULL)
									free(nfrom_list_temp);
								if(nto_list_temp!=NULL)
									free(nto_list_temp);

								if(pathid_list_temp!=NULL)
									free(pathid_list_temp);
								if(link_pos_temp!=NULL)
									free(link_pos_temp);
								if(OCITransCommit(ociHdbc, ociError, 0)!=OCI_SUCCESS)
								{
									if(rescode==0)	
										rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
								}
								GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
								status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);

								return -1;
							}
							else
							{
								memcpy(nfrom_list_temp+nfrom_list_len_prev,nfrom_temp,nfromlen+1);
								memcpy(nto_list_temp+nto_list_len_prev,nto_temp,ntolen+1);
								nfrom_list_len_prev=nfrom_list_len;
								nto_list_len_prev=nto_list_len;

								i++;
							}
						}
						if(i>0 && (i%prefetch_rows)==0)
						{
							round_trip++;
							pathid_list_temp=(int*)realloc(pathid_list_temp,round_trip*prefetch_rows*sizeof(int));
							link_pos_temp=(int*)realloc(link_pos_temp,round_trip*prefetch_rows*sizeof(int));
							if( pathid_list_temp==NULL || link_pos_temp==NULL)
							{
								if(nfrom_temp!=NULL)
									free(nfrom_temp);
								if(nto_temp!=NULL)
									free(nto_temp);

								if(nfrom_list_temp!=NULL)
									free(nfrom_list_temp);
								if(nto_list_temp!=NULL)
									free(nto_list_temp);

								if(pathid_list_temp!=NULL)
									free(pathid_list_temp);
								if(link_pos_temp!=NULL)
									free(link_pos_temp);
								if(OCITransCommit(ociHdbc, ociError, 0)!=OCI_SUCCESS)
								{
									if(rescode==0)	
										rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
								}
								GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
								status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);
								return -1;
							}
						}
					}
					if(status!=OCI_SUCCESS && status != OCI_SUCCESS_WITH_INFO && i==0)
					{
						if(rescode==0)	
						{
							if(OCITransCommit(ociHdbc, ociError, 0)!=OCI_SUCCESS)
							{
								if(rescode==0)	
									rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
							}
							rescode=ShowErrors (status, ociError, "OCIFETCH unsuccessful");
							if(nfrom_temp!=NULL)
								free(nfrom_temp);
							if(nto_temp!=NULL)
								free(nto_temp);


							if(nfrom_list_temp!=NULL)
								free(nfrom_list_temp);
							if(nto_list_temp!=NULL)
								free(nto_list_temp);

							if(pathid_list_temp!=NULL)
								free(pathid_list_temp);
							if(link_pos_temp!=NULL)
								free(link_pos_temp);

							GetErrorMess(appliName, "NO_ROWS_SELECTED",ErrMess,1);
							status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);

							return 0;
						}
					}
					if(nfrom_arraylen>nfrom_list_len && rescode==0)
					{
						nfrom_arraylen=nfrom_list_len;
						memcpy(nfrom_list,nfrom_list_temp,nfrom_list_len);
						if(len_lastnfrominfo>strlen(nfrom_temp))
							strcpy(last_nfrominfo,nfrom_temp);
						else
						{
							len_lastnfrominfo=strlen(nfrom_temp);
							strcpy(last_nfrominfo,"BUFFER_TOO_SMALL");
						}
						status=0;
					}
					else
					{
						nfrom_arraylen=nfrom_list_len;
						status+=-1;
					}

					if(nto_arraylen>nto_list_len  && rescode==0)
					{
						nto_arraylen=nto_list_len;
						memcpy(nto_list,nto_list_temp,nto_list_len);
						if(len_lastntoinfo>strlen(nto_temp))
							strcpy(last_ntoinfo,nto_temp);
						else
						{
							len_lastntoinfo=strlen(nto_temp)+1;
							strcpy(last_ntoinfo,"BUFFER_TOO_SMALL");
						}
						status+=0;
					}
					else
					{
						nto_arraylen=nto_list_len;
						status+=-1;
					}


					if(len_array>i-1  && rescode==0)
					{
						for (j=0;j<i;j++)
						{

							pathid_list[j]=pathid_list_temp[j];
							link_pos_list[j]=link_pos_temp[j];
						}
						len_array=i;
						status+=0;
					}
					else
					{
						len_array=i;
						status+=-1;
					}



				}
				if(nfrom_temp!=NULL)
					free(nfrom_temp);
				if(nto_temp!=NULL)
					free(nto_temp);

				if(nfrom_list_temp!=NULL)
					free(nfrom_list_temp);
				if(nto_list_temp!=NULL)
					free(nto_list_temp);

				if(pathid_list_temp!=NULL)
					free(pathid_list_temp);
				if(link_pos_temp!=NULL)
					free(link_pos_temp);


			}
		}



		if(OCITransCommit(ociHdbc, ociError, 0)!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}

		status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 
		else
			OCIReportError(ociError,appliName, ErrMess,0); 


		return (rescode+status);
	}
	/**********************************************************************************************************/
	/**
	* Get the path details  between dev_from and dev_to, returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* you need grant execute if you have your own db account
	* @param mboardportid_from : portid_from of the motherboard. it will start the micro path
	* @param cpntname : put "none" if you provide a mboardportid_to
	* @param mboardportid_to : portid of the motherboard. Put 0 if you provide a board cpnt name
	* @param nfrom_list : list of the cpnts which start the link (separated by '\\0') and for the first link  we have cpntname|port_nbr|port_type|port_way
	* @param len_array : length of the pfrom_list (which is equal to pto_list,pathid_list and link_pos_list) you have allocated. If the int * buffer is too small, we put the size needed here.
	* @param nfrom_arraylen : length of the nfrom_list  you have allocated. If the char * buffer is too small, we put the size needed here.
	* @param nto_arraylen : length of the nto_list  you have allocated. If the char * buffer is too small, we put the size needed here.
	* @param nto_list : list of the cpnts which end the link concatenation of cpntname|port_nbr and for the last link we have cpntname|port_nbr|port_type|port_way 
	* @param pathid_list : list of the pathid
	* @param last_nfrominfo : details  which start the last link cpntname|port_nbr|port_type|port_way 
	* @param last_ntoinfo : details  which ends the last link cpntname|port_nbr|port_type|port_way 
	* @param len_lastnfrominfo :length of last_nfrominfo you have allocated, if too small we put the size
	* @param len_lastntoinfo :length of last_ntoinfo you have allocated, if too small we put the size
	* @param link_pos_list : position of the link in the path (start with 1)
	* NB: the i-th entry of the nfrom_list,pfrom_list,nto_list,pto_list,pathid_list and link_pos_list corresponds to one link of the path
	* @param ErrMess : error message in case of failure (even NO_ROWS_SELECTED for no result)
	* @return 0 if it is successful
	*/

	EXTERN_CONFDB
		int GetMicroConnectFromPortid(int mboardportid_from, int mboardportid_to, char* cpntname,int* pathid_list, int* link_pos_list,int &len_array,char* nfrom_list,int &nfrom_arraylen,char* nto_list,int &nto_arraylen,char* last_nfrominfo,unsigned int &len_lastnfrominfo,char* last_ntoinfo,unsigned int &len_lastntoinfo,char* ErrMess)
	{
		char appliName[100]="GetMicroConnectFromPortid";
		int i=0;
		int j=0;
		int res_code=0;
		int pos2=0;
		int len1=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		sword status;
		int rescode=0;
		int rescode_pl=1;
		int rescode_pl2=1;
		int resquery=1;
		int cpntid=1;
		//int portid=-mboardportid_from;
		int free_stmt=0;
		OCIStmt *stmthp;
		OCIBind  *bnd1p[9]; 
		OCIDefine* def[6];
		ub4  descriptionlen=0;
		ub4 devtypelen=0;
		char * nfrom_temp = NULL;
		char *nto_temp=NULL;
		int prefetch_rows=10000;
		int len11=101;
		char * pfrom_temp = NULL;
		char *pto_temp=NULL;

		int nfrom_list_len=0;
		int nto_list_len=0;
		int nfrom_list_len_prev=0;
		int nto_list_len_prev=0;
		int pathid=0;
		int round_trip=1;
		int nfromlen=0;
		int ntolen=0;
		int link_pos=0;
		char* nfrom_list_temp=NULL;
		OCIParam *parmdp;
		char* nto_list_temp=NULL;
		int mboard=mboardportid_from;
		//int mbpid_to=-mboardportid_to;
		//int mbpid_from=-mboardportid_from;

		int* pathid_list_temp=NULL; 
		int* link_pos_temp=NULL;
		char selectdevtype[1000];
		//clock_t start,finish;
		//double time;
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);

		if(status!=OCI_SUCCESS || ociEnv==0)
		{	
			rescode=ShowErrors (status, ociError, "OCIHandleAlloc unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrMess,1); 

			return -1;
		}
		else
		{

			//start = clock();

			//std:://cout <<"value of systemname "<<systemname<<"  and value of dev_from "<<dev_from <<" and dev_to "<< dev_to <<std::endl;
			sprintf(selectdevtype,"BEGIN %s(:mboard,1,:rescode,:cpntid); if :rescode=0 then :resquery:=%s(:portidfrom); else :resquery:=-1; end if; if :resquery=0 then :rcode:=%s(:portidfrom,:portidto,:cpntname); else :rcode:=-1; end if;END;",_CreateMicroLgLinks,_CreateMicroPaths_Host,_InsertMicroPathFromPortID);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevtype,(ub4) strlen(selectdevtype),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			//std:://cout <<"selectdevtype= "<<selectdevtype<<std::endl;
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[0], ociError,(text*)":mboard",-1,(dvoid*)&mboard,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName1 unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[1], ociError,(text*)":cpntname",-1,(dvoid*)cpntname,strlen(cpntname)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName2 unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[2], ociError,(text*)":rescode",-1,(dvoid*)&rescode_pl,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName3 unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[3], ociError,(text*)":cpntid",-1,(dvoid*)&cpntid,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName4 unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[4], ociError,(text*)":resquery",-1,(dvoid*)&resquery,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName5 unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[5], ociError,(text*)":rcode",-1,(dvoid*)&rescode_pl2,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName6 unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[6], ociError,(text*)":portidfrom",-1,(dvoid*)&mboardportid_from,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName8 unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[7], ociError,(text*)":portidto",-1,(dvoid*)&mboardportid_to,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName9 unsuccessful");
		}
		else
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 1, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

		//finish = clock();
		//time = (double(finish)-double(start))/CLOCKS_PER_SEC;  
		//std:://cout <<"after firts execute "<<std::endl;

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
			//std:://cout <<"value of maboard= "<<mboard<<" and value of cpntid="<<cpntid<<"value of rescode_pl2= "<<rescode_pl2<<" value of rescode_pl="<<rescode_pl<<" and value of resquery ="<<resquery<<std::endl;

			if(rescode_pl2!=0)
			{
				if(resquery!=0) //means that the PL/SQL fct unsuccessfully executed
				{
					if(rescode_pl!=0)	
					{
						//rescode=ShowErrors (OCI_SUCCESS_WITH_INFO, ociError, "OCIPL/SQL fct unsuccessful");
						//std:://cout <<"length tabname "<<strlen(tabname)<<std::endl;

						GetErrorMess(appliName, "Pb when creating the micro lg links",ErrMess,1);

						if(OCITransCommit(ociHdbc, ociError, 0)!=OCI_SUCCESS)
						{
							if(rescode==0)	
								rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
						}
						status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);

						return -1;
					}
					else
					{
						GetErrorMess(appliName, "Pb when finding the micro paths",ErrMess,1);
						if(OCITransCommit(ociHdbc, ociError, 0)!=OCI_SUCCESS)
						{
							if(rescode==0)	
								rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
						}
						status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						return -1;
					}
				}
				else
				{
					GetErrorMess(appliName, "Error  when trying to find the paths",ErrMess,1);
					if(OCITransCommit(ociHdbc, ociError, 0)!=OCI_SUCCESS)
					{
						if(rescode==0)	
							rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
					}
					status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					return -1;
				}
			}
			else
			{
				sprintf(selectdevtype,"select d.pathid,d.link_pos,d.nfrom||'?',d.nto||'?' from %s d order by pathid,link_pos ",MICROPATH_TEMP);
				status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevtype,(ub4) strlen(selectdevtype),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
				//std:://cout <<"value of selectdevtype "<<selectdevtype <<std::endl;

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
				}
				else
					status=OCIAttrSet (stmthp,OCI_HTYPE_STMT,&prefetch_rows,0,OCI_ATTR_PREFETCH_ROWS,ociError);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIAttrSet  unsuccessful");
				}
				else
					status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);
				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIStmtExecute  unsuccessful");
				}
				else
					status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 3);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIParamGet  unsuccessful");
				}
				else
					status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &nfromlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIStmtExecute  unsuccessful");
				}
				else
					status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 4);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIParamGet  unsuccessful");
				}
				else
					status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &ntolen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

				status=OCIDefineByPos (stmthp, &def[0], ociError,1,&pathid,sizeof(pathid), SQLT_INT, 0, 0, 0, OCI_DEFAULT);
				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
				else
					status=OCIDefineByPos (stmthp, &def[1], ociError,2,&link_pos,sizeof(link_pos), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
				else
				{

					nfrom_temp=(char *) realloc(nfrom_temp,(nfromlen+1)*sizeof(char));

					nto_temp= (char *) realloc(nto_temp,(ntolen+1)*sizeof(char));

					if(nfrom_temp==NULL || nto_temp==NULL )
					{	
						if(nfrom_temp!=NULL)
							free(nfrom_temp);
						if(nto_temp!=NULL)
							free(nto_temp);

						GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
						status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);

						return -1;
					}
					else
						status =OCIDefineByPos(stmthp, &def[2], ociError,3, (ub1 *) (nfrom_temp), nfromlen+1,SQLT_STR, (dvoid *) 0,(ub2 *) 0,0, OCI_DEFAULT);	
				}


				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
				else
					status =OCIDefineByPos(stmthp, &def[3], ociError,4, (ub1 *) (nto_temp), ntolen + 1,SQLT_STR, (dvoid *) 0,(ub2 *) 0,0, OCI_DEFAULT);


				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
				else
				{
					i=0;
					pathid_list_temp=(int*)realloc(pathid_list_temp,prefetch_rows*sizeof(int));
					link_pos_temp=(int*)realloc(link_pos_temp,prefetch_rows*sizeof(int));

					if( pathid_list_temp==NULL || link_pos_temp==NULL)
					{
						if(nfrom_temp!=NULL)
							free(nfrom_temp);
						if(nto_temp!=NULL)
							free(nto_temp);

						if(pathid_list_temp!=NULL)
							free(nfrom_temp);
						if(link_pos_temp!=NULL)
							free(nto_temp);

						status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);

						GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
						return -1;
					}
					while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
					{
						status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);
						if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
						{

							pathid_list_temp[i]=pathid;
							link_pos_temp[i]=link_pos;
							ntolen=strcspn(nto_temp,"?");
							nto_list_len+=ntolen+1;
							nto_temp[ntolen]='\0';

							nfromlen=strcspn(nfrom_temp,"?");
							nfrom_temp[nfromlen]='\0';
							nfrom_list_len+=nfromlen+1;

							nfrom_list_temp=(char *) realloc(nfrom_list_temp,nfrom_list_len*sizeof(char));
							nto_list_temp=(char *) realloc(nto_list_temp,nto_list_len*sizeof(char));
							if(nfrom_list_temp==NULL || nto_list_temp==NULL )
							{
								if(nfrom_temp!=NULL)
									free(nfrom_temp);
								if(nto_temp!=NULL)
									free(nto_temp);

								if(nfrom_list_temp!=NULL)
									free(nfrom_list_temp);
								if(nto_list_temp!=NULL)
									free(nto_list_temp);

								if(pathid_list_temp!=NULL)
									free(pathid_list_temp);
								if(link_pos_temp!=NULL)
									free(link_pos_temp);
								if(OCITransCommit(ociHdbc, ociError, 0)!=OCI_SUCCESS)
								{
									if(rescode==0)	
										rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
								}
								GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
								status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);

								return -1;
							}
							else
							{
								memcpy(nfrom_list_temp+nfrom_list_len_prev,nfrom_temp,nfromlen+1);
								memcpy(nto_list_temp+nto_list_len_prev,nto_temp,ntolen+1);
								nfrom_list_len_prev=nfrom_list_len;
								nto_list_len_prev=nto_list_len;

								i++;
							}
						}
						if(i>0 && (i%prefetch_rows)==0)
						{
							round_trip++;
							pathid_list_temp=(int*)realloc(pathid_list_temp,round_trip*prefetch_rows*sizeof(int));
							link_pos_temp=(int*)realloc(link_pos_temp,round_trip*prefetch_rows*sizeof(int));
							if( pathid_list_temp==NULL || link_pos_temp==NULL)
							{
								if(nfrom_temp!=NULL)
									free(nfrom_temp);
								if(nto_temp!=NULL)
									free(nto_temp);

								if(nfrom_list_temp!=NULL)
									free(nfrom_list_temp);
								if(nto_list_temp!=NULL)
									free(nto_list_temp);

								if(pathid_list_temp!=NULL)
									free(pathid_list_temp);
								if(link_pos_temp!=NULL)
									free(link_pos_temp);
								if(OCITransCommit(ociHdbc, ociError, 0)!=OCI_SUCCESS)
								{
									if(rescode==0)	
										rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
								}
								GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
								status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);
								return -1;
							}
						}
					}
					if(status!=OCI_SUCCESS && status != OCI_SUCCESS_WITH_INFO && i==0)
					{
						if(rescode==0)	
						{
							if(OCITransCommit(ociHdbc, ociError, 0)!=OCI_SUCCESS)
							{
								if(rescode==0)	
									rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
							}
							rescode=ShowErrors (status, ociError, "OCIFETCH unsuccessful");
							if(nfrom_temp!=NULL)
								free(nfrom_temp);
							if(nto_temp!=NULL)
								free(nto_temp);


							if(nfrom_list_temp!=NULL)
								free(nfrom_list_temp);
							if(nto_list_temp!=NULL)
								free(nto_list_temp);

							if(pathid_list_temp!=NULL)
								free(pathid_list_temp);
							if(link_pos_temp!=NULL)
								free(link_pos_temp);

							GetErrorMess(appliName, "NO_ROWS_SELECTED",ErrMess,1);
							status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);

							return -1;
						}
					}
					if(nfrom_arraylen>nfrom_list_len && rescode==0)
					{
						nfrom_arraylen=nfrom_list_len;
						memcpy(nfrom_list,nfrom_list_temp,nfrom_list_len);
						if(len_lastnfrominfo>strlen(nfrom_temp))
							strcpy(last_nfrominfo,nfrom_temp);
						else
						{
							len_lastnfrominfo=strlen(nfrom_temp);
							strcpy(last_nfrominfo,"BUFFER_TOO_SMALL");
						}
						status=0;
					}
					else
					{
						nfrom_arraylen=nfrom_list_len;
						status+=-1;
					}

					if(nto_arraylen>nto_list_len  && rescode==0)
					{
						nto_arraylen=nto_list_len;
						memcpy(nto_list,nto_list_temp,nto_list_len);
						if(len_lastntoinfo>strlen(nto_temp))
							strcpy(last_ntoinfo,nto_temp);
						else
						{
							len_lastntoinfo=strlen(nto_temp)+1;
							strcpy(last_ntoinfo,"BUFFER_TOO_SMALL");
						}
						status+=0;
					}
					else
					{
						nto_arraylen=nto_list_len;
						status+=-1;
					}


					if(len_array>i-1  && rescode==0)
					{
						for (j=0;j<i;j++)
						{

							pathid_list[j]=pathid_list_temp[j];
							link_pos_list[j]=link_pos_temp[j];
						}
						len_array=i;
						status+=0;
					}
					else
					{
						len_array=i;
						status+=-1;
					}



				}
				if(nfrom_temp!=NULL)
					free(nfrom_temp);
				if(nto_temp!=NULL)
					free(nto_temp);

				if(nfrom_list_temp!=NULL)
					free(nfrom_list_temp);
				if(nto_list_temp!=NULL)
					free(nto_list_temp);

				if(pathid_list_temp!=NULL)
					free(pathid_list_temp);
				if(link_pos_temp!=NULL)
					free(link_pos_temp);


			}
		}



		if(OCITransCommit(ociHdbc, ociError, 0)!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}

		status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 
		else
			OCIReportError(ociError,appliName, ErrMess,0); 


		return (rescode+status);
	}
	/*************************************************************************/
	/**********************************************************************************************************/
	/**
	* Get the path details  between dev_from and dev_to, returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* you need grant execute if you have your own db account
	* @param mboardportid_from : portid from of the mboard start the micro path
	* @param cpnttype : cpnt type
	* @param cpnttype_given : 1 if you have given a cpnt type, 0 if it should be consider as like 'cpnttype'
	* @param nfrom_list : list of the cpnt which start the link (separated by '\\0') and for the first link  we have cpntname|port_nbr|port_type|port_way
	* @param len_array : length of the pfrom_list (which is equal to pto_list,pathid_list and link_pos_list) you have allocated. If the int * buffer is too small, we put the size needed here.
	* @param nfrom_arraylen : length of the nfrom_list  you have allocated. If the char * buffer is too small, we put the size needed here.
	* @param nto_arraylen : length of the nto_list  you have allocated. If the char * buffer is too small, we put the size needed here.
	* @param last_nfrominfo : details  which start the last link cpntname|port_nbr|port_type|port_way 
	* @param last_ntoinfo : details  which ends the last link cpntname|port_nbr|port_type|port_way 
	* @param len_lastnfrominfo :length of last_nfrominfo you have allocated, if too small we put the size
	* @param len_lastntoinfo :length of last_ntoinfo you have allocated, if too small we put the size
	* @param nto_list : list of the cpnt which end the link concatenation of cpntname|port_nbr and for the last link we have cpntname|port_nbr|port_type|port_way 
	* @param pathid_list : list of the pathid
	* @param link_pos_list : position of the link in the path (start with 1)
	* NB: the i-th entry of the nfrom_list,pfrom_list,nto_list,pto_list,pathid_list and link_pos_list corresponds to one link of the path
	* @param ErrMess : error message in case of failure (even NO_ROWS_SELECTED for no result)
	* @return 0 if it is successful
	*/

	EXTERN_CONFDB
		int GetMicroConnectToCpntType(int mboardportid_from, int cpnttype_given, char* cpnttype,int* pathid_list, int* link_pos_list,int &len_array,char* nfrom_list,int &nfrom_arraylen,char* nto_list,int &nto_arraylen,char* last_nfrominfo,unsigned int &len_lastnfrominfo,char* last_ntoinfo,unsigned int &len_lastntoinfo,char* ErrMess)
	{
		char appliName[100]="GetMicroConnectToCpntType";
		int i=0;
		int j=0;
		int res_code=0;
		int pos2=0;
		int len1=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		sword status;
		int rescode=0;
		int rescode_pl=-1;
		int rescode_pl2=-1;
		int resquery=-1;
		int cpntid=1;
		int portid=-mboardportid_from;
		int free_stmt=0;
		OCIStmt *stmthp;
		OCIBind  *bnd1p[9]; 
		OCIDefine* def[9];
		ub4  descriptionlen=0;
		ub4 devtypelen=0;
		char * nfrom_temp = NULL;
		char *nto_temp=NULL;
		int prefetch_rows=10000;
		int len11=101;
		OCIParam *parmdp;

		int mbpid_from=mboardportid_from;

		int nfrom_list_len=0;
		int nto_list_len=0;
		int nfrom_list_len_prev=0;
		int nto_list_len_prev=0;
		int free_mem=0;
		int pathid=0;
		int round_trip=1;
		int nfromlen=0;
		int ntolen=0;
		int link_pos=0;
		char* nfrom_list_temp=NULL;
		char* nto_list_temp=NULL;
		int mboard=mboardportid_from;
		int* pathid_list_temp=NULL; 
		int* link_pos_temp=NULL;
		char selectdevtype[1000];
		//clock_t start,finish;
		//double time;
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);

		if(status!=OCI_SUCCESS || ociEnv==0)
		{	
			rescode=ShowErrors (status, ociError, "OCIHandleAlloc unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrMess,1); 

			return -1;
		}
		else
		{

			//start = clock();

			//std:://cout <<"value of systemname "<<systemname<<"  and value of dev_from "<<dev_from <<" and dev_to "<< dev_to <<std::endl;
			sprintf(selectdevtype,"BEGIN %s(:mboard,1,:rescode,:cpntid); if :rescode=0 then :resquery:=%s(:portidfrom); else :resquery:=-1; end if; if :resquery=0 then :rcode:=%s(:portidfrom,:cpntname,:cpnttype_given); else :rcode:=-1; end if;END;",_CreateMicroLgLinks,_CreateMicroPaths_Host,_InsertMicroPathCpntType);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevtype,(ub4) strlen(selectdevtype),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			//std:://cout <<"value of selectdevtype= "<<selectdevtype<<" and mboard= "<<mboard<<std::endl;

		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[0], ociError,(text*)":mboard",-1,(dvoid*)&mboard,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[1], ociError,(text*)":cpntname",-1,(dvoid*)cpnttype,strlen(cpnttype)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[2], ociError,(text*)":rescode",-1,(dvoid*)&rescode_pl,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[3], ociError,(text*)":cpntid",-1,(dvoid*)&cpntid,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[4], ociError,(text*)":resquery",-1,(dvoid*)&resquery,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[5], ociError,(text*)":rcode",-1,(dvoid*)&rescode_pl2,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[6], ociError,(text*)":portidfrom",-1,(dvoid*)&mboardportid_from,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[7], ociError,(text*)":cpnttype_given",-1,(dvoid*)&cpnttype_given,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 1, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

		//finish = clock();
		//time = (double(finish)-double(start))/CLOCKS_PER_SEC;  
		//std:://cout <<"value of tabame "<<tabname <<" time elapsed "<<time<<std::endl;
		//std:://cout <<"vallue of maboard= "<<mboard<<"value of rescode_pl2= "<<rescode_pl2<<" value of rescode_pl="<<rescode_pl<<" and value of resquery ="<<resquery<<std::endl;
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
			if(rescode_pl2!=0)
			{
				if(resquery!=0) //means that the PL/SQL fct unsuccessfully executed
				{
					if(rescode_pl!=0)	
					{
						//rescode=ShowErrors (OCI_SUCCESS_WITH_INFO, ociError, "OCIPL/SQL fct unsuccessful");
						//std:://cout <<"length tabname "<<strlen(tabname)<<std::endl;
						if(rescode==-1)
							GetErrorMess(appliName, "Cpnt Name not found",ErrMess,1);
						else
							GetErrorMess(appliName, "Pb when creating the micro lg links",ErrMess,1);

						if(OCITransCommit(ociHdbc, ociError, 0)!=OCI_SUCCESS)
						{
							if(rescode==0)	
								rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
						}
						status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);

						return -1;
					}
					else
					{
						GetErrorMess(appliName, "Pb when finding the micro paths",ErrMess,1);
						if(OCITransCommit(ociHdbc, ociError, 0)!=OCI_SUCCESS)
						{
							if(rescode==0)	
								rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
						}
						status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						return -1;
					}
				}
				else
				{
					GetErrorMess(appliName, "Error  when trying to find the paths",ErrMess,1);
					if(OCITransCommit(ociHdbc, ociError, 0)!=OCI_SUCCESS)
					{
						if(rescode==0)	
							rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
					}
					status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					return -1;
				}
			}
			else
			{
				sprintf(selectdevtype,"select d.pathid,d.link_pos,d.nfrom||'?',d.nto||'?' from %s d order by pathid,link_pos ",MICROPATH_TEMP);
				status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevtype,(ub4) strlen(selectdevtype),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
				//std:://cout <<"value of selectdevtype "<<selectdevtype <<std::endl;

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
				}
				else
					status=OCIAttrSet (stmthp,OCI_HTYPE_STMT,&prefetch_rows,0,OCI_ATTR_PREFETCH_ROWS,ociError);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIAttrSet  unsuccessful");
				}
				else
					status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);
				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIStmtExecute  unsuccessful");
				}
				else
					status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 3);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIParamGet  unsuccessful");
				}
				else
					status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &nfromlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIStmtExecute  unsuccessful");
				}
				else
					status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 4);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIParamGet  unsuccessful");
				}
				else
					status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &ntolen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

				status=OCIDefineByPos (stmthp, &def[0], ociError,1,&pathid,sizeof(pathid), SQLT_INT, 0, 0, 0, OCI_DEFAULT);
				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
				else
					status=OCIDefineByPos (stmthp, &def[1], ociError,2,&link_pos,sizeof(link_pos), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
				else
				{

					nfrom_temp=(char *) realloc(nfrom_temp,(nfromlen+1)*sizeof(char));

					nto_temp= (char *) realloc(nto_temp,(ntolen+1)*sizeof(char));

					if(nfrom_temp==NULL || nto_temp==NULL )
					{	
						if(nfrom_temp!=NULL)
							free(nfrom_temp);
						if(nto_temp!=NULL)
							free(nto_temp);

						GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
						status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);

						return -1;
					}
					else
						status =OCIDefineByPos(stmthp, &def[2], ociError,3, (ub1 *) (nfrom_temp), nfromlen+1,SQLT_STR, (dvoid *) 0,(ub2 *) 0,0, OCI_DEFAULT);	
				}


				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
				else
					status =OCIDefineByPos(stmthp, &def[3], ociError,4, (ub1 *) (nto_temp), ntolen + 1,SQLT_STR, (dvoid *) 0,(ub2 *) 0,0, OCI_DEFAULT);


				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
				else
				{
					i=0;
					pathid_list_temp=(int*)realloc(pathid_list_temp,prefetch_rows*sizeof(int));
					link_pos_temp=(int*)realloc(link_pos_temp,prefetch_rows*sizeof(int));

					if( pathid_list_temp==NULL || link_pos_temp==NULL)
					{
						if(nfrom_temp!=NULL)
							free(nfrom_temp);
						if(nto_temp!=NULL)
							free(nto_temp);

						if(pathid_list_temp!=NULL)
							free(nfrom_temp);
						if(link_pos_temp!=NULL)
							free(nto_temp);

						status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);

						GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
						return -1;
					}
					while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
					{
						status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);
						if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
						{

							pathid_list_temp[i]=pathid;
							link_pos_temp[i]=link_pos;
							ntolen=strcspn(nto_temp,"?");
							nto_list_len+=ntolen+1;
							nto_temp[ntolen]='\0';

							nfromlen=strcspn(nfrom_temp,"?");
							nfrom_temp[nfromlen]='\0';
							nfrom_list_len+=nfromlen+1;
							nfrom_list_temp=(char *) realloc(nfrom_list_temp,nfrom_list_len*sizeof(char));
							nto_list_temp=(char *) realloc(nto_list_temp,nto_list_len*sizeof(char));
							if(nfrom_list_temp==NULL || nto_list_temp==NULL )
							{
								if(nfrom_temp!=NULL)
									free(nfrom_temp);
								if(nto_temp!=NULL)
									free(nto_temp);

								if(nfrom_list_temp!=NULL)
									free(nfrom_list_temp);
								if(nto_list_temp!=NULL)
									free(nto_list_temp);

								if(pathid_list_temp!=NULL)
									free(pathid_list_temp);
								if(link_pos_temp!=NULL)
									free(link_pos_temp);
								if(OCITransCommit(ociHdbc, ociError, 0)!=OCI_SUCCESS)
								{
									if(rescode==0)	
										rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
								}
								GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
								status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);

								return -1;
							}
							else
							{
								memcpy(nfrom_list_temp+nfrom_list_len_prev,nfrom_temp,nfromlen+1);
								memcpy(nto_list_temp+nto_list_len_prev,nto_temp,ntolen+1);
								nfrom_list_len_prev=nfrom_list_len;
								nto_list_len_prev=nto_list_len;

								i++;
							}
						}
						if(i>0 && (i%prefetch_rows)==0)
						{
							round_trip++;
							pathid_list_temp=(int*)realloc(pathid_list_temp,round_trip*prefetch_rows*sizeof(int));
							link_pos_temp=(int*)realloc(link_pos_temp,round_trip*prefetch_rows*sizeof(int));
							if( pathid_list_temp==NULL || link_pos_temp==NULL)
							{
								if(nfrom_temp!=NULL)
									free(nfrom_temp);
								if(nto_temp!=NULL)
									free(nto_temp);

								if(nfrom_list_temp!=NULL)
									free(nfrom_list_temp);
								if(nto_list_temp!=NULL)
									free(nto_list_temp);

								if(pathid_list_temp!=NULL)
									free(pathid_list_temp);
								if(link_pos_temp!=NULL)
									free(link_pos_temp);
								if(OCITransCommit(ociHdbc, ociError, 0)!=OCI_SUCCESS)
								{
									if(rescode==0)	
										rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
								}
								GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
								status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);
								return -1;
							}
						}
					}
					if(status!=OCI_SUCCESS && status != OCI_SUCCESS_WITH_INFO && i==0)
					{
						if(rescode==0)	
						{
							if(OCITransCommit(ociHdbc, ociError, 0)!=OCI_SUCCESS)
							{
								if(rescode==0)	
									rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
							}
							rescode=ShowErrors (status, ociError, "OCIFETCH unsuccessful");
							if(nfrom_temp!=NULL)
								free(nfrom_temp);
							if(nto_temp!=NULL)
								free(nto_temp);


							if(nfrom_list_temp!=NULL)
								free(nfrom_list_temp);
							if(nto_list_temp!=NULL)
								free(nto_list_temp);

							if(pathid_list_temp!=NULL)
								free(pathid_list_temp);
							if(link_pos_temp!=NULL)
								free(link_pos_temp);

							GetErrorMess(appliName, "NO_ROWS_SELECTED",ErrMess,1);
							status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);

							return 0;
						}
					}
					if(nfrom_arraylen>nfrom_list_len && rescode==0)
					{
						nfrom_arraylen=nfrom_list_len;
						memcpy(nfrom_list,nfrom_list_temp,nfrom_list_len);
						if(len_lastnfrominfo>strlen(nfrom_temp))
							strcpy(last_nfrominfo,nfrom_temp);
						else
						{
							len_lastnfrominfo=strlen(nfrom_temp);
							strcpy(last_nfrominfo,"BUFFER_TOO_SMALL");
						}
						status=0;
					}
					else
					{
						nfrom_arraylen=nfrom_list_len;
						status+=-1;
					}

					if(nto_arraylen>nto_list_len  && rescode==0)
					{
						nto_arraylen=nto_list_len;
						memcpy(nto_list,nto_list_temp,nto_list_len);
						if(len_lastntoinfo>strlen(nto_temp))
							strcpy(last_ntoinfo,nto_temp);
						else
						{
							len_lastntoinfo=strlen(nto_temp)+1;
							strcpy(last_ntoinfo,"BUFFER_TOO_SMALL");
						}
						status+=0;
					}
					else
					{
						nto_arraylen=nto_list_len;
						status+=-1;
					}


					if(len_array>i-1  && rescode==0)
					{
						for (j=0;j<i;j++)
						{

							pathid_list[j]=pathid_list_temp[j];
							link_pos_list[j]=link_pos_temp[j];
						}
						len_array=i;
						status+=0;
					}
					else
					{
						len_array=i;
						status+=-1;
					}



				}
				if(nfrom_temp!=NULL)
					free(nfrom_temp);
				if(nto_temp!=NULL)
					free(nto_temp);

				if(nfrom_list_temp!=NULL)
					free(nfrom_list_temp);
				if(nto_list_temp!=NULL)
					free(nto_list_temp);

				if(pathid_list_temp!=NULL)
					free(pathid_list_temp);
				if(link_pos_temp!=NULL)
					free(link_pos_temp);


			}
		}



		if(OCITransCommit(ociHdbc, ociError, 0)!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}

		status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 
		else
			OCIReportError(ociError,appliName, ErrMess,0); 


		return (rescode+status);
	}
	/**********************************************************************************************************/
	/**
	* Load the routing table of a switch (used for the DAQ)
	* The user should manage the memory : there is no memory allocation.
	* you need grant execute if you have your own db account
	* @param switch_name : name of the switch
	* @param len_array_destinname : length of the destinname_list (correspond to the memory allocation). if too small, we put the size here.
	* @param destinname_list : list of the destination name (separated with '\\0')
	* @param len_array_destinip : length of the destinip_list (correspond to the memory allocation). if too small, we put the size here.
	* @param destinip_list : list of the ipaddress destination (separated by '\\0') 
	* @param len_array_portnext : length of the port_list (correspond to the memory allocation). if too small, we put the size here.
	* @param port_list : list of the port (concatenation of port_nbr|port_type|port_way). (each row is separated by '\\0') 
	* @param len_array_ipaddnext : length of the ipaddnext_list  you have allocated. If the char * buffer is too small, we put the size needed here.
	* @param ipaddnext_list : list of the ip address of the next hop (separated by '\\0') 
	* @param len_array_subnetnext : length of the subnetnext_list  (correspond to the memory allocation). if too small, we put the size here.
	* @param subnetnext_list : list of the subnet mask of the next hop
	* @param len_array_macaddnext : length of the macaddnext_list  you have allocated. If the char * buffer is too small, we put the size needed here.
	* @param macaddnext_list : list of the mac address of the next hop (separated by '\\0') 
	* @param ErrMess : error message in case of failure (even NO_ROWS_SELECTED for no result)
	* @return 0 if it is successful
	*/
	/************************************************************************************/
	EXTERN_CONFDB
		int LoadRoutingtable( char* switch_name,int &len_array_destinname,char* destinname_list, int &len_array_destinip,char* destinip_list, int &len_array_portnext,char* port_list,int &len_array_ipaddnext,char* ipaddnext_list,int &len_array_subnetnext,char* subnetnext_list,int &len_array_macaddnext,char* macaddnext_list,char* ErrMess)
	{
		clock_t start,finish;
		double time;
		start=clock();

		char appliName[100]="LoadRoutingtable";
		OCIStmt *stmthp;
		OCIBind  *bnd1p[1]; 
		OCIDefine* def[6];
		char selectdevtype[2000];
		char* portinfo=NULL;
		int mac_len=25;
		char ipadd_next[25];
		char ipadd_destin[25];
		char subnet_next[25];
		char macadd_next[25];
		char* destin_name=NULL;
		int destin_len=0;
		int portinfo_len=0;
		int prefetch_rows=5000;
		char* portlist_temp=NULL;
		char* destinlist_temp=NULL;
		char* ipnextlist_temp=NULL;
		char* subnetlist_temp=NULL;
		char* ipdestinlist_temp=NULL;
		char* macaddlist_temp=NULL;
		int rescode=0;
		int status=0;
		OCIParam *parmdp;

		int pos1,pos2,pos3,pos4,pos5,pos11,pos22,pos33,pos44,pos55,pos11_bis,pos22_bis,pos33_bis,pos44_bis,pos55_bis,pos6,pos66,pos66_bis;
		//clock_t start,finish;
		//double time;
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);

		if(status!=OCI_SUCCESS || ociEnv==0)
		{	
			rescode=ShowErrors (status, ociError, "OCIHandleAlloc unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrMess,1); 

			return -1;
		}
		else
		{
			sprintf(selectdevtype,"select t.port_nbr||'|'||nvl(t.port_type,'none')||'|'||t.port_way||'?', e.ipaddress||'?',e.subnet_info||'?',f.ipaddress||'?',m.devicename||'?',u.macaddress||'?' from %s t,%s p,%s e,%s f, %s r,%s m,%s u,%s d where d.deviceid=p.deviceid and u.serialnb=d.serialnb and p.port_nbr=u.port_nbr and nvl(p.port_type,'none')=nvl(u.port_type,'none') and u.port_way=p.port_way and p.ipaddress=e.ipaddress and p.portid=r.ptoid0 and t.portid=r.pfromid0 and r.ptoid1=f.portid and r.routingpathused=1 and r.nodeid_start0=(select deviceid from %s where devicename=:dfrom) and r.nodeid_end1=m.deviceid", PORT_TABLE,PORT_TABLE,IPINFO_TABLE,PORT_TABLE,ROUTINGTABLE,LOGICAL_DEVICE_TABLE,HWPORT_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevtype,(ub4) strlen(selectdevtype),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);

		}
		//std:://cout << "switch_name ...." <<switch_name<<  std::endl;
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[0], ociError,(text*)":dfrom",-1,(dvoid*)switch_name,strlen(switch_name)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIAttrSet (stmthp,OCI_HTYPE_STMT,&prefetch_rows,0,OCI_ATTR_PREFETCH_ROWS,ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrSet  unsuccessful");
		}
		else
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute  unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 1);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet  unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &portinfo_len, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet  unsuccessful");
		}
		else
		{
			portinfo=(char*)malloc(portinfo_len*sizeof(char));
			if(portinfo!=NULL)
				status=OCIDefineByPos (stmthp, &def[0], ociError,1,portinfo,portinfo_len, SQLT_STR, 0, 0, 0, OCI_DEFAULT);
			else
			{
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"MALLOC UNSUCCESSFUL",ErrMess,1);
				return rescode;
			}
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[1], ociError,2,ipadd_next,mac_len, SQLT_STR, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[2], ociError,3,subnet_next,mac_len, SQLT_STR, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[3], ociError,4,ipadd_destin,mac_len, SQLT_STR, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute  unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 5);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet  unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &destin_len, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet  unsuccessful");
		}
		else
		{
			destin_name=(char*)malloc(destin_len*sizeof(char));
			if(destin_name!=NULL)
				status=OCIDefineByPos (stmthp, &def[4], ociError,5,destin_name,destin_len, SQLT_STR, 0, 0, 0, OCI_DEFAULT);
			else
			{
				rescode=-1;
				free(portinfo);
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"MALLOC UNSUCCESSFUL",ErrMess,1);
				return rescode;	
			}
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet  unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[5], ociError,6,macadd_next,mac_len, SQLT_STR, 0, 0, 0, OCI_DEFAULT);
		if(status==OCI_SUCCESS)
		{
			pos11=0;
			pos22=0;
			pos33=0;
			pos44=0;
			pos55=0;
			pos66=0;
			pos11_bis=0;
			pos22_bis=0;
			pos33_bis=0;
			pos44_bis=0;
			pos55_bis=0;
			pos66_bis=0;
			//std:://cout<<"ebefore loop"<<std::endl;
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT, 1,OCI_DEFAULT);

				OCIReportError(ociError,appliName, ErrMess,1); 
				//std:://cout<<"in the loop status="<<ErrMess<<std::endl;
				if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
				{
					pos1=strcspn(portinfo,"?");
					portinfo[pos1]='\0';
					pos11+=pos1+1;
					pos2=strcspn(destin_name,"?");
					destin_name[pos2]='\0';
					pos22+=pos2+1;
					pos3=strcspn(ipadd_next,"?");
					ipadd_next[pos3]='\0';
					pos33+=pos3+1;
					pos4=strcspn(subnet_next,"?");
					subnet_next[pos4]='\0';
					pos44+=pos4+1;
					pos5=strcspn(ipadd_destin,"?");
					ipadd_destin[pos5]='\0';
					pos55+=pos5+1;
					pos6=strcspn(macadd_next,"?");
					macadd_next[pos6]='\0';
					pos66+=pos6+1;
					portlist_temp=(char*) realloc(portlist_temp,pos11*sizeof(char));
					destinlist_temp=(char*) realloc(destinlist_temp,pos22*sizeof(char));
					ipnextlist_temp=(char*) realloc(ipnextlist_temp,pos33*sizeof(char));
					subnetlist_temp=(char*) realloc(subnetlist_temp,pos44*sizeof(char));
					ipdestinlist_temp=(char*) realloc(ipdestinlist_temp,pos55*sizeof(char));
					macaddlist_temp=(char*) realloc(macaddlist_temp,pos66*sizeof(char));

					if(portlist_temp==NULL||destinlist_temp==NULL||ipnextlist_temp==NULL||subnetlist_temp==NULL||ipdestinlist_temp==NULL||macaddlist_temp==NULL)
					{

						rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
						rescode=-1;
						free(destin_name);
						free(portinfo);
						if(portlist_temp!=NULL)
							free(portlist_temp);
						if(destinlist_temp!=NULL)
							free(destinlist_temp);
						if(ipnextlist_temp!=NULL)
							free(ipnextlist_temp);
						if(ipdestinlist_temp!=NULL)
							free(ipdestinlist_temp);
						if(subnetlist_temp!=NULL)
							free(subnetlist_temp);
						if(macaddlist_temp!=NULL)
							free(macaddlist_temp);
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrMess,1);
						return rescode;	
					}
					else
					{
						memcpy(portlist_temp+pos11_bis,portinfo,pos1+1);
						memcpy(destinlist_temp+pos22_bis,destin_name,pos2+1);
						memcpy(ipnextlist_temp+pos33_bis,ipadd_next,pos3+1);
						memcpy(subnetlist_temp+pos44_bis,subnet_next,pos4+1);
						memcpy(ipdestinlist_temp+pos55_bis,ipadd_destin,pos5+1);
						memcpy(macaddlist_temp+pos66_bis,macadd_next,pos6+1);
						pos11_bis=pos11;
						pos22_bis=pos22;
						pos33_bis=pos33;
						pos44_bis=pos44;
						pos55_bis=pos55;
						pos66_bis=pos66;
					}
				}
			}
			status=0;
			//std:://cout<<"pos3="<<len_array_subnetnext<<std::endl;
			//std:://cout<<"pos4="<<len_array_destinip<<std::endl;
			//std:://cout<<"pos5="<<len_array_portnext<<std::endl;
			//std:://cout<<"pos6="<<rescode<<std::endl;
			if(pos22>0)
			{
				if(len_array_destinname>pos22)
				{
					memcpy(destinname_list,destinlist_temp,pos22);
					len_array_destinname=pos22;
				}
				else
				{
					len_array_destinname=pos22;
					rescode=-1;
				}
				if(len_array_ipaddnext>pos33)
				{
					memcpy(ipaddnext_list,ipnextlist_temp,pos33);
					len_array_ipaddnext=pos33;
				}
				else
				{
					rescode=-1;
					len_array_ipaddnext=pos33;
				}
				if(len_array_portnext>pos11)
				{
					memcpy(port_list,portlist_temp,pos11);
					len_array_portnext=pos11;
				}
				else
				{
					rescode=-1;
					len_array_portnext=pos11;
				}
				if(len_array_subnetnext>pos44)
				{
					memcpy(subnetnext_list,subnetlist_temp,pos44);
					len_array_subnetnext=pos44;
				}
				else
				{
					rescode=-1;
					len_array_subnetnext=pos44;
				}
				if(len_array_destinip>pos55)
				{
					memcpy(destinip_list,ipdestinlist_temp,pos55);
					len_array_destinip=pos55;
				}
				else
				{
					rescode=-1;
					len_array_destinip=pos55;
				}
				if(len_array_macaddnext>pos66)
				{
					memcpy(macaddnext_list,macaddlist_temp,pos66);
					len_array_macaddnext=pos66;
					rescode=0;
				}
				else
				{
					rescode=-1;
					len_array_macaddnext=pos66;
				}
				if(rescode==-1)
				{
					free(destin_name);
					free(portinfo);
					if(portlist_temp!=NULL)
						free(portlist_temp);
					if(destinlist_temp!=NULL)
						free(destinlist_temp);
					if(ipnextlist_temp!=NULL)
						free(ipnextlist_temp);
					if(ipdestinlist_temp!=NULL)
						free(ipdestinlist_temp);
					if(subnetlist_temp!=NULL)
						free(subnetlist_temp);
					if(macaddlist_temp!=NULL)
						free(macaddlist_temp);
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrMess,1);
					return rescode;	
				}
			}
			else
			{
				rescode=-1;
				free(destin_name);
				free(portinfo);
				if(portlist_temp!=NULL)
					free(portlist_temp);
				if(destinlist_temp!=NULL)
					free(destinlist_temp);
				if(ipnextlist_temp!=NULL)
					free(ipnextlist_temp);
				if(ipdestinlist_temp!=NULL)
					free(ipdestinlist_temp);
				if(subnetlist_temp!=NULL)
					free(subnetlist_temp);
				if(macaddlist_temp!=NULL)
					free(macaddlist_temp);
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrMess,1);
				return rescode;	
			}
		}
		else
		{
			free(destin_name);
			free(portinfo);
		}
		status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 
		else
			OCIReportError(ociError,appliName, ErrMess,0); 
		finish = clock();
		time = (double(finish)-double(start))/CLOCKS_PER_SEC;  
		//std::cout<<"method 1 time= "<<time <<std::endl;
		return (rescode+status);
	}

	/************************************/
	/**********************************************************************************************************/
	/**
	* Get the path details  between dev_from and dev_to,  returning an integer value. NOT TO BE USED ONLY FOR INTERNAL USAGE
	* The user should manage the memory : there is no memory allocation.
	* you need grant execute if you have your own db account
	* @param dev_from : name of the device 1 (ex. THOR_00)
	* @param dev_to : name of the device 2 (ex. ECAL_L1FE)
	* @param nfrom_list : list of the devices which start the link (separated by '\\0')
	* @param len_array : length of the pfrom_list (which is equal to pto_list,pathid_list and link_pos_list) you have allocated. If the int * buffer is too small, we put the size needed here.
	* @param nfrom_arraylen : length of the nfrom_list  you have allocated. If the char * buffer is too small, we put the size needed here.
	* @param nto_arraylen : length of the nto_list  you have allocated. If the char * buffer is too small, we put the size needed here.
	* @param pfrom_arraylen : length of the pfrom_list  you have allocated. If the char * buffer is too small, we put the size needed here.
	* @param pto_arraylen : length of the pto_list  you have allocated. If the char * buffer is too small, we put the size needed here.
	* @param lkinfo_arraylen : length of the llkinfo_list  you have allocated. If the char * buffer is too small, we put the size needed here.
	* @param pfrom_list : list of the port from numbers (+ type)
	* @param pto_list : list of the port to numbers (+ type)
	* @param nto_list : list of the devices which end the link
	* @param pathid_list : list of the pathid
	* @param link_pos_list : position of the link in the path (start with 1)
	* @param link_info_list : info about the link
	* NB: the i-th entry of the nfrom_list,pfrom_list,nto_list,pto_list,pathid_list and link_pos_list corresponds to one link of the path
	* @param ErrMess : error message in case of failure (even NO_ROWS_SELECTED for no result)
	* @return 0 if it is successful
	*/
	EXTERN_CONFDB
		int GetDetailedConnectivityBetweenDevices_plsql(char* dev_from, char* dev_to, char* nfrom_list,int& len_array,int & nfrom_arraylen,int & pfrom_arraylen, int & nto_arraylen, int & pto_arraylen,int &lkinfo_arraylen,char* pfrom_list,char* pto_list,int* pfrom1_list,int* pto1_list,char* nto_list,int* pathid_list, int* link_pos_list,char* lkinfo_list,char* ErrMess)
	{

		char appliName[100]="GetDetailedConnectivityBetweenDevices_plsql";
		int i=0;
		int j=0;
		int res_code=0;
		int pos2=0;
		int len1=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		sword status;
		int rescode=0;
		int free_stmt=0;
		OCIStmt *stmthp;

		OCIBind  *bnd1p[6]; 
		OCIDefine* def[9];
		ub4  descriptionlen=0;
		ub4 devtypelen=0;
		char * nfrom_temp = NULL;
		char *nto_temp=NULL;
		int prefetch_rows=10000;
		int len11=101;
		char * tabname=(char*)malloc(len11*sizeof(char));
		char * pfrom_temp = NULL;
		char *pto_temp=NULL;
		char *lkinfo_temp=NULL;
		int pfromlen=0;
		int ptolen=0;
		int lkinfolen=0;
		int lkinfo_list_len=0;
		int nfrom_list_len=0;
		int nto_list_len=0;
		int nfrom_list_len_prev=0;
		int nto_list_len_prev=0;
		int pfrom_list_len=0;
		int pto_list_len=0;
		int pfrom_list_len_prev=0;
		int pto_list_len_prev=0;
		int free_mem=0;
		int pathid=0;
		int pto=0;
		int pfrom=0;
		int round_trip=1;
		int nfromlen=0;
		int ntolen=0;
		int link_pos=0;
		char* nfrom_list_temp=NULL;
		char* pfrom1_list_temp=NULL;
		int* pfrom_list_temp=NULL;
		int* pto_list_temp=NULL;
		char* nto_list_temp=NULL;
		char* pto1_list_temp=NULL;
		char* lkinfo_list_temp=NULL;
		int* pathid_list_temp=NULL; 
		int* link_pos_temp=NULL;
		char selectdevtype[1000];
		//clock_t start,finish;
		//double time;
		int lkinfo_list_len_prev=0;
		if(tabname==NULL)
		{
			GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
			return -1;
		}
		else
		{
			status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
			strcpy(tabname,"tetst");
		}
		if(status!=OCI_SUCCESS)
		{	
			rescode=ShowErrors (status, ociError, "OCIHandleAlloc unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrMess,1); 
			free(tabname);
			return -1;
		}
		else
		{
			//start = clock();

			//std:://cout <<"value of systemname "<<systemname<<"  and value of dev_from "<<dev_from <<" and dev_to "<< dev_to <<std::endl;
			sprintf(selectdevtype,"select t.port_nbr||'|'||nvl(t.port_type,'none')||'|'||t.port_way||'?', e.ipaddress||'?',e.subnet_info||'?',f.ipaddress||'?',m.devicename||'?',u.macaddress||'?' from %s t,%s p,%s e,%s f, %s r,%s m,%s u,%s d where d.deviceid=p.deviceid and u.serialnb=d.serialnb and t.port_nbr=u.port_nbr and nvl(t.port_type,'none')=nvl(u.port_type,'none') and u.port_way=p.port_way and p.ipaddress=e.ipaddress and p.portid=r.ptoid0 and t.portid=r.pfromid0 and r.ptoid1=f.portid and r.routingpathused=1 and r.nodeid_start0=:dfrom and r.nodeid_end1=m.deviceid", PORT_TABLE,PORT_TABLE,IPINFO_TABLE,PORT_TABLE,ROUTINGTABLE,LOGICAL_DEVICE_TABLE,HWPORT_TABLE,LOGICAL_DEVICE_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevtype,(ub4) strlen(selectdevtype),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[0], ociError,(text*)":tabname",-1,(dvoid*)tabname,len11, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[1], ociError,(text*)":devf",-1,(dvoid*)dev_from,strlen(dev_from)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[2], ociError,(text*)":devto",-1,(dvoid*)dev_to,strlen(dev_to)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);




		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 1, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

		//finish = clock();
		//time = (double(finish)-double(start))/CLOCKS_PER_SEC;  
		//std:://cout <<"value of tabame "<<tabname <<" time elapsed "<<time<<std::endl;

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
			if(strncmp(tabname,"ERROR",5)==0) //means that the PL/SQL fct unsuccessfully executed
			{
				if(rescode==0)	
				{
					//rescode=ShowErrors (OCI_SUCCESS_WITH_INFO, ociError, "OCIPL/SQL fct unsuccessful");
					//std:://cout <<"length tabname "<<strlen(tabname)<<std::endl;
					GetErrorMess(appliName, tabname,ErrMess,1);
					status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					free(tabname);
					return -1;
				}
			}
			else
			{
				sprintf(selectdevtype,"select d.pathid,d.link_pos,t.devicename||'?',d.pfrom,d.port_way1, g.devicename||'?',d.pto,d.port_way2,nvl(l.link_info,'none')||'?' from %s d, %s t,%s g,%s l,%s p where p.portid=decode(d.port_way1,1,l.portidto,l.portidfrom) and p.deviceid=d.nfrom and p.port_nbr||'|'||nvl(p.port_type,'none')=d.pfrom and d.port_way1=p.port_way and  d.nfrom=t.deviceid and d.nto=g.deviceid order by pathid,link_pos ",PATH_TEMP,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,MACRO_CONNECTIVITY_TABLE,PORT_TABLE);
				status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevtype,(ub4) strlen(selectdevtype),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
				//std:://cout <<"value of selectdevtype "<<selectdevtype <<std::endl;

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
				}
				else
					status=OCIAttrSet (stmthp,OCI_HTYPE_STMT,&prefetch_rows,0,OCI_ATTR_PREFETCH_ROWS,ociError);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIAttrSet  unsuccessful");
				}
				else
					status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCI execute  unsuccessful");
				}
				else
					status=OCIDefineByPos (stmthp, &def[0], ociError,1,&pathid,sizeof(pathid), SQLT_INT, 0, 0, 0, OCI_DEFAULT);
				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
				else
					status=OCIDefineByPos (stmthp, &def[1], ociError,2,&link_pos,sizeof(link_pos), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
				else
				{
					nfromlen=200;
					nfrom_temp=(char *) realloc(nfrom_temp,(nfromlen+1)*sizeof(char));
					ntolen=200;
					nto_temp= (char *) realloc(nto_temp,(ntolen+1)*sizeof(char));
					pfromlen=200;
					pfrom_temp=(char *) realloc(pfrom_temp,(pfromlen+1)*sizeof(char));
					ptolen=200;
					pto_temp= (char *) realloc(pto_temp,(ptolen+1)*sizeof(char));
					lkinfolen=1000;
					lkinfo_temp=(char *) realloc(lkinfo_temp,(lkinfolen+1)*sizeof(char));
					free_mem=1;
					if(nfrom_temp==NULL || nto_temp==NULL ||pfrom_temp==NULL||pto_temp==NULL||lkinfo_temp==NULL)
					{	
						if(nfrom_temp!=NULL)
							free(nfrom_temp);
						if(nto_temp!=NULL)
							free(nto_temp);
						if(pfrom_temp!=NULL)
							free(pfrom_temp);
						if(pto_temp!=NULL)
							free(pto_temp);
						if(lkinfo_temp!=NULL)
							free(lkinfo_temp);
						free(tabname);
						GetErrorMess(appliName, "Malloc unsuccessful first",ErrMess,1);
						status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);

						return -1;
					}
					else
						status =OCIDefineByPos(stmthp, &def[2], ociError,3, (ub1 *) (nfrom_temp), nfromlen+1,SQLT_STR, (dvoid *) 0,(ub2 *) 0,0, OCI_DEFAULT);	
				}

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
				else
					status=OCIDefineByPos (stmthp, &def[3], ociError,4,pfrom_temp,pfromlen+1, SQLT_STR, 0, 0, 0, OCI_DEFAULT);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
				else
					status=OCIDefineByPos (stmthp, &def[4], ociError,5,&pfrom,sizeof(int), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
				else
					status =OCIDefineByPos(stmthp, &def[5], ociError,6, (ub1 *) (nto_temp), ntolen + 1,SQLT_STR, (dvoid *) 0,(ub2 *) 0,0, OCI_DEFAULT);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
				else
					status=OCIDefineByPos (stmthp, &def[6], ociError,7,pto_temp,ptolen+1,SQLT_STR, 0, 0, 0, OCI_DEFAULT);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
				else
					status=OCIDefineByPos (stmthp, &def[7], ociError,8,&pto,sizeof(int), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
				else
					status=OCIDefineByPos (stmthp, &def[8], ociError,9,lkinfo_temp,lkinfolen+1, SQLT_STR, 0, 0, 0, OCI_DEFAULT);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
				else
				{
					i=0;
					pfrom_list_temp=(int*)realloc(pfrom_list_temp,prefetch_rows*sizeof(int));
					pto_list_temp=(int*)realloc(pto_list_temp,prefetch_rows*sizeof(int));
					pathid_list_temp=(int*)realloc(pathid_list_temp,prefetch_rows*sizeof(int));
					link_pos_temp=(int*)realloc(link_pos_temp,prefetch_rows*sizeof(int));
					free_mem=2;
					if(pfrom_list_temp==NULL || pto_list_temp==NULL || pathid_list_temp==NULL || link_pos_temp==NULL)
					{
						if(nfrom_temp!=NULL)
							free(nfrom_temp);
						if(nto_temp!=NULL)
							free(nto_temp);
						if(pfrom_list_temp!=NULL)
							free(nfrom_temp);
						if(pto_list_temp!=NULL)
							free(nto_temp);
						if(pathid_list_temp!=NULL)
							free(nfrom_temp);
						if(link_pos_temp!=NULL)
							free(nto_temp);
						free(tabname);
						status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);


						GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
						return -1;
					}
					while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
					{
						status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);
						if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
						{
							pfrom_list_temp[i]=pfrom;
							pto_list_temp[i]=pto;
							pathid_list_temp[i]=pathid;
							link_pos_temp[i]=link_pos;

							ntolen=strcspn(nto_temp,"?");
							nto_list_len+=ntolen+1;
							nto_temp[ntolen]='\0';

							lkinfolen=strcspn(lkinfo_temp,"?");
							lkinfo_list_len+=lkinfolen+1;
							lkinfo_temp[lkinfolen]='\0';

							ptolen=strcspn(pto_temp,"?");
							pto_list_len+=ptolen+1;
							pto_temp[ptolen]='\0';
							nfromlen=strcspn(nfrom_temp,"?");
							nfrom_temp[nfromlen]='\0';
							nfrom_list_len+=nfromlen+1;
							pfromlen=strcspn(pfrom_temp,"?");
							pfrom_temp[pfromlen]='\0';
							pfrom_list_len+=pfromlen+1;
							nfrom_list_temp=(char *) realloc(nfrom_list_temp,nfrom_list_len*sizeof(char));
							nto_list_temp=(char *) realloc(nto_list_temp,nto_list_len*sizeof(char));
							pfrom1_list_temp=(char *) realloc(pfrom1_list_temp,nfrom_list_len*sizeof(char));
							pto1_list_temp=(char *) realloc(pto1_list_temp,nto_list_len*sizeof(char));
							lkinfo_list_temp=(char *) realloc(lkinfo_list_temp,lkinfo_list_len*sizeof(char));
							if(lkinfo_list_temp==NULL||nfrom_list_temp==NULL || nto_list_temp==NULL ||pfrom1_list_temp==NULL||pto1_list_temp==NULL)
							{
								if(nfrom_temp!=NULL)
									free(nfrom_temp);
								if(nto_temp!=NULL)
									free(nto_temp);
								if(pfrom_list_temp!=NULL)
									free(pfrom_list_temp);
								if(pto_list_temp!=NULL)
									free(pto_list_temp);
								if(nfrom_list_temp!=NULL)
									free(nfrom_list_temp);
								if(nto_list_temp!=NULL)
									free(nto_list_temp);
								if(pfrom1_list_temp!=NULL)
									free(pfrom1_list_temp);
								if(pto1_list_temp!=NULL)
									free(pto1_list_temp);
								if(pathid_list_temp!=NULL)
									free(pathid_list_temp);
								if(link_pos_temp!=NULL)
									free(link_pos_temp);
								if(lkinfo_temp!=NULL)
									free(lkinfo_temp);
								if(lkinfo_list_temp!=NULL)
									free(lkinfo_list_temp);
								free(tabname);
								GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
								status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);

								return -1;
							}
							else
							{
								memcpy(nfrom_list_temp+nfrom_list_len_prev,nfrom_temp,nfromlen+1);
								memcpy(nto_list_temp+nto_list_len_prev,nto_temp,ntolen+1);
								memcpy(pfrom1_list_temp+pfrom_list_len_prev,pfrom_temp,pfromlen+1);
								memcpy(pto1_list_temp+pto_list_len_prev,pto_temp,ptolen+1);
								memcpy(lkinfo_list_temp+lkinfo_list_len_prev,lkinfo_temp,lkinfolen+1);
								nfrom_list_len_prev=nfrom_list_len;
								nto_list_len_prev=nto_list_len;
								pfrom_list_len_prev=pfrom_list_len;
								pto_list_len_prev=pto_list_len;
								lkinfo_list_len_prev=lkinfo_list_len;
								i++;

							}
						}
						if(i>0 && (i%prefetch_rows)==0)
						{
							round_trip++;
							pfrom_list_temp=(int*)realloc(pfrom_list_temp,round_trip*prefetch_rows*sizeof(int));
							pto_list_temp=(int*)realloc(pto_list_temp,round_trip*prefetch_rows*sizeof(int));
							pathid_list_temp=(int*)realloc(pathid_list_temp,round_trip*prefetch_rows*sizeof(int));
							link_pos_temp=(int*)realloc(link_pos_temp,round_trip*prefetch_rows*sizeof(int));
							if(pfrom_list_temp==NULL || pto_list_temp==NULL || pathid_list_temp==NULL || link_pos_temp==NULL)
							{
								if(nfrom_temp!=NULL)
									free(nfrom_temp);
								if(nto_temp!=NULL)
									free(nto_temp);
								if(pfrom_list_temp!=NULL)
									free(pfrom_list_temp);
								if(pto_list_temp!=NULL)
									free(pto_list_temp);
								if(nfrom_list_temp!=NULL)
									free(nfrom_list_temp);
								if(nto_list_temp!=NULL)
									free(nto_list_temp);
								if(pfrom1_list_temp!=NULL)
									free(pfrom1_list_temp);
								if(pto1_list_temp!=NULL)
									free(pto1_list_temp);
								if(pathid_list_temp!=NULL)
									free(pathid_list_temp);
								if(link_pos_temp!=NULL)
									free(link_pos_temp);
								if(lkinfo_temp!=NULL)
									free(lkinfo_temp);
								if(lkinfo_list_temp!=NULL)
									free(lkinfo_list_temp);
								free(tabname);
								GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
								status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);

								return -1;
							}
						}
					}
					if(status!=OCI_SUCCESS && status != OCI_SUCCESS_WITH_INFO && i==0)
					{
						if(rescode==0)	
						{
							rescode=ShowErrors (status, ociError, "OCIFETCH unsuccessful");
							if(nfrom_temp!=NULL)
								free(nfrom_temp);
							if(nto_temp!=NULL)
								free(nto_temp);
							if(pfrom_list_temp!=NULL)
								free(pfrom_list_temp);
							if(pto_list_temp!=NULL)
								free(pto_list_temp);
							if(nfrom_list_temp!=NULL)
								free(nfrom_list_temp);
							if(nto_list_temp!=NULL)
								free(nto_list_temp);
							if(pfrom1_list_temp!=NULL)
								free(pfrom1_list_temp);
							if(pto1_list_temp!=NULL)
								free(pto1_list_temp);
							if(pathid_list_temp!=NULL)
								free(pathid_list_temp);
							if(link_pos_temp!=NULL)
								free(link_pos_temp);
							if(lkinfo_temp!=NULL)
								free(lkinfo_temp);
							if(lkinfo_list_temp!=NULL)
								free(lkinfo_list_temp);
							free(tabname);
							GetErrorMess(appliName, "NO_ROWS_SELECTED",ErrMess,1);
							status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);

							return -1;
						}
					}





					if(nfrom_arraylen>nfrom_list_len && rescode==0)
					{
						nfrom_arraylen=nfrom_list_len;
						memcpy(nfrom_list,nfrom_list_temp,nfrom_list_len);			
						status=0;
					}
					else
					{
						nfrom_arraylen=nfrom_list_len;
						GetErrorMess(appliName, "BUFFER_TOO_SMALL",ErrMess,1);
						status=-1;
					}
					if(lkinfo_arraylen>lkinfo_list_len && rescode==0)
					{
						lkinfo_arraylen=lkinfo_list_len;
						memcpy(lkinfo_list,lkinfo_list_temp,lkinfo_list_len);
						status=0;
					}
					else
					{
						lkinfo_arraylen=lkinfo_list_len;
						GetErrorMess(appliName, "BUFFER_TOO_SMALL",ErrMess,1);
						status+=-1;
					}
					if(pfrom_arraylen>pfrom_list_len && rescode==0)
					{
						pfrom_arraylen=pfrom_list_len;
						memcpy(pfrom_list,pfrom1_list_temp,pfrom_list_len);
						status=0;
					}
					else
					{
						pfrom_arraylen=pfrom_list_len;
						GetErrorMess(appliName, "BUFFER_TOO_SMALL",ErrMess,1);
						status+=-1;
					}
					if(nto_arraylen>nto_list_len  && rescode==0)
					{
						nto_arraylen=nto_list_len;
						memcpy(nto_list,nto_list_temp,nto_list_len);
						status+=0;
					}
					else
					{
						nto_arraylen=nto_list_len;
						GetErrorMess(appliName, "BUFFER_TOO_SMALL",ErrMess,1);
						status+=-1;
					}
					if(pto_arraylen>pto_list_len  && rescode==0)
					{
						pto_arraylen=pto_list_len;
						memcpy(pto_list,pto1_list_temp,pto_list_len);
						status+=0;
					}
					else
					{
						GetErrorMess(appliName, "BUFFER_TOO_SMALL",ErrMess,1);
						pto_arraylen=pto_list_len;
						status+=-1;
					}

					if(len_array>i-1  && rescode==0)
					{
						for (j=0;j<i;j++)
						{
							pfrom1_list[j]=pfrom_list_temp[j];
							pto1_list[j]=pto_list_temp[j];
							pathid_list[j]=pathid_list_temp[j];
							link_pos_list[j]=link_pos_temp[j];
						}
						len_array=i;
						status+=0;
					}
					else
					{
						len_array=i;
						GetErrorMess(appliName, "BUFFER_TOO_SMALL",ErrMess,1);
						status+=-1;
					}

					//commit to delete the temporary table path
					if(OCITransCommit(ociHdbc, ociError, 0)!=OCI_SUCCESS)
					{
						if(rescode==0)	
							rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
					}


				}
				if(nfrom_temp!=NULL)
					free(nfrom_temp);
				if(nto_temp!=NULL)
					free(nto_temp);
				if(pfrom_list_temp!=NULL)
					free(pfrom_list_temp);
				if(pto_list_temp!=NULL)
					free(pto_list_temp);
				if(nfrom_list_temp!=NULL)
					free(nfrom_list_temp);
				if(nto_list_temp!=NULL)
					free(nto_list_temp);
				if(pfrom1_list_temp!=NULL)
					free(pfrom1_list_temp);
				if(pto1_list_temp!=NULL)
					free(pto1_list_temp);
				if(pathid_list_temp!=NULL)
					free(pathid_list_temp);
				if(link_pos_temp!=NULL)
					free(link_pos_temp);
				if(lkinfo_temp!=NULL)
					free(lkinfo_temp);
				if(lkinfo_list_temp!=NULL)
					free(lkinfo_list_temp);
				free(tabname);

			}
		}





		status+=OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if((strstr(ErrMess,"BUFFER_TOO_SMALL"))==NULL)
		{
			if(rescode!=0)
				OCIReportError(ociError,appliName, ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0); 
		}

		return (rescode+status);
	}

	/**********************************************************************************************************/
	/**
	* Get the path details  between dev_from and dev_to,  returning an integer value. NOT TO BE USED ONLY FOR INTERNAL USAGE
	* The user should manage the memory : there is no memory allocation.
	* you need grant execute if you have your own db account
	* @param dev_from : name of the device 1 (ex. THOR_00)
	* @param devto_type : name of the device 2 (ex. ECAL_L1FE)
	* @param devto_type_given : 1 if it's the device type you gave, 0 if it's the list of devicename which is similar to 'devto_type_given%'
	* @param nfrom_list : list of the devices which start the link (separated by '\\0')
	* @param len_array : length of the pfrom_list (which is equal to pto_list,pathid_list and link_pos_list) you have allocated. If the int * buffer is too small, we put the size needed here.
	* @param nfrom_arraylen : length of the nfrom_list  you have allocated. If the char * buffer is too small, we put the size needed here.
	* @param nto_arraylen : length of the nto_list  you have allocated. If the char * buffer is too small, we put the size needed here.
	* @param pfrom_arraylen : length of the pfrom_list  you have allocated. If the char * buffer is too small, we put the size needed here.
	* @param pto_arraylen : length of the pto_list  you have allocated. If the char * buffer is too small, we put the size needed here. 	  
	* @param lkinfo_arraylen : length of the lkinfo_list  you have allocated. If the char * buffer is too small, we put the size needed here.
	* @param pfrom_list1 : list of the port from numbers concatenated with port type (port_nbr|port_type)
	* @param pto_list1 : list of the port to numbers concatenated with port type (port_nbr|port_type)
	* @param nto_list : list of the devices which end the link
	* @param pfrom_list : list of the port way  associated with pfrom_list numbers concatenated with port type (port_nbr|port_type)
	* @param pto_list : list of the port way associated with pto_list numbers concatenated with port type (port_nbr|port_type)
	* @param pathid_list : list of the pathid
	* @param link_pos_list : position of the link in the path (start with 1)
	* @param lkinfo_list : info about the link
	* @param ErrMess : error message in case of failure (even NO_ROWS_SELECTED for no result)
	* NB: the i-th entry of the nfrom_list,pfrom_list,nto_list,pto_list,pathid_list and link_pos_list corresponds to one link of the path
	* @return 0 if it is successful
	*/
	EXTERN_CONFDB
		int GetDetailedConnBetweenDeviceDevType_plsql(char* dev_from, char* devto_type,int devto_type_given, char* nfrom_list,char* pfrom1_list,char* pto1_list,int& len_array,int & nfrom_arraylen,int & pfrom1_arraylen, int & nto_arraylen,int & pto1_arraylen, int &lkinfo_arraylen,int* pfrom_list,int* pto_list,char* nto_list,int* pathid_list, int* link_pos_list,char* lkinfo_list,char* ErrMess)
	{
		char appliName[100]="GetDetailedConnBetweenDeviceDevType_plsql";
		int i=0;
		int j=0;
		int res_code=0;
		int pos2=0;
		int len1=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		sword status;
		int rescode=0;
		int free_stmt=0;
		OCIStmt *stmthp;
		OCIBind  *bnd1p[6]; 
		OCIDefine* def[9];
		ub4  descriptionlen=0;
		ub4 devtypelen=0;
		char * nfrom_temp = NULL;
		char *nto_temp=NULL;
		char *lkinfo_temp=NULL;
		int prefetch_rows=10000;
		int len11=101;
		char * tabname=(char*)malloc(len11*sizeof(char));
		char * pfrom_temp = NULL;
		char *pto_temp=NULL;
		int nfrom_list_len=0;
		int nto_list_len=0;
		int lkinfo_list_len=0;
		int nfrom_list_len_prev=0;
		int nto_list_len_prev=0;
		int pfrom_list_len=0;
		int pto_list_len=0;
		int pfrom_list_len_prev=0;
		int pto_list_len_prev=0;
		int lkinfo_list_len_prev=0;
		int free_mem=0;
		int pathid=0;
		int pto=0;
		int pfromlen=0;
		int ptolen=0;
		int pfrom=0;
		int round_trip=1;
		int nfromlen=0;
		int lkinfolen=0;
		int ntolen=0;
		int link_pos=0;
		char* nfrom_list_temp=NULL;
		char* pfrom1_list_temp=NULL;
		char* pto1_list_temp=NULL;
		int* pfrom_list_temp=NULL;
		int* pto_list_temp=NULL;
		char* nto_list_temp=NULL;
		int* pathid_list_temp=NULL; 
		int* link_pos_temp=NULL;
		char* lkinfo_list_temp=NULL;
		char selectdevtype[1000];
		//clock_t start,finish;
		//double time;

		if(tabname==NULL)
		{
			GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
			return -1;
		}
		else
		{
			status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
			strcpy(tabname,"tetst");
		}
		if(status!=OCI_SUCCESS)
		{	
			rescode=ShowErrors (status, ociError, "OCIHandleAlloc unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrMess,1); 
			free(tabname);
			return -1;
		}
		else
		{
			//start = clock();
			//std:://cout <<"value of systemname "<<systemname<<"  and value of dev_from "<<dev_from <<" and dev_to "<< dev_to <<std::endl;
			sprintf(selectdevtype,"BEGIN :tabname:=%s(:devf,:devto,:dtypegiven); END;",_GiveDestinType);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevtype,(ub4) strlen(selectdevtype),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[0], ociError,(text*)":tabname",-1,(dvoid*)tabname,len11, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[1], ociError,(text*)":devf",-1,(dvoid*)dev_from,strlen(dev_from)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[2], ociError,(text*)":devto",-1,(dvoid*)devto_type,strlen(devto_type)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[3], ociError,(text*)":dtypegiven",-1,(dvoid*)&devto_type_given,sizeof(devto_type_given), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);



		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 1, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

		//finish = clock();
		//time = (double(finish)-double(start))/CLOCKS_PER_SEC;  
		//std:://cout <<"value of tabame "<<tabname <<std::endl;

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
			if(strncmp(tabname,"ERROR",5)==0) //means that the PL/SQL fct unsuccessfully executed
			{
				if(rescode==0)	
				{
					GetErrorMess(appliName,tabname,ErrMess,1); 
					if(OCITransCommit(ociHdbc, ociError, 0)!=OCI_SUCCESS)
					{
						if(rescode==0)	
							rescode=ShowErrors (status, ociError, "OCITransCommit unsuccessful");
					}
					status+=OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					free(tabname);
					return -1;
				}
			}
			else
			{
				sprintf(selectdevtype,"select d.pathid,d.link_pos,t.devicename||'?',d.pfrom,d.port_way1, g.devicename||'?',d.pto,d.port_way2,nvl(l.link_info,'none')||'?' from %s d, %s t,%s g,%s l,%s p where p.portid=decode(d.port_way1,1,l.portidto,l.portidfrom) and p.deviceid=d.nfrom and p.port_nbr||'|'||nvl(p.port_type,'none')=d.pfrom and d.port_way1=p.port_way and  d.nfrom=t.deviceid and d.nto=g.deviceid order by pathid,link_pos ",PATH_TEMP,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,MACRO_CONNECTIVITY_TABLE,PORT_TABLE);
				status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevtype,(ub4) strlen(selectdevtype),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
				//std:://cout <<"value of selectdevtype "<<selectdevtype <<std::endl;

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
				}
				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
				}
				else
					status=OCIAttrSet (stmthp,OCI_HTYPE_STMT,&prefetch_rows,0,OCI_ATTR_PREFETCH_ROWS,ociError);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIAttrSet  unsuccessful");
				}
				else
					status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);
				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIAttrSet  unsuccessful");
				}
				else
					status=OCIDefineByPos (stmthp, &def[0], ociError,1,&pathid,sizeof(pathid), SQLT_INT, 0, 0, 0, OCI_DEFAULT);
				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
				else
					status=OCIDefineByPos (stmthp, &def[1], ociError,2,&link_pos,sizeof(link_pos), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
				else
				{
					nfromlen=200;
					nfrom_temp=(char *) realloc(nfrom_temp,(nfromlen+1)*sizeof(char));
					ntolen=200;
					nto_temp= (char *) realloc(nto_temp,(ntolen+1)*sizeof(char));
					pfromlen=200;
					pfrom_temp=(char *) realloc(pfrom_temp,(pfromlen+1)*sizeof(char));
					ptolen=200;
					pto_temp= (char *) realloc(pto_temp,(ptolen+1)*sizeof(char));
					free_mem=1;
					lkinfolen=1000;
					lkinfo_temp=(char*) realloc(lkinfo_temp,(lkinfolen+1)*sizeof(char));
					if(nfrom_temp==NULL || nto_temp==NULL ||pfrom_temp==NULL||pto_temp==NULL||lkinfo_temp==NULL)
					{	
						if(nfrom_temp!=NULL)
							free(nfrom_temp);
						if(nto_temp!=NULL)
							free(nto_temp);
						if(pfrom_temp!=NULL)
							free(pfrom_temp);
						if(pto_temp!=NULL)
							free(pto_temp);
						if(lkinfo_temp!=NULL)
							free(lkinfo_temp);
						free(tabname);
						GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
						if(OCITransCommit(ociHdbc, ociError, 0)!=OCI_SUCCESS)
						{
							if(rescode==0)	
								rescode=ShowErrors (status, ociError, "OCITransCommit unsuccessful");
						}
						status+=OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						return -1;
					}
					else
						status =OCIDefineByPos(stmthp, &def[2], ociError,3, (ub1 *) (nfrom_temp), nfromlen+1,SQLT_STR, (dvoid *) 0,(ub2 *) 0,0, OCI_DEFAULT);	
				}

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
				else
					status=OCIDefineByPos (stmthp, &def[3], ociError,4,pfrom_temp,pfromlen+1, SQLT_STR, 0, 0, 0, OCI_DEFAULT);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
				else
					status=OCIDefineByPos (stmthp, &def[4], ociError,5,&pfrom,sizeof(int), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
				else
					status =OCIDefineByPos(stmthp, &def[5], ociError,6, (ub1 *) (nto_temp), ntolen + 1,SQLT_STR, (dvoid *) 0,(ub2 *) 0,0, OCI_DEFAULT);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
				else
					status=OCIDefineByPos (stmthp, &def[6], ociError,7,pto_temp,ptolen+1 ,SQLT_STR, 0, 0, 0, OCI_DEFAULT);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
				else
					status=OCIDefineByPos (stmthp, &def[7], ociError,8,&pto,sizeof(int), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
				else
					status=OCIDefineByPos (stmthp, &def[8], ociError,9,lkinfo_temp,lkinfolen+1, SQLT_STR, 0, 0, 0, OCI_DEFAULT);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
				else
				{
					i=0;
					pfrom_list_temp=(int*)realloc(pfrom_list_temp,prefetch_rows*sizeof(int));
					pto_list_temp=(int*)realloc(pto_list_temp,prefetch_rows*sizeof(int));
					pathid_list_temp=(int*)realloc(pathid_list_temp,prefetch_rows*sizeof(int));
					link_pos_temp=(int*)realloc(link_pos_temp,prefetch_rows*sizeof(int));
					free_mem=2;
					if(pfrom_list_temp==NULL || pto_list_temp==NULL || pathid_list_temp==NULL || link_pos_temp==NULL)
					{
						if(nfrom_temp!=NULL)
							free(nfrom_temp);
						if(nto_temp!=NULL)
							free(nto_temp);
						if(pfrom_list_temp!=NULL)
							free(nfrom_temp);
						if(pto_list_temp!=NULL)
							free(nto_temp);
						if(pathid_list_temp!=NULL)
							free(nfrom_temp);
						if(link_pos_temp!=NULL)
							free(nto_temp);
						if(lkinfo_temp!=NULL)
							free(lkinfo_temp);
						free(tabname);
						if(OCITransCommit(ociHdbc, ociError, 0)!=OCI_SUCCESS)
						{
							if(rescode==0)	
								rescode=ShowErrors (status, ociError, "OCITransCommit unsuccessful");
						}
						status+=OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
						return -1;
					}
					while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
					{
						status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);
						if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
						{
							pfrom_list_temp[i]=pfrom;
							pto_list_temp[i]=pto;
							pathid_list_temp[i]=pathid;
							link_pos_temp[i]=link_pos;
							ntolen=strcspn(nto_temp,"?");
							nto_list_len+=ntolen+1;
							nto_temp[ntolen]='\0';
							ptolen=strcspn(pto_temp,"?");
							pto_list_len+=ptolen+1;
							pto_temp[ptolen]='\0';
							nfromlen=strcspn(nfrom_temp,"?");
							nfrom_temp[nfromlen]='\0';
							nfrom_list_len+=nfromlen+1;
							pfromlen=strcspn(pfrom_temp,"?");
							pfrom_temp[pfromlen]='\0';
							pfrom_list_len+=pfromlen+1;
							lkinfolen=strcspn(lkinfo_temp,"?");
							lkinfo_temp[lkinfolen]='\0';
							lkinfo_list_len+=lkinfolen+1;
							nfrom_list_temp=(char *) realloc(nfrom_list_temp,nfrom_list_len*sizeof(char));
							nto_list_temp=(char *) realloc(nto_list_temp,nto_list_len*sizeof(char));
							pfrom1_list_temp=(char *) realloc(pfrom1_list_temp,nfrom_list_len*sizeof(char));
							pto1_list_temp=(char *) realloc(pto1_list_temp,nto_list_len*sizeof(char));
							lkinfo_list_temp=(char *) realloc(lkinfo_list_temp,lkinfo_list_len*sizeof(char));

							if(lkinfo_list_temp==NULL||nfrom_list_temp==NULL || nto_list_temp==NULL ||pfrom1_list_temp==NULL||pto1_list_temp==NULL)
							{
								if(nfrom_temp!=NULL)
									free(nfrom_temp);
								if(nto_temp!=NULL)
									free(nto_temp);
								if(pfrom_list_temp!=NULL)
									free(pfrom_list_temp);
								if(pto_list_temp!=NULL)
									free(pto_list_temp);
								if(nfrom_list_temp!=NULL)
									free(nfrom_list_temp);
								if(nto_list_temp!=NULL)
									free(nto_list_temp);
								if(pfrom1_list_temp!=NULL)
									free(pfrom1_list_temp);
								if(pto1_list_temp!=NULL)
									free(pto1_list_temp);
								if(pathid_list_temp!=NULL)
									free(pathid_list_temp);
								if(link_pos_temp!=NULL)
									free(link_pos_temp);
								if(lkinfo_temp!=NULL)
									free(lkinfo_temp);
								if(lkinfo_list_temp!=NULL)
									free(lkinfo_list_temp);
								free(tabname);
								GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
								if(OCITransCommit(ociHdbc, ociError, 0)!=OCI_SUCCESS)
								{
									if(rescode==0)	
										rescode=ShowErrors (status, ociError, "OCITransCommit unsuccessful");
								}
								status+=OCIHandleFree (stmthp,OCI_HTYPE_STMT);
								return -1;
							}
							else
							{
								memcpy(nfrom_list_temp+nfrom_list_len_prev,nfrom_temp,nfromlen+1);
								memcpy(nto_list_temp+nto_list_len_prev,nto_temp,ntolen+1);
								memcpy(pfrom1_list_temp+pfrom_list_len_prev,pfrom_temp,pfromlen+1);
								memcpy(pto1_list_temp+pto_list_len_prev,pto_temp,ptolen+1);
								memcpy(lkinfo_list_temp+lkinfo_list_len_prev,lkinfo_temp,lkinfolen+1);

								nfrom_list_len_prev=nfrom_list_len;
								nto_list_len_prev=nto_list_len;
								pfrom_list_len_prev=pfrom_list_len;
								pto_list_len_prev=pto_list_len;
								lkinfo_list_len_prev=lkinfo_list_len;


								i++;
							}
						}
						if(i>0 && (i%prefetch_rows)==0)
						{
							round_trip++;
							pfrom_list_temp=(int*)realloc(pfrom_list_temp,round_trip*prefetch_rows*sizeof(int));
							pto_list_temp=(int*)realloc(pto_list_temp,round_trip*prefetch_rows*sizeof(int));
							pathid_list_temp=(int*)realloc(pathid_list_temp,round_trip*prefetch_rows*sizeof(int));
							link_pos_temp=(int*)realloc(link_pos_temp,round_trip*prefetch_rows*sizeof(int));
							if(pfrom_list_temp==NULL || pto_list_temp==NULL || pathid_list_temp==NULL || link_pos_temp==NULL)
							{
								if(nfrom_temp!=NULL)
									free(nfrom_temp);
								if(nto_temp!=NULL)
									free(nto_temp);
								if(pfrom_list_temp!=NULL)
									free(pfrom_list_temp);
								if(pto_list_temp!=NULL)
									free(pto_list_temp);
								if(nfrom_list_temp!=NULL)
									free(nfrom_list_temp);
								if(nto_list_temp!=NULL)
									free(nto_list_temp);
								if(pfrom1_list_temp!=NULL)
									free(pfrom1_list_temp);
								if(pto1_list_temp!=NULL)
									free(pto1_list_temp);
								if(pathid_list_temp!=NULL)
									free(pathid_list_temp);
								if(link_pos_temp!=NULL)
									free(link_pos_temp);
								if(lkinfo_temp!=NULL)
									free(lkinfo_temp);
								if(lkinfo_list_temp!=NULL)
									free(lkinfo_list_temp);
								free(tabname);
								GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
								if(OCITransCommit(ociHdbc, ociError, 0)!=OCI_SUCCESS)
								{
									if(rescode==0)	
										rescode=ShowErrors (status, ociError, "OCITransCommit unsuccessful");
								}
								status+=OCIHandleFree (stmthp,OCI_HTYPE_STMT);
								return -1;
							}
						}
					}
					if(status!=OCI_SUCCESS && status != OCI_SUCCESS_WITH_INFO && i==0)
					{
						if(rescode==0)	
						{
							rescode=ShowErrors (status, ociError, "OCIFETCH unsuccessful");
							if(nfrom_temp!=NULL)
								free(nfrom_temp);
							if(nto_temp!=NULL)
								free(nto_temp);
							if(pfrom_list_temp!=NULL)
								free(pfrom_list_temp);
							if(pto_list_temp!=NULL)
								free(pto_list_temp);
							if(nfrom_list_temp!=NULL)
								free(nfrom_list_temp);
							if(nto_list_temp!=NULL)
								free(nto_list_temp);
							if(pfrom1_list_temp!=NULL)
								free(pfrom1_list_temp);
							if(pto1_list_temp!=NULL)
								free(pto1_list_temp);
							if(pathid_list_temp!=NULL)
								free(pathid_list_temp);
							if(link_pos_temp!=NULL)
								free(link_pos_temp);
							if(lkinfo_temp!=NULL)
								free(lkinfo_temp);
							if(lkinfo_list_temp!=NULL)
								free(lkinfo_list_temp);
							free(tabname);
							GetErrorMess(appliName, "NO_ROWS_SELECTED",ErrMess,1);
							if(OCITransCommit(ociHdbc, ociError, 0)!=OCI_SUCCESS)
							{
								if(rescode==0)	
									rescode=ShowErrors (status, ociError, "OCITransCommit unsuccessful");
							}
							status+=OCIHandleFree (stmthp,OCI_HTYPE_STMT);
							return -1;
						}
					}
					if(nfrom_arraylen>nfrom_list_len && rescode==0)
					{
						nfrom_arraylen=nfrom_list_len;
						memcpy(nfrom_list,nfrom_list_temp,nfrom_list_len);
						status=0;
					}
					else
					{
						nfrom_arraylen=nfrom_list_len;
						GetErrorMess(appliName, "BUFFER_TOO_SMALL",ErrMess,1);
						status+=-1;
					}
					if(lkinfo_arraylen>lkinfo_list_len && rescode==0)
					{
						lkinfo_arraylen=lkinfo_list_len;
						memcpy(lkinfo_list,lkinfo_list_temp,lkinfo_list_len);
						status=0;
					}
					else
					{
						lkinfo_arraylen=lkinfo_list_len;
						status+=-1;
					}
					if(pfrom1_arraylen>pfrom_list_len && rescode==0)
					{
						pfrom1_arraylen=pfrom_list_len;
						memcpy(pfrom1_list,pfrom1_list_temp,pfrom_list_len);
						status=0;
					}
					else
					{
						pfrom1_arraylen=pfrom_list_len;
						GetErrorMess(appliName, "BUFFER_TOO_SMALL",ErrMess,1);
						status+=-1;
					}
					if(nto_arraylen>nto_list_len  && rescode==0)
					{
						nto_arraylen=nto_list_len;
						memcpy(nto_list,nto_list_temp,nto_list_len);
						status+=0;
					}
					else
					{
						nto_arraylen=nto_list_len;
						GetErrorMess(appliName, "BUFFER_TOO_SMALL",ErrMess,1);
						status+=-1;
					}
					if(pto1_arraylen>pto_list_len  && rescode==0)
					{
						pto1_arraylen=pto_list_len;
						memcpy(pto1_list,pto1_list_temp,pto_list_len);
						status+=0;
					}
					else
					{
						pto1_arraylen=pto_list_len;
						GetErrorMess(appliName, "BUFFER_TOO_SMALL",ErrMess,1);
						status+=-1;
					}

					if(len_array>i-1  && rescode==0)
					{
						for (j=0;j<i;j++)
						{
							pfrom_list[j]=pfrom_list_temp[j];
							pto_list[j]=pto_list_temp[j];
							pathid_list[j]=pathid_list_temp[j];
							link_pos_list[j]=link_pos_temp[j];
						}
						len_array=i;
						status+=0;
					}
					else
					{
						len_array=i;
						GetErrorMess(appliName, "BUFFER_TOO_SMALL",ErrMess,1);
						status+=-1;
					}
					//commit to delete the temporary table path



				}
				if(nfrom_temp!=NULL)
					free(nfrom_temp);
				if(nto_temp!=NULL)
					free(nto_temp);
				if(pfrom_list_temp!=NULL)
					free(pfrom_list_temp);
				if(pto_list_temp!=NULL)
					free(pto_list_temp);
				if(nfrom_list_temp!=NULL)
					free(nfrom_list_temp);
				if(nto_list_temp!=NULL)
					free(nto_list_temp);
				if(pfrom1_list_temp!=NULL)
					free(pfrom1_list_temp);
				if(pto1_list_temp!=NULL)
					free(pto1_list_temp);
				if(pathid_list_temp!=NULL)
					free(pathid_list_temp);
				if(link_pos_temp!=NULL)
					free(link_pos_temp);
				free(tabname);
				if(lkinfo_temp!=NULL)
					free(lkinfo_temp);
				if(lkinfo_list_temp!=NULL)
					free(lkinfo_list_temp);

			}
		}




		if(OCITransCommit(ociHdbc, ociError, 0)!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCITransCommit unsuccessful");
		}
		status+=OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if((strstr(ErrMess,"BUFFER_TOO_SMALL"))==NULL)
		{
			if(rescode!=0)
				OCIReportError(ociError,appliName, ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0); 
		}

		return (rescode+status);
	}
#if defined(__cplusplus) || defined(c_plusplus)
}
#endif


