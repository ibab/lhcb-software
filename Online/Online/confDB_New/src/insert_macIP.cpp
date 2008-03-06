/********************************************************************************/
//	Author: L.Abadie
// version:v3.5
// changes: handling error such that can be extended to C++: (create a class SQLException) (strlen ck + update stmt)
// file which performs bulk collect insert for the macIP table
// N.B : Memory management (allocation+release) should be handled by the user application thus, for all the fct provided by this lib
/********************************************************************************/


#include "list_structures.h"
#include "system_info.h"

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
	extern char SQLROWCOUNT[20];
	/**************************GV for macIP************************************************/

	char* _ip_addList;
	int _max_ip_add_len;
	int _ip_addNbElement;
	int _ip_addList_len;

	int* _ipadd_nullvalue;
	int _ipaddNbEl;


	char* _subnet_maskList;
	int _max_subnet_mask_len;
	int _subnet_maskNbElement;
	int _subnet_maskList_len;

	int* _subnetmask_nullvalue;
	int _subnetmaskNbEl;
	//////////////////
	char *_ethernet_addList;
	int _max_ethernet_add_len;
	int _ethernet_addList_len;
	int _ethernet_addNbElement;
	int *_ethernet_nullvalue;
	int _ethernetNbEl;

	int *_lkaggregList;
	int _lkaggregList_len;
	int _lkaggregNbElement;

	char *_vlan_prefixList;
	int _max_vlan_prefix_len;
	int _vlan_prefixList_len;
	int _vlan_prefixNbElement;
	int *_vlanprefix_nullvalue;
	int _vlanprefixNbEl;


	//////////////////


	extern char* _ipnameList;
	extern int _max_ipname_len;
	extern int _ipnameNbElement;
	extern int _ipnameList_len;

	extern int* _ipname_nullvalue;
	extern int _ipnameNbEl;


	char* _devicenameList3;
	int _max_devicename_len3;
	int _devicenameNbElement3;
	int _devicenameList_len3;

	int* _devicename3_nullvalue;
	int _devicename3NbEl;

	int* _port_nbList;
	int _port_nbNbElement;
	int FIRST_TIME_MAC=0;
	int freeIPEthernet()
	{
		int status=0;
		_ethernet_addList=(char*)realloc(_ethernet_addList,0*sizeof(char));
		_ethernet_addList=NULL;

		_subnet_maskList=(char*)realloc(_subnet_maskList,0*sizeof(char));
		_subnet_maskList=NULL;

		_vlan_prefixList=(char*)realloc(_vlan_prefixList,0*sizeof(char));
		_vlan_prefixList=NULL;

		_ipnameList=(char*)realloc(_ipnameList,0*sizeof(char));
		_ipnameList=NULL;

		_lkaggregList=(int*)realloc(_lkaggregList,0*sizeof(int));
		_lkaggregList=NULL;

		_devicenameList3=(char*)realloc(_devicenameList3,0*sizeof(char));
		_devicenameList3=NULL;

		_port_nbList=(int*)realloc(_port_nbList,0*sizeof(int));
		_port_nbList=NULL;

		_devicename3_nullvalue=(int*)realloc(_devicename3_nullvalue,0*sizeof(int));
		_devicename3_nullvalue=NULL;

		_ipname_nullvalue=(int*)realloc(_ipname_nullvalue,0*sizeof(int));
		_ipname_nullvalue=NULL;

		_vlanprefix_nullvalue=(int*)realloc(_vlanprefix_nullvalue,0*sizeof(int));
		_vlanprefix_nullvalue=NULL;

		_subnetmask_nullvalue=(int*)realloc(_subnetmask_nullvalue,0*sizeof(int));
		_subnetmask_nullvalue=NULL;

		_ethernet_nullvalue=(int*)realloc(_ethernet_nullvalue,0*sizeof(int));
		_ethernet_nullvalue=NULL;

		_ipadd_nullvalue=(int*)realloc(_ipadd_nullvalue,0*sizeof(int));
		_ipadd_nullvalue=NULL;

		FIRST_TIME_MAC=0;
		return status;
	}








	/**
	* Insert a MAC entry,  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param systemname : name of the subsystem and "_" appended such as "TFC_".
	* @param ethernet_add   
	* @param ip_add
	* @param subnet_mask
	* @param vlan_prefix
	* @param ipname
	* @param lkaggreg :1 if the link is aggregated, 0 otherwise
	* @param devicename : name of the device
	* @param port_nbr   : port nb
	* @param first_time1 : 1 if it's your first MacIP insert (save it into the database), 0 otherwise
	* @param last_rows1 : 1 if it's your last MacIP insert (save it into the database), 0 otherwise
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/ 



	EXTERN_CONFDB
		int InsertMultipleMacIPs(char* systemname, char* ethernet_add,char* ip_add,char* subnet_mask,char* vlan_prefix,char* ipname,int lkaggreg,char* devicename,int port_nb,int first_time1,int last_rows1,char* ErrMess)
	{
		char appliName[100]="InsertMultipleMacIPs";
		int* numrows_inserted=NULL;
		int numrows=0;
		OCIBind  *bndp3 = (OCIBind *) 0;
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[8]; 
		int res_query=0;
		int free_mem=0;
		int rescode=0;
		sword status=0;
		char* ipadd=NULL;
		char* ethadd=NULL;
		char* iname=NULL;
		char* submask=NULL;
		char* vlanprefix=NULL;
		char* devname=NULL;
		char seq_name[100];
		int force_insert=0;
		int first_time=first_time1;
		int last_rows=last_rows1;
		char subsystem_name[10];
		strncpy(subsystem_name,systemname,9);
		int pos5=strcspn(subsystem_name,"_");
		subsystem_name[pos5]='\0';
		int i=0;
		sprintf(seq_name,"%strunk_link_LINK_TRUNKID.nextval",systemname);
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
		int ipadd1=null_charptr(ip_add);
		int ethadd1=null_charptr(ethernet_add);
		int submask1=null_charptr(subnet_mask);
		int iname1=null_charptr(ipname);
		int devname1=null_charptr(devicename);
		int vlanprefix1=null_charptr(vlan_prefix);

		if(FIRST_TIME_MAC==1 && _devicenameList3==NULL)
			first_time=1;

		res_query=AppendString(devicename,_devicenameList3,_devicenameList_len3,_devicenameNbElement3, _max_devicename_len3,first_time);
		res_query+=AppendString(vlan_prefix,_vlan_prefixList,_vlan_prefixList_len,_vlan_prefixNbElement,_max_vlan_prefix_len,first_time);
		res_query+=AppendString(ethernet_add,_ethernet_addList,_ethernet_addList_len,_ethernet_addNbElement,_max_ethernet_add_len,first_time);
		res_query+=AppendString(ip_add,_ip_addList,_ip_addList_len,_ip_addNbElement,_max_ip_add_len,first_time);
		res_query+=AppendString(subnet_mask,_subnet_maskList,_subnet_maskList_len,_subnet_maskNbElement,_max_subnet_mask_len,first_time);
		res_query+=AppendString(ipname,_ipnameList,_ipnameList_len,_ipnameNbElement,_max_ipname_len,first_time);

		res_query+=AppendInt(lkaggreg,_lkaggregList,_lkaggregNbElement,first_time);
		res_query+=AppendInt(port_nb,_port_nbList,_port_nbNbElement,first_time);

		res_query+=AppendInt(iname1,_ipname_nullvalue,_ipnameNbEl,first_time);
		res_query+=AppendInt(submask1,_subnetmask_nullvalue,_subnetmaskNbEl,first_time);
		res_query+=AppendInt(ipadd1,_ipadd_nullvalue,_ipaddNbEl,first_time);
		res_query+=AppendInt(devname1,_devicename3_nullvalue,_devicename3NbEl,first_time);
		res_query+=AppendInt(ethadd1,_ethernet_nullvalue,_ethernetNbEl,first_time);
		res_query+=AppendInt(vlanprefix1,_vlanprefix_nullvalue,_vlanprefixNbEl,first_time);


		status=res_query;
		if(first_time==1)
			FIRST_TIME_MAC=1;

		if(last_rows!=1 && _devicenameNbElement3==MAXROWS)
		{
			last_rows=1;
			force_insert=1;
		}
		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME_MAC==1)
		{
			int len_host=LOGIN_LEN;
			char login[LOGIN_LEN];
			char host[LOGIN_LEN];
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
				status=freeIPEthernet();
				if(vlanprefix!=NULL)
					free(vlanprefix);
				if(devname!=NULL)
					free(devname);
				if(ethadd!=NULL)
					free(ethadd);
				if(ipadd!=NULL)
					free(ipadd);
				if(iname!=NULL)
					free(iname);
				if(submask!=NULL)
					free(submask);
				free(errmessg1);
				free(errmessg2);
				return -1;
			}
			//need to proceed with messages


			status =OCIHandleAlloc (ociEnv, (void**)&hstmt, OCI_HTYPE_STMT , 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIHandleAlloc unsuccessful");
			}
			else
				sprintf(sqlstmt,"BEGIN insert ALL WHEN (:lkaggreg> 0) THEN into %sip_ethernet_view (ETHERNET_ADD,ip_add,subnet_mask,vlan_prefix,portid,ipname,lkaggreg,CREATED,LAST_UPDATE,author,user_update,terminal_name,subsystem_name) values (:ethadd,:ipadd,:submask,:vlan,ptid,:ipname,:lkaggreg,sysdate,sysdate,'%s','%s','%s','%s') into %strunk_link_view (LINK_TRUNKID,LKAGGREG_ADD,created,last_update,author,user_update,terminal_name,subsystem_name) values (%s,:ethadd,sysdate,sysdate,'%s','%s','%s','%s') ELSE into %sip_ethernet_view (ETHERNET_ADD,ip_add,subnet_mask,vlan_prefix,portid,ipname,lkaggreg,CREATED,last_update,author,user_update,terminal_name,subsystem_name) values (:ethadd,:ipadd,:submask,:vlan,ptid,:ipname,:lkaggreg,sysdate,sysdate,'%s','%s','%s','%s')  select e.portid as ptid from %sport_properties_view e,%sdevice_view t where t.devicename=:dname and e.deviceid=t.deviceid and e.port_nbr=:ptnb; :numrows:=%s; end;  ",systemname,login,login,host,subsystem_name,systemname,seq_name,login,login,host,subsystem_name,systemname,login,login,host,subsystem_name,systemname,systemname,SQLROWCOUNT);

			if (OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT))
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
			{
				devname=(char*)malloc(_devicenameNbElement3*_max_devicename_len3*sizeof(char));
				vlanprefix=(char*)malloc(_vlan_prefixNbElement*_max_vlan_prefix_len*sizeof(char));
				submask=(char*)malloc(_subnet_maskNbElement*_max_subnet_mask_len*sizeof(char));
				iname=(char*)malloc(_ipnameNbElement*_max_ipname_len*sizeof(char));
				ipadd=(char*)malloc(_ip_addNbElement*_max_ip_add_len*sizeof(char));
				ethadd=(char*)malloc(_ethernet_addNbElement*_max_ethernet_add_len*sizeof(char));
				if(devname!=NULL)
					status=NormalizeVector(_devicenameList3,_devicenameNbElement3, _max_devicename_len3,devname);
				if(vlanprefix!=NULL)
					status+=NormalizeVector(_vlan_prefixList, _vlan_prefixNbElement,_max_vlan_prefix_len,vlanprefix);
				if(submask!=NULL)
					status+=NormalizeVector(_subnet_maskList,_subnet_maskNbElement,_max_subnet_mask_len,submask);
				if(iname!=NULL)
					status+=NormalizeVector(_ipnameList,_ipnameNbElement,_max_ipname_len,iname);
				if(ipadd!=NULL)
					status+=NormalizeVector(_ip_addList,_ip_addNbElement,_max_ip_add_len,ipadd);
				if(ethadd!=NULL)
					status+=NormalizeVector(_ethernet_addList,_ethernet_addNbElement,_max_ethernet_add_len,ethadd);
				numrows_inserted=(int*)malloc(sizeof(int)*_ethernet_addNbElement);
				if(numrows_inserted!=NULL)
				{
					for(i=0;i<_ethernet_addNbElement;i++)
						numrows_inserted[i]=0;
				}
				if(devname==NULL || vlanprefix==NULL || submask==NULL || iname==NULL||ipadd==NULL||ethadd==NULL||numrows_inserted==NULL)
				{
					status=-10;
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");

					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);

					if(vlanprefix!=NULL)
						free(vlanprefix);
					if(devname!=NULL)
						free(devname);
					if(ethadd!=NULL)
						free(ethadd);
					if(ipadd!=NULL)
						free(ipadd);
					if(iname!=NULL)
						free(iname);
					if(submask!=NULL)
						free(submask);
					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					status=freeIPEthernet();
					return -1;
				}
				else
					status =OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":ethadd", -1,(dvoid*)ethadd,_max_ethernet_add_len,  SQLT_STR, (dvoid *)&_ethernet_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
				free_mem=1;
			}

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[1], ociError,(text*) ":ipadd", -1,(dvoid*)ipadd, _max_ip_add_len,  SQLT_STR, (dvoid *)&_ipadd_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[2], ociError,(text*) ":submask", -1,(dvoid*)submask,_max_subnet_mask_len,  SQLT_STR, (dvoid *) &_subnetmask_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[3], ociError,(text*) ":vlan", -1,(dvoid*)vlanprefix, _max_vlan_prefix_len,  SQLT_STR, (dvoid *)& _vlanprefix_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[4], ociError,(text*) ":ipname", -1,(dvoid*)iname, _max_ipname_len,  SQLT_STR, (dvoid *) &_ipname_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[5], ociError,(text*) ":lkaggreg", -1,(dvoid*)&_lkaggregList[0], sizeof(lkaggreg),  SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[6], ociError,(text*) ":dname", -1,(dvoid*)devname,  _max_devicename_len3,  SQLT_STR, (dvoid *)&_devicename3_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[7], ociError,(text*) ":ptnb", -1,(dvoid*)&_port_nbList[0], sizeof(port_nb),  SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp3, ociError,(text*) ":numrows", -1,(dvoid*)&numrows_inserted[0], sizeof(numrows),SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[0], ociError,_max_ethernet_add_len, sizeof(int),0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[1], ociError,_max_ip_add_len, sizeof(int),0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[2], ociError,_max_subnet_mask_len, sizeof(int),0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[3], ociError,_max_vlan_prefix_len, sizeof(int),0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[4], ociError,_max_ipname_len, sizeof(int),0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");

			}
			else
				status=OCIBindArrayOfStruct(bndp[5], ociError,sizeof(int), 0,0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");	
			}
			else
				status=OCIBindArrayOfStruct(bndp[6], ociError,_max_devicename_len3, sizeof(int),0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");	
			}
			else
				status=OCIBindArrayOfStruct(bndp[7], ociError,sizeof(int), 0,0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp3, ociError, sizeof(int),0, 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status= OCIStmtExecute(ociHdbc, hstmt, ociError, _ethernet_addNbElement, 0, 0, 0, OCI_DEFAULT );
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
			}
			else
			{
				numrows=-1;
				for(i=0;i< _ethernet_addNbElement;i++)
				{
					if(numrows_inserted[i]==0)
					{
						numrows=0;
						i=  _ethernet_addNbElement+5;
					}
					else
						numrows=numrows_inserted[i];
				}
				if(numrows==0)
				{
					status = OCITransRollback(ociHdbc, ociError, 0);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status=freeIPEthernet();
					if(vlanprefix!=NULL)
						free(vlanprefix);
					if(devname!=NULL)
						free(devname);
					if(ethadd!=NULL)
						free(ethadd);
					if(ipadd!=NULL)
						free(ipadd);
					if(iname!=NULL)
						free(iname);
					if(submask!=NULL)
						free(submask);
					free(numrows_inserted);
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
			}
			status=freeIPEthernet();
			if(rescode!=0)
				if(ociError!=0)
					OCIReportError(ociError,appliName, ErrMess,1); 
				else
					GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0); 


			if(vlanprefix!=NULL)
				free(vlanprefix);
			if(devname!=NULL)
				free(devname);
			if(ethadd!=NULL)
				free(ethadd);
			if(ipadd!=NULL)
				free(ipadd);
			if(iname!=NULL)
				free(iname);
			if(submask!=NULL)
				free(submask);
			if(numrows_inserted!=NULL)
				free(numrows_inserted);

			status+=OCIHandleFree (hstmt,OCI_HTYPE_STMT);
			if(force_insert==1 && rescode==0)
			{

				FIRST_TIME_MAC=1;
			}
		}
		else
		{
			if(res_query!=0)
			{
				status=freeIPEthernet();
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
			}
			if(FIRST_TIME_MAC!=1)
			{
				status=freeIPEthernet();
				res_query=-1;
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
			}
			if(ociEnv==0)
			{
				status=freeIPEthernet();
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
			}
			rescode=res_query;
		}

		return (rescode+status);
	}


#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

