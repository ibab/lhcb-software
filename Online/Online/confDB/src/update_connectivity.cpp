/********************************************************************************/
//	Author: L.Abadie
// version:v3.5
// changes: handling error such that can be extended to C++: (create a class SQLException) (strlen ck + update stmt)
// file which performs bulk collect insert for the connectivity table
// N.B : Memory management (allocation+release) should be handled by the user application thus, for all the fct provided by this lib
/********************************************************************************/

//update bidirectional, lkused, lktype,lktk
//after bidirectional,lktk and lktype need to update the path tables

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
	int FIRST_TIME_CONNECTIVITYUPDATE1=0;
	int FIRST_TIME_CONNECTIVITYUPDATE2=0;
	int FIRST_TIME_CONNECTIVITYUPDATE3=0;
	int FIRST_TIME_CONNECTIVITYUPDATE4=0;
	int FIRST_TIME_CONNECTIVITYUPDATE5=0;

	char* _NodeFrom1List;
	int _max_nodefrom1List_len;
	int _nodefrom1List_len;
	int _nodefrom1NbElement;

	int* _nodefrom1_nullvalue;
	int _nodefrom1NbEl;

	char* _ptFrom1List;
	int _max_ptfrom1List_len;
	int _ptfrom1List_len;
	int _ptfrom1NbElement;

	int* _ptfrom1_nullvalue;
	int _ptfrom1NbEl;

	char* _ptFrom5List;
	int _max_ptfrom5List_len;
	int _ptfrom5List_len;
	int _ptfrom5NbElement;

	int* _ptfrom5_nullvalue;
	int _ptfrom5NbEl;


	char* _ptFrom2List;
	int _max_ptfrom2List_len;
	int _ptfrom2List_len;
	int _ptfrom2NbElement;

	int* _ptfrom2_nullvalue;
	int _ptfrom2NbEl;

	char* _ptFrom3List;
	int _max_ptfrom3List_len;
	int _ptfrom3List_len;
	int _ptfrom3NbElement;

	int* _ptfrom3_nullvalue;
	int _ptfrom3NbEl;



	char* _ptTo1List;
	int _max_ptto1List_len;
	int _ptto1List_len;
	int _ptto1NbElement;

	int* _ptto1_nullvalue;
	int _ptto1NbEl;

	char* _ptTo2List;
	int _max_ptto2List_len;
	int _ptto2List_len;
	int _ptto2NbElement;

	int* _ptto2_nullvalue;
	int _ptto2NbEl;

	char* _ptTo3List;
	int _max_ptto3List_len;
	int _ptto3List_len;
	int _ptto3NbElement;

	int* _ptto3_nullvalue;
	int _ptto3NbEl;

	char* _ptTo5List;
	int _max_ptto5List_len;
	int _ptto5List_len;
	int _ptto5NbElement;

	int* _ptto5_nullvalue;
	int _ptto5NbEl;

	char* _ptypeFrom1List;
	int _max_ptypefrom1List_len;
	int _ptypefrom1List_len;
	int _ptypefrom1NbElement;

	int* _ptypefrom1_nullvalue;
	int _ptypefrom1NbEl;

	char* _ptypeFrom5List;
	int _max_ptypefrom5List_len;
	int _ptypefrom5List_len;
	int _ptypefrom5NbElement;

	int* _ptypefrom5_nullvalue;
	int _ptypefrom5NbEl;

	char* _ptypeFrom2List;
	int _max_ptypefrom2List_len;
	int _ptypefrom2List_len;
	int _ptypefrom2NbElement;

	int* _ptypefrom2_nullvalue;
	int _ptypefrom2NbEl;

	char* _ptypeFrom3List;
	int _max_ptypefrom3List_len;
	int _ptypefrom3List_len;
	int _ptypefrom3NbElement;

	int* _ptypefrom3_nullvalue;
	int _ptypefrom3NbEl;

	char* _ptypeTo1List;
	int _max_ptypeto1List_len;
	int _ptypeto1List_len;
	int _ptypeto1NbElement;

	int* _ptypeto1_nullvalue;
	int _ptypeto1NbEl;

	char* _ptypeTo5List;
	int _max_ptypeto5List_len;
	int _ptypeto5List_len;
	int _ptypeto5NbElement;

	int* _ptypeto5_nullvalue;
	int _ptypeto5NbEl;

	char* _ptypeTo2List;
	int _max_ptypeto2List_len;
	int _ptypeto2List_len;
	int _ptypeto2NbElement;

	int* _ptypeto2_nullvalue;
	int _ptypeto2NbEl;

	char* _ptypeTo3List;
	int _max_ptypeto3List_len;
	int _ptypeto3List_len;
	int _ptypeto3NbElement;

	int* _ptypeto3_nullvalue;
	int _ptypeto3NbEl;


	char* _NodeFrom2List;
	int _max_nodefrom2List_len;
	int _nodefrom2List_len;
	int _nodefrom2NbElement;

	int* _nodefrom2_nullvalue;
	int _nodefrom2NbEl;

	char* _NodeFrom5List;
	int _max_nodefrom5List_len;
	int _nodefrom5List_len;
	int _nodefrom5NbElement;

	int* _nodefrom5_nullvalue;
	int _nodefrom5NbEl;

	char* _NodeFrom3List;
	int _max_nodefrom3List_len;
	int _nodefrom3List_len;
	int _nodefrom3NbElement;

	int* _nodefrom3_nullvalue;
	int _nodefrom3NbEl;

	char* _NodeFrom4List;
	int _max_nodefrom4List_len;
	int _nodefrom4List_len;
	int _nodefrom4NbElement;

	int* _nodefrom4_nullvalue;
	int _nodefrom4NbEl;

	char* _NodeTo1List;
	int _max_nodeto1List_len;
	int _nodeto1List_len;
	int _nodeto1NbElement;

	int* _nodeto1_nullvalue;
	int _nodeto1NbEl;

	char* _NodeTo2List;
	int _max_nodeto2List_len;
	int _nodeto2List_len;
	int _nodeto2NbElement;

	int* _nodeto2_nullvalue;
	int _nodeto2NbEl;

	char* _NodeTo5List;
	int _max_nodeto5List_len;
	int _nodeto5List_len;
	int _nodeto5NbElement;

	int* _nodeto5_nullvalue;
	int _nodeto5NbEl;

	char* _NodeTo3List;
	int _max_nodeto3List_len;
	int _nodeto3List_len;
	int _nodeto3NbElement;

	int* _nodeto3_nullvalue;
	int _nodeto3NbEl;

	char* _NodeTo4List;
	int _max_nodeto4List_len;
	int _nodeto4List_len;
	int _nodeto4NbElement;

	int* _nodeto4_nullvalue;
	int _nodeto4NbEl;






	int* _LinkUsedListUpdate;
	int _LinkUsedNbElementUpdate;

	char* _LinkTypeList2Update;
	int _max_linktypeList_len2Update;
	int _linktypeList_len2Update;
	int _linktypeNbElement2Update;

	int* _linktype_nullvalueUpdate;
	int _linktypeNbElUpdate;


	char* _LinkInfoList5Update;
	int _max_linkinfoList_len5Update;
	int _linkinfoList_len5Update;
	int _linkinfoNbElement5Update;

	int* _linkinfo_nullvalueUpdate;
	int _linkinfoNbElUpdate;

	int* _BidirectionalListUpdate;
	int _bidirectionalNbElementUpdate;

	/******************************************************/
	int freeConnectivityUpdate1()
	{
		int status=0;
		_BidirectionalListUpdate=(int*)realloc(_BidirectionalListUpdate,0*sizeof(int));
		_BidirectionalListUpdate=NULL;

		_NodeTo1List=(char*)realloc(_NodeTo1List,0*sizeof(char));
		_NodeTo1List=NULL;
		_NodeFrom1List=(char*)realloc(_NodeFrom1List,0*sizeof(char));
		_NodeFrom1List=NULL;

		_ptTo1List=(char*)realloc(_ptTo1List,0*sizeof(char));
		_ptTo1List=NULL;
		_ptFrom1List=(char*)realloc(_ptFrom1List,0*sizeof(char));
		_ptFrom1List=NULL;

		_ptypeTo1List=(char*)realloc(_ptypeTo1List,0*sizeof(char));
		_ptypeTo1List=NULL;
		_ptypeFrom1List=(char*)realloc(_ptypeFrom1List,0*sizeof(char));
		_ptypeFrom1List=NULL;



		_nodeto1_nullvalue=(int*)realloc(_nodeto1_nullvalue,0*sizeof(int));
		_nodeto1_nullvalue=NULL;
		_nodefrom1_nullvalue=(int*)realloc(_nodefrom1_nullvalue,0*sizeof(int));
		_nodefrom1_nullvalue=NULL;

		_ptto1_nullvalue=(int*)realloc(_ptto1_nullvalue,0*sizeof(int));
		_ptto1_nullvalue=NULL;
		_ptfrom1_nullvalue=(int*)realloc(_ptfrom1_nullvalue,0*sizeof(int));
		_ptfrom1_nullvalue=NULL;

		_ptypeto1_nullvalue=(int*)realloc(_ptypeto1_nullvalue,0*sizeof(int));
		_ptypeto1_nullvalue=NULL;
		_ptypefrom1_nullvalue=(int*)realloc(_ptypefrom1_nullvalue,0*sizeof(int));
		_ptypefrom1_nullvalue=NULL;


		FIRST_TIME_CONNECTIVITYUPDATE1=0;


		return status;
	}
	/******************************************************/
	int freeConnectivityUpdate2()
	{
		int status=0;
		_LinkTypeList2Update=(char*)realloc(_LinkTypeList2Update,0*sizeof(char));
		_LinkTypeList2Update=NULL;

		_NodeTo2List=(char*)realloc(_NodeTo2List,0*sizeof(char));
		_NodeTo2List=NULL;
		_NodeFrom2List=(char*)realloc(_NodeFrom2List,0*sizeof(char));
		_NodeFrom2List=NULL;
		_ptTo2List=(char*)realloc(_ptTo2List,0*sizeof(char));
		_ptTo2List=NULL;
		_ptFrom2List=(char*)realloc(_ptFrom2List,0*sizeof(char));
		_ptFrom2List=NULL;

		_ptypeTo2List=(char*)realloc(_ptypeTo2List,0*sizeof(char));
		_ptypeTo2List=NULL;
		_ptypeFrom2List=(char*)realloc(_ptypeFrom2List,0*sizeof(char));
		_ptypeFrom2List=NULL;

		_nodeto2_nullvalue=(int*)realloc(_nodeto2_nullvalue,0*sizeof(int));
		_nodeto2_nullvalue=NULL;
		_nodefrom2_nullvalue=(int*)realloc(_nodefrom2_nullvalue,0*sizeof(int));
		_nodefrom2_nullvalue=NULL;
		_linktype_nullvalueUpdate=(int*)realloc(_linktype_nullvalueUpdate,0*sizeof(int));
		_linktype_nullvalueUpdate=NULL;

		_ptto2_nullvalue=(int*)realloc(_ptto2_nullvalue,0*sizeof(int));
		_ptto2_nullvalue=NULL;
		_ptfrom2_nullvalue=(int*)realloc(_ptfrom2_nullvalue,0*sizeof(int));
		_ptfrom2_nullvalue=NULL;

		_ptypeto2_nullvalue=(int*)realloc(_ptypeto2_nullvalue,0*sizeof(int));
		_ptypeto2_nullvalue=NULL;
		_ptypefrom2_nullvalue=(int*)realloc(_ptypefrom2_nullvalue,0*sizeof(int));
		_ptypefrom2_nullvalue=NULL;
		FIRST_TIME_CONNECTIVITYUPDATE2=0;


		return status;
	}
	/******************************************************/
	int freeConnectivityUpdate5()
	{
		int status=0;
		_LinkInfoList5Update=(char*)realloc(_LinkInfoList5Update,0*sizeof(char));
		_LinkInfoList5Update=NULL;

		_NodeTo5List=(char*)realloc(_NodeTo5List,0*sizeof(char));
		_NodeTo5List=NULL;
		_NodeFrom5List=(char*)realloc(_NodeFrom5List,0*sizeof(char));
		_NodeFrom5List=NULL;
		_ptTo5List=(char*)realloc(_ptTo5List,0*sizeof(char));
		_ptTo5List=NULL;
		_ptFrom5List=(char*)realloc(_ptFrom5List,0*sizeof(char));
		_ptFrom5List=NULL;

		_ptypeTo5List=(char*)realloc(_ptypeTo5List,0*sizeof(char));
		_ptypeTo5List=NULL;
		_ptypeFrom5List=(char*)realloc(_ptypeFrom5List,0*sizeof(char));
		_ptypeFrom5List=NULL;

		_nodeto5_nullvalue=(int*)realloc(_nodeto5_nullvalue,0*sizeof(int));
		_nodeto5_nullvalue=NULL;
		_nodefrom5_nullvalue=(int*)realloc(_nodefrom5_nullvalue,0*sizeof(int));
		_nodefrom5_nullvalue=NULL;
		_linkinfo_nullvalueUpdate=(int*)realloc(_linkinfo_nullvalueUpdate,0*sizeof(int));
		_linkinfo_nullvalueUpdate=NULL;

		_ptto5_nullvalue=(int*)realloc(_ptto5_nullvalue,0*sizeof(int));
		_ptto5_nullvalue=NULL;
		_ptfrom5_nullvalue=(int*)realloc(_ptfrom5_nullvalue,0*sizeof(int));
		_ptfrom5_nullvalue=NULL;

		_ptypeto5_nullvalue=(int*)realloc(_ptypeto5_nullvalue,0*sizeof(int));
		_ptypeto5_nullvalue=NULL;
		_ptypefrom5_nullvalue=(int*)realloc(_ptypefrom5_nullvalue,0*sizeof(int));
		_ptypefrom5_nullvalue=NULL;
		FIRST_TIME_CONNECTIVITYUPDATE5=0;


		return status;
	}
	/******************************************************/
	int freeConnectivityUpdate3()
	{
		int status=0;

		_NodeTo3List=(char*)realloc(_NodeTo3List,0*sizeof(char));
		_NodeTo3List=NULL;
		_NodeFrom3List=(char*)realloc(_NodeFrom3List,0*sizeof(char));
		_NodeFrom3List=NULL;
		_LinkUsedListUpdate=(int*)realloc(_LinkUsedListUpdate,0*sizeof(int));
		_LinkUsedListUpdate=NULL;
		_ptTo3List=(char*)realloc(_ptTo3List,0*sizeof(char));
		_ptTo3List=NULL;
		_ptFrom3List=(char*)realloc(_ptFrom3List,0*sizeof(char));
		_ptFrom3List=NULL;

		_ptypeTo3List=(char*)realloc(_ptypeTo3List,0*sizeof(char));
		_ptypeTo3List=NULL;
		_ptypeFrom3List=(char*)realloc(_ptypeFrom3List,0*sizeof(char));
		_ptypeFrom3List=NULL;

		_nodeto3_nullvalue=(int*)realloc(_nodeto3_nullvalue,0*sizeof(int));
		_nodeto3_nullvalue=NULL;
		_nodefrom3_nullvalue=(int*)realloc(_nodefrom3_nullvalue,0*sizeof(int));
		_nodefrom3_nullvalue=NULL;
		_ptto3_nullvalue=(int*)realloc(_ptto3_nullvalue,0*sizeof(int));
		_ptto3_nullvalue=NULL;
		_ptfrom3_nullvalue=(int*)realloc(_ptfrom3_nullvalue,0*sizeof(int));
		_ptfrom3_nullvalue=NULL;

		_ptypeto3_nullvalue=(int*)realloc(_ptypeto3_nullvalue,0*sizeof(int));
		_ptypeto3_nullvalue=NULL;
		_ptypefrom3_nullvalue=(int*)realloc(_ptypefrom3_nullvalue,0*sizeof(int));
		_ptypefrom3_nullvalue=NULL;
		FIRST_TIME_CONNECTIVITYUPDATE3=0;


		return status;
	}

	/**
	* Update bidirectional link field for a given link  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* either you give node_from,pfrom or node_to,pto.
	* @param node_from : name of the device starting the link: put "none" if you give node_to
	* @param node_to : name of the device ending the link: put "none" if you give node_from
	* @param port_nbrfrom : port nb of the device from
	* @param port_nbrto : port nb of the device to
	* @param port_typefrom : port nb of the device from: put "none" if it doesn't exist or if you provide about the node_to 
	* @param port_typeto : port nb of the device to:put "none" if it doesn't exist or if you provide about the node_from
	* @param bidirectional_link_used :  1 if it's a bidirectional link, 0 otherwise
	* @param first_time1 : 1 if it's your MultipleLink first insert (save it into the database), 0 otherwise
	* @param last_rows1 : 1 if it's your MultipleLink last insert (save it into the database), 0 otherwise. Maximum in one go: 10000 rows. (we force the insert)
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/ 
	EXTERN_CONFDB
		int UpdateMultipleBidirectionalLinks(char* node_from,char* node_to,char* port_nbrfrom,char* port_typefrom,char* port_nbrto,char* port_typeto,int bidirectional_link_used,int first_time1, int last_rows1,char* ErrMess)
	{
		int numrows=0;
		int num_errs1=0;
		char sqlstmt[1000];

		int i=0;
		char appliName[100]="UpdateMultipleBidirectionalLinks";
		OCIStmt *hstmt;
		OCIBind  *bndp2[7];
		OCIBind  *bndp3 = (OCIBind *) 0; 
		char res_query1[100]="blabla";
		int res_query=0;
		sword status=0;
		char* nodefrom=NULL;
		char* nodeto=NULL;
		char* pfrom=NULL;
		char* pto=NULL;
		char* ptypefrom=NULL;
		char* ptypeto=NULL;
		int rescode=0;
		int last_rows=last_rows1;
		int* numrows_inserted=NULL;
		char port_typefrom1[100];
		if(port_typefrom!=NULL)
		{
			if(strlen(port_typefrom)<2)
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
			if(strlen(port_typeto)<2)
				strcpy(port_typeto1,"none");
			else
				strcpy(port_typeto1,port_typeto);
		}
		else
		{
			strcpy(port_typeto1,"none");
		}

		int nodefrom_null=null_charptr(node_from);
		int nodeto_null=null_charptr(node_to);
		int pfrom_null=null_charptr(port_nbrfrom);
		int pto_null=null_charptr(port_nbrto);
		int ptypefrom_null=null_charptr(port_typefrom);
		int ptypeto_null=null_charptr(port_typeto);

		int force_insert=0;
		int first_time=first_time1;
		if(FIRST_TIME_CONNECTIVITYUPDATE1==1 && _NodeFrom1List==NULL) //means new loop in the mass insertion (by 10000)
		{
			first_time=1; //need to put to 1, as if new cache
		}
		if(first_time==1)
		{
			status+=freeConnectivityUpdate1();
			FIRST_TIME_CONNECTIVITYUPDATE1=1;


		}
		res_query=AppendString(node_from,_NodeFrom1List, _nodefrom1List_len,_nodefrom1NbElement,_max_nodefrom1List_len,first_time);
		res_query+=AppendString(node_to,_NodeTo1List,_nodeto1List_len,_nodeto1NbElement,_max_nodeto1List_len,first_time);
		res_query+=AppendInt(bidirectional_link_used,_BidirectionalListUpdate,_bidirectionalNbElementUpdate,first_time);
		res_query=AppendString(port_nbrfrom,_ptFrom1List, _ptfrom1List_len,_ptfrom1NbElement,_max_ptfrom1List_len,first_time);
		res_query+=AppendString(port_nbrto,_ptTo1List,_ptto1List_len,_ptto1NbElement,_max_ptto1List_len,first_time);
		res_query=AppendString(port_typefrom1,_ptypeFrom1List, _ptypefrom1List_len,_ptypefrom1NbElement,_max_ptypefrom1List_len,first_time);
		res_query+=AppendString(port_typeto1,_ptypeTo1List,_ptypeto1List_len,_ptypeto1NbElement,_max_ptypeto1List_len,first_time);


		res_query+=AppendInt(nodeto_null,_nodeto1_nullvalue,_nodeto1NbEl,first_time);
		res_query+=AppendInt(nodefrom_null,_nodefrom1_nullvalue,_nodefrom1NbEl,first_time);
		res_query+=AppendInt(pto_null,_ptto1_nullvalue,_ptto1NbEl,first_time);
		res_query+=AppendInt(pfrom_null,_ptfrom1_nullvalue,_ptfrom1NbEl,first_time);
		res_query+=AppendInt(ptypeto_null,_ptypeto1_nullvalue,_ptypeto1NbEl,first_time);
		res_query+=AppendInt(ptypefrom_null,_ptypefrom1_nullvalue,_ptypefrom1NbEl,first_time);


		if(_nodefrom1NbElement==MAXROWS && last_rows!=1)
		{
			last_rows=1;
			force_insert=1;
		}
		status=res_query;
		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME_CONNECTIVITYUPDATE1==1)
		{
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
				status+=freeConnectivityUpdate1();
				if(nodefrom!=NULL) //means not null otehriwse it's -1
					free(nodefrom);
				if(nodeto!=NULL)
					free(nodeto);
				if(pfrom!=NULL) //means not null otehriwse it's -1
					free(pfrom);
				if(pto!=NULL)
					free(pto);
				if(ptypefrom!=NULL) //means not null otehriwse it's -1
					free(ptypefrom);
				if(ptypeto!=NULL)
					free(ptypeto);
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

				sprintf(sqlstmt,"BEGIN if lower(nvl(:nto,'none'))='none' then update %s t set t.bidirectional_link_used=:biused,t.last_update=sysdate,t.user_update='%s' where  t.portidfrom=(select e.portid from %s e,%s l where  l.deviceid=e.deviceid and l.devicename=:nfrom and e.port_nbr=:pfrom and nvl(e.port_type,'none')=:ptypefrom and e.port_way=2) ; else if lower(nvl(:nfrom,'none'))='none' then update %s t set t.bidirectional_link_used=:biused,t.last_update=sysdate,t.user_update='%s' where t.portidto=(select e.portid from %s e, %s l where l.devicename=:nto and l.deviceid=e.deviceid and e.port_way=1 and nvl(e.port_type,'none')=:ptypeto and e.port_nbr=:pto) ;  end if; end if; :numrows:=%s; END; ",MACRO_CONNECTIVITY_TABLE,login,PORT_TABLE,LOGICAL_DEVICE_TABLE,MACRO_CONNECTIVITY_TABLE,login,PORT_TABLE,LOGICAL_DEVICE_TABLE,SQLROWCOUNT);
				status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
				//std:://cout<<"value of stmt "<<sqlstmt <<std::endl;
			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
			{
				nodefrom=(char*)malloc(_nodefrom1NbElement*_max_nodefrom1List_len*sizeof(char));
				if(nodefrom!=NULL)
					status+=NormalizeVector(_NodeFrom1List, _nodefrom1NbElement,_max_nodefrom1List_len,nodefrom);

				nodeto=(char*)malloc(_nodeto1NbElement*_max_nodeto1List_len*sizeof(char));
				if(nodeto!=NULL)
					status+=NormalizeVector(_NodeTo1List, _nodeto1NbElement,_max_nodeto1List_len,nodeto);


				pfrom=(char*)malloc(_ptfrom1NbElement*_max_ptfrom1List_len*sizeof(char));
				if(pfrom!=NULL)
					status+=NormalizeVector(_ptFrom1List, _ptfrom1NbElement,_max_ptfrom1List_len,pfrom);

				ptypefrom=(char*)malloc(_ptypefrom1NbElement*_max_ptypefrom1List_len*sizeof(char));
				if(ptypefrom!=NULL)
					status+=NormalizeVector(_ptypeFrom1List, _ptypefrom1NbElement,_max_ptypefrom1List_len,ptypefrom);

				pto=(char*)malloc(_ptto1NbElement*_max_ptto1List_len*sizeof(char));
				if(pto!=NULL)
					status+=NormalizeVector(_ptTo1List, _ptto1NbElement,_max_ptto1List_len,pto);

				ptypeto=(char*)malloc(_ptypeto1NbElement*_max_ptypeto1List_len*sizeof(char));
				if(ptypeto!=NULL)
					status+=NormalizeVector(_ptypeTo1List, _ptypeto1NbElement,_max_ptypeto1List_len,ptypeto);


				numrows_inserted=(int*)malloc(sizeof(int)*_nodefrom1NbElement);
				if(nodefrom==NULL || nodeto==NULL ||pto==NULL||pfrom==NULL||ptypefrom==NULL||ptypeto==NULL||numrows_inserted==NULL)
				{

					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					status+=freeConnectivityUpdate1();
					if(pfrom!=NULL) //means not null otehriwse it's -1
						free(pfrom);
					if(pto!=NULL)
						free(pto);
					if(ptypefrom!=NULL) //means not null otehriwse it's -1
						free(ptypefrom);
					if(ptypeto!=NULL)
						free(ptypeto);
					if(nodefrom!=NULL) //means not null otehriwse it's -1
						free(nodefrom);
					if(nodeto!=NULL)
						free(nodeto);
					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					return -1;
				}
				else
				{
					for(i=0;i<_nodefrom1NbElement;i++)
						numrows_inserted[i]=0;

					status =OCIBindByName(hstmt, &bndp2[0], ociError,(text*) ":pfrom", -1,(dvoid*)pfrom, _max_ptfrom1List_len,  SQLT_STR, (dvoid *) &_ptfrom1_nullvalue[0] ,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
				}
			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp2[1], ociError,(text*) ":pto", -1,(dvoid*)pto, _max_ptto1List_len,  SQLT_STR, (dvoid *) &_ptto1_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp2[2], ociError,(text*) ":biused", -1,(dvoid*)&_BidirectionalListUpdate[0], sizeof(bidirectional_link_used),  SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp2[3], ociError,(text*) ":nfrom", -1,(dvoid*)nodefrom, _max_nodefrom1List_len,  SQLT_STR, (dvoid *) & _nodefrom1_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp2[4], ociError,(text*) ":nto", -1,(dvoid*)nodeto, _max_nodeto1List_len,  SQLT_STR,  (dvoid *) & _nodeto1_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp2[5], ociError,(text*) ":ptypefrom", -1,(dvoid*)ptypefrom, _max_ptypefrom1List_len,  SQLT_STR, (dvoid *) & _ptypefrom1_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp2[6], ociError,(text*) ":ptypeto", -1,(dvoid*)ptypeto, _max_ptypeto1List_len,  SQLT_STR,  (dvoid *) & _ptypeto1_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

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
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[0], ociError,  _max_ptfrom1List_len,sizeof(int),  0, 0);



			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[1], ociError,  _max_ptto1List_len,sizeof(int), 0, 0);

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
				status=OCIBindArrayOfStruct(bndp2[3], ociError,_max_nodefrom1List_len, sizeof(int), 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[4], ociError, _max_nodeto1List_len, sizeof(int), 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[5], ociError, _max_ptypefrom1List_len, sizeof(int), 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[6], ociError, _max_ptypeto1List_len, sizeof(int), 0, 0);


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
				status= OCIStmtExecute(ociHdbc, hstmt, ociError, _nodefrom1NbElement, 0, 0, 0, OCI_DEFAULT );

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
			}
			else
			{
				//check if all rows has been inserted
				//if not we commit all the same because if the nb of rows inserted >10000, can't manage, we also update tables...
				numrows=-1;
				for(i=0;i<_nodefrom1NbElement;i++)
				{
					if(numrows_inserted[i]==0)
					{
						numrows=0;
						i=_nodefrom1NbElement+5;
					}
					else
						numrows=numrows_inserted[i];
				}
				if(numrows==0)
				{
					status = OCITransCommit(ociHdbc, ociError, 0);
					int sysID=GetSubsystemID("DAQ,TFC");
					sprintf(sqlstmt,"BEGIN :rescode:=%s(%d); :numrows:=%s; END; ",_UpdateDynamicTables,sysID,SQLROWCOUNT);
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
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status+=freeConnectivityUpdate1();


					if(nodefrom!=NULL) //means not null otehriwse it's -1
						free(nodefrom);
					if(nodeto!=NULL)
						free(nodeto);
					if(pfrom!=NULL) //means not null otehriwse it's -1
						free(pfrom);
					if(pto!=NULL)
						free(pto);
					if(ptypefrom!=NULL) //means not null otehriwse it's -1
						free(ptypefrom);
					if(ptypeto!=NULL)
						free(ptypeto);
					free(numrows_inserted);
					GetErrorMess(appliName, "COULDNOT_INSERT_ALL_ROWS",ErrMess,1);

					return -1;
				}
				else
				{
					status = OCITransCommit(ociHdbc, ociError, 0);
					//need to update thse line
					int sysID=GetSubsystemID("DAQ,TFC");
					sprintf(sqlstmt,"BEGIN :rescode:=%s(%d); :numrows:=%s; END; ",_UpdateDynamicTables,sysID,SQLROWCOUNT);
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

				}


			}
			status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
			status+=freeConnectivityUpdate1();
			if(nodefrom!=NULL) //means not null otehriwse it's -1
				free(nodefrom);
			if(nodeto!=NULL)
				free(nodeto);
			if(pfrom!=NULL) //means not null otehriwse it's -1
				free(pfrom);
			if(pto!=NULL)
				free(pto);
			if(ptypefrom!=NULL) //means not null otehriwse it's -1
				free(ptypefrom);
			if(ptypeto!=NULL)
				free(ptypeto);
			if(numrows_inserted!=NULL)
				free(numrows_inserted);
			if(rescode!=0)
				OCIReportError(ociError,appliName, ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0);  
			if(force_insert==1 && rescode==0)
			{
				FIRST_TIME_CONNECTIVITYUPDATE1=1;
				force_insert=0;
			}


		}
		else
		{
			if(res_query!=0)
			{
				status=freeConnectivityUpdate1();
				status+=res_query;
				GetErrorMess(appliName, "Cache pb",ErrMess,1);
			}
			if(FIRST_TIME_CONNECTIVITYUPDATE1!=1)
			{
				status=freeConnectivityUpdate1();
				status+=res_query;
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
			}
			if(ociEnv==0)
			{
				status=freeConnectivityUpdate1();
				status+=res_query;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
			}
			if(ociEnv!=0 && FIRST_TIME_CONNECTIVITYUPDATE1==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}

		}
		return (status+rescode);
	}
	/**
	* Update the lkused field of a Link  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* you give either node_from,portnbrfrom or node_to,pto
	* @param node_from : name of the device starting the link. Put "none" if you give node_to
	* @param node_to : name of the device ending the link. Put "none" if you give node_to
	* @param port_nbrfrom : port nb of the device from
	* @param port_nbrto : port nb of the device to
	* @param port_typefrom : port type of the device from
	* @param port_typeto : port type of the device to
	* @param lkused : 1 if the link is used in the configuration, 0 otherwise
	* @param first_time1 : 1 if it's your MultipleLink first insert (save it into the database), 0 otherwise
	* @param last_rows1 : 1 if it's your MultipleLink last insert (save it into the database), 0 otherwise. Maximum in one go: 10000 rows. (we force the insert)
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/ 
	EXTERN_CONFDB
		int UpdateMultipleLkUsedLinks(char* node_from,char* node_to,char* port_nbrfrom,char* port_nbrto,char* port_typefrom,char* port_typeto,int lkused,int first_time1, int last_rows1,char* ErrMess)
	{
		int numrows=0;
		int num_errs1=0;
		char sqlstmt[1500];
		int i=0;

		char appliName[100]="UpdateMultipleLkUsedLinks";
		OCIStmt *hstmt;
		OCIBind  *bndp2[8];
		OCIBind  *bndp3 = (OCIBind *) 0; 
		int res_query=0;
		sword status=0;
		char* nodefrom=NULL;
		char* nodeto=NULL;
		char* pfrom=NULL;
		char* pto=NULL;
		char* ptypefrom=NULL;
		char* ptypeto=NULL;
		int rescode=0;
		int last_rows=last_rows1;
		int* numrows_inserted=NULL;

		char port_typefrom1[100];
		if(port_typefrom!=NULL)
		{
			if(strlen(port_typefrom)<2)
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
			if(strlen(port_typeto)<2)
				strcpy(port_typeto1,"none");
			else
				strcpy(port_typeto1,port_typeto);
		}
		else
		{
			strcpy(port_typeto1,"none");
		}
		int nodefrom_null=null_charptr(node_from);
		int nodeto_null=null_charptr(node_to);
		int pfrom_null=null_charptr(port_nbrfrom);
		int pto_null=null_charptr(port_nbrto);
		int ptypefrom_null=null_charptr(port_typefrom1);
		int ptypeto_null=null_charptr(port_typeto1);
		int force_insert=0;
		int first_time=first_time1;
		if(FIRST_TIME_CONNECTIVITYUPDATE3==1 && _NodeFrom3List==NULL) //means new loop in the mass insertion (by 10000)
		{
			first_time=1; //need to put to 1, as if new cache
		}
		if(first_time==1)
		{

			status+=freeConnectivityUpdate3();
			FIRST_TIME_CONNECTIVITYUPDATE3=1;
		}
		res_query=AppendString(node_from,_NodeFrom3List, _nodefrom3List_len,_nodefrom3NbElement,_max_nodefrom3List_len,first_time);
		res_query+=AppendString(node_to,_NodeTo3List,_nodeto3List_len,_nodeto3NbElement,_max_nodeto3List_len,first_time);
		res_query+=AppendInt(lkused,_LinkUsedListUpdate,_LinkUsedNbElementUpdate,first_time);
		res_query+=AppendInt(nodeto_null,_nodeto3_nullvalue,_nodeto3NbEl,first_time);
		res_query+=AppendInt(nodefrom_null,_nodefrom3_nullvalue,_nodefrom3NbEl,first_time);

		res_query=AppendString(port_nbrfrom,_ptFrom3List, _ptfrom3List_len,_ptfrom3NbElement,_max_ptfrom3List_len,first_time);
		res_query+=AppendString(port_nbrto,_ptTo3List,_ptto3List_len,_ptto3NbElement,_max_ptto3List_len,first_time);
		res_query=AppendString(port_typefrom1,_ptypeFrom3List, _ptypefrom3List_len,_ptypefrom3NbElement,_max_ptypefrom3List_len,first_time);
		res_query+=AppendString(port_typeto1,_ptypeTo3List,_ptypeto3List_len,_ptypeto3NbElement,_max_ptypeto3List_len,first_time);


		res_query+=AppendInt(pto_null,_ptto3_nullvalue,_ptto3NbEl,first_time);
		res_query+=AppendInt(pfrom_null,_ptfrom3_nullvalue,_ptfrom3NbEl,first_time);
		res_query+=AppendInt(ptypeto_null,_ptypeto3_nullvalue,_ptypeto3NbEl,first_time);
		res_query+=AppendInt(ptypefrom_null,_ptypefrom3_nullvalue,_ptypefrom3NbEl,first_time);

		if(_nodefrom3NbElement==MAXROWS && last_rows!=1)
		{
			last_rows=1;
			force_insert=1;
		}
		status=res_query;
		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME_CONNECTIVITYUPDATE3==1)
		{
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
				status+=freeConnectivityUpdate3();
				if(nodefrom!=NULL) //means not null otehriwse it's -1
					free(nodefrom);
				if(nodeto!=NULL)
					free(nodeto);
				if(pfrom!=NULL) //means not null otehriwse it's -1
					free(pfrom);
				if(pto!=NULL)
					free(pto);
				if(ptypefrom!=NULL) //means not null otehriwse it's -1
					free(ptypefrom);
				if(ptypeto!=NULL)
					free(ptypeto);
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
				sprintf(sqlstmt,"BEGIN if lower(nvl(:nto,'none'))='none' then update %s t set t.lkused=:lkused,t.last_update=sysdate,t.user_update='%s' where t.portidfrom=(select e.portid from %s e,%s l where l.deviceid=e.deviceid and l.devicename=:nfrom and e.port_nbr=:pfrom and nvl(e.port_type,'none')=:ptypefrom and e.port_way=2) ; else if lower(nvl(:nfrom,'none'))='none' then update %s t set t.lkused=:lkused,t.last_update=sysdate,t.user_update='%s' where t.portidto=(select e.portid from %s e, %s l where l.devicename=:nto and l.deviceid=e.deviceid and e.port_way=1 and nvl(e.port_type,'none')=:ptypeto and e.port_nbr=:pto) ;  end if; end if; :numrows:=%s; END; ",MACRO_CONNECTIVITY_TABLE,login,PORT_TABLE,LOGICAL_DEVICE_TABLE,MACRO_CONNECTIVITY_TABLE,login,PORT_TABLE,LOGICAL_DEVICE_TABLE,SQLROWCOUNT);
				status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
				//std:://cout<<"value of stmt "<<sqlstmt << "and len= "<<strlen(sqlstmt)<<std::endl;
			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
			{
				nodefrom=(char*)malloc(_nodefrom3NbElement*_max_nodefrom3List_len*sizeof(char));
				if(nodefrom!=NULL)
					status+=NormalizeVector(_NodeFrom3List, _nodefrom3NbElement,_max_nodefrom3List_len,nodefrom);
				nodeto=(char*)malloc(_nodeto3NbElement*_max_nodeto3List_len*sizeof(char));
				if(nodeto!=NULL)
					status+=NormalizeVector(_NodeTo3List, _nodeto3NbElement,_max_nodeto3List_len,nodeto);


				pfrom=(char*)malloc(_ptfrom3NbElement*_max_ptfrom3List_len*sizeof(char));
				if(pfrom!=NULL)
					status+=NormalizeVector(_ptFrom3List, _ptfrom3NbElement,_max_ptfrom3List_len,pfrom);

				ptypefrom=(char*)malloc(_ptypefrom3NbElement*_max_ptypefrom3List_len*sizeof(char));
				if(ptypefrom!=NULL)
					status+=NormalizeVector(_ptypeFrom3List, _ptypefrom3NbElement,_max_ptypefrom3List_len,ptypefrom);

				pto=(char*)malloc(_ptto3NbElement*_max_ptto3List_len*sizeof(char));
				if(pto!=NULL)
					status+=NormalizeVector(_ptTo3List, _ptto3NbElement,_max_ptto3List_len,pto);

				ptypeto=(char*)malloc(_ptypeto3NbElement*_max_ptypeto3List_len*sizeof(char));
				if(ptypeto!=NULL)
					status+=NormalizeVector(_ptypeTo3List, _ptypeto3NbElement,_max_ptypeto3List_len,ptypeto);

				numrows_inserted=(int*)malloc(sizeof(int)*_nodefrom3NbElement);


				if(nodefrom==NULL || nodeto==NULL||pto==NULL||pfrom==NULL||ptypeto==NULL||ptypefrom==NULL ||numrows_inserted==NULL)
				{

					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					status+=freeConnectivityUpdate3();
					if(nodefrom!=NULL) //means not null otehriwse it's -1
						free(nodefrom);
					if(nodeto!=NULL)
						free(nodeto);

					if(pfrom!=NULL) //means not null otehriwse it's -1
						free(pfrom);
					if(pto!=NULL)
						free(pto);
					if(ptypefrom!=NULL) //means not null otehriwse it's -1
						free(ptypefrom);
					if(ptypeto!=NULL)
						free(ptypeto);
					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					return -1;
				}
				else
				{
					for(i=0;i<_nodefrom3NbElement;i++)
					{
						numrows_inserted[i]=0;

					}
					//rescode1[_nodefrom3NbElement-1]=1;
					status =OCIBindByName(hstmt, &bndp2[0], ociError,(text*) ":pfrom", -1,(dvoid*)pfrom,  _max_ptfrom3List_len,  SQLT_STR, (dvoid *) &_ptfrom3_nullvalue[0] ,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
				}
			}
			//std:://cout<<"before binding "<<std::endl;
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp2[1], ociError,(text*) ":pto", -1,(dvoid*)pto, _max_ptto3List_len,  SQLT_STR, (dvoid *) &_ptto3_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp2[2], ociError,(text*) ":lkused", -1,(dvoid*)&_LinkUsedListUpdate[0], sizeof(lkused),  SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp2[3], ociError,(text*) ":nfrom", -1,(dvoid*)nodefrom, _max_nodefrom3List_len,  SQLT_STR, (dvoid *) & _nodefrom3_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp2[4], ociError,(text*) ":nto", -1,(dvoid*)nodeto, _max_nodeto3List_len,  SQLT_STR,  (dvoid *) & _nodeto3_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp2[5], ociError,(text*) ":ptypeto", -1,(dvoid*)ptypeto, _max_ptypeto3List_len,  SQLT_STR,  (dvoid *) & _ptypeto3_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp2[6], ociError,(text*) ":ptypefrom", -1,(dvoid*)ptypefrom, _max_ptypefrom3List_len,  SQLT_STR,  (dvoid *) & _ptypefrom3_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp3, ociError,(text*) ":numrows", -1,(dvoid*)&numrows_inserted[0], sizeof(int),SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);




			//std:://cout<<"after binding "<<std::endl;

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}



			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[0], ociError,   _max_ptfrom3List_len, sizeof(int),  0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[1], ociError,  _max_ptto3List_len,  sizeof(int),  0, 0);

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
				status=OCIBindArrayOfStruct(bndp2[3], ociError,_max_nodefrom3List_len,sizeof(int), 0, 0);



			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[4], ociError, _max_nodeto3List_len, sizeof(int), 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[5], ociError, _max_ptypeto3List_len, sizeof(int), 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[6], ociError, _max_ptypefrom3List_len, sizeof(int), 0, 0);


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
				status= OCIStmtExecute(ociHdbc, hstmt, ociError, _nodefrom3NbElement, 0, 0, 0, OCI_DEFAULT );
			//std:://cout<<"after execute "<<std::endl;
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
				for(i=0;i<_nodefrom3NbElement;i++)
				{
					if(numrows_inserted[i]==0)
					{
						numrows=0;
						i=_nodefrom3NbElement+5;
					}
					else
						numrows=numrows_inserted[i];
				}
				//std:://cout<<"numrows= "<<numrows<<std::endl;
				if(numrows==0)
				{
					int res_query1;
					status = OCITransCommit(ociHdbc, ociError, 0);
					int sysID=GetSubsystemID("DAQ,TFC");
					sprintf(sqlstmt,"BEGIN :rescode:=%s(%d); :numrows:=%s; END; ",_UpdatePathUsed,sysID,SQLROWCOUNT);
					status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
					//std:://cout<<"value of sqlstmt 2 "<<sqlstmt<<std::endl;			
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
						status =OCIBindByName(hstmt, &bndp2[0], ociError,(text*) ":rescode", -1,(dvoid*)&res_query1, sizeof(int),SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

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
					status+=freeConnectivityUpdate3();


					if(nodefrom!=NULL) //means not null otehriwse it's -1
						free(nodefrom);
					if(nodeto!=NULL)
						free(nodeto);
					if(pfrom!=NULL) //means not null otehriwse it's -1
						free(pfrom);
					if(pto!=NULL)
						free(pto);
					if(ptypefrom!=NULL) //means not null otehriwse it's -1
						free(ptypefrom);
					if(ptypeto!=NULL)
						free(ptypeto);


					free(numrows_inserted);
					if(rescode==0)
						GetErrorMess(appliName, "COULDNOT_INSERT_ALL_ROWS",ErrMess,1);
					else
						GetErrorMess(appliName, "COULDNOT_INSERT_ALL_ROWS and PB WHEN UPDATING PATHS",ErrMess,1);

					return -1;
				}
				else
				{
					int res_query1;
					//status = OCITransCommit(ociHdbc, ociError, 0);
					int sysID=GetSubsystemID("DAQ,TFC");
					sprintf(sqlstmt,"BEGIN :rescode:=%s(%d); :numrows:=%s; END; ",_UpdatePathUsed,sysID,SQLROWCOUNT);
					status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
					//std:://cout<<"value of sqlstmt 2 "<<sqlstmt<<std::endl;			
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
						status =OCIBindByName(hstmt, &bndp2[0], ociError,(text*) ":rescode", -1,(dvoid*)&res_query1, sizeof(int),SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

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


				}

			}
			status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
			status+=freeConnectivityUpdate3();
			if(nodefrom!=NULL) //means not null otehriwse it's -1
				free(nodefrom);
			if(nodeto!=NULL)
				free(nodeto);
			if(pfrom!=NULL) //means not null otehriwse it's -1
				free(pfrom);
			if(pto!=NULL)
				free(pto);
			if(ptypefrom!=NULL) //means not null otehriwse it's -1
				free(ptypefrom);
			if(ptypeto!=NULL)
				free(ptypeto);

			if(numrows_inserted!=NULL)
				free(numrows_inserted);
			if(rescode!=0)
				OCIReportError(ociError,appliName, ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0);  
			if(force_insert==1 && rescode==0)
			{
				FIRST_TIME_CONNECTIVITYUPDATE3=1;
				force_insert=0;
			}


		}
		else
		{
			if(res_query!=0)
			{
				status=freeConnectivityUpdate3();
				status+=res_query;
				GetErrorMess(appliName, "Cache pb",ErrMess,1);
			}
			if(FIRST_TIME_CONNECTIVITYUPDATE3!=1)
			{
				status=freeConnectivityUpdate3();
				status+=res_query;
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
			}
			if(ociEnv==0)
			{
				status=freeConnectivityUpdate3();
				status+=res_query;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
			}
			if(ociEnv!=0 && FIRST_TIME_CONNECTIVITYUPDATE3==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}

		}
		return (status+rescode);
	}
	/**
	* Update the link type of a Link  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* give either nfrom,pfrom or nto,pto

	* @param node_from : name of the device starting the link. Put "none" if giving node_to
	* @param node_to : name of the device ending the link. Put "none" if giving node_from
	* @param port_nbrfrom : port nb of the device from
	* @param port_nbrto : port nb of the device to
	* @param port_typefrom : port nb of the device from
	* @param port_typeto : port nb of the device to
	* @param link_type : name of the link type
	* @param first_time1 : 1 if it's your MultipleLink first insert (save it into the database), 0 otherwise
	* @param last_rows1 : 1 if it's your MultipleLink last insert (save it into the database), 0 otherwise. Maximum in one go: 10000 rows. (we force the insert)
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/ 
	EXTERN_CONFDB
		int UpdateMultipleLkTypeLinks(char* node_from,char* node_to,char* port_nbrfrom,char* port_nbrto,char* port_typefrom,char* port_typeto,char* link_type,int first_time1, int last_rows1,char* ErrMess)
	{
		int numrows=0;
		int num_errs1=0;
		char sqlstmt[1500];
		int i=0;
		char appliName[100]="UpdateMultipleLkTypeLinks";
		OCIStmt *hstmt;
		OCIBind  *bndp2[7];
		OCIBind  *bndp3 = (OCIBind *) 0; 
		OCIBind  *bndp1 = (OCIBind *) 0;
		char* nodefrom=NULL;
		char* nodeto=NULL;
		char* pfrom=NULL;
		char* pto=NULL;
		char* ptypefrom=NULL;
		char* ptypeto=NULL;
		char res_query1[100]="blbla";
		int res_query=0;
		sword status=0;
		char* linktype=NULL;
		int rescode=0;
		int last_rows=last_rows1;
		OCIError* ociError1=0 ;
		int* numrows_inserted=NULL;

		char port_typefrom1[100];
		if(port_typefrom!=NULL)
		{
			if(strlen(port_typefrom)<2)
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
			if(strlen(port_typeto)<2)
				strcpy(port_typeto1,"none");
			else
				strcpy(port_typeto1,port_typeto);
		}
		else
		{
			strcpy(port_typeto1,"none");
		}
		int nodefrom_null=null_charptr(node_from);
		int nodeto_null=null_charptr(node_to);
		int pfrom_null=null_charptr(port_nbrfrom);
		int pto_null=null_charptr(port_nbrto);
		int ptypefrom_null=null_charptr(port_typefrom);
		int ptypeto_null=null_charptr(port_typeto);
		int linktype_null=null_charptr(link_type);
		int force_insert=0;
		int first_time=first_time1;
		if(FIRST_TIME_CONNECTIVITYUPDATE2==1 && _NodeFrom2List==NULL) //means new loop in the mass insertion (by 10000)
		{
			first_time=1; //need to put to 1, as if new cache
		}
		if(first_time==1)
		{

			status+=freeConnectivityUpdate2();
			FIRST_TIME_CONNECTIVITYUPDATE2=1;

		}
		res_query=AppendString(node_from,_NodeFrom2List, _nodefrom2List_len,_nodefrom2NbElement,_max_nodefrom2List_len,first_time);
		res_query+=AppendString(node_to,_NodeTo2List,_nodeto2List_len,_nodeto2NbElement,_max_nodeto2List_len,first_time);
		res_query+=AppendString(link_type,_LinkTypeList2Update,_linktypeList_len2Update,_linktypeNbElement2Update,_max_linktypeList_len2Update,first_time);
		res_query=AppendString(port_nbrfrom,_ptFrom2List, _ptfrom2List_len,_ptfrom2NbElement,_max_ptfrom2List_len,first_time);
		res_query+=AppendString(port_nbrto,_ptTo2List,_ptto2List_len,_ptto2NbElement,_max_ptto2List_len,first_time);
		res_query=AppendString(port_typefrom1,_ptypeFrom2List, _ptypefrom2List_len,_ptypefrom2NbElement,_max_ptypefrom2List_len,first_time);
		res_query+=AppendString(port_typeto1,_ptypeTo2List,_ptypeto2List_len,_ptypeto2NbElement,_max_ptypeto2List_len,first_time);


		res_query+=AppendInt(pto_null,_ptto2_nullvalue,_ptto2NbEl,first_time);
		res_query+=AppendInt(pfrom_null,_ptfrom2_nullvalue,_ptfrom2NbEl,first_time);
		res_query+=AppendInt(ptypeto_null,_ptypeto2_nullvalue,_ptypeto2NbEl,first_time);
		res_query+=AppendInt(ptypefrom_null,_ptypefrom2_nullvalue,_ptypefrom2NbEl,first_time);


		res_query+=AppendInt(nodeto_null,_nodeto2_nullvalue,_nodeto2NbEl,first_time);
		res_query+=AppendInt(linktype_null,_linktype_nullvalueUpdate,_linktypeNbElUpdate,first_time);
		res_query+=AppendInt(nodefrom_null,_nodefrom2_nullvalue,_nodefrom2NbEl,first_time);


		if(_nodefrom2NbElement==MAXROWS && last_rows!=1)
		{
			last_rows=1;
			force_insert=1;
		}
		status=res_query;
		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME_CONNECTIVITYUPDATE2==1)
		{
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
				status+=freeConnectivityUpdate2();
				if(nodefrom!=NULL) //means not null otehriwse it's -1
					free(nodefrom);
				if(nodeto!=NULL)
					free(nodeto);
				if(linktype!=NULL)
					free(linktype);
				if(pfrom!=NULL) //means not null otehriwse it's -1
					free(pfrom);
				if(pto!=NULL)
					free(pto);
				if(ptypefrom!=NULL) //means not null otehriwse it's -1
					free(ptypefrom);
				if(ptypeto!=NULL)
					free(ptypeto);
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

				sprintf(sqlstmt,"BEGIN if lower(nvl(:nto,'none'))='none' then update %s t set t.link_type=nvl((select p.linktypeid from %s p where p.link_name=:lkname),-1),t.last_update=sysdate,t.user_update='%s' where t.portIDfrom=(select k.portid from %s k,%s g where g.devicename=:nfrom and g.deviceid=k.deviceid and k.port_nbr=:pfrom and k.port_way=2 and nvl(k.port_type,'none')=:ptypefrom); else if lower(nvl(:nfrom,'none'))='none' then update %s t set t.link_type=(select f.linktypeID from %s f where f.link_name=:lkname),t.last_update=sysdate,t.user_update='%s' where t.portIDto=(select l.portid from %s l,%s e where e.devicename=:nto and e.deviceid=l.deviceid and l.port_nbr=:pto and  l.port_way=1 and nvl(l.port_type,'none')=:ptypeto) ;  end if; end if;  :numrows:=%s; END; ",MACRO_CONNECTIVITY_TABLE,LINKTYPE_TABLE,login,PORT_TABLE,LOGICAL_DEVICE_TABLE,MACRO_CONNECTIVITY_TABLE,LINKTYPE_TABLE,login,PORT_TABLE,LOGICAL_DEVICE_TABLE,SQLROWCOUNT);		
				status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
				//std:://cout<<"value of stmt "<<sqlstmt <<std::endl;
			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
			{
				nodefrom=(char*)malloc(_nodefrom2NbElement*_max_nodefrom2List_len*sizeof(char));
				if(nodefrom!=NULL)
					status+=NormalizeVector(_NodeFrom2List, _nodefrom2NbElement,_max_nodefrom2List_len,nodefrom);
				nodeto=(char*)malloc(_nodeto2NbElement*_max_nodeto2List_len*sizeof(char));
				if(nodeto!=NULL)
					status+=NormalizeVector(_NodeTo2List, _nodeto2NbElement,_max_nodeto2List_len,nodeto);
				linktype=(char*)malloc(_linktypeNbElement2Update*_max_linktypeList_len2Update*sizeof(char));
				if(linktype!=NULL)
					status+=NormalizeVector(_LinkTypeList2Update, _linktypeNbElement2Update,_max_linktypeList_len2Update,linktype);

				pfrom=(char*)malloc(_ptfrom2NbElement*_max_ptfrom2List_len*sizeof(char));
				if(pfrom!=NULL)
					status+=NormalizeVector(_ptFrom2List, _ptfrom2NbElement,_max_ptfrom2List_len,pfrom);

				ptypefrom=(char*)malloc(_ptypefrom2NbElement*_max_ptypefrom2List_len*sizeof(char));
				if(ptypefrom!=NULL)
					status+=NormalizeVector(_ptypeFrom2List, _ptypefrom2NbElement,_max_ptypefrom2List_len,ptypefrom);

				pto=(char*)malloc(_ptto2NbElement*_max_ptto2List_len*sizeof(char));
				if(pto!=NULL)
					status+=NormalizeVector(_ptTo2List, _ptto2NbElement,_max_ptto2List_len,pto);

				ptypeto=(char*)malloc(_ptypeto2NbElement*_max_ptypeto2List_len*sizeof(char));
				if(ptypeto!=NULL)
					status+=NormalizeVector(_ptypeTo2List, _ptypeto2NbElement,_max_ptypeto2List_len,ptypeto);


				numrows_inserted=(int*)malloc(sizeof(int)*_nodefrom2NbElement);
				if(nodefrom==NULL || nodeto==NULL ||linktype==NULL ||numrows_inserted==NULL||pfrom==NULL||pto==NULL||ptypefrom==NULL||ptypeto==NULL)
				{

					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					status+=freeConnectivityUpdate2();
					if(nodefrom!=NULL) //means not null otehriwse it's -1
						free(nodefrom);
					if(nodeto!=NULL)
						free(nodeto);
					if(linktype!=NULL)
						free(linktype);
					if(pfrom!=NULL) //means not null otehriwse it's -1
						free(pfrom);
					if(pto!=NULL)
						free(pto);
					if(ptypefrom!=NULL) //means not null otehriwse it's -1
						free(ptypefrom);
					if(ptypeto!=NULL)
						free(ptypeto);
					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					return -1;
				}
				else
				{
					for(i=0;i<_nodefrom2NbElement;i++)
						numrows_inserted[i]=0;
					//std:://cout<<"before binding "<<std::endl;
					status =OCIBindByName(hstmt, &bndp2[0], ociError,(text*) ":pfrom", -1,(dvoid*)pfrom, _max_ptfrom2List_len,  SQLT_STR, (dvoid *)& _ptfrom2_nullvalue[0] ,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
				}
			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp2[1], ociError,(text*) ":pto", -1,(dvoid*)pto, _max_ptto2List_len,  SQLT_STR, (dvoid *)& _ptto2_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			//std:://cout<<"before binding [2]"<<std::endl;
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp2[2], ociError,(text*) ":nfrom", -1,(dvoid*)nodefrom, _max_nodefrom2List_len,  SQLT_STR, (dvoid *) & _nodefrom2_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			//std:://cout<<"before binding [3]"<<std::endl;
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp2[3], ociError,(text*) ":nto", -1,(dvoid*)nodeto, _max_nodeto2List_len,  SQLT_STR,  (dvoid *) & _nodeto2_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			//std:://cout<<"before binding [4]"<<std::endl;
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp2[4], ociError,(text*) ":lkname", -1,(dvoid*)linktype, _max_linktypeList_len2Update,SQLT_STR, (dvoid *) & _linktype_nullvalueUpdate[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			//std:://cout<<"before binding [5]"<<std::endl;
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
				status =OCIBindByName(hstmt, &bndp2[5], ociError,(text*) ":ptypefrom", -1,(dvoid*)ptypefrom, _max_ptypefrom2List_len,  SQLT_STR, (dvoid *) &_ptypefrom2_nullvalue[0] ,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			//std:://cout<<"before binding [6]"<<std::endl;
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp2[6], ociError,(text*) ":ptypeto", -1,(dvoid*)ptypeto, _max_ptypeto2List_len,  SQLT_STR, (dvoid *)& _ptypeto2_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[0], ociError,_max_ptfrom2List_len, sizeof(int),0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[1], ociError, _max_ptto2List_len, sizeof(int),  0, 0);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[2], ociError,_max_nodefrom2List_len, sizeof(int), 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[3], ociError, _max_nodeto2List_len, sizeof(int), 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[4], ociError,_max_linktypeList_len2Update, sizeof(int), 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[5], ociError,_max_ptypefrom2List_len, sizeof(int), 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[6], ociError,_max_ptypeto2List_len, sizeof(int), 0, 0);
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
				status= OCIStmtExecute(ociHdbc, hstmt, ociError, _nodefrom2NbElement, 0, 0, 0, OCI_DEFAULT );

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
				for(i=0;i<_nodefrom2NbElement;i++)
				{
					if(numrows_inserted[i]==0)
					{
						numrows=0;
						i=_nodefrom2NbElement+5;
					}
					else
						numrows=numrows_inserted[i];
				}
				//std:://cout<<"after execute numrows="<<numrows<<std::endl;
				if(numrows==0)
				{
					status = OCITransCommit(ociHdbc, ociError, 0);
					int sysID=GetSubsystemID("DAQ,TFC");
					sprintf(sqlstmt,"BEGIN :rescode:=%s(%d); :numrows:=%s; END; ",_UpdateDynamicTables,sysID,SQLROWCOUNT);
					status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
					//std:://cout<<"sql 2"<< sqlstmt<<std::endl;
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
					status+=freeConnectivityUpdate2();


					if(nodefrom!=NULL) //means not null otehriwse it's -1
						free(nodefrom);
					if(nodeto!=NULL)
						free(nodeto);
					if(linktype!=NULL)
						free(linktype);
					if(pfrom!=NULL) //means not null otehriwse it's -1
						free(pfrom);
					if(pto!=NULL)
						free(pto);
					if(ptypefrom!=NULL) //means not null otehriwse it's -1
						free(ptypefrom);
					if(ptypeto!=NULL)
						free(ptypeto);	
					free(numrows_inserted);
					GetErrorMess(appliName, "COULDNOT_UPDATE_ALL_ROWS",ErrMess,1);

					return -1;
				}
				else
				{
					status = OCITransCommit(ociHdbc, ociError, 0);
					int sysID=GetSubsystemID("DAQ,TFC");
					sprintf(sqlstmt,"BEGIN :rescode:=%s(%d); :numrows:=%s; END; ",_UpdateDynamicTables,sysID,SQLROWCOUNT);
					status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
					//std:://cout<<"sql 2"<< sqlstmt<<std::endl;
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
				}
			}
			status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
			status+=freeConnectivityUpdate2();
			if(nodefrom!=NULL) //means not null otehriwse it's -1
				free(nodefrom);
			if(nodeto!=NULL)
				free(nodeto);
			if(linktype!=NULL)
				free(linktype);
			if(numrows_inserted!=NULL)
				free(numrows_inserted);
			if(pfrom!=NULL) //means not null otehriwse it's -1
				free(pfrom);
			if(pto!=NULL)
				free(pto);
			if(ptypefrom!=NULL) //means not null otehriwse it's -1
				free(ptypefrom);
			if(ptypeto!=NULL)
				free(ptypeto);	
			if(rescode!=0)
				OCIReportError(ociError,appliName, ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0);  
			if(force_insert==1 && rescode==0)
			{
				FIRST_TIME_CONNECTIVITYUPDATE2=1;
				force_insert=0;
			}


		}
		else
		{
			if(res_query!=0)
			{
				status=freeConnectivityUpdate2();
				status+=res_query;
				GetErrorMess(appliName, "Cache pb",ErrMess,1);
			}
			if(FIRST_TIME_CONNECTIVITYUPDATE2!=1)
			{
				status=freeConnectivityUpdate2();
				status+=res_query;
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
			}
			if(ociEnv==0)
			{
				status=freeConnectivityUpdate2();
				status+=res_query;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
			}
			if(ociEnv!=0 && FIRST_TIME_CONNECTIVITYUPDATE2==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}

		}
		return (status+rescode);
	}



	/**
	* Update the link info of a Link  returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* give either nfrom,pfrom or nto,pto

	* @param node_from : name of the device starting the link. Put "none" if giving node_to
	* @param node_to : name of the device ending the link. Put "none" if giving node_from
	* @param port_nbrfrom : port nb of the device from
	* @param port_nbrto : port nb of the device to
	* @param port_typefrom : port nb of the device from
	* @param port_typeto : port nb of the device to
	* @param link_information : new comment
	* @param first_time1 : 1 if it's your MultipleLink first insert (save it into the database), 0 otherwise
	* @param last_rows1 : 1 if it's your MultipleLink last insert (save it into the database), 0 otherwise. Maximum in one go: 10000 rows. (we force the insert)
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/ 
	EXTERN_CONFDB
		int UpdateMultipleLkInfoLinks(char* node_from,char* node_to,char* port_nbrfrom,char* port_nbrto,char* port_typefrom,char* port_typeto,char* link_information,int first_time1, int last_rows1,char* ErrMess)
	{
		int numrows=0;
		int num_errs1=0;
		char sqlstmt[1500];
		int i=0;
		char appliName[100]="UpdateMultipleLkInfoLinks";
		OCIStmt *hstmt;
		OCIBind  *bndp2[7];
		OCIBind  *bndp3 = (OCIBind *) 0; 
		OCIBind  *bndp1 = (OCIBind *) 0;
		char* nodefrom=NULL;
		char* nodeto=NULL;
		char* pfrom=NULL;
		char* pto=NULL;
		char* ptypefrom=NULL;
		char* ptypeto=NULL;
		char res_query1[100]="blbla";
		int res_query=0;
		sword status=0;
		char* linkinfo=NULL;
		int rescode=0;
		int last_rows=last_rows1;
		OCIError* ociError1=0 ;
		int* numrows_inserted=NULL;

		char port_typefrom1[100];
		if(port_typefrom!=NULL)
		{
			if(strlen(port_typefrom)<2)
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
			if(strlen(port_typeto)<2)
				strcpy(port_typeto1,"none");
			else
				strcpy(port_typeto1,port_typeto);
		}
		else
		{
			strcpy(port_typeto1,"none");
		}
		int nodefrom_null=null_charptr(node_from);
		int nodeto_null=null_charptr(node_to);
		int pfrom_null=null_charptr(port_nbrfrom);
		int pto_null=null_charptr(port_nbrto);
		int ptypefrom_null=null_charptr(port_typefrom);
		int ptypeto_null=null_charptr(port_typeto);
		int linkinfo_null=null_charptr(link_information);
		int force_insert=0;
		int first_time=first_time1;
		if(FIRST_TIME_CONNECTIVITYUPDATE5==1 && _NodeFrom5List==NULL) //means new loop in the mass insertion (by 10000)
		{
			first_time=1; //need to put to 1, as if new cache
		}
		if(first_time==1)
		{

			status+=freeConnectivityUpdate5();
			FIRST_TIME_CONNECTIVITYUPDATE5=1;

		}
		res_query=AppendString(node_from,_NodeFrom5List, _nodefrom5List_len,_nodefrom5NbElement,_max_nodefrom5List_len,first_time);
		res_query+=AppendString(node_to,_NodeTo5List,_nodeto5List_len,_nodeto5NbElement,_max_nodeto5List_len,first_time);
		res_query+=AppendString(link_information,_LinkInfoList5Update,_linkinfoList_len5Update,_linkinfoNbElement5Update,_max_linkinfoList_len5Update,first_time);
		res_query=AppendString(port_nbrfrom,_ptFrom5List, _ptfrom5List_len,_ptfrom5NbElement,_max_ptfrom5List_len,first_time);
		res_query+=AppendString(port_nbrto,_ptTo5List,_ptto5List_len,_ptto5NbElement,_max_ptto5List_len,first_time);
		res_query=AppendString(port_typefrom1,_ptypeFrom5List, _ptypefrom5List_len,_ptypefrom5NbElement,_max_ptypefrom5List_len,first_time);
		res_query+=AppendString(port_typeto1,_ptypeTo5List,_ptypeto5List_len,_ptypeto5NbElement,_max_ptypeto5List_len,first_time);


		res_query+=AppendInt(pto_null,_ptto5_nullvalue,_ptto5NbEl,first_time);
		res_query+=AppendInt(pfrom_null,_ptfrom5_nullvalue,_ptfrom5NbEl,first_time);
		res_query+=AppendInt(ptypeto_null,_ptypeto5_nullvalue,_ptypeto5NbEl,first_time);
		res_query+=AppendInt(ptypefrom_null,_ptypefrom5_nullvalue,_ptypefrom5NbEl,first_time);


		res_query+=AppendInt(nodeto_null,_nodeto5_nullvalue,_nodeto5NbEl,first_time);
		res_query+=AppendInt(linkinfo_null,_linkinfo_nullvalueUpdate,_linkinfoNbElUpdate,first_time);
		res_query+=AppendInt(nodefrom_null,_nodefrom5_nullvalue,_nodefrom5NbEl,first_time);


		if(_nodefrom5NbElement==MAXROWS && last_rows!=1)
		{
			last_rows=1;
			force_insert=1;
		}
		status=res_query;
		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME_CONNECTIVITYUPDATE5==1)
		{
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
				status+=freeConnectivityUpdate5();
				if(nodefrom!=NULL) //means not null otehriwse it's -1
					free(nodefrom);
				if(nodeto!=NULL)
					free(nodeto);
				if(linkinfo!=NULL)
					free(linkinfo);
				if(pfrom!=NULL) //means not null otehriwse it's -1
					free(pfrom);
				if(pto!=NULL)
					free(pto);
				if(ptypefrom!=NULL) //means not null otehriwse it's -1
					free(ptypefrom);
				if(ptypeto!=NULL)
					free(ptypeto);
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

				sprintf(sqlstmt,"BEGIN if lower(nvl(:nto,'none'))='none' then update %s t set t.link_info=:lkinfo,t.last_update=sysdate,t.user_update='%s' where t.portIDfrom=(select k.portid from %s k,%s g where g.devicename=:nfrom and g.deviceid=k.deviceid and k.port_nbr=:pfrom and k.port_way=2 and nvl(k.port_type,'none')=:ptypefrom); else if lower(nvl(:nfrom,'none'))='none' then update %s t set t.link_info=:lkinfo,t.last_update=sysdate,t.user_update='%s' where t.portIDto=(select l.portid from %s l,%s e where e.devicename=:nto and e.deviceid=l.deviceid and l.port_nbr=:pto and  l.port_way=1 and nvl(l.port_type,'none')=:ptypeto) ;  end if; end if;  :numrows:=%s; END; ",MACRO_CONNECTIVITY_TABLE,login,PORT_TABLE,LOGICAL_DEVICE_TABLE,MACRO_CONNECTIVITY_TABLE,login,PORT_TABLE,LOGICAL_DEVICE_TABLE,SQLROWCOUNT);		
				status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
				//std:://cout<<"value of stmt "<<sqlstmt <<std::endl;
			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
			{
				nodefrom=(char*)malloc(_nodefrom5NbElement*_max_nodefrom5List_len*sizeof(char));
				if(nodefrom!=NULL)
					status+=NormalizeVector(_NodeFrom5List, _nodefrom5NbElement,_max_nodefrom5List_len,nodefrom);
				nodeto=(char*)malloc(_nodeto5NbElement*_max_nodeto5List_len*sizeof(char));
				if(nodeto!=NULL)
					status+=NormalizeVector(_NodeTo5List, _nodeto5NbElement,_max_nodeto5List_len,nodeto);
				linkinfo=(char*)malloc(_linkinfoNbElement5Update*_max_linkinfoList_len5Update*sizeof(char));
				if(linkinfo!=NULL)
					status+=NormalizeVector(_LinkInfoList5Update, _linkinfoNbElement5Update,_max_linkinfoList_len5Update,linkinfo);

				pfrom=(char*)malloc(_ptfrom5NbElement*_max_ptfrom5List_len*sizeof(char));
				if(pfrom!=NULL)
					status+=NormalizeVector(_ptFrom5List, _ptfrom5NbElement,_max_ptfrom5List_len,pfrom);

				ptypefrom=(char*)malloc(_ptypefrom5NbElement*_max_ptypefrom5List_len*sizeof(char));
				if(ptypefrom!=NULL)
					status+=NormalizeVector(_ptypeFrom5List, _ptypefrom5NbElement,_max_ptypefrom5List_len,ptypefrom);

				pto=(char*)malloc(_ptto5NbElement*_max_ptto5List_len*sizeof(char));
				if(pto!=NULL)
					status+=NormalizeVector(_ptTo5List, _ptto5NbElement,_max_ptto5List_len,pto);

				ptypeto=(char*)malloc(_ptypeto5NbElement*_max_ptypeto5List_len*sizeof(char));
				if(ptypeto!=NULL)
					status+=NormalizeVector(_ptypeTo5List, _ptypeto5NbElement,_max_ptypeto5List_len,ptypeto);


				numrows_inserted=(int*)malloc(sizeof(int)*_nodefrom5NbElement);
				if(nodefrom==NULL || nodeto==NULL ||linkinfo==NULL ||numrows_inserted==NULL||pfrom==NULL||pto==NULL||ptypefrom==NULL||ptypeto==NULL)
				{

					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					status+=freeConnectivityUpdate5();
					if(nodefrom!=NULL) //means not null otehriwse it's -1
						free(nodefrom);
					if(nodeto!=NULL)
						free(nodeto);
					if(linkinfo!=NULL)
						free(linkinfo);
					if(pfrom!=NULL) //means not null otehriwse it's -1
						free(pfrom);
					if(pto!=NULL)
						free(pto);
					if(ptypefrom!=NULL) //means not null otehriwse it's -1
						free(ptypefrom);
					if(ptypeto!=NULL)
						free(ptypeto);
					if(numrows_inserted!=NULL)
						free(numrows_inserted);
					return -1;
				}
				else
				{
					for(i=0;i<_nodefrom2NbElement;i++)
						numrows_inserted[i]=0;
					//std:://cout<<"before binding "<<std::endl;
					status =OCIBindByName(hstmt, &bndp2[0], ociError,(text*) ":pfrom", -1,(dvoid*)pfrom, _max_ptfrom5List_len,  SQLT_STR, (dvoid *)& _ptfrom5_nullvalue[0] ,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
				}
			}
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp2[1], ociError,(text*) ":pto", -1,(dvoid*)pto, _max_ptto5List_len,  SQLT_STR, (dvoid *)& _ptto5_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			//std:://cout<<"before binding [2]"<<std::endl;
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp2[2], ociError,(text*) ":nfrom", -1,(dvoid*)nodefrom, _max_nodefrom5List_len,  SQLT_STR, (dvoid *) & _nodefrom5_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			//std:://cout<<"before binding [3]"<<std::endl;
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp2[3], ociError,(text*) ":nto", -1,(dvoid*)nodeto, _max_nodeto5List_len,  SQLT_STR,  (dvoid *) & _nodeto5_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			//std:://cout<<"before binding [4]"<<std::endl;
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp2[4], ociError,(text*) ":lkinfo", -1,(dvoid*)linkinfo, _max_linkinfoList_len5Update,SQLT_STR, (dvoid *) & _linkinfo_nullvalueUpdate[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			//std:://cout<<"before binding [5]"<<std::endl;
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
				status =OCIBindByName(hstmt, &bndp2[5], ociError,(text*) ":ptypefrom", -1,(dvoid*)ptypefrom, _max_ptypefrom5List_len,  SQLT_STR, (dvoid *) &_ptypefrom5_nullvalue[0] ,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			//std:://cout<<"before binding [6]"<<std::endl;
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp2[6], ociError,(text*) ":ptypeto", -1,(dvoid*)ptypeto, _max_ptypeto5List_len,  SQLT_STR, (dvoid *)& _ptypeto5_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[0], ociError,_max_ptfrom5List_len, sizeof(int),0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[1], ociError, _max_ptto5List_len, sizeof(int),  0, 0);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[2], ociError,_max_nodefrom5List_len, sizeof(int), 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[3], ociError, _max_nodeto5List_len, sizeof(int), 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[4], ociError,_max_linkinfoList_len5Update, sizeof(int), 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[5], ociError,_max_ptypefrom5List_len, sizeof(int), 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp2[6], ociError,_max_ptypeto5List_len, sizeof(int), 0, 0);
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
				status= OCIStmtExecute(ociHdbc, hstmt, ociError, _nodefrom5NbElement, 0, 0, 0, OCI_DEFAULT );

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
				for(i=0;i<_nodefrom5NbElement;i++)
				{
					if(numrows_inserted[i]==0)
					{
						numrows=0;
						i=_nodefrom5NbElement+5;
					}
					else
						numrows=numrows_inserted[i];
				}
				//std:://cout<<"after execute numrows="<<numrows<<std::endl;
				if(numrows==0)
				{
					status = OCITransCommit(ociHdbc, ociError, 0);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status+=freeConnectivityUpdate2();


					if(nodefrom!=NULL) //means not null otehriwse it's -1
						free(nodefrom);
					if(nodeto!=NULL)
						free(nodeto);
					if(linkinfo!=NULL)
						free(linkinfo);
					if(pfrom!=NULL) //means not null otehriwse it's -1
						free(pfrom);
					if(pto!=NULL)
						free(pto);
					if(ptypefrom!=NULL) //means not null otehriwse it's -1
						free(ptypefrom);
					if(ptypeto!=NULL)
						free(ptypeto);	
					free(numrows_inserted);
					GetErrorMess(appliName, "COULDNOT_UPDATE_ALL_ROWS",ErrMess,1);

					return -1;
				}
				else
				{
					status = OCITransCommit(ociHdbc, ociError, 0);


				}
			}
			status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
			status+=freeConnectivityUpdate5();
			if(nodefrom!=NULL) //means not null otehriwse it's -1
				free(nodefrom);
			if(nodeto!=NULL)
				free(nodeto);
			if(linkinfo!=NULL)
				free(linkinfo);
			if(numrows_inserted!=NULL)
				free(numrows_inserted);
			if(pfrom!=NULL) //means not null otehriwse it's -1
				free(pfrom);
			if(pto!=NULL)
				free(pto);
			if(ptypefrom!=NULL) //means not null otehriwse it's -1
				free(ptypefrom);
			if(ptypeto!=NULL)
				free(ptypeto);	
			if(rescode!=0)
				OCIReportError(ociError,appliName, ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0);  
			if(force_insert==1 && rescode==0)
			{
				FIRST_TIME_CONNECTIVITYUPDATE5=1;
				force_insert=0;
			}


		}
		else
		{
			if(res_query!=0)
			{
				status=freeConnectivityUpdate5();
				status+=res_query;
				GetErrorMess(appliName, "Cache pb",ErrMess,1);
			}
			if(FIRST_TIME_CONNECTIVITYUPDATE5!=1)
			{
				status=freeConnectivityUpdate5();
				status+=res_query;
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
			}
			if(ociEnv==0)
			{
				status=freeConnectivityUpdate5();
				status+=res_query;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
			}
			if(ociEnv!=0 && FIRST_TIME_CONNECTIVITYUPDATE5==1 && res_query==0)
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





