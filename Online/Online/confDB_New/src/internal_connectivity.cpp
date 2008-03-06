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


	/****GV for internal connectivity ************************/

	extern int len_array_gv_internal; 
	int* lkid_list_internal; 
	extern int * node_from_list_internal;
	int* bidirectional_list_internal; 
	int* lkused_list_internal; 
	int* lktype_list_internal;
	int* lk_weight_list_internal;
	char** pfrom_array_list_internal;
	char** pto_array_list_internal;
	extern int max_rows_internal;

	extern int len_array_gv_scd_internal; 
	int* lkid_list_scd_internal; 
	extern int * node_from_list_scd_internal;
	extern int* bidirectional_list_scd_internal; 
	int* lkused_list_scd_internal; 
	int* lktype_list_scd_internal;
	int* lk_weight_list_scd_internal;
	char** pfrom_array_list_scd_internal;
	char** pto_array_list_scd_internal;
	extern int max_rows_scd_internal;

	extern int len_array_gv_bis_internal; 
	int* lkid_list_bis_internal; 
	extern int * node_from_list_bis_internal;
	int* bidirectional_list_bis_internal; 
	int* lkused_list_bis_internal; 
	int* lktype_list_bis_internal;
	int* lk_weight_list_bis_internal;
	char** pfrom_array_list_bis_internal;
	char** pto_array_list_bis_internal;
	extern int max_rows_bis_internal;


	extern int len_array_gv_bis_rev_internal; 
	int* lkid_list_bis_rev_internal; 
	extern int * node_from_list_bis_rev_internal;
	int* bidirectional_list_bis_rev_internal; 
	int* lkused_list_bis_rev_internal; 
	int* lktype_list_bis_rev_internal;
	int* lk_weight_list_bis_rev_internal;
	char** pfrom_array_list_bis_rev_internal;
	char** pto_array_list_bis_rev_internal;
	extern int max_rows_bis_rev_internal;

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

	//int LoadInternalConnectivity_InCache(char* systemnameList,char * dfrom,char* dto,int bidi_link,int getallpaths,char* ErrMess)
	//{
	//
	//int i = 0;
	//int j=0;
	//int len=0;
	//int k=0;
	//char appliName[100]="LoadInternalConnectivity_InCache";
	//sword status;
	//OCIStmt *stmthp;
	//OCIDefine* def[12];
	//OCIParam *parmdp;
	//OCIBind  *bndp[2]; 
	//int pfrom_null=0;
	//int pto_null=0;
	//int linkid=0;
	//int node_from=0;
	//int rescode=0;
	//int node_to=0;
	//char* port_nbrfrom=NULL;
	//char* port_nbrto=NULL;
	//char* nodename_from1=NULL;
	//char* nodename_to1=NULL;
	//char* lkinfo_temp=NULL;
	//int nfrom_null=0;
	//int nto_null=0;
	//int pfromlen=0;
	//int lkinfo_null=0;
	//int ptolen=0;
	//int pfromlen_temp=0;
	//int ptolen_temp=0;
	//int lkinfolen=0;
	//int lkinfolen_temp=0;
	//int nfromlen1=0;
	//int ntolen1=0;
	//int nfromlen_temp1=0;
	//int ntolen_temp1=0;
	//
	//int link_type=0; 
	//int bidirectional_link_used=0;
	//int lkused=0;
	//int link_weight=0;
	//int* lkid_list_temp=NULL;
	//int* nfrom_list_temp=NULL;
	//int* lktype_temp=NULL;
	//char* pfrom_list_temp=NULL;
	//char* pto_list_temp=NULL;
	//int* bidirectional_list_temp=NULL;
	//int* lkweight_list_temp=NULL;
	//int* lkused_list_temp=NULL;
	//int round_trip=0;
	//int prefetch_rows=20000;
	//char selectconn[2000];
	//int pos1=0;
	//int pos2=0;
	//int pos3=0;
	//int pos1bis=0;
	//int pos2bis=0;
	//int pos3bis=0;
	//
	//int pos6=0;
	//int pos7=0;
	//int pos8=0;
	//int pos6bis=0;
	//int pos7bis=0;
	//int pos8bis=0;
	//int lkweight=0;
	//int node_pos1=0;
	//int node_pos2=0;
	//int node_pos3=0;
	//int node_pos1bis=0;
	//int node_pos2bis=0;
	//int node_pos3bis=0;
	//int pos9=0;
	//int pos9bis=0;
	//int lkinfo_pos1=0;
	//int lkinfo_pos2=0;
	//int lkinfo_pos3=0;
	//char logmessage[100];
	//int sysID=0;
	//char sysIDlist[100];
	//int max_pfrom_len=0;
	//int max_pto_len=0;
	//
	//
	//if(strncmp(systemnameList,"ALL",3)==0)
	//{
	//	sysID=1;
	//}
	//else
	//{
	//	if(strcspn(systemnameList,",")<strlen(systemnameList))
	//	{
	//		sysID=RetrieveSysIDList(systemnameList,sysIDlist);
	//		if(sysID==-1)
	//		{
	//			GetErrorMess(appliName, "systemnameList is null",ErrMess,1);
	//			return -1;
	//		}
	//	}
	//	else
	//	{
	//		sysID=GetSubsystemID(systemnameList);
	//	}
	//}
	////std ::cout<<"value of sysIDlist "<<sysIDlist <<std::endl;
	//status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
	//
	//if(status!=OCI_SUCCESS)
	//{	
	//	rescode=ShowErrors (status, ociError, "OCIStmtHAndleStmt unsuccessful");
	//	if(ociError!=0)
	//		OCIReportError(ociError,appliName,ErrMess,1); 
	//	else
	//		GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrMess,1); 
	//	return -1;
	//}
	//else
	//{
	//	if(bidi_link==0 || bidi_link==1)
	//	{
	//		if(bidi_link==0)
	//			sprintf(selectconn,"select t.linkid,t.link_type,t.BIDIRECTIONAL_LINK_USED,t.link_weight,g.deviceid,g.port_nbr||'|'||nvl(g.port_type,'none')||'?',p.port_nbr||'|'||nvl(p.port_type,'none')||'?' from %s t, %s p,%s g, %s m,%s n,%s l where g.portid=t.portidfrom and p.portid=t.portidto and t.link_weight=3 and m.deviceid=g.deviceid and mod(n.system_name,l.systemid)=0 and mod(m.system_name,l.systemid)=0 and n.devicename=:devname",MICRO_CONNECTIVITY_TABLE,PORT_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,SUBSYSTEM_TABLE);
	//		else
	//			sprintf(selectconn,"select -t.linkid,t.link_type,t.BIDIRECTIONAL_LINK_USED,t.link_weight,g.deviceid,g.port_nbr||'|'||nvl(g.port_type,'none')||'?',p.port_nbr||'|'||nvl(p.port_type,'none')||'?' from %s t, %s p,%s g, %s m,%s n,%s l where g.portid=t.portidto and p.portid=t.portidfrom and t.link_weight=3 and m.deviceid=g.deviceid and mod(n.system_name,l.systemid)=0 and mod(m.system_name,l.systemid)=0 and n.devicename=:devname",MICRO_CONNECTIVITY_TABLE,PORT_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,SUBSYSTEM_TABLE);
	//	}
	//	else
	//	{
	//		if(bidi_link==2)  //case where the devfrom is really the from and the devto is really the to  as it has been saved in the connectivity
	//		{
	//			if(sysID==1)
	//				sprintf(selectconn,"select t.linkid,t.link_type,t.BIDIRECTIONAL_LINK_USED,t.link_weight,g.deviceid,g.port_nbr||'|'||nvl(g.port_type,'none')||'?',p.port_nbr||'|'||nvl(p.port_type,'none')||'?' from %s t, %s p,%s g, %s m where g.portid=t.portidfrom and p.portid=t.portidto and t.link_weight=3 and m.deviceid=g.deviceid ",MICRO_CONNECTIVITY_TABLE,PORT_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE);
	//			else
	//			{
	//				if(sysID==0)
	//					sprintf(selectconn,"select t.linkid,t.link_type,t.BIDIRECTIONAL_LINK_USED,t.link_weight,g.deviceid,g.port_nbr||'|'||nvl(g.port_type,'none')||'?',p.port_nbr||'|'||nvl(p.port_type,'none')||'?' from %s t, %s p,%s g, %s m,%s l where g.portid=t.portidfrom and p.portid=t.portidto and t.link_weight=3 and m.deviceid=g.deviceid and  mod(m.system_name,l.systemid)=0 and l.system_name in (%s)",MICRO_CONNECTIVITY_TABLE,PORT_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,SUBSYSTEM_TABLE);
	//				else
	//					sprintf(selectconn,"select t.linkid,t.link_type,t.BIDIRECTIONAL_LINK_USED,t.link_weight,g.deviceid,g.port_nbr||'|'||nvl(g.port_type,'none')||'?',p.port_nbr||'|'||nvl(p.port_type,'none')||'?' from %s t, %s p,%s g, %s m,%s l where g.portid=t.portidfrom and p.portid=t.portidto and t.link_weight=3 and m.deviceid=g.deviceid and  mod(m.system_name,%d)=0 ",MICRO_CONNECTIVITY_TABLE,PORT_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,SUBSYSTEM_TABLE,sysID);
	//			}
	//		}
	//		else
	//		{
	//			if(sysID==1)
	//				sprintf(selectconn,"select -t.linkid,t.link_type,t.BIDIRECTIONAL_LINK_USED,t.link_weight,g.deviceid,g.port_nbr||'|'||nvl(g.port_type,'none')||'?',p.port_nbr||'|'||nvl(p.port_type,'none')||'?' from %s t, %s p,%s g, %s m where g.portid=t.portidto and p.portid=t.portidfrom and t.link_weight=3 and m.deviceid=g.deviceid ",MICRO_CONNECTIVITY_TABLE,PORT_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE);
	//			else
	//			{
	//				if(sysID==0)
	//					sprintf(selectconn,"select -t.linkid,t.link_type,t.BIDIRECTIONAL_LINK_USED,t.link_weight,g.deviceid,g.port_nbr||'|'||nvl(g.port_type,'none')||'?',p.port_nbr||'|'||nvl(p.port_type,'none')||'?' from %s t, %s p,%s g, %s m,%s l where g.portid=t.portidto and p.portid=t.portidfrom and t.link_weight=3 and m.deviceid=g.deviceid and  mod(m.system_name,l.systemid)=0 and l.system_name in (%s)",MICRO_CONNECTIVITY_TABLE,PORT_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,SUBSYSTEM_TABLE);
	//				else
	//					sprintf(selectconn,"select -t.linkid,t.link_type,t.BIDIRECTIONAL_LINK_USED,t.link_weight,g.deviceid,g.port_nbr||'|'||nvl(g.port_type,'none')||'?',p.port_nbr||'|'||nvl(p.port_type,'none')||'?' from %s t, %s p,%s g, %s m,%s l where g.portid=t.portidto and p.portid=t.portidfrom and t.link_weight=3 and m.deviceid=g.deviceid and  mod(m.system_name,%d)=0 ",MICRO_CONNECTIVITY_TABLE,PORT_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,SUBSYSTEM_TABLE,sysID);
	//			}
	//		
	//		}
	//		
	//
	//	}
	//	status=OCIStmtPrepare(stmthp, ociError, (text*) selectconn,(ub4) strlen(selectconn),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
	//}
	//if(bidi_link==0 ||bidi_link==1)
	//{
	//	if(status!=OCI_SUCCESS)
	//	{
	//		if(rescode==0)	
	//			rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
	//	}
	//	else
	//	{	
	//		status =OCIBindByName(stmthp, &bndp[0], ociError,(text*) ":devname", -1,(dvoid*)dfrom, strlen(dfrom)+1,  SQLT_STR, (dvoid *)0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
	//	}
	//}
	//if(status!=OCI_SUCCESS)
	//{
	//	if(rescode==0)	
	//		rescode=ShowErrors (status, ociError, "OCIBind unsuccessful");
	//}
	//else
	//	status=OCIAttrSet (stmthp,OCI_HTYPE_STMT,&prefetch_rows,0,OCI_ATTR_PREFETCH_ROWS,ociError);
	//
	//if(status!=OCI_SUCCESS)
	//{
	//	if(rescode==0)	
	//		rescode=ShowErrors (status, ociError, "OCIStmtAttribute unsuccessful");
	//}
	//else
	//	status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);
	//
	//if(status!=OCI_SUCCESS)
	//{
	//	if(rescode==0)
	//		rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
	//}
	//else
	//{
	//	status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 5);
	//}
	//	
	//if(status!=OCI_SUCCESS)
	//{
	//	if(rescode==0)
	//		rescode=ShowErrors (status, ociError, "OCIParamGet1 unsuccessful");
	//}
	//else
	//	status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &pfromlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);
	//
	//if(status!=OCI_SUCCESS)
	//{
	//	if(rescode==0)
	//		rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
	//}
	//else
	//{
	//	/* The next two statements describe the select-list item, dept, and return its length to allocate the memory*/
	//	status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 6);
	//}
	//	
	//if(status!=OCI_SUCCESS)
	//{
	//	if(rescode==0)
	//		rescode=ShowErrors (status, ociError, "OCIParamGet1 unsuccessful");
	//}
	//else
	//	status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &ptolen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);
	//
	//
	//
	//if(status!=OCI_SUCCESS)
	//{
	//	if(rescode==0)	
	//		rescode=ShowErrors (status, ociError, "OCIAttrGet unsuccessful");
	//}
	//else
	//	status=OCIDefineByPos (stmthp, &def[0], ociError,1,&linkid,sizeof(linkid), SQLT_INT, 0, 0, 0, OCI_DEFAULT);
	//
	//if(status!=OCI_SUCCESS)
	//{
	//	if(rescode==0)	
	//		rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
	//}
	//else
	//	status=OCIDefineByPos (stmthp, &def[1], ociError,2,&link_type,sizeof(link_type), SQLT_INT, 0, 0, 0, OCI_DEFAULT);
	//
	//if(status!=OCI_SUCCESS)
	//{
	//	if(rescode==0)	
	//		rescode=ShowErrors(status, ociError, "OCIDefineByPos unsuccessful");
	//}
	//else
	//	status=OCIDefineByPos(stmthp, &def[2], ociError,3,&bidirectional_link_used,sizeof(bidirectional_link_used), SQLT_INT, 0, 0, 0, OCI_DEFAULT);
	//
	//if(status!=OCI_SUCCESS)
	//{
	//	if(rescode==0)	
	//		rescode=ShowErrors(status, ociError, "OCIDefineByPos unsuccessful");
	//}
	//else
	//	status=OCIDefineByPos(stmthp, &def[3], ociError,4,&lkweight,sizeof(lkweight), SQLT_INT, 0, 0, 0, OCI_DEFAULT);
	//
	//
	//if(status!=OCI_SUCCESS)
	//{
	//	if(rescode==0)	
	//		rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
	//}
	//else
	//	status=OCIDefineByPos (stmthp, &def[4], ociError,5,&node_from,sizeof(node_from), SQLT_INT, 0, 0, 0, OCI_DEFAULT);
	//
	//
	//if(status!=OCI_SUCCESS)
	//{
	//	if(rescode==0)	
	//		rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
	//}
	//else
	//{
	//	port_nbrfrom=(char*)malloc((pfromlen+1)*sizeof(char));
	//	if(port_nbrfrom!=NULL)
	//		status=OCIDefineByPos (stmthp, &def[5], ociError, 6,port_nbrfrom,pfromlen+1, SQLT_STR, &pfrom_null, 0, 0, OCI_DEFAULT);
	//	else
	//	{
	//		rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
	//		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
	//		GetErrorMess(appliName, "Malloc unsuccessful 1",ErrMess,1);
	//		return -1;
	//	}
	//}
	//
	//if(status!=OCI_SUCCESS)
	//{
	//	if(rescode==0)	
	//		rescode=ShowErrors(status, ociError, "OCIDefineByPos unsuccessful");
	//}
	//else
	//{
	//	port_nbrto=(char*)malloc((ptolen+1)*sizeof(char));
	//	if(port_nbrto!=NULL)
	//		status=OCIDefineByPos(stmthp, &def[6], ociError, 7,port_nbrto,ptolen+1, SQLT_STR, &pto_null, 0, 0, OCI_DEFAULT);
	//	else
	//	{
	//		rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
	//		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
	//		GetErrorMess(appliName, "Malloc unsuccessful 2",ErrMess,1);
	//		free(port_nbrfrom);
	//		return -1;
	//	}
	//}
	//	
	////std ::cout<<"after define " <<std::endl;
	//if(status!=OCI_SUCCESS)
	//{
	//	if(rescode==0)	
	//		rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
	//}
	//else
	//{
	//	i=0;
	//	round_trip=1;
	//	lkid_list_temp=(int*)realloc(lkid_list_temp,prefetch_rows*sizeof(int));
	//	nfrom_list_temp=(int*)realloc(nfrom_list_temp,prefetch_rows*sizeof(int));
	//	
	//	if(lkid_list_temp==NULL  || nfrom_list_temp==NULL )
	//	{
	//		rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
	//		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
	//		GetErrorMess(appliName, "Malloc unsuccessful 6",ErrMess,1);
	//		if(lkid_list_temp!=NULL)
	//			free(lkid_list_temp);
	//		if(nfrom_list_temp!=NULL)
	//			free(nfrom_list_temp);
	//		if(port_nbrfrom!=NULL)
	//			free(port_nbrfrom);
	//		if(port_nbrto!=NULL)
	//			free(port_nbrto);
	//		return -1;
	//	
	//	}
	//	lktype_temp=(int*)realloc(lktype_temp,prefetch_rows*sizeof(int));
	//	pfrom_list_temp=(char*)realloc(pfrom_list_temp,(2)*sizeof(char));
	//	pto_list_temp=(char*)realloc(pto_list_temp,(2)*sizeof(char));
	//	if(lktype_temp==NULL || pfrom_list_temp==NULL || pto_list_temp==NULL)
	//	{
	//		rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
	//		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
	//		GetErrorMess(appliName, "Malloc unsuccessful 7",ErrMess,1);
	//		if(lkid_list_temp!=NULL)
	//			free(lkid_list_temp);
	//		if(nfrom_list_temp!=NULL)
	//			free(nfrom_list_temp);
	//		if(lktype_temp!=NULL)
	//			free(lktype_temp);
	//		if(pfrom_list_temp!=NULL)
	//			free(pfrom_list_temp);
	//		if(pto_list_temp!=NULL)
	//			free(pto_list_temp);
	//		if(port_nbrfrom!=NULL)
	//			free(port_nbrfrom);
	//		if(port_nbrto!=NULL)
	//			free(port_nbrto);
	//		return -1;
	//	}
	//	lkweight_list_temp=(int*)realloc(lkweight_list_temp,prefetch_rows*sizeof(int));
	//	bidirectional_list_temp=(int*)realloc(bidirectional_list_temp,prefetch_rows*sizeof(int));
	//	lkused_list_temp=(int*)realloc(lkused_list_temp,prefetch_rows*sizeof(int));
	//	if(bidirectional_list_temp==NULL  || lkused_list_temp==NULL ||lkweight_list_temp==NULL)
	//	{
	//		rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
	//		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
	//		GetErrorMess(appliName, "Malloc unsuccessful 8",ErrMess,1);
	//		if(lkid_list_temp!=NULL)
	//			free(lkid_list_temp);
	//		if(nfrom_list_temp!=NULL)
	//			free(nfrom_list_temp);
	//		if(lktype_temp!=NULL)
	//			free(lktype_temp);
	//		if(pfrom_list_temp!=NULL)
	//			free(pfrom_list_temp);
	//		if(pto_list_temp!=NULL)
	//			free(pto_list_temp);
	//		if(bidirectional_list_temp!=NULL)
	//			free(bidirectional_list_temp);
	//		if(lkused_list_temp!=NULL)
	//			free(lkused_list_temp);
	//		if(lkweight_list_temp!=NULL)
	//			free(lkweight_list_temp);
	//		if(port_nbrfrom!=NULL)
	//			free(port_nbrfrom);
	//		if(port_nbrto!=NULL)
	//			free(port_nbrto);
	//		return -1;
	//	}
	//}
	////std ::cout<<"before strcpy all " <<std::endl;
	//strcpy(pfrom_list_temp,"?");
	//strcpy(pto_list_temp,"?");
	//pos2=2;
	//pos2bis=2;
	//
	//node_pos2=2;
	//node_pos2bis=2;
	//
	//lkinfo_pos2=2;
	////std ::cout<<"before the loop " <<std::endl;
	//while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
	//{
	//	Format_output(pfrom_null, port_nbrfrom, logmessage, '|');
	//	Format_output(pto_null, port_nbrto, logmessage, '|');
	//	status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);
	//	if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
	//	{
	//		lkid_list_temp[i]=linkid;
	//		lkweight_list_temp[i]=lkweight;
	//		nfrom_list_temp[i]=node_from;
	//		bidirectional_list_temp[i]=bidirectional_link_used;
	//		lkused_list_temp[i]=lkused;
	//		lktype_temp[i]=link_type;
	//		pos1=strcspn(port_nbrfrom,"?");
	//		pos1bis=strcspn(port_nbrto,"?");
	//		pos3=pos2-1;
	//		pos3bis=pos2bis-1;
	//		pos2+=pos1+1;
	//		pos2bis+=pos1bis+1;
	//		if(pos1+1>max_pfrom_len)
	//			max_pfrom_len=pos1+1;
	//		if(pos1bis+1>max_pto_len)
	//			max_pto_len=pos1bis+1;
	//
	//		
	//	
	//		
	//		if((bidi_link==0 || bidi_link==1) && bidirectional_link_used==1) // we revert the link...
	//		{
	//			pos2+=pos1bis+1;
	//			pos2bis+=pos1+1;
	//			
	//		}
	//
	//		pfrom_list_temp=(char*)realloc(pfrom_list_temp,(pos2)*sizeof(char));
	//		pto_list_temp=(char*)realloc(pto_list_temp,(pos2bis)*sizeof(char));
	//
	//		
	//		if(pfrom_list_temp==NULL ||pto_list_temp==NULL)
	//		{
	//			rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
	//			status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
	//			GetErrorMess(appliName, "Malloc unsuccessful 9",ErrMess,1);
	//			if(lkid_list_temp!=NULL)
	//				free(lkid_list_temp);
	//			if(nfrom_list_temp!=NULL)
	//				free(nfrom_list_temp);
	//			if(lktype_temp!=NULL)
	//				free(lktype_temp);
	//			if(pfrom_list_temp!=NULL)
	//				free(pfrom_list_temp);
	//			if(pto_list_temp!=NULL)
	//				free(pto_list_temp);
	//			if(bidirectional_list_temp!=NULL)
	//				free(bidirectional_list_temp);
	//			if(lkused_list_temp!=NULL)
	//				free(lkused_list_temp);
	//			if(port_nbrfrom!=NULL)
	//				free(port_nbrfrom);
	//			if(port_nbrto!=NULL)
	//				free(port_nbrto);
	//			
	//			if(lkweight_list_temp!=NULL)
	//				free(lkweight_list_temp);
	//			
	//			return -1;
	//		}
	//		else
	//		{
	//			//if(i==0)
	//				//std ::cout<<"before copy line by line port_nbrfrom="<<port_nbrfrom<<std::endl;
	//			memcpy(pfrom_list_temp+pos3,port_nbrfrom,pos1+1);
	//			memcpy(pto_list_temp+pos3bis,port_nbrto,pos1bis+1);
	//			if((bidi_link==0 || bidi_link==1) && bidirectional_link_used==1) // we revert the link...
	//			{
	//				pos3+=pos1;
	//				pos3bis+=pos1bis;
	//				
	//				memcpy(pfrom_list_temp+pos3,port_nbrto,pos1bis+1);
	//				memcpy(pto_list_temp+pos3bis,port_nbrfrom,pos1+1);
	//				i++;
	//				lkid_list_temp[i]=-linkid;
	//				if(lkweight==2)
	//					lkweight_list_temp[i]=1;
	//				else
	//				{
	//					if(lkweight==1)
	//						lkweight_list_temp[i]=2;
	//					else
	//						lkweight_list_temp[i]=lkweight;
	//				}
	//				nfrom_list_temp[i]=node_to;
	//				bidirectional_list_temp[i]=bidirectional_link_used;
	//				lkused_list_temp[i]=lkused;
	//				lktype_temp[i]=link_type;
	//			}
	//			//if(i==0)
	//			//	std ::cout<<"after copy line by line lkinfo_temp="<<lkinfo_temp<<std::endl;
	//		}
	//		i++;
	//	}
	//	
	//	if(i>0 && (i%prefetch_rows)==0)
	//	{
	//		round_trip++;
	//		lkid_list_temp=(int*)realloc(lkid_list_temp,round_trip*prefetch_rows*sizeof(int));
	//		nfrom_list_temp=(int*)realloc(nfrom_list_temp,round_trip*prefetch_rows*sizeof(int));
	//		bidirectional_list_temp=(int*)realloc(bidirectional_list_temp,round_trip*prefetch_rows*sizeof(int));
	//	    lkused_list_temp=(int*)realloc(lkused_list_temp,round_trip*prefetch_rows*sizeof(int));
	//		lktype_temp=(int*)realloc(lktype_temp,round_trip*prefetch_rows*sizeof(int));
	//		lkweight_list_temp=(int*)realloc(lkweight_list_temp,round_trip*prefetch_rows*sizeof(int));
	//		if(lkid_list_temp==NULL  || nfrom_list_temp==NULL)
	//		{
	//			rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
	//			status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
	//			GetErrorMess(appliName, "Malloc unsuccessful 10",ErrMess,1);
	//			if(lkid_list_temp!=NULL)
	//				free(lkid_list_temp);
	//			if(nfrom_list_temp!=NULL)
	//				free(nfrom_list_temp);
	//			
	//			if(lktype_temp!=NULL)
	//				free(lktype_temp);
	//			if(pfrom_list_temp!=NULL)
	//				free(pfrom_list_temp);
	//			if(pto_list_temp!=NULL)
	//				free(pto_list_temp);
	//			if(bidirectional_list_temp!=NULL)
	//				free(bidirectional_list_temp);
	//			if(lkused_list_temp!=NULL)
	//				free(lkused_list_temp);
	//			if(port_nbrfrom!=NULL)
	//				free(port_nbrfrom);
	//			if(port_nbrto!=NULL)
	//				free(port_nbrto);
	//			if(lkweight_list_temp!=NULL)
	//				free(lkweight_list_temp);
	//			return -1;
	//		}
	//	
	//		if(bidirectional_list_temp==NULL  || lkused_list_temp==NULL ||lktype_temp==NULL)
	//		{
	//			rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
	//			status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
	//			GetErrorMess(appliName, "Malloc unsuccessful 11",ErrMess,1);
	//			if(lkid_list_temp!=NULL)
	//				free(lkid_list_temp);
	//			if(nfrom_list_temp!=NULL)
	//				free(nfrom_list_temp);
	//			
	//			if(lktype_temp!=NULL)
	//				free(lktype_temp);
	//			if(pfrom_list_temp!=NULL)
	//				free(pfrom_list_temp);
	//			if(pto_list_temp!=NULL)
	//				free(pto_list_temp);
	//			if(bidirectional_list_temp!=NULL)
	//				free(bidirectional_list_temp);
	//			if(lkused_list_temp!=NULL)
	//				free(lkused_list_temp);
	//			if(port_nbrfrom!=NULL)
	//				free(port_nbrfrom);
	//			if(port_nbrto!=NULL)
	//				free(port_nbrto);
	//			if(lkweight_list_temp!=NULL)
	//			free(lkweight_list_temp);
	//			return -1;
	//		}
	//	}
	//}
	////std ::cout<<"after looping i=" <<i<<std::endl;
	//if(status!=OCI_SUCCESS && status != OCI_SUCCESS_WITH_INFO)
	//{
	//	if(rescode==0 && i==0)
	//	{
	//		rescode=ShowErrors (status, ociError, "OCIFETCH third stmt unsuccessful");
	//		if(lkid_list_temp!=NULL)
	//			free(lkid_list_temp);
	//		if(nfrom_list_temp!=NULL)
	//			free(nfrom_list_temp);
	//		
	//		if(lktype_temp!=NULL)
	//			free(lktype_temp);
	//		if(pfrom_list_temp!=NULL)
	//			free(pfrom_list_temp);
	//		if(pto_list_temp!=NULL)
	//			free(pto_list_temp);
	//		if(bidirectional_list_temp!=NULL)
	//			free(bidirectional_list_temp);
	//		if(lkused_list_temp!=NULL)
	//			free(lkused_list_temp);
	//		if(port_nbrfrom!=NULL)
	//			free(port_nbrfrom);
	//		if(port_nbrto!=NULL)
	//			free(port_nbrto);
	//		
	//		if(lkweight_list_temp!=NULL)
	//			free(lkweight_list_temp);
	//		
	//		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
	//		GetErrorMess(appliName, "NO_ROWS_SELECTED",ErrMess,1);
	//		return -1; //error here we don't treat 
	//			
	//	}
	//}
	////std::cout<<"value of _nfrom_name_temp="<<nfrom_list_temp1<<std::endl;
	////std::cout<<"before copying in the array " <<std::endl;
	//if(rescode==0)
	//{
	//	if(getallpaths==0)
	//	{
	//		if(bidi_link==0)
	//		{
	//	max_rows_internal=i;	
	//	pfrom_array_list_internal=(char**)malloc(i*sizeof(char*));
	//	pto_array_list_internal=(char**)malloc(i*sizeof(char*));
	//	
	//	
	//	len_array_gv_internal=i;
	//	pfrom_list_temp[pos2-1]='\0';
	//	pto_list_temp[pos2bis-1]='\0';
	//	
	//	pos1=2;
	//	pos6=2;
	//	pos7=2;
	//	pos8=2;
	//	pos9=2;
	//	if(pfrom_array_list_internal==NULL||pto_array_list_internal==NULL)
	//	{
	//		if(pfrom_array_list_internal!=NULL)
	//			free(pfrom_array_list_internal);
	//		if(pto_array_list_internal!=NULL)
	//			free(pto_array_list_internal);
	//		
	//		if(lkid_list_temp!=NULL)
	//			free(lkid_list_temp);
	//		if(nfrom_list_temp!=NULL)
	//			free(nfrom_list_temp);
	//		
	//		
	//		if(lktype_temp!=NULL)
	//			free(lktype_temp);
	//		if(pfrom_list_temp!=NULL)
	//			free(pfrom_list_temp);
	//		if(pto_list_temp!=NULL)
	//			free(pto_list_temp);
	//		if(bidirectional_list_temp!=NULL)
	//			free(bidirectional_list_temp);
	//		if(lkweight_list_temp!=NULL)
	//			free(lkweight_list_temp);
	//		if(lkused_list_temp!=NULL)
	//			free(lkused_list_temp);
	//		if(port_nbrfrom!=NULL)
	//			free(port_nbrfrom);
	//		if(port_nbrto!=NULL)
	//			free(port_nbrto);
	//		
	//		rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
	//		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
	//		GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
	//		return -1;
	//	}
	//	lkid_list_internal=(int*)malloc(len_array_gv_internal*sizeof(int));
	//	node_from_list_internal=(int*)malloc(len_array_gv_internal*sizeof(int));
	//	bidirectional_list_internal=(int*)malloc(len_array_gv_internal*sizeof(int));
	//	lkused_list_internal=(int*)malloc(len_array_gv_internal*sizeof(int));
	//	lk_weight_list_internal=(int*)malloc(len_array_gv_internal*sizeof(int));
	//	lktype_list_internal=(int*)malloc(len_array_gv_internal*sizeof(int));
	//	if(lkid_list_internal==NULL||node_from_list_internal==NULL||bidirectional_list_internal==NULL||lkused_list_internal==NULL||lk_weight_list_internal==NULL||lktype_list_internal==NULL)
	//	{
	//		if(lkid_list_internal!=NULL)
	//			free(lkid_list_internal);
	//		if(node_from_list_internal!=NULL)
	//			free(node_from_list_internal);
	//		
	//		if(bidirectional_list_internal!=NULL)
	//			free(bidirectional_list_internal);
	//		if(lk_weight_list_internal!=NULL)
	//			free(lk_weight_list_internal);
	//		if(lktype_list_internal!=NULL)
	//			free(lktype_list_internal);
	//		if(lkused_list_internal!=NULL)
	//			free(lkused_list_internal);
	//
	//		if(pfrom_array_list_internal!=NULL)
	//			free(pfrom_array_list_internal);
	//		if(pto_array_list_internal!=NULL)
	//			free(pto_array_list_internal);
	//		
	//		if(lkid_list_temp!=NULL)
	//			free(lkid_list_temp);
	//		if(nfrom_list_temp!=NULL)
	//			free(nfrom_list_temp);
	//	
	//		
	//		if(lktype_temp!=NULL)
	//			free(lktype_temp);
	//		if(pfrom_list_temp!=NULL)
	//			free(pfrom_list_temp);
	//		if(pto_list_temp!=NULL)
	//			free(pto_list_temp);
	//		if(bidirectional_list_temp!=NULL)
	//			free(bidirectional_list_temp);
	//		if(lkweight_list_temp!=NULL)
	//			free(lkweight_list_temp);
	//		if(lkused_list_temp!=NULL)
	//			free(lkused_list_temp);
	//		if(port_nbrfrom!=NULL)
	//			free(port_nbrfrom);
	//		if(port_nbrto!=NULL)
	//			free(port_nbrto);
	//		rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
	//		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
	//		GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
	//		return -1;
	//	}
	//	for(j=0;j<i;j++)
	//	{
	//			lkid_list_internal[j]=lkid_list_temp[j];
	//			node_from_list_internal[j]=nfrom_list_temp[j];
	//			bidirectional_list_internal[j]=bidirectional_list_temp[j];
	//			lkused_list_internal[j]=lkused_list_temp[j];
	//			lktype_list_internal[j]=lktype_temp[j];
	//			lk_weight_list_internal[j]=lkweight_list_temp[j];
	//			pfrom_array_list_internal[j]=(char*)malloc(max_pfrom_len*sizeof(char));
	//			pto_array_list_internal[j]=(char*)malloc(max_pto_len*sizeof(char));
	//			if(pfrom_array_list_internal[j]==NULL||pto_array_list_internal[j]==NULL)
	//			{
	//				for(k=0;k<j;k++)
	//				{
	//					free(pfrom_array_list_internal[k]);
	//					free(pto_array_list_internal[k]);
	//				}
	//				if(pfrom_array_list_internal[j]!=NULL)
	//					free(pfrom_array_list_internal[j]);
	//				if(pto_array_list_internal[j]!=NULL)
	//					free(pto_array_list_internal[j]);
	//				if(pfrom_array_list_internal!=NULL)
	//					free(pfrom_array_list_internal);
	//				if(pto_array_list_internal!=NULL)
	//					free(pto_array_list_internal);
	//			
	//				
	//				if(lkid_list_temp!=NULL)
	//					free(lkid_list_temp);
	//				if(nfrom_list_temp!=NULL)
	//					free(nfrom_list_temp);
	//				
	//				if(lkid_list_internal!=NULL)
	//					free(lkid_list_internal);
	//				if(node_from_list_internal!=NULL)
	//					free(node_from_list_internal);
	//
	//				if(lktype_temp!=NULL)
	//					free(lktype_temp);
	//				if(pfrom_list_temp!=NULL)
	//					free(pfrom_list_temp);
	//				if(pto_list_temp!=NULL)
	//					free(pto_list_temp);
	//
	//				if(lktype_list_internal!=NULL)
	//					free(lktype_list_internal);
	//				if(bidirectional_list_internal!=NULL)
	//					free(bidirectional_list_internal);
	//				if(lk_weight_list_internal!=NULL)
	//					free(lk_weight_list_internal);
	//				if(bidirectional_list_temp!=NULL)
	//					free(bidirectional_list_temp);
	//				if(lkweight_list_temp!=NULL)
	//					free(lkweight_list_temp);
	//				if(lkused_list_temp!=NULL)
	//					free(lkused_list_temp);
	//				if(lkused_list_internal!=NULL)
	//					free(lkused_list_internal);
	//				if(port_nbrfrom!=NULL)
	//					free(port_nbrfrom);
	//				if(port_nbrto!=NULL)
	//					free(port_nbrto);
	//				
	//				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
	//				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
	//				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
	//				return -1;
	//			}
	//			else
	//			{
	//				pos1bis=strcspn(pfrom_list_temp+pos1,"?");
	//				pfrom_list_temp[pos1bis+pos1]='\0';
	//				strcpy(pfrom_array_list_internal[j],pfrom_list_temp+pos1);
	//				pos1+=pos1bis+1;
	//
	//				pos6bis=strcspn(pto_list_temp+pos6,"?");
	//				pto_list_temp[pos6bis+pos6]='\0';
	//				strcpy(pto_array_list_internal[j],pto_list_temp+pos6);
	//				pos6+=pos6bis+1;
	//
	//			}
	//
	//		}	
	//		}
	//		else
	//		{
	//			if(bidi_link==1)
	//			{
	//			max_rows_scd_internal=i;	
	//			pfrom_array_list_scd_internal=(char**)malloc(i*sizeof(char*));
	//			pto_array_list_scd_internal=(char**)malloc(i*sizeof(char*));
	//	
	//	len_array_gv_scd_internal=i;
	//	pfrom_list_temp[pos2-1]='\0';
	//	pto_list_temp[pos2bis-1]='\0';
	//	pos1=2;
	//	pos6=2;
	//	pos7=2;
	//	pos8=2;
	//	pos9=2;
	//	if(pfrom_array_list_scd_internal==NULL||pto_array_list_scd_internal==NULL)
	//	{
	//		if(pfrom_array_list_scd_internal!=NULL)
	//			free(pfrom_array_list_scd_internal);
	//		if(pto_array_list_scd_internal!=NULL)
	//			free(pto_array_list_scd_internal);
	//		
	//
	//
	//		
	//
	//		if(lkid_list_temp!=NULL)
	//			free(lkid_list_temp);
	//		if(nfrom_list_temp!=NULL)
	//			free(nfrom_list_temp);
	//		
	//
	//	
	//
	//		if(lktype_temp!=NULL)
	//			free(lktype_temp);
	//		if(pfrom_list_temp!=NULL)
	//			free(pfrom_list_temp);
	//		if(pto_list_temp!=NULL)
	//			free(pto_list_temp);
	//		if(bidirectional_list_temp!=NULL)
	//			free(bidirectional_list_temp);
	//		if(lkweight_list_temp!=NULL)
	//			free(lkweight_list_temp);
	//		if(lkused_list_temp!=NULL)
	//			free(lkused_list_temp);
	//		if(port_nbrfrom!=NULL)
	//			free(port_nbrfrom);
	//		if(port_nbrto!=NULL)
	//			free(port_nbrto);
	//		if(nodename_from1!=NULL)
	//			free(nodename_from1);
	//		if(nodename_to1!=NULL)
	//			free(nodename_to1);
	//		
	//
	//		rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
	//		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
	//		GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
	//		return -1;
	//	}
	//	lkid_list_scd_internal=(int*)malloc(len_array_gv_scd_internal*sizeof(int));
	//	node_from_list_scd_internal=(int*)malloc(len_array_gv_scd_internal*sizeof(int));
	//	bidirectional_list_scd_internal=(int*)malloc(len_array_gv_scd_internal*sizeof(int));
	//	
	//	lkused_list_scd_internal=(int*)malloc(len_array_gv_scd_internal*sizeof(int));
	//	lk_weight_list_scd_internal=(int*)malloc(len_array_gv_scd_internal*sizeof(int));
	//	lktype_list_scd_internal=(int*)malloc(len_array_gv_scd_internal*sizeof(int));
	//	if(lkid_list_scd_internal==NULL||node_from_list_scd_internal==NULL||bidirectional_list_scd_internal==NULL||lkused_list_scd_internal==NULL||lk_weight_list_scd_internal==NULL||lktype_list_scd_internal==NULL)
	//	{
	//		if(lkid_list_scd_internal!=NULL)
	//			free(lkid_list_scd_internal);
	//		if(node_from_list_scd_internal!=NULL)
	//			free(node_from_list_scd_internal);
	//		
	//		if(bidirectional_list_scd_internal!=NULL)
	//			free(bidirectional_list_scd_internal);
	//		if(lk_weight_list_scd_internal!=NULL)
	//			free(lk_weight_list_scd_internal);
	//		if(lktype_list_scd_internal!=NULL)
	//			free(lktype_list_scd_internal);
	//		if(lkused_list_scd_internal!=NULL)
	//			free(lkused_list_scd_internal);
	//
	//		if(pfrom_array_list_scd_internal!=NULL)
	//			free(pfrom_array_list_scd_internal);
	//		if(pto_array_list_scd_internal!=NULL)
	//			free(pto_array_list_scd_internal);
	//		
	//	
	//		if(lkid_list_temp!=NULL)
	//			free(lkid_list_temp);
	//		if(nfrom_list_temp!=NULL)
	//			free(nfrom_list_temp);
	//		
	//	
	//		if(lktype_temp!=NULL)
	//			free(lktype_temp);
	//		if(pfrom_list_temp!=NULL)
	//			free(pfrom_list_temp);
	//		if(pto_list_temp!=NULL)
	//			free(pto_list_temp);
	//		if(bidirectional_list_temp!=NULL)
	//			free(bidirectional_list_temp);
	//		if(lkweight_list_temp!=NULL)
	//			free(lkweight_list_temp);
	//		if(lkused_list_temp!=NULL)
	//			free(lkused_list_temp);
	//		if(port_nbrfrom!=NULL)
	//			free(port_nbrfrom);
	//		if(port_nbrto!=NULL)
	//			free(port_nbrto);
	//	
	//		
	//		if(lkid_list_scd_internal!=NULL)
	//			free(lkid_list_scd_internal);
	//		if(node_from_list_scd_internal!=NULL)
	//			free(node_from_list_scd_internal);
	//		
	//		if(bidirectional_list_scd_internal!=NULL)
	//			free(bidirectional_list_scd_internal);
	//		if(lk_weight_list_scd_internal!=NULL)
	//			free(lk_weight_list_scd_internal);
	//		if(lktype_list_scd_internal!=NULL)
	//			free(lktype_list_scd_internal);
	//		if(lkused_list_scd_internal!=NULL)
	//			free(lkused_list_scd_internal);
	//		rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
	//		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
	//		GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
	//		return -1;
	//	}
	//	for(j=0;j<i;j++)
	//	{
	//			lkid_list_scd_internal[j]=lkid_list_temp[j];
	//			node_from_list_scd_internal[j]=nfrom_list_temp[j];
	//			bidirectional_list_scd_internal[j]=bidirectional_list_temp[j];
	//			lkused_list_scd_internal[j]=lkused_list_temp[j];
	//			lktype_list_scd_internal[j]=lktype_temp[j];
	//			lk_weight_list_scd_internal[j]=lkweight_list_temp[j];
	//			pfrom_array_list_scd_internal[j]=(char*)malloc(max_pfrom_len*sizeof(char));
	//			pto_array_list_scd_internal[j]=(char*)malloc(max_pto_len*sizeof(char));
	//			if(pfrom_array_list_scd_internal[j]==NULL ||pto_array_list_scd_internal[j]==NULL)
	//			{
	//				for(k=0;k<j;k++)
	//				{
	//					free(pfrom_array_list_scd_internal[k]);
	//					free(pto_array_list_scd_internal[k]);
	//					
	//				}
	//				if(pfrom_array_list_scd_internal[j]!=NULL)
	//					free(pfrom_array_list_scd_internal[j]);
	//				if(pto_array_list_scd_internal[j]!=NULL)
	//					free(pto_array_list_scd_internal[j]);
	//				
	//				if(pfrom_array_list_scd_internal!=NULL)
	//					free(pfrom_array_list_scd_internal);
	//				if(pto_array_list_scd_internal!=NULL)
	//					free(pto_array_list_scd_internal);
	//			
	//				
	//				if(lkid_list_temp!=NULL)
	//					free(lkid_list_temp);
	//				if(nfrom_list_temp!=NULL)
	//					free(nfrom_list_temp);
	//				
	//			
	//				if(lktype_temp!=NULL)
	//					free(lktype_temp);
	//				if(pfrom_list_temp!=NULL)
	//					free(pfrom_list_temp);
	//				if(pto_list_temp!=NULL)
	//					free(pto_list_temp);
	//				if(bidirectional_list_temp!=NULL)
	//					free(bidirectional_list_temp);
	//				if(lkweight_list_temp!=NULL)
	//					free(lkweight_list_temp);
	//				if(lkused_list_temp!=NULL)
	//					free(lkused_list_temp);
	//				if(port_nbrfrom!=NULL)
	//					free(port_nbrfrom);
	//				if(port_nbrto!=NULL)
	//					free(port_nbrto);
	//			
	//				
	//			if(lkid_list_scd_internal!=NULL)
	//			free(lkid_list_scd_internal);
	//		if(node_from_list_scd_internal!=NULL)
	//			free(node_from_list_scd_internal);
	//	
	//		if(bidirectional_list_scd_internal!=NULL)
	//			free(bidirectional_list_scd_internal);
	//		if(lk_weight_list_scd_internal!=NULL)
	//			free(lk_weight_list_scd_internal);
	//		if(lktype_list_scd_internal!=NULL)
	//			free(lktype_list_scd_internal);
	//		if(lkused_list_scd_internal!=NULL)
	//			free(lkused_list_scd_internal);
	//				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
	//				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
	//				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
	//				return -1;
	//			}
	//			else
	//			{
	//				pos1bis=strcspn(pfrom_list_temp+pos1,"?");
	//				pfrom_list_temp[pos1bis+pos1]='\0';
	//				strcpy(pfrom_array_list_scd_internal[j],pfrom_list_temp+pos1);
	//				pos1+=pos1bis+1;
	//
	//				pos6bis=strcspn(pto_list_temp+pos6,"?");
	//				pto_list_temp[pos6bis+pos6]='\0';
	//				strcpy(pto_array_list_scd_internal[j],pto_list_temp+pos6);
	//				pos6+=pos6bis+1;
	//
	//				
	//
	//
	//
	//
	//			}
	//
	//		}	
	//
	//			}
	//
	//
	//		}
	//	}
	//	else
	//	{
	//		if(bidi_link==2)
	//		{
	//			max_rows_bis_internal=i;	
	//			pfrom_array_list_bis_internal=(char**)malloc(i*sizeof(char*));
	//			pto_array_list_bis_internal=(char**)malloc(i*sizeof(char*));
	//			
	//			len_array_gv_bis_internal=i;
	//			pfrom_list_temp[pos2-1]='\0';
	//			pto_list_temp[pos2bis-1]='\0';
	//			
	//			pos1=2;
	//			pos6=2;
	//			pos7=2;
	//			pos8=2;
	//			pos9=2;
	//			if(pfrom_array_list_bis_internal==NULL||pto_array_list_bis_internal==NULL)
	//			{
	//				if(pfrom_array_list_bis_internal!=NULL)
	//					free(pfrom_array_list_bis_internal);
	//				if(pto_array_list_bis_internal!=NULL)
	//					free(pto_array_list_bis_internal);
	//				
	//				
	//				if(lkid_list_temp!=NULL)
	//					free(lkid_list_temp);
	//				if(nfrom_list_temp!=NULL)
	//					free(nfrom_list_temp);
	//				
	//				
	//				if(lktype_temp!=NULL)
	//					free(lktype_temp);
	//				if(pfrom_list_temp!=NULL)
	//					free(pfrom_list_temp);
	//				if(pto_list_temp!=NULL)
	//					free(pto_list_temp);
	//				if(bidirectional_list_temp!=NULL)
	//					free(bidirectional_list_temp);
	//				if(lkweight_list_temp!=NULL)
	//					free(lkweight_list_temp);
	//				if(lkused_list_temp!=NULL)
	//					free(lkused_list_temp);
	//				if(port_nbrfrom!=NULL)
	//					free(port_nbrfrom);
	//				if(port_nbrto!=NULL)
	//					free(port_nbrto);
	//			
	//				
	//				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
	//				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
	//				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
	//				return -1;
	//			}
	//			lkid_list_bis_internal=(int*)malloc(len_array_gv_bis_internal*sizeof(int));
	//			node_from_list_bis_internal=(int*)malloc(len_array_gv_bis_internal*sizeof(int));
	//			bidirectional_list_bis_internal=(int*)malloc(len_array_gv_bis_internal*sizeof(int));
	//			lkused_list_bis_internal=(int*)malloc(len_array_gv_bis_internal*sizeof(int));
	//			lk_weight_list_bis_internal=(int*)malloc(len_array_gv_bis_internal*sizeof(int));
	//			lktype_list_bis_internal=(int*)malloc(len_array_gv_bis_internal*sizeof(int));
	//			if(lkid_list_bis_internal==NULL||node_from_list_bis_internal==NULL||bidirectional_list_bis_internal==NULL||lkused_list_bis_internal==NULL||lk_weight_list_bis_internal==NULL||lktype_list_bis_internal==NULL)
	//			{
	//				if(lkid_list_bis_internal!=NULL)
	//					free(lkid_list_bis_internal);
	//				if(node_from_list_bis_internal!=NULL)
	//					free(node_from_list_bis_internal);
	//			
	//				if(bidirectional_list_bis_internal!=NULL)
	//					free(bidirectional_list_bis_internal);
	//				if(lk_weight_list_bis_internal!=NULL)
	//					free(lk_weight_list_bis_internal);
	//				if(lktype_list_bis_internal!=NULL)
	//					free(lktype_list_bis_internal);
	//				if(lkused_list_bis_internal!=NULL)
	//					free(lkused_list_bis_internal);
	//
	//				if(pfrom_array_list_bis_internal!=NULL)
	//					free(pfrom_array_list_bis_internal);
	//				if(pto_array_list_bis_internal!=NULL)
	//					free(pto_array_list_bis_internal);
	//				
	//				
	//				if(lkid_list_temp!=NULL)
	//					free(lkid_list_temp);
	//				if(nfrom_list_temp!=NULL)
	//					free(nfrom_list_temp);
	//				
	//				
	//				if(lktype_temp!=NULL)
	//					free(lktype_temp);
	//				if(pfrom_list_temp!=NULL)
	//					free(pfrom_list_temp);
	//				if(pto_list_temp!=NULL)
	//					free(pto_list_temp);
	//				if(bidirectional_list_temp!=NULL)
	//					free(bidirectional_list_temp);
	//				if(lkweight_list_temp!=NULL)
	//					free(lkweight_list_temp);
	//				if(lkused_list_temp!=NULL)
	//					free(lkused_list_temp);
	//				if(port_nbrfrom!=NULL)
	//					free(port_nbrfrom);
	//				if(port_nbrto!=NULL)
	//					free(port_nbrto);
	//				
	//			
	//				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
	//				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
	//				GetErrorMess(appliName, "Malloc unsuccessful 8",ErrMess,1);
	//				return -1;
	//			}
	//			for(j=0;j<i;j++)
	//			{
	//			lkid_list_bis_internal[j]=lkid_list_temp[j];
	//			node_from_list_bis_internal[j]=nfrom_list_temp[j];
	//			
	//			bidirectional_list_bis_internal[j]=bidirectional_list_temp[j];
	//			lkused_list_bis_internal[j]=lkused_list_temp[j];
	//			lktype_list_bis_internal[j]=lktype_temp[j];
	//			lk_weight_list_bis_internal[j]=lkweight_list_temp[j];
	//			pfrom_array_list_bis_internal[j]=(char*)malloc(max_pfrom_len*sizeof(char));
	//			pto_array_list_bis_internal[j]=(char*)malloc(max_pto_len*sizeof(char));
	//			if(pfrom_array_list_bis_internal[j]==NULL ||pto_array_list_bis_internal[j]==NULL)
	//			{
	//				for(k=0;k<j;k++)
	//				{
	//					free(pfrom_array_list_bis_internal[k]);
	//					free(pto_array_list_bis_internal[k]);
	//					
	//				}
	//				if(pfrom_array_list_bis_internal[j]!=NULL)
	//					free(pfrom_array_list_bis_internal[j]);
	//				if(pto_array_list_bis_internal[j]!=NULL)
	//					free(pto_array_list_bis_internal[j]);
	//			
	//				if(pfrom_array_list_bis_internal!=NULL)
	//					free(pfrom_array_list_bis_internal);
	//				if(pto_array_list_bis_internal!=NULL)
	//					free(pto_array_list_bis_internal);
	//				
	//			
	//				if(lkid_list_temp!=NULL)
	//					free(lkid_list_temp);
	//				if(nfrom_list_temp!=NULL)
	//					free(nfrom_list_temp);
	//				
	//				if(lktype_temp!=NULL)
	//					free(lktype_temp);
	//				if(pfrom_list_temp!=NULL)
	//					free(pfrom_list_temp);
	//				if(pto_list_temp!=NULL)
	//					free(pto_list_temp);
	//				if(bidirectional_list_temp!=NULL)
	//					free(bidirectional_list_temp);
	//				if(lkweight_list_temp!=NULL)
	//					free(lkweight_list_temp);
	//				if(lkused_list_temp!=NULL)
	//					free(lkused_list_temp);
	//				if(port_nbrfrom!=NULL)
	//					free(port_nbrfrom);
	//				if(port_nbrto!=NULL)
	//					free(port_nbrto);
	//			
	//			
	//			if(lkid_list_bis_internal!=NULL)
	//			free(lkid_list_bis_internal);
	//		if(node_from_list_bis_internal!=NULL)
	//			free(node_from_list_bis_internal);
	//	
	//		if(bidirectional_list_bis_internal!=NULL)
	//			free(bidirectional_list_bis_internal);
	//		if(lk_weight_list_bis_internal!=NULL)
	//			free(lk_weight_list_bis_internal);
	//		if(lktype_list_bis_internal!=NULL)
	//			free(lktype_list_bis_internal);
	//		if(lkused_list_bis_internal!=NULL)
	//			free(lkused_list_bis_internal);
	//				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
	//				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
	//				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
	//				return -1;
	//			}
	//			else
	//			{
	//				pos1bis=strcspn(pfrom_list_temp+pos1,"?");
	//				pfrom_list_temp[pos1bis+pos1]='\0';
	//				strcpy(pfrom_array_list_bis_internal[j],pfrom_list_temp+pos1);
	//				pos1+=pos1bis+1;
	//
	//				pos6bis=strcspn(pto_list_temp+pos6,"?");
	//				pto_list_temp[pos6bis+pos6]='\0';
	//				strcpy(pto_array_list_bis_internal[j],pto_list_temp+pos6);
	//				pos6+=pos6bis+1;
	//			}
	//
	//		}	
	//
	//	}
	//	else
	//	{
	//			max_rows_bis_rev_internal=i;	
	//			pfrom_array_list_bis_rev_internal=(char**)malloc(i*sizeof(char*));
	//			pto_array_list_bis_rev_internal=(char**)malloc(i*sizeof(char*));
	//			
	//			len_array_gv_bis_rev_internal=i;
	//			pfrom_list_temp[pos2-1]='\0';
	//			pto_list_temp[pos2bis-1]='\0';
	//			
	//			pos1=2;
	//			pos6=2;
	//			pos7=2;
	//			pos8=2;
	//			pos9=2;
	//			if(pfrom_array_list_bis_rev_internal==NULL||pto_array_list_bis_rev_internal==NULL)
	//			{
	//				if(pfrom_array_list_bis_rev_internal!=NULL)
	//					free(pfrom_array_list_bis_rev_internal);
	//				if(pto_array_list_bis_rev_internal!=NULL)
	//					free(pto_array_list_bis_rev_internal);
	//			
	//			
	//				if(lkid_list_temp!=NULL)
	//					free(lkid_list_temp);
	//				if(nfrom_list_temp!=NULL)
	//					free(nfrom_list_temp);
	//			
	//				if(lktype_temp!=NULL)
	//					free(lktype_temp);
	//				if(pfrom_list_temp!=NULL)
	//					free(pfrom_list_temp);
	//				if(pto_list_temp!=NULL)
	//					free(pto_list_temp);
	//				if(bidirectional_list_temp!=NULL)
	//					free(bidirectional_list_temp);
	//				if(lkweight_list_temp!=NULL)
	//					free(lkweight_list_temp);
	//				if(lkused_list_temp!=NULL)
	//					free(lkused_list_temp);
	//				if(port_nbrfrom!=NULL)
	//					free(port_nbrfrom);
	//				if(port_nbrto!=NULL)
	//					free(port_nbrto);
	//				
	//			
	//				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
	//				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
	//				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
	//				return -1;
	//			}
	//			lkid_list_bis_rev_internal=(int*)malloc(len_array_gv_bis_rev_internal*sizeof(int));
	//			node_from_list_bis_rev_internal=(int*)malloc(len_array_gv_bis_rev_internal*sizeof(int));
	//			bidirectional_list_bis_rev_internal=(int*)malloc(len_array_gv_bis_rev_internal*sizeof(int));
	//			lkused_list_bis_rev_internal=(int*)malloc(len_array_gv_bis_rev_internal*sizeof(int));
	//			lk_weight_list_bis_rev_internal=(int*)malloc(len_array_gv_bis_rev_internal*sizeof(int));
	//			lktype_list_bis_rev_internal=(int*)malloc(len_array_gv_bis_rev_internal*sizeof(int));
	//			if(lkid_list_bis_rev_internal==NULL||node_from_list_bis_rev_internal==NULL||bidirectional_list_bis_rev_internal==NULL||lkused_list_bis_rev_internal==NULL||lk_weight_list_bis_rev_internal==NULL||lktype_list_bis_rev_internal==NULL)
	//			{
	//				if(lkid_list_bis_rev_internal!=NULL)
	//					free(lkid_list_bis_rev_internal);
	//				if(node_from_list_bis_rev_internal!=NULL)
	//					free(node_from_list_bis_rev_internal);
	//				
	//				if(bidirectional_list_bis_rev_internal!=NULL)
	//					free(bidirectional_list_bis_rev_internal);
	//				if(lk_weight_list_bis_rev_internal!=NULL)
	//					free(lk_weight_list_bis_rev_internal);
	//				if(lktype_list_bis_rev_internal!=NULL)
	//					free(lktype_list_bis_rev_internal);
	//				if(lkused_list_bis_rev_internal!=NULL)
	//					free(lkused_list_bis_rev_internal);
	//
	//				if(pfrom_array_list_bis_rev_internal!=NULL)
	//					free(pfrom_array_list_bis_rev_internal);
	//				if(pto_array_list_bis_rev_internal!=NULL)
	//					free(pto_array_list_bis_rev_internal);
	//				
	//				
	//				if(lkid_list_temp!=NULL)
	//					free(lkid_list_temp);
	//				if(nfrom_list_temp!=NULL)
	//					free(nfrom_list_temp);
	//			
	//				if(pfrom_list_temp!=NULL)
	//					free(pfrom_list_temp);
	//				if(pto_list_temp!=NULL)
	//					free(pto_list_temp);
	//				if(bidirectional_list_temp!=NULL)
	//					free(bidirectional_list_temp);
	//				if(lkweight_list_temp!=NULL)
	//					free(lkweight_list_temp);
	//				if(lkused_list_temp!=NULL)
	//					free(lkused_list_temp);
	//				if(port_nbrfrom!=NULL)
	//					free(port_nbrfrom);
	//				if(port_nbrto!=NULL)
	//					free(port_nbrto);
	//			
	//			
	//				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
	//				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
	//				GetErrorMess(appliName, "Malloc unsuccessful 8",ErrMess,1);
	//				return -1;
	//			}
	//			for(j=0;j<i;j++)
	//			{
	//			lkid_list_bis_rev_internal[j]=lkid_list_temp[j];
	//			node_from_list_bis_rev_internal[j]=nfrom_list_temp[j];
	//			bidirectional_list_bis_rev_internal[j]=bidirectional_list_temp[j];
	//			lkused_list_bis_rev_internal[j]=lkused_list_temp[j];
	//			lktype_list_bis_rev_internal[j]=lktype_temp[j];
	//			lk_weight_list_bis_rev_internal[j]=lkweight_list_temp[j];
	//			pfrom_array_list_bis_rev_internal[j]=(char*)malloc(max_pfrom_len*sizeof(char));
	//			pto_array_list_bis_rev_internal[j]=(char*)malloc(max_pto_len*sizeof(char));
	//			if(pfrom_array_list_bis_rev_internal[j]==NULL ||pto_array_list_bis_rev_internal[j]==NULL)
	//			{
	//				for(k=0;k<j;k++)
	//				{
	//					free(pfrom_array_list_bis_rev_internal[k]);
	//					free(pto_array_list_bis_rev_internal[k]);
	//					
	//				
	//				}
	//				if(pfrom_array_list_bis_rev_internal[j]!=NULL)
	//					free(pfrom_array_list_bis_rev_internal[j]);
	//				if(pto_array_list_bis_rev_internal[j]!=NULL)
	//					free(pto_array_list_bis_rev_internal[j]);
	//				
	//				if(pfrom_array_list_bis_rev_internal!=NULL)
	//					free(pfrom_array_list_bis_rev_internal);
	//				if(pto_array_list_bis_rev_internal!=NULL)
	//					free(pto_array_list_bis_rev_internal);
	//				
	//				
	//				if(lkid_list_temp!=NULL)
	//					free(lkid_list_temp);
	//				if(nfrom_list_temp!=NULL)
	//					free(nfrom_list_temp);
	//			
	//				
	//				if(lktype_temp!=NULL)
	//					free(lktype_temp);
	//				if(pfrom_list_temp!=NULL)
	//					free(pfrom_list_temp);
	//				if(pto_list_temp!=NULL)
	//					free(pto_list_temp);
	//				if(bidirectional_list_temp!=NULL)
	//					free(bidirectional_list_temp);
	//				if(lkweight_list_temp!=NULL)
	//					free(lkweight_list_temp);
	//				if(lkused_list_temp!=NULL)
	//					free(lkused_list_temp);
	//				if(port_nbrfrom!=NULL)
	//					free(port_nbrfrom);
	//				if(port_nbrto!=NULL)
	//					free(port_nbrto);
	//			
	//			
	//			if(lkid_list_bis_rev_internal!=NULL)
	//			free(lkid_list_bis_rev_internal);
	//		if(node_from_list_bis_rev_internal!=NULL)
	//			free(node_from_list_bis_rev_internal);
	//		
	//		if(bidirectional_list_bis_rev_internal!=NULL)
	//			free(bidirectional_list_bis_rev_internal);
	//		if(lk_weight_list_bis_rev_internal!=NULL)
	//			free(lk_weight_list_bis_rev_internal);
	//		if(lktype_list_bis_rev_internal!=NULL)
	//			free(lktype_list_bis_rev_internal);
	//		if(lkused_list_bis_rev_internal!=NULL)
	//			free(lkused_list_bis_rev_internal);
	//				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
	//				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
	//				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
	//				return -1;
	//			}
	//			else
	//			{
	//				pos1bis=strcspn(pfrom_list_temp+pos1,"?");
	//				pfrom_list_temp[pos1bis+pos1]='\0';
	//				strcpy(pfrom_array_list_bis_rev_internal[j],pfrom_list_temp+pos1);
	//				pos1+=pos1bis+1;
	//
	//				pos6bis=strcspn(pto_list_temp+pos6,"?");
	//				pto_list_temp[pos6bis+pos6]='\0';
	//				strcpy(pto_array_list_bis_rev_internal[j],pto_list_temp+pos6);
	//				pos6+=pos6bis+1;
	//
	//			}
	//
	//		}	
	//
	//	}
	//	}
	//}
	//
	//if(lkid_list_temp!=NULL)
	//	free(lkid_list_temp);
	//
	//if(nfrom_list_temp!=NULL)
	//	free(nfrom_list_temp);
	//
	//if(lktype_temp!=NULL)
	//	free(lktype_temp);
	//
	//if(pfrom_list_temp!=NULL)
	//	free(pfrom_list_temp);
	//
	//if(pto_list_temp!=NULL)
	//	free(pto_list_temp);
	//
	//if(bidirectional_list_temp!=NULL)
	//	free(bidirectional_list_temp);
	//if(lkweight_list_temp!=NULL)
	//	free(lkweight_list_temp);
	//if(lkused_list_temp!=NULL)
	//	free(lkused_list_temp);
	//if(port_nbrfrom!=NULL)
	//	free(port_nbrfrom);
	//if(port_nbrto!=NULL)
	//	free(port_nbrto);
	//
	//status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
	//if(rescode!=0)
	//	OCIReportError(ociError,appliName, ErrMess,1); 
	//else
	//	OCIReportError(ociError,appliName, ErrMess,0); 
	//
	//return rescode;
	//}
	//
#if defined(__cplusplus) || defined(c_plusplus)
}
#endif