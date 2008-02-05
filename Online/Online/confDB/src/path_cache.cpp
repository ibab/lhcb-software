/********************************************************************************/
//	Author: L.Abadie
// version:v3.5
// changes: handling error such that can be extended to C++: (create a class SQLException) (strlen ck + update stmt)
// file which performs select statements
// N.B : Memory management (allocation+release) should be handled by the user application thus, for all the fct provided by this lib
/********************************************************************************/
#include "list_structures.h"
#include "system_info.h"
#include "cicDB.h"
#include <ctime>

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

	int len_array_gv; 
	int* lkid_list; 
	int * node_from_list;
	int* node_to_list; 
	int* bidirectional_list; 
	int* lkused_list; 
	int* lktype_list;
	int* lk_weight_list;
	char** pfrom_array_list;
	char** nfrom_array_list;
	char** nto_array_list;
	char** pto_array_list;
	char** lkinfo_array_list;
	int max_rows;

	int len_array_gv_scd; 
	int* lkid_list_scd; 
	int * node_from_list_scd;
	int* node_to_list_scd; 
	int* bidirectional_list_scd; 
	int* lkused_list_scd; 
	int* lktype_list_scd;
	int* lk_weight_list_scd;
	char** pfrom_array_list_scd;
	char** nfrom_array_list_scd;
	char** nto_array_list_scd;
	char** pto_array_list_scd;
	char** lkinfo_array_list_scd;
	int max_rows_scd;

	int len_array_gv_bis; 
	int* lkid_list_bis; 
	int * node_from_list_bis;
	int* node_to_list_bis; 
	int* bidirectional_list_bis; 
	int* lkused_list_bis; 
	int* lktype_list_bis;
	int* lk_weight_list_bis;
	char** pfrom_array_list_bis;
	char** nfrom_array_list_bis;
	char** nto_array_list_bis;
	char** pto_array_list_bis;
	char** lkinfo_array_list_bis;
	int max_rows_bis;


	int len_array_gv_bis_rev; 
	int* lkid_list_bis_rev; 
	int * node_from_list_bis_rev;
	int* node_to_list_bis_rev; 
	int* bidirectional_list_bis_rev; 
	int* lkused_list_bis_rev; 
	int* lktype_list_bis_rev;
	int* lk_weight_list_bis_rev;
	char** pfrom_array_list_bis_rev;
	char** nfrom_array_list_bis_rev;
	char** nto_array_list_bis_rev;
	char** pto_array_list_bis_rev;
	char** lkinfo_array_list_bis_rev;
	int max_rows_bis_rev;

	int BIDIRECTIONAL_WAY_USED;

	/****GV for internal connectivity ************************/

	int len_array_gv_internal; 
	int * node_from_list_internal;
	int max_rows_internal;

	int len_array_gv_scd_internal; 
	int * node_from_list_scd_internal;
	int max_rows_scd_internal;

	int len_array_gv_bis_internal; 
	int * node_from_list_bis_internal;
	int max_rows_bis_internal;


	int len_array_gv_bis_rev_internal; 
	int * node_from_list_bis_rev_internal;
	int max_rows_bis_rev_internal;
	/**********************************************************************************/
	/*****************************************************************************************/
	/**
	* Load the link type table of a given system , returning a int :internal fct . INTERNAL FCT CANNOT BE USED
	* @param systemnameList : list of the subsystems (UPPER CASE) for which you want to load the connectivity, separated with a coma : VELO,DAQ,TFC. for instance. Put ALL, if you want to load the whole connectivity table.
	* @param len_array : length of the lkid_list array you have allocated (same as node_from_list,...,lktype_list) we return the length we need if too small)
	* @param dfrom : name of the device (start of the path) : mandatory
	* @param dto : name of the device which ends the path (put "none" if absent)
	* @param bidi_link : 0 to get the real bidirectional link values / 1 if you want to consider all the links as bidirectional used for GetAllPathsPerDevice
	* @param ErrMess : error message in case of failure (NO_ROWS_SELECTED if table empty)
	* @return  0 if successful
	*/
	/*****************************************************************************/

	int LoadConnectivityTable_InCache(char* systemnameList,char * dfrom,char* dto,int bidi_link,int getallpaths,char* ErrMess)
	{
		int i = 0;
		int j=0;
		int len=0;
		int k=0;
		char appliName[100]="LoadConnectivityTable_InCache";
		sword status;
		OCIStmt *stmthp;
		OCIDefine* def[12];
		OCIParam *parmdp;
		OCIBind  *bndp[2]; 
		int pfrom_null=0;
		int pto_null=0;
		int linkid=0;
		int node_from=0;
		int rescode=0;
		int node_to=0;
		char* port_nbrfrom=NULL;
		char* port_nbrto=NULL;
		char* nodename_from1=NULL;
		char* nodename_to1=NULL;
		char* lkinfo_temp=NULL;
		int nfrom_null=0;
		int nto_null=0;
		int pfromlen=0;
		int lkinfo_null=0;
		int ptolen=0;
		int pfromlen_temp=0;
		int ptolen_temp=0;
		int lkinfolen=0;
		int lkinfolen_temp=0;
		int nfromlen1=0;
		int ntolen1=0;
		int nfromlen_temp1=0;
		int ntolen_temp1=0;

		int link_type=0; 
		int bidirectional_link_used=0;
		int lkused=0;
		int link_weight=0;
		int* lkid_list_temp=NULL;
		int* nfrom_list_temp=NULL;
		int* nto_list_temp=NULL;
		int* lktype_temp=NULL;
		char* pfrom_list_temp=NULL;
		char* pto_list_temp=NULL;
		char* nfrom_list_temp1=NULL;
		char* nto_list_temp1=NULL;
		char* lkinfo_list_temp=NULL;
		int* bidirectional_list_temp=NULL;
		int * lkweight_list_temp=NULL;
		int* lkused_list_temp=NULL;
		int round_trip=0;
		int prefetch_rows=20000;
		char selectconn[2000];
		int pos1=0;
		int pos2=0;
		int pos3=0;
		int pos1bis=0;
		int pos2bis=0;
		int pos3bis=0;

		int pos6=0;
		int pos7=0;
		int pos8=0;
		int pos6bis=0;
		int pos7bis=0;
		int pos8bis=0;
		int lkweight=0;
		int node_pos1=0;
		int node_pos2=0;
		int node_pos3=0;
		int node_pos1bis=0;
		int node_pos2bis=0;
		int node_pos3bis=0;
		int pos9=0;
		int pos9bis=0;
		int lkinfo_pos1=0;
		int lkinfo_pos2=0;
		int lkinfo_pos3=0;
		char logmessage[100];
		int sysID=0;
		char sysIDlist[100];
		int max_pfrom_len=0;
		int max_pto_len=0;
		int max_nto_len=0;
		int max_nfrom_len=0;
		int max_lkinfo_len=0;
		len_array_gv_internal=0; 
		len_array_gv_scd_internal=0; 
		len_array_gv_bis_internal=0; 
		len_array_gv_bis_rev_internal=0; 
		if(strncmp(systemnameList,"ALL",3)==0)
		{
			sysID=1;
		}
		else
		{
			if(strcspn(systemnameList,",")<strlen(systemnameList))
			{
				//std :://cout<<"value of sysIDlist "<<systemnameList<<std::endl;
				sysID=RetrieveSysIDList(systemnameList,sysIDlist);
				if(sysID==-1)
				{
					GetErrorMess(appliName, "systemnameList is null",ErrMess,1);
					return -1;
				}
			}
			else
			{
				sysID=GetSubsystemID(systemnameList);
			}
		}
		//std :://cout<<"value of sysIDlist "<<sysID<<std::endl;
		//std :://cout<<"value of sysIDlist "<<sysIDlist<<std::endl;
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
			if(bidi_link==0 || bidi_link==1)
			{
				if(bidi_link==0)
					sprintf(selectconn,"select t.linkid,g.deviceid,f.deviceid,g.port_nbr||'|'||nvl(g.port_type,'none')||'?',f.port_nbr||'|'||nvl(f.port_type,'none')||'?',t.link_type,t.bidirectional_link_used,t.lkused,t.link_weight,m.devicename||'?',n.devicename||'?',nvl(t.link_info,'none')||'?' from %s t,%s g,%s f, %s l,%s m,%s n where t.lkused=1 and g.deviceid=m.deviceid and f.deviceid=n.deviceid and t.portidfrom=g.portid and t.portidto=f.portid and mod(l.system_name,t.system_name)*mod(t.system_name,l.system_name)=0 and (l.devicename=:dfrom  )",MACRO_CONNECTIVITY_TABLE,PORT_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE);
				else
					sprintf(selectconn,"select -t.linkid,f.deviceid,g.deviceid,f.port_nbr||'|'||nvl(f.port_type,'none')||'?',g.port_nbr||'|'||nvl(g.port_type,'none')||'?',t.link_type,t.bidirectional_link_used,t.lkused,decode(t.link_weight,1,2,2,1,0),n.devicename||'?',m.devicename||'?',nvl(t.link_info,'none')||'?' from %s t,%s g,%s f,%s m,%s n,%s l where t.lkused=1 and g.deviceid=m.deviceid and f.deviceid=n.deviceid and t.portidfrom=g.portid and t.portidto=f.portid and mod(l.system_name,t.system_name)*mod(t.system_name,l.system_name)=0 and (l.devicename=:dfrom  )",MACRO_CONNECTIVITY_TABLE,PORT_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE);

				//std::cout<<"value of dfrom "<<dfrom<<std::endl;
				//if(bidi_link==0)
				//	sprintf(selectconn,"select t.linkid,g.deviceid,f.deviceid,g.port_nbr||'|'||nvl(g.port_type,'none')||'?',f.port_nbr||'|'||nvl(f.port_type,'none')||'?',t.link_type,t.bidirectional_link_used,t.lkused,t.link_weight,m.devicename||'?',n.devicename||'?',nvl(t.link_info,'none')||'?' from %s t,%s g,%s f, %s l,%s m,%s n, lhcb_device_system r,lhcb_device_system y,lhcb_device_system z where t.lkused=1 and g.deviceid=m.deviceid and f.deviceid=n.deviceid and t.portidfrom=g.portid and t.portidto=f.portid and l.devicename=:dfrom and l.deviceid=r.deviceid and y.deviceid=m.deviceid and z.deviceid=n.deviceid and r.systemid=y.systemid and y.systemid=z.systemid  ",MACRO_CONNECTIVITY_TABLE,PORT_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE);
				//else
				//	sprintf(selectconn,"select -t.linkid,f.deviceid,g.deviceid,f.port_nbr||'|'||nvl(f.port_type,'none')||'?',g.port_nbr||'|'||nvl(g.port_type,'none')||'?',t.link_type,t.bidirectional_link_used,t.lkused,decode(t.link_weight,1,2,2,1,0),n.devicename||'?',m.devicename||'?',nvl(t.link_info,'none')||'?' from %s t,%s g,%s f,%s m,%s n,%s l,lhcb_device_system r,lhcb_device_system z,lhcb_device_system y where t.lkused=1 and g.deviceid=m.deviceid and f.deviceid=n.deviceid and t.portidfrom=g.portid and t.portidto=f.portid and l.devicename=:dfrom and  l.deviceid=r.deviceid and y.deviceid=m.deviceid and z.deviceid=n.deviceid and r.systemid=y.systemid and y.systemid=z.systemid",MACRO_CONNECTIVITY_TABLE,PORT_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE);





			}
			else
			{
				if(bidi_link==2)  //case where the devfrom is really the from and the devto is really the to  as it has been saved in the connectivity
				{
					if(sysID==1)
						sprintf(selectconn,"select distinct t.linkid,g.deviceid,f.deviceid,g.port_nbr||'|'||nvl(g.port_type,'none')||'?',f.port_nbr||'|'||nvl(f.port_type,'none')||'?',t.link_type,t.bidirectional_link_used,t.lkused,t.link_weight,m.devicename||'?',n.devicename||'?',nvl(t.link_info,'none')||'?' from %s t,%s g,%s f,%s m,%s n where t.lkused=1 and g.deviceid=m.deviceid and f.deviceid=n.deviceid and t.portidfrom=g.portid and t.portidto=f.portid",MACRO_CONNECTIVITY_TABLE,PORT_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE);
					else
					{
						if(sysID==0)
							sprintf(selectconn,"select distinct t.linkid,g.deviceid,f.deviceid,g.port_nbr||'|'||nvl(g.port_type,'none')||'?',f.port_nbr||'|'||nvl(f.port_type,'none')||'?',t.link_type,t.bidirectional_link_used,t.lkused,t.link_weight,m.devicename||'?',n.devicename||'?',nvl(t.link_info,'none')||'?' from %s t,%s g,%s f, %s l, %s m,% s n where t.lkused=1 and g.deviceid=m.deviceid and f.deviceid=n.deviceid and t.portidfrom=g.portid and t.portidto=f.portid and  l.systemID in (%s) and (mod(t.system_name,l.systemID)=0 or mod(l.systemid,t.system_name)=0)",MACRO_CONNECTIVITY_TABLE,PORT_TABLE,PORT_TABLE,SUBSYSTEM_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,sysIDlist);
						else
							sprintf(selectconn,"select distinct t.linkid,g.deviceid,f.deviceid,g.port_nbr||'|'||nvl(g.port_type,'none')||'?',f.port_nbr||'|'||nvl(f.port_type,'none')||'?',t.link_type,t.bidirectional_link_used,t.lkused,t.link_weight,m.devicename||'?',n.devicename||'?',nvl(t.link_info,'none')||'?' from %s t,%s g,%s f, %s m,%s n where t.lkused=1 and g.deviceid=m.deviceid and f.deviceid=n.deviceid and t.portidfrom=g.portid and t.portidto=f.portid and (mod(t.system_name,%d)=0 or mod(%d,t.system_name)=0)",MACRO_CONNECTIVITY_TABLE,PORT_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,sysID,sysID);
					}
				}
				else
				{
					if(sysID==1)
						sprintf(selectconn,"select distinct -t.linkid,f.deviceid,g.deviceid,f.port_nbr||'|'||nvl(f.port_type,'none')||'?',g.port_nbr||'|'||nvl(g.port_type,'none')||'?',t.link_type,t.bidirectional_link_used,t.lkused,decode(t.link_weight,1,2,2,1,0),n.devicename||'?',m.devicename||'?',nvl(t.link_info,'none')||'?' from %s t,%s g,%s f,%s m,%s n where t.lkused=1 and g.deviceid=m.deviceid and f.deviceid=n.deviceid and t.portidfrom=g.portid and t.portidto=f.portid ",MACRO_CONNECTIVITY_TABLE,PORT_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE);
					else
					{
						if(sysID==0)
							sprintf(selectconn,"select distinct -t.linkid,f.deviceid,g.deviceid,f.port_nbr||'|'||nvl(f.port_type,'none')||'?',g.port_nbr||'|'||nvl(g.port_type,'none')||'?',t.link_type,t.bidirectional_link_used,t.lkused,decode(t.link_weight,1,2,2,1,0),n.devicename||'?',m.devicename||'?',nvl(t.link_info,'none')||'?' from %s t,%s g,%s f,%s l, %s m,%s n where t.lkused=1 and g.deviceid=m.deviceid and f.deviceid=n.deviceid and t.portidfrom=g.portid and t.portidto=f.portid and (mod(t.system_name,l.systemID)=0 or mod(l.systemID,t.system_name)=0) and l.systemID in (%s) ",MACRO_CONNECTIVITY_TABLE,PORT_TABLE,PORT_TABLE,SUBSYSTEM_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,sysIDlist);
						else
							sprintf(selectconn,"select distinct -t.linkid,f.deviceid,g.deviceid,f.port_nbr||'|'||nvl(f.port_type,'none')||'?',g.port_nbr||'|'||nvl(g.port_type,'none')||'?',t.link_type,t.bidirectional_link_used,t.lkused,decode(t.link_weight,1,2,2,1,0),n.devicename||'?',m.devicename||'?',nvl(t.link_info,'none')||'?' from %s t,%s g,%s f, %s m,%s n where t.lkused=1 and g.deviceid=m.deviceid and f.deviceid=n.deviceid and t.portidfrom=g.portid and t.portidto=f.portid and (mod(t.system_name,%d)=0 or mod(%d,t.system_name)=0) ",MACRO_CONNECTIVITY_TABLE,PORT_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,sysID,sysID);
					}

				}


			}
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectconn,(ub4) strlen(selectconn),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}
		//std:://cout<<"selectconn="<<selectconn<<std::endl;
		if(bidi_link==0 ||bidi_link==1)
		{
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
			{	
				status =OCIBindByName(stmthp, &bndp[0], ociError,(text*) ":dfrom", -1,(dvoid*)dfrom, strlen(dfrom)+1,  SQLT_STR, (dvoid *)0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			}

			//if(status!=OCI_SUCCESS)
			//{
			//	if(rescode==0)	
			//		rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			//}
			//else
			//{	
			//	status =OCIBindByName(stmthp, &bndp[1], ociError,(text*) ":dto", -1,(dvoid*)dto, strlen(dto)+1,  SQLT_STR, (dvoid *)0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			//}
			//std :://cout<<"value of selectconn "<<selectconn <<std::endl;
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBind unsuccessful");
		}
		else
			status=OCIAttrSet (stmthp,OCI_HTYPE_STMT,&prefetch_rows,0,OCI_ATTR_PREFETCH_ROWS,ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtAttribute unsuccessful 1");
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
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 4);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet1 unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &pfromlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
			/* The next two statements describe the select-list item, dept, and return its length to allocate the memory*/
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 5);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet1 unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &ptolen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet 2 unsuccessful");
		}
		else
		{
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 10);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet1 unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &nfromlen1, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet 3 unsuccessful");
		}
		else
		{
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 11);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet2 unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &ntolen1, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet 4 unsuccessful");
		}
		else
		{
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 12);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet1 unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &lkinfolen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIAttrGet 5 unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[0], ociError,1,&linkid,sizeof(linkid), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[1], ociError,2,&node_from,sizeof(node_from), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[2], ociError, 3,&node_to,sizeof(node_to), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
		{
			port_nbrfrom=(char*)malloc(prefetch_rows*(pfromlen+1)*sizeof(char));
			if(port_nbrfrom!=NULL)
				status=OCIDefineByPos (stmthp, &def[3], ociError, 4,port_nbrfrom,pfromlen+1, SQLT_STR, &pfrom_null, 0, 0, OCI_DEFAULT);
			else
			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName, "Malloc unsuccessful 1",ErrMess,1);
				return -1;
			}
		}	
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors(status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
		{
			port_nbrto=(char*)malloc(prefetch_rows*(ptolen+1)*sizeof(char));
			if(port_nbrto!=NULL)
				status=OCIDefineByPos(stmthp, &def[4], ociError, 5,port_nbrto,ptolen+1, SQLT_STR, &pto_null, 0, 0, OCI_DEFAULT);
			else
			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName, "Malloc unsuccessful 2",ErrMess,1);
				free(port_nbrfrom);
				return -1;
			}
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors(status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos(stmthp, &def[5], ociError,6,&link_type,sizeof(link_type), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors(status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos(stmthp, &def[6], ociError,7,&bidirectional_link_used,sizeof(bidirectional_link_used), SQLT_INT, 0, 0, 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors(status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos(stmthp, &def[7], ociError,8,&lkused,sizeof(lkused), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors(status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos(stmthp, &def[8], ociError,9,&lkweight,sizeof(lkweight), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors(status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
		{
			nodename_from1=(char*)malloc((nfromlen1+1)*sizeof(char));
			if(nodename_from1!=NULL)
				status=OCIDefineByPos(stmthp, &def[9], ociError,10,nodename_from1,nfromlen1+1, SQLT_STR, &nfrom_null, 0, 0, OCI_DEFAULT);
			else
			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName, "Malloc unsuccessful 3",ErrMess,1);
				free(port_nbrfrom);
				free(port_nbrto);
				return -1;
			}
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors(status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
		{
			nodename_to1=(char*)malloc((ntolen1+1)*sizeof(char));
			if(nodename_to1!=NULL)
				status=OCIDefineByPos(stmthp, &def[10], ociError,11,nodename_to1,ntolen1+1, SQLT_STR,&nto_null, 0, 0, OCI_DEFAULT);
			else
			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName, "Malloc unsuccessful 4",ErrMess,1);
				free(port_nbrfrom);
				free(port_nbrto);
				free(nodename_from1);
				return -1;
			}
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors(status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
		{
			lkinfo_temp=(char*)malloc((lkinfolen+1)*sizeof(char));
			if(lkinfo_temp!=NULL)
				status=OCIDefineByPos(stmthp, &def[11], ociError,12,lkinfo_temp,lkinfolen+1, SQLT_STR,&lkinfo_null, 0, 0, OCI_DEFAULT);
			else
			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName, "Malloc unsuccessful 5",ErrMess,1);
				free(port_nbrfrom);
				free(port_nbrto);
				free(nodename_from1);
				free(nodename_to1);
				return -1;
			}
		}
		//std :://cout<<"after define " <<std::endl;
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
		{
			i=0;
			round_trip=1;
			lkid_list_temp=(int*)realloc(lkid_list_temp,prefetch_rows*sizeof(int));
			nfrom_list_temp=(int*)realloc(nfrom_list_temp,prefetch_rows*sizeof(int));
			nto_list_temp=(int*)realloc(nto_list_temp,prefetch_rows*sizeof(int));

			if(lkid_list_temp==NULL  || nfrom_list_temp==NULL || nto_list_temp==NULL)
			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName, "Malloc unsuccessful 6",ErrMess,1);
				if(lkid_list_temp!=NULL)
					free(lkid_list_temp);
				if(nfrom_list_temp!=NULL)
					free(nfrom_list_temp);
				if(nto_list_temp!=NULL)
					free(nto_list_temp);	
				if(port_nbrfrom!=NULL)
					free(port_nbrfrom);
				if(port_nbrto!=NULL)
					free(port_nbrto);
				if(nodename_from1!=NULL)
					free(nodename_from1);
				if(nodename_to1!=NULL)
					free(nodename_to1);
				if(lkinfo_temp!=NULL)
					free(lkinfo_temp);
				return -1;

			}
			lktype_temp=(int*)realloc(lktype_temp,prefetch_rows*sizeof(int));
			pfrom_list_temp=(char*)realloc(pfrom_list_temp,(2)*sizeof(char));
			pto_list_temp=(char*)realloc(pto_list_temp,(2)*sizeof(char));
			nfrom_list_temp1=(char*)realloc(nfrom_list_temp1,(2)*sizeof(char));
			nto_list_temp1=(char*)realloc(nto_list_temp1,(2)*sizeof(char));
			lkinfo_list_temp=(char*)realloc(lkinfo_list_temp,(2)*sizeof(char));
			if(lktype_temp==NULL ||lkinfo_list_temp==NULL || pfrom_list_temp==NULL || pto_list_temp==NULL||nfrom_list_temp1==NULL||nto_list_temp1==NULL)
			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName, "Malloc unsuccessful 7",ErrMess,1);
				if(lkid_list_temp!=NULL)
					free(lkid_list_temp);
				if(nfrom_list_temp!=NULL)
					free(nfrom_list_temp);
				if(nto_list_temp!=NULL)
					free(nto_list_temp);
				if(nfrom_list_temp1!=NULL)
					free(nfrom_list_temp1);
				if(nto_list_temp1!=NULL)
					free(nto_list_temp1);
				if(lktype_temp!=NULL)
					free(lktype_temp);
				if(lkinfo_list_temp!=NULL)
					free(lkinfo_list_temp);
				if(pfrom_list_temp!=NULL)
					free(pfrom_list_temp);
				if(pto_list_temp!=NULL)
					free(pto_list_temp);
				if(port_nbrfrom!=NULL)
					free(port_nbrfrom);
				if(port_nbrto!=NULL)
					free(port_nbrto);
				if(nodename_from1!=NULL)
					free(nodename_from1);
				if(nodename_to1!=NULL)
					free(nodename_to1);
				if(lkinfo_temp!=NULL)
					free(lkinfo_temp);
				if(lkinfo_list_temp!=NULL)
					free(lkinfo_list_temp);
				return -1;
			}
			lkweight_list_temp=(int*)realloc(lkweight_list_temp,prefetch_rows*sizeof(int));
			bidirectional_list_temp=(int*)realloc(bidirectional_list_temp,prefetch_rows*sizeof(int));
			lkused_list_temp=(int*)realloc(lkused_list_temp,prefetch_rows*sizeof(int));
			if(bidirectional_list_temp==NULL  || lkused_list_temp==NULL ||lkweight_list_temp==NULL)
			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName, "Malloc unsuccessful 8",ErrMess,1);
				if(lkid_list_temp!=NULL)
					free(lkid_list_temp);
				if(nfrom_list_temp!=NULL)
					free(nfrom_list_temp);
				if(nto_list_temp!=NULL)
					free(nto_list_temp);
				if(nfrom_list_temp1!=NULL)
					free(nfrom_list_temp1);
				if(nto_list_temp1!=NULL)
					free(nto_list_temp1);
				if(lktype_temp!=NULL)
					free(lktype_temp);
				if(pfrom_list_temp!=NULL)
					free(pfrom_list_temp);
				if(pto_list_temp!=NULL)
					free(pto_list_temp);
				if(bidirectional_list_temp!=NULL)
					free(bidirectional_list_temp);
				if(lkused_list_temp!=NULL)
					free(lkused_list_temp);
				if(lkweight_list_temp!=NULL)
					free(lkweight_list_temp);
				if(port_nbrfrom!=NULL)
					free(port_nbrfrom);
				if(port_nbrto!=NULL)
					free(port_nbrto);
				if(nodename_from1!=NULL)
					free(nodename_from1);
				if(nodename_to1!=NULL)
					free(nodename_to1);
				if(lkinfo_temp!=NULL)
					free(lkinfo_temp);
				if(lkinfo_list_temp!=NULL)
					free(lkinfo_list_temp);
				return -1;
			}
		}
		//std :://cout<<"before strcpy all " <<std::endl;
		//strcpy(pfrom_list_temp,"?");
		//strcpy(pto_list_temp,"?");
		//strcpy(nfrom_list_temp1,"?");
		//strcpy(nto_list_temp1,"?");
		//strcpy(lkinfo_list_temp,"?");
		//pos2=2;
		//pos2bis=2;

		//node_pos2=2;
		//node_pos2bis=2;

		//lkinfo_pos2=2;
		pos2=0;
		pos2bis=0;
		i=0;
		node_pos2=0;
		node_pos2bis=0;
		pos3=0;
		pos3bis=0;
		node_pos3=0;
		node_pos3bis=0;
		lkinfo_pos2=0;
		lkinfo_pos3=0;
		//std :://cout<<"before the loop " <<std::endl;
		while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
		{
			Format_output(pfrom_null, port_nbrfrom, logmessage, '|');
			Format_output(pto_null, port_nbrto, logmessage, '|');
			Format_output(nfrom_null, nodename_from1, logmessage, '|');
			Format_output(nto_null, nodename_to1, logmessage, '|');
			Format_output(lkinfo_null, lkinfo_temp, logmessage, '|');
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);
			if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{

				//std :://cout<<"after copy line by line nfrom="<<nfrom_list_temp1<<std::endl;
				//std :://cout<<"after copy line by line nto="<<nto_list_temp1<<std::endl;
				//std :://cout<<"after copy line by line pfrom="<<pfrom_list_temp<<std::endl;
				//std :://cout<<"after copy line by line pto_temp="<<pto_list_temp<<std::endl;
				//std :://cout<<"after copy line by line lkinfo_temp="<<lkinfo_list_temp<<std::endl;

				lkid_list_temp[i]=linkid;
				lkweight_list_temp[i]=lkweight;
				nfrom_list_temp[i]=node_from;
				nto_list_temp[i]=node_to;
				bidirectional_list_temp[i]=bidirectional_link_used;
				lkused_list_temp[i]=lkused;
				lktype_temp[i]=link_type;
				pos1=strcspn(port_nbrfrom,"?");
				pos1bis=strcspn(port_nbrto,"?");

				pos2+=pos1+1;
				pos2bis+=pos1bis+1;
				if(pos1+1>max_pfrom_len)
					max_pfrom_len=pos1+1;
				if(pos1bis+1>max_pto_len)
					max_pto_len=pos1bis+1;

				node_pos1=strcspn(nodename_from1,"?");
				node_pos1bis=strcspn(nodename_to1,"?");

				node_pos2+=node_pos1+1;
				node_pos2bis+=node_pos1bis+1;
				if(node_pos1+1>max_nfrom_len)
					max_nfrom_len=node_pos1+1;
				if(node_pos1bis+1>max_nto_len)
					max_nto_len=node_pos1bis+1;

				lkinfo_pos1=strcspn(lkinfo_temp,"?");

				lkinfo_pos2+=lkinfo_pos1+1;
				if(lkinfo_pos1+1>max_lkinfo_len)
					max_lkinfo_len=lkinfo_pos1+1;
				if((bidi_link==0 || bidi_link==1) && bidirectional_link_used==1) // we revert the link...
				{
					pos2+=pos1bis+1;
					pos2bis+=pos1+1;
					node_pos2+=node_pos1bis+1;
					node_pos2bis+=node_pos1+1;
					lkinfo_pos2+=lkinfo_pos1+1;
				}

				pfrom_list_temp=(char*)realloc(pfrom_list_temp,(pos2)*sizeof(char));
				pto_list_temp=(char*)realloc(pto_list_temp,(pos2bis)*sizeof(char));

				nfrom_list_temp1=(char*)realloc(nfrom_list_temp1,(node_pos2)*sizeof(char));
				nto_list_temp1=(char*)realloc(nto_list_temp1,(node_pos2bis)*sizeof(char));

				lkinfo_list_temp=(char*)realloc(lkinfo_list_temp,lkinfo_pos2*sizeof(char));

				if(lkinfo_list_temp==NULL||pfrom_list_temp==NULL ||pto_list_temp==NULL ||nfrom_list_temp1==NULL ||nto_list_temp1==NULL)
				{
					rescode=ShowErrors (status, ociError, "Malloc unsuccessful");
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName, "Malloc unsuccessful 9",ErrMess,1);
					if(lkid_list_temp!=NULL)
						free(lkid_list_temp);
					if(nfrom_list_temp!=NULL)
						free(nfrom_list_temp);
					if(nto_list_temp!=NULL)
						free(nto_list_temp);
					if(nfrom_list_temp1!=NULL)
						free(nfrom_list_temp1);
					if(nto_list_temp1!=NULL)
						free(nto_list_temp1);
					if(lktype_temp!=NULL)
						free(lktype_temp);
					if(pfrom_list_temp!=NULL)
						free(pfrom_list_temp);
					if(pto_list_temp!=NULL)
						free(pto_list_temp);
					if(bidirectional_list_temp!=NULL)
						free(bidirectional_list_temp);
					if(lkused_list_temp!=NULL)
						free(lkused_list_temp);
					if(port_nbrfrom!=NULL)
						free(port_nbrfrom);
					if(port_nbrto!=NULL)
						free(port_nbrto);
					if(nodename_from1!=NULL)
						free(nodename_from1);
					if(nodename_to1!=NULL)
						free(nodename_to1);
					if(lkweight_list_temp!=NULL)
						free(lkweight_list_temp);
					if(lkinfo_temp!=NULL)
						free(lkinfo_temp);
					if(lkinfo_list_temp!=NULL)
						free(lkinfo_list_temp);
					return -1;
				}
				else
				{
					//if(i==0)
					//	std :://cout<<"before copy line by line port_nbrfrom="<<port_nbrfrom<<std::endl;
					memcpy(pfrom_list_temp+pos3,port_nbrfrom,pos1+1);
					memcpy(pto_list_temp+pos3bis,port_nbrto,pos1bis+1);
					memcpy(nfrom_list_temp1+node_pos3,nodename_from1,node_pos1+1);
					memcpy(nto_list_temp1+node_pos3bis,nodename_to1,node_pos1bis+1);
					memcpy(lkinfo_list_temp+lkinfo_pos3,lkinfo_temp,lkinfo_pos1+1);

					pos3+=pos1+1;
					pos3bis+=pos1bis+1;
					node_pos3+=node_pos1+1;
					node_pos3bis+=node_pos1bis+1;
					lkinfo_pos3+=lkinfo_pos1+1;
					i++;
					if((bidi_link==0 || bidi_link==1) && bidirectional_link_used==1) // we revert the link...
					{

						if(pos1bis+1>max_pfrom_len)
							max_pfrom_len=pos1bis+1;
						if(pos1+1>max_pto_len)
							max_pto_len=pos1+1;
						if(node_pos1bis+1>max_nfrom_len)
							max_nfrom_len=node_pos1bis+1;
						if(node_pos1+1>max_nto_len)
							max_nto_len=node_pos1+1;

						memcpy(pfrom_list_temp+pos3,port_nbrto,pos1bis+1);
						memcpy(pto_list_temp+pos3bis,port_nbrfrom,pos1+1);
						memcpy(nfrom_list_temp1+node_pos3,nodename_to1,node_pos1bis+1);
						memcpy(nto_list_temp1+node_pos3bis,nodename_from1,node_pos1+1);
						memcpy(lkinfo_list_temp+lkinfo_pos3,lkinfo_temp,lkinfo_pos1+1);

						lkid_list_temp[i]=-linkid;
						if(lkweight==2)
							lkweight_list_temp[i]=1;
						else
						{
							if(lkweight==1)
								lkweight_list_temp[i]=2;
							else
								lkweight_list_temp[i]=lkweight;
						}
						nfrom_list_temp[i]=node_to;
						nto_list_temp[i]=node_from;
						bidirectional_list_temp[i]=bidirectional_link_used;
						lkused_list_temp[i]=lkused;
						lktype_temp[i]=link_type;
						pos3+=pos1bis+1;
						pos3bis+=pos1+1;
						node_pos3+=node_pos1bis+1;
						node_pos3bis+=node_pos1+1;
						lkinfo_pos3+=lkinfo_pos1+1;
						i++;
					}

				}

			}

			if(i>0 && (i%prefetch_rows)==0)
			{
				round_trip++;
				lkid_list_temp=(int*)realloc(lkid_list_temp,round_trip*prefetch_rows*sizeof(int));
				nfrom_list_temp=(int*)realloc(nfrom_list_temp,round_trip*prefetch_rows*sizeof(int));
				nto_list_temp=(int*)realloc(nto_list_temp,round_trip*prefetch_rows*sizeof(int));
				bidirectional_list_temp=(int*)realloc(bidirectional_list_temp,round_trip*prefetch_rows*sizeof(int));
				lkused_list_temp=(int*)realloc(lkused_list_temp,round_trip*prefetch_rows*sizeof(int));
				lktype_temp=(int*)realloc(lktype_temp,round_trip*prefetch_rows*sizeof(int));
				lkweight_list_temp=(int*)realloc(lkweight_list_temp,round_trip*prefetch_rows*sizeof(int));
				if(lkid_list_temp==NULL  || nfrom_list_temp==NULL || nto_list_temp==NULL)
				{
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName, "Malloc unsuccessful 10",ErrMess,1);
					if(lkid_list_temp!=NULL)
						free(lkid_list_temp);
					if(nfrom_list_temp!=NULL)
						free(nfrom_list_temp);
					if(nto_list_temp!=NULL)
						free(nto_list_temp);
					if(nfrom_list_temp1!=NULL)
						free(nfrom_list_temp1);
					if(nto_list_temp1!=NULL)
						free(nto_list_temp1);
					if(lktype_temp!=NULL)
						free(lktype_temp);
					if(pfrom_list_temp!=NULL)
						free(pfrom_list_temp);
					if(pto_list_temp!=NULL)
						free(pto_list_temp);
					if(bidirectional_list_temp!=NULL)
						free(bidirectional_list_temp);
					if(lkused_list_temp!=NULL)
						free(lkused_list_temp);
					if(port_nbrfrom!=NULL)
						free(port_nbrfrom);
					if(port_nbrto!=NULL)
						free(port_nbrto);
					if(nodename_from1!=NULL)
						free(nodename_from1);
					if(nodename_to1!=NULL)
						free(nodename_to1);
					if(lkweight_list_temp!=NULL)
						free(lkweight_list_temp);
					if(lkinfo_temp!=NULL)
						free(lkinfo_temp);
					if(lkinfo_list_temp!=NULL)
						free(lkinfo_list_temp);
					return -1;
				}

				if(bidirectional_list_temp==NULL  || lkused_list_temp==NULL ||lktype_temp==NULL)
				{
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName, "Malloc unsuccessful 11",ErrMess,1);
					if(lkid_list_temp!=NULL)
						free(lkid_list_temp);
					if(nfrom_list_temp!=NULL)
						free(nfrom_list_temp);
					if(nto_list_temp!=NULL)
						free(nto_list_temp);
					if(nfrom_list_temp1!=NULL)
						free(nfrom_list_temp1);
					if(nto_list_temp1!=NULL)
						free(nto_list_temp1);
					if(lktype_temp!=NULL)
						free(lktype_temp);
					if(pfrom_list_temp!=NULL)
						free(pfrom_list_temp);
					if(pto_list_temp!=NULL)
						free(pto_list_temp);
					if(bidirectional_list_temp!=NULL)
						free(bidirectional_list_temp);
					if(lkused_list_temp!=NULL)
						free(lkused_list_temp);
					if(port_nbrfrom!=NULL)
						free(port_nbrfrom);
					if(port_nbrto!=NULL)
						free(port_nbrto);
					if(nodename_from1!=NULL)
						free(nodename_from1);
					if(nodename_to1!=NULL)
						free(nodename_to1);
					if(lkweight_list_temp!=NULL)
						free(lkweight_list_temp);
					if(lkinfo_temp!=NULL)
						free(lkinfo_temp);
					if(lkinfo_list_temp!=NULL)
						free(lkinfo_list_temp);
					return -1;
				}
			}
		}
		//std :://cout<<"after looping i=" <<i<<std::endl;
		if(status!=OCI_SUCCESS && status != OCI_SUCCESS_WITH_INFO)
		{
			if(rescode==0 && i==0)
			{
				rescode=ShowErrors (status, ociError, "NO_ROWS_SELECTED");
				if(lkid_list_temp!=NULL)
					free(lkid_list_temp);
				if(nfrom_list_temp!=NULL)
					free(nfrom_list_temp);
				if(nto_list_temp!=NULL)
					free(nto_list_temp);
				if(nfrom_list_temp1!=NULL)
					free(nfrom_list_temp1);
				if(nto_list_temp1!=NULL)
					free(nto_list_temp1);
				if(lktype_temp!=NULL)
					free(lktype_temp);
				if(pfrom_list_temp!=NULL)
					free(pfrom_list_temp);
				if(pto_list_temp!=NULL)
					free(pto_list_temp);
				if(bidirectional_list_temp!=NULL)
					free(bidirectional_list_temp);
				if(lkused_list_temp!=NULL)
					free(lkused_list_temp);
				if(port_nbrfrom!=NULL)
					free(port_nbrfrom);
				if(port_nbrto!=NULL)
					free(port_nbrto);
				if(nodename_from1!=NULL)
					free(nodename_from1);
				if(nodename_to1!=NULL)
					free(nodename_to1);
				if(lkweight_list_temp!=NULL)
					free(lkweight_list_temp);
				if(lkinfo_temp!=NULL)
					free(lkinfo_temp);
				if(lkinfo_list_temp!=NULL)
					free(lkinfo_list_temp);
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName, "NO_ROWS_SELECTED",ErrMess,1);
				len_array_gv=0;
				return -1; //error here we don't treat 

			}
		}
		//std:://cout<<"value of _nfrom_name_temp="<<nfrom_list_temp1<<std::endl;
		//std:://cout<<"before copying in the array " <<std::endl;
		//std:://cout<<"1 end of pto_list_temp+pos1= " <<pto_list_temp+1300<<std::endl;
		//std:://cout<<"1 end of nfrom_array_list+pos1= " <<nfrom_array_list+1300<<std::endl;
		if(rescode==0)
		{
			if(getallpaths==0)
			{
				if(bidi_link==0)
				{
					max_rows=i;	
					pfrom_array_list=(char**)malloc(i*sizeof(char*));
					pto_array_list=(char**)malloc(i*sizeof(char*));
					nfrom_array_list=(char**)malloc(i*sizeof(char*));
					nto_array_list=(char**)malloc(i*sizeof(char*));
					lkinfo_array_list=(char**)malloc(i*sizeof(char*));
					len_array_gv=i;
					pfrom_list_temp[pos2-1]='\0';
					pto_list_temp[pos2bis-1]='\0';
					nfrom_list_temp1[node_pos2-1]='\0';
					nto_list_temp1[node_pos2bis-1]='\0';
					lkinfo_list_temp[lkinfo_pos2-1]='\0';
					pos1=0;
					pos6=0;
					pos7=0;
					pos8=0;
					pos9=0;
					if(pfrom_array_list==NULL||pto_array_list==NULL||nfrom_array_list==NULL||nto_array_list==NULL||lkinfo_array_list==NULL)
					{
						if(pfrom_array_list!=NULL)
							free(pfrom_array_list);
						if(pto_array_list!=NULL)
							free(pto_array_list);
						if(nfrom_array_list!=NULL)
							free(nfrom_array_list);
						if(nto_array_list!=NULL)
							free(nto_array_list);
						if(lkinfo_array_list!=NULL)
							free(lkinfo_array_list);
						if(lkid_list_temp!=NULL)
							free(lkid_list_temp);
						if(nfrom_list_temp!=NULL)
							free(nfrom_list_temp);
						if(nto_list_temp!=NULL)
							free(nto_list_temp);
						if(nfrom_list_temp1!=NULL)
							free(nfrom_list_temp1);
						if(nto_list_temp1!=NULL)
							free(nto_list_temp1);
						if(lktype_temp!=NULL)
							free(lktype_temp);
						if(pfrom_list_temp!=NULL)
							free(pfrom_list_temp);
						if(pto_list_temp!=NULL)
							free(pto_list_temp);
						if(bidirectional_list_temp!=NULL)
							free(bidirectional_list_temp);
						if(lkweight_list_temp!=NULL)
							free(lkweight_list_temp);
						if(lkused_list_temp!=NULL)
							free(lkused_list_temp);
						if(port_nbrfrom!=NULL)
							free(port_nbrfrom);
						if(port_nbrto!=NULL)
							free(port_nbrto);
						if(nodename_from1!=NULL)
							free(nodename_from1);
						if(nodename_to1!=NULL)
							free(nodename_to1);
						if(lkinfo_temp!=NULL)
							free(lkinfo_temp);
						if(lkinfo_list_temp!=NULL)
							free(lkinfo_list_temp);
						rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
						return -1;
					}
					lkid_list=(int*)malloc(len_array_gv*sizeof(int));
					node_from_list=(int*)malloc(len_array_gv*sizeof(int));
					bidirectional_list=(int*)malloc(len_array_gv*sizeof(int));
					node_to_list=(int*)malloc(len_array_gv*sizeof(int));
					lkused_list=(int*)malloc(len_array_gv*sizeof(int));
					lk_weight_list=(int*)malloc(len_array_gv*sizeof(int));
					lktype_list=(int*)malloc(len_array_gv*sizeof(int));
					if(lkid_list==NULL||node_from_list==NULL||bidirectional_list==NULL||node_to_list==NULL||lkused_list==NULL||lk_weight_list==NULL||lktype_list==NULL)
					{
						if(lkid_list!=NULL)
							free(lkid_list);
						if(node_from_list!=NULL)
							free(node_from_list);
						if(node_to_list!=NULL)
							free(node_to_list);
						if(bidirectional_list!=NULL)
							free(bidirectional_list);
						if(lk_weight_list!=NULL)
							free(lk_weight_list);
						if(lktype_list!=NULL)
							free(lktype_list);
						if(lkused_list!=NULL)
							free(lkused_list);

						if(pfrom_array_list!=NULL)
							free(pfrom_array_list);
						if(pto_array_list!=NULL)
							free(pto_array_list);
						if(nfrom_array_list!=NULL)
							free(nfrom_array_list);
						if(nto_array_list!=NULL)
							free(nto_array_list);
						if(lkinfo_array_list!=NULL)
							free(lkinfo_array_list);
						if(lkid_list_temp!=NULL)
							free(lkid_list_temp);
						if(nfrom_list_temp!=NULL)
							free(nfrom_list_temp);
						if(nto_list_temp!=NULL)
							free(nto_list_temp);
						if(nfrom_list_temp1!=NULL)
							free(nfrom_list_temp1);
						if(nto_list_temp1!=NULL)
							free(nto_list_temp1);
						if(lktype_temp!=NULL)
							free(lktype_temp);
						if(pfrom_list_temp!=NULL)
							free(pfrom_list_temp);
						if(pto_list_temp!=NULL)
							free(pto_list_temp);
						if(bidirectional_list_temp!=NULL)
							free(bidirectional_list_temp);
						if(lkweight_list_temp!=NULL)
							free(lkweight_list_temp);
						if(lkused_list_temp!=NULL)
							free(lkused_list_temp);
						if(port_nbrfrom!=NULL)
							free(port_nbrfrom);
						if(port_nbrto!=NULL)
							free(port_nbrto);
						if(nodename_from1!=NULL)
							free(nodename_from1);
						if(nodename_to1!=NULL)
							free(nodename_to1);
						if(lkinfo_temp!=NULL)
							free(lkinfo_temp);
						if(lkinfo_list_temp!=NULL)
							free(lkinfo_list_temp);
						rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
						return -1;
					}
					//std:://cout<<"first allocation success " <<std::endl;
					for(j=0;j<i;j++)
					{

						lkid_list[j]=lkid_list_temp[j];
						node_from_list[j]=nfrom_list_temp[j];
						node_to_list[j]=nto_list_temp[j];
						bidirectional_list[j]=bidirectional_list_temp[j];
						lkused_list[j]=lkused_list_temp[j];
						lktype_list[j]=lktype_temp[j];
						lk_weight_list[j]=lkweight_list_temp[j];
						pfrom_array_list[j]=(char*)malloc(max_pfrom_len*sizeof(char));
						pto_array_list[j]=(char*)malloc(max_pto_len*sizeof(char));
						nfrom_array_list[j]=(char*)malloc(max_nfrom_len*sizeof(char));
						nto_array_list[j]=(char*)malloc(max_nto_len*sizeof(char));
						lkinfo_array_list[j]=(char*)malloc(max_lkinfo_len*sizeof(char));
						if(pfrom_array_list[j]==NULL ||nfrom_array_list[j]==NULL||pto_array_list[j]==NULL||nto_array_list[j]==NULL||lkinfo_array_list[j]==NULL)
						{
							for(k=0;k<j;k++)
							{
								free(pfrom_array_list[k]);
								free(pto_array_list[k]);
								free(nfrom_array_list[k]);
								free(nto_array_list[k]);
								free(lkinfo_array_list[k]);
							}
							if(pfrom_array_list[j]!=NULL)
								free(pfrom_array_list[j]);
							if(pto_array_list[j]!=NULL)
								free(pto_array_list[j]);
							if(nfrom_array_list[j]!=NULL)
								free(nfrom_array_list[j]);
							if(nto_array_list[j]!=NULL)
								free(nto_array_list[j]);
							if(pfrom_array_list!=NULL)
								free(pfrom_array_list);
							if(pto_array_list!=NULL)
								free(pto_array_list);
							if(nfrom_array_list!=NULL)
								free(nfrom_array_list);
							if(nto_array_list!=NULL)
								free(nto_array_list);
							if(lkinfo_array_list!=NULL)
								free(lkinfo_array_list);
							if(lkid_list_temp!=NULL)
								free(lkid_list_temp);
							if(nfrom_list_temp!=NULL)
								free(nfrom_list_temp);
							if(nto_list_temp!=NULL)
								free(nto_list_temp);
							if(nfrom_list_temp1!=NULL)
								free(nfrom_list_temp1);
							if(nto_list_temp1!=NULL)
								free(nto_list_temp1);
							if(lktype_temp!=NULL)
								free(lktype_temp);
							if(pfrom_list_temp!=NULL)
								free(pfrom_list_temp);
							if(pto_list_temp!=NULL)
								free(pto_list_temp);
							if(bidirectional_list_temp!=NULL)
								free(bidirectional_list_temp);
							if(lkweight_list_temp!=NULL)
								free(lkweight_list_temp);
							if(lkused_list_temp!=NULL)
								free(lkused_list_temp);
							if(port_nbrfrom!=NULL)
								free(port_nbrfrom);
							if(port_nbrto!=NULL)
								free(port_nbrto);
							if(nodename_from1!=NULL)
								free(nodename_from1);
							if(nodename_to1!=NULL)
								free(nodename_to1);
							if(lkinfo_temp!=NULL)
								free(lkinfo_temp);
							if(lkinfo_list_temp!=NULL)
								free(lkinfo_list_temp);
							if(lkid_list!=NULL)
								free(lkid_list);
							if(node_from_list!=NULL)
								free(node_from_list);
							if(node_to_list!=NULL)
								free(node_to_list);
							if(bidirectional_list!=NULL)
								free(bidirectional_list);
							if(lk_weight_list!=NULL)
								free(lk_weight_list);
							if(lktype_list!=NULL)
								free(lktype_list);
							if(lkused_list!=NULL)
								free(lkused_list);
							rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
							status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
							GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
							return -1;
						}
						else
						{
							//	if(j>9950)
							//	std:://cout<<"before copying success " <<j<<std::endl;
							pos1bis=strcspn(pfrom_list_temp+pos1,"?");
							pfrom_list_temp[pos1bis+pos1]='\0';
							strcpy(pfrom_array_list[j],pfrom_list_temp+pos1);
							pos1+=pos1bis+1;

							pos6bis=strcspn(pto_list_temp+pos6,"?");
							pto_list_temp[pos6bis+pos6]='\0';
							strcpy(pto_array_list[j],pto_list_temp+pos6);
							pos6+=pos6bis+1;

							//if(j==1330)
							//std:://cout<<"1 end of pto_list_temp+pos1= " <<pto_list_temp+pos6<<std::endl;

							pos7bis=strcspn(nfrom_list_temp1+pos7,"?");
							nfrom_list_temp1[pos7bis+pos7]='\0';
							strcpy(nfrom_array_list[j],nfrom_list_temp1+pos7);
							pos7+=pos7bis+1;

							//if(j==1330)
							//std:://cout<<"3 end of copy nfrom_list_temp1+pos7=" <<nfrom_list_temp1+pos7<<std::endl;

							pos8bis=strcspn(nto_list_temp1+pos8,"?");
							nto_list_temp1[pos8bis+pos8]='\0';
							strcpy(nto_array_list[j],nto_list_temp1+pos8);
							pos8+=pos8bis+1;

							//if(j==1330)
							//std:://cout<<"4 end of copy " <<nto_list_temp1+pos8<<std::endl;
							pos9bis=strcspn(lkinfo_list_temp+pos9,"?");
							lkinfo_list_temp[pos9bis+pos9]='\0';
							strcpy(lkinfo_array_list[j],lkinfo_list_temp+pos9);
							pos9+=pos9bis+1;

							//if(j==1330)
							//std:://cout<<"end of copy " <<lkinfo_list_temp+pos9<<std::endl;


						}

					}	
				}
				else
				{
					if(bidi_link==1)
					{
						max_rows_scd=i;	
						pfrom_array_list_scd=(char**)malloc(i*sizeof(char*));
						pto_array_list_scd=(char**)malloc(i*sizeof(char*));
						nfrom_array_list_scd=(char**)malloc(i*sizeof(char*));
						nto_array_list_scd=(char**)malloc(i*sizeof(char*));
						lkinfo_array_list_scd=(char**)malloc(i*sizeof(char*));
						len_array_gv_scd=i;
						pfrom_list_temp[pos2-1]='\0';
						pto_list_temp[pos2bis-1]='\0';
						nfrom_list_temp1[node_pos2-1]='\0';
						nto_list_temp1[node_pos2bis-1]='\0';
						lkinfo_list_temp[lkinfo_pos2-1]='\0';
						pos1=0;
						pos6=0;
						pos7=0;
						pos8=0;
						pos9=0;
						if(pfrom_array_list_scd==NULL||pto_array_list_scd==NULL||nfrom_array_list_scd==NULL||nto_array_list_scd==NULL||lkinfo_array_list_scd==NULL)
						{
							if(pfrom_array_list_scd!=NULL)
								free(pfrom_array_list_scd);
							if(pto_array_list_scd!=NULL)
								free(pto_array_list_scd);
							if(nfrom_array_list_scd!=NULL)
								free(nfrom_array_list_scd);
							if(nto_array_list_scd!=NULL)
								free(nto_array_list_scd);
							if(lkinfo_array_list_scd!=NULL)
								free(lkinfo_array_list_scd);
							if(lkid_list_temp!=NULL)
								free(lkid_list_temp);
							if(nfrom_list_temp!=NULL)
								free(nfrom_list_temp);
							if(nto_list_temp!=NULL)
								free(nto_list_temp);
							if(nfrom_list_temp1!=NULL)
								free(nfrom_list_temp1);
							if(nto_list_temp1!=NULL)
								free(nto_list_temp1);
							if(lktype_temp!=NULL)
								free(lktype_temp);
							if(pfrom_list_temp!=NULL)
								free(pfrom_list_temp);
							if(pto_list_temp!=NULL)
								free(pto_list_temp);
							if(bidirectional_list_temp!=NULL)
								free(bidirectional_list_temp);
							if(lkweight_list_temp!=NULL)
								free(lkweight_list_temp);
							if(lkused_list_temp!=NULL)
								free(lkused_list_temp);
							if(port_nbrfrom!=NULL)
								free(port_nbrfrom);
							if(port_nbrto!=NULL)
								free(port_nbrto);
							if(nodename_from1!=NULL)
								free(nodename_from1);
							if(nodename_to1!=NULL)
								free(nodename_to1);
							if(lkinfo_temp!=NULL)
								free(lkinfo_temp);
							if(lkinfo_list_temp!=NULL)
								free(lkinfo_list_temp);
							rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
							status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
							GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
							return -1;
						}
						lkid_list_scd=(int*)malloc(len_array_gv_scd*sizeof(int));
						node_from_list_scd=(int*)malloc(len_array_gv_scd*sizeof(int));
						bidirectional_list_scd=(int*)malloc(len_array_gv_scd*sizeof(int));
						node_to_list_scd=(int*)malloc(len_array_gv_scd*sizeof(int));
						lkused_list_scd=(int*)malloc(len_array_gv_scd*sizeof(int));
						lk_weight_list_scd=(int*)malloc(len_array_gv_scd*sizeof(int));
						lktype_list_scd=(int*)malloc(len_array_gv_scd*sizeof(int));
						if(lkid_list_scd==NULL||node_from_list_scd==NULL||bidirectional_list_scd==NULL||node_to_list_scd==NULL||lkused_list_scd==NULL||lk_weight_list_scd==NULL||lktype_list_scd==NULL)
						{
							if(lkid_list_scd!=NULL)
								free(lkid_list_scd);
							if(node_from_list_scd!=NULL)
								free(node_from_list_scd);
							if(node_to_list_scd!=NULL)
								free(node_to_list_scd);
							if(bidirectional_list_scd!=NULL)
								free(bidirectional_list_scd);
							if(lk_weight_list_scd!=NULL)
								free(lk_weight_list_scd);
							if(lktype_list_scd!=NULL)
								free(lktype_list_scd);
							if(lkused_list_scd!=NULL)
								free(lkused_list_scd);

							if(pfrom_array_list_scd!=NULL)
								free(pfrom_array_list_scd);
							if(pto_array_list_scd!=NULL)
								free(pto_array_list_scd);
							if(nfrom_array_list_scd!=NULL)
								free(nfrom_array_list_scd);
							if(nto_array_list_scd!=NULL)
								free(nto_array_list_scd);
							if(lkinfo_array_list_scd!=NULL)
								free(lkinfo_array_list_scd);
							if(lkid_list_temp!=NULL)
								free(lkid_list_temp);
							if(nfrom_list_temp!=NULL)
								free(nfrom_list_temp);
							if(nto_list_temp!=NULL)
								free(nto_list_temp);
							if(nfrom_list_temp1!=NULL)
								free(nfrom_list_temp1);
							if(nto_list_temp1!=NULL)
								free(nto_list_temp1);
							if(lktype_temp!=NULL)
								free(lktype_temp);
							if(pfrom_list_temp!=NULL)
								free(pfrom_list_temp);
							if(pto_list_temp!=NULL)
								free(pto_list_temp);
							if(bidirectional_list_temp!=NULL)
								free(bidirectional_list_temp);
							if(lkweight_list_temp!=NULL)
								free(lkweight_list_temp);
							if(lkused_list_temp!=NULL)
								free(lkused_list_temp);
							if(port_nbrfrom!=NULL)
								free(port_nbrfrom);
							if(port_nbrto!=NULL)
								free(port_nbrto);
							if(nodename_from1!=NULL)
								free(nodename_from1);
							if(nodename_to1!=NULL)
								free(nodename_to1);
							if(lkinfo_temp!=NULL)
								free(lkinfo_temp);
							if(lkinfo_list_temp!=NULL)
								free(lkinfo_list_temp);
							rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
							status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
							GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
							return -1;
						}
						for(j=0;j<i;j++)
						{
							lkid_list_scd[j]=lkid_list_temp[j];
							node_from_list_scd[j]=nfrom_list_temp[j];
							node_to_list_scd[j]=nto_list_temp[j];
							bidirectional_list_scd[j]=bidirectional_list_temp[j];
							lkused_list_scd[j]=lkused_list_temp[j];
							lktype_list_scd[j]=lktype_temp[j];
							lk_weight_list_scd[j]=lkweight_list_temp[j];
							pfrom_array_list_scd[j]=(char*)malloc(max_pfrom_len*sizeof(char));
							pto_array_list_scd[j]=(char*)malloc(max_pto_len*sizeof(char));
							nfrom_array_list_scd[j]=(char*)malloc(max_nfrom_len*sizeof(char));
							nto_array_list_scd[j]=(char*)malloc(max_nto_len*sizeof(char));
							lkinfo_array_list_scd[j]=(char*)malloc(max_lkinfo_len*sizeof(char));
							if(pfrom_array_list_scd[j]==NULL ||nfrom_array_list_scd[j]==NULL||pto_array_list_scd[j]==NULL||nto_array_list_scd[j]==NULL||lkinfo_array_list_scd[j]==NULL)
							{
								for(k=0;k<j;k++)
								{
									free(pfrom_array_list_scd[k]);
									free(pto_array_list_scd[k]);
									free(nfrom_array_list_scd[k]);
									free(nto_array_list_scd[k]);
									free(lkinfo_array_list_scd[k]);
								}
								if(pfrom_array_list_scd[j]!=NULL)
									free(pfrom_array_list_scd[j]);
								if(pto_array_list_scd[j]!=NULL)
									free(pto_array_list_scd[j]);
								if(nfrom_array_list_scd[j]!=NULL)
									free(nfrom_array_list_scd[j]);
								if(nto_array_list_scd[j]!=NULL)
									free(nto_array_list_scd[j]);
								if(pfrom_array_list_scd!=NULL)
									free(pfrom_array_list_scd);
								if(pto_array_list_scd!=NULL)
									free(pto_array_list_scd);
								if(nfrom_array_list_scd!=NULL)
									free(nfrom_array_list_scd);
								if(nto_array_list_scd!=NULL)
									free(nto_array_list_scd);
								if(lkinfo_array_list_scd!=NULL)
									free(lkinfo_array_list_scd);
								if(lkid_list_temp!=NULL)
									free(lkid_list_temp);
								if(nfrom_list_temp!=NULL)
									free(nfrom_list_temp);
								if(nto_list_temp!=NULL)
									free(nto_list_temp);
								if(nfrom_list_temp1!=NULL)
									free(nfrom_list_temp1);
								if(nto_list_temp1!=NULL)
									free(nto_list_temp1);
								if(lktype_temp!=NULL)
									free(lktype_temp);
								if(pfrom_list_temp!=NULL)
									free(pfrom_list_temp);
								if(pto_list_temp!=NULL)
									free(pto_list_temp);
								if(bidirectional_list_temp!=NULL)
									free(bidirectional_list_temp);
								if(lkweight_list_temp!=NULL)
									free(lkweight_list_temp);
								if(lkused_list_temp!=NULL)
									free(lkused_list_temp);
								if(port_nbrfrom!=NULL)
									free(port_nbrfrom);
								if(port_nbrto!=NULL)
									free(port_nbrto);
								if(nodename_from1!=NULL)
									free(nodename_from1);
								if(nodename_to1!=NULL)
									free(nodename_to1);
								if(lkinfo_temp!=NULL)
									free(lkinfo_temp);
								if(lkinfo_list_temp!=NULL)
									free(lkinfo_list_temp);
								if(lkid_list_scd!=NULL)
									free(lkid_list_scd);
								if(node_from_list_scd!=NULL)
									free(node_from_list_scd);
								if(node_to_list_scd!=NULL)
									free(node_to_list_scd);
								if(bidirectional_list_scd!=NULL)
									free(bidirectional_list_scd);
								if(lk_weight_list_scd!=NULL)
									free(lk_weight_list_scd);
								if(lktype_list_scd!=NULL)
									free(lktype_list_scd);
								if(lkused_list_scd!=NULL)
									free(lkused_list_scd);
								rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
								status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
								GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
								return -1;
							}
							else
							{
								pos1bis=strcspn(pfrom_list_temp+pos1,"?");
								pfrom_list_temp[pos1bis+pos1]='\0';
								strcpy(pfrom_array_list_scd[j],pfrom_list_temp+pos1);
								pos1+=pos1bis+1;

								pos6bis=strcspn(pto_list_temp+pos6,"?");
								pto_list_temp[pos6bis+pos6]='\0';
								strcpy(pto_array_list_scd[j],pto_list_temp+pos6);
								pos6+=pos6bis+1;

								pos7bis=strcspn(nfrom_list_temp1+pos7,"?");
								nfrom_list_temp1[pos7bis+pos7]='\0';
								strcpy(nfrom_array_list_scd[j],nfrom_list_temp1+pos7);
								pos7+=pos7bis+1;

								pos8bis=strcspn(nto_list_temp1+pos8,"?");
								nto_list_temp1[pos8bis+pos8]='\0';
								strcpy(nto_array_list_scd[j],nto_list_temp1+pos8);
								pos8+=pos8bis+1;

								pos9bis=strcspn(lkinfo_list_temp+pos9,"?");
								lkinfo_list_temp[pos9bis+pos9]='\0';
								strcpy(lkinfo_array_list_scd[j],lkinfo_list_temp+pos9);
								pos9+=pos9bis+1;




							}

						}	

					}


				}
			}
			else
			{
				if(bidi_link==2)
				{
					max_rows_bis=i;	
					pfrom_array_list_bis=(char**)malloc(i*sizeof(char*));
					pto_array_list_bis=(char**)malloc(i*sizeof(char*));
					nfrom_array_list_bis=(char**)malloc(i*sizeof(char*));
					nto_array_list_bis=(char**)malloc(i*sizeof(char*));
					lkinfo_array_list_bis=(char**)malloc(i*sizeof(char*));
					len_array_gv_bis=i;
					pfrom_list_temp[pos2-1]='\0';
					pto_list_temp[pos2bis-1]='\0';
					nfrom_list_temp1[node_pos2-1]='\0';
					nto_list_temp1[node_pos2bis-1]='\0';
					lkinfo_list_temp[lkinfo_pos2-1]='\0';
					pos1=0;
					pos6=0;
					pos7=0;
					pos8=0;
					pos9=0;
					if(pfrom_array_list_bis==NULL||pto_array_list_bis==NULL||nfrom_array_list_bis==NULL||nto_array_list_bis==NULL||lkinfo_array_list_bis==NULL)
					{
						if(pfrom_array_list_bis!=NULL)
							free(pfrom_array_list_bis);
						if(pto_array_list_bis!=NULL)
							free(pto_array_list_bis);
						if(nfrom_array_list_bis!=NULL)
							free(nfrom_array_list_bis);
						if(nto_array_list_bis!=NULL)
							free(nto_array_list_bis);
						if(lkinfo_array_list_bis!=NULL)
							free(lkinfo_array_list_bis);
						if(lkid_list_temp!=NULL)
							free(lkid_list_temp);
						if(nfrom_list_temp!=NULL)
							free(nfrom_list_temp);
						if(nto_list_temp!=NULL)
							free(nto_list_temp);
						if(nfrom_list_temp1!=NULL)
							free(nfrom_list_temp1);
						if(nto_list_temp1!=NULL)
							free(nto_list_temp1);
						if(lktype_temp!=NULL)
							free(lktype_temp);
						if(pfrom_list_temp!=NULL)
							free(pfrom_list_temp);
						if(pto_list_temp!=NULL)
							free(pto_list_temp);
						if(bidirectional_list_temp!=NULL)
							free(bidirectional_list_temp);
						if(lkweight_list_temp!=NULL)
							free(lkweight_list_temp);
						if(lkused_list_temp!=NULL)
							free(lkused_list_temp);
						if(port_nbrfrom!=NULL)
							free(port_nbrfrom);
						if(port_nbrto!=NULL)
							free(port_nbrto);
						if(nodename_from1!=NULL)
							free(nodename_from1);
						if(nodename_to1!=NULL)
							free(nodename_to1);
						if(lkinfo_temp!=NULL)
							free(lkinfo_temp);
						if(lkinfo_list_temp!=NULL)
							free(lkinfo_list_temp);
						rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
						return -1;
					}
					lkid_list_bis=(int*)malloc(len_array_gv_bis*sizeof(int));
					node_from_list_bis=(int*)malloc(len_array_gv_bis*sizeof(int));
					bidirectional_list_bis=(int*)malloc(len_array_gv_bis*sizeof(int));
					node_to_list_bis=(int*)malloc(len_array_gv_bis*sizeof(int));
					lkused_list_bis=(int*)malloc(len_array_gv_bis*sizeof(int));
					lk_weight_list_bis=(int*)malloc(len_array_gv_bis*sizeof(int));
					lktype_list_bis=(int*)malloc(len_array_gv_bis*sizeof(int));
					if(lkid_list_bis==NULL||node_from_list_bis==NULL||bidirectional_list_bis==NULL||node_to_list_bis==NULL||lkused_list_bis==NULL||lk_weight_list_bis==NULL||lktype_list_bis==NULL)
					{
						if(lkid_list_bis!=NULL)
							free(lkid_list_bis);
						if(node_from_list_bis!=NULL)
							free(node_from_list_bis);
						if(node_to_list_bis!=NULL)
							free(node_to_list_bis);
						if(bidirectional_list_bis!=NULL)
							free(bidirectional_list_bis);
						if(lk_weight_list_bis!=NULL)
							free(lk_weight_list_bis);
						if(lktype_list_bis!=NULL)
							free(lktype_list_bis);
						if(lkused_list_bis!=NULL)
							free(lkused_list_bis);

						if(pfrom_array_list_bis!=NULL)
							free(pfrom_array_list_bis);
						if(pto_array_list_bis!=NULL)
							free(pto_array_list_bis);
						if(nfrom_array_list_bis!=NULL)
							free(nfrom_array_list_bis);
						if(nto_array_list_bis!=NULL)
							free(nto_array_list_bis);
						if(lkinfo_array_list_bis!=NULL)
							free(lkinfo_array_list_bis);
						if(lkid_list_temp!=NULL)
							free(lkid_list_temp);
						if(nfrom_list_temp!=NULL)
							free(nfrom_list_temp);
						if(nto_list_temp!=NULL)
							free(nto_list_temp);
						if(nfrom_list_temp1!=NULL)
							free(nfrom_list_temp1);
						if(nto_list_temp1!=NULL)
							free(nto_list_temp1);
						if(lktype_temp!=NULL)
							free(lktype_temp);
						if(pfrom_list_temp!=NULL)
							free(pfrom_list_temp);
						if(pto_list_temp!=NULL)
							free(pto_list_temp);
						if(bidirectional_list_temp!=NULL)
							free(bidirectional_list_temp);
						if(lkweight_list_temp!=NULL)
							free(lkweight_list_temp);
						if(lkused_list_temp!=NULL)
							free(lkused_list_temp);
						if(port_nbrfrom!=NULL)
							free(port_nbrfrom);
						if(port_nbrto!=NULL)
							free(port_nbrto);
						if(nodename_from1!=NULL)
							free(nodename_from1);
						if(nodename_to1!=NULL)
							free(nodename_to1);
						if(lkinfo_temp!=NULL)
							free(lkinfo_temp);
						if(lkinfo_list_temp!=NULL)
							free(lkinfo_list_temp);
						rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName, "Malloc unsuccessful 8",ErrMess,1);
						return -1;
					}
					for(j=0;j<i;j++)
					{
						lkid_list_bis[j]=lkid_list_temp[j];
						node_from_list_bis[j]=nfrom_list_temp[j];
						node_to_list_bis[j]=nto_list_temp[j];
						bidirectional_list_bis[j]=bidirectional_list_temp[j];
						lkused_list_bis[j]=lkused_list_temp[j];
						lktype_list_bis[j]=lktype_temp[j];
						lk_weight_list_bis[j]=lkweight_list_temp[j];
						pfrom_array_list_bis[j]=(char*)malloc(max_pfrom_len*sizeof(char));
						pto_array_list_bis[j]=(char*)malloc(max_pto_len*sizeof(char));
						nfrom_array_list_bis[j]=(char*)malloc(max_nfrom_len*sizeof(char));
						nto_array_list_bis[j]=(char*)malloc(max_nto_len*sizeof(char));
						lkinfo_array_list_bis[j]=(char*)malloc(max_lkinfo_len*sizeof(char));
						if(pfrom_array_list_bis[j]==NULL ||nfrom_array_list_bis[j]==NULL||pto_array_list_bis[j]==NULL||nto_array_list_bis[j]==NULL||lkinfo_array_list_bis[j]==NULL)
						{
							for(k=0;k<j;k++)
							{
								free(pfrom_array_list_bis[k]);
								free(pto_array_list_bis[k]);
								free(nfrom_array_list_bis[k]);
								free(nto_array_list_bis[k]);
								free(lkinfo_array_list_bis[k]);
							}
							if(pfrom_array_list_bis[j]!=NULL)
								free(pfrom_array_list_bis[j]);
							if(pto_array_list_bis[j]!=NULL)
								free(pto_array_list_bis[j]);
							if(nfrom_array_list_bis[j]!=NULL)
								free(nfrom_array_list_bis[j]);
							if(nto_array_list_bis[j]!=NULL)
								free(nto_array_list_bis[j]);
							if(pfrom_array_list_bis!=NULL)
								free(pfrom_array_list_bis);
							if(pto_array_list_bis!=NULL)
								free(pto_array_list_bis);
							if(nfrom_array_list_bis!=NULL)
								free(nfrom_array_list_bis);
							if(nto_array_list_bis!=NULL)
								free(nto_array_list_bis);
							if(lkinfo_array_list_bis!=NULL)
								free(lkinfo_array_list_bis);
							if(lkid_list_temp!=NULL)
								free(lkid_list_temp);
							if(nfrom_list_temp!=NULL)
								free(nfrom_list_temp);
							if(nto_list_temp!=NULL)
								free(nto_list_temp);
							if(nfrom_list_temp1!=NULL)
								free(nfrom_list_temp1);
							if(nto_list_temp1!=NULL)
								free(nto_list_temp1);
							if(lktype_temp!=NULL)
								free(lktype_temp);
							if(pfrom_list_temp!=NULL)
								free(pfrom_list_temp);
							if(pto_list_temp!=NULL)
								free(pto_list_temp);
							if(bidirectional_list_temp!=NULL)
								free(bidirectional_list_temp);
							if(lkweight_list_temp!=NULL)
								free(lkweight_list_temp);
							if(lkused_list_temp!=NULL)
								free(lkused_list_temp);
							if(port_nbrfrom!=NULL)
								free(port_nbrfrom);
							if(port_nbrto!=NULL)
								free(port_nbrto);
							if(nodename_from1!=NULL)
								free(nodename_from1);
							if(nodename_to1!=NULL)
								free(nodename_to1);
							if(lkinfo_temp!=NULL)
								free(lkinfo_temp);
							if(lkinfo_list_temp!=NULL)
								free(lkinfo_list_temp);
							if(lkid_list_bis!=NULL)
								free(lkid_list_bis);
							if(node_from_list_bis!=NULL)
								free(node_from_list_bis);
							if(node_to_list_bis!=NULL)
								free(node_to_list_bis);
							if(bidirectional_list_bis!=NULL)
								free(bidirectional_list_bis);
							if(lk_weight_list_bis!=NULL)
								free(lk_weight_list_bis);
							if(lktype_list_bis!=NULL)
								free(lktype_list_bis);
							if(lkused_list_bis!=NULL)
								free(lkused_list_bis);
							rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
							status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
							GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
							return -1;
						}
						else
						{
							pos1bis=strcspn(pfrom_list_temp+pos1,"?");
							pfrom_list_temp[pos1bis+pos1]='\0';
							strcpy(pfrom_array_list_bis[j],pfrom_list_temp+pos1);
							pos1+=pos1bis+1;

							pos6bis=strcspn(pto_list_temp+pos6,"?");
							pto_list_temp[pos6bis+pos6]='\0';
							strcpy(pto_array_list_bis[j],pto_list_temp+pos6);
							pos6+=pos6bis+1;

							pos7bis=strcspn(nfrom_list_temp1+pos7,"?");
							nfrom_list_temp1[pos7bis+pos7]='\0';
							strcpy(nfrom_array_list_bis[j],nfrom_list_temp1+pos7);
							pos7+=pos7bis+1;

							pos8bis=strcspn(nto_list_temp1+pos8,"?");
							nto_list_temp1[pos8bis+pos8]='\0';
							strcpy(nto_array_list_bis[j],nto_list_temp1+pos8);
							pos8+=pos8bis+1;

							pos9bis=strcspn(lkinfo_list_temp+pos9,"?");
							lkinfo_list_temp[pos9bis+pos9]='\0';
							strcpy(lkinfo_array_list_bis[j],lkinfo_list_temp+pos9);
							pos9+=pos9bis+1;




						}

					}	

				}
				else
				{
					max_rows_bis_rev=i;	
					pfrom_array_list_bis_rev=(char**)malloc(i*sizeof(char*));
					pto_array_list_bis_rev=(char**)malloc(i*sizeof(char*));
					nfrom_array_list_bis_rev=(char**)malloc(i*sizeof(char*));
					nto_array_list_bis_rev=(char**)malloc(i*sizeof(char*));
					lkinfo_array_list_bis_rev=(char**)malloc(i*sizeof(char*));
					len_array_gv_bis_rev=i;
					pfrom_list_temp[pos2-1]='\0';
					pto_list_temp[pos2bis-1]='\0';
					nfrom_list_temp1[node_pos2-1]='\0';
					nto_list_temp1[node_pos2bis-1]='\0';
					lkinfo_list_temp[lkinfo_pos2-1]='\0';
					pos1=0;
					pos6=0;
					pos7=0;
					pos8=0;
					pos9=0;
					if(pfrom_array_list_bis_rev==NULL||pto_array_list_bis_rev==NULL||nfrom_array_list_bis_rev==NULL||nto_array_list_bis_rev==NULL||lkinfo_array_list_bis_rev==NULL)
					{
						if(pfrom_array_list_bis_rev!=NULL)
							free(pfrom_array_list_bis_rev);
						if(pto_array_list_bis_rev!=NULL)
							free(pto_array_list_bis_rev);
						if(nfrom_array_list_bis_rev!=NULL)
							free(nfrom_array_list_bis_rev);
						if(nto_array_list_bis_rev!=NULL)
							free(nto_array_list_bis_rev);
						if(lkinfo_array_list_bis_rev!=NULL)
							free(lkinfo_array_list_bis_rev);
						if(lkid_list_temp!=NULL)
							free(lkid_list_temp);
						if(nfrom_list_temp!=NULL)
							free(nfrom_list_temp);
						if(nto_list_temp!=NULL)
							free(nto_list_temp);
						if(nfrom_list_temp1!=NULL)
							free(nfrom_list_temp1);
						if(nto_list_temp1!=NULL)
							free(nto_list_temp1);
						if(lktype_temp!=NULL)
							free(lktype_temp);
						if(pfrom_list_temp!=NULL)
							free(pfrom_list_temp);
						if(pto_list_temp!=NULL)
							free(pto_list_temp);
						if(bidirectional_list_temp!=NULL)
							free(bidirectional_list_temp);
						if(lkweight_list_temp!=NULL)
							free(lkweight_list_temp);
						if(lkused_list_temp!=NULL)
							free(lkused_list_temp);
						if(port_nbrfrom!=NULL)
							free(port_nbrfrom);
						if(port_nbrto!=NULL)
							free(port_nbrto);
						if(nodename_from1!=NULL)
							free(nodename_from1);
						if(nodename_to1!=NULL)
							free(nodename_to1);
						if(lkinfo_temp!=NULL)
							free(lkinfo_temp);
						if(lkinfo_list_temp!=NULL)
							free(lkinfo_list_temp);
						rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
						return -1;
					}
					lkid_list_bis_rev=(int*)malloc(len_array_gv_bis_rev*sizeof(int));
					node_from_list_bis_rev=(int*)malloc(len_array_gv_bis_rev*sizeof(int));
					bidirectional_list_bis_rev=(int*)malloc(len_array_gv_bis_rev*sizeof(int));
					node_to_list_bis_rev=(int*)malloc(len_array_gv_bis_rev*sizeof(int));
					lkused_list_bis_rev=(int*)malloc(len_array_gv_bis_rev*sizeof(int));
					lk_weight_list_bis_rev=(int*)malloc(len_array_gv_bis_rev*sizeof(int));
					lktype_list_bis_rev=(int*)malloc(len_array_gv_bis_rev*sizeof(int));
					if(lkid_list_bis_rev==NULL||node_from_list_bis_rev==NULL||bidirectional_list_bis_rev==NULL||node_to_list_bis_rev==NULL||lkused_list_bis_rev==NULL||lk_weight_list_bis_rev==NULL||lktype_list_bis_rev==NULL)
					{
						if(lkid_list_bis_rev!=NULL)
							free(lkid_list_bis_rev);
						if(node_from_list_bis_rev!=NULL)
							free(node_from_list_bis_rev);
						if(node_to_list_bis_rev!=NULL)
							free(node_to_list_bis_rev);
						if(bidirectional_list_bis_rev!=NULL)
							free(bidirectional_list_bis_rev);
						if(lk_weight_list_bis_rev!=NULL)
							free(lk_weight_list_bis_rev);
						if(lktype_list_bis_rev!=NULL)
							free(lktype_list_bis_rev);
						if(lkused_list_bis_rev!=NULL)
							free(lkused_list_bis_rev);

						if(pfrom_array_list_bis_rev!=NULL)
							free(pfrom_array_list_bis_rev);
						if(pto_array_list_bis_rev!=NULL)
							free(pto_array_list_bis_rev);
						if(nfrom_array_list_bis_rev!=NULL)
							free(nfrom_array_list_bis_rev);
						if(nto_array_list_bis_rev!=NULL)
							free(nto_array_list_bis_rev);
						if(lkinfo_array_list_bis_rev!=NULL)
							free(lkinfo_array_list_bis_rev);
						if(lkid_list_temp!=NULL)
							free(lkid_list_temp);
						if(nfrom_list_temp!=NULL)
							free(nfrom_list_temp);
						if(nto_list_temp!=NULL)
							free(nto_list_temp);
						if(nfrom_list_temp1!=NULL)
							free(nfrom_list_temp1);
						if(nto_list_temp1!=NULL)
							free(nto_list_temp1);
						if(lktype_temp!=NULL)
							free(lktype_temp);
						if(pfrom_list_temp!=NULL)
							free(pfrom_list_temp);
						if(pto_list_temp!=NULL)
							free(pto_list_temp);
						if(bidirectional_list_temp!=NULL)
							free(bidirectional_list_temp);
						if(lkweight_list_temp!=NULL)
							free(lkweight_list_temp);
						if(lkused_list_temp!=NULL)
							free(lkused_list_temp);
						if(port_nbrfrom!=NULL)
							free(port_nbrfrom);
						if(port_nbrto!=NULL)
							free(port_nbrto);
						if(nodename_from1!=NULL)
							free(nodename_from1);
						if(nodename_to1!=NULL)
							free(nodename_to1);
						if(lkinfo_temp!=NULL)
							free(lkinfo_temp);
						if(lkinfo_list_temp!=NULL)
							free(lkinfo_list_temp);
						rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName, "Malloc unsuccessful 8",ErrMess,1);
						return -1;
					}
					for(j=0;j<i;j++)
					{
						lkid_list_bis_rev[j]=lkid_list_temp[j];
						node_from_list_bis_rev[j]=nfrom_list_temp[j];
						node_to_list_bis_rev[j]=nto_list_temp[j];
						bidirectional_list_bis_rev[j]=bidirectional_list_temp[j];
						lkused_list_bis_rev[j]=lkused_list_temp[j];
						lktype_list_bis_rev[j]=lktype_temp[j];
						lk_weight_list_bis_rev[j]=lkweight_list_temp[j];
						pfrom_array_list_bis_rev[j]=(char*)malloc(max_pfrom_len*sizeof(char));
						pto_array_list_bis_rev[j]=(char*)malloc(max_pto_len*sizeof(char));
						nfrom_array_list_bis_rev[j]=(char*)malloc(max_nfrom_len*sizeof(char));
						nto_array_list_bis_rev[j]=(char*)malloc(max_nto_len*sizeof(char));
						lkinfo_array_list_bis_rev[j]=(char*)malloc(max_lkinfo_len*sizeof(char));
						if(pfrom_array_list_bis_rev[j]==NULL ||nfrom_array_list_bis_rev[j]==NULL||pto_array_list_bis_rev[j]==NULL||nto_array_list_bis_rev[j]==NULL||lkinfo_array_list_bis_rev[j]==NULL)
						{
							for(k=0;k<j;k++)
							{
								free(pfrom_array_list_bis_rev[k]);
								free(pto_array_list_bis_rev[k]);
								free(nfrom_array_list_bis_rev[k]);
								free(nto_array_list_bis_rev[k]);
								free(lkinfo_array_list_bis_rev[k]);
							}
							if(pfrom_array_list_bis_rev[j]!=NULL)
								free(pfrom_array_list_bis_rev[j]);
							if(pto_array_list_bis_rev[j]!=NULL)
								free(pto_array_list_bis_rev[j]);
							if(nfrom_array_list_bis_rev[j]!=NULL)
								free(nfrom_array_list_bis_rev[j]);
							if(nto_array_list_bis_rev[j]!=NULL)
								free(nto_array_list_bis_rev[j]);
							if(pfrom_array_list_bis_rev!=NULL)
								free(pfrom_array_list_bis_rev);
							if(pto_array_list_bis_rev!=NULL)
								free(pto_array_list_bis_rev);
							if(nfrom_array_list_bis_rev!=NULL)
								free(nfrom_array_list_bis_rev);
							if(nto_array_list_bis_rev!=NULL)
								free(nto_array_list_bis_rev);
							if(lkinfo_array_list_bis_rev!=NULL)
								free(lkinfo_array_list_bis_rev);
							if(lkid_list_temp!=NULL)
								free(lkid_list_temp);
							if(nfrom_list_temp!=NULL)
								free(nfrom_list_temp);
							if(nto_list_temp!=NULL)
								free(nto_list_temp);
							if(nfrom_list_temp1!=NULL)
								free(nfrom_list_temp1);
							if(nto_list_temp1!=NULL)
								free(nto_list_temp1);
							if(lktype_temp!=NULL)
								free(lktype_temp);
							if(pfrom_list_temp!=NULL)
								free(pfrom_list_temp);
							if(pto_list_temp!=NULL)
								free(pto_list_temp);
							if(bidirectional_list_temp!=NULL)
								free(bidirectional_list_temp);
							if(lkweight_list_temp!=NULL)
								free(lkweight_list_temp);
							if(lkused_list_temp!=NULL)
								free(lkused_list_temp);
							if(port_nbrfrom!=NULL)
								free(port_nbrfrom);
							if(port_nbrto!=NULL)
								free(port_nbrto);
							if(nodename_from1!=NULL)
								free(nodename_from1);
							if(nodename_to1!=NULL)
								free(nodename_to1);
							if(lkinfo_temp!=NULL)
								free(lkinfo_temp);
							if(lkinfo_list_temp!=NULL)
								free(lkinfo_list_temp);
							if(lkid_list_bis_rev!=NULL)
								free(lkid_list_bis_rev);
							if(node_from_list_bis_rev!=NULL)
								free(node_from_list_bis_rev);
							if(node_to_list_bis_rev!=NULL)
								free(node_to_list_bis_rev);
							if(bidirectional_list_bis_rev!=NULL)
								free(bidirectional_list_bis_rev);
							if(lk_weight_list_bis_rev!=NULL)
								free(lk_weight_list_bis_rev);
							if(lktype_list_bis_rev!=NULL)
								free(lktype_list_bis_rev);
							if(lkused_list_bis_rev!=NULL)
								free(lkused_list_bis_rev);
							rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
							status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
							GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
							return -1;
						}
						else
						{
							pos1bis=strcspn(pfrom_list_temp+pos1,"?");
							pfrom_list_temp[pos1bis+pos1]='\0';
							strcpy(pfrom_array_list_bis_rev[j],pfrom_list_temp+pos1);
							pos1+=pos1bis+1;

							pos6bis=strcspn(pto_list_temp+pos6,"?");
							pto_list_temp[pos6bis+pos6]='\0';
							strcpy(pto_array_list_bis_rev[j],pto_list_temp+pos6);
							pos6+=pos6bis+1;

							pos7bis=strcspn(nfrom_list_temp1+pos7,"?");
							nfrom_list_temp1[pos7bis+pos7]='\0';
							strcpy(nfrom_array_list_bis_rev[j],nfrom_list_temp1+pos7);
							pos7+=pos7bis+1;

							pos8bis=strcspn(nto_list_temp1+pos8,"?");
							nto_list_temp1[pos8bis+pos8]='\0';
							strcpy(nto_array_list_bis_rev[j],nto_list_temp1+pos8);
							pos8+=pos8bis+1;

							pos9bis=strcspn(lkinfo_list_temp+pos9,"?");
							lkinfo_list_temp[pos9bis+pos9]='\0';
							strcpy(lkinfo_array_list_bis_rev[j],lkinfo_list_temp+pos9);
							pos9+=pos9bis+1;
						}

					}	

				}
			}
		}

		if(lkid_list_temp!=NULL)
			free(lkid_list_temp);

		if(nfrom_list_temp!=NULL)
			free(nfrom_list_temp);

		if(nto_list_temp!=NULL)
			free(nto_list_temp);

		if(nfrom_list_temp1!=NULL)
			free(nfrom_list_temp1);

		if(nto_list_temp1!=NULL)
			free(nto_list_temp1);


		if(lktype_temp!=NULL)
			free(lktype_temp);

		if(pfrom_list_temp!=NULL)
			free(pfrom_list_temp);

		if(pto_list_temp!=NULL)
			free(pto_list_temp);

		if(bidirectional_list_temp!=NULL)
			free(bidirectional_list_temp);
		if(lkweight_list_temp!=NULL)
			free(lkweight_list_temp);
		if(lkused_list_temp!=NULL)
			free(lkused_list_temp);
		if(port_nbrfrom!=NULL)
			free(port_nbrfrom);
		if(port_nbrto!=NULL)
			free(port_nbrto);
		if(nodename_from1!=NULL)
			free(nodename_from1);
		if(nodename_to1!=NULL)
			free(nodename_to1);
		if(lkinfo_temp!=NULL)
			free(lkinfo_temp);
		if(lkinfo_list_temp!=NULL)
			free(lkinfo_list_temp);
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		//std:://cout<<"loading conn"<<std::endl;
		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 
		else
			OCIReportError(ociError,appliName, ErrMess,0); 

		return rescode;
	}

	/**************************************************************************************/
	/**********************************************************************************/
	/*****************************************************************************************/
	/**
	* Load the link type table of a given system , returning a int :internal fct . INTERNAL FCT CANNOT BE USED
	* @param systemnameList : list of the subsystems (UPPER CASE) for which you want to load the connectivity, separated with a coma : VELO,DAQ,TFC. for instance. Put ALL, if you want to load the whole connectivity table.
	* @param len_array : length of the lkid_list array you have allocated (same as node_from_list,...,lktype_list) we return the length we need if too small)
	* @param dfrom : name of the device (start of the path) : mandatory
	* @param dto : name of the device which ends the path (put "none" if absent)
	* @param bidi_link : 0 to get the real bidirectional link values / 1 if you want to consider all the links as bidirectional used for GetAllPathsPerDevice
	* @param ErrMess : error message in case of failure (NO_ROWS_SELECTED if table empty)
	* @return  0 if successful
	*/
	/*****************************************************************************/

	int LoadInternalConnectivity_InCache(char* systemnameList,char * dfrom,char* dto,int bidi_link,int getallpaths,char* ErrMess)
	{

		int i = 0;
		int j=0;
		int len=0;
		int k=0;
		char appliName[100]="LoadInternalConnectivity_InCache";
		sword status;
		OCIStmt *stmthp;
		OCIDefine* def[1];
		OCIBind  *bndp[2]; 
		int pfrom_null=0;
		int pto_null=0;
		int linkid=0;
		int node_from=0;
		int rescode=0;
		int node_to=0;
		int nfrom_null=0;
		int nto_null=0;
		int pfromlen=0;
		int lkinfo_null=0;
		int ptolen=0;
		int* nfrom_list_temp=NULL;
		int round_trip=0;
		int prefetch_rows=1000;
		char selectconn[2000];

		int sysID=0;
		char sysIDlist[100];


		if(strncmp(systemnameList,"ALL",3)==0)
		{
			sysID=1;
		}
		else
		{
			if(strcspn(systemnameList,",")<strlen(systemnameList))
			{
				sysID=RetrieveSysIDList(systemnameList,sysIDlist);

				if(sysID==-1)
				{
					GetErrorMess(appliName, "systemnameList is null",ErrMess,1);
					return -1;
				}
			}
			else
			{
				sysID=GetSubsystemID(systemnameList);
			}
		}
		//std :://cout<<"value of sysIDlist "<<sysID<<std::endl;
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
			if(bidi_link==0 || bidi_link==1)
			{
				if(bidi_link==0)
					sprintf(selectconn,"select distinct g.deviceid from %s t, %s g, %s m,%s n,%s l where g.portid=t.portidfrom  and m.deviceid=g.deviceid and mod(n.system_name,l.systemid)=0 and mod(m.system_name,l.systemid)=0 and n.devicename=:devname",MICRO_CONNECTIVITY_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,SUBSYSTEM_TABLE);
				else
					sprintf(selectconn,"select distinct g.deviceid from %s t, %s g, %s m,%s n,%s l where  g.portid=t.portidto and m.deviceid=g.deviceid and mod(n.system_name,l.systemid)=0 and mod(m.system_name,l.systemid)=0 and n.devicename=:devname",MICRO_CONNECTIVITY_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,SUBSYSTEM_TABLE);
			}
			else
			{
				if(bidi_link==2)  //case where the devfrom is really the from and the devto is really the to  as it has been saved in the connectivity
				{
					if(sysID==1)
						sprintf(selectconn,"select distinct g.deviceid from %s t, %s g, %s m where  g.portid=t.portidfrom   and m.deviceid=g.deviceid ",MICRO_CONNECTIVITY_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE);
					else
					{
						if(sysID==0)
							sprintf(selectconn,"select distinct g.deviceid from %s t, %s g, %s m,%s l where  g.portid=t.portidfrom  and m.deviceid=g.deviceid and  mod(m.system_name,l.systemid)=0 and l.systemid in (%s)",MICRO_CONNECTIVITY_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE,SUBSYSTEM_TABLE,sysIDlist);
						else
							sprintf(selectconn,"select distinct g.deviceid from %s t, %s g, %s m  where g.portid=t.portidfrom  and m.deviceid=g.deviceid and  mod(m.system_name,%d)=0 and t.cpntidto=-1",MICRO_CONNECTIVITY_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE,sysID);
					}
				}
				else
				{
					if(sysID==1)
						sprintf(selectconn,"select distinct g.deviceid from %s t, %s g, %s m where g.portid=t.portidto and  m.deviceid=g.deviceid ",MICRO_CONNECTIVITY_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE);
					else
					{
						if(sysID==0)
							sprintf(selectconn,"select distinct g.deviceid from %s t,%s g, %s m,%s l where g.portid=t.portidto and m.deviceid=g.deviceid and  mod(m.system_name,l.systemid)=0 and l.systemid in (%s)",MICRO_CONNECTIVITY_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE,SUBSYSTEM_TABLE,sysIDlist);
						else
							sprintf(selectconn,"select distinct g.deviceid from %s t, %s g, %s m where g.portid=t.portidto and m.deviceid=g.deviceid and  mod(m.system_name,%d)=0 and t.cpntidto=-1",MICRO_CONNECTIVITY_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE,sysID);
					}

				}


			}
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectconn,(ub4) strlen(selectconn),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}
		////printf("Internal Conn: %s %s\n",selectconn,dfrom);
		//std :://cout<<"value of sysIDlist "<<selectconn<<std::endl;
		if(bidi_link==0 ||bidi_link==1)
		{
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
			{	
				status =OCIBindByName(stmthp, &bndp[0], ociError,(text*) ":devname", -1,(dvoid*)dfrom, strlen(dfrom)+1,  SQLT_STR, (dvoid *)0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			}
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBind unsuccessful");
		}
		else
			status=OCIAttrSet (stmthp,OCI_HTYPE_STMT,&prefetch_rows,0,OCI_ATTR_PREFETCH_ROWS,ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtAttribute unsuccessful");
		}
		else
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);




		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[0], ociError,1,&linkid,sizeof(linkid), SQLT_INT, 0, 0, 0, OCI_DEFAULT);





		//std :://cout<<"after define " <<std::endl;
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
		{
			i=0;
			round_trip=1;

			nfrom_list_temp=(int*)realloc(nfrom_list_temp,prefetch_rows*sizeof(int));

			if( nfrom_list_temp==NULL )
			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName, "Malloc unsuccessful 6",ErrMess,1);
				if(nfrom_list_temp!=NULL)
					free(nfrom_list_temp);
				return -1;

			}

		}
		//std :://cout<<"before strcpy all " <<std::endl;

		//std :://cout<<"before the loop " <<std::endl;
		while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
		{
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);
			if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				nfrom_list_temp[i]=linkid;
				i++;
				if(i>0 && (i%prefetch_rows)==0)
				{
					round_trip++;
					nfrom_list_temp=(int*)realloc(nfrom_list_temp,round_trip*prefetch_rows*sizeof(int));
					if( nfrom_list_temp==NULL)
					{
						rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName, "Malloc unsuccessful 10",ErrMess,1);
						if(nfrom_list_temp!=NULL)
							free(nfrom_list_temp);
						return -1;
					}
				}
			}
		}
		//std :://cout<<"after looping i=" <<i<<std::endl;
		if(status!=OCI_SUCCESS && status != OCI_SUCCESS_WITH_INFO)
		{
			if(rescode==0 && i==0)
			{
				rescode=ShowErrors (status, ociError, "No internal links");
				if(nfrom_list_temp!=NULL)
					free(nfrom_list_temp);
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName, "NO_ROWS_SELECTED",ErrMess,1);
				return -1; //error here we don't treat 

			}
		}
		//std:://cout<<"value of _nfrom_name_temp="<<nfrom_list_temp1<<std::endl;
		//std:://cout<<"before copying in the array " <<std::endl;
		if(rescode==0)
		{
			if(getallpaths==0)
			{
				if(bidi_link==0)
				{
					max_rows_internal=i;	
					len_array_gv_internal=i;
					node_from_list_internal=(int*)malloc(len_array_gv_internal*sizeof(int));
					if(node_from_list_internal==NULL)
					{
						if(node_from_list_internal!=NULL)
							free(node_from_list_internal);
						if(nfrom_list_temp!=NULL)
							free(nfrom_list_temp);
						len_array_gv_internal=0;
						rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
						return -1;
					}
					for(j=0;j<i;j++)
					{
						node_from_list_internal[j]=nfrom_list_temp[j];
					}	
				}
				else
				{
					if(bidi_link==1)
					{
						max_rows_scd_internal=i;	
						len_array_gv_scd_internal=i;
						node_from_list_scd_internal=(int*)malloc(len_array_gv_scd_internal*sizeof(int));
						if(node_from_list_scd_internal==NULL)
						{
							if(node_from_list_scd_internal!=NULL)
								free(node_from_list_scd_internal);
							if(nfrom_list_temp!=NULL)
								free(nfrom_list_temp);
							rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
							status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
							GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
							return -1;
						}
						for(j=0;j<i;j++)
						{
							node_from_list_scd_internal[j]=nfrom_list_temp[j];
						}
					}


				}
			}
			else
			{
				if(bidi_link==2)
				{
					max_rows_bis_internal=i;	

					len_array_gv_bis_internal=i;


					node_from_list_bis_internal=(int*)malloc(len_array_gv_bis_internal*sizeof(int));
					if(node_from_list_bis_internal==NULL)
					{
						if(node_from_list_bis_internal!=NULL)
							free(node_from_list_bis_internal);


						if(nfrom_list_temp!=NULL)
							free(nfrom_list_temp);


						len_array_gv_bis_internal=0;

						rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName, "Malloc unsuccessful 8",ErrMess,1);
						return -1;
					}
					for(j=0;j<i;j++)
					{
						node_from_list_bis_internal[j]=nfrom_list_temp[j];
					}	

				}
				else
				{
					max_rows_bis_rev_internal=i;	

					len_array_gv_bis_rev_internal=i;


					node_from_list_bis_rev_internal=(int*)malloc(len_array_gv_bis_rev_internal*sizeof(int));
					if(node_from_list_bis_rev_internal==NULL)
					{
						if(node_from_list_bis_rev_internal!=NULL)
							free(node_from_list_bis_rev_internal);

						if(nfrom_list_temp!=NULL)
							free(nfrom_list_temp);


						len_array_gv_bis_rev_internal=0;

						rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName, "Malloc unsuccessful 8",ErrMess,1);
						return -1;
					}
					for(j=0;j<i;j++)
					{
						node_from_list_bis_rev_internal[j]=nfrom_list_temp[j];


					}	

				}
			}
		}


		if(nfrom_list_temp!=NULL)
			free(nfrom_list_temp);


		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 
		else
			OCIReportError(ociError,appliName, ErrMess,0); 

		return rescode;
	}
	/*****************************************************************************/
	int DecomposePort(char* PortInfo,int PortInfo_len, char* port_nb,char* port_type)
	{
		int pos1=0;
		int pos2=0;
		pos1=strcspn(PortInfo,"|");
		if(pos1>0)
		{
			memcpy(port_nb,PortInfo,pos1+1);
			pos2=PortInfo_len-pos1-1;
			port_nb[pos1]='\0';
			memcpy(port_type,PortInfo+pos1+1,pos2+1);
			port_type[pos2]='\0';
			return 0;
		}
		else
			return -1;

	}
	/****************************************************************************/

	//fct which returns the indexes of the elt
	int GetIndexForTabINT(int * element_List,int elt_List_len, int element_searched, int* elt_idx, int& elt_idx_len, int increment_val)
	{
		int i=0;
		int res=-1;
		int stop=1;
		int j=0;
		////printf ("In GetIndexForTabINT: searching for element %d \n",element_searched);
		while(i<elt_List_len)
		{
			if(element_List[i]==element_searched)
			{
				res=i;
				elt_idx[j]=i;
				j++;

			}

			i+=increment_val;
		}
		elt_idx_len=j;
		if(j>0)
			res=0;
		return res;
	}

	/****************************************************************************/

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


	int CheckInternalConnectivity(int mboardportid_from, int mboardportid_to,char* ErrMess)
	{
		char appliName[100]="CheckInternalConnectivity";
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
		int len11=101;

		int pathid=-1;
		int round_trip=1;
		int nfromlen=0;
		int ntolen=0;
		int link_pos=0;

		int mboard=mboardportid_from;
		//int mbpid_to=-mboardportid_to;
		//int mbpid_from=-mboardportid_from;

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
			sprintf(selectdevtype,"BEGIN %s(:mboard,1,:rescode,:cpntid); if :rescode=0 then :resquery:=%s(:portidfrom); else :resquery:=-1; end if; END;",_CreateMicroLgLinks,_CreateMicroPaths_Host);
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
				rescode=ShowErrors (status, ociError, "OCIBindByName2 unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[1], ociError,(text*)":rescode",-1,(dvoid*)&rescode_pl,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName3 unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[2], ociError,(text*)":cpntid",-1,(dvoid*)&cpntid,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName4 unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[3], ociError,(text*)":resquery",-1,(dvoid*)&resquery,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName6 unsuccessful");
		}
		else
			status=OCIBindByName(stmthp, &bnd1p[4], ociError,(text*)":portidfrom",-1,(dvoid*)&mboardportid_from,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

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

						status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						return -1;
					}

				}
			}
			else
			{
				//        sprintf(selectdevtype,"select pathid from %s d where path_weight=3 and node11=-:pto ",MICRODETPATH_TEMP);
				sprintf(selectdevtype,"select pathid from %s where path_weight=3 and node11=-:pto ",MICRODETPATH_TEMP);
				////printf("select statement: %s\n        PortID = %d\n",selectdevtype,mboardportid_to);
				status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevtype,(ub4) strlen(selectdevtype),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
				//std:://cout <<"value of selectdevtype "<<selectdevtype <<std::endl;
				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCIBindByName6 unsuccessful");
				}
				else
					status=OCIBindByName(stmthp, &bnd1p[0], ociError,(text*)":pto",-1,(dvoid*)&mboardportid_to,sizeof(int), SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)
						rescode=ShowErrors (status, ociError, "OCIAttrSet  unsuccessful");
				}
				else
					status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

				status=OCIDefineByPos (stmthp, &def[0], ociError,1,&pathid,sizeof(pathid), SQLT_INT, 0, 0, 0, OCI_DEFAULT);
				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
				}
				else
				{
					status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);
					if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
					{
						//std:://cout <<"value of pathid 1"<<pathid <<std::endl;
						if(pathid>0)
							rescode=0;
						else
							rescode=-1;

						status=OCITransCommit(ociHdbc, ociError, 0);
					}
					else
					{
						rescode=-1;
						//std:://cout <<"value of pathid 2"<<pathid <<std::endl;
						if(status!=OCI_SUCCESS && status != OCI_SUCCESS_WITH_INFO )
						{
							rescode=-1;
							status=OCITransCommit(ociHdbc, ociError, 0);
							GetErrorMess(appliName, "NO_ROWS_SELECTED",ErrMess,1);
							status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);
							return -1;
						}
						else
							rescode=-1;
					}

				}
			}
		}



		status=OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 
		else
			OCIReportError(ociError,appliName, ErrMess,0); 


		return (rescode+status);
	}


	//fct which returns the indexes of the elt
	int GetIndexForTabCHAR_second(char**  element_List,int elt_List_nb, char* element_searched, int* elt_idx, int& elt_idx_len)
	{
		int i=0;
		int res=-1;
		int stop=1;
		int pos2=2;
		int pos1=0;
		int j=0;
		int elt_searched_len=strlen(element_searched);
		//std:://cout<<"elt_searched_len="<<elt_searched_len<<" and elt_List_nb="<<elt_List_nb<<" and element_searched="<<element_searched<<std::endl;
		////printf ("In GetIndexForTabCHAR: searching for element %s \n",element_searched);
		while(i<elt_List_nb)
		{
			if(strncmp(element_List[i],element_searched,elt_searched_len)==0)
			{
				elt_idx[j]=i;
				j++;
				//std:://cout<<"found matching element element_List[i]="<<element_List[i]<<" and i="<<i<<std::endl; 
			}

			i++;
		}
		elt_idx_len=j;
		if(j>0)
		{
			res=0;
			//std:://cout<<"j greater than 0"<<std::endl;
		}
		return res;
	}


	void CopyMatrix(int * path_matrix_new,int * path_matrix_old,int path_matrix_old_len,int j,int i,int elt_idx_temp_len, int nb_of_hop)
	{
		int k,l,m;
		for(k=0;k<i;k++)
		{
			for(l=0;l<j;l++)
				path_matrix_new[k*nb_of_hop+l]=path_matrix_old[k*nb_of_hop+l];

		}
		m=i+elt_idx_temp_len;
		for(k=i;k<m;k++)
		{
			for(l=0;l<j;l++)
				path_matrix_new[k*nb_of_hop+l]=path_matrix_old[i*nb_of_hop+l];
		}
		for(k=i;k<path_matrix_old_len;k++)
		{
			m=k+elt_idx_temp_len;
			for(l=0;l<j;l++)
				path_matrix_new[m*nb_of_hop+l]=path_matrix_old[k*nb_of_hop+l];
		}

	}
	void CopyVector(int * path_vector_new,int *path_vector_old,int path_vector_old_len,int i,int elt_idx_temp_len)
	{
		int k,m;
		for(k=0;k<i;k++)
		{
			path_vector_new[k]=path_vector_old[k];
		}
		m=i+elt_idx_temp_len;
		for(k=i;k<m;k++)
		{
			path_vector_new[k]=path_vector_old[i];
		}
		for(k=i;k<path_vector_old_len;k++)
		{
			m=k+elt_idx_temp_len;
			path_vector_new[m]=path_vector_old[k];
		}


	}

	int GetElementAt(char * elt_vector,int index,char* separator,char* elt_value)
	{

		int nb_of_elt=0;
		int pos1;
		int pos2=2;
		int rescode=-1;
		while(nb_of_elt<index && nb_of_elt!=-1)
		{
			pos1=strcspn(elt_vector+pos2,separator);
			if(pos1!=strlen((elt_vector+pos2)))
			{
				pos2+=pos1+1;
				nb_of_elt++;
			}
			else
				nb_of_elt=-1;

		}
		if(nb_of_elt!=-1)
		{
			pos1=strcspn(elt_vector+pos2,separator);
			if(pos1!=strlen((elt_vector+pos2)))
			{
				memcpy(elt_value,elt_vector+pos2,pos1);
				elt_value[pos1]='\0';
				rescode=0;
			}

		}

		return  rescode;
	}


	int ComputePathWeight(int *path_end_new, int path_end_len)
	{
		int i=0;
		int stop=1;
		for(i=0;i<path_end_len;i++)
		{
			if(path_end_new[i]==0)
				stop=0;
		}
		return stop;
	}


	int GetDetailedConnectivityBetweenDeviceDeviceType_Cache(char appliName[100],char* systemnameList,char* dev_from, char* dev_to,int devtype_given, int& len_array, int &pfrom1_arraylen,int &nfrom1_arraylen,int &pto1_arraylen,int &nto1_arraylen,int &lkinfo1_arraylen,char* nfrom1_list,char* nto1_list,char* pfrom1_list,int* pfrom_list,char* pto1_list,int* pto_list,int* pathid_list,int* link_pos_list,char* lkinfo_list,int reload_connectivity,int delete_connectivity,char* ErrMessage)
	{
		//char appliName[100]="GetDetailedConnectivityBetweenDeviceDeviceTypes_Cache";
		int elt_value=0;
		int elt_idx_len=1000;
		int elt_idx_temp_len=1000;
		int* elt_idx=NULL;
		int* elt_idx_temp=NULL;
		int rescode=0;
		int err_len=1000;
		clock_t start,finish;
		double time;
		int nb_of_hop=23;
		int rescode1=0;
		int rescode_internal=0;
		int deviceid_to=0;
		int len_array_cpy=30000;
		int *path_matrix_old=NULL;
		int *path_matrix_new=NULL;
		int* path_matrix_temp=NULL;
		int path_matrix_old_len=0;
		int path_matrix_new_len=0;
		int *path_end_old=NULL;
		int *path_end_new=NULL;
		int* path_valid=NULL;
		int* path_type=NULL;
		int* path_type_new=NULL;
		char* nfrom1_list_temp=NULL;
		char* nto1_list_temp=NULL;
		char* pfrom1_list_temp=NULL;
		int* pfrom_list_temp=NULL;
		char* pto1_list_temp=NULL;
		char* lkinfo_list_temp=NULL;
		int* pto_list_temp=NULL;
		int* pathid_list_temp=NULL;
		int* link_pos_list_temp=NULL;
		int pos5=0;
		int dev_len=200;
		int valid_path=0;
		int i=0;
		int j=0;
		int k=0;
		int v=0;
		int l=0;
		int elt_value_temp1=0;
		int elt_value_temp2=0;
		int stop=0;
		int nb_of_line=0;
		int* elt_idx_temp1=NULL;
		int elt_idx_temp1_len=1000;
		int m=0;
		int lk_pos=1;
		char node_name_val[200];
		int pos1,pos2,pos3,pos4;
		int p=0;
		int dto_len=strlen(dev_to);

		int _len_array_devto=20000;
		int* _devicetoList_possible=NULL;
		int elt_idx_int_temp1[100];
		int elt_idx_int_temp1_len=100;

		char ErrMess_temp[1000];
		int nb_of_line1=0;

		int linkid_int=0;
		int pfrom_int=0;
		int pto_int=0;
		char ptnb_from[20];
		char ptype_from[50];
		char ptnb_to[20];
		char ptype_to[50];
		int port_idx=0;
		int next_step=0;
		//char portfrom_info[100];
		//char portto_info[100];
		////printf("GetDetailedConnectivityBetweenDeviceDeviceTypes_Cache: %s %s \n",dev_from,dev_to );
		if(devtype_given==1)
		{
			_devicetoList_possible=(int*)malloc(_len_array_devto*sizeof(int));
			if(_devicetoList_possible==NULL)
			{

				GetErrorMess(appliName,"Unsuccessful malloc 1",ErrMessage,1);
				return -1;
			}
			else
			{

				rescode=GetDeviceIDsPerType(dev_to, _len_array_devto, _devicetoList_possible,ErrMess_temp);
			}
			if(rescode!=0)
			{

				free(_devicetoList_possible);
				GetErrorMess(appliName,ErrMess_temp,ErrMessage,1);
				return -1;
			}
		}	
		//std:://cout<<"before starting"<<std::endl;


		if(reload_connectivity==1 || lkid_list==NULL)
		{
			//std:://cout<<"loading connectivity table reload_connectivity="<<reload_connectivity<<std::endl;
			if(devtype_given==1 ||devtype_given==0)
			{

				rescode=LoadConnectivityTable_InCache( systemnameList,dev_from,"none",BIDIRECTIONAL_WAY_USED,0,ErrMess_temp);
				rescode_internal=LoadInternalConnectivity_InCache( systemnameList,dev_from,"none",BIDIRECTIONAL_WAY_USED,0,ErrMess_temp);
			}
			else
			{
				if(devtype_given==2)
				{
					rescode=LoadConnectivityTable_InCache( systemnameList,dev_from,dev_to,BIDIRECTIONAL_WAY_USED, 0,ErrMess_temp);
					rescode_internal=LoadInternalConnectivity_InCache( systemnameList,dev_from,dev_to,BIDIRECTIONAL_WAY_USED, 0,ErrMess_temp);

				}
			}

			//std:://cout<<"after LoadConnectivityTable_InCache and rescode="<<rescode<<"_len_array_gv="<<_len_array_gv<< std::endl;
			if(rescode!=0)
			{

				for(p=0;p<max_rows;p++)
				{
					free(pto_array_list[p]);
					free(nto_array_list[p]);
					free(pfrom_array_list[p]);
					free(nfrom_array_list[p]);
					free(lkinfo_array_list[p]);

				}
				free(pto_array_list);
				free(nto_array_list);
				free(pfrom_array_list);
				free(nfrom_array_list);
				free(lkinfo_array_list);
				if(lkid_list!=NULL)
					free(lkid_list);
				if(node_from_list!=NULL)
					free(node_from_list);
				if(node_to_list!=NULL)
					free(node_to_list);
				if(bidirectional_list!=NULL)
					free(bidirectional_list);
				if(lk_weight_list!=NULL)
					free(lk_weight_list);
				if(lktype_list!=NULL)
					free(lktype_list);
				if(lkused_list!=NULL)
					free(lkused_list);
				GetErrorMess(appliName,ErrMess_temp,ErrMessage,1);
				return -1;
			}
		}
		else
		{
			rescode=0;
		}	
		//std:://cout<<"Connectivity loaded"<<std::endl;
		elt_idx=(int*)malloc(elt_idx_len*sizeof(int));
		elt_idx_temp=(int*)malloc(elt_idx_temp_len*sizeof(int));

		if(elt_idx==NULL || elt_idx_temp==NULL)
		{
			for(p=0;p<max_rows;p++)
			{
				free(pto_array_list[p]);
				free(nto_array_list[p]);
				free(pfrom_array_list[p]);
				free(nfrom_array_list[p]);
				free(lkinfo_array_list[p]);

			}
			free(pto_array_list);
			free(nto_array_list);
			free(pfrom_array_list);
			free(nfrom_array_list);
			free(lkinfo_array_list);
			if(lkid_list!=NULL)
				free(lkid_list);
			if(node_from_list!=NULL)
				free(node_from_list);
			if(node_to_list!=NULL)
				free(node_to_list);

			if(bidirectional_list!=NULL)
				free(bidirectional_list);
			if(lkused_list!=NULL)
				free(lkused_list);
			if(lktype_list!=NULL)
				free(lktype_list);
			if(elt_idx!=NULL)
				free(elt_idx);
			if(elt_idx_temp!=NULL)
				free(elt_idx_temp);

			GetErrorMess(appliName,"Unsuccessful malloc",ErrMessage,1);
			return -1;
		}
		i=0;
		int deviceid_from=0;
		//std:://cout<<"before getting the tabCHAR"<<std::endl;
		rescode=GetIndexForTabCHAR_second(nfrom_array_list,max_rows, dev_from, elt_idx,elt_idx_len);

		//std:://cout<<"max_rows="<<max_rows<<" and dev_from="<<dev_from<<std::endl;
		if(rescode==-1)
		{
			if(delete_connectivity==1)
			{
				for(p=0;p<max_rows;p++)
				{
					free(pto_array_list[p]);
					free(nto_array_list[p]);
					free(pfrom_array_list[p]);
					free(nfrom_array_list[p]);
					free(lkinfo_array_list[p]);

				}
				free(pto_array_list);
				free(nto_array_list);
				free(pfrom_array_list);
				free(nfrom_array_list);
				free(lkinfo_array_list);
				if(lkid_list!=NULL)
					free(lkid_list);
				if(node_from_list!=NULL)
					free(node_from_list);
				if(node_to_list!=NULL)
					free(node_to_list);

				if(bidirectional_list!=NULL)
					free(bidirectional_list);
				if(lkused_list!=NULL)
					free(lkused_list);
				if(lktype_list!=NULL)
					free(lktype_list);
			}
			free(elt_idx);
			free(elt_idx_temp);
			GetErrorMess(appliName,"DEVICE NOT CONNECTED",ErrMessage,1);
			BIDIRECTIONAL_WAY_USED=1;
			return -1;
		}
		else
		{
			i=elt_idx[0];
			deviceid_from=node_from_list[i];
		}
		if(devtype_given==2 ||devtype_given==0)
		{
			rescode=GetIndexForTabCHAR_second(nto_array_list,max_rows, dev_to, elt_idx,elt_idx_len);
			if(rescode!=0)
			{
				if(delete_connectivity==1)
				{
					for(p=0;p<max_rows;p++)
					{
						free(pto_array_list[p]);
						free(nto_array_list[p]);
						free(pfrom_array_list[p]);
						free(nfrom_array_list[p]);
						free(lkinfo_array_list[p]);

					}
					free(pto_array_list);
					free(nto_array_list);
					free(pfrom_array_list);
					free(nfrom_array_list);
					free(lkinfo_array_list);
					if(lkid_list!=NULL)
						free(lkid_list);
					if(node_from_list!=NULL)
						free(node_from_list);
					if(node_to_list!=NULL)
						free(node_to_list);

					if(bidirectional_list!=NULL)
						free(bidirectional_list);
					if(lkused_list!=NULL)
						free(lkused_list);
					if(lktype_list!=NULL)
						free(lktype_list);
				}
				free(elt_idx);
				free(elt_idx_temp);
				GetErrorMess(appliName,"DEVICE NOT CONNECTED",ErrMessage,1);
				BIDIRECTIONAL_WAY_USED=1;
				return -1;

			}
			else
			{
				if(devtype_given==2)
				{
					i=elt_idx[0];
					deviceid_to=node_to_list[i];
				}
				else
				{
					_devicetoList_possible=(int*)malloc(_len_array_devto*sizeof(int));
					if(_devicetoList_possible==NULL)
					{
						GetErrorMess(appliName,"Unsuccessful malloc 1",ErrMessage,1);
						return -1;
					}
					else
					{
						for(i=0;i<elt_idx_len;i++)
						{
							j=elt_idx[i];
							_devicetoList_possible[i]=node_to_list[j];
						}
						_len_array_devto=elt_idx_len;
					}
				}
			}
		}
		//rescode=GetIndexForTabINT(node_from_list_internal,len_array_gv_internal, deviceid_from, elt_idx,elt_idx_len,1);
		//if(elt_idx_len==0)
		rescode=GetIndexForTabINT(node_from_list,len_array_gv, deviceid_from, elt_idx,elt_idx_len,1);

		path_matrix_old=(int*)realloc(path_matrix_old,elt_idx_len*nb_of_hop*sizeof(int));
		path_end_new=(int*)realloc(path_end_new,elt_idx_len*sizeof(int));
		path_end_old=(int*)realloc(path_end_old,elt_idx_len*sizeof(int));
		path_type=(int*)realloc(path_type,elt_idx_len*sizeof(int));
		path_type_new=(int*)realloc(path_type_new,elt_idx_len*sizeof(int));

		path_matrix_temp=(int*)malloc((nb_of_hop+1)*sizeof(int));
		elt_idx_temp1=(int*)malloc(elt_idx_temp_len*sizeof(int));
		if(elt_idx_temp1==NULL||path_matrix_temp==NULL||path_end_old==NULL||path_end_new==NULL||path_type==NULL||path_type_new==NULL)
		{
			free(elt_idx_temp);
			free(elt_idx);
			if(elt_idx_temp1!=NULL)
				free(elt_idx_temp1);
			if(path_matrix_temp!=NULL)
				free(path_matrix_temp);
			if(path_end_old!=NULL)
				free(path_end_old);
			if(path_end_new!=NULL)
				free(path_end_new);
			if(path_type!=NULL)
				free(path_type);
			if(path_type_new!=NULL)
				free(path_type_new);
			if(path_matrix_old!=NULL )
			{
				free(path_matrix_old);
			}
			if(lkid_list!=NULL)
				free(lkid_list);
			if(node_from_list!=NULL)
				free(node_from_list);
			if(node_to_list!=NULL)
				free(node_to_list);
			if(bidirectional_list!=NULL)
				free(bidirectional_list);
			if(lk_weight_list!=NULL)
				free(lk_weight_list);
			if(lktype_list!=NULL)
				free(lktype_list);
			if(lkused_list!=NULL)
				free(lkused_list);
			for(p=0;p<max_rows;p++)
			{
				free(pto_array_list[p]);
				free(nto_array_list[p]);
				free(pfrom_array_list[p]);
				free(nfrom_array_list[p]);
				free(lkinfo_array_list[p]);

			}
			free(pto_array_list);
			free(nto_array_list);
			free(pfrom_array_list);
			free(nfrom_array_list);
			free(lkinfo_array_list);
			GetErrorMess(appliName,"Unsuccessful malloc",ErrMessage,1);
			return -1;

		}
		nb_of_line=3;
		for(i=0;i<elt_idx_len;i++)
		{
			for(j=0;j<nb_of_line;j++)
			{
				m=elt_idx[i];
				path_matrix_old[i*nb_of_hop+j]=node_from_list[m];
				j++;
				path_matrix_old[i*nb_of_hop+j]=lkid_list[m];
				j++;
				path_matrix_old[i*nb_of_hop+j]=node_to_list[m];
				if(devtype_given==1 ||devtype_given==0)
				{
					rescode=GetIndexForTabINT(_devicetoList_possible,_len_array_devto,node_to_list[m] , elt_idx_temp1,elt_idx_temp1_len,1);
					if(elt_idx_temp1_len>0 && rescode==0)
						path_end_old[i]=1;
					else
						path_end_old[i]=0;
					path_type[i]=lktype_list[m];
				}
				else
				{
					path_type[i]=lktype_list[m];
					if(node_to_list[m]==deviceid_to)
					{
						path_end_old[i]=1;
						//std:://cout<<"case 1 deviceid_to="<<deviceid_to<<std::endl;
					}
					else
					{
						path_end_old[i]=0;
						//std:://cout<<"case 2 deviceid_to="<<deviceid_to<<std::endl;

					}
				}
			}
		}
		path_matrix_old_len=elt_idx_len;
		stop=ComputePathWeight(path_end_old,path_matrix_old_len);
		i=0;
		j=3;
		nb_of_line=0;
		path_matrix_new_len=0;
		pos1=0;
		pos2=0;
		pos3=0;
		pos4=0;
		pos5=0;
		//std:://cout<<"before getting in the loop"<<std::endl;
		start = clock();
		while(stop==0 && j<nb_of_hop)
		{
			for(i=0;i<path_matrix_old_len;i++)
			{

				if(path_end_old[i]==0)
				{

					elt_value=path_matrix_old[i*nb_of_hop+(j-1)];

					rescode=GetIndexForTabINT(node_from_list,len_array_gv,elt_value , elt_idx_temp,elt_idx_temp_len,1);
					if(elt_idx_temp_len==0)
					{
						if(elt_idx_temp==0)
							path_end_old[i]=-1;
					}
					else
					{
						path_matrix_new_len+=elt_idx_temp_len; //nb of paths so far
						path_matrix_new=(int*)realloc(path_matrix_new,path_matrix_new_len*nb_of_hop*sizeof(int));
						path_end_new=(int*)realloc(path_end_new,path_matrix_new_len*sizeof(int));
						path_type_new=(int*)realloc(path_type_new,path_matrix_new_len*sizeof(int));
						if(path_matrix_new==NULL || path_end_new==NULL||path_type_new==NULL)
						{
							if(path_matrix_old!=NULL)
								path_matrix_old=(int*)realloc(path_matrix_old,0*sizeof(int));
							if(path_matrix_new!=NULL)
								path_matrix_new=(int*)realloc(path_matrix_new,0*sizeof(int));
							if(path_end_new!=NULL)
								path_end_new=(int*)realloc(path_end_new,0*sizeof(int));
							if(path_end_old!=NULL)
								path_end_old=(int*)realloc(path_end_old,0*sizeof(int));
							free(elt_idx_temp);
							free(path_matrix_temp);
							if(elt_idx_temp1!=NULL)
								free(elt_idx_temp1);
							if(path_valid!=NULL )
								realloc(path_valid,0*sizeof(int));
							free(elt_idx);
							if(lkid_list!=NULL)
								free(lkid_list);
							if(node_from_list!=NULL)
								free(node_from_list);
							if(node_to_list!=NULL)
								free(node_to_list);
							if(bidirectional_list!=NULL)
								free(bidirectional_list);
							if(lk_weight_list!=NULL)
								free(lk_weight_list);
							if(lktype_list!=NULL)
								free(lktype_list);
							if(lkused_list!=NULL)
								free(lkused_list);
							if(path_type!=NULL)
								free(path_type);
							if(path_type_new!=NULL)
								free(path_type_new);
							for(p=0;p<max_rows;p++)
							{
								free(pto_array_list[p]);
								free(nto_array_list[p]);
								free(pfrom_array_list[p]);
								free(nfrom_array_list[p]);
								free(lkinfo_array_list[p]);

							}
							free(pto_array_list);
							free(nto_array_list);
							free(pfrom_array_list);
							free(nfrom_array_list);
							free(lkinfo_array_list);
							if(link_pos_list_temp!=NULL)
								link_pos_list_temp=(int*)realloc(link_pos_list_temp,0*sizeof(int));
							if(lkinfo_list_temp!=NULL)
								lkinfo_list_temp=(char*)realloc(lkinfo_list_temp,0*sizeof(char));
							if(pathid_list_temp!=NULL)
								pathid_list_temp=(int*)realloc(pathid_list_temp,0*sizeof(int));
							if(nfrom1_list_temp!=NULL)
								free(nfrom1_list_temp);
							if(nto1_list_temp!=NULL)
								free(nto1_list_temp);
							if(pfrom1_list_temp!=NULL)
								free(pfrom1_list_temp);
							if(pfrom_list_temp!=NULL)
								free(pfrom_list_temp);
							if(pto1_list_temp!=NULL)
								free(pto1_list_temp);
							if(pto_list_temp!=NULL)
								free(pto_list_temp);
							GetErrorMess(appliName,"Unsuccessful malloc",ErrMessage,1);
							return -1;
						}
						else
						{
							for(l=0;l<j;l++)//copy of the corresponding path line to check loop
							{
								path_matrix_temp[l]=path_matrix_old[i*nb_of_hop+l];

							}

						}

						for(k=0;k<elt_idx_temp_len;k++)
						{
							elt_value_temp1=elt_idx_temp[k];
							elt_value_temp2=node_to_list[elt_value_temp1];
							if(path_type[i]%lktype_list[elt_value_temp1]==0 || lktype_list[elt_value_temp1]%path_type[i]==0)
							{
								rescode=GetIndexForTabINT(path_matrix_temp,j,elt_value_temp2 , elt_idx_temp1,elt_idx_temp1_len,2);
								if(rescode==-1 ||elt_idx_temp1_len==0)
								{
									finish=clock();
									time = (double(finish)-double(start))/CLOCKS_PER_SEC;  
									if(time>TIMEOUT_DEFINED)
									{
										if(path_matrix_old!=NULL)
											path_matrix_old=(int*)realloc(path_matrix_old,0*sizeof(int));
										if(path_matrix_new!=NULL)
											path_matrix_new=(int*)realloc(path_matrix_new,0*sizeof(int));
										if(path_end_new!=NULL)
											path_end_new=(int*)realloc(path_end_new,0*sizeof(int));
										if(path_end_old!=NULL)
											path_end_old=(int*)realloc(path_end_old,0*sizeof(int));
										free(elt_idx_temp);
										free(elt_idx);
										free(path_matrix_temp);
										if(elt_idx_temp1!=NULL)
											free(elt_idx_temp1);
										if(path_valid!=NULL )
											realloc(path_valid,0*sizeof(int));
										if(lkid_list!=NULL)
											free(lkid_list);
										if(node_from_list!=NULL)
											free(node_from_list);
										if(node_to_list!=NULL)
											free(node_to_list);
										if(bidirectional_list!=NULL)
											free(bidirectional_list);
										if(lk_weight_list!=NULL)
											free(lk_weight_list);
										if(lktype_list!=NULL)
											free(lktype_list);
										if(lkused_list!=NULL)
											free(lkused_list);
										if(path_type!=NULL)
											free(path_type);
										for(p=0;p<max_rows;p++)
										{
											free(pto_array_list[p]);
											free(nto_array_list[p]);
											free(pfrom_array_list[p]);
											free(nfrom_array_list[p]);
											free(lkinfo_array_list[p]);

										}
										free(pto_array_list);
										free(nto_array_list);
										free(pfrom_array_list);
										free(nfrom_array_list);
										free(lkinfo_array_list);
										if(link_pos_list_temp!=NULL)
											link_pos_list_temp=(int*)realloc(link_pos_list_temp,0*sizeof(int));
										if(lkinfo_list_temp!=NULL)
											lkinfo_list_temp=(char*)realloc(lkinfo_list_temp,0*sizeof(char));
										if(pathid_list_temp!=NULL)
											pathid_list_temp=(int*)realloc(pathid_list_temp,0*sizeof(int));
										if(nfrom1_list_temp!=NULL)
											free(nfrom1_list_temp);
										if(nto1_list_temp!=NULL)
											free(nto1_list_temp);
										if(pfrom1_list_temp!=NULL)
											free(pfrom1_list_temp);
										if(pfrom_list_temp!=NULL)
											free(pfrom_list_temp);
										if(pto1_list_temp!=NULL)
											free(pto1_list_temp);
										if(pto_list_temp!=NULL)
											free(pto_list_temp);
										GetErrorMess(appliName,"CONNECTIVITY TOO COMPLEX, EXECUTION TIME TOO LONG",ErrMessage,1);
										return -1;
									}
									if(len_array_gv_internal>0)
									{
										//std:://cout<<"elt_value="<<elt_value<<" and len_array_gv_internal="<<len_array_gv_internal<<std::endl;
										rescode=GetIndexForTabINT(node_from_list_internal,len_array_gv_internal,elt_value , elt_idx_int_temp1,elt_idx_int_temp1_len,1);
										if(elt_idx_int_temp1_len>0)
										{
											linkid_int=path_matrix_old[i*nb_of_hop+(j-2)];
											////printf("Link ID = %d\n", linkid_int);
											rescode=GetIndexForTabINT(lkid_list,len_array_gv,linkid_int , elt_idx_int_temp1,elt_idx_int_temp1_len,1);
											port_idx=elt_idx_int_temp1[0];
											//std:://cout<<"linkid_int="<<linkid_int<<" and pto_array_list[port_idx]="<<pto_array_list[port_idx]<<std::endl;

											rescode=DecomposePort(pfrom_array_list[elt_value_temp1],strlen(pfrom_array_list[elt_value_temp1]),  ptnb_from, ptype_from);
											rescode+=DecomposePort(pto_array_list[port_idx],strlen(pto_array_list[port_idx]),  ptnb_to, ptype_to);
											//std:://cout<<"ptnb_to="<<ptnb_to<<" and ptype_to="<<ptype_to<<std::endl;

											if(linkid_int>0)
											{
												rescode+=GetPortID_portinfo(elt_value,ptnb_from,ptype_from,1,pfrom_int,ErrMess_temp);
												rescode+=GetPortID_portinfo(elt_value,ptnb_to,ptype_to,2,pto_int,ErrMess_temp);
												rescode+=CheckInternalConnectivity(pfrom_int, pto_int,ErrMess_temp);
												//std:://cout<<"pfrom_int="<<pfrom_int<<" and pto_int="<<pto_int<<" and rescode="<<rescode<<std::endl;

											}
											else
											{
												rescode+=GetPortID_portinfo(elt_value,ptnb_from,ptype_from,2,pfrom_int,ErrMess_temp);
												rescode+=GetPortID_portinfo(elt_value,ptnb_to,ptype_to,1,pto_int,ErrMess_temp);
												rescode+=CheckInternalConnectivity(pto_int, pfrom_int,ErrMess_temp);

											}	
											if(rescode==0)//then it's OK link allowed
												next_step=0;
											else 
												next_step=-1;
										}
										else
											next_step=0;

									}
									else
										next_step=0;//measn it's OK we go to the next step
									if(next_step==0)
									{
										for(m=0;m<j;m++)
											path_matrix_new[nb_of_line*nb_of_hop+m]=path_matrix_old[i*nb_of_hop+m];	

										path_matrix_new[nb_of_line*nb_of_hop+j]=lkid_list[elt_value_temp1];
										path_matrix_new[nb_of_line*nb_of_hop+j+1]=elt_value_temp2;
										if(devtype_given==1)
										{
											rescode=GetIndexForTabINT(_devicetoList_possible,_len_array_devto,elt_value_temp2 , elt_idx_temp1,elt_idx_temp1_len,1);
											if(elt_idx_temp1_len>0 && rescode==0)
											{
												path_end_new[nb_of_line]=1;
												if(path_type[i]%lktype_list[elt_value_temp1]==0)
													path_type_new[nb_of_line]=lktype_list[elt_value_temp1];
												else
													path_type_new[nb_of_line]=path_type[i];
											}
											else
											{
												if(lk_weight_list[elt_value_temp1]!=2)
												{
													path_end_new[nb_of_line]=0;
													if(path_type[i]%lktype_list[elt_value_temp1]==0)
														path_type_new[nb_of_line]=lktype_list[elt_value_temp1];
													else
														path_type_new[nb_of_line]=path_type[i];
												}
												else
												{
													path_end_new[nb_of_line]=-1;
													path_type_new[nb_of_line]=-1;
												}
											}
										}
										else
										{
											strcpy(node_name_val,nto_array_list[elt_value_temp1]);

											if(strstr(node_name_val,dev_to)!=NULL)
											{
												path_end_new[nb_of_line]=1;
												if(path_type[i]%lktype_list[elt_value_temp1]==0)
													path_type_new[nb_of_line]=lktype_list[elt_value_temp1];
												else
													path_type_new[nb_of_line]=path_type[i];
											}
											else
											{
												if(lk_weight_list[elt_value_temp1]==2)
												{
													path_type_new[nb_of_line]=-1;
													path_end_new[nb_of_line]=-1;
												}
												else
												{
													path_end_new[nb_of_line]=0;
													if(path_type[i]%lktype_list[elt_value_temp1]==0)
														path_type_new[nb_of_line]=lktype_list[elt_value_temp1];
													else
														path_type_new[nb_of_line]=path_type[i];
												}
											}

										}
										nb_of_line++;
									}
									else
									{
										path_end_new[nb_of_line]=-1;
										path_type_new[nb_of_line]=-1;
										nb_of_line++;
									}
								}
								else
								{
									path_end_new[nb_of_line]=-1;
									path_type_new[nb_of_line]=-1;
									nb_of_line++;
								}
							}
						}

					}
				}
				else
				{
					if(path_end_old[i]==1)
					{
						//std:://cout<<"je suis dans path end 1"<<std::endl;
						path_valid=(int*)realloc(path_valid,(valid_path+1)*nb_of_hop*sizeof(int));
						pathid_list_temp=(int*)realloc(pathid_list_temp,(nb_of_line1+j)*sizeof(int));
						link_pos_list_temp=(int*)realloc(link_pos_list_temp,(nb_of_line1+j)*sizeof(int));
						pfrom_list_temp=(int*)realloc(pfrom_list_temp,(nb_of_line1+j)*sizeof(int));
						pto_list_temp=(int*)realloc(pto_list_temp,(nb_of_line1+j)*sizeof(int));
						pfrom1_list_temp=(char*)realloc(pfrom1_list_temp,(nb_of_line1+j)*dev_len*sizeof(char));
						pto1_list_temp=(char*)realloc(pto1_list_temp,(nb_of_line1+j)*dev_len*sizeof(char));
						nfrom1_list_temp=(char*)realloc(nfrom1_list_temp,(nb_of_line1+j)*dev_len*sizeof(char));
						nto1_list_temp=(char*)realloc(nto1_list_temp,(nb_of_line1+j)*dev_len*sizeof(char));
						lkinfo_list_temp=(char*)realloc(lkinfo_list_temp,(nb_of_line1+j)*dev_len*sizeof(char));
						if(lkinfo_list_temp==NULL||nfrom1_list_temp==NULL||pfrom1_list_temp==NULL||nto1_list_temp==NULL||pto1_list_temp==NULL||path_valid==NULL||pathid_list_temp==NULL||link_pos_list_temp==NULL||pfrom_list_temp==NULL||pto_list_temp==NULL)
						{
							if(path_matrix_old!=NULL)
								path_matrix_old=(int*)realloc(path_matrix_old,0*sizeof(int));
							if(path_matrix_new!=NULL)
								path_matrix_new=(int*)realloc(path_matrix_new,0*sizeof(int));
							if(path_valid!=NULL)
								path_valid=(int*)realloc(path_valid,0*sizeof(int));
							if(path_end_new!=NULL)
								path_end_new=(int*)realloc(path_end_new,0*sizeof(int));
							free(elt_idx);
							if(path_end_old!=NULL)
								path_end_old=(int*)realloc(path_end_old,0*sizeof(int));
							free(elt_idx_temp);
							free(path_matrix_temp);
							if(elt_idx_temp1!=NULL)
								free(elt_idx_temp1);
							if(lkid_list!=NULL)
								free(lkid_list);
							if(node_from_list!=NULL)
								free(node_from_list);
							if(node_to_list!=NULL)
								free(node_to_list);
							if(bidirectional_list!=NULL)
								free(bidirectional_list);
							if(lk_weight_list!=NULL)
								free(lk_weight_list);
							if(lktype_list!=NULL)
								free(lktype_list);
							if(lkused_list!=NULL)
								free(lkused_list);
							if(path_type!=NULL)
								free(path_type);
							if(path_type_new!=NULL)
								free(path_type_new);
							for(p=0;p<max_rows;p++)
							{
								free(pto_array_list[p]);
								free(nto_array_list[p]);
								free(pfrom_array_list[p]);
								free(nfrom_array_list[p]);
								free(lkinfo_array_list[p]);

							}
							free(pto_array_list);
							free(nto_array_list);
							free(pfrom_array_list);
							free(nfrom_array_list);
							free(lkinfo_array_list);
							if(link_pos_list_temp!=NULL)
								link_pos_list_temp=(int*)realloc(link_pos_list_temp,0*sizeof(int));
							if(pathid_list_temp!=NULL)
								pathid_list_temp=(int*)realloc(pathid_list_temp,0*sizeof(int));
							if(nfrom1_list_temp!=NULL)
								free(nfrom1_list_temp);
							if(nto1_list_temp!=NULL)
								free(nto1_list_temp);
							if(pfrom1_list_temp!=NULL)
								free(pfrom1_list_temp);
							if(pfrom_list_temp!=NULL)
								free(pfrom_list_temp);
							if(pto1_list_temp!=NULL)
								free(pto1_list_temp);
							if(pto_list_temp!=NULL)
								free(pto_list_temp);
							if(lkinfo_list_temp!=NULL)
								free(lkinfo_list_temp);
							GetErrorMess(appliName,"Unsuccessful malloc",ErrMessage,1);
							return -1;
						}
						lk_pos=1;
						for(m=1;m<j;m++)
						{

							path_valid[valid_path*nb_of_hop+m]=path_matrix_old[i*nb_of_hop+m];
							rescode=GetIndexForTabINT(lkid_list,len_array_gv,path_valid[valid_path*nb_of_hop+m] , elt_idx_temp1,elt_idx_temp1_len,1);
							pathid_list_temp[nb_of_line1]=valid_path;
							link_pos_list_temp[nb_of_line1]=lk_pos;
							elt_value=elt_idx_temp1[0];//necessary one possibility
							if(lkid_list[elt_value]>0)
							{
								pto_list_temp[nb_of_line1]=2;
								pfrom_list_temp[nb_of_line1]=1;
							}
							else
							{
								pto_list_temp[nb_of_line1]=1;
								pfrom_list_temp[nb_of_line1]=2;
							}
							if(rescode==0)
							{
								strcpy(nfrom1_list_temp+pos2,nfrom_array_list[elt_value]);
								//std:://cout<<"value of nfrom_array_list[elt_value]="<<nfrom_array_list[elt_value]<<std::endl;
								//std:://cout<<"value of nfrom1_list_temp"<<nfrom1_list_temp<<std::endl;
								//std:://cout<<"value of nfrom1_list_temp+pos2"<<nfrom1_list_temp+pos2<<std::endl;
								//	f//printf (f, "node_from_name=%s and node_id=%d \n",nfrom1_list+pos2,_node_from_list_gv[elt_value]);
								pos2+=strlen(nfrom_array_list[elt_value])+1;

							}
							if(rescode==0)
							{
								strcpy(nto1_list_temp+pos1,nto_array_list[elt_value]);
								//std:://cout<<"value of nto_array_list[elt_value]="<<nto_array_list[elt_value]<<std::endl;
								//std:://cout<<"value of nto1_list_temp"<<nto1_list_temp<<std::endl;
								pos1+=strlen(nto_array_list[elt_value])+1;
							}
							if(rescode==0)
							{
								strcpy(pto1_list_temp+pos3,pto_array_list[elt_value]);
								pos3+=strlen(pto_array_list[elt_value])+1;
							}
							if(rescode==0)
							{
								strcpy(pfrom1_list_temp+pos4,pfrom_array_list[elt_value]);
								pos4+=strlen(pfrom_array_list[elt_value])+1;
							}
							if(rescode==0)
							{
								strcpy(lkinfo_list_temp+pos5,lkinfo_array_list[elt_value]);
								pos5+=strlen(lkinfo_array_list[elt_value])+1;
							}
							nb_of_line1++;
							lk_pos++;
							m++;
						}
						valid_path++;
					}

				}
			}
			//finish = clock();
			//std:://cout<<"time to executed iteration j="<<j<<" and time="<<finish-start<<std::endl;
			j=j+2;
			stop=ComputePathWeight(path_end_old,path_matrix_old_len);
			//std:://cout<<"time 1 to executed iteration j="<<j<<" and stop="<<stop<<std::endl;
			path_matrix_old=(int*)realloc(path_matrix_old,nb_of_line*nb_of_hop*sizeof(int));
			path_end_old=(int*)realloc(path_end_old,nb_of_line*sizeof(int));
			path_type=(int*)realloc(path_type,nb_of_line*sizeof(int));
			CopyMatrix(path_matrix_old,path_matrix_new,nb_of_line,j,nb_of_line,0,nb_of_hop); //pure copy of the matrix
			CopyVector(path_end_old,path_end_new,nb_of_line,nb_of_line,0);//copy + element shift
			CopyVector(path_type,path_type_new,nb_of_line,nb_of_line,0);//copy + element shift
			path_matrix_old_len=nb_of_line;
			stop+=ComputePathWeight(path_end_new,nb_of_line);
			//std:://cout<<"time 2 to executed iteration j="<<j<<" and stop="<<stop<<std::endl;
			path_matrix_new_len=0;
			nb_of_line=0;



		}					
		if(stop>0) // get the last valid path of the loop
		{
			for(i=0;i<path_matrix_old_len;i++)
			{

				if(path_end_old[i]==1)
				{
					path_valid=(int*)realloc(path_valid,(valid_path+1)*nb_of_hop*sizeof(int));
					pathid_list_temp=(int*)realloc(pathid_list_temp,(nb_of_line1+j)*sizeof(int));
					link_pos_list_temp=(int*)realloc(link_pos_list_temp,(nb_of_line1+j)*sizeof(int));
					pfrom_list_temp=(int*)realloc(pfrom_list_temp,(nb_of_line1+j)*sizeof(int));
					pto_list_temp=(int*)realloc(pto_list_temp,(nb_of_line1+j)*sizeof(int));
					pfrom1_list_temp=(char*)realloc(pfrom1_list_temp,(nb_of_line1+j)*dev_len*sizeof(char));
					pto1_list_temp=(char*)realloc(pto1_list_temp,(nb_of_line1+j)*dev_len*sizeof(char));
					nfrom1_list_temp=(char*)realloc(nfrom1_list_temp,(nb_of_line1+j)*dev_len*sizeof(char));
					nto1_list_temp=(char*)realloc(nto1_list_temp,(nb_of_line1+j)*dev_len*sizeof(char));
					lkinfo_list_temp=(char*)realloc(lkinfo_list_temp,(nb_of_line1+j)*dev_len*sizeof(char));
					if(lkinfo_list_temp==NULL || nfrom1_list_temp==NULL||pfrom1_list_temp==NULL||nto1_list_temp==NULL||pto1_list_temp==NULL||path_valid==NULL||pathid_list_temp==NULL||link_pos_list_temp==NULL||pfrom_list_temp==NULL||pto_list_temp==NULL)
					{
						if(path_matrix_old!=NULL)
							path_matrix_old=(int*)realloc(path_matrix_old,0*sizeof(int));
						if(path_matrix_new!=NULL)
							path_matrix_new=(int*)realloc(path_matrix_new,0*sizeof(int));
						if(path_valid!=NULL)
							path_valid=(int*)realloc(path_valid,0*sizeof(int));
						if(path_end_new!=NULL)
							path_end_new=(int*)realloc(path_end_new,0*sizeof(int));
						free(elt_idx);
						if(path_end_old!=NULL)
							path_end_old=(int*)realloc(path_end_old,0*sizeof(int));
						free(elt_idx_temp);
						free(path_matrix_temp);
						if(elt_idx_temp1!=NULL)
							free(elt_idx_temp1);
						if(lkid_list!=NULL)
							free(lkid_list);
						if(node_from_list!=NULL)
							free(node_from_list);
						if(node_to_list!=NULL)
							free(node_to_list);
						if(bidirectional_list!=NULL)
							free(bidirectional_list);
						if(lk_weight_list!=NULL)
							free(lk_weight_list);
						if(lktype_list!=NULL)
							free(lktype_list);
						if(lkused_list!=NULL)
							free(lkused_list);
						if(path_type!=NULL)
							free(path_type);
						if(path_type_new!=NULL)
							free(path_type_new);
						for(p=0;p<max_rows;p++)
						{
							free(pto_array_list[p]);
							free(nto_array_list[p]);
							free(pfrom_array_list[p]);
							free(nfrom_array_list[p]);
							free(lkinfo_array_list[p]);

						}
						free(pto_array_list);
						free(nto_array_list);
						free(pfrom_array_list);
						free(nfrom_array_list);
						free(lkinfo_array_list);
						if(link_pos_list_temp!=NULL)
							link_pos_list_temp=(int*)realloc(link_pos_list_temp,0*sizeof(int));
						if(pathid_list_temp!=NULL)
							pathid_list_temp=(int*)realloc(pathid_list_temp,0*sizeof(int));
						if(nfrom1_list_temp!=NULL)
							free(nfrom1_list_temp);
						if(nto1_list_temp!=NULL)
							free(nto1_list_temp);
						if(pfrom1_list_temp!=NULL)
							free(pfrom1_list_temp);
						if(pfrom_list_temp!=NULL)
							free(pfrom_list_temp);
						if(pto1_list_temp!=NULL)
							free(pto1_list_temp);
						if(pto_list_temp!=NULL)
							free(pto_list_temp);
						if(lkinfo_list_temp!=NULL)
							free(lkinfo_list_temp);
						GetErrorMess(appliName,"Unsuccessful malloc",ErrMessage,1);
						return -1;
					}
					lk_pos=1;
					for(m=1;m<j;m++)
					{
						path_valid[valid_path*nb_of_hop+m]=path_matrix_old[i*nb_of_hop+m];
						rescode=GetIndexForTabINT(lkid_list,len_array_gv,path_valid[valid_path*nb_of_hop+m] , elt_idx_temp1,elt_idx_temp1_len,1);
						pathid_list_temp[nb_of_line1]=valid_path;
						link_pos_list_temp[nb_of_line1]=lk_pos;
						elt_value=elt_idx_temp1[0];//necessary one possibility
						if(lkid_list[elt_value]>0)
						{
							pto_list_temp[nb_of_line1]=1;
							pfrom_list_temp[nb_of_line1]=2;
						}
						else
						{
							pto_list_temp[nb_of_line1]=2;
							pfrom_list_temp[nb_of_line1]=1;
						}
						if(rescode==0)
						{
							strcpy(nfrom1_list_temp+pos2,nfrom_array_list[elt_value]);
							pos2+=strlen(nfrom_array_list[elt_value])+1;


						}
						if(rescode==0)
						{
							strcpy(nto1_list_temp+pos1,nto_array_list[elt_value]);
							pos1+=strlen(nto_array_list[elt_value])+1;
						}
						if(rescode==0)
						{
							strcpy(pto1_list_temp+pos3,pto_array_list[elt_value]);
							pos3+=strlen(pto_array_list[elt_value])+1;
						}
						if(rescode==0)
						{
							strcpy(pfrom1_list_temp+pos4,pfrom_array_list[elt_value]);
							pos4+=strlen(pfrom_array_list[elt_value])+1;
						}
						if(rescode==0)
						{
							strcpy(lkinfo_list_temp+pos5,lkinfo_array_list[elt_value]);
							pos5+=strlen(lkinfo_array_list[elt_value])+1;
						}
						nb_of_line1++;
						lk_pos++;
						m++;	
					}
					valid_path++;
				}
			}
			stop=j;

			if(valid_path>0)
			{
				if(len_array<nb_of_line1 || pfrom1_arraylen<pos4 || nfrom1_arraylen<pos2 || nto1_arraylen<pos1 || pto1_arraylen<pos3 || lkinfo1_arraylen<pos5)
				{
					GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrMessage,1);
					len_array=nb_of_line1;
					pfrom1_arraylen=pos4;
					nfrom1_arraylen=pos2;
					nto1_arraylen=pos1;
					pto1_arraylen=pos3;
					lkinfo1_arraylen=pos5;
					//std:://cout<<"len_array="<<len_array<<" and nb of line="<<nb_of_line1<<std::endl;
					//std:://cout<<"pfrom1_arraylen="<<pfrom1_arraylen<<" and pos4="<<pos4<<std::endl;
					//std:://cout<<"nfrom1_arraylen="<<nfrom1_arraylen<<" and pos2="<<pos2<<std::endl;
					//std:://cout<<"pto1_arraylen="<<pto1_arraylen<<" and pos3="<<pos3<<std::endl;
					//std:://cout<<"nto1_arraylen="<<nto1_arraylen<<" and pos1="<<pos1<<std::endl;
					//std:://cout<<"lkinfo1_arraylen="<<lkinfo1_arraylen<<" and pos5="<<pos5<<std::endl;
					rescode=-1;

				}
				else
				{
					memcpy(pfrom1_list,pfrom1_list_temp,pos4);
					memcpy(nfrom1_list,nfrom1_list_temp,pos2);
					memcpy(pto1_list,pto1_list_temp,pos3);
					memcpy(nto1_list,nto1_list_temp,pos1);
					memcpy(lkinfo_list,lkinfo_list_temp,pos5);
					for(i=0;i<nb_of_line1;i++)
					{
						pfrom_list[i]=pfrom_list_temp[i];
						pto_list[i]=pto_list_temp[i];
						pathid_list[i]=pathid_list_temp[i];
						link_pos_list[i]=link_pos_list_temp[i];

					}
					len_array=nb_of_line1;
					pfrom1_arraylen=pos4;
					nfrom1_arraylen=pos2;
					nto1_arraylen=pos1;
					pto1_arraylen=pos3;
					lkinfo1_arraylen=pos5;
				}

			}	


		}
		nb_of_line=0;
		//std:://cout<<"out of the last loop and valid_path="<<valid_path<<std::endl;
		free(elt_idx_temp);
		free(elt_idx);
		if(elt_idx_temp1!=NULL)
			free(elt_idx_temp1);
		if(path_matrix_temp!=NULL)
			free(path_matrix_temp);
		if(path_end_old!=NULL)
			realloc(path_end_old,0*sizeof(int));
		if(path_end_new!=NULL)
			realloc(path_end_new,0*sizeof(int));
		if(path_matrix_old!=NULL )
			realloc(path_matrix_old,0*sizeof(int));	
		if(path_matrix_new!=NULL )
			realloc(path_matrix_new,0*sizeof(int));
		if(path_type!=NULL)
			free(path_type);
		if(path_type_new!=NULL)
			free(path_type_new);
		if(path_valid!=NULL )
		{
			realloc(path_valid,0*sizeof(int));
		}


		if(devtype_given==1 ||devtype_given==0)
			free(_devicetoList_possible);

		if(delete_connectivity==1)
		{
			for(p=0;p<max_rows;p++)
			{
				free(pto_array_list[p]);
				free(nto_array_list[p]);
				free(pfrom_array_list[p]);
				free(nfrom_array_list[p]);
				free(lkinfo_array_list[p]);
			}
			len_array_gv=0;
			if(lkid_list!=NULL)
				free(lkid_list);
			if(node_from_list!=NULL)
				free(node_from_list);
			if(node_to_list!=NULL)
				free(node_to_list);
			if(bidirectional_list!=NULL)
				free(bidirectional_list);
			if(lk_weight_list!=NULL)
				free(lk_weight_list);
			if(lktype_list!=NULL)
				free(lktype_list);
			if(lkused_list!=NULL)
				free(lkused_list);
			free(pto_array_list);
			free(nto_array_list);
			free(pfrom_array_list);
			free(nfrom_array_list);
			free(lkinfo_array_list);
		}
		if(link_pos_list_temp!=NULL)
			link_pos_list_temp=(int*)realloc(link_pos_list_temp,0*sizeof(int));
		if(lkinfo_list_temp!=NULL)
			lkinfo_list_temp=(char*)realloc(lkinfo_list_temp,0*sizeof(char));
		if(pathid_list_temp!=NULL)
			pathid_list_temp=(int*)realloc(pathid_list_temp,0*sizeof(int));
		if(nfrom1_list_temp!=NULL)
			free(nfrom1_list_temp);
		if(nto1_list_temp!=NULL)
			free(nto1_list_temp);
		if(pfrom1_list_temp!=NULL)
			free(pfrom1_list_temp);
		if(pfrom_list_temp!=NULL)
			free(pfrom_list_temp);
		if(pto1_list_temp!=NULL)
			free(pto1_list_temp);
		if(pto_list_temp!=NULL)
			free(pto_list_temp);


		if((strstr(ErrMessage,"BUFFER_TOO_SMALL"))==NULL)
		{
			if(valid_path>0)
			{
				GetErrorMess(appliName,"No_ERROR",ErrMessage,0);
				rescode=0;
			}
			else
			{
				GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrMessage,1);
				BIDIRECTIONAL_WAY_USED=1;
				rescode=-1;
			}
		}

		//std:://cout<<"end of connec_cache "<<std::endl;	
		return rescode;
	}
	int GetDetailedConnectivityBetweenDeviceDeviceType_CacheReverse(char appliName[100],char* systemnameList,char* dev_from, char* dev_to,int devtype_given, int& len_array, int &pfrom1_arraylen,int &nfrom1_arraylen,int &pto1_arraylen,int &nto1_arraylen,int &lkinfo1_arraylen,char* nfrom1_list,char* nto1_list,char* pfrom1_list,int* pfrom_list,char* pto1_list,int* pto_list,int* pathid_list,int* link_pos_list,char* lkinfo_list,int reload_connectivity,int delete_connectivity,char* ErrMessage)
	{
		//char appliName[100]="GetDetailedConnectivityBetweenDeviceDeviceTypes_Cache";
		int elt_value=0;
		int elt_idx_len=1000;
		int elt_idx_temp_len=1000;
		int* elt_idx=NULL;
		int* elt_idx_temp=NULL;
		int rescode=0;
		int err_len=1000;
		clock_t start,finish;
		double time;
		int nb_of_hop=23;
		int rescode1=0;
		int deviceid_to=0;
		int len_array_cpy=30000;
		int *path_matrix_old=NULL;
		int *path_matrix_new=NULL;
		int* path_matrix_temp=NULL;
		int path_matrix_old_len=0;
		int path_matrix_new_len=0;
		int *path_end_old=NULL;
		int *path_end_new=NULL;
		int* path_valid=NULL;
		char* nfrom1_list_temp=NULL;
		char* nto1_list_temp=NULL;
		char* pfrom1_list_temp=NULL;
		int* pfrom_list_temp=NULL;
		char* pto1_list_temp=NULL;
		char* lkinfo_list_temp=NULL;
		int* pto_list_temp=NULL;
		int* pathid_list_temp=NULL;
		int* link_pos_list_temp=NULL;
		int pos5=0;
		int dev_len=200;
		int valid_path=0;
		int i=0;
		int j=0;
		int k=0;
		int v=0;
		int l=0;
		int elt_value_temp1=0;
		int elt_value_temp2=0;
		int stop=0;
		int nb_of_line=0;
		int* elt_idx_temp1=NULL;
		int elt_idx_temp1_len=1000;
		int m=0;
		int lk_pos=1;
		char node_name_val[200];
		int pos1,pos2,pos3,pos4;
		int p=0;
		int dto_len=strlen(dev_to);
		int* path_type_new=NULL;
		int* path_type=NULL;
		int _len_array_devto=20000;
		int* _devicetoList_possible=NULL;
		char ErrMess_temp[1000];
		int nb_of_line1=0;
		int elt_idx_int_temp1[100];
		int elt_idx_int_temp1_len=100;

		int linkid_int=0;
		int pfrom_int=0;
		int pto_int=0;
		char ptnb_from[20];
		char ptype_from[50];
		char ptnb_to[20];
		char ptype_to[50];
		int port_idx=0;
		int next_step=0;


		if(devtype_given==1)
		{
			_devicetoList_possible=(int*)malloc(_len_array_devto*sizeof(int));
			if(_devicetoList_possible==NULL)
			{

				GetErrorMess(appliName,"Unsuccessful malloc 1",ErrMessage,1);
				return -1;
			}
			else
			{

				rescode=GetDeviceIDsPerType(dev_to, _len_array_devto, _devicetoList_possible,ErrMess_temp);
			}
			if(rescode!=0)
			{

				free(_devicetoList_possible);
				GetErrorMess(appliName,ErrMess_temp,ErrMessage,1);
				return -1;
			}
		}	
		//std:://cout<<"in reverse conn, before starting"<<std::endl;


		if(reload_connectivity==1 || lkid_list_scd==NULL || nfrom_array_list_scd==NULL)
		{
			//std:://cout<<"loading connectivity table BIDIRECTIONAL_WAY_USED="<<BIDIRECTIONAL_WAY_USED<<std::endl;
			if(devtype_given==1 ||devtype_given==0)
				rescode=LoadConnectivityTable_InCache( systemnameList,dev_from,"none",BIDIRECTIONAL_WAY_USED,0,ErrMess_temp);
			else
			{
				if(devtype_given==2)
					rescode=LoadConnectivityTable_InCache( systemnameList,dev_from,dev_to,BIDIRECTIONAL_WAY_USED, 0,ErrMess_temp);
			}

			//std:://cout<<"after LoadConnectivityTable_InCache and rescode="<<rescode<<"_len_array_gv="<<_len_array_gv<< std::endl;
			if(rescode!=0)
			{

				for(p=0;p<max_rows_scd;p++)
				{
					free(pto_array_list_scd[p]);
					free(nto_array_list_scd[p]);
					free(pfrom_array_list_scd[p]);
					free(nfrom_array_list_scd[p]);
					free(lkinfo_array_list_scd[p]);

				}
				free(pto_array_list_scd);
				free(nto_array_list_scd);
				free(pfrom_array_list_scd);
				free(nfrom_array_list_scd);
				free(lkinfo_array_list_scd);
				if(lkid_list_scd!=NULL)
					free(lkid_list_scd);
				if(node_from_list_scd!=NULL)
					free(node_from_list_scd);
				if(node_to_list_scd!=NULL)
					free(node_to_list_scd);
				if(bidirectional_list_scd!=NULL)
					free(bidirectional_list_scd);
				if(lk_weight_list_scd!=NULL)
					free(lk_weight_list_scd);
				if(lktype_list_scd!=NULL)
					free(lktype_list_scd);
				if(lkused_list_scd!=NULL)
					free(lkused_list_scd);
				GetErrorMess(appliName,ErrMess_temp,ErrMessage,1);
				return -1;
			}
		}
		else
		{
			rescode=0;
		}	
		//std:://cout<<"Connectivity loaded"<<std::endl;
		elt_idx=(int*)malloc(elt_idx_len*sizeof(int));
		elt_idx_temp=(int*)malloc(elt_idx_temp_len*sizeof(int));
		if(elt_idx==NULL || elt_idx_temp==NULL)
		{
			for(p=0;p<max_rows_scd;p++)
			{
				free(pto_array_list_scd[p]);
				free(nto_array_list_scd[p]);
				free(pfrom_array_list_scd[p]);
				free(nfrom_array_list_scd[p]);
				free(lkinfo_array_list_scd[p]);

			}
			free(pto_array_list_scd);
			free(nto_array_list_scd);
			free(pfrom_array_list_scd);
			free(nfrom_array_list_scd);
			free(lkinfo_array_list_scd);
			if(lkid_list_scd!=NULL)
				free(lkid_list_scd);
			if(node_from_list_scd!=NULL)
				free(node_from_list_scd);
			if(node_to_list_scd!=NULL)
				free(node_to_list_scd);
			if(bidirectional_list_scd!=NULL)
				free(bidirectional_list_scd);
			if(lk_weight_list_scd!=NULL)
				free(lk_weight_list_scd);
			if(lktype_list_scd!=NULL)
				free(lktype_list_scd);
			if(lkused_list_scd!=NULL)
				free(lkused_list_scd);
			if(elt_idx!=NULL)
				free(elt_idx);
			if(elt_idx_temp!=NULL)
				free(elt_idx_temp);

			GetErrorMess(appliName,"Unsuccessful malloc",ErrMessage,1);
			return -1;
		}
		i=0;
		int deviceid_from=0;
		//std:://cout<<"in reverse, before getting the tabCHAR, devfrom="<<dev_from<<std::endl;
		rescode=GetIndexForTabCHAR_second(nfrom_array_list_scd,max_rows_scd, dev_from, elt_idx,elt_idx_len);
		//std:://cout<<"in reversr max_rows="<<max_rows_scd<<" and rescode="<<dev_from<<std::endl;
		if(rescode==-1)
		{

			if(delete_connectivity==1)
			{

				for(p=0;p<max_rows_scd;p++)
				{
					free(pto_array_list_scd[p]);
					free(nto_array_list_scd[p]);
					free(pfrom_array_list_scd[p]);
					free(nfrom_array_list_scd[p]);
					free(lkinfo_array_list_scd[p]);

				}
				free(pto_array_list_scd);
				free(nto_array_list_scd);
				free(pfrom_array_list_scd);
				free(nfrom_array_list_scd);
				free(lkinfo_array_list_scd);
				if(lkid_list_scd!=NULL)
					free(lkid_list_scd);
				if(node_from_list_scd!=NULL)
					free(node_from_list_scd);
				if(node_to_list_scd!=NULL)
					free(node_to_list_scd);
				if(bidirectional_list_scd!=NULL)
					free(bidirectional_list_scd);
				if(lk_weight_list_scd!=NULL)
					free(lk_weight_list_scd);
				if(lktype_list_scd!=NULL)
					free(lktype_list_scd);
				if(lkused_list_scd!=NULL)
					free(lkused_list_scd);
			}
			free(elt_idx_temp);
			free(elt_idx);
			GetErrorMess(appliName,"DEVICE NOT CONNECTED",ErrMessage,1);
			return -1;
		}
		else
		{
			i=elt_idx[0];
			deviceid_from=node_from_list_scd[i];
		}
		if(devtype_given==2 ||devtype_given==0)
		{
			rescode=GetIndexForTabCHAR_second(nto_array_list_scd,max_rows_scd, dev_to, elt_idx,elt_idx_len);
			//std:://cout<<"in reversr max_rows="<<max_rows_scd<<" and rescode="<<dev_from<<std::endl;
			if(rescode==-1)
			{

				if(delete_connectivity==1)
				{

					for(p=0;p<max_rows_scd;p++)
					{
						free(pto_array_list_scd[p]);
						free(nto_array_list_scd[p]);
						free(pfrom_array_list_scd[p]);
						free(nfrom_array_list_scd[p]);
						free(lkinfo_array_list_scd[p]);

					}
					free(pto_array_list_scd);
					free(nto_array_list_scd);
					free(pfrom_array_list_scd);
					free(nfrom_array_list_scd);
					free(lkinfo_array_list_scd);
					if(lkid_list_scd!=NULL)
						free(lkid_list_scd);
					if(node_from_list_scd!=NULL)
						free(node_from_list_scd);
					if(node_to_list_scd!=NULL)
						free(node_to_list_scd);
					if(bidirectional_list_scd!=NULL)
						free(bidirectional_list_scd);
					if(lk_weight_list_scd!=NULL)
						free(lk_weight_list_scd);
					if(lktype_list_scd!=NULL)
						free(lktype_list_scd);
					if(lkused_list_scd!=NULL)
						free(lkused_list_scd);
				}
				free(elt_idx_temp);
				free(elt_idx);
				GetErrorMess(appliName,"DEVICE NOT CONNECTED",ErrMessage,1);
				return -1;
			}
			else
			{
				if(devtype_given==2)
				{
					i=elt_idx[0];
					deviceid_to=node_to_list_scd[i];
				}
				else
				{
					_devicetoList_possible=(int*)malloc(_len_array_devto*sizeof(int));
					if(_devicetoList_possible==NULL)
					{
						GetErrorMess(appliName,"Unsuccessful malloc 1",ErrMessage,1);
						return -1;
					}
					else
					{
						for(i=0;i<elt_idx_len;i++)
						{
							j=elt_idx[i];
							_devicetoList_possible[i]=node_to_list_scd[j];
						}
						_len_array_devto=elt_idx_len;
					}
				}
			}
		}
		rescode=GetIndexForTabINT(node_from_list_scd,len_array_gv_scd, deviceid_from, elt_idx,elt_idx_len,1);
		path_matrix_old=(int*)realloc(path_matrix_old,elt_idx_len*nb_of_hop*sizeof(int));
		path_end_new=(int*)realloc(path_end_new,elt_idx_len*sizeof(int));
		path_end_old=(int*)realloc(path_end_old,elt_idx_len*sizeof(int));
		path_type=(int*)realloc(path_type,elt_idx_len*sizeof(int));
		path_type_new=(int*)realloc(path_type_new,elt_idx_len*sizeof(int));
		path_matrix_temp=(int*)malloc((nb_of_hop+1)*sizeof(int));
		elt_idx_temp1=(int*)malloc(elt_idx_temp_len*sizeof(int));
		if(elt_idx_temp1==NULL||path_matrix_temp==NULL||path_end_old==NULL||path_end_new==NULL||path_type_new==NULL||path_type==NULL)
		{
			free(elt_idx_temp);
			free(elt_idx);
			if(elt_idx_temp1!=NULL)
				free(elt_idx_temp1);
			if(path_matrix_temp!=NULL)
				free(path_matrix_temp);
			if(path_end_old!=NULL)
				free(path_end_old);
			if(path_end_new!=NULL)
				free(path_end_new);
			if(path_type_new!=NULL)
				free(path_type_new);
			if(path_type!=NULL)
				free(path_type);
			if(path_matrix_old!=NULL )
			{
				free(path_matrix_old);
			}
			for(p=0;p<max_rows_scd;p++)
			{
				free(pto_array_list_scd[p]);
				free(nto_array_list_scd[p]);
				free(pfrom_array_list_scd[p]);
				free(nfrom_array_list_scd[p]);
				free(lkinfo_array_list_scd[p]);

			}
			free(pto_array_list_scd);
			free(nto_array_list_scd);
			free(pfrom_array_list_scd);
			free(nfrom_array_list_scd);
			free(lkinfo_array_list_scd);
			if(lkid_list_scd!=NULL)
				free(lkid_list_scd);
			if(node_from_list_scd!=NULL)
				free(node_from_list_scd);
			if(node_to_list_scd!=NULL)
				free(node_to_list_scd);
			if(bidirectional_list_scd!=NULL)
				free(bidirectional_list_scd);
			if(lk_weight_list_scd!=NULL)
				free(lk_weight_list_scd);
			if(lktype_list_scd!=NULL)
				free(lktype_list_scd);
			if(lkused_list_scd!=NULL)
				free(lkused_list_scd);
			GetErrorMess(appliName,"Unsuccessful malloc",ErrMessage,1);
			return -1;

		}
		nb_of_line=3;
		for(i=0;i<elt_idx_len;i++)
		{
			for(j=0;j<nb_of_line;j++)
			{
				m=elt_idx[i];
				path_matrix_old[i*nb_of_hop+j]=node_from_list_scd[m];
				j++;
				path_matrix_old[i*nb_of_hop+j]=lkid_list_scd[m];
				j++;
				path_matrix_old[i*nb_of_hop+j]=node_to_list_scd[m];

				if(devtype_given==1 ||devtype_given==0)
				{
					rescode=GetIndexForTabINT(_devicetoList_possible,_len_array_devto,node_to_list_scd[m] , elt_idx_temp1,elt_idx_temp1_len,1);
					if(elt_idx_temp1_len>0 && rescode==0)
						path_end_old[i]=1;
					else
						path_end_old[i]=0;
					path_type[i]=lktype_list_scd[m];
				}
				else
				{
					path_type[i]=lktype_list_scd[m];
					if(node_to_list_scd[m]==deviceid_to)
					{
						path_end_old[i]=1;

					}
					else
					{
						path_end_old[i]=0;

					}

				}
			}
		}
		i=0;
		j=3;
		nb_of_line=0;
		path_matrix_old_len=elt_idx_len;
		stop=ComputePathWeight(path_end_old,path_matrix_old_len);
		path_matrix_new_len=0;
		pos1=0;
		pos2=0;
		pos3=0;
		pos4=0;
		pos5=0;
		//std:://cout<<"before getting in the loop"<<std::endl;
		start = clock();
		while(stop==0 && j<nb_of_hop)
		{
			for(i=0;i<path_matrix_old_len;i++)
			{

				if(path_end_old[i]==0)
				{

					elt_value=path_matrix_old[i*nb_of_hop+(j-1)];

					rescode=GetIndexForTabINT(node_from_list_scd,len_array_gv_scd,elt_value , elt_idx_temp,elt_idx_temp_len,1);
					//std:://cout<<"in the loop rescode="<<rescode<<" and elt_idx_temp_len="<<elt_idx_temp_len<<std::endl;
					if(elt_idx_temp_len==0)
					{
						if(elt_idx_temp==0)
							path_end_old[i]=-1;
					}
					else
					{
						path_matrix_new_len+=elt_idx_temp_len; //nb of paths so far
						path_matrix_new=(int*)realloc(path_matrix_new,path_matrix_new_len*nb_of_hop*sizeof(int));
						path_end_new=(int*)realloc(path_end_new,path_matrix_new_len*sizeof(int));
						path_type_new=(int*)realloc(path_type_new,path_matrix_new_len*sizeof(int));
						if(path_matrix_new==NULL || path_end_new==NULL||path_type_new==NULL)
						{
							if(path_matrix_old!=NULL)
								path_matrix_old=(int*)realloc(path_matrix_old,0*sizeof(int));
							if(path_matrix_new!=NULL)
								path_matrix_new=(int*)realloc(path_matrix_new,0*sizeof(int));
							if(path_end_new!=NULL)
								path_end_new=(int*)realloc(path_end_new,0*sizeof(int));
							if(path_end_old!=NULL)
								path_end_old=(int*)realloc(path_end_old,0*sizeof(int));
							free(elt_idx_temp);
							free(elt_idx);
							if(path_type_new!=NULL)
								free(path_type_new);
							if(path_type!=NULL)
								free(path_type);
							free(path_matrix_temp);
							if(elt_idx_temp1!=NULL)
								free(elt_idx_temp1);
							if(path_valid!=NULL )
								realloc(path_valid,0*sizeof(int));

							for(p=0;p<max_rows_scd;p++)
							{
								free(pto_array_list_scd[p]);
								free(nto_array_list_scd[p]);
								free(pfrom_array_list_scd[p]);
								free(nfrom_array_list_scd[p]);
								free(lkinfo_array_list_scd[p]);

							}
							free(pto_array_list_scd);
							free(nto_array_list_scd);
							free(pfrom_array_list_scd);
							free(nfrom_array_list_scd);
							free(lkinfo_array_list_scd);
							if(lkid_list_scd!=NULL)
								free(lkid_list_scd);
							if(node_from_list_scd!=NULL)
								free(node_from_list_scd);
							if(node_to_list_scd!=NULL)
								free(node_to_list_scd);
							if(bidirectional_list_scd!=NULL)
								free(bidirectional_list_scd);
							if(lk_weight_list_scd!=NULL)
								free(lk_weight_list_scd);
							if(lktype_list_scd!=NULL)
								free(lktype_list_scd);
							if(lkused_list_scd!=NULL)
								free(lkused_list_scd);
							if(link_pos_list_temp!=NULL)
								link_pos_list_temp=(int*)realloc(link_pos_list_temp,0*sizeof(int));
							if(lkinfo_list_temp!=NULL)
								lkinfo_list_temp=(char*)realloc(lkinfo_list_temp,0*sizeof(char));
							if(pathid_list_temp!=NULL)
								pathid_list_temp=(int*)realloc(pathid_list_temp,0*sizeof(int));
							if(nfrom1_list_temp!=NULL)
								free(nfrom1_list_temp);
							if(nto1_list_temp!=NULL)
								free(nto1_list_temp);
							if(pfrom1_list_temp!=NULL)
								free(pfrom1_list_temp);
							if(pfrom_list_temp!=NULL)
								free(pfrom_list_temp);
							if(pto1_list_temp!=NULL)
								free(pto1_list_temp);
							if(pto_list_temp!=NULL)
								free(pto_list_temp);
							GetErrorMess(appliName,"Unsuccessful malloc",ErrMessage,1);
							return -1;
						}
						else
						{
							for(l=0;l<j;l++)//copy of the corresponding path line to check loop
							{
								path_matrix_temp[l]=path_matrix_old[i*nb_of_hop+l];

							}

						}

						for(k=0;k<elt_idx_temp_len;k++)
						{
							elt_value_temp1=elt_idx_temp[k];
							elt_value_temp2=node_to_list_scd[elt_value_temp1];

							if(path_type[i]%lktype_list_scd[elt_value_temp1]==0||lktype_list_scd[elt_value_temp1]%path_type[i]==0)
							{
								rescode=GetIndexForTabINT(path_matrix_temp,j,elt_value_temp2 , elt_idx_temp1,elt_idx_temp1_len,2);
								//std:://cout<<"in the loop k="<<k<<" and rescode="<<rescode<<std::endl;

								if(rescode==-1 ||elt_idx_temp1_len==0)
								{
									finish=clock();
									time = (double(finish)-double(start))/CLOCKS_PER_SEC;  
									if(time>TIMEOUT_DEFINED)
									{
										if(path_matrix_old!=NULL)
											path_matrix_old=(int*)realloc(path_matrix_old,0*sizeof(int));
										if(path_matrix_new!=NULL)
											path_matrix_new=(int*)realloc(path_matrix_new,0*sizeof(int));
										if(path_end_new!=NULL)
											path_end_new=(int*)realloc(path_end_new,0*sizeof(int));
										if(path_end_old!=NULL)
											path_end_old=(int*)realloc(path_end_old,0*sizeof(int));
										free(elt_idx_temp);
										free(elt_idx);
										free(path_matrix_temp);
										if(elt_idx_temp1!=NULL)
											free(elt_idx_temp1);
										if(path_valid!=NULL )
											realloc(path_valid,0*sizeof(int));
										for(p=0;p<max_rows_scd;p++)
										{
											free(pto_array_list_scd[p]);
											free(nto_array_list_scd[p]);
											free(pfrom_array_list_scd[p]);
											free(nfrom_array_list_scd[p]);
											free(lkinfo_array_list_scd[p]);

										}
										free(pto_array_list_scd);
										free(nto_array_list_scd);
										free(pfrom_array_list_scd);
										free(nfrom_array_list_scd);
										free(lkinfo_array_list_scd);
										if(lkid_list_scd!=NULL)
											free(lkid_list_scd);
										if(node_from_list_scd!=NULL)
											free(node_from_list_scd);
										if(node_to_list_scd!=NULL)
											free(node_to_list_scd);
										if(bidirectional_list_scd!=NULL)
											free(bidirectional_list_scd);
										if(lk_weight_list_scd!=NULL)
											free(lk_weight_list_scd);
										if(lktype_list_scd!=NULL)
											free(lktype_list_scd);
										if(lkused_list_scd!=NULL)
											free(lkused_list_scd);
										if(link_pos_list_temp!=NULL)
											link_pos_list_temp=(int*)realloc(link_pos_list_temp,0*sizeof(int));
										if(lkinfo_list_temp!=NULL)
											lkinfo_list_temp=(char*)realloc(lkinfo_list_temp,0*sizeof(char));
										if(pathid_list_temp!=NULL)
											pathid_list_temp=(int*)realloc(pathid_list_temp,0*sizeof(int));
										if(nfrom1_list_temp!=NULL)
											free(nfrom1_list_temp);
										if(nto1_list_temp!=NULL)
											free(nto1_list_temp);
										if(pfrom1_list_temp!=NULL)
											free(pfrom1_list_temp);
										if(pfrom_list_temp!=NULL)
											free(pfrom_list_temp);
										if(pto1_list_temp!=NULL)
											free(pto1_list_temp);
										if(pto_list_temp!=NULL)
											free(pto_list_temp);
										GetErrorMess(appliName,"CONNECTIVITY TOO COMPLEX, EXECUTION TIME TOO LONG",ErrMessage,1);
										return -1;
									}

									if(len_array_gv_scd_internal>0)
									{
										rescode=GetIndexForTabINT(node_from_list_scd_internal,len_array_gv_scd_internal,elt_value , elt_idx_int_temp1,elt_idx_int_temp1_len,1);
										if(elt_idx_int_temp1_len>0)
										{
											linkid_int=path_matrix_old[i*nb_of_hop+(j-2)];
											rescode=GetIndexForTabINT(lkid_list_scd,len_array_gv_scd,linkid_int , elt_idx_int_temp1,elt_idx_int_temp1_len,1);
											port_idx=elt_idx_int_temp1[0];
											rescode=DecomposePort(pfrom_array_list_scd[elt_value_temp1],strlen(pfrom_array_list_scd[elt_value_temp1]),  ptnb_from, ptype_from);
											rescode+=DecomposePort(pto_array_list_scd[port_idx],strlen(pto_array_list_scd[port_idx]),  ptnb_to, ptype_to);
											if(linkid_int>0)
											{
												rescode+=GetPortID_portinfo(elt_value,ptnb_from,ptype_from,1,pfrom_int,ErrMess_temp);
												rescode+=GetPortID_portinfo(elt_value,ptnb_to,ptype_to,2,pto_int,ErrMess_temp);
												rescode+=CheckInternalConnectivity(pfrom_int, pto_int,ErrMess_temp);

											}
											else
											{
												rescode+=GetPortID_portinfo(elt_value,ptnb_from,ptype_from,2,pfrom_int,ErrMess_temp);
												rescode+=GetPortID_portinfo(elt_value,ptnb_to,ptype_to,1,pto_int,ErrMess_temp);
												rescode+=CheckInternalConnectivity(pto_int, pfrom_int,ErrMess_temp);

											}
											if(rescode==0)//then it's OK link allowed
												next_step=0;
											else
												next_step=-1;
										}
										else
											next_step=0;

									}
									else
										next_step=0;//measn it's OK we go to the next step
									if(next_step==0)
									{

										for(m=0;m<j;m++)
											path_matrix_new[nb_of_line*nb_of_hop+m]=path_matrix_old[i*nb_of_hop+m];	
										path_matrix_new[nb_of_line*nb_of_hop+j]=lkid_list_scd[elt_value_temp1];
										path_matrix_new[nb_of_line*nb_of_hop+j+1]=elt_value_temp2;
										if(devtype_given==1)
										{
											rescode=GetIndexForTabINT(_devicetoList_possible,_len_array_devto,elt_value_temp2 , elt_idx_temp1,elt_idx_temp1_len,1);
											if(elt_idx_temp1_len>0 && rescode==0)
											{
												path_end_new[nb_of_line]=1;
												if(path_type[i]%lktype_list_scd[elt_value_temp1]==0)
													path_type_new[nb_of_line]=lktype_list_scd[elt_value_temp1];
												else
													path_type_new[nb_of_line]=path_type[i];
											}
											else
											{
												if(lk_weight_list_scd[elt_value_temp1]!=2)
												{
													path_end_new[nb_of_line]=0;
													if(path_type[i]%lktype_list_scd[elt_value_temp1]==0)
														path_type_new[nb_of_line]=lktype_list_scd[elt_value_temp1];
													else
														path_type_new[nb_of_line]=path_type[i];
												}
												else
												{
													path_end_new[nb_of_line]=-1;
													path_type_new[nb_of_line]=-1;
												}
											}
										}
										else
										{
											strcpy(node_name_val,nto_array_list_scd[elt_value_temp1]);

											if(strstr(node_name_val,dev_to)!=NULL)
											{
												path_end_new[nb_of_line]=1;
												if(path_type[i]%lktype_list_scd[elt_value_temp1]==0)
													path_type_new[nb_of_line]=lktype_list_scd[elt_value_temp1];
												else
													path_type_new[nb_of_line]=path_type[i];
											}
											else
											{
												if(lk_weight_list_scd[elt_value_temp1]==2)
												{
													path_end_new[nb_of_line]=-1;
													path_type_new[nb_of_line]=-1;
												}
												else
												{
													path_end_new[nb_of_line]=0;
													if(path_type[i]%lktype_list_scd[elt_value_temp1]==0)
														path_type_new[nb_of_line]=lktype_list_scd[elt_value_temp1];
													else
														path_type_new[nb_of_line]=path_type[i];
												}
											}

										}
										nb_of_line++;
									}
									else
									{
										path_end_new[nb_of_line]=-1;
										path_type_new[nb_of_line]=-1;
										nb_of_line++;
									}
								}
								else
								{
									path_end_new[nb_of_line]=-1;
									path_type_new[nb_of_line]=-1;
									nb_of_line++;
								}

							}
							else
							{
								path_end_new[nb_of_line]=-1;
								path_type_new[nb_of_line]=-1;
								nb_of_line++;
							}
						}

					}
				}
				else
				{
					if(path_end_old[i]==1)
					{
						path_valid=(int*)realloc(path_valid,(valid_path+1)*nb_of_hop*sizeof(int));
						pathid_list_temp=(int*)realloc(pathid_list_temp,(nb_of_line1+j)*sizeof(int));
						link_pos_list_temp=(int*)realloc(link_pos_list_temp,(nb_of_line1+j)*sizeof(int));
						pfrom_list_temp=(int*)realloc(pfrom_list_temp,(nb_of_line1+j)*sizeof(int));
						pto_list_temp=(int*)realloc(pto_list_temp,(nb_of_line1+j)*sizeof(int));
						pfrom1_list_temp=(char*)realloc(pfrom1_list_temp,(nb_of_line1+j)*dev_len*sizeof(char));
						pto1_list_temp=(char*)realloc(pto1_list_temp,(nb_of_line1+j)*dev_len*sizeof(char));
						nfrom1_list_temp=(char*)realloc(nfrom1_list_temp,(nb_of_line1+j)*dev_len*sizeof(char));
						nto1_list_temp=(char*)realloc(nto1_list_temp,(nb_of_line1+j)*dev_len*sizeof(char));
						lkinfo_list_temp=(char*)realloc(lkinfo_list_temp,(nb_of_line1+j)*dev_len*sizeof(char));
						if(lkinfo_list_temp==NULL||nfrom1_list_temp==NULL||pfrom1_list_temp==NULL||nto1_list_temp==NULL||pto1_list_temp==NULL||path_valid==NULL||pathid_list_temp==NULL||link_pos_list_temp==NULL||pfrom_list_temp==NULL||pto_list_temp==NULL)
						{
							if(path_matrix_old!=NULL)
								path_matrix_old=(int*)realloc(path_matrix_old,0*sizeof(int));
							if(path_matrix_new!=NULL)
								path_matrix_new=(int*)realloc(path_matrix_new,0*sizeof(int));
							if(path_valid!=NULL)
								path_valid=(int*)realloc(path_valid,0*sizeof(int));
							if(path_end_new!=NULL)
								path_end_new=(int*)realloc(path_end_new,0*sizeof(int));
							if(path_type_new!=NULL)
								path_type_new=(int*)realloc(path_type_new,0*sizeof(int));
							if(path_type!=NULL)
								path_type=(int*)realloc(path_type,0*sizeof(int));
							if(path_end_old!=NULL)
								path_end_old=(int*)realloc(path_end_old,0*sizeof(int));
							free(elt_idx_temp);
							free(path_matrix_temp);
							if(elt_idx_temp1!=NULL)
								free(elt_idx_temp1);
							for(p=0;p<max_rows_scd;p++)
							{
								free(pto_array_list_scd[p]);
								free(nto_array_list_scd[p]);
								free(pfrom_array_list_scd[p]);
								free(nfrom_array_list_scd[p]);
								free(lkinfo_array_list_scd[p]);

							}
							free(pto_array_list_scd);
							free(nto_array_list_scd);
							free(pfrom_array_list_scd);
							free(nfrom_array_list_scd);
							free(lkinfo_array_list_scd);
							if(lkid_list_scd!=NULL)
								free(lkid_list_scd);
							if(node_from_list_scd!=NULL)
								free(node_from_list_scd);
							if(node_to_list_scd!=NULL)
								free(node_to_list_scd);
							if(bidirectional_list_scd!=NULL)
								free(bidirectional_list_scd);
							if(lk_weight_list_scd!=NULL)
								free(lk_weight_list_scd);
							if(lktype_list_scd!=NULL)
								free(lktype_list_scd);
							if(lkused_list_scd!=NULL)
								free(lkused_list_scd);
							free(elt_idx);
							if(link_pos_list_temp!=NULL)
								link_pos_list_temp=(int*)realloc(link_pos_list_temp,0*sizeof(int));
							if(pathid_list_temp!=NULL)
								pathid_list_temp=(int*)realloc(pathid_list_temp,0*sizeof(int));
							if(nfrom1_list_temp!=NULL)
								free(nfrom1_list_temp);
							if(nto1_list_temp!=NULL)
								free(nto1_list_temp);
							if(pfrom1_list_temp!=NULL)
								free(pfrom1_list_temp);
							if(pfrom_list_temp!=NULL)
								free(pfrom_list_temp);
							if(pto1_list_temp!=NULL)
								free(pto1_list_temp);
							if(pto_list_temp!=NULL)
								free(pto_list_temp);
							if(lkinfo_list_temp!=NULL)
								free(lkinfo_list_temp);
							GetErrorMess(appliName,"Unsuccessful malloc",ErrMessage,1);
							return -1;
						}
						lk_pos=1;
						for(m=1;m<j;m++)
						{

							path_valid[valid_path*nb_of_hop+m]=path_matrix_old[i*nb_of_hop+m];
							rescode=GetIndexForTabINT(lkid_list_scd,len_array_gv_scd,path_valid[valid_path*nb_of_hop+m] , elt_idx_temp1,elt_idx_temp1_len,1);
							pathid_list_temp[nb_of_line1]=valid_path;
							link_pos_list_temp[nb_of_line1]=lk_pos;
							elt_value=elt_idx_temp1[0];//necessary one possibility
							if(lkid_list_scd[elt_value]>0)
							{
								pto_list_temp[nb_of_line1]=2;
								pfrom_list_temp[nb_of_line1]=1;
							}
							else
							{
								pto_list_temp[nb_of_line1]=1;
								pfrom_list_temp[nb_of_line1]=2;
							}
							if(rescode==0)
							{
								strcpy(nfrom1_list_temp+pos2,nfrom_array_list_scd[elt_value]);
								//std:://cout<<"value of nfrom_array_list[elt_value]="<<nfrom_array_list[elt_value]<<std::endl;
								//std:://cout<<"value of nfrom1_list_temp"<<nfrom1_list_temp<<std::endl;
								//std:://cout<<"value of nfrom1_list_temp+pos2"<<nfrom1_list_temp+pos2<<std::endl;
								//	f//printf (f, "node_from_name=%s and node_id=%d \n",nfrom1_list+pos2,_node_from_list_gv[elt_value]);
								pos2+=strlen(nfrom_array_list_scd[elt_value])+1;

							}
							if(rescode==0)
							{
								strcpy(nto1_list_temp+pos1,nto_array_list_scd[elt_value]);
								//std:://cout<<"value of nto_array_list[elt_value]="<<nto_array_list[elt_value]<<std::endl;
								//std:://cout<<"value of nto1_list_temp"<<nto1_list_temp<<std::endl;
								pos1+=strlen(nto_array_list_scd[elt_value])+1;
							}
							if(rescode==0)
							{
								strcpy(pto1_list_temp+pos3,pto_array_list_scd[elt_value]);
								pos3+=strlen(pto_array_list_scd[elt_value])+1;
							}
							if(rescode==0)
							{
								strcpy(pfrom1_list_temp+pos4,pfrom_array_list_scd[elt_value]);
								pos4+=strlen(pfrom_array_list_scd[elt_value])+1;
							}
							if(rescode==0)
							{
								strcpy(lkinfo_list_temp+pos5,lkinfo_array_list_scd[elt_value]);
								pos5+=strlen(lkinfo_array_list_scd[elt_value])+1;
							}
							nb_of_line1++;
							lk_pos++;
							m++;
						}
						valid_path++;
					}

				}
			}
			//finish = clock();
			//std:://cout<<"time to executed iteration j="<<j<<" and time="<<finish-start<<std::endl;
			j=j+2;
			stop=ComputePathWeight(path_end_old,path_matrix_old_len);
			path_matrix_old=(int*)realloc(path_matrix_old,nb_of_line*nb_of_hop*sizeof(int));
			path_end_old=(int*)realloc(path_end_old,nb_of_line*sizeof(int));
			path_type=(int*)realloc(path_type,nb_of_line*sizeof(int));
			CopyMatrix(path_matrix_old,path_matrix_new,nb_of_line,j,nb_of_line,0,nb_of_hop); //pure copy of the matrix
			CopyVector(path_end_old,path_end_new,nb_of_line,nb_of_line,0);//copy + element shift
			CopyVector(path_type,path_type_new,nb_of_line,nb_of_line,0);//copy + element shift
			path_matrix_old_len=nb_of_line;
			stop+=ComputePathWeight(path_end_new,nb_of_line);

			path_matrix_new_len=0;
			nb_of_line=0;



		}					
		if(stop>0) // get the last valid path of the loop
		{
			for(i=0;i<path_matrix_old_len;i++)
			{

				if(path_end_old[i]==1)
				{
					path_valid=(int*)realloc(path_valid,(valid_path+1)*nb_of_hop*sizeof(int));
					pathid_list_temp=(int*)realloc(pathid_list_temp,(nb_of_line1+j)*sizeof(int));
					link_pos_list_temp=(int*)realloc(link_pos_list_temp,(nb_of_line1+j)*sizeof(int));
					pfrom_list_temp=(int*)realloc(pfrom_list_temp,(nb_of_line1+j)*sizeof(int));
					pto_list_temp=(int*)realloc(pto_list_temp,(nb_of_line1+j)*sizeof(int));
					pfrom1_list_temp=(char*)realloc(pfrom1_list_temp,(nb_of_line1+j)*dev_len*sizeof(char));
					pto1_list_temp=(char*)realloc(pto1_list_temp,(nb_of_line1+j)*dev_len*sizeof(char));
					nfrom1_list_temp=(char*)realloc(nfrom1_list_temp,(nb_of_line1+j)*dev_len*sizeof(char));
					nto1_list_temp=(char*)realloc(nto1_list_temp,(nb_of_line1+j)*dev_len*sizeof(char));
					lkinfo_list_temp=(char*)realloc(lkinfo_list_temp,(nb_of_line1+j)*dev_len*sizeof(char));
					if(lkinfo_list_temp==NULL || nfrom1_list_temp==NULL||pfrom1_list_temp==NULL||nto1_list_temp==NULL||pto1_list_temp==NULL||path_valid==NULL||pathid_list_temp==NULL||link_pos_list_temp==NULL||pfrom_list_temp==NULL||pto_list_temp==NULL)
					{
						if(path_matrix_old!=NULL)
							path_matrix_old=(int*)realloc(path_matrix_old,0*sizeof(int));
						if(path_matrix_new!=NULL)
							path_matrix_new=(int*)realloc(path_matrix_new,0*sizeof(int));
						if(path_valid!=NULL)
							path_valid=(int*)realloc(path_valid,0*sizeof(int));
						if(path_end_new!=NULL)
							path_end_new=(int*)realloc(path_end_new,0*sizeof(int));
						if(path_type_new!=NULL)
							path_type_new=(int*)realloc(path_type_new,0*sizeof(int));
						if(path_type!=NULL)
							path_type=(int*)realloc(path_type,0*sizeof(int));
						if(path_end_old!=NULL)
							path_end_old=(int*)realloc(path_end_old,0*sizeof(int));
						free(elt_idx_temp);
						free(elt_idx);
						free(path_matrix_temp);
						if(elt_idx_temp1!=NULL)
							free(elt_idx_temp1);
						for(p=0;p<max_rows_scd;p++)
						{
							free(pto_array_list_scd[p]);
							free(nto_array_list_scd[p]);
							free(pfrom_array_list_scd[p]);
							free(nfrom_array_list_scd[p]);
							free(lkinfo_array_list_scd[p]);

						}
						free(pto_array_list_scd);
						free(nto_array_list_scd);
						free(pfrom_array_list_scd);
						free(nfrom_array_list_scd);
						free(lkinfo_array_list_scd);
						if(lkid_list_scd!=NULL)
							free(lkid_list_scd);
						if(node_from_list_scd!=NULL)
							free(node_from_list_scd);
						if(node_to_list_scd!=NULL)
							free(node_to_list_scd);
						if(bidirectional_list_scd!=NULL)
							free(bidirectional_list_scd);
						if(lk_weight_list_scd!=NULL)
							free(lk_weight_list_scd);
						if(lktype_list_scd!=NULL)
							free(lktype_list_scd);
						if(lkused_list_scd!=NULL)
							free(lkused_list_scd);
						if(link_pos_list_temp!=NULL)
							link_pos_list_temp=(int*)realloc(link_pos_list_temp,0*sizeof(int));
						if(pathid_list_temp!=NULL)
							pathid_list_temp=(int*)realloc(pathid_list_temp,0*sizeof(int));
						if(nfrom1_list_temp!=NULL)
							free(nfrom1_list_temp);
						if(nto1_list_temp!=NULL)
							free(nto1_list_temp);
						if(pfrom1_list_temp!=NULL)
							free(pfrom1_list_temp);
						if(pfrom_list_temp!=NULL)
							free(pfrom_list_temp);
						if(pto1_list_temp!=NULL)
							free(pto1_list_temp);
						if(pto_list_temp!=NULL)
							free(pto_list_temp);
						if(lkinfo_list_temp!=NULL)
							free(lkinfo_list_temp);
						GetErrorMess(appliName,"Unsuccessful malloc",ErrMessage,1);
						return -1;
					}
					lk_pos=1;
					for(m=1;m<j;m++)
					{
						path_valid[valid_path*nb_of_hop+m]=path_matrix_old[i*nb_of_hop+m];
						rescode=GetIndexForTabINT(lkid_list_scd,len_array_gv_scd,path_valid[valid_path*nb_of_hop+m] , elt_idx_temp1,elt_idx_temp1_len,1);
						pathid_list_temp[nb_of_line1]=valid_path;
						link_pos_list_temp[nb_of_line1]=lk_pos;
						elt_value=elt_idx_temp1[0];//necessary one possibility
						if(lkid_list_scd[elt_value]>0)
						{
							pto_list_temp[nb_of_line1]=1;
							pfrom_list_temp[nb_of_line1]=2;
						}
						else
						{
							pto_list_temp[nb_of_line1]=2;
							pfrom_list_temp[nb_of_line1]=1;
						}
						if(rescode==0)
						{
							strcpy(nfrom1_list_temp+pos2,nfrom_array_list_scd[elt_value]);
							pos2+=strlen(nfrom_array_list_scd[elt_value])+1;


						}
						if(rescode==0)
						{
							strcpy(nto1_list_temp+pos1,nto_array_list_scd[elt_value]);
							pos1+=strlen(nto_array_list_scd[elt_value])+1;
						}
						if(rescode==0)
						{
							strcpy(pto1_list_temp+pos3,pto_array_list_scd[elt_value]);
							pos3+=strlen(pto_array_list_scd[elt_value])+1;
						}
						if(rescode==0)
						{
							strcpy(pfrom1_list_temp+pos4,pfrom_array_list_scd[elt_value]);
							pos4+=strlen(pfrom_array_list_scd[elt_value])+1;
						}
						if(rescode==0)
						{
							strcpy(lkinfo_list_temp+pos5,lkinfo_array_list_scd[elt_value]);
							pos5+=strlen(lkinfo_array_list_scd[elt_value])+1;
						}
						nb_of_line1++;
						lk_pos++;
						m++;	
					}
					valid_path++;
				}
			}
			stop=j;

			if(valid_path>0)
			{
				if(len_array<nb_of_line1 || pfrom1_arraylen<pos4 || nfrom1_arraylen<pos2 || nto1_arraylen<pos1 || pto1_arraylen<pos3 || lkinfo1_arraylen<pos5)
				{
					GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrMessage,1);
					len_array=nb_of_line1;
					pfrom1_arraylen=pos4;
					nfrom1_arraylen=pos2;
					nto1_arraylen=pos1;
					pto1_arraylen=pos3;
					lkinfo1_arraylen=pos5;
					rescode=-1;

				}
				else
				{
					memcpy(pfrom1_list,pfrom1_list_temp,pos4);
					memcpy(nfrom1_list,nfrom1_list_temp,pos2);
					memcpy(pto1_list,pto1_list_temp,pos3);
					memcpy(nto1_list,nto1_list_temp,pos1);
					memcpy(lkinfo_list,lkinfo_list_temp,pos5);
					for(i=0;i<nb_of_line1;i++)
					{
						pfrom_list[i]=pfrom_list_temp[i];
						pto_list[i]=pto_list_temp[i];
						pathid_list[i]=pathid_list_temp[i];
						link_pos_list[i]=link_pos_list_temp[i];

					}
				}

				len_array=nb_of_line1;
				pfrom1_arraylen=pos4;
				nfrom1_arraylen=pos2;
				nto1_arraylen=pos1;
				pto1_arraylen=pos3;
				lkinfo1_arraylen=pos5;
			}

		}
		nb_of_line=0;
		//std:://cout<<"out of the last loop and valid_path="<<valid_path<<std::endl;
		free(elt_idx_temp);
		free(elt_idx);
		if(elt_idx_temp1!=NULL)
			free(elt_idx_temp1);
		if(path_matrix_temp!=NULL)
			free(path_matrix_temp);
		if(path_end_old!=NULL)
			realloc(path_end_old,0*sizeof(int));
		if(path_end_new!=NULL)
			realloc(path_end_new,0*sizeof(int));
		if(path_matrix_old!=NULL )
			realloc(path_matrix_old,0*sizeof(int));	
		if(path_matrix_new!=NULL )
			realloc(path_matrix_new,0*sizeof(int));
		if(path_type_new!=NULL)
			path_type_new=(int*)realloc(path_type_new,0*sizeof(int));
		if(path_type!=NULL)
			path_type=(int*)realloc(path_type,0*sizeof(int));
		if(path_valid!=NULL )
		{
			realloc(path_valid,0*sizeof(int));
		}


		if(devtype_given==1 ||devtype_given==0)
			free(_devicetoList_possible);

		if(delete_connectivity==1)
		{
			for(p=0;p<max_rows_scd;p++)
			{
				free(pto_array_list_scd[p]);
				free(nto_array_list_scd[p]);
				free(pfrom_array_list_scd[p]);
				free(nfrom_array_list_scd[p]);
				free(lkinfo_array_list_scd[p]);

			}
			free(pto_array_list_scd);
			free(nto_array_list_scd);
			free(pfrom_array_list_scd);
			free(nfrom_array_list_scd);
			free(lkinfo_array_list_scd);
			if(lkid_list_scd!=NULL)
				free(lkid_list_scd);
			if(node_from_list_scd!=NULL)
				free(node_from_list_scd);
			if(node_to_list_scd!=NULL)
				free(node_to_list_scd);
			if(bidirectional_list_scd!=NULL)
				free(bidirectional_list_scd);
			if(lk_weight_list_scd!=NULL)
				free(lk_weight_list_scd);
			if(lktype_list_scd!=NULL)
				free(lktype_list_scd);
			if(lkused_list_scd!=NULL)
				free(lkused_list_scd);
		}
		if(link_pos_list_temp!=NULL)
			link_pos_list_temp=(int*)realloc(link_pos_list_temp,0*sizeof(int));
		if(lkinfo_list_temp!=NULL)
			lkinfo_list_temp=(char*)realloc(lkinfo_list_temp,0*sizeof(char));
		if(pathid_list_temp!=NULL)
			pathid_list_temp=(int*)realloc(pathid_list_temp,0*sizeof(int));
		if(nfrom1_list_temp!=NULL)
			free(nfrom1_list_temp);
		if(nto1_list_temp!=NULL)
			free(nto1_list_temp);
		if(pfrom1_list_temp!=NULL)
			free(pfrom1_list_temp);
		if(pfrom_list_temp!=NULL)
			free(pfrom_list_temp);
		if(pto1_list_temp!=NULL)
			free(pto1_list_temp);
		if(pto_list_temp!=NULL)
			free(pto_list_temp);


		if((strstr(ErrMessage,"BUFFER_TOO_SMALL"))==NULL)
		{
			if(valid_path>0)
			{
				GetErrorMess(appliName,"No_ERROR",ErrMessage,0);
				rescode=0;
			}
			else
			{
				GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrMessage,1);

				rescode=-1;
			}
		}

		//std:://cout<<"end of connec_cache "<<std::endl;	
		return rescode;
	}


	int GetAllPathsPerDevice_cache(char appliName[100],char* systemnameList,char* devname,int& len_array, int* lkid_list5,int* pathid_list,int* link_pos_list,int reload_connectivity,int delete_connectivity,char* ErrMessage)
	{

		int elt_value=0;
		int elt_idx_len=1000;
		int elt_idx_temp_len=1000;
		int* elt_idx=NULL;
		int* elt_idx_temp=NULL;
		int rescode=0;
		int err_len=1000;
		clock_t start,finish;
		double time;
		int nb_of_hop=23;
		int rescode1=0;
		int deviceid_to=0;
		int len_array_cpy=20000;
		int *path_matrix_old=NULL;
		int *path_matrix_new=NULL;
		int* path_matrix_temp=NULL;
		int path_matrix_old_len=0;
		int path_matrix_new_len=0;
		int *path_end_old=NULL;
		int *path_end_new=NULL;
		int* path_valid=NULL;
		int valid_path=0;
		int i=0;
		int j=0;
		int k=0;
		int v=0;
		int l=0;
		int elt_value_temp1=0;
		int elt_value_temp2=0;
		int stop=0;
		int nb_of_line=0;
		int* elt_idx_temp1=NULL;
		int elt_idx_temp1_len=1000;
		int m=0;
		int lk_pos=1;
		int* lkid_list6=NULL;
		int * pathid_list6=NULL;
		int* link_pos_list6=NULL;
		int pos1,pos2,pos3,pos4;
		int p=0;
		int _len_array_devto=20000;
		int* _devicetoList_possible=NULL;
		char ErrMess_temp[1000];
		//char ErrMess_temp[1000];
		int nb_of_line1=0;

		int elt_idx_int_temp1[100];
		int elt_idx_int_temp1_len=100;

		int linkid_int=0;
		int pfrom_int=0;
		int pto_int=0;
		char ptnb_from[20];
		char ptype_from[50];
		char ptnb_to[20];
		char ptype_to[50];
		int port_idx=0;
		int next_step=0;
		int rescode_int=0;
		//FILE * f;
		//char* filename="path_matrix.txt";
		if(reload_connectivity==1 || lkid_list_bis==NULL)
		{
			rescode=LoadConnectivityTable_InCache( systemnameList,devname,"none",2,1, ErrMess_temp);
			rescode_int=LoadInternalConnectivity_InCache( systemnameList,devname,"none",2,1, ErrMess_temp);

			if(rescode!=0)
			{
				GetErrorMess(appliName,ErrMess_temp,ErrMessage,1);
				return -1;
			}
			else
			{
				rescode=0;
			}	
		}
		elt_idx=(int*)malloc(elt_idx_len*sizeof(int));
		elt_idx_temp=(int*)malloc(elt_idx_temp_len*sizeof(int));
		if(elt_idx==NULL || elt_idx_temp==NULL)
		{

			for(i=0;i<max_rows_bis;i++)
			{
				free(pto_array_list_bis[i]);
				free(nto_array_list_bis[i]);
				free(pfrom_array_list_bis[i]);
				free(nfrom_array_list_bis[i]);

			}
			free(pto_array_list_bis);
			free(nto_array_list_bis);
			free(pfrom_array_list_bis);
			free(nfrom_array_list_bis);
			if(elt_idx!=NULL)
				free(elt_idx);
			if(elt_idx_temp!=NULL)
				free(elt_idx_temp);
			if(lkid_list_bis!=NULL)
				free(lkid_list_bis);
			if(node_from_list_bis!=NULL)
				free(node_from_list_bis);
			if(node_to_list_bis!=NULL)
				free(node_to_list_bis);
			if(bidirectional_list_bis!=NULL)
				free(bidirectional_list_bis);
			if(lk_weight_list_bis!=NULL)
				free(lk_weight_list_bis);
			if(lktype_list_bis!=NULL)
				free(lktype_list_bis);
			if(lkused_list_bis!=NULL)
				free(lkused_list_bis);
			GetErrorMess(appliName,"Unsuccessful malloc 3",ErrMessage,1);
			return -1;
		}
		i=0;
		int deviceid_from=0;
		rescode=GetIndexForTabCHAR_second(nfrom_array_list_bis,max_rows_bis, devname, elt_idx,elt_idx_len);
		if(rescode==-1)
		{
			if(delete_connectivity==1)
			{
				for(i=0;i<max_rows_bis;i++)
				{
					free(pto_array_list_bis[i]);
					free(nto_array_list_bis[i]);
					free(pfrom_array_list_bis[i]);
					free(nfrom_array_list_bis[i]);

				}
				free(pto_array_list_bis);
				free(nto_array_list_bis);
				free(pfrom_array_list_bis);
				free(nfrom_array_list_bis);
				if(lkid_list_bis!=NULL)
					free(lkid_list_bis);
				if(node_from_list_bis!=NULL)
					free(node_from_list_bis);
				if(node_to_list_bis!=NULL)
					free(node_to_list_bis);
				if(bidirectional_list_bis!=NULL)
					free(bidirectional_list_bis);
				if(lk_weight_list_bis!=NULL)
					free(lk_weight_list_bis);
				if(lktype_list_bis!=NULL)
					free(lktype_list_bis);
				if(lkused_list_bis!=NULL)
					free(lkused_list_bis);
			}
			free(elt_idx_temp);
			free(elt_idx);
			GetErrorMess(appliName,"DEVICE NOT CONNECTED",ErrMessage,1);

			return -1;
		}
		else
		{
			i=elt_idx[0];
			deviceid_from=node_from_list_bis[i];
		}
		rescode=GetIndexForTabINT(node_from_list_bis,len_array_gv_bis, deviceid_from, elt_idx,elt_idx_len,1);
		path_matrix_old=(int*)realloc(path_matrix_old,elt_idx_len*nb_of_hop*sizeof(int));
		path_end_new=(int*)realloc(path_end_new,elt_idx_len*sizeof(int));
		path_end_old=(int*)realloc(path_end_old,elt_idx_len*sizeof(int));
		path_matrix_temp=(int*)malloc((nb_of_hop+1)*sizeof(int));
		elt_idx_temp1=(int*)malloc(elt_idx_temp_len*sizeof(int));
		if(elt_idx_temp1==NULL||path_matrix_temp==NULL||path_end_old==NULL||path_end_new==NULL||path_matrix_old==NULL)
		{
			free(elt_idx_temp);
			free(elt_idx);
			if(elt_idx_temp1!=NULL)
				free(elt_idx_temp1);
			if(path_matrix_temp!=NULL)
				free(path_matrix_temp);
			if(path_end_old!=NULL)
				free(path_end_old);
			if(path_end_new!=NULL)
				free(path_end_new);
			if(path_matrix_old!=NULL )
			{
				free(path_matrix_old);
			}
			for(i=0;i<max_rows_bis;i++)
			{
				free(pto_array_list_bis[i]);
				free(nto_array_list_bis[i]);
				free(pfrom_array_list_bis[i]);
				free(nfrom_array_list_bis[i]);

			}
			free(pto_array_list_bis);
			free(nto_array_list_bis);
			free(pfrom_array_list_bis);
			free(nfrom_array_list_bis);
			if(lkid_list_bis!=NULL)
				free(lkid_list_bis);
			if(node_from_list_bis!=NULL)
				free(node_from_list_bis);
			if(node_to_list_bis!=NULL)
				free(node_to_list_bis);
			if(bidirectional_list_bis!=NULL)
				free(bidirectional_list_bis);
			if(lk_weight_list_bis!=NULL)
				free(lk_weight_list_bis);
			if(lktype_list_bis!=NULL)
				free(lktype_list_bis);
			if(lkused_list_bis!=NULL)
				free(lkused_list_bis);
			GetErrorMess(appliName,"Unsuccessful malloc 5",ErrMessage,1);
			return -1;
		}
		nb_of_line=3;
		for(i=0;i<elt_idx_len;i++)
		{
			for(j=0;j<nb_of_line;j++)
			{
				m=elt_idx[i];
				path_matrix_old[i*nb_of_hop+j]=node_from_list_bis[m];
				j++;
				path_matrix_old[i*nb_of_hop+j]=lkid_list_bis[m];
				j++;
				path_matrix_old[i*nb_of_hop+j]=node_to_list_bis[m];
				if(lk_weight_list_bis[m]==2)
				{
					path_end_old[i]=1;

				}
				else
				{
					path_end_old[i]=0;

				}
			}
		}
		i=0;
		j=3;
		nb_of_line=0;
		path_matrix_old_len=elt_idx_len;
		path_matrix_new_len=0;
		pos1=0;
		pos2=0;
		pos3=0;
		pos4=0;
		start = clock();
		while(stop==0 && j<nb_of_hop)
		{
			for(i=0;i<path_matrix_old_len;i++)
			{
				if(path_end_old[i]==0)
				{
					elt_value=path_matrix_old[i*nb_of_hop+(j-1)];
					rescode=GetIndexForTabINT(node_from_list_bis,len_array_gv_bis,elt_value , elt_idx_temp,elt_idx_temp_len,1);
					if(elt_idx_temp_len==0)
					{
						if(elt_idx_temp==0)
							path_end_old[i]=-1;
					}
					else
					{
						path_matrix_new_len+=elt_idx_temp_len; //nb of paths so far
						path_matrix_new=(int*)realloc(path_matrix_new,path_matrix_new_len*nb_of_hop*sizeof(int));
						path_end_new=(int*)realloc(path_end_new,path_matrix_new_len*sizeof(int));
						if(path_matrix_new==NULL || path_end_new==NULL)
						{
							if(path_matrix_old!=NULL)
								path_matrix_old=(int*)realloc(path_matrix_old,0*sizeof(int));
							if(path_matrix_new!=NULL)
								path_matrix_new=(int*)realloc(path_matrix_new,0*sizeof(int));
							if(path_end_new!=NULL)
								path_end_new=(int*)realloc(path_end_new,0*sizeof(int));
							if(path_end_old!=NULL)
								path_end_old=(int*)realloc(path_end_old,0*sizeof(int));
							if(path_valid==NULL)
								free(path_valid);
							free(elt_idx_temp);
							free(elt_idx);
							free(path_matrix_temp);
							if(elt_idx_temp1!=NULL)
								free(elt_idx_temp1);
							for(p=0;p<max_rows_bis;p++)
							{
								free(pto_array_list_bis[p]);
								free(nto_array_list_bis[p]);
								free(pfrom_array_list_bis[p]);
								free(nfrom_array_list_bis[p]);

							}
							free(pto_array_list_bis);
							free(nto_array_list_bis);
							free(pfrom_array_list_bis);
							free(nfrom_array_list_bis);
							if(lkid_list_bis!=NULL)
								free(lkid_list_bis);
							if(node_from_list_bis!=NULL)
								free(node_from_list_bis);
							if(node_to_list_bis!=NULL)
								free(node_to_list_bis);
							if(bidirectional_list_bis!=NULL)
								free(bidirectional_list_bis);
							if(lk_weight_list_bis!=NULL)
								free(lk_weight_list_bis);
							if(lktype_list_bis!=NULL)
								free(lktype_list_bis);
							if(lkused_list_bis!=NULL)
								free(lkused_list_bis);
							if(pathid_list6!=NULL)
								free(pathid_list6);
							if(link_pos_list6!=NULL)
								free(link_pos_list6);
							if(lkid_list6!=NULL)
								free(lkid_list6);
							GetErrorMess(appliName,"Unsuccessful malloc 6",ErrMessage,1);
							return -1;
						}
						else
						{
							for(l=0;l<j;l++)//copy of the corresponding path line to check loop
							{
								path_matrix_temp[l]=path_matrix_old[i*nb_of_hop+l];

							}
							//if(j==11)
							//std:://cout<<"copy of the corresponding path line to check loop"<<std::endl;

						}
						for(k=0;k<elt_idx_temp_len;k++)
						{
							elt_value_temp1=elt_idx_temp[k];
							elt_value_temp2=node_to_list_bis[elt_value_temp1];
							rescode=GetIndexForTabINT(path_matrix_temp,j,elt_value_temp2 , elt_idx_temp1,elt_idx_temp1_len,2);			
							finish=clock();
							time = (double(finish)-double(start))/CLOCKS_PER_SEC;  
							if(time>TIMEOUT_DEFINED)
							{
								if(path_matrix_old!=NULL)
									path_matrix_old=(int*)realloc(path_matrix_old,0*sizeof(int));
								if(path_matrix_new!=NULL)
									path_matrix_new=(int*)realloc(path_matrix_new,0*sizeof(int));
								if(path_end_new!=NULL)
									path_end_new=(int*)realloc(path_end_new,0*sizeof(int));
								if(path_end_old!=NULL)
									path_end_old=(int*)realloc(path_end_old,0*sizeof(int));
								if(path_valid==NULL)
									free(path_valid);
								free(elt_idx_temp);
								free(elt_idx);
								free(path_matrix_temp);
								if(elt_idx_temp1!=NULL)
									free(elt_idx_temp1);
								for(p=0;p<max_rows_bis;p++)
								{
									free(pto_array_list_bis[p]);
									free(nto_array_list_bis[p]);
									free(pfrom_array_list_bis[p]);
									free(nfrom_array_list_bis[p]);
								}
								free(pto_array_list_bis);
								free(nto_array_list_bis);
								free(pfrom_array_list_bis);
								free(nfrom_array_list_bis);
								if(lkid_list_bis!=NULL)
									free(lkid_list_bis);
								if(node_from_list_bis!=NULL)
									free(node_from_list_bis);
								if(node_to_list_bis!=NULL)
									free(node_to_list_bis);
								if(bidirectional_list_bis!=NULL)
									free(bidirectional_list_bis);
								if(lk_weight_list_bis!=NULL)
									free(lk_weight_list_bis);
								if(lktype_list_bis!=NULL)
									free(lktype_list_bis);
								if(lkused_list_bis!=NULL)
									free(lkused_list_bis);
								if(pathid_list6!=NULL)
									free(pathid_list6);
								if(link_pos_list6!=NULL)
									free(link_pos_list6);
								if(lkid_list6!=NULL)
									free(lkid_list6);
								GetErrorMess(appliName,"CONNECTIVITY TOO COMPLEX, EXECUTION TIME TOO LONG",ErrMessage,1);
								return -1;
							}

							if(rescode==-1 ||elt_idx_temp1_len==0)
							{

								if(len_array_gv_bis_internal>0)
								{
									rescode=GetIndexForTabINT(node_from_list_bis_internal,len_array_gv_bis_internal,elt_value , elt_idx_int_temp1,elt_idx_int_temp1_len,1);
									if(elt_idx_int_temp1_len>0)
									{
										linkid_int=path_matrix_old[i*nb_of_hop+(j-2)];
										rescode=GetIndexForTabINT(lkid_list_bis,len_array_gv_bis,linkid_int , elt_idx_int_temp1,elt_idx_int_temp1_len,1);
										port_idx=elt_idx_int_temp1[0];
										rescode=DecomposePort(pfrom_array_list_bis[elt_value_temp1],strlen(pfrom_array_list_bis[elt_value_temp1]),  ptnb_from, ptype_from);
										rescode+=DecomposePort(pto_array_list_bis[port_idx],strlen(pto_array_list_bis[port_idx]),  ptnb_to, ptype_to);
										if(linkid_int>0)
										{
											rescode+=GetPortID_portinfo(elt_value,ptnb_from,ptype_from,1,pfrom_int,ErrMess_temp);
											rescode+=GetPortID_portinfo(elt_value,ptnb_to,ptype_to,2,pto_int,ErrMess_temp);
											rescode+=CheckInternalConnectivity(pfrom_int, pto_int,ErrMess_temp);

										}
										else
										{
											rescode+=GetPortID_portinfo(elt_value,ptnb_from,ptype_from,2,pfrom_int,ErrMess_temp);
											rescode+=GetPortID_portinfo(elt_value,ptnb_to,ptype_to,1,pto_int,ErrMess_temp);
											rescode+=CheckInternalConnectivity(pto_int, pfrom_int,ErrMess_temp);

										}	
										if(rescode==0)//then it's OK link allowed
											next_step=0;
										else
											next_step=-1;
									}
									else
										next_step=0;

								}
								else
									next_step=0;//measn it's OK we go to the next step

								if(next_step==0)
								{
									for(m=0;m<j;m++)
										path_matrix_new[nb_of_line*nb_of_hop+m]=path_matrix_old[i*nb_of_hop+m];	
									path_matrix_new[nb_of_line*nb_of_hop+j]=lkid_list_bis[elt_value_temp1];
									path_matrix_new[nb_of_line*nb_of_hop+j+1]=elt_value_temp2;
									if(lk_weight_list_bis[elt_value_temp1]==2)
									{
										path_end_new[nb_of_line]=1;
									}
									else
									{
										if(lk_weight_list_bis[elt_value_temp1]==0)
											path_end_new[nb_of_line]=0;
										else
											path_end_new[nb_of_line]=-1;
									}
								}
								else
								{
									path_end_new[nb_of_line]=-1;
								}
								nb_of_line++;
							}
							else
							{
								path_end_new[nb_of_line]=-1;
								nb_of_line++;
							}
						}
					}
				}
				else
				{
					if(path_end_old[i]==1)
					{
						path_valid=(int*)realloc(path_valid,(valid_path+1)*nb_of_hop*sizeof(int));
						if(path_valid==NULL)
						{
							if(path_matrix_old!=NULL)
								path_matrix_old=(int*)realloc(path_matrix_old,0*sizeof(int));
							if(path_matrix_new!=NULL)
								path_matrix_new=(int*)realloc(path_matrix_new,0*sizeof(int));
							if(path_end_new!=NULL)
								path_end_new=(int*)realloc(path_end_new,0*sizeof(int));
							if(path_end_old!=NULL)
								path_end_old=(int*)realloc(path_end_old,0*sizeof(int));
							free(elt_idx_temp);
							free(path_matrix_temp);
							if(elt_idx_temp1!=NULL)
								free(elt_idx_temp1);
							free(elt_idx);
							for(p=0;p<max_rows_bis;p++)
							{
								free(pto_array_list_bis[p]);
								free(nto_array_list_bis[p]);
								free(pfrom_array_list_bis[p]);
								free(nfrom_array_list_bis[p]);

							}
							free(pto_array_list_bis);
							free(nto_array_list_bis);
							free(pfrom_array_list_bis);
							free(nfrom_array_list_bis);
							if(lkid_list_bis!=NULL)
								free(lkid_list_bis);
							if(node_from_list_bis!=NULL)
								free(node_from_list_bis);
							if(node_to_list_bis!=NULL)
								free(node_to_list_bis);
							if(bidirectional_list_bis!=NULL)
								free(bidirectional_list_bis);
							if(lk_weight_list_bis!=NULL)
								free(lk_weight_list_bis);
							if(lktype_list_bis!=NULL)
								free(lktype_list_bis);
							if(lkused_list_bis!=NULL)
								free(lkused_list_bis);
							if(pathid_list6!=NULL)
								free(pathid_list6);
							if(link_pos_list6!=NULL)
								free(link_pos_list6);
							if(lkid_list6!=NULL)
								free(lkid_list6);
							GetErrorMess(appliName,"Unsuccessful malloc 6",ErrMessage,1);
							return -1;

						}
						lk_pos=1;
						pathid_list6=(int*)realloc(pathid_list6,(nb_of_line1+j)*sizeof(int));
						link_pos_list6=(int*)realloc(link_pos_list6,(nb_of_line1+j)*sizeof(int));
						lkid_list6=(int*)realloc(lkid_list6,(nb_of_line1+j)*sizeof(int));
						if(pathid_list6==NULL||link_pos_list6==NULL||lkid_list6==NULL)
						{
							if(path_matrix_old!=NULL)
								path_matrix_old=(int*)realloc(path_matrix_old,0*sizeof(int));
							if(path_matrix_new!=NULL)
								path_matrix_new=(int*)realloc(path_matrix_new,0*sizeof(int));
							if(path_end_new!=NULL)
								path_end_new=(int*)realloc(path_end_new,0*sizeof(int));
							if(path_end_old!=NULL)
								path_end_old=(int*)realloc(path_end_old,0*sizeof(int));
							free(elt_idx_temp);
							free(path_matrix_temp);
							if(elt_idx_temp1!=NULL)
								free(elt_idx_temp1);
							free(elt_idx);
							for(p=0;p<max_rows_bis;p++)
							{
								free(pto_array_list_bis[p]);
								free(nto_array_list_bis[p]);
								free(pfrom_array_list_bis[p]);
								free(nfrom_array_list_bis[p]);

							}
							free(pto_array_list_bis);
							free(nto_array_list_bis);
							free(pfrom_array_list_bis);
							free(nfrom_array_list_bis);
							if(lkid_list_bis!=NULL)
								free(lkid_list_bis);
							if(node_from_list_bis!=NULL)
								free(node_from_list_bis);
							if(node_to_list_bis!=NULL)
								free(node_to_list_bis);
							if(bidirectional_list_bis!=NULL)
								free(bidirectional_list_bis);
							if(lk_weight_list_bis!=NULL)
								free(lk_weight_list_bis);
							if(lktype_list_bis!=NULL)
								free(lktype_list_bis);
							if(lkused_list_bis!=NULL)
								free(lkused_list_bis);
							if(pathid_list6!=NULL)
								free(pathid_list6);
							if(link_pos_list6!=NULL)
								free(link_pos_list6);
							if(lkid_list6!=NULL)
								free(lkid_list6);
							GetErrorMess(appliName,"Unsuccessful malloc 7",ErrMessage,1);
							return -1;
						}
						for(m=1;m<j;m++)
						{
							path_valid[valid_path*nb_of_hop+m]=path_matrix_old[i*nb_of_hop+m];	
							rescode=GetIndexForTabINT(lkid_list_bis,len_array_gv_bis,path_valid[valid_path*nb_of_hop+m] , elt_idx_temp1,elt_idx_temp1_len,1);		
							pathid_list6[nb_of_line1]=valid_path;
							link_pos_list6[nb_of_line1]=lk_pos;
							elt_value=elt_idx_temp1[0];//necessary one possibility
							lkid_list6[nb_of_line1]=lkid_list_bis[elt_value];
							nb_of_line1++;
							lk_pos++;
							m++;
						}
						valid_path++;
					}

				}
			}
			j=j+2;
			path_matrix_old=(int*)realloc(path_matrix_old,nb_of_line*nb_of_hop*sizeof(int));
			path_end_old=(int*)realloc(path_end_old,nb_of_line*sizeof(int));
			CopyMatrix(path_matrix_old,path_matrix_new,nb_of_line,j,nb_of_line,0,nb_of_hop); //pure copy of the matrix
			CopyVector(path_end_old,path_end_new,nb_of_line,nb_of_line,0);//copy + element shift
			path_matrix_old_len=nb_of_line;
			stop=ComputePathWeight(path_end_new,nb_of_line);
			stop+=ComputePathWeight(path_end_old,path_matrix_old_len);
			path_matrix_new_len=0;
			nb_of_line=0;
			//std:://cout<<"j="<<j<<" and stop="<<stop<<std::endl;
		}	
		//std:://cout<<"out of the loop"<<std::endl;
		if(stop>0) // get the last valid path of the loop
		{
			for(i=0;i<path_matrix_old_len;i++)
			{
				if(path_end_old[i]==1)
				{
					path_valid=(int*)realloc(path_valid,(valid_path+1)*nb_of_hop*sizeof(int));
					if(path_valid==NULL)
					{
						if(path_matrix_old!=NULL)
							path_matrix_old=(int*)realloc(path_matrix_old,0*sizeof(int));
						if(path_matrix_new!=NULL)
							path_matrix_new=(int*)realloc(path_matrix_new,0*sizeof(int));
						if(path_end_new!=NULL)
							path_end_new=(int*)realloc(path_end_new,0*sizeof(int));
						if(path_end_old!=NULL)
							path_end_old=(int*)realloc(path_end_old,0*sizeof(int));
						free(elt_idx_temp);
						free(elt_idx);
						free(path_matrix_temp);
						if(elt_idx_temp1!=NULL)
							free(elt_idx_temp1);
						for(i=0;i<max_rows_bis;i++)
						{
							free(pto_array_list_bis[i]);
							free(nto_array_list_bis[i]);
							free(pfrom_array_list_bis[i]);
							free(nfrom_array_list_bis[i]);

						}
						free(pto_array_list_bis);
						free(nto_array_list_bis);
						free(pfrom_array_list_bis);
						free(nfrom_array_list_bis);
						if(lkid_list_bis!=NULL)
							free(lkid_list_bis);
						if(node_from_list_bis!=NULL)
							free(node_from_list_bis);
						if(node_to_list_bis!=NULL)
							free(node_to_list_bis);
						if(bidirectional_list_bis!=NULL)
							free(bidirectional_list_bis);
						if(lk_weight_list_bis!=NULL)
							free(lk_weight_list_bis);
						if(lktype_list_bis!=NULL)
							free(lktype_list_bis);
						if(lkused_list_bis!=NULL)
							free(lkused_list_bis);
						if(pathid_list6!=NULL)
							free(pathid_list6);
						if(link_pos_list6!=NULL)
							free(link_pos_list6);
						if(lkid_list6!=NULL)
							free(lkid_list6);
						GetErrorMess(appliName,"Unsuccessful malloc 6",ErrMessage,1);
						return -1;

					}
					lk_pos=1;
					pathid_list6=(int*)realloc(pathid_list6,(nb_of_line1+j)*sizeof(int));
					link_pos_list6=(int*)realloc(link_pos_list6,(nb_of_line1+j)*sizeof(int));
					lkid_list6=(int*)realloc(lkid_list6,(nb_of_line1+j)*sizeof(int));
					if(pathid_list6==NULL||link_pos_list6==NULL||lkid_list6==NULL)
					{
						if(path_matrix_old!=NULL)
							path_matrix_old=(int*)realloc(path_matrix_old,0*sizeof(int));
						if(path_matrix_new!=NULL)
							path_matrix_new=(int*)realloc(path_matrix_new,0*sizeof(int));
						if(path_end_new!=NULL)
							path_end_new=(int*)realloc(path_end_new,0*sizeof(int));
						if(path_end_old!=NULL)
							path_end_old=(int*)realloc(path_end_old,0*sizeof(int));
						free(elt_idx_temp);
						free(path_matrix_temp);
						if(elt_idx_temp1!=NULL)
							free(elt_idx_temp1);
						free(elt_idx);
						for(p=0;p<max_rows_bis;p++)
						{
							free(pto_array_list_bis[p]);
							free(nto_array_list_bis[p]);
							free(pfrom_array_list_bis[p]);
							free(nfrom_array_list_bis[p]);
						}
						free(pto_array_list_bis);
						free(nto_array_list_bis);
						free(pfrom_array_list_bis);
						free(nfrom_array_list_bis);
						if(lkid_list_bis!=NULL)
							free(lkid_list_bis);
						if(node_from_list_bis!=NULL)
							free(node_from_list_bis);
						if(node_to_list_bis!=NULL)
							free(node_to_list_bis);
						if(bidirectional_list_bis!=NULL)
							free(bidirectional_list_bis);
						if(lk_weight_list_bis!=NULL)
							free(lk_weight_list_bis);
						if(lktype_list_bis!=NULL)
							free(lktype_list_bis);
						if(lkused_list_bis!=NULL)
							free(lkused_list_bis);
						if(pathid_list6!=NULL)
							free(pathid_list6);
						if(link_pos_list6!=NULL)
							free(link_pos_list6);
						if(lkid_list6!=NULL)
							free(lkid_list6);
						GetErrorMess(appliName,"Unsuccessful malloc 7",ErrMessage,1);
						return -1;
					}
					for(m=1;m<j;m++)
					{
						path_valid[valid_path*nb_of_hop+m]=path_matrix_old[i*nb_of_hop+m];
						rescode=GetIndexForTabINT(lkid_list_bis,len_array_gv_bis,path_valid[valid_path*nb_of_hop+m] , elt_idx_temp1,elt_idx_temp1_len,1);
						pathid_list6[nb_of_line1]=valid_path;
						link_pos_list6[nb_of_line1]=lk_pos;
						elt_value=elt_idx_temp1[0];//necessary one possibility
						lkid_list6[nb_of_line1]=lkid_list_bis[elt_value];
						nb_of_line1++;
						lk_pos++;
						m++;	
					}
					valid_path++;
				}
			}
			stop=j;

			if(len_array<nb_of_line1)
			{
				GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrMessage,1);
				len_array=nb_of_line1;
				rescode=-1;
			}
			else
			{
				for(i=0;i<nb_of_line1;i++)
				{
					pathid_list[i]=pathid_list6[i];
					link_pos_list[i]=link_pos_list6[i];
					lkid_list5[i]=lkid_list6[i];
				}
				len_array=nb_of_line1;
			}



		}

		//fclose(f);
		nb_of_line=0;
		//std:://cout<<"out of the last loop and valid_opath="<<valid_path<<std::endl;
		free(elt_idx_temp);
		free(elt_idx);
		if(pathid_list6!=NULL)
			free(pathid_list6);
		if(link_pos_list6!=NULL)
			free(link_pos_list6);
		if(lkid_list6!=NULL)
			free(lkid_list6);
		if(elt_idx_temp1!=NULL)
			free(elt_idx_temp1);
		if(path_matrix_temp!=NULL)
			free(path_matrix_temp);
		if(path_end_old!=NULL)
			realloc(path_end_old,0*sizeof(int));
		if(path_end_new!=NULL)
			realloc(path_end_new,0*sizeof(int));
		if(path_matrix_old!=NULL )
			realloc(path_matrix_old,0*sizeof(int));	
		if(path_matrix_new!=NULL )
			realloc(path_matrix_new,0*sizeof(int));
		if(path_valid!=NULL )
		{
			realloc(path_valid,0*sizeof(int));
		}
		if(delete_connectivity==1)
		{
			for(i=0;i<max_rows_bis;i++)
			{
				free(pto_array_list_bis[i]);
				free(nto_array_list_bis[i]);
				free(pfrom_array_list_bis[i]);
				free(nfrom_array_list_bis[i]);

			}
			free(pto_array_list_bis);
			free(nto_array_list_bis);
			free(pfrom_array_list_bis);
			free(nfrom_array_list_bis);
			if(lkid_list_bis!=NULL)
				free(lkid_list_bis);
			if(node_from_list_bis!=NULL)
				free(node_from_list_bis);
			if(node_to_list_bis!=NULL)
				free(node_to_list_bis);
			if(bidirectional_list_bis!=NULL)
				free(bidirectional_list_bis);
			if(lk_weight_list_bis!=NULL)
				free(lk_weight_list_bis);
			if(lktype_list_bis!=NULL)
				free(lktype_list_bis);
			if(lkused_list_bis!=NULL)
				free(lkused_list_bis);
		}
		if((strstr(ErrMessage,"BUFFER_TOO_SMALL"))==NULL)
		{
			if(valid_path>0)
			{
				GetErrorMess(appliName,"No_ERROR",ErrMessage,0);
				rescode=0;
			}
			else
			{
				GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrMessage,1);
				rescode=-1;
			}
		}

		//std:://cout<<"before returning "<<std::endl;	
		return rescode;
	}

	/*****************************************************************************************/

	int GetAllPathsPerDevice_cache_reverse(char appliName[100],char* systemnameList,char* devname,int& len_array, int* lkid_list5,int* pathid_list,int* link_pos_list,int reload_connectivity,int delete_connectivity,char* ErrMessage)
	{

		int elt_value=0;
		int elt_idx_len=1000;
		int elt_idx_temp_len=1000;
		int* elt_idx=NULL;
		int* elt_idx_temp=NULL;
		int rescode=0;
		int err_len=1000;
		clock_t start,finish;
		double time;
		int nb_of_hop=23;
		int rescode1=0;
		int deviceid_to=0;
		int len_array_cpy=20000;
		int *path_matrix_old=NULL;
		int *path_matrix_new=NULL;
		int* path_matrix_temp=NULL;
		int path_matrix_old_len=0;
		int path_matrix_new_len=0;
		int *path_end_old=NULL;
		int *path_end_new=NULL;
		int* path_valid=NULL;
		int valid_path=0;
		int i=0;
		int j=0;
		int k=0;
		int v=0;
		int l=0;
		int elt_value_temp1=0;
		int elt_value_temp2=0;
		int stop=0;
		int nb_of_line=0;
		int* elt_idx_temp1=NULL;
		int elt_idx_temp1_len=1000;
		int m=0;
		int lk_pos=1;
		int* lkid_list6=NULL;
		int * pathid_list6=NULL;
		int* link_pos_list6=NULL;
		int pos1,pos2,pos3,pos4;
		int p=0;
		int _len_array_devto=20000;
		int* _devicetoList_possible=NULL;
		char ErrMess_temp[1000];
		int nb_of_line1=0;
		int elt_idx_int_temp1[100];
		int elt_idx_int_temp1_len=100;

		int linkid_int=0;
		int pfrom_int=0;
		int pto_int=0;
		char ptnb_from[20];
		char ptype_from[50];
		char ptnb_to[20];
		char ptype_to[50];
		int port_idx=0;
		int next_step=0;
		int rescode_int=0;
		//FILE * f;
		//char* filename="path_matrix.txt";
		if(reload_connectivity==1 || lkid_list_bis_rev==NULL)
		{
			rescode=LoadConnectivityTable_InCache( systemnameList,devname,"none",3,1, ErrMess_temp);
			rescode_int=LoadInternalConnectivity_InCache( systemnameList,devname,"none",3,1, ErrMess_temp);
			if(rescode!=0)
			{
				GetErrorMess(appliName,ErrMess_temp,ErrMessage,1);
				return -1;
			}
			else
			{
				rescode=0;
			}	
		}
		elt_idx=(int*)malloc(elt_idx_len*sizeof(int));
		elt_idx_temp=(int*)malloc(elt_idx_temp_len*sizeof(int));
		if(elt_idx==NULL || elt_idx_temp==NULL)
		{

			for(i=0;i<max_rows_bis_rev;i++)
			{
				free(pto_array_list_bis_rev[i]);
				free(nto_array_list_bis_rev[i]);
				free(pfrom_array_list_bis_rev[i]);
				free(nfrom_array_list_bis_rev[i]);

			}
			free(pto_array_list_bis_rev);
			free(nto_array_list_bis_rev);
			free(pfrom_array_list_bis_rev);
			free(nfrom_array_list_bis_rev);
			if(elt_idx!=NULL)
				free(elt_idx);
			if(elt_idx_temp!=NULL)
				free(elt_idx_temp);
			if(lkid_list_bis_rev!=NULL)
				free(lkid_list_bis_rev);
			if(node_from_list_bis_rev!=NULL)
				free(node_from_list_bis_rev);
			if(node_to_list_bis_rev!=NULL)
				free(node_to_list_bis_rev);
			if(bidirectional_list_bis_rev!=NULL)
				free(bidirectional_list_bis_rev);
			if(lk_weight_list_bis_rev!=NULL)
				free(lk_weight_list_bis_rev);
			if(lktype_list_bis_rev!=NULL)
				free(lktype_list_bis_rev);
			if(lkused_list_bis_rev!=NULL)
				free(lkused_list_bis_rev);
			GetErrorMess(appliName,"Unsuccessful malloc 3",ErrMessage,1);
			return -1;
		}
		i=0;
		int deviceid_from=0;
		rescode=GetIndexForTabCHAR_second(nfrom_array_list_bis_rev,max_rows_bis_rev, devname, elt_idx,elt_idx_len);
		if(rescode==-1)
		{
			if(delete_connectivity==1)
			{
				for(i=0;i<max_rows_bis_rev;i++)
				{
					free(pto_array_list_bis_rev[i]);
					free(nto_array_list_bis_rev[i]);
					free(pfrom_array_list_bis_rev[i]);
					free(nfrom_array_list_bis_rev[i]);

				}
				free(pto_array_list_bis_rev);
				free(nto_array_list_bis_rev);
				free(pfrom_array_list_bis_rev);
				free(nfrom_array_list_bis_rev);
				if(lkid_list_bis_rev!=NULL)
					free(lkid_list_bis_rev);
				if(node_from_list_bis_rev!=NULL)
					free(node_from_list_bis_rev);
				if(node_to_list_bis_rev!=NULL)
					free(node_to_list_bis_rev);
				if(bidirectional_list_bis_rev!=NULL)
					free(bidirectional_list_bis_rev);
				if(lk_weight_list_bis_rev!=NULL)
					free(lk_weight_list_bis_rev);
				if(lktype_list_bis_rev!=NULL)
					free(lktype_list_bis_rev);
				if(lkused_list_bis_rev!=NULL)
					free(lkused_list_bis_rev);
			}
			free(elt_idx_temp);
			free(elt_idx);
			GetErrorMess(appliName,"DEVICE NOT CONNECTED",ErrMessage,1);
			return -1;
		}
		else
		{
			i=elt_idx[0];
			deviceid_from=node_from_list_bis_rev[i];
		}
		rescode=GetIndexForTabINT(node_from_list_bis_rev,len_array_gv_bis_rev, deviceid_from, elt_idx,elt_idx_len,1);
		path_matrix_old=(int*)realloc(path_matrix_old,elt_idx_len*nb_of_hop*sizeof(int));
		path_end_new=(int*)realloc(path_end_new,elt_idx_len*sizeof(int));
		path_end_old=(int*)realloc(path_end_old,elt_idx_len*sizeof(int));
		path_matrix_temp=(int*)malloc((nb_of_hop+1)*sizeof(int));
		elt_idx_temp1=(int*)malloc(elt_idx_temp_len*sizeof(int));
		if(elt_idx_temp1==NULL||path_matrix_temp==NULL||path_end_old==NULL||path_end_new==NULL||path_matrix_old==NULL)
		{
			free(elt_idx_temp);
			free(elt_idx);
			if(elt_idx_temp1!=NULL)
				free(elt_idx_temp1);
			if(path_matrix_temp!=NULL)
				free(path_matrix_temp);
			if(path_end_old!=NULL)
				free(path_end_old);
			if(path_end_new!=NULL)
				free(path_end_new);
			if(path_matrix_old!=NULL )
			{
				free(path_matrix_old);
			}
			for(i=0;i<max_rows_bis_rev;i++)
			{
				free(pto_array_list_bis_rev[i]);
				free(nto_array_list_bis_rev[i]);
				free(pfrom_array_list_bis_rev[i]);
				free(nfrom_array_list_bis_rev[i]);

			}
			free(pto_array_list_bis_rev);
			free(nto_array_list_bis_rev);
			free(pfrom_array_list_bis_rev);
			free(nfrom_array_list_bis_rev);
			if(lkid_list_bis_rev!=NULL)
				free(lkid_list_bis_rev);
			if(node_from_list_bis_rev!=NULL)
				free(node_from_list_bis_rev);
			if(node_to_list_bis_rev!=NULL)
				free(node_to_list_bis_rev);
			if(bidirectional_list_bis_rev!=NULL)
				free(bidirectional_list_bis_rev);
			if(lk_weight_list_bis_rev!=NULL)
				free(lk_weight_list_bis_rev);
			if(lktype_list_bis_rev!=NULL)
				free(lktype_list_bis_rev);
			if(lkused_list_bis_rev!=NULL)
				free(lkused_list_bis_rev);
			GetErrorMess(appliName,"Unsuccessful malloc 5",ErrMessage,1);
			return -1;
		}
		nb_of_line=3;
		for(i=0;i<elt_idx_len;i++)
		{
			for(j=0;j<nb_of_line;j++)
			{
				m=elt_idx[i];
				path_matrix_old[i*nb_of_hop+j]=node_from_list_bis_rev[m];
				j++;
				path_matrix_old[i*nb_of_hop+j]=lkid_list_bis_rev[m];
				j++;
				path_matrix_old[i*nb_of_hop+j]=node_to_list_bis_rev[m];
				if(lk_weight_list_bis_rev[m]==2)
				{
					path_end_old[i]=1;

				}
				else
				{
					path_end_old[i]=0;

				}
			}
		}
		i=0;
		j=3;
		nb_of_line=0;
		path_matrix_old_len=elt_idx_len;
		path_matrix_new_len=0;
		pos1=0;
		pos2=0;
		pos3=0;
		pos4=0;
		start = clock();
		while(stop==0 && j<nb_of_hop)
		{
			for(i=0;i<path_matrix_old_len;i++)
			{
				if(path_end_old[i]==0)
				{
					elt_value=path_matrix_old[i*nb_of_hop+(j-1)];
					rescode=GetIndexForTabINT(node_from_list_bis_rev,len_array_gv_bis_rev,elt_value , elt_idx_temp,elt_idx_temp_len,1);
					if(elt_idx_temp_len==0)
					{
						if(elt_idx_temp==0)
							path_end_old[i]=-1;
					}
					else
					{
						path_matrix_new_len+=elt_idx_temp_len; //nb of paths so far
						path_matrix_new=(int*)realloc(path_matrix_new,path_matrix_new_len*nb_of_hop*sizeof(int));
						path_end_new=(int*)realloc(path_end_new,path_matrix_new_len*sizeof(int));
						if(path_matrix_new==NULL || path_end_new==NULL)
						{
							if(path_matrix_old!=NULL)
								path_matrix_old=(int*)realloc(path_matrix_old,0*sizeof(int));
							if(path_matrix_new!=NULL)
								path_matrix_new=(int*)realloc(path_matrix_new,0*sizeof(int));
							if(path_end_new!=NULL)
								path_end_new=(int*)realloc(path_end_new,0*sizeof(int));
							if(path_end_old!=NULL)
								path_end_old=(int*)realloc(path_end_old,0*sizeof(int));
							if(path_valid==NULL)
								free(path_valid);
							free(elt_idx_temp);
							free(elt_idx);
							free(path_matrix_temp);
							if(elt_idx_temp1!=NULL)
								free(elt_idx_temp1);
							for(p=0;p<max_rows_bis_rev;p++)
							{
								free(pto_array_list_bis_rev[p]);
								free(nto_array_list_bis_rev[p]);
								free(pfrom_array_list_bis_rev[p]);
								free(nfrom_array_list_bis_rev[p]);

							}
							free(pto_array_list_bis_rev);
							free(nto_array_list_bis_rev);
							free(pfrom_array_list_bis_rev);
							free(nfrom_array_list_bis_rev);
							if(lkid_list_bis_rev!=NULL)
								free(lkid_list_bis_rev);
							if(node_from_list_bis_rev!=NULL)
								free(node_from_list_bis_rev);
							if(node_to_list_bis_rev!=NULL)
								free(node_to_list_bis_rev);
							if(bidirectional_list_bis_rev!=NULL)
								free(bidirectional_list_bis_rev);
							if(lk_weight_list_bis_rev!=NULL)
								free(lk_weight_list_bis_rev);
							if(lktype_list_bis_rev!=NULL)
								free(lktype_list_bis_rev);
							if(lkused_list_bis_rev!=NULL)
								free(lkused_list_bis_rev);
							if(pathid_list6!=NULL)
								free(pathid_list6);
							if(link_pos_list6!=NULL)
								free(link_pos_list6);
							if(lkid_list6!=NULL)
								free(lkid_list6);
							GetErrorMess(appliName,"Unsuccessful malloc 6",ErrMessage,1);
							return -1;
						}
						else
						{
							for(l=0;l<j;l++)//copy of the corresponding path line to check loop
							{
								path_matrix_temp[l]=path_matrix_old[i*nb_of_hop+l];

							}
							//if(j==11)
							//std:://cout<<"copy of the corresponding path line to check loop"<<std::endl;

						}
						for(k=0;k<elt_idx_temp_len;k++)
						{
							elt_value_temp1=elt_idx_temp[k];
							elt_value_temp2=node_to_list_bis_rev[elt_value_temp1];
							rescode=GetIndexForTabINT(path_matrix_temp,j,elt_value_temp2 , elt_idx_temp1,elt_idx_temp1_len,2);			
							finish=clock();
							time = (double(finish)-double(start))/CLOCKS_PER_SEC;  
							if(time>TIMEOUT_DEFINED)
							{
								if(path_matrix_old!=NULL)
									path_matrix_old=(int*)realloc(path_matrix_old,0*sizeof(int));
								if(path_matrix_new!=NULL)
									path_matrix_new=(int*)realloc(path_matrix_new,0*sizeof(int));
								if(path_end_new!=NULL)
									path_end_new=(int*)realloc(path_end_new,0*sizeof(int));
								if(path_end_old!=NULL)
									path_end_old=(int*)realloc(path_end_old,0*sizeof(int));
								if(path_valid==NULL)
									free(path_valid);
								free(elt_idx_temp);
								free(elt_idx);
								free(path_matrix_temp);
								if(elt_idx_temp1!=NULL)
									free(elt_idx_temp1);
								for(p=0;p<max_rows_bis_rev;p++)
								{
									free(pto_array_list_bis_rev[p]);
									free(nto_array_list_bis_rev[p]);
									free(pfrom_array_list_bis_rev[p]);
									free(nfrom_array_list_bis_rev[p]);
								}
								free(pto_array_list_bis_rev);
								free(nto_array_list_bis_rev);
								free(pfrom_array_list_bis_rev);
								free(nfrom_array_list_bis_rev);
								if(lkid_list_bis_rev!=NULL)
									free(lkid_list_bis_rev);
								if(node_from_list_bis_rev!=NULL)
									free(node_from_list_bis_rev);
								if(node_to_list_bis_rev!=NULL)
									free(node_to_list_bis_rev);
								if(bidirectional_list_bis_rev!=NULL)
									free(bidirectional_list_bis_rev);
								if(lk_weight_list_bis_rev!=NULL)
									free(lk_weight_list_bis_rev);
								if(lktype_list_bis_rev!=NULL)
									free(lktype_list_bis_rev);
								if(lkused_list_bis_rev!=NULL)
									free(lkused_list_bis_rev);
								if(pathid_list6!=NULL)
									free(pathid_list6);
								if(link_pos_list6!=NULL)
									free(link_pos_list6);
								if(lkid_list6!=NULL)
									free(lkid_list6);
								GetErrorMess(appliName,"CONNECTIVITY TOO COMPLEX, EXECUTION TIME TOO LONG",ErrMessage,1);
								return -1;
							}
							if(rescode==-1 ||elt_idx_temp1_len==0)
							{

								if(len_array_gv_bis_rev_internal>0)
								{
									rescode=GetIndexForTabINT(node_from_list_bis_rev_internal,len_array_gv_bis_rev_internal,elt_value , elt_idx_int_temp1,elt_idx_int_temp1_len,1);
									if(elt_idx_int_temp1_len>0)
									{
										linkid_int=path_matrix_old[i*nb_of_hop+(j-2)];
										rescode=GetIndexForTabINT(lkid_list_bis_rev,len_array_gv_bis_rev,linkid_int , elt_idx_int_temp1,elt_idx_int_temp1_len,1);
										port_idx=elt_idx_int_temp1[0];
										rescode=DecomposePort(pfrom_array_list_bis_rev[elt_value_temp1],strlen(pfrom_array_list_bis_rev[elt_value_temp1]),  ptnb_from, ptype_from);
										rescode+=DecomposePort(pto_array_list_bis_rev[port_idx],strlen(pto_array_list_bis_rev[port_idx]),  ptnb_to, ptype_to);
										if(linkid_int>0)
										{
											rescode+=GetPortID_portinfo(elt_value,ptnb_from,ptype_from,1,pfrom_int,ErrMess_temp);
											rescode+=GetPortID_portinfo(elt_value,ptnb_to,ptype_to,2,pto_int,ErrMess_temp);
											rescode+=CheckInternalConnectivity(pfrom_int, pto_int,ErrMess_temp);

										}
										else
										{
											rescode+=GetPortID_portinfo(elt_value,ptnb_from,ptype_from,2,pfrom_int,ErrMess_temp);
											rescode+=GetPortID_portinfo(elt_value,ptnb_to,ptype_to,1,pto_int,ErrMess_temp);
											rescode+=CheckInternalConnectivity(pto_int, pfrom_int,ErrMess_temp);

										}	
										if(rescode==0)//then it's OK link allowed
											next_step=0;
										else
											next_step=-1;
									}
									else
										next_step=0;

								}
								else
									next_step=0;//measn it's OK we go to the next step
								if(next_step==0)
								{
									for(m=0;m<j;m++)
										path_matrix_new[nb_of_line*nb_of_hop+m]=path_matrix_old[i*nb_of_hop+m];	
									path_matrix_new[nb_of_line*nb_of_hop+j]=lkid_list_bis_rev[elt_value_temp1];
									path_matrix_new[nb_of_line*nb_of_hop+j+1]=elt_value_temp2;
									if(lk_weight_list_bis_rev[elt_value_temp1]==2)
									{
										path_end_new[nb_of_line]=1;
									}
									else
									{
										if(lk_weight_list_bis_rev[elt_value_temp1]==0)
											path_end_new[nb_of_line]=0;
										else
											path_end_new[nb_of_line]=-1;
									}
								}
								else
								{
									path_end_new[nb_of_line]=-1;
								}
								nb_of_line++;
							}
							else
							{
								path_end_new[nb_of_line]=-1;
								nb_of_line++;
							}
						}
					}
				}
				else
				{
					if(path_end_old[i]==1)
					{
						path_valid=(int*)realloc(path_valid,(valid_path+1)*nb_of_hop*sizeof(int));
						if(path_valid==NULL)
						{
							if(path_matrix_old!=NULL)
								path_matrix_old=(int*)realloc(path_matrix_old,0*sizeof(int));
							if(path_matrix_new!=NULL)
								path_matrix_new=(int*)realloc(path_matrix_new,0*sizeof(int));
							if(path_end_new!=NULL)
								path_end_new=(int*)realloc(path_end_new,0*sizeof(int));
							if(path_end_old!=NULL)
								path_end_old=(int*)realloc(path_end_old,0*sizeof(int));
							free(elt_idx_temp);
							free(path_matrix_temp);
							if(elt_idx_temp1!=NULL)
								free(elt_idx_temp1);
							free(elt_idx);
							for(p=0;p<max_rows_bis_rev;p++)
							{
								free(pto_array_list_bis_rev[p]);
								free(nto_array_list_bis_rev[p]);
								free(pfrom_array_list_bis_rev[p]);
								free(nfrom_array_list_bis_rev[p]);

							}
							free(pto_array_list_bis_rev);
							free(nto_array_list_bis_rev);
							free(pfrom_array_list_bis_rev);
							free(nfrom_array_list_bis_rev);
							if(lkid_list_bis_rev!=NULL)
								free(lkid_list_bis_rev);
							if(node_from_list_bis_rev!=NULL)
								free(node_from_list_bis_rev);
							if(node_to_list_bis_rev!=NULL)
								free(node_to_list_bis_rev);
							if(bidirectional_list_bis_rev!=NULL)
								free(bidirectional_list_bis_rev);
							if(lk_weight_list_bis_rev!=NULL)
								free(lk_weight_list_bis_rev);
							if(lktype_list_bis_rev!=NULL)
								free(lktype_list_bis_rev);
							if(lkused_list_bis_rev!=NULL)
								free(lkused_list_bis_rev);
							if(pathid_list6!=NULL)
								free(pathid_list6);
							if(link_pos_list6!=NULL)
								free(link_pos_list6);
							if(lkid_list6!=NULL)
								free(lkid_list6);
							GetErrorMess(appliName,"Unsuccessful malloc 6",ErrMessage,1);
							return -1;

						}
						lk_pos=1;
						pathid_list6=(int*)realloc(pathid_list6,(nb_of_line1+j)*sizeof(int));
						link_pos_list6=(int*)realloc(link_pos_list6,(nb_of_line1+j)*sizeof(int));
						lkid_list6=(int*)realloc(lkid_list6,(nb_of_line1+j)*sizeof(int));
						if(pathid_list6==NULL||link_pos_list6==NULL||lkid_list6==NULL)
						{
							if(path_matrix_old!=NULL)
								path_matrix_old=(int*)realloc(path_matrix_old,0*sizeof(int));
							if(path_matrix_new!=NULL)
								path_matrix_new=(int*)realloc(path_matrix_new,0*sizeof(int));
							if(path_end_new!=NULL)
								path_end_new=(int*)realloc(path_end_new,0*sizeof(int));
							if(path_end_old!=NULL)
								path_end_old=(int*)realloc(path_end_old,0*sizeof(int));
							free(elt_idx_temp);
							free(path_matrix_temp);
							if(elt_idx_temp1!=NULL)
								free(elt_idx_temp1);
							free(elt_idx);
							for(p=0;p<max_rows_bis_rev;p++)
							{
								free(pto_array_list_bis_rev[p]);
								free(nto_array_list_bis_rev[p]);
								free(pfrom_array_list_bis_rev[p]);
								free(nfrom_array_list_bis_rev[p]);

							}
							free(pto_array_list_bis_rev);
							free(nto_array_list_bis_rev);
							free(pfrom_array_list_bis_rev);
							free(nfrom_array_list_bis_rev);
							if(lkid_list_bis_rev!=NULL)
								free(lkid_list_bis_rev);
							if(node_from_list_bis_rev!=NULL)
								free(node_from_list_bis_rev);
							if(node_to_list_bis_rev!=NULL)
								free(node_to_list_bis_rev);
							if(bidirectional_list_bis_rev!=NULL)
								free(bidirectional_list_bis_rev);
							if(lk_weight_list_bis_rev!=NULL)
								free(lk_weight_list_bis_rev);
							if(lktype_list_bis_rev!=NULL)
								free(lktype_list_bis_rev);
							if(lkused_list_bis_rev!=NULL)
								free(lkused_list_bis_rev);
							if(pathid_list6!=NULL)
								free(pathid_list6);
							if(link_pos_list6!=NULL)
								free(link_pos_list6);
							if(lkid_list6!=NULL)
								free(lkid_list6);
							GetErrorMess(appliName,"Unsuccessful malloc 7",ErrMessage,1);
							return -1;
						}
						for(m=1;m<j;m++)
						{
							path_valid[valid_path*nb_of_hop+m]=path_matrix_old[i*nb_of_hop+m];	
							rescode=GetIndexForTabINT(lkid_list_bis_rev,len_array_gv_bis_rev,path_valid[valid_path*nb_of_hop+m] , elt_idx_temp1,elt_idx_temp1_len,1);		
							pathid_list6[nb_of_line1]=valid_path;
							link_pos_list6[nb_of_line1]=lk_pos;
							elt_value=elt_idx_temp1[0];//necessary one possibility
							lkid_list6[nb_of_line1]=lkid_list_bis_rev[elt_value];
							nb_of_line1++;
							lk_pos++;
							m++;
						}
						valid_path++;
					}

				}
			}
			j=j+2;
			path_matrix_old=(int*)realloc(path_matrix_old,nb_of_line*nb_of_hop*sizeof(int));
			path_end_old=(int*)realloc(path_end_old,nb_of_line*sizeof(int));
			CopyMatrix(path_matrix_old,path_matrix_new,nb_of_line,j,nb_of_line,0,nb_of_hop); //pure copy of the matrix
			CopyVector(path_end_old,path_end_new,nb_of_line,nb_of_line,0);//copy + element shift
			path_matrix_old_len=nb_of_line;
			stop=ComputePathWeight(path_end_new,nb_of_line);
			stop+=ComputePathWeight(path_end_old,path_matrix_old_len);
			path_matrix_new_len=0;
			nb_of_line=0;
			//std:://cout<<"j="<<j<<" and stop="<<stop<<std::endl;
		}	
		//std:://cout<<"out of the loop"<<std::endl;
		if(stop>0) // get the last valid path of the loop
		{
			for(i=0;i<path_matrix_old_len;i++)
			{
				if(path_end_old[i]==1)
				{
					path_valid=(int*)realloc(path_valid,(valid_path+1)*nb_of_hop*sizeof(int));
					if(path_valid==NULL)
					{
						if(path_matrix_old!=NULL)
							path_matrix_old=(int*)realloc(path_matrix_old,0*sizeof(int));
						if(path_matrix_new!=NULL)
							path_matrix_new=(int*)realloc(path_matrix_new,0*sizeof(int));
						if(path_end_new!=NULL)
							path_end_new=(int*)realloc(path_end_new,0*sizeof(int));
						if(path_end_old!=NULL)
							path_end_old=(int*)realloc(path_end_old,0*sizeof(int));
						free(elt_idx_temp);
						free(elt_idx);
						free(path_matrix_temp);
						if(elt_idx_temp1!=NULL)
							free(elt_idx_temp1);
						for(i=0;i<max_rows_bis_rev;i++)
						{
							free(pto_array_list_bis_rev[i]);
							free(nto_array_list_bis_rev[i]);
							free(pfrom_array_list_bis_rev[i]);
							free(nfrom_array_list_bis_rev[i]);

						}
						free(pto_array_list_bis_rev);
						free(nto_array_list_bis_rev);
						free(pfrom_array_list_bis_rev);
						free(nfrom_array_list_bis_rev);
						if(lkid_list_bis_rev!=NULL)
							free(lkid_list_bis_rev);
						if(node_from_list_bis_rev!=NULL)
							free(node_from_list_bis_rev);
						if(node_to_list_bis_rev!=NULL)
							free(node_to_list_bis_rev);
						if(bidirectional_list_bis_rev!=NULL)
							free(bidirectional_list_bis_rev);
						if(lk_weight_list_bis_rev!=NULL)
							free(lk_weight_list_bis_rev);
						if(lktype_list_bis_rev!=NULL)
							free(lktype_list_bis_rev);
						if(lkused_list_bis_rev!=NULL)
							free(lkused_list_bis_rev);
						if(pathid_list6!=NULL)
							free(pathid_list6);
						if(link_pos_list6!=NULL)
							free(link_pos_list6);
						if(lkid_list6!=NULL)
							free(lkid_list6);
						GetErrorMess(appliName,"Unsuccessful malloc 6",ErrMessage,1);
						return -1;

					}
					lk_pos=1;
					pathid_list6=(int*)realloc(pathid_list6,(nb_of_line1+j)*sizeof(int));
					link_pos_list6=(int*)realloc(link_pos_list6,(nb_of_line1+j)*sizeof(int));
					lkid_list6=(int*)realloc(lkid_list6,(nb_of_line1+j)*sizeof(int));
					if(pathid_list6==NULL||link_pos_list6==NULL||lkid_list6==NULL)
					{
						if(path_matrix_old!=NULL)
							path_matrix_old=(int*)realloc(path_matrix_old,0*sizeof(int));
						if(path_matrix_new!=NULL)
							path_matrix_new=(int*)realloc(path_matrix_new,0*sizeof(int));
						if(path_end_new!=NULL)
							path_end_new=(int*)realloc(path_end_new,0*sizeof(int));
						if(path_end_old!=NULL)
							path_end_old=(int*)realloc(path_end_old,0*sizeof(int));
						free(elt_idx_temp);
						free(path_matrix_temp);
						if(elt_idx_temp1!=NULL)
							free(elt_idx_temp1);
						free(elt_idx);
						for(p=0;p<max_rows_bis_rev;p++)
						{
							free(pto_array_list_bis_rev[p]);
							free(nto_array_list_bis_rev[p]);
							free(pfrom_array_list_bis_rev[p]);
							free(nfrom_array_list_bis_rev[p]);
						}
						free(pto_array_list_bis_rev);
						free(nto_array_list_bis_rev);
						free(pfrom_array_list_bis_rev);
						free(nfrom_array_list_bis_rev);
						if(lkid_list_bis_rev!=NULL)
							free(lkid_list_bis_rev);
						if(node_from_list_bis_rev!=NULL)
							free(node_from_list_bis_rev);
						if(node_to_list_bis_rev!=NULL)
							free(node_to_list_bis_rev);
						if(bidirectional_list_bis_rev!=NULL)
							free(bidirectional_list_bis_rev);
						if(lk_weight_list_bis_rev!=NULL)
							free(lk_weight_list_bis_rev);
						if(lktype_list_bis_rev!=NULL)
							free(lktype_list_bis_rev);
						if(lkused_list_bis_rev!=NULL)
							free(lkused_list_bis_rev);
						if(pathid_list6!=NULL)
							free(pathid_list6);
						if(link_pos_list6!=NULL)
							free(link_pos_list6);
						if(lkid_list6!=NULL)
							free(lkid_list6);
						GetErrorMess(appliName,"Unsuccessful malloc 7",ErrMessage,1);
						return -1;
					}
					for(m=1;m<j;m++)
					{
						path_valid[valid_path*nb_of_hop+m]=path_matrix_old[i*nb_of_hop+m];
						rescode=GetIndexForTabINT(lkid_list_bis_rev,len_array_gv_bis_rev,path_valid[valid_path*nb_of_hop+m] , elt_idx_temp1,elt_idx_temp1_len,1);
						pathid_list6[nb_of_line1]=valid_path;
						link_pos_list6[nb_of_line1]=lk_pos;
						elt_value=elt_idx_temp1[0];//necessary one possibility
						lkid_list6[nb_of_line1]=lkid_list_bis_rev[elt_value];
						nb_of_line1++;
						lk_pos++;
						m++;	
					}
					valid_path++;
				}
			}
			stop=j;

			if(len_array<nb_of_line1)
			{
				GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrMessage,1);
				len_array=nb_of_line1;
				rescode=-1;
			}
			else
			{
				for(i=0;i<nb_of_line1;i++)
				{
					pathid_list[i]=pathid_list6[i];
					link_pos_list[i]=link_pos_list6[i];
					lkid_list5[i]=lkid_list6[i];
				}
				len_array=nb_of_line1;
			}



		}

		//fclose(f);
		nb_of_line=0;
		//std:://cout<<"out of the last loop and valid_opath="<<valid_path<<std::endl;
		free(elt_idx_temp);
		free(elt_idx);
		if(pathid_list6!=NULL)
			free(pathid_list6);
		if(link_pos_list6!=NULL)
			free(link_pos_list6);
		if(lkid_list6!=NULL)
			free(lkid_list6);
		if(elt_idx_temp1!=NULL)
			free(elt_idx_temp1);
		if(path_matrix_temp!=NULL)
			free(path_matrix_temp);
		if(path_end_old!=NULL)
			realloc(path_end_old,0*sizeof(int));
		if(path_end_new!=NULL)
			realloc(path_end_new,0*sizeof(int));
		if(path_matrix_old!=NULL )
			realloc(path_matrix_old,0*sizeof(int));	
		if(path_matrix_new!=NULL )
			realloc(path_matrix_new,0*sizeof(int));
		if(path_valid!=NULL )
		{
			realloc(path_valid,0*sizeof(int));
		}
		if(delete_connectivity==1)
		{
			for(i=0;i<max_rows_bis_rev;i++)
			{
				free(pto_array_list_bis_rev[i]);
				free(nto_array_list_bis_rev[i]);
				free(pfrom_array_list_bis_rev[i]);
				free(nfrom_array_list_bis_rev[i]);

			}
			free(pto_array_list_bis_rev);
			free(nto_array_list_bis_rev);
			free(pfrom_array_list_bis_rev);
			free(nfrom_array_list_bis_rev);
			if(lkid_list_bis_rev!=NULL)
				free(lkid_list_bis_rev);
			if(node_from_list_bis_rev!=NULL)
				free(node_from_list_bis_rev);
			if(node_to_list_bis_rev!=NULL)
				free(node_to_list_bis_rev);
			if(bidirectional_list_bis_rev!=NULL)
				free(bidirectional_list_bis_rev);
			if(lk_weight_list_bis_rev!=NULL)
				free(lk_weight_list_bis_rev);
			if(lktype_list_bis_rev!=NULL)
				free(lktype_list_bis_rev);
			if(lkused_list_bis_rev!=NULL)
				free(lkused_list_bis_rev);
		}
		if((strstr(ErrMessage,"BUFFER_TOO_SMALL"))==NULL)
		{
			if(valid_path>0)
			{
				GetErrorMess(appliName,"No_ERROR",ErrMessage,0);
				rescode=0;
			}
			else
			{
				GetErrorMess(appliName,"NO_ROWS_SELECTED",ErrMessage,1);
				rescode=-1;
			}
		}

		//std:://cout<<"before returning "<<std::endl;	
		return rescode;
	}
	/************************************************************************/
	/**********************************************************************************************************/
	/**
	* Get the path going through a given device returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* you need grant execute if you have your own db account
	* @param systemnameList : name of the system: (part of the device,upper case). Put "ALL" if you want all
	* @param devname : name of the device
	* @param len_array : length of the lkid_list5 (which is equal to pathid_list,link_pos_list ) you have allocated. If the int * buffer is too small, we put the size needed here.
	* @param lkid_list5 : list of the lkid 
	* @param pathid_list : list of the pathid
	* @param link_pos_list : position of the link in the path (start with 1)
	* @param reload_connectivity : 1 if you want to load or reload the connectivity (to set to 1, after a change or if it is the first time) otherwise 0;
	* @param delete_connectivity : 1 if you want to delete the connectivity cache(to set to 1, last query using it) otherwise 0;
	* NB: the i-th entry of the lkid_list,pathid_list and link_pos_list corresponds to one link of the path
	* @param ErrMessage : error message in case of failure (even NO_ROWS_SELECTED for no result)
	* @return 0 if it is successful
	*/
	/*********************************************************************************************/


	EXTERN_CONFDB
		int GetAllPathsPerDevice(char* systemnameList,char* devname,int& len_array, int* lkid_list5,int* pathid_list,int* link_pos_list,int reload_connectivity,int delete_connectivity,char* ErrMessage)
	{
		char appliName[100]="GetAllPathsPerDevice";
		int rescode;
		int i=0;
		int j=0;
		int len_array_cache=len_array;
		int* lkid_list5_cache=(int*)malloc(len_array*sizeof(int));
		int* pathid_list_cache=(int*)malloc(len_array*sizeof(int));
		int* link_pos_list_cache=(int*)malloc(len_array*sizeof(int));
		char ErrMessage_cache[1000];
		int rescode_rev;
		int len_array_cache_rev=len_array;
		int* lkid_list5_cache_rev=(int*)malloc(len_array*sizeof(int));
		int* pathid_list_cache_rev=(int*)malloc(len_array*sizeof(int));
		int* link_pos_list_cache_rev=(int*)malloc(len_array*sizeof(int));
		char ErrMessage_cache_rev[1000];
		if(lkid_list5_cache==NULL||lkid_list5_cache_rev==NULL||pathid_list_cache_rev==NULL||pathid_list_cache==NULL||link_pos_list_cache_rev==NULL||link_pos_list_cache_rev==NULL)
		{
			if(lkid_list5_cache!=NULL)
				free(lkid_list5_cache);
			if(pathid_list_cache!=NULL)
				free(pathid_list_cache);
			if(link_pos_list_cache!=NULL)
				free(link_pos_list_cache);
			if(lkid_list5_cache_rev!=NULL)
				free(lkid_list5_cache_rev);
			if(pathid_list_cache_rev!=NULL)
				free(pathid_list_cache_rev);
			if(link_pos_list_cache_rev!=NULL)
				free(link_pos_list_cache_rev);
		}
		rescode=GetAllPathsPerDevice_cache(appliName, systemnameList, devname,len_array_cache, lkid_list5_cache, pathid_list_cache, link_pos_list_cache, reload_connectivity, delete_connectivity, ErrMessage_cache);
		//std:://cout<<"rescode="<<rescode<<" and ermess="<<ErrMessage_cache<<" and len_array_cache="<<len_array_cache<<std::endl;
		rescode_rev=GetAllPathsPerDevice_cache_reverse(appliName, systemnameList, devname,len_array_cache_rev, lkid_list5_cache_rev, pathid_list_cache_rev, link_pos_list_cache_rev, reload_connectivity, delete_connectivity, ErrMessage_cache_rev);
		//std:://cout<<"rescode_rev="<<rescode_rev<<" and ermess="<<ErrMessage_cache_rev<<std::endl;

		if(rescode==0 && rescode_rev==0)
		{
			if(len_array_cache+len_array_cache_rev<len_array+1)
			{
				for(i=0;i<len_array_cache;i++)
				{
					lkid_list5[i]=lkid_list5_cache[i];
					pathid_list[i]=pathid_list_cache[i];
					link_pos_list[i]=link_pos_list_cache[i];

				}
				for(i=0;i<len_array_cache_rev;i++)
				{
					j=len_array_cache+i;
					lkid_list5[j]=lkid_list5_cache_rev[i];
					pathid_list[j]=pathid_list_cache_rev[i];
					link_pos_list[j]=link_pos_list_cache_rev[i];
				}
				strcpy(ErrMessage,ErrMessage_cache_rev);
				len_array=len_array_cache+len_array_cache_rev;
			}
			else
			{
				GetErrorMess(appliName,"BUFFER_TOO_SMALL",ErrMessage,1);
				len_array=len_array_cache+len_array_cache_rev;
				rescode=-1;

			}

		}
		else
		{
			if(rescode==0 && strstr(ErrMessage_cache_rev,"BUFFER_TOO_SMALL")==NULL)
			{
				for(i=0;i<len_array_cache;i++)
				{
					lkid_list5[i]=lkid_list5_cache[i];
					pathid_list[i]=pathid_list_cache[i];
					link_pos_list[i]=link_pos_list_cache[i];
				}
				len_array=len_array_cache;
				strcpy(ErrMessage,ErrMessage_cache);
			}
			else
			{

				if(rescode_rev==0 && strstr(ErrMessage_cache,"BUFFER_TOO_SMALL")==NULL)
				{
					for(i=0;i<len_array_cache_rev;i++)
					{
						lkid_list5[i]=lkid_list5_cache_rev[i];
						pathid_list[i]=pathid_list_cache_rev[i];
						link_pos_list[i]=link_pos_list_cache_rev[i];
					}
					len_array=len_array_cache_rev;
					strcpy(ErrMessage,ErrMessage_cache_rev);
					rescode=0;
				}
				else
				{
					if(strstr(ErrMessage_cache,"BUFFER_TOO_SMALL")!=NULL || strstr(ErrMessage_cache_rev,"BUFFER_TOO_SMALL")!=NULL)
					{
						len_array=len_array_cache_rev+len_array_cache;
						sprintf(ErrMessage,"%s and %s ",ErrMessage_cache,ErrMessage_cache_rev);
						rescode=-1;
					}
					else
					{
						sprintf(ErrMessage,"%s and %s ",ErrMessage_cache,ErrMessage_cache_rev);
						rescode=-1;
					}
				}

			}
		}
		free(lkid_list5_cache);
		free(pathid_list_cache);
		free(link_pos_list_cache);
		free(lkid_list5_cache_rev);
		free(pathid_list_cache_rev);
		free(link_pos_list_cache_rev);
		return rescode;


	}
	/******************************************************************************************************/
	/**
	* Get the port nbs out between dev_from and devtype,  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* you need grant execute if you have your own db account
	* @param dev_from : name of the device 1 (ex. THOR_00)
	* @param devtype : name of a devicetype or fixed part of the group of devices  (ex. ECAL_L1FE)
	* @param devtype_give : 1 if you give the device type, 0 if it's a fixed part
	* @param len_array : length of the pfrom_list  you have allocated. If the int * buffer is too small, we put the size needed here.
	* @param pfrom_arraylist : list of the port from numbers: concatenation of port_nbr|port_type. each line is separated with '\\0'. the first entry is formatted port_way|port_nb|ptype. pway is the same for the other that's why it is mentioned once.
	* @param reload_connectivity : 1 if you want to load or reload the connectivity (we load only the connectivity of the subsystems in which dev_from is part of . for instance if dev_from belongs to HCAL, we load only the connectivity of HCAL.(To set to 1, after a change or if it is the first time or if the device is not in the same subsystem) otherwise 0. 
	* @param delete_connectivity : 1 if you want to delete the connectivity cache(to set to 1, last query using it) otherwise 0. N.B If ErrMess is like "Unsuccessful Malloc" or like NB of paths is too big,too many bidirectional_links, then you have to reload the connectivity table (it has been deleted).  If ErrMess looks like "NO_ROWS_SELECTED" or "DEVICE_NOT_CONNECTED", you don't need to reload the table
	* @param ErrMess : error message in case of failure (even NO_ROWS_SELECTED for no result)
	* @return 0 if it is successful
	*/
	EXTERN_CONFDB
		int GetConnOutBetweenDeviceDeviceType(char* dev_from, char* devtype,int devtype_give, int& len_array, char* pfrom_arraylist, int reload_connectivity,int delete_connectivity,char* ErrMess)
	{
		char appliName[100]="GetConnOutBetweenDeviceDeviceType";
		int rescode=0;
		int len_array1=5000;
		int pfrom1_arraylen=50000;
		int nfrom1_arraylen=1000000;
		int pto1_arraylen=pfrom1_arraylen;
		int nto1_arraylen=nfrom1_arraylen;
		int lkinfo_arraylen=10000000;
		char *p=NULL;
		char* nfrom_list=(char*)malloc(nfrom1_arraylen*sizeof(char));
		char* nto_list=(char*)malloc(nto1_arraylen*sizeof(char));
		char* pfrom1_list=(char*)malloc(pfrom1_arraylen*sizeof(char));
		char* pto1_list=(char*)malloc(pto1_arraylen*sizeof(char));
		char* lkinfo_list=(char*)malloc(lkinfo_arraylen*sizeof(char));
		int* pfrom_list=(int*)malloc(len_array1*sizeof(int));
		int* pto_list=(int*)malloc(len_array1*sizeof(int));
		int* pathid_list=(int*)malloc(len_array*sizeof(int));
		int* link_pos_list=(int*)malloc(len_array*sizeof(int));
		BIDIRECTIONAL_WAY_USED=0;
		char ErrTemp[1000];
		if(lkinfo_list==NULL||link_pos_list==NULL || pathid_list==NULL ||pto_list==NULL||pto1_list==NULL||pfrom_list==NULL||pfrom1_list==NULL||nto_list==NULL||nfrom_list==NULL)
		{
			if(link_pos_list!=NULL)
				free(link_pos_list);
			if(lkinfo_list!=NULL)
				free(lkinfo_list);
			if(pathid_list!=NULL)
				free(pathid_list);
			if(pto_list!=NULL)
				free(pto_list);
			if(pto1_list!=NULL)
				free(pto1_list);
			if(pfrom_list!=NULL)
				free(pfrom_list);
			if(pfrom1_list!=NULL)
				free(pfrom1_list);
			if(nto_list!=NULL)
				free(nto_list);
			if(nfrom_list!=NULL)
				free(nfrom_list);
			GetErrorMess(appliName,"UNSUCCESSFUL MALLOC",ErrMess,1);
			return -1;
		}
		else
			rescode=GetDetailedConnectivityBetweenDeviceDeviceType_Cache(appliName,"ALL",dev_from, devtype,devtype_give, len_array1, pfrom1_arraylen,nfrom1_arraylen,pto1_arraylen,nto1_arraylen,lkinfo_arraylen,nfrom_list,nto_list,pfrom1_list, pfrom_list,pto1_list,pto_list, pathid_list,link_pos_list,lkinfo_list, reload_connectivity,delete_connectivity,ErrMess);

		if(rescode==0)
		{
			len_array=pfrom1_arraylen+2;
			strcpy(pfrom_arraylist,"2|");
			memcpy(pfrom_arraylist+2,pfrom1_list,pfrom1_arraylen);

		}
		else
		{
			if((p=strstr(ErrMess,"NO_ROWS_SELECTED"))!=NULL || (p=strstr(ErrMess,"DEVICE NOT CONNECTED"))!=NULL)
			{
				rescode=GetDetailedConnectivityBetweenDeviceDeviceType_CacheReverse(appliName,"ALL",dev_from, devtype,devtype_give, len_array1, pfrom1_arraylen,nfrom1_arraylen,pto1_arraylen,nto1_arraylen,lkinfo_arraylen,nfrom_list,nto_list,pfrom1_list, pfrom_list,pto1_list,pto_list, pathid_list,link_pos_list,lkinfo_list, reload_connectivity,delete_connectivity,ErrTemp);
				if(rescode==0)
				{
					len_array=pfrom1_arraylen+2;
					strcpy(pfrom_arraylist,"1|");
					memcpy(pfrom_arraylist,pfrom1_list,pfrom1_arraylen);
				}
				else
				{
					if((p=strstr(ErrTemp,"DEVICE NOT CONNECTED"))==NULL)
						strcpy(ErrMess,ErrTemp);
				}
			}


		}
		if(link_pos_list!=NULL)
			free(link_pos_list);
		if(pathid_list!=NULL)
			free(pathid_list);
		if(pto_list!=NULL)
			free(pto_list);
		if(pto1_list!=NULL)
			free(pto1_list);
		if(pfrom_list!=NULL)
			free(pfrom_list);
		if(pfrom1_list!=NULL)
			free(pfrom1_list);
		if(nto_list!=NULL)
			free(nto_list);
		if(nfrom_list!=NULL)
			free(nfrom_list);
		if(lkinfo_list!=NULL)
			free(lkinfo_list);

		return rescode;
	}
	/******************************************************************************************************/
	/**
	* Get the path (just the first and last link details)  between dev_from and dev_to,  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* you need grant execute if you have your own db account
	* @param dev_from : name of the device 1 (ex. THOR_00)
	* @param dev_to : name of the device 2 (ex. ECAL_L1FE06_00_00)
	* @param len_array : length of the pfrom_list (which is equal to pto_list,pathid_list) you have allocated. If the int * buffer is too small, we put the size needed here.
	* @param nfrom1_list : device from (starting)
	* @param nfrom1_arraylen : length of the nfrom_list
	* @param pfrom1_arraylen : length of the pfrom_list
	* @param nto1_arraylen : length of the nto1_list
	* @param pto1_arraylen : length of the pto1_list
	* @param	lkinfo_arraylen : length of the lkinfo_list you have allocated. if the buffer is too small, we put the size needed here. 
	* @param nto1_list : device to (ending)
	* @param pfrom_list : list of the port way 
	* @param pto_list : list of the port way 
	* @param pfrom1_list : list of the port from numbers concatenated with port_type (port_nbr|port_type)
	* @param pto1_list : list of the port to numbers concatenated with port_type (port_nbr|port_type)
	* @param pathid_list : list of the pathid
	* @param link_pos_list : position of the link in the path (start with 1)
	* @param lkinfo_list : comments related to the link
	* @param reload_connectivity : 1 if you want to load or reload the connectivity (to set to 1, after a change or if it is the first time) otherwise 0;
	* @param delete_connectivity : 1 if you want to delete the connectivity cache(to set to 1, last query using it) otherwise 0;
	* @param ErrMess : error message in case of failure (even NO_ROWS_SELECTED for no result)
	* @return 0 if it is successful
	*/
	EXTERN_CONFDB
		int GetConnectivityBetweenDevices(char* dev_from, char* dev_to, int& len_array, int &pfrom1_arraylen,int &nfrom1_arraylen,int &pto1_arraylen,int &nto1_arraylen,int &lkinfo_arraylen,char* nfrom1_list,char* nto1_list,char* pfrom1_list,int* pfrom_list,char* pto1_list,int* pto_list,int* pathid_list,int* link_pos_list,char* lkinfo_list,int reload_connectivity,int delete_connectivity,char* ErrMess)
	{
		char appliName[100]="GetConnectivityBetweenDevices";
		int rescode=0;
		char* p=NULL;
		BIDIRECTIONAL_WAY_USED=0;
		char ErrTemp[1000];
		rescode=GetDetailedConnectivityBetweenDeviceDeviceType_Cache(appliName,"ALL",dev_from, dev_to,2, len_array, pfrom1_arraylen,nfrom1_arraylen,pto1_arraylen,nto1_arraylen,lkinfo_arraylen,nfrom1_list,nto1_list,pfrom1_list, pfrom_list,pto1_list,pto_list, pathid_list,link_pos_list,lkinfo_list,reload_connectivity,delete_connectivity, ErrMess);	
		if((p=strstr(ErrMess,"NO_ROWS_SELECTED"))!=NULL || (p=strstr(ErrMess,"DEVICE NOT CONNECTED"))!=NULL)
		{
			rescode=GetDetailedConnectivityBetweenDeviceDeviceType_CacheReverse(appliName,"ALL",dev_from, dev_to,2, len_array, pfrom1_arraylen,nfrom1_arraylen,pto1_arraylen,nto1_arraylen,lkinfo_arraylen,nfrom1_list,nto1_list,pfrom1_list, pfrom_list,pto1_list,pto_list, pathid_list,link_pos_list,lkinfo_list,reload_connectivity,delete_connectivity, ErrTemp);	
			if((p=strstr(ErrTemp,"DEVICE NOT CONNECTED"))==NULL)
				strcpy(ErrMess,ErrTemp);
		}	
		return rescode;
	}
	/**********************************************************************************************************/
	/**
	* Get the path details  between dev_from and dev_to, returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* you need grant execute if you have your own db account
	* @param dev_from : name of the device 1 (ex. THOR_00)
	* @param dev_to : name of the device 2 (ex. ECAL_L1FE)
	* @param nfrom_list : list of the devices which start the link (separated by '\\0')
	* @param len_array : length of the pfrom_list (which is equal to pto_list,pathid_list and link_pos_list) you have allocated. If the int * buffer is too small, we put the size needed here.
	* @param nfrom_arraylen : length of the nfrom_list  you have allocated. If the char * buffer is too small, we put the size needed here.
	* @param nto_arraylen : length of the nto_list  you have allocated. If the char * buffer is too small, we put the size needed here.
	* @param	lkinfo_arraylen : length of the lkinfo_list you have allocated. if the buffer is too small, we put the size needed here. 
	* @param pfrom_list : list of the port from numbers concatenated with port type port_nbr|port_type
	* @param pto_list : list of the port to numbers concatenated with port type port_nbr|port_type
	* @param nto_list : list of the devices which end the link
	* @param pfrom1_list : list of the port way (1 or 2) of the corresponding (to fully identify the port). useful for bidirectional link
	* @param pto1_list : list of the port way (1 or 2) of the corresponding (to fully identify the port)
	* @param pfrom_arraylen : length of the pfrom_list  you have allocated. If the char * buffer is too small, we put the size needed here.
	* @param pto_arraylen : length of the pto_list  you have allocated. If the char * buffer is too small, we put the size needed here.
	* @param pathid_list : list of the pathid
	* @param link_pos_list : position of the link in the path (start with 1)
	* @param lkinfo_list : comments related to the link
	* @param reload_connectivity : 1 if you want to load or reload the connectivity (we load only the connectivity of the subsystems in which dev_from is part of . for instance if dev_from belongs to HCAL, we load only the connectivity of HCAL.(To set to 1, after a change or if it is the first time or if the device is not in the same subsystem) otherwise 0. 
	* @param delete_connectivity : 1 if you want to delete the connectivity cache(to set to 1, last query using it) otherwise 0. N.B If ErrMess is like "Unsuccessful Malloc" or like NB of paths is too big,too many bidirectional_links, then you have to reload the connectivity table (it has been deleted).  If ErrMess looks like "NO_ROWS_SELECTED" or "DEVICE_NOT_CONNECTED", you don't need to reload the table
	* NB: the i-th entry of the nfrom_list,pfrom_list,nto_list,pto_list,pathid_list and link_pos_list corresponds to one link of the path
	* @param ErrMess : error message in case of failure (even NO_ROWS_SELECTED for no result)
	* @return 0 if it is successful
	*/
	EXTERN_CONFDB
		int GetDetailedConnectivityBetweenDevices(char* dev_from, char* dev_to, char* nfrom_list,int& len_array,int & nfrom_arraylen,int & pfrom_arraylen, int & nto_arraylen, int & pto_arraylen,int &lkinfo_arraylen,char* pfrom_list,char* pto_list,int* pfrom1_list,int* pto1_list,char* nto_list,int* pathid_list, int* link_pos_list,char* lkinfo_list,int reload_connectivity,int delete_connectivity,char* ErrMess)
	{

		char appliName[100]="GetDetailedConnectivityBetweenDevices";
		int rescode=0;
		BIDIRECTIONAL_WAY_USED=0;
		char* p=NULL;
		char ErrTemp[1000];
		rescode=GetDetailedConnectivityBetweenDeviceDeviceType_Cache(appliName,"ALL",dev_from, dev_to,2, len_array, pfrom_arraylen,nfrom_arraylen,pto_arraylen,nto_arraylen,lkinfo_arraylen,nfrom_list,nto_list,pfrom_list, pfrom1_list,pto_list,pto1_list, pathid_list,link_pos_list, lkinfo_list,reload_connectivity,delete_connectivity,ErrMess);	
		if((p=strstr(ErrMess,"NO_ROWS_SELECTED"))!=NULL || (p=strstr(ErrMess,"DEVICE NOT CONNECTED"))!=NULL)
		{
			rescode=GetDetailedConnectivityBetweenDeviceDeviceType_CacheReverse(appliName,"ALL",dev_from, dev_to,2, len_array, pfrom_arraylen,nfrom_arraylen,pto_arraylen,nto_arraylen,lkinfo_arraylen,nfrom_list,nto_list,pfrom_list, pfrom1_list,pto_list,pto1_list, pathid_list,link_pos_list, lkinfo_list,reload_connectivity,delete_connectivity,ErrTemp);	
			if((p=strstr(ErrTemp,"DEVICE NOT CONNECTED"))==NULL)
				strcpy(ErrMess,ErrTemp);
		}
		return rescode;

	}
	/**********************************************************************************************************/
	/**
	* Get the path details  between dev_from and dev_to,  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* you need grant execute if you have your own db account
	* @param dev_from : name of the device 1 (ex. THOR_00)
	* @param dev_to : name of the device 2 (ex. ECAL_L1FE)
	* @param devto_type_given : 1 if it's the device type you gave, 0 if it's the list of devicename which is similar to 'devto_type_given%'
	* @param nfrom_list : list of the devices which start the link (separated by '\\0')
	* @param len_array : length of the pfrom_list (which is equal to pto_list,pathid_list and link_pos_list) you have allocated. If the int * buffer is too small, we put the size needed here.
	* @param nfrom_arraylen : length of the nfrom_list  you have allocated. If the char * buffer is too small, we put the size needed here.
	* @param nto_arraylen : length of the nto_list  you have allocated. If the char * buffer is too small, we put the size needed here.
	* @param	lkinfo_arraylen : length of the lkinfo_list you have allocated. if the buffer is too small, we put the size needed here. 
	* @param pfrom_list : list of the port from numbers concatenated with port type (port_nbr|port_type)
	* @param pto_list : list of the port to numbers concatenated with port type (port_nbr|port_type)
	* @param nto_list : list of the devices which end the link
	* @param pfrom1_list : list of the port way (1 or 2) of the corresponding (to fully identify the port). useful for bidirectional link
	* @param pto1_list : list of the port way (1 or 2) of the corresponding (to fully identify the port)
	* @param pfrom_arraylen : length of the pfrom_list  you have allocated. If the char * buffer is too small, we put the size needed here.
	* @param pto_arraylen : length of the pto_list  you have allocated. If the char * buffer is too small, we put the size needed here.
	* @param pathid_list : list of the pathid
	* @param link_pos_list : position of the link in the path (start with 1)
	* @param lkinfo_list : comments related to the link
	* @param reload_connectivity : 1 if you want to load or reload the connectivity (we load only the connectivity of the subsystems in which dev_from is part of . for instance if dev_from belongs to HCAL, we load only the connectivity of HCAL.(To set to 1, after a change or if it is the first time or if the device is not in the same subsystem) otherwise 0. 
	* @param delete_connectivity : 1 if you want to delete the connectivity cache(to set to 1, last query using it) otherwise 0. N.B If ErrMess is like "Unsuccessful Malloc" or like NB of paths is too big,too many bidirectional_links, then you have to reload the connectivity table (it has been deleted).  If ErrMess looks like "NO_ROWS_SELECTED" or "DEVICE_NOT_CONNECTED", you don't need to reload the table
	* @param ErrMess : error message in case of failure (even NO_ROWS_SELECTED for no result)
	* NB: the i-th entry of the nfrom_list,pfrom_list,nto_list,pto_list,pathid_list and link_pos_list corresponds to one link of the path
	* @return 0 if it is successful
	*/
	EXTERN_CONFDB
		int GetDetailedConnBetweenDeviceDevType(char* dev_from, char* dev_to,int devto_type_given,  char* nfrom_list,char* pfrom_list,char* pto_list,int& len_array,int & nfrom_arraylen,int & pfrom_arraylen, int & nto_arraylen, int & pto_arraylen,int &lkinfo_arraylen, int* pfrom1_list,int* pto1_list,char* nto_list,int* pathid_list, int* link_pos_list,char* lkinfo_list,int reload_connectivity,int delete_connectivity,char* ErrMess)
	{

		char appliName[100]="GetDetailedConnBetweenDeviceDevType";
		int rescode=0;
		char* p=NULL;
		BIDIRECTIONAL_WAY_USED=0;
		char ErrTemp[1000];
		rescode=GetDetailedConnectivityBetweenDeviceDeviceType_Cache(appliName,"ALL",dev_from, dev_to,devto_type_given, len_array, pfrom_arraylen,nfrom_arraylen,pto_arraylen,nto_arraylen,lkinfo_arraylen,nfrom_list,nto_list,pfrom_list, pfrom1_list,pto_list,pto1_list, pathid_list,link_pos_list,lkinfo_list,reload_connectivity, delete_connectivity,ErrMess);	
		if((p=strstr(ErrMess,"NO_ROWS_SELECTED"))!=NULL || (p=strstr(ErrMess,"DEVICE NOT CONNECTED"))!=NULL)
		{
			rescode=GetDetailedConnectivityBetweenDeviceDeviceType_CacheReverse(appliName,"ALL",dev_from, dev_to,devto_type_given, len_array, pfrom_arraylen,nfrom_arraylen,pto_arraylen,nto_arraylen,lkinfo_arraylen,nfrom_list,nto_list,pfrom_list, pfrom1_list,pto_list,pto1_list, pathid_list,link_pos_list,lkinfo_list,reload_connectivity,delete_connectivity, ErrTemp);	
			//std:://cout<<"value of errtemp="<<ErrTemp<<std::endl;
			if((p=strstr(ErrTemp,"DEVICE NOT CONNECTED"))==NULL)
				strcpy(ErrMess,ErrTemp);
		}

		return rescode;
	}
#if defined(__cplusplus) || defined(c_plusplus)
}
#endif