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

	/**********************************************************************************/
	/*****************************************************************************************/
	/**
	* Load the link type table of a given system , returning a int 
	* @param systemnameList : list of the subsystems (UPPER CASE) for which you want to load the connectivity, separated with a coma : VELO,DAQ,TFC. for instance. Put ALL, if you want to load the whole connectivity table.
	* @param len_array : length of the lkid_list array you have allocated (same as node_from_list,...,lktype_list) we return the length we need if too small)
	* @param lkid_list : list of lkid
	* @param node_from_list : list of node_from (corresponding to the lkid)
	* @param node_to_list : list of node_to (corresponding to the lkid)
	* @param port_nbrfrom_list : list of port_from (corresponding to the lkid)
	* @param port_nbrto_list : list of port_to (corresponding to the lkid)
	* @param portfrom_len : length of port_from 
	* @param portto_len : length of port_to 
	* @param bidirectional_list : list of bidrectional (corresponding to the lkid)
	* @param lkused_list : list of lkused (corresponding to the lkid)
	* @param lktype_list : list of lktype (corresponding to the lkid)
	* @param lkinfo_list : list of lkinfo (corresponding to the lkid)
	* @param lkinfo_len : len of the lkinfo_list. If too small we put the required size
	* @param ErrMess : error message in case of failure (NO_ROWS_SELECTED if table empty)
	* @return  0 if successful
	*/
	/*****************************************************************************/

	EXTERN_CONFDB int LoadConnectivityTable(char* systemnameList,int &len_array, int* lkid_list, int * node_from_list,int* node_to_list,int &portfrom_len,int &portto_len,char* port_nbrfrom_list, char* port_nbrto_list, int* bidirectional_list, int* lkused_list, int* lktype_list,char* lkinfo_list,int & lkinfo_len,char* ErrMess)
	{
		int i = 0;
		int j=0;
		int len=0;
		char appliName[100]="LoadConnectivityTable";
		sword status;
		OCIStmt *stmthp;
		OCIDefine* def[9];
		OCIParam *parmdp;
		int pfrom_null=0;
		int pto_null=0;
		int lkinfo_null=0;
		int linkid=0;
		int node_from=0;
		int rescode=0;
		int node_to=0;
		char* port_nbrfrom=NULL;
		char* port_nbrto=NULL;
		char* lkinfo_temp=NULL;
		int pfromlen=0;
		int ptolen=0;
		int pfromlen_temp=0;
		int ptolen_temp=0;
		int link_type=0; 
		int bidirectional_link_used=0;
		int trunk_lkid=0;
		int lkused=0;
		int link_weight=0;
		int* lkid_list_temp=NULL;
		int* nfrom_list_temp=NULL;
		int* nto_list_temp=NULL;
		int* lktype_temp=NULL;
		char* pfrom_list_temp=NULL;
		char* pto_list_temp=NULL;
		char* lkinfo_list_temp=NULL;
		int* bidirectional_list_temp=NULL;
		int* lkused_list_temp=NULL;
		int round_trip=0;
		int prefetch_rows=10000;
		char selectconn[1000];
		int pos1=0;
		int pos2=0;
		int pos3=0;
		int pos1bis=0;
		int pos2bis=0;
		int pos3bis=0;
		int pos5=0;
		int pos6=0;
		int pos7=0;
		char logmessage[100];
		int sysID;
		int lkinfolen=0;
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
			if(sysID==1)
				sprintf(selectconn,"select t.linkid,g.deviceid,f.deviceid,g.port_nbr||'|'||nvl(g.port_type,'none')||'?',f.port_nbr||'|'||nvl(f.port_type,'none')||'?',t.link_type,t.bidirectional_link_used,t.lkused,nvl(t.link_info,'none')||'?' from %s t,%s g,%s f where t.portidfrom=g.portid and t.portidto=f.portid ",MACRO_CONNECTIVITY_TABLE,PORT_TABLE,PORT_TABLE);
			else
			{
				if(sysID==0)
					sprintf(selectconn,"select t.linkid,g.deviceid,f.deviceid,g.port_nbr||'|'||nvl(g.port_type,'none')||'?',f.port_nbr||'|'||nvl(f.port_type,'none')||'?',t.link_type,t.bidirectional_link_used,t.lkused,nvl(t.link_info,'none')||'?' from %s t,%s g,%s f, %s l where t.portidfrom=g.portid and t.portidto=f.portid and mod(t.system_name,l.systemID)=0 and l.systemID in (%s) ",MACRO_CONNECTIVITY_TABLE,PORT_TABLE,PORT_TABLE,SUBSYSTEM_TABLE,sysIDlist);
				else
				{

					sprintf(selectconn,"select t.linkid,g.deviceid,f.deviceid,g.port_nbr||'|'||nvl(g.port_type,'none')||'?',f.port_nbr||'|'||nvl(f.port_type,'none')||'?',t.link_type,t.bidirectional_link_used,t.lkused,nvl(t.link_info,'none')||'?'  from %s t,%s g,%s f where t.portidfrom=g.portid and t.portidto=f.portid and mod(t.system_name,%d)=0  ",MACRO_CONNECTIVITY_TABLE,PORT_TABLE,PORT_TABLE,sysID);
				}
			}
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectconn,(ub4) strlen(selectconn),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}
		//std ::cout<<"value of selectconn "<<selectconn <<std::endl;
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
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
			/* The next two statements describe the select-list item, dept, and return its length to allocate the memory*/
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
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
			/* The next two statements describe the select-list item, dept, and return its length to allocate the memory*/
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 9);
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
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
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
			port_nbrfrom=(char*)malloc((pfromlen+1)*sizeof(char));
			if(port_nbrfrom!=NULL)
				status=OCIDefineByPos (stmthp, &def[3], ociError, 4,port_nbrfrom,pfromlen+1, SQLT_STR, &pfrom_null, 0, 0, OCI_DEFAULT);
			else
			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
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
			port_nbrto=(char*)malloc((ptolen+1)*sizeof(char));
			if(port_nbrto!=NULL)
				status=OCIDefineByPos(stmthp, &def[4], ociError, 5,port_nbrto,ptolen+1, SQLT_STR, &pto_null, 0, 0, OCI_DEFAULT);
			else
			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
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
		{
			lkinfo_temp=(char*)malloc((lkinfolen+1)*sizeof(char));
			if(lkinfo_temp!=NULL)
				status=OCIDefineByPos(stmthp, &def[8], ociError,9,lkinfo_temp,lkinfolen+1, SQLT_STR, &lkinfo_null, 0, 0, OCI_DEFAULT);
			else
			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				free(port_nbrfrom);
				free(port_nbrto);
				return -1;
			}
		}
		std ::cout<<"after defining"<<std::endl;	
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
			//pfromlen_temp=pfromlen*prefetch_rows;
			//ptolen_temp=ptolen*prefetch_rows;

			if(lkid_list_temp==NULL  || nfrom_list_temp==NULL || nto_list_temp==NULL)
			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");	
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
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
				if(lkinfo_temp!=NULL)
					free(lkinfo_temp);
				return -1;

			}
			lktype_temp=(int*)realloc(lktype_temp,prefetch_rows*sizeof(int));


			if(lktype_temp==NULL )
			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				if(lkid_list_temp!=NULL)
					free(lkid_list_temp);
				if(nfrom_list_temp!=NULL)
					free(nfrom_list_temp);
				if(nto_list_temp!=NULL)
					free(nto_list_temp);
				if(lktype_temp!=NULL)
					free(lktype_temp);

				if(port_nbrfrom!=NULL)
					free(port_nbrfrom);
				if(port_nbrto!=NULL)
					free(port_nbrto);
				if(lkinfo_temp!=NULL)
					free(lkinfo_temp);
				return -1;
			}
			bidirectional_list_temp=(int*)realloc(bidirectional_list_temp,prefetch_rows*sizeof(int));
			lkused_list_temp=(int*)realloc(lkused_list_temp,prefetch_rows*sizeof(int));
			if(bidirectional_list_temp==NULL  || lkused_list_temp==NULL)
			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				if(lkid_list_temp!=NULL)
					free(lkid_list_temp);
				if(nfrom_list_temp!=NULL)
					free(nfrom_list_temp);
				if(nto_list_temp!=NULL)
					free(nto_list_temp);
				if(lktype_temp!=NULL)
					free(lktype_temp);


				if(bidirectional_list_temp!=NULL)
					free(bidirectional_list_temp);
				if(lkused_list_temp!=NULL)
					free(lkused_list_temp);
				if(port_nbrfrom!=NULL)
					free(port_nbrfrom);
				if(port_nbrto!=NULL)
					free(port_nbrto);
				if(lkinfo_temp!=NULL)
					free(lkinfo_temp);
				return -1;
			}
		}
		pos7=0;
		pos2=0;
		pos2bis=0;
		while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
		{
			Format_output(pfrom_null, port_nbrfrom, logmessage, '?');
			Format_output(pto_null, port_nbrto, logmessage, '?');
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);
			if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				lkid_list_temp[i]=linkid;
				nfrom_list_temp[i]=node_from;
				nto_list_temp[i]=node_to;
				bidirectional_list_temp[i]=bidirectional_link_used;
				lkused_list_temp[i]=lkused;
				lktype_temp[i]=link_type;
				pos1=strcspn(port_nbrfrom,"?");
				pos1bis=strcspn(port_nbrto,"?");
				pos5=strcspn(lkinfo_temp,"?");
				port_nbrfrom[pos1]='\0';
				port_nbrto[pos1bis]='\0';
				lkinfo_temp[pos5]='\0';
				pos6=pos7;
				pos7+=pos5+1;
				pos3=pos2;
				pos3bis=pos2bis;
				pos2+=pos1+1;
				pos2bis+=pos1bis+1;
				pfrom_list_temp=(char*)realloc(pfrom_list_temp,(pos2)*sizeof(char));
				pto_list_temp=(char*)realloc(pto_list_temp,(pos2bis)*sizeof(char));
				lkinfo_list_temp=(char*)realloc(lkinfo_list_temp,(pos7)*sizeof(char));
				if(pfrom_list_temp==NULL ||pto_list_temp==NULL||lkinfo_list_temp==NULL)
				{
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					if(lkid_list_temp!=NULL)
						free(lkid_list_temp);
					if(nfrom_list_temp!=NULL)
						free(nfrom_list_temp);
					if(nto_list_temp!=NULL)
						free(nto_list_temp);
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
					if(lkinfo_temp!=NULL)
						free(lkinfo_temp);
					if(lkinfo_list_temp!=NULL)
						free(lkinfo_list_temp);
					return -1;
				}
				else
				{
					memcpy(pfrom_list_temp+pos3,port_nbrfrom,pos1+1);
					memcpy(pto_list_temp+pos3bis,port_nbrto,pos1bis+1);
					memcpy(lkinfo_list_temp+pos6,lkinfo_temp,pos5+1);
				}
				i++;
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
				if(lkid_list_temp==NULL  || nfrom_list_temp==NULL || nto_list_temp==NULL)
				{
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					if(lkid_list_temp!=NULL)
						free(lkid_list_temp);
					if(nfrom_list_temp!=NULL)
						free(nfrom_list_temp);
					if(nto_list_temp!=NULL)
						free(nto_list_temp);
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
					if(lkinfo_temp!=NULL)
						free(lkinfo_temp);
					if(lkinfo_list_temp!=NULL)
						free(lkinfo_list_temp);
					return -1;
				}
				if(lktype_temp==NULL)
				{
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					if(lkid_list_temp!=NULL)
						free(lkid_list_temp);
					if(nfrom_list_temp!=NULL)
						free(nfrom_list_temp);
					if(nto_list_temp!=NULL)
						free(nto_list_temp);
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
					if(lkinfo_temp!=NULL)
						free(lkinfo_temp);
					if(lkinfo_list_temp!=NULL)
						free(lkinfo_list_temp);
					return -1;
				}
				if(bidirectional_list_temp==NULL  || lkused_list_temp==NULL)
				{
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					if(lkid_list_temp!=NULL)
						free(lkid_list_temp);
					if(nfrom_list_temp!=NULL)
						free(nfrom_list_temp);
					if(nto_list_temp!=NULL)
						free(nto_list_temp);
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
					if(lkinfo_temp!=NULL)
						free(lkinfo_temp);
					if(lkinfo_list_temp!=NULL)
						free(lkinfo_list_temp);
					return -1;
				}
			}
		}
		//std ::cout<<"value of i="<<i << " and value of rescode"<<rescode<<std::endl;	
		if(status!=OCI_SUCCESS && status != OCI_SUCCESS_WITH_INFO)
		{
			if(rescode==0 && i==0)
			{
				rescode=ShowErrors (status, ociError, "OCIFETCH third stmt unsuccessful");
				if(lkid_list_temp!=NULL)
					free(lkid_list_temp);
				if(nfrom_list_temp!=NULL)
					free(nfrom_list_temp);
				if(nto_list_temp!=NULL)
					free(nto_list_temp);
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
				if(lkinfo_temp!=NULL)
					free(lkinfo_temp);
				if(lkinfo_list_temp!=NULL)
					free(lkinfo_list_temp);
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName, "NO_ROWS_SELECTED",ErrMess,1);
				return 0;

			}
		}
		std ::cout<<"value of len_array="<<len_array <<" and value of i="<<i<<std::endl;
		if(rescode==0)
		{
			if(len_array>i-1)
			{
				for(j=0;j<i;j++)
				{
					lkid_list[j]=lkid_list_temp[j];
					node_from_list[j]=nfrom_list_temp[j];
					node_to_list[j]=nto_list_temp[j];
					bidirectional_list[j]=bidirectional_list_temp[j];
					lkused_list[j]=lkused_list_temp[j];
					lktype_list[j]=lktype_temp[j];
				}
				len_array=i;
				//port_nbrfrom_list[j]=pfrom_list_temp[j];
				//port_nbrto_list[j]=pto_list_temp[j];
			}
			else
			{
				rescode=-1;
				len_array=i;
			}
			//std ::cout<<"value of pfromarray="<<portfrom_len <<" and value of pos2="<<pos2<<std::endl;

			if(portfrom_len>pos2-1)
			{

				memcpy(port_nbrfrom_list,pfrom_list_temp,pos2);
				portfrom_len=pos2;
			}
			else
			{
				rescode=-1;
				portfrom_len=pos2;
			}
			//std ::cout<<"value of ptoarray="<<portto_len <<" and value of pos2bis="<<pos2bis<<std::endl;
			if(portto_len>pos2bis-1)
			{

				memcpy(port_nbrto_list,pto_list_temp,pos2bis);
				portto_len=pos2bis;
			}
			else
			{
				rescode=-1;
				portto_len=pos2bis;
			}
			if(lkinfo_len>pos6-1)
			{

				memcpy(lkinfo_list,lkinfo_list_temp,pos6);
				lkinfo_len=pos6;
			}
			else
			{
				rescode=-1;
				lkinfo_len=pos6;
			}
		}
		//std ::cout<<"value of pfromlist="<<pfrom_list_temp <<std::endl;

		if(lkid_list_temp!=NULL)
			free(lkid_list_temp);

		if(nfrom_list_temp!=NULL)
			free(nfrom_list_temp);

		if(nto_list_temp!=NULL)
			free(nto_list_temp);

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


		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 
		else
			OCIReportError(ociError,appliName, ErrMess,0); 

		return rescode;

	}
	/*****************************************************************************************/
	/**********************************************************************************/
	/*****************************************************************************************/
	/**
	* Match the deviceid devicename table of a given system , returning a int
	* @param systemnameList : name of the subsystem.    
	* @param len_did : length of the devid_list array you have allocated we return the length we need if too small)
	* @param len_dname : length of the devname_list  you have allocated we return the length we need if too small)
	* @param devid_list : list of deviceid
	* @param devname_list : list of devname (corresponding to the deviceid)
	* @param ErrMess : error message in case of failure (NO_ROWS_SELECTED if table empty)
	* @return  0 if successful
	*/
	/*****************************************************************************/

	/**********************************************************************************/


	EXTERN_CONFDB int MatchDeviceIDDeviceName(char* systemnameList,int &len_did, int* devid_list,int& len_dname,char* devname_list,char* ErrMess)
	{
		int i = 0;
		int j=0;
		int len=0;
		char appliName[100]="MatchDeviceIDDeviceName";
		sword status;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def[2];
		int devid=0;
		OCIParam *parmdp;
		int rescode=0;
		int deviceid=0;
		char* devicename=NULL;
		int devlen=0;
		int devlen_currentsize=0;
		int* devid_list_temp=NULL;
		char* devname_list_temp=NULL;
		int dname_null=0;
		int round_trip=0;
		int prefetch_rows=10000;
		char selectconn[1000];
		int pos1=0;
		int pos2=0;
		int pos3=0;
		char logmessage[100];
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		char sysIDlist[100];
		int sysID=0;
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
			if(sysID==1)
				sprintf(selectconn,"select deviceid,devicename||'|' from %s where deviceid!=-1",LOGICAL_DEVICE_TABLE);
			else
			{
				if(sysID==0)
					sprintf(selectconn,"select t.deviceid,t.devicename||'|' from %s t,%s e where t.deviceid!=-1 and mod(t.system_name,e.systemID)=0 and e.systemID in (%s)",LOGICAL_DEVICE_TABLE,SUBSYSTEM_TABLE,sysIDlist);
				else
					sprintf(selectconn,"select t.deviceid,t.devicename||'|' from %s t where t.deviceid!=-1 and mod(t.system_name,%d)=0 ",LOGICAL_DEVICE_TABLE,sysID);
			}
			//std::cout<<"selectconn "<<selectconn <<std::endl;
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectconn,(ub4) strlen(selectconn),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIPrepare unsuccessful");
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
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
			/* The next two statements describe the select-list item, dept, and return its length to allocate the memory*/
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 2);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet1 unsuccessful");
		}
		else
		{
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &devlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);
			devicename=(char*)malloc((devlen+1)*sizeof(char));
			//std::cout<<"value of devlen "<<devlen << std::endl;
			if(devicename==NULL)
			{
				if(rescode==0)	
				{
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					return -1;
				}

			}

		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtAttributeParam unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[0], ociError,1,&deviceid,sizeof(deviceid), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[1], ociError,2,(dvoid*)devicename,devlen+1, SQLT_STR, &dname_null, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
		{
			i=0;
			round_trip=1;
			devid_list_temp=(int*)realloc(devid_list_temp,prefetch_rows*sizeof(int));
			if(devid_list_temp==NULL)
			{
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				return -1;
			}
		}

		pos2=0;
		while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
		{
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);
			if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				devid_list_temp[i]=deviceid;
				Format_output(dname_null, devicename, logmessage, '|');
				pos1=strcspn(devicename,"|");
				devicename[pos1]='\0';
				pos3=pos2;
				pos2+=pos1+1;

				devname_list_temp=(char*) realloc(devname_list_temp,pos2*sizeof(char));
				if(devname_list_temp==NULL)
				{
					free(devicename);
					free(devid_list_temp);
					rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					return -1;	
				}
				else
				{
					memcpy(devname_list_temp+pos3,devicename,pos1+1);
				}
				i++;
			}
			if(i>0 && (i%prefetch_rows)==0)
			{
				round_trip++;
				devid_list_temp=(int*)realloc(devid_list_temp,round_trip*prefetch_rows*sizeof(int));
				if(devid_list_temp==NULL)
				{
					free(devname_list_temp);
					free(devicename);
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					return -1;
				}
			}
		}

		if(status!=OCI_SUCCESS && status != OCI_SUCCESS_WITH_INFO)
		{
			if(rescode==0 && i==0)
			{
				rescode=ShowErrors (status, ociError, "OCIFETCH  stmt unsuccessful");
				GetErrorMess(appliName, "NO_ROWS_SELECTED",ErrMess,1);
				if(devid_list_temp!=NULL)
					free(devid_list_temp);

				if(devname_list_temp!=NULL)
					free(devname_list_temp);
				free(devicename);
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				return 0;
			}
		}

		if(rescode==0)
		{
			if(len_did>i-1)
			{
				for(j=0;j<i;j++)
				{
					devid_list[j]=devid_list_temp[j];
				}
				len_did=i;
			}
			else
			{
				rescode=-1;
				len_did=i;
			}
			if(len_dname>pos2)
			{

				memcpy(devname_list,devname_list_temp,pos2);
				len_dname=pos2;
			}
			else
			{
				rescode=-1;
				len_dname=pos2;
			}
		}

		if(devid_list_temp!=NULL)
			free(devid_list_temp);

		if(devname_list_temp!=NULL)
			free(devname_list_temp);
		free(devicename);
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 
		else
			OCIReportError(ociError,appliName, ErrMess,0); 

		return rescode;

	}
	/**************************************************************************************************/
	/**********************************************************************************/
	/**********************************************************************************/
	/*****************************************************************************************/
	/**
	* Load the device type table of a given system , returning a int
	* @param systemnameList : list of the subsystems (UPPER CASE) for which you want to load the devicetypetable, separated with a coma : VELO,DAQ,TFC. for instance. Put ALL, if you want to load the whole connectivity table.
	* @param len_did : length of the devtypename_list you have allocated (we return the length we need if too small)
	* @param devtypename_list : list of devtypename 
	* @param len_array : length of the devtypeID_list you have allocated (we return the length we need if too small)
	* @param devtypeID_list : list of devtypeid 
	* @param ErrMess : error message in case of failure (NO_ROWS_SELECTED if table empty)
	* @return  0 if successful
	*/
	/*****************************************************************************/


	EXTERN_CONFDB int LoadDeviceTypeTable(char* systemnameList,int &len_did, char* devtypename_list,int &len_array,int* devtypeID_list, char* ErrMess)
	{
		int i = 0;
		int j=0;
		int len=0;
		char appliName[100]="LoadDeviceTypeTable";
		sword status;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def[2];
		int devid=0;
		OCIParam *parmdp;
		int rescode=0;
		char* devicename=NULL;
		int devlen=0;
		int devlen_currentsize=0;
		char* devname_list_temp=NULL;
		int dname_null=0;
		int round_trip=0;
		int prefetch_rows=10000;
		char selectconn[1000];
		int pos1=0;
		int pos2=0;
		int pos3=0;
		int devtypeID=0;
		int *dtypeID_list=NULL;
		char logmessage[100];
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		char sysIDlist[100];
		int sysID=0;
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
			if(sysID==1)
				sprintf(selectconn,"select devicetypeid,devicetype||'|' from %s ",DEVICETYPE_TABLE);
			else
			{
				if(sysID==0)
					sprintf(selectconn,"select t.devicetypeid,t.devicetype||'|' from %s t,%s f where mod(t.system_name,f.systemID)=0 and f.systemID in('%s')",DEVICETYPE_TABLE, SUBSYSTEM_TABLE,sysIDlist);
				else
					sprintf(selectconn,"select devicetypeid,devicetype||'|' from %s where mod(system_name,%d)=0 ",DEVICETYPE_TABLE,sysID);
			}
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectconn,(ub4) strlen(selectconn),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIPrepare unsuccessful");
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
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
			/* The next two statements describe the select-list item, dept, and return its length to allocate the memory*/
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 2);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet1 unsuccessful");
		}
		else
		{
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &devlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);
			devicename=(char*)malloc((devlen+1)*sizeof(char));
			//std::cout<<"value of devlen "<<devlen << std::endl;
			if(devicename==NULL)
			{
				if(rescode==0)	
				{
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					return -1;
				}

			}

		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[0], ociError,1,(dvoid*)&devtypeID,sizeof(int), SQLT_INT, 0, 0, 0, OCI_DEFAULT);
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[1], ociError,2,(dvoid*)devicename,devlen+1, SQLT_STR, &dname_null, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
		{
			i=0;
			round_trip=1;
			dtypeID_list=(int*)realloc(dtypeID_list,prefetch_rows*sizeof(int));
			if(dtypeID_list==NULL)
			{
				free(devicename);
				rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				return -1;
			}
		}

		pos2=0;

		while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
		{
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);
			if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{

				Format_output(dname_null, devicename, logmessage, '|');
				pos1=strcspn(devicename,"|");
				devicename[pos1]='\0';
				pos3=pos2;
				pos2+=pos1+1;

				devname_list_temp=(char*) realloc(devname_list_temp,pos2*sizeof(char));
				if(devname_list_temp==NULL)
				{
					free(devicename);
					free(dtypeID_list);
					rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					return -1;	
				}
				else
				{
					memcpy(devname_list_temp+pos3,devicename,pos1+1);
					dtypeID_list[i]=devtypeID;
				}
				i++;
				if(i%prefetch_rows==0)
				{
					round_trip++;
					dtypeID_list=(int*)realloc(dtypeID_list,round_trip*prefetch_rows*sizeof(int));
					if(dtypeID_list==NULL)
					{
						free(devicename);
						free(devname_list_temp);
						rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
						status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
						GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
						return -1;
					}

				}
			}

		}

		if(status!=OCI_SUCCESS && status != OCI_SUCCESS_WITH_INFO)
		{
			if(rescode==0 && i==0)
			{
				if(devname_list_temp!=NULL)
					free(devname_list_temp);
				free(devicename);
				if(dtypeID_list!=NULL)
					free(dtypeID_list);
				rescode=ShowErrors (status, ociError, "OCIFETCH third stmt unsuccessful");
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName, "NO_ROWS_SELECTED",ErrMess,1);
				return 0;

			}
		}

		if(rescode==0)
		{

			if(len_did>pos2-1)
			{

				memcpy(devtypename_list,devname_list_temp,pos2);
				len_did=pos2;
				if(len_array>i)
				{
					len_array=i;
					for(j=0;j<i;j++)
						devtypeID_list[j]=dtypeID_list[j];
				}
				else
				{
					len_array=i;
					rescode=-1;
				}
			}
			else
			{
				rescode=-1;
				len_did=pos2;
				len_array=i;
			}
		}


		if(devname_list_temp!=NULL)
			free(devname_list_temp);
		free(devicename);
		if(dtypeID_list!=NULL)
			free(dtypeID_list);
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 
		else
			OCIReportError(ociError,appliName, ErrMess,0); 

		return rescode;

	}

	/****************************************************/
	/**********************************************************************************/
	/*****************************************************************************************/
	/**
	* Match the lkid lktypename table of a given system , returning a int
	* @param len_lkid : length of the lkid_list array you have allocated we return the length we need if too small)
	* @param len_lkname : length of the lktypename_list  you have allocated we return the length we need if too small)
	* @param lkid_list : list of lktypeid
	* @param lktypename_list : list of lktypename (corresponding to the deviceid)
	* @param ErrMess : error message in case of failure (NO_ROWS_SELECTED if table empty)
	* @return  0 if successful
	*/
	/*****************************************************************************/


	EXTERN_CONFDB int LoadLinkTypeTable(int &len_lkid, int* lkid_list,int& len_lkname,char* lktypename_list,char* ErrMess)
	{
		int i = 0;
		int j=0;
		int len=0;
		char appliName[100]="LoadLinkTypeTable";
		sword status;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def[2];
		int devid=0;
		OCIParam *parmdp;
		int rescode=0;
		int lkid=0;
		char* lkname=NULL;
		int devlen=0;
		int devlen_currentsize=0;
		int* lkid_list_temp=NULL;
		char* lkname_list_temp=NULL;
		int dname_null=0;
		int round_trip=0;
		int prefetch_rows=10000;
		char selectconn[1000];
		int pos1=0;
		int pos2=0;
		int pos3=0;
		char logmessage[100];
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
			sprintf(selectconn,"select linktypeID,link_name||'|' from %s",LINKTYPE_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectconn,(ub4) strlen(selectconn),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIPrepare unsuccessful");
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
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
			/* The next two statements describe the select-list item, dept, and return its length to allocate the memory*/
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 2);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet1 unsuccessful");
		}
		else
		{
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &devlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);
			lkname=(char*)malloc((devlen+1)*sizeof(char));
			//std::cout<<"value of devlen "<<devlen << std::endl;
			if(lkname==NULL)
			{
				if(rescode==0)	
				{
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					return -1;
				}

			}

		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtAttributeParam unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[0], ociError,1,&lkid,sizeof(lkid), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIDefineByPos unsuccessful");
		}
		else
			status=OCIDefineByPos (stmthp, &def[1], ociError,2,(dvoid*)lkname,devlen+1, SQLT_STR, &dname_null, 0, 0, OCI_DEFAULT);

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
			if(lkid_list_temp==NULL)
			{
				free(lkname);
				rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				return -1;
			}
		}

		pos2=0;

		while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
		{
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1, OCI_DEFAULT);
			if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				lkid_list_temp[i]=lkid;
				Format_output(dname_null, lkname, logmessage, '|');
				pos1=strcspn(lkname,"|");
				lkname[pos1]='\0';
				pos3=pos2;
				pos2+=pos1+1;

				lkname_list_temp=(char*) realloc(lkname_list_temp,pos2*sizeof(char));
				if(lkname_list_temp==NULL)
				{
					free(lkname);
					free(lkid_list_temp);
					rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					return -1;	
				}
				else
				{
					memcpy(lkname_list_temp+pos3,lkname,pos1+1);
				}
				i++;
			}
			if(i>0 && (i%prefetch_rows)==0)
			{
				round_trip++;
				lkid_list_temp=(int*)realloc(lkid_list_temp,round_trip*prefetch_rows*sizeof(int));
				if(lkid_list_temp==NULL)
				{
					free(lkname_list_temp);
					free(lkname);
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					return -1;
				}
			}
		}

		if(status!=OCI_SUCCESS && status != OCI_SUCCESS_WITH_INFO)
		{
			if(rescode==0 && i==0)
			{
				rescode=ShowErrors (status, ociError, "OCIFETCH third stmt unsuccessful");
				GetErrorMess(appliName, "NO_ROWS_SELECTED",ErrMess,1);
				if(lkid_list_temp!=NULL)
					free(lkid_list_temp);
				free(lkname);
				if(lkname_list_temp!=NULL)
					free(lkname_list_temp);

				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				return 0;
			}
		}

		if(rescode==0)
		{
			if(len_lkid>i-1)
			{
				for(j=0;j<i;j++)
				{
					lkid_list[j]=lkid_list_temp[j];
				}
				len_lkid=i;
			}
			else
			{
				rescode=-1;
				len_lkid=i;
			}
			if(len_lkname>pos2-1)
			{

				memcpy(lktypename_list,lkname_list_temp,pos2);
				len_lkname=pos2;
			}
			else
			{
				rescode=-1;
				len_lkname=pos2;
			}
		}

		if(lkid_list_temp!=NULL)
			free(lkid_list_temp);
		free(lkname);
		if(lkname_list_temp!=NULL)
			free(lkname_list_temp);

		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 
		else
			OCIReportError(ociError,appliName, ErrMess,0); 

		return rescode;

	}
	/**************************************************************************************************/





#if defined(__cplusplus) || defined(c_plusplus)
}
#endif