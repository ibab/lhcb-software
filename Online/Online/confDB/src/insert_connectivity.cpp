/********************************************************************************/
//	Author: L.Abadie
// version:v3.5
// changes: handling error such that can be extended to C++: (create a class SQLException) (strlen ck + update stmt)
// file which performs bulk collect insert for the connectivity table
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

	/************************global variables for the connectivity***********************/
	int FIRST_TIME_CONNECTIVITY=0;
	char* _NodeFromList;
	int _max_nodefromList_len;
	int _nodefromList_len;
	int _nodefromNbElement;

	int* _nodefrom_nullvalue;
	int _nodefromNbEl;

	char* _portnbfromList;
	int _max_portnbfromList_len;
	int _portnbfromList_len;
	int _portnbfromNbElement;

	int* _portnbfrom_nullvalue;
	int _portnbfromNbEl;

	char* _NodeToList;
	int _max_nodetoList_len;
	int _nodetoList_len;
	int _nodetoNbElement;

	int* _nodeto_nullvalue;
	int _nodetoNbEl;

	char* _portnbtoList;
	int _max_portnbtoList_len;
	int _portnbtoList_len;
	int _portnbtoNbElement;

	int* _portnbto_nullvalue;
	int _portnbtoNbEl;

	char* _portypetoList;
	int _max_portypetoList_len;
	int _portypetoList_len;
	int _portypetoNbElement;

	int* _portypeto_nullvalue;
	int _portypetoNbEl;

	char* _portypefromList;
	int _max_portypefromList_len;
	int _portypefromList_len;
	int _portypefromNbElement;

	int* _portypefrom_nullvalue;
	int _portypefromNbEl;


	char* _lkinfoList;
	int _max_lkinfoList_len;
	int _lkinfoList_len;
	int _lkinfoNbElement;

	int* _lkinfo_nullvalue;
	int _lkinfoNbEl;


	char* _LinkTypeList2;
	int _max_linktypeList_len2;
	int _linktypeList_len2;
	int _linktypeNbElement2;

	int* _linktype_nullvalue;
	int _linktypeNbEl;



	int* _BidirectionalList;
	int _bidirectionalNbElement;


	/******************************************************/
	int freeConnectivity()
	{
		int status=0;
		_LinkTypeList2=(char*)realloc(_LinkTypeList2,0*sizeof(char));
		_LinkTypeList2=NULL;

		_NodeToList=(char*)realloc(_NodeToList,0*sizeof(char));
		_NodeToList=NULL;

		_portnbtoList=(char*)realloc(_portnbtoList,0*sizeof(char));
		_portnbtoList=NULL;

		_portypetoList=(char*)realloc(_portypetoList,0*sizeof(char));
		_portypetoList=NULL;

		_NodeFromList=(char*)realloc(_NodeFromList,0*sizeof(char));
		_NodeFromList=NULL;

		_portnbfromList=(char*)realloc(_portnbfromList,0*sizeof(char));
		_portnbfromList=NULL;

		_portypefromList=(char*)realloc(_portypefromList,0*sizeof(char));
		_portypefromList=NULL;

		_lkinfoList=(char*)realloc(_lkinfoList,0*sizeof(char));
		_lkinfoList=NULL;

		_BidirectionalList=(int*)realloc(_BidirectionalList,0*sizeof(int));
		_BidirectionalList=NULL;



		_nodeto_nullvalue=(int*)realloc(_nodeto_nullvalue,0*sizeof(int));
		_nodeto_nullvalue=NULL;

		_nodefrom_nullvalue=(int*)realloc(_nodefrom_nullvalue,0*sizeof(int));
		_nodefrom_nullvalue=NULL;

		_portnbto_nullvalue=(int*)realloc(_portnbto_nullvalue,0*sizeof(int));
		_portnbto_nullvalue=NULL;

		_portnbfrom_nullvalue=(int*)realloc(_portnbfrom_nullvalue,0*sizeof(int));
		_portnbfrom_nullvalue=NULL;

		_portypeto_nullvalue=(int*)realloc(_portypeto_nullvalue,0*sizeof(int));
		_portypeto_nullvalue=NULL;

		_portypefrom_nullvalue=(int*)realloc(_portypefrom_nullvalue,0*sizeof(int));
		_portypefrom_nullvalue=NULL;

		_lkinfo_nullvalue=(int*)realloc(_lkinfo_nullvalue,0*sizeof(int));
		_lkinfo_nullvalue=NULL;

		_linktype_nullvalue=(int*)realloc(_linktype_nullvalue,0*sizeof(int));
		_linktype_nullvalue=NULL;
		FIRST_TIME_CONNECTIVITY=0;


		return status;
	}

	/**
	* Insert Multiple MacroLink  (i.e. these are physical cables)returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param node_from : name of the device starting the link
	* @param node_to : name of the device ending the link
	* @param port_nbfrom : port nb of the device from
	* @param port_nbto : port nb of the device to
	* @param port_typefrom : port type (used for the DAQ: as one physical port is considered as several functional interfaces). Put "", if none.
	* @param port_typeto : port type (used for the DAQ: as one physical port is considered as several functional interfaces). Put "", if none.
	* @param link_type : name of the link type
	* @param link_information : some comments on the links. Put "" if none.
	* @param bidirectional_link_used :  1 if it's a bidirectional link (it means that if it is consider as unidirectional some paths won't be found), 0 otherwise. 
	* @param first_time1 : 1 if it's your MultipleLink first insert (save it into the database), 0 otherwise
	* @param last_rows1 : 1 if it's your MultipleLink last insert (save it into the database), 0 otherwise. Maximum in one go: 10000 rows. (we force the insert)
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/ 
	EXTERN_CONFDB
		int InsertMultipleMacroLinks(char* node_from,char* node_to,char* port_nbfrom,char* port_nbto,char* port_typefrom,char* port_typeto,char* link_type,char* link_information,int bidirectional_link_used,int first_time1, int last_rows1,char* ErrMess)
	{
		int numrows=0;
		int num_errs1=0;
		char sqlstmt[2500];
		int i=0;
		char appliName[100]="InsertMultipleMacroLinks";
		OCIStmt *hstmt;
		OCIBind  *bndp2[9];
		OCIBind  *bndp3 = (OCIBind *) 0; 
		int res_query=0;
		sword status=0;
		char* nodefrom=NULL;
		char* nodeto=NULL;
		char* portnbfrom=NULL;
		char* portypefrom=NULL;
		char* portnbto=NULL;
		char* portypeto=NULL;
		char* lkinfo=NULL;
		char* linktype=NULL;
		int rescode=0;
		char res_query1[100]="lll";
		char seq_name[100]="lhcb_macroconnseq.nextval";
		int last_rows=last_rows1;

		OCIError* ociError1=0 ;
		int* numrows_inserted=NULL;

		int nodefrom_null=null_charptr(node_from);
		int nodeto_null=null_charptr(node_to);
		int portnbfrom_null=null_charptr(port_nbfrom);
		int portnbto_null=null_charptr(port_nbto);
		int portypefrom_null=null_charptr(port_typefrom);
		int portypeto_null=null_charptr(port_typeto);
		int lkinfo_null=null_charptr(link_information);
		int linktype_null=null_charptr(link_type);
		int force_insert=0;
		int first_time=first_time1;
		char port_typefrom1[100];
		if(port_typefrom!=NULL)
		{
			if(strlen(port_typefrom)<1)
				strcpy(port_typefrom1,"none");
			else
				strcpy(port_typefrom1,port_typefrom);
		}
		else
		{
			strcpy(port_typefrom1,"none");
		}
		char port_typeto1[100];
		if(port_typeto!=NULL)
		{
			if(strlen(port_typeto)<1)
				strcpy(port_typeto1,"none");
			else
				strcpy(port_typeto1,port_typeto);
		}
		else
		{
			strcpy(port_typeto1,"none");
		}
		//std:://cout<<"value of port_typeto1 "<<port_typeto1<<"value of port_typeto"<<port_typeto <<std::endl;
		if(FIRST_TIME_CONNECTIVITY==1 && _NodeFromList==NULL) //means new loop in the mass insertion (by 10000)
		{
			first_time=1; //need to put to 1, as if new cache

		}
		if(first_time==1)
		{

			status+=freeConnectivity();
			FIRST_TIME_CONNECTIVITY=1;

		}
		res_query=AppendString(node_from,_NodeFromList, _nodefromList_len,_nodefromNbElement,_max_nodefromList_len,first_time);
		res_query+=AppendString(node_to,_NodeToList,_nodetoList_len,_nodetoNbElement,_max_nodetoList_len,first_time);
		res_query+=AppendString(link_type,_LinkTypeList2,_linktypeList_len2,_linktypeNbElement2,_max_linktypeList_len2,first_time);

		res_query=AppendString(port_nbfrom,_portnbfromList, _portnbfromList_len,_portnbfromNbElement,_max_portnbfromList_len,first_time);
		res_query+=AppendString(port_nbto,_portnbtoList,_portnbtoList_len,_portnbtoNbElement,_max_portnbtoList_len,first_time);
		res_query+=AppendString(link_information,_lkinfoList,_lkinfoList_len,_lkinfoNbElement,_max_lkinfoList_len,first_time);

		res_query=AppendString(port_typefrom1,_portypefromList, _portypefromList_len,_portypefromNbElement,_max_portypefromList_len,first_time);
		res_query+=AppendString(port_typeto1,_portypetoList,_portypetoList_len,_portypetoNbElement,_max_portypetoList_len,first_time);

		res_query+=AppendInt(bidirectional_link_used,_BidirectionalList,_bidirectionalNbElement,first_time);


		res_query+=AppendInt(nodeto_null,_nodeto_nullvalue,_nodetoNbEl,first_time);
		res_query+=AppendInt(linktype_null,_linktype_nullvalue,_linktypeNbEl,first_time);
		res_query+=AppendInt(nodefrom_null,_nodefrom_nullvalue,_nodefromNbEl,first_time);
		res_query+=AppendInt(portnbto_null,_portnbto_nullvalue,_portnbtoNbEl,first_time);
		res_query+=AppendInt(portnbfrom_null,_portnbfrom_nullvalue,_portnbfromNbEl,first_time);
		res_query+=AppendInt(portypeto_null,_portypeto_nullvalue,_portypetoNbEl,first_time);
		res_query+=AppendInt(portypefrom_null,_portypefrom_nullvalue,_portypefromNbEl,first_time);
		res_query+=AppendInt(lkinfo_null,_lkinfo_nullvalue,_lkinfoNbEl,first_time);


		if(_nodefromNbElement==MAXROWS && last_rows!=1)
		{
			last_rows=1;
			force_insert=1;
		}
		status=res_query;
		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME_CONNECTIVITY==1)
		{
			int len_host=LOGIN_LEN;
			char login[LOGIN_LEN];
			char host[LOGIN_LEN];
			char* errmessg1=(char*)malloc(ERROR_BUFFER*sizeof(char));
			int sysID=GetSubsystemID("DAQ,TFC");
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
				rescode=-1;
				status+=freeConnectivity();
				if(nodefrom!=NULL) //means not null otehriwse it's -1
					free(nodefrom);
				if(nodeto!=NULL)
					free(nodeto);
				if(linktype!=NULL)
					free(linktype);
				if(portnbfrom!=NULL)
					free(portnbfrom);
				if(portnbto!=NULL)
					free(portnbto);
				if(portypefrom!=NULL)
					free(portypefrom);
				if(portypeto!=NULL)
					free(portypeto);
				if(lkinfo!=NULL)
					free(lkinfo);
				free(errmessg1);
				free(errmessg2);
				return rescode;
			}

			//need to proceed with messages

			status =OCIHandleAlloc (ociEnv, (void**)&hstmt, OCI_HTYPE_STMT , 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIHandleAlloc unsuccessful");
			}
			else
			{
				sprintf(sqlstmt,"BEGIN insert ALL into %s (linkid,portidfrom,portidto,link_type,bidirectional_link_used,link_weight,lkused,link_info,author,user_update,terminal_name) values (%s,portidfrom,portidto,lknb,:biused, lkweight,1,:lkinfo,'%s','%s','%s') select f.portid as portidfrom,g.portid as portidto,s.linktypeID as lknb,decode(m.node,1,l.node+m.node+1,l.node+m.node) as lkweight from %s f,%s g,%s l,%s m,%s s ,%s k where f.deviceid=l.deviceid and l.devicename=:nfrom and g.deviceid=m.deviceid and m.devicename=:nto and s.link_name=:lktype and f.port_nbr=:portnbfrom and f.port_way=2 and nvl(f.port_type,'none')=:ptypefrom and g.port_nbr=:portnbto and g.port_way=1 and nvl(g.port_type,'none')=:ptypeto and mod(l.system_name,k.systemID)=0 and mod(m.system_name,k.systemID)=0 and rownum=1; if %s=1 then update %s r set r.system_name=(select case when ceil(power(10,sum(distinct log(10,t.systemID))))-power(10,sum(distinct log(10,t.systemID)))>0.5  then floor(power(10,sum(distinct log(10,t.systemid)))) else ceil(power(10,sum(distinct log(10,t.systemid)))) end from %s t,%s o,%s d,%s p,%s h where t.systemid not in (select q.systemid from %s v, %s q where mod(v.systemid,q.systemid)=0 and v.systemid!=q.systemid) and mod(o.system_name,t.systemid)=0 and mod(d.system_name,t.systemid)=0 and o.deviceid=p.deviceid and p.portid=r.portidfrom and r.portidto=h.portid and h.deviceid=d.deviceid) where r.system_name=-1 and r.portidfrom in (select v.portid from %s v,%s b where v.deviceid=b.deviceid and b.devicename=:nfrom) and r.portidto in (select v.portid from %s v,%s b where v.deviceid=b.deviceid and b.devicename=:nto);  end if;  :numrows:=%s; END;",MACRO_CONNECTIVITY_TABLE,seq_name,login,login,host,PORT_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,LINKTYPE_TABLE,SUBSYSTEM_TABLE,SQLROWCOUNT,MACRO_CONNECTIVITY_TABLE,SUBSYSTEM_TABLE,LOGICAL_DEVICE_TABLE,LOGICAL_DEVICE_TABLE,PORT_TABLE,PORT_TABLE,SUBSYSTEM_TABLE,SUBSYSTEM_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE,PORT_TABLE,LOGICAL_DEVICE_TABLE,SQLROWCOUNT);	
				status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
				//std:://cout<<"value of stmt "<<sqlstmt <<" and length="<<strlen(sqlstmt)<<std::endl;
			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
			{
				nodefrom=(char*)malloc(_nodefromNbElement*_max_nodefromList_len*sizeof(char));
				if(nodefrom!=NULL)
					status+=NormalizeVector(_NodeFromList, _nodefromNbElement,_max_nodefromList_len,nodefrom);

				portnbfrom=(char*)malloc(_portnbfromNbElement*_max_portnbfromList_len*sizeof(char));
				if(portnbfrom!=NULL)
					status+=NormalizeVector(_portnbfromList, _portnbfromNbElement,_max_portnbfromList_len,portnbfrom);

				portnbto=(char*)malloc(_portnbtoNbElement*_max_portnbtoList_len*sizeof(char));
				if(portnbto!=NULL)
					status+=NormalizeVector(_portnbtoList, _portnbtoNbElement,_max_portnbtoList_len,portnbto);
				lkinfo=(char*)malloc(_lkinfoNbElement*_max_lkinfoList_len*sizeof(char));
				if(lkinfo!=NULL)
					status+=NormalizeVector(_lkinfoList, _lkinfoNbElement,_max_lkinfoList_len,lkinfo);


				portypefrom=(char*)malloc(_portypefromNbElement*_max_portypefromList_len*sizeof(char));
				if(portypefrom!=NULL)
					status+=NormalizeVector(_portypefromList, _portypefromNbElement,_max_portypefromList_len,portypefrom);

				portypeto=(char*)malloc(_portypetoNbElement*_max_portypetoList_len*sizeof(char));
				if(portypeto!=NULL)
					status+=NormalizeVector(_portypetoList, _portypetoNbElement,_max_portypetoList_len,portypeto);

				nodeto=(char*)malloc(_nodetoNbElement*_max_nodetoList_len*sizeof(char));
				if(nodeto!=NULL)
					status+=NormalizeVector(_NodeToList, _nodetoNbElement,_max_nodetoList_len,nodeto);

				linktype=(char*)malloc(_linktypeNbElement2*_max_linktypeList_len2*sizeof(char));
				if(linktype!=NULL)
					status+=NormalizeVector(_LinkTypeList2, _linktypeNbElement2,_max_linktypeList_len2,linktype);


				numrows_inserted=(int*)malloc(sizeof(int)*_nodefromNbElement);
				if(lkinfo==NULL||nodefrom==NULL || nodeto==NULL ||linktype==NULL ||portnbfrom==NULL||portnbto==NULL||portypefrom==NULL||portypeto==NULL||numrows_inserted==NULL)
				{

					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					status+=freeConnectivity();
					if(nodefrom!=NULL) //means not null otehriwse it's -1
						free(nodefrom);
					if(nodeto!=NULL)
						free(nodeto);
					if(linktype!=NULL)
						free(linktype);
					if(portnbto!=NULL)
						free(portnbto);
					if(portnbfrom!=NULL)
						free(portnbfrom);
					if(portypeto!=NULL)
						free(portypeto);
					if(portypefrom!=NULL)
						free(portypefrom);
					if(lkinfo!=NULL)
						free(lkinfo);
					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					return -1;
				}
				else
				{
					for(i=0;i<_nodefromNbElement;i++)
						numrows_inserted[i]=0;

					status =OCIBindByName(hstmt, &bndp2[0], ociError,(text*) ":portnbfrom", -1,(dvoid*)portnbfrom, _max_portnbfromList_len,  SQLT_STR, (dvoid *) &_portnbfrom_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
				}
			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp2[1], ociError,(text*) ":portnbto", -1,(dvoid*)portnbto, _max_portnbtoList_len,  SQLT_STR, (dvoid *) &_portnbto_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp2[2], ociError,(text*) ":biused", -1,(dvoid*)&_BidirectionalList[0], sizeof(bidirectional_link_used),  SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp2[3], ociError,(text*) ":nfrom", -1,(dvoid*)nodefrom, _max_nodefromList_len,  SQLT_STR, (dvoid *) & _nodefrom_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp2[4], ociError,(text*) ":nto", -1,(dvoid*)nodeto, _max_nodetoList_len,  SQLT_STR,  (dvoid *) & _nodeto_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp2[5], ociError,(text*) ":lktype", -1,(dvoid*)linktype, _max_linktypeList_len2,SQLT_STR, (dvoid *) & _linktype_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp2[6], ociError,(text*) ":ptypeto", -1,(dvoid*)portypeto, _max_portypetoList_len,  SQLT_STR, (dvoid *) &_portypeto_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp2[7], ociError,(text*) ":ptypefrom", -1,(dvoid*)portypefrom, _max_portypefromList_len,  SQLT_STR, (dvoid *) &_portypefrom_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp2[8], ociError,(text*) ":lkinfo", -1,(dvoid*)lkinfo, _max_lkinfoList_len,  SQLT_STR, (dvoid *) &_lkinfo_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);




			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp3, ociError,(text*) ":numrows", -1,(dvoid*)&numrows_inserted[0], sizeof(int),SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[0], ociError,  _max_portnbfromList_len, sizeof(int), 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[1], ociError,  _max_portnbtoList_len, sizeof(int), 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[2], ociError, sizeof(int), 0, 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[3], ociError,_max_nodefromList_len, sizeof(int), 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[4], ociError, _max_nodetoList_len, sizeof(int), 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[5], ociError,_max_linktypeList_len2, sizeof(int), 0, 0);

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
				status=OCIBindArrayOfStruct(bndp2[6], ociError,  _max_portypetoList_len, sizeof(int), 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[7], ociError,  _max_portypefromList_len, sizeof(int), 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[8], ociError,  _max_lkinfoList_len, sizeof(int), 0, 0);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status= OCIStmtExecute(ociHdbc, hstmt, ociError, _nodefromNbElement, 0, 0, 0, OCI_DEFAULT );

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
			}
			else
			{
				//check if all rows has been inserted
				//if not we do a rollback
				numrows=-1;
				for(i=0;i<_nodefromNbElement;i++)
				{
					if(numrows_inserted[i]==0)
					{
						numrows=0;
						i=_nodefromNbElement+5;
					}
					else
						numrows=numrows_inserted[i];
				}
				//std::cout<<"before updating tables "<<sqlstmt <<std::endl;
				if(numrows==0)
				{
					status = OCITransCommit(ociHdbc, ociError, 0);
					sprintf(sqlstmt,"BEGIN  :rescode:=%s(%d); :numrows:=%s; END; ",_UpdateDynamicTables,sysID,SQLROWCOUNT); 
					//std::cout<<"value of stmt 3 "<<sqlstmt <<std::endl;
					status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
					if(status!=OCI_SUCCESS)
					{
						if(rescode==0)	
							rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
					}
					else
						status =OCIBindByName(hstmt, &bndp3, ociError,(text*) ":numrows", -1,(dvoid*)&numrows, sizeof(int),SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

					if(status!=OCI_SUCCESS)
					{
						if(rescode==0)	
							rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
					}
					else
						status =OCIBindByName(hstmt, &bndp2[0], ociError,(text*) ":rescode", -1,(dvoid*)res_query1, 101,SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

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
							rescode=ShowErrors (status, ociError, "OCIExecute unsuccessful");
					}
					else
						status = OCITransCommit(ociHdbc, ociError, 0);
					if(status!=OCI_SUCCESS)
					{	
						if(rescode==0)	
							rescode=ShowErrors (status, ociError, "OCITransCommit unsuccessful");
					}	
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status+=freeConnectivity();


					if(nodefrom!=NULL) //means not null otehriwse it's -1
						free(nodefrom);
					if(nodeto!=NULL)
						free(nodeto);
					if(linktype!=NULL)
						free(linktype);
					if(portnbfrom!=NULL)
						free(portnbfrom);
					if(portnbto!=NULL)
						free(portnbto);
					if(portypefrom!=NULL)
						free(portypefrom);
					if(portypeto!=NULL)
						free(portypeto);
					if(lkinfo!=NULL)
						free(lkinfo);
					free(numrows_inserted);
					GetErrorMess(appliName, "COULDNOT_INSERT_ALL_ROWS:CHECK SYSTEM COMPATIBILITY",ErrMess,1);

					return -1;
				}
				else
				{
					status = OCITransCommit(ociHdbc, ociError, 0);
					//update routing and destination tables only if it's part of the DAQ and TFC...
					sprintf(sqlstmt,"BEGIN  :rescode:=%s(%d); :numrows:=%s; END; ",_UpdateDynamicTables,sysID,SQLROWCOUNT); 
					//std:://cout<<"value of stmt 4 "<<sqlstmt <<std::endl;
					status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
					if(status!=OCI_SUCCESS)
					{
						if(rescode==0)	
							rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
					}
					else
						status =OCIBindByName(hstmt, &bndp3, ociError,(text*) ":numrows", -1,(dvoid*)&numrows, sizeof(int),SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

					if(status!=OCI_SUCCESS)
					{
						if(rescode==0)	
							rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
					}
					else
						status =OCIBindByName(hstmt, &bndp2[0], ociError,(text*) ":rescode", -1,(dvoid*)res_query1, 101,SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
					//std:://cout<<"value of stmt 4 OCIBindByName" <<std::endl;

					if(status!=OCI_SUCCESS)
					{
						if(rescode==0)	
							rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
					}
					else
						status= OCIStmtExecute(ociHdbc, hstmt, ociError, 1, 0, 0, 0, OCI_DEFAULT );
					//std:://cout<<"value of stmt 4 OCIStmtExecute" <<std::endl;
					if(status!=OCI_SUCCESS)
					{
						if(rescode==0)	
							rescode=ShowErrors (status, ociError, "OCIExecute unsuccessful");
					}
					else
						status = OCITransCommit(ociHdbc, ociError, 0);
					if(status!=OCI_SUCCESS)
					{	
						if(rescode==0)	
							rescode=ShowErrors (status, ociError, "OCITransCommit unsuccessful");
					}	
				}

			}
			status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
			status+=freeConnectivity();
			//std::cout<<"after freeconnect"<<std::endl;
			if(nodefrom!=NULL) //means not null otehriwse it's -1
				free(nodefrom);
			if(nodeto!=NULL)
				free(nodeto);
			if(linktype!=NULL)
				free(linktype);
			if(lkinfo!=NULL)
				free(lkinfo);
			if(portnbfrom!=NULL)
				free(portnbfrom);
			//std:://cout<<"after portnbfrom"<<std::endl;
			if(portnbto!=NULL)
				free(portnbto);
			if(portypefrom!=NULL)
				free(portypefrom);
			if(portypeto!=NULL)
				free(portypeto);
			if(numrows_inserted!=NULL)
				free(numrows_inserted);
			if(rescode!=0)
				OCIReportError(ociError,appliName, ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0);  
			if(force_insert==1 && rescode==0)
			{
				FIRST_TIME_CONNECTIVITY=1;
				force_insert=0;
			}


		}
		else
		{
			if(res_query!=0)
			{
				status=freeConnectivity();
				status+=res_query;
				GetErrorMess(appliName, "Cache pb",ErrMess,1);
			}
			if(FIRST_TIME_CONNECTIVITY!=1)
			{
				status=freeConnectivity();
				status+=res_query;
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
			}
			if(ociEnv==0)
			{
				status=freeConnectivity();
				status+=res_query;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
			}
			if(ociEnv!=0 && FIRST_TIME_CONNECTIVITY==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}

		}
		return (status+rescode);
	}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif





